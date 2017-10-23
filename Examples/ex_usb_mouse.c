/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////                        ex_usb_mouse.c                           ////
////                                                                 ////
//// An example of creating a USB mouse.  Uses your operating        ////
//// system's HID drivers, which on most systems should already be   ////
//// installed.                                                      ////
////                                                                 ////
//// If everything is working, the mouse cursor will move in a       ////
//// circle when connected to a PC.                                  ////
////                                                                 ////
//// This file is part of CCS's PIC USB driver code.  See USB.H      ////
//// for more documentation and a list of examples.                  ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// NOTE ABOUT ENDPOINT BUFFER SIZE:                                ////
////                                                                 ////
//// Although this application sends 4 bytes to the PC, this demo    ////
//// defines USB_EP1_TX_SIZE  to 8 to allocate 8 bytes for this      ////
//// endpoint.  This constant is also used in the endpoint           ////
//// descriptor to specify the endpoint max size.  If you were       ////
//// concious of RAM savings you could redefine these to 4           ////
//// (or even 1!), but you would lose throughput.The reason for      ////
//// throughput loss is that if you send a packet that is the same   ////
//// size as the max packet size then you need to send a 0 len       ////
//// packet to specify end of message  marker.  The routines         ////
//// usb_puts() and usb_gets() send and  receive multiple packet     ////
//// message, waiting for a 0 len packet or a packet that is smaller ////
//// than max-packet size.                                           ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// VERSION HISTORY                                                 ////
////                                                                 ////
//// Mar 11, 2013:                                                   ////
////  Added Enhanced PIC16 support.                                  ////
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
//// June 20th, 2005: 18Fxx5x Release                                ////
////                                                                 ////
//// March 21st, 2005: Initial PIC18Fxx5x support                    ////
////                                                                 ////
//// June 24th, 2004:  Cleanup to work with updated USB API.         ////
////                   Mouse now goes in a circle.                   ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2010 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#include <ex_usb_common.h>

/////////////////////////////////////////////////////////////////////////////
//
// Include the CCS USB Libraries.  See the comments at the top of these
// files for more information
//
/////////////////////////////////////////////////////////////////////////////
#if __USB_PIC_PERIF__
 #if defined(__PCM__)
  #include <pic16f_usb.h>   //Microchip PIC16Fxxxx hardware layer for usb.c
 #elif defined(__PCH__)
  #include <pic18_usb.h>   //Microchip PIC18Fxx5x hardware layer for usb.c
 #elif defined(__PCD__)
  #include <pic24_usb.h>   //Microchip PIC18Fxx5x hardware layer for usb.c
 #endif
#else
 #include <usbn960x.h>   //National 960x hardware layer for usb.c
#endif
#include <usb_desc_mouse.h>    //USB Configuration and Device descriptors for this UBS device
#include <usb.c>        //handles usb setup tokens and get descriptor reports


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

   new_connected = usb_attached();
   new_enumerated = usb_enumerated();

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

typedef unsigned int16 TICK;
#use timer(timer=1, bits=16, tick=1ms)

#define MOUSE_UPDATE_RATE_TICKS  (TICKS_PER_SECOND/100)  //10ms rate

void main(void)
{
   TICK t;
  #if defined(LED1)
   TICK b;
  #endif

   #define MOUSE_SEQUENCE_STEPS  16
   const char mouse_seq[MOUSE_SEQUENCE_STEPS]=
      {0, 1, 3, 4, 4, 4, 3, 1, 0, -1, -3, -4, -4, -4, -3, -1};

   int8 out_data[4];
   unsigned int8 x_seq;
   unsigned int8 y_seq;
   unsigned int8 count;

   x_seq = 0;
   y_seq = MOUSE_SEQUENCE_STEPS/4;
   count = 0;

   HW_INIT();
   
   LEDS_OFF();

   LED_ON(LED1);

   delay_ms(72);

   uart_printf(uart_putc, "\r\n\nCCS USB Mouse Example\r\n");
  #if defined(__PCH__)
   uart_printf(uart_putc, "PCH: v");
   uart_printf(uart_putc, __PCH__);
  #elif defined(__PCD__)
   uart_printf(uart_putc, "PCD: v");
   uart_printf(uart_putc, __PCD__);
  #else
   uart_printf(uart_putc, "PCM: v");
   uart_printf(uart_putc, __PCM__);
  #endif

   usb_init_cs();

  #if !(__USB_PIC_PERIF__)
   uart_printf(uart_putc, "\r\nUSBN: 0x%X", usbn_get_version());
  #endif
   uart_printf(uart_putc, "\r\n");

   while (TRUE)
   {
      uart_task();

      usb_task();

      usb_debug_task();
      
     #if defined(LED1)
      if ((get_ticks() - b) > TICKS_PER_SECOND/2)
      {
         b = get_ticks();
         
         output_toggle(LED1);
      }
     #endif

      if (
            usb_enumerated() &&
            ((get_ticks() - t) > MOUSE_UPDATE_RATE_TICKS) &&
            usb_tbe(1)
         )
      {
         t = get_ticks();

         out_data[0] = 0; //button state goes here
         out_data[1] = mouse_seq[x_seq];
         out_data[2] = mouse_seq[y_seq];
         out_data[3] = 0; //wheel state goes here

         if (usb_put_packet(1,out_data,4,USB_DTS_TOGGLE))
         {
            if (++count > 10)
            {
               if (++x_seq>=MOUSE_SEQUENCE_STEPS) {x_seq=0;}
               if (++y_seq>=MOUSE_SEQUENCE_STEPS) {y_seq=0;}
               count=0;
            }
         }
      }
   }
}
