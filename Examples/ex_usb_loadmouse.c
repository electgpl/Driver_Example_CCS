/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////                      ex_usb_loadmouse.c                         ////
////                                                                 ////
//// This is a re-work of ex_usb_mouse.c to be used by the the       ////
//// USB/CDC bootloader ex_usb_bootloader.c.  For more information,  ////
//// see those files.                                                ////
////                                                                 ////
//// This file is part of CCS's PIC USB driver code.  See USB.H      ////
//// for more documentation and a list of examples.                  ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// VERSION HISTORY                                                 ////
////                                                                 ////
//// July 9th, 2012:                                                 ////
////  Added support for PCD (see ex_usb_bootloader.c).               ////
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

#include <usb_bootloader.h>

/////////////////////////////////////////////////////////////////////////////
//
// Include the CCS USB Libraries.  See the comments at the top of these
// files for more information
//
/////////////////////////////////////////////////////////////////////////////
#if __USB_PIC_PERIF__
 #if defined(__PCM__)
  #include <pic_usb.h>   //Microchip PIC16C765 hardware layer for usb.c
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

   last_connected = new_connected;
   last_enumerated = new_enumerated;
}

void main(void) 
{
   #define MOUSE_SEQUENCE_STEPS  16
   const char mouse_seq[MOUSE_SEQUENCE_STEPS]=
      {0, 1, 3, 4, 4, 4, 3, 1, 0, -1, -3, -4, -4, -4, -3, -1};

   int8 out_data[4];
   int8 x_seq = 0;  int8 y_seq = MOUSE_SEQUENCE_STEPS/4;
   int8 count = 0;
   
   HW_INIT();

   LED_ON(LED1);
   LED_OFF(LED2);
   LED_OFF(LED3);

   uart_printf("\r\n\nCCS USB Mouse Example");
  #ifdef __PCH__
   uart_printf("\r\nPCH: v");
   uart_printf(__PCH__);
  #endif
  #ifdef __PCM__
   uart_printf("\r\n\PCM: v");
   uart_printf(__PCM__);
  #endif
  #ifdef __PCD__
   uart_printf("\r\n\PCD: v");
   uart_printf(__PCD__);
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
         out_data[0]=0; //button state goes here
         out_data[1]=mouse_seq[x_seq];
         out_data[2]=mouse_seq[y_seq];
         out_data[3]=0; //wheel state goes here
         if (usb_put_packet(1,out_data,4,USB_DTS_TOGGLE))
            count++;
         if (count > 10) 
         {
            if (++x_seq>=MOUSE_SEQUENCE_STEPS) {x_seq=0;}
            if (++y_seq>=MOUSE_SEQUENCE_STEPS) {y_seq=0;}
            count=0;
         }
         delay_ms(10);
      }
   }
}
