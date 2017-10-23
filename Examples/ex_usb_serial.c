/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////                        ex_usb_serial.c                          ////
////                                                                 ////
//// A demonstration of the USB CDC API that is provided by CCS.     ////
//// The USB CDC API that CCS provides will create a virtual UART    ////
//// port.  USB CDC drivers are included with most versions of       ////
//// Microsoft Windows, and when properly loaded will create a COMx  ////
//// port from which you can write and read to your PIC device       ////
//// like any serial device that has a COMx port.                    ////
////                                                                 ////
//// This example creates a USB<->UART converter.  Open              ////
//// Hyperterminal to COM1 (or whatever COM port is your usual RS232 ////
//// serial COM port).  Plug the PIC to USB.  Open Hypertimernal to  ////
//// the new COM port that is the USB<->UART COM port (for this      ////
//// example say it is COM2).  Typing a character in COM1 will cause ////
//// it to be sent out COM2, and vice-versa.                         ////
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
//// Mar 11th, 2011:                                                 ////
////  Shortened one printf() so it will fit on PIC16.                ////
////                                                                 ////
//// Oct 27th, 2011:                                                 ////
////  Added SERIAL_STATE support.  See usb_cdc_serial_state() for    ////
////     documentation.  In order for this to work,                  ////
////     USB_CDC_COMM_IN_SIZE has to be 11 bytes or bigger.          ////
////     DSR/DCD is sent to host/PC and it's value is the current    ////
////     state of the button.  This is the same routine to send ring ////
////     or break signals to the host/PC.                            ////
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

#include <ex_usb_common.h>

#include <usb_cdc.h>

#include <string.h>

/////////////////////////////////////////////////////////////////////////////
//
// usb_debug_task()
//
// When called periodically, displays debugging information over serial
// to display enumeration and connection states.  Also lights LED1 based upon
// enumeration and status.
//
/////////////////////////////////////////////////////////////////////////////
void usb_debug_task(void)
{
   static int8 last_connected;
   static int8 last_enumerated;
   int8 new_connected;
   int8 new_enumerated;
   static int8 last_cdc;
   int8 new_cdc;

   new_connected=usb_attached();
   new_enumerated=usb_enumerated();
   new_cdc=usb_cdc_connected();

   if (new_enumerated)
      LED_ON(LED1);
   else
      LED_OFF(LED1);

   if (new_cdc)
      LED_ON(LED2);
   else
      LED_OFF(LED2);

   if (usb_cdc_carrier.dte_present)
      LED_ON(LED3);
   else
      LED_OFF(LED3);

   if (new_connected && !last_connected)
      uart_printf("USB connected, waiting for enumaration...\r\n\n");
   if (!new_connected && last_connected)
      uart_printf("USB disconnected, waiting for connection...\r\n\n");
   if (new_enumerated && !last_enumerated)
      uart_printf("USB enumerated by PC/HOST\r\n\n");
   if (!new_enumerated && last_enumerated)
      uart_printf("USB unenumerated by PC/HOST, waiting for enumeration...\r\n\n");
   if (new_cdc && !last_cdc)
      uart_printf("Serial program initiated on USB<->UART COM Port\r\n\n");

   last_connected=new_connected;
   last_enumerated=new_enumerated;
   last_cdc=new_cdc;
}

// transmit to the host new dcd/dsr, it's value based on button.
void cdc_serial_state_task(void)
{
   cdc_serial_state_t newState;
   static cdc_serial_state_t lastState;

   memset(&newState, 0x00, sizeof(newState));

   newState.bRxCarrier = BUTTON_PRESSED();
   newState.bTxCarrier = newState.bRxCarrier;

   if (memcmp(&newState, &lastState, sizeof(newState)) != 0)
   {
      uart_printf("\r\nNew button state being sent: ");
      //change in state, send it.
      if (usb_cdc_serial_state(newState))
      {
         //state was sent ok, save it in memory so we don't resend it.
         memcpy(&lastState, &newState, sizeof(lastState));
         uart_printf("OK");
      }
      else
      {
         uart_printf("FAIL");
      }
      uart_printf("\r\n");
   }
}

void main(void)
{
   char c;

   HW_INIT();

   LED_OFF(LED1);
   LED_OFF(LED2);
   LED_OFF(LED3);

   uart_printf("\r\n\nCCS CDC (Virtual RS232) Example\r\n");

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
   uart_printf("\r\n");

   // init USB (non-blocking)
   // usb_task() need to be called in your loop to finish USB initialization.
   usb_init_cs();

  #if !(__USB_PIC_PERIF__)
   uart_printf("USBN: 0x%X", usbn_get_version());
   uart_printf("\r\n\n");
  #endif


   while (TRUE)
   {
      // service low level USB operations.
      usb_task();

      // special debugging routines for this program.
      usb_debug_task();

      // send host DSR state, which is based on button.
      cdc_serial_state_task();

      if (uart_kbhit())
      {
         c=uart_getc();
         if (c=='\n') {usb_cdc_putc('\r'); usb_cdc_putc('\n');}
         else if (c=='\r') {usb_cdc_putc('\r'); usb_cdc_putc('\n');}
        #if defined(__PCM__)
         else if (c=='!') {uart_printf(usb_cdc_putc,"HELLO-WORLD-HELLO-WORLD-HELLO-WORLD-DONE");}
        #else
         else if (c=='!') {uart_printf(usb_cdc_putc,"HELLO-WORLD-HELLO-WORLD-HELLO-WORLD-HELLO-WORLD-HELLO-WORLD-HELLO-WORLD-HELLO-WORLD-HELLO-WORLD-DONE");}
        #endif
         else {usb_cdc_putc(c);}
      }
      if (usb_cdc_kbhit())
      {
         c=usb_cdc_getc();
         if (c=='\n') {uart_putc('\r'); uart_putc('\n');}
         else if (c=='\r') {uart_putc('\r'); uart_putc('\n');}
         else {uart_putc(c);}
      }
   }
}
