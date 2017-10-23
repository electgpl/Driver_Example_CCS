/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////                        ex_usb_kbmouse.c                         ////
////                                                                 ////
//// An example of creating a mouse and keyboard combo USB device,   ////
//// implemented by using multiple interfaces.  One interface is     ////
//// used for the mouse, another interface for the keyboard.         ////
//// Uses your operating system's HID drivers, which on most systems ////
//// should already be installed.                                    ////
////                                                                 ////
//// Once enumrated, the PIC will move the mouse cursor around in    ////
//// a circle if the button is held down.  Also, whenever the        ////
//// button is held down the PIC will send an 'a' as a keyboard      ////
//// press.  (open a text editor to see the 'a' being sent)          ////
////                                                                 ////
//// This file is part of CCS's PIC USB driver code.  See USB.H      ////
//// for more documentation and a list of examples.                  ////
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
//// June 20th, 2005: Initial 18fxx5x release.                       ////
////                                                                 ////
//// May 13th, 2005: Initial Beta.  Only works with 18Fxx5x          ////
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
#include <usb_desc_kbmouse.h>    //USB Configuration and Device descriptors for this UBS device
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
      uart_printf("\r\n\nUSB unenumerated by PC/HOST, waiting for enumeration...");

   last_connected=new_connected;
   last_enumerated=new_enumerated;
}

/////////////////////////////////////////////////////////////////////////////
//
// usb_mouse_task()
//
// Sends a packet of data containing mouse information.  The protocol was
// defined in the HID report descriptor (see usb_desc_kbmouse.h), and is:
//   out_data[0]=button state
//   out_data[1]=x delta
//   out_data[2]=y delta
//   out_data[3]=wheel delta
//
/////////////////////////////////////////////////////////////////////////////
void usb_mouse_task(void) 
{
   #define MOUSE_SEQUENCE_STEPS  16
   const char mouse_seq[MOUSE_SEQUENCE_STEPS]=
      {0, 1, 3, 4, 4, 4, 3, 1, 0, -1, -3, -4, -4, -4, -3, -1};

   int8 out_data[4];
   static int8 x_seq=0;  static int8 y_seq=MOUSE_SEQUENCE_STEPS/4;
   static int8 skip=0;

   if (BUTTON_PRESSED()) 
   {
      out_data[1] = mouse_seq[x_seq];
      out_data[2] = mouse_seq[y_seq];
   }
   else {
      out_data[1] = 0;
      out_data[2] = 0;
   }
   out_data[0] = 0;
   out_data[3] = 0;

   if (usb_put_packet(1,out_data,4,USB_DTS_TOGGLE)) 
   {
      skip++;
      if (skip>10) 
      {
         if (++x_seq>=MOUSE_SEQUENCE_STEPS) {x_seq=0;}
         if (++y_seq>=MOUSE_SEQUENCE_STEPS) {y_seq=0;}
         skip=0;
      }
   }
}

/////////////////////////////////////////////////////////////////////////////
//
// usb_keyboard_task()
//
// Sends a packet of keyboard data.  The protocol was specified in the HID
// report descriptor (see usb_desc_kbmouse.h), and is:
//     tx_msg[0] = modifier (an 8bit bitmap of shift, tab, alt keypress)
//     tx_msg[1] = const 0
//     tx_msg[2:6] = an array of held down keys.  a=4, b=5, etc.
//                   if msg[2:7]={0} then no keys are held down
//
//     rx_msg[0] = 5bit bitmap of led status
//
/////////////////////////////////////////////////////////////////////////////
void usb_keyboard_task(void) 
{
   static int8 tx_msg[7]={0,0,0,0,0,0,0};
   static int8 leds;

   if (BUTTON_PRESSED())
      tx_msg[2]=4;
   else
      tx_msg[2]=0;

   usb_put_packet(2,tx_msg,sizeof(tx_msg),USB_DTS_TOGGLE);

   //receive NUM LOCK, CAPS LOCK, etc LED status from PC.
   //we won't do anything with it.
   if (usb_kbhit(2)) 
   {
      usb_get_packet(2, &leds, 1);
   }
}

void main(void) 
{
   HW_INIT();
   
   LED_ON(LED1);
   LED_OFF(LED2);
   LED_OFF(LED3);

   uart_printf("\r\n\nCCS HID Mouse/Keyboard Multiple Interface Demo");
   uart_printf("\r\nPress button to make mouse move in a circle and ");
   uart_printf("send an A over the keyboard");
   
  #ifdef __PCH__
   uart_printf("\r\nPCH: v");
   uart_printf(__PCH__);
  #elif defined(__PCD__)
   uart_printf("\r\nPCD: v");
   uart_printf(__PCD__);
  #else
   uart_printf("\r\nPCM: v");
   uart_printf(__PCM__);
  #endif

   usb_init_cs();

  #if !(__USB_PIC_PERIF__)
   uart_printf("\r\nUSBN: 0x%X", usbn_get_version());
  #endif
   uart_printf("\r\n");

   while (TRUE)
   {
      usb_task();
      usb_debug_task();

      if (usb_enumerated())
      {
         usb_mouse_task();
         usb_keyboard_task();
         delay_ms(10);
      }
   }
}

