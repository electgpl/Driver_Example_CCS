/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////                        ex_usb_serial2.c                         ////
////                                                                 ////
//// A demonstration of the USB CDC API that is provided by CCS.     ////
//// The USB CDC API that CCS provides will create a virtual UART    ////
//// port.  USB CDC drivers are included with most versions of       ////
//// Microsoft Windows, and when properly loaded will create a COMx  ////
//// port from which you can write and read to your PIC device       ////
//// like any serial device that has a COMx port.                    ////
////                                                                 ////
//// This example is a conversion of the original EX_INTEE.C to use  ////
//// the USB CDC API to read and display serial data over USB.       ////
////                                                                 ////
//// Only two lines were added to initialize USB:                    ////
////   usb_init() - init USB and enable USB interrupt.               ////
////   while(!usb_cdc_connected()) - wait until user opens           ////
////       Hyperterminal before displaying serial data.  This line   ////
////       is not necessary.                                         ////
////                                                                 ////
//// Two other changes were also made to convert to USB.  First,     ////
//// printf will call usb_cdc_putc() to put the character out USB    ////
//// instead of the normal RS232 stream.  Second, gethex() was       ////
//// replaced with gethex_usb().  All input functions normally found ////
//// in input.c have been converted to use the USB CDC API in        ////
//// usb_cdc.h, and gethex_usb() is one of these converted           ////
//// functions.                                                      ////
////                                                                 ////
//// See usb_cdc.h for API documentation.                            ////
////                                                                 ////
//// This file is part of CCS's PIC USB driver code.  See USB.H      ////
//// for more documentation and a list of examples.                  ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// VERSION HISTORY                                                 ////
////                                                                 ////
//// Oct 15th, 2010:                                                 ////
////  Potential bug in CDC that causes lost PIC->PC chars resolved?  ////
////  Added initial 18F47J53 family support.                         ////
////  Added USB_ISR_POLLING support.  Define this and interrupts     ////
////     will not be used.  usb_task() must be called periodically   ////
////     in your main loop.  If it is not called faster than once    ////
////     per millisecond, USB may not work (PIC18 and PIC24 only).   ////
////                                                                 ////
//// April 28th, 2010:                                               ////
////  Added ex_usb_common.h.                                         ////
////  Initial support for CCS PIC24USB board.                        ////
////                                                                 ////
//// March 5th, 2009:                                                ////
////   Cleanup for Wizard.                                           ////
////   PIC24 Initial release.                                        ////
////                                                                 ////
//// July 1st, 2005: Initial Release.                                ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2005 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

////// end configuration /////////////////////////////////////////////////

#include <ex_usb_common.h>

// Includes all USB code and interrupts, as well as the CDC API
#include <usb_cdc.h>

#if (getenv("DATA_EEPROM") > 0)
 #define MY_READ_EEPROM(x)    read_eeprom(x)
 #define MY_WRITE_EEPROM(x, y)   write_eeprom(x, y)
#else
 // if chip doesn't have an eeprom, then simulate one in RAM
 char data[256];
 #define MY_READ_EEPROM(x)    data[x]
 #define MY_WRITE_EEPROM(x, y)   data[x] = y
#endif

/////////////////////////////////////////////////////////////////////////////
//
// usb_debug_task()
//
// When called periodically, displays debugging information over serial
// to display enumeration and connection states.  Also lights LED2 and LED3
// based upon enumeration and connection status.
//
/////////////////////////////////////////////////////////////////////////////
void usb_debug_task(void)
{
   static int8 last_connected;
   static int8 last_enumerated;
   int8 new_connected;
   int8 new_enumerated;

   new_connected=usb_attached();
   new_enumerated=usb_enumerated();

  #if defined(LEDS_RED) && defined(LEDS_GREEN)
   if (new_enumerated)
   {
      LEDS_GREEN();
   }
   else
   {
      LEDS_RED();
   }
  #else
   if (new_connected)
   {
      LED_ON(LED2);
   }
   else
   {
      LED_OFF(LED2);
   }

   if (new_enumerated)
   {
      LED_ON(LED3);
   }
   else
   {
      LED_OFF(LED3);
   }
  #endif

   if (new_connected && !last_connected)
   {
      uart_printf(uart_putc, "\r\n\nUSB connected, waiting for enumaration...");
   }
   if (!new_connected && last_connected)
   {
      uart_printf(uart_putc, "\r\n\nUSB disconnected, waiting for connection...");
   }
   if (new_enumerated && !last_enumerated)
   {
      uart_printf(uart_putc, "\r\n\nUSB enumerated by PC/HOST");
   }
   if (!new_enumerated && last_enumerated)
   {
      uart_printf(uart_putc, "\r\n\nUSB un-enumerated by PC/HOST, waiting for enumeration...");
   }

   last_connected=new_connected;
   last_enumerated=new_enumerated;
}

static void handle_incoming_cdc(void)
{
   BYTE i, j, address, value;
   
   #if USB_CDC_DATA_IN_SIZE<20
    #error not big enough to hold str[]
   #endif
   char str[20];

   i = toupper(usb_cdc_getc());

   if (i == 'R')
   {
      printf(usb_cdc_putc, "\r\n\nEEPROM:\r\n");              // Display contents of the first 64
      for(i=0; i<=3; ++i)
      {
         for(j=0; j<=15; ++j)
         {
            printf(usb_cdc_putc, "%2x ", MY_READ_EEPROM( i*16+j ) );
         }
         printf(usb_cdc_putc, "\n\r");
      }
   }

   if (i == 'W')
   {
      printf(usb_cdc_putc, "\r\n\nLocation to change: ");
      address = gethex_usb();

      printf(usb_cdc_putc, "\r\nNew value: ");
      value = gethex_usb();

      MY_WRITE_EEPROM(address, value);

      printf(usb_cdc_putc, "\r\n");
   }

   sprintf(str, "\r\nR)ead or W)rite");
   while (!usb_cdc_puts(str))
   {
     #if defined(USB_ISR_POLLING)
      usb_task();
     #endif   
   }
}

void main(void)
{
   HW_INIT();

   LEDS_OFF();

   uart_printf(uart_putc, "\r\n\nCCS USB Serial2 Example");
  #if defined(__PCH__)
   uart_printf(uart_putc, "\r\nPCH: v");
   uart_printf(uart_putc, __PCH__);
  #elif defined(__PCD__)
   uart_printf(uart_putc, "\r\nPCD: v");
   uart_printf(uart_putc, __PCD__);
  #else
   uart_printf(uart_putc, "\r\n\PCM: v");
   uart_printf(uart_putc, __PCM__);
  #endif

  #if defined(LEDS_RED)
   LEDS_RED();
  #else
   LED_ON(LED1);
  #endif

   // init USB (non-blocking)
   // usb_task() need to be called in your loop to finish USB initialization.
   usb_init_cs();

   for(;;)
   {
      // service low level USB operations.
      usb_task();

      // special debugging routines for this program.
      usb_debug_task();

      if (usb_enumerated())
      {
         if (usb_cdc_kbhit())
         {
            // application specific routine that handles incoming CDC (virtual
            // com port) commands for reading/writing EEPROM.
            handle_incoming_cdc();
         }
      }
   }
}
