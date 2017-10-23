/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////                        ex_usb_serial3.c                         ////
////                                                                 ////
//// This is a reworking of ex_usb_serial2.c to show how to          ////
//// workaround the limitations of using USB routines inside         ////
//// interrupts and how to use USB_CDC_ISR() option.                 ////
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
//// Feb 18th, 2013:                                                 ////
////     Initial version.                                            ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2013 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

// if USB_CDC_ISR is defined, then this function will be called
// by the USB ISR when there incoming CDC (virtual com port) data.
// this is useful if you want to port old RS232 code that was use
// #int_rda to CDC.
#define USB_CDC_ISR() handle_incoming_usb()

// in order for handle_incoming_usb() to be able to transmit the entire
// USB message in one pass, we need to increase the CDC buffer size from
// the normal size and use the USB_CDC_DELAYED_FLUSH option.
// failure to do this would cause some loss of data.
#define USB_CDC_DELAYED_FLUSH
#define USB_CDC_DATA_LOCAL_SIZE  128

////// end configuration /////////////////////////////////////////////////

#include <ex_usb_common.h>

static void handle_incoming_usb(void);

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

#include <stdlib.h>

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

   if (new_connected)
      LED_ON(LED2);
   else
      LED_OFF(LED2);

   if (new_enumerated)
      LED_ON(LED3);
   else
      LED_OFF(LED3);

   if (new_connected && !last_connected)
      uart_printf("\r\n\nUSB connected, waiting for enumaration...");
   if (!new_connected && last_connected)
      uart_printf("\r\n\nUSB disconnected, waiting for connection...");
   if (new_enumerated && !last_enumerated)
      uart_printf("\r\n\nUSB enumerated by PC/HOST");
   if (!new_enumerated && last_enumerated)
      uart_printf("\r\n\nUSB un-enumerated by PC/HOST, waiting for enumeration...");

   last_connected=new_connected;
   last_enumerated=new_enumerated;
}

static void handle_incoming_usb(void)
{
   BYTE i, j, value;
   int1 prompt=FALSE;
   static BYTE address;
   static enum
   {
      GET_COMMAND = 0,
      GET_ADDY0,
      GET_ADDY1,
      GET_VAL0,
      GET_VAL1
   } state;
   static char str[3];
   char c;

   while(usb_cdc_kbhit())
   {
      // since we cannot call usb_cdc_getc() from an ISR unless there
      // is already data in the buffer, we must state machine this routine
      // to continue from previous location.
      c = usb_cdc_getc();

      switch(state)
      {
         default:
         case GET_COMMAND:
            if (toupper(c)=='R')
            {
               printf(usb_cdc_putc_fast, "\r\n\nEEPROM:\r\n");              // Display contents of the first 64
               for(i=0; i<=3; ++i)
               {
                  for(j=0; j<=15; ++j)
                  {
                     printf(usb_cdc_putc_fast, "%2x ", MY_READ_EEPROM( i*16+j ) );
                  }
                  printf(usb_cdc_putc_fast, "\n\r");
               }

               prompt=TRUE;
            }
            else if (toupper(c)=='W')
            {
               printf(usb_cdc_putc_fast, "\r\n\nLocation to change (HEX): ");

               state++;
            }
            else
            {
               prompt=TRUE;
            }
            break;

         case GET_ADDY0:
            str[0] = c;
            usb_cdc_putc_fast(c);   //echo
            state++;
            break;

         case GET_ADDY1:
            str[1] = c;
            str[2] = 0;
            address = strtoul(str, null, 16);
            usb_cdc_putc_fast(c);   //echo
            printf(usb_cdc_putc_fast, "\r\nNew value: ");
            state++;
            break;

         case GET_VAL0:
            str[0] = c;
            usb_cdc_putc_fast(c);   //echo
            state++;
            break;


         case GET_VAL1:
            str[1] = c;
            str[2] = 0;
            usb_cdc_putc_fast(c);   //echo
            printf(usb_cdc_putc_fast, "\r\n");
            value = strtoul(str, null, 16);
            MY_WRITE_EEPROM(address, value);
            state = GET_COMMAND;
            prompt = TRUE;
            break;
      }
   }

   if (prompt)
   {
      printf(usb_cdc_putc_fast, "\r\nR)ead memory or W)rite Memory");
   }
}

void main(void)
{
   HW_INIT();

   LED_OFF(LED1);
   LED_OFF(LED2);
   LED_OFF(LED3);

   uart_printf("\r\n\nCCS USB Serial3 Example");
  #if defined(__PCH__)
   uart_printf("\r\nPCH: v");
   uart_printf(__PCH__);
  #elif defined(__PCD__)
   uart_printf("\r\nPCD: v");
   uart_printf(__PCD__);
  #else
   uart_printf("\r\n\PCM: v");
   uart_printf(__PCM__);
  #endif

   LED_ON(LED1);

   // init USB (non-blocking)
   // usb_task() need to be called in your loop to finish USB initialization.
   usb_init_cs();

   for(;;)
   {
      // service low level USB operations.
      usb_task();

      // special debugging routines for this program.
      usb_debug_task();
   }
}
