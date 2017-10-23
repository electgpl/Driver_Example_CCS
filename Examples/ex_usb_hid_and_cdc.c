/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////                        ex_usb_keyboard.c                        ////
////                                                                 ////
//// This creates a composite USB device containing both CDC         //// 
//// (virtual COM port) and HID interface.  This is a merge of       ////
//// ex_usb_serial2.c and ex_usb_hid.c.                               ////
////                                                                 ////
//// This file is part of CCS's PIC USB driver code.  See USB.H      ////
//// for more documentation and a list of examples.                  ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// VERSION HISTORY                                                 ////
////                                                                 ////
////  July 9th, 2013                                                 ////
////     Initial version.                                            ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2009 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#include <ex_usb_common.h>

// you can change the tx and rx packet sizes here.
// in order to be compatbile with hiddemo.exe, these values must be 2.
#define USB_CONFIG_HID_TX_SIZE   2
#define USB_CONFIG_HID_RX_SIZE   2

/////////////////////////////////////////////////////////////////////////////
//
// Include the CCS USB Libraries.  See the comments at the top of these
// files for more information
//
/////////////////////////////////////////////////////////////////////////////
#include <usb_desc_hid_cdc.h>    //USB Configuration and Device descriptors for this UBS device
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

   if (new_enumerated)
   {
      LED_ON(LED1);
   }
   else
   {
      LED_OFF(LED1);
   }

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
      uart_printf(uart_putc, "\r\n\nUSB unenumerated by PC/HOST, waiting for enumeration...");
   }
   
   last_connected=new_connected;
   last_enumerated=new_enumerated;
}

static void my_cdc_demo(void)
{
   BYTE i, j, address, value;
   
   #if USB_CDC_DATA_IN_SIZE<20
    #error not big enough to hold str[]
   #endif
   char str[20];

   if (!usb_cdc_kbhit())
      return;
   
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

void display_array(unsigned int8 *p, unsigned int8 n)
{
   while(n--)
   {
      uart_printf(uart_putc, "%X ", *p++);
   }
}

static void my_hid_demo(void)
{
   unsigned int8 out_data[USB_CONFIG_HID_TX_SIZE];
   unsigned int8 in_data[USB_CONFIG_HID_RX_SIZE];
   static unsigned int8 send_timer=0;

   if (!send_timer)
   {
      send_timer=250;
      
      out_data[0]=read_adc();
      out_data[1]=BUTTON_PRESSED();
      
      if (usb_put_packet(USB_HID_ENDPOINT, out_data, USB_CONFIG_HID_TX_SIZE, USB_DTS_TOGGLE))
      {
         uart_printf(uart_putc, "\r\n<-- HID Sending %u bytes:", USB_CONFIG_HID_TX_SIZE);
         
         display_array(out_data, USB_CONFIG_HID_TX_SIZE);
      }
   }
   
   if (usb_kbhit(USB_HID_ENDPOINT))
   {
      usb_get_packet(USB_HID_ENDPOINT, in_data, USB_CONFIG_HID_RX_SIZE);
      
      uart_printf(uart_putc, "\r\n--> HID Received %u bytes:", USB_CONFIG_HID_RX_SIZE);
      
      display_array(in_data, USB_CONFIG_HID_RX_SIZE);
      
     #if defined(LEDS_GREEN)
      if (in_data[0] && in_data[1])
      {
         LEDS_RED();
      }
      else if (in_data[0] && in_data[1])
      {
         LEDS_GREEN();
      }
      else
      {
         LEDS_OFF();
      }
     #else
      if (in_data[0]) {LED_ON(LED2);} else {LED_OFF(LED2);}
      if (in_data[1]) {LED_ON(LED3);} else {LED_OFF(LED3);}
     #endif
   }
   
   send_timer--;
}

void main(void) 
{
   HW_INIT();

   LEDS_OFF();
   
   setup_adc_ports(HW_ADC_PORTS);
   setup_adc(HW_ADC_CONFIG);
   set_adc_channel(HW_ADC_CHANNEL);

   LED_ON(LED1);

   uart_printf(uart_putc, "\r\n\nCCS CDC + HID Demo");

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

   usb_init_cs();

  #if !defined(__USB_PIC_PERIF__)
   uart_printf(uart_putc, "\r\nUSBN: 0x%X", usbn_get_version());
  #endif
   uart_printf(uart_putc, "\r\n");

   while (TRUE)
   {
      // service low level USB operations.
      usb_task();

      // special debugging routines for this program.
      usb_debug_task();

      if (usb_enumerated())
      {
         my_cdc_demo();
         
         my_hid_demo();
         
         delay_ms(1);
      }
   }
}

