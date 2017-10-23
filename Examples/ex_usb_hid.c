/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////                        ex_usb_hid.c                             ////
////                                                                 ////
//// A simple demo that shows how to use the HID class to send       ////
//// and receive custom data from the PC.  Normally HID is used      ////
//// with pre-defined classes, such as mice and keyboards, but       ////
//// can be used for vendor specific classes as shown in this        ////
//// example.                                                        ////
////                                                                 ////
//// The first byte sent by the PIC is the ADC reading               ////
//// on channel 0 (AN0), the second byte is the status of the        ////
//// pushbutton.  The two bytes sent by the PC are LED on/off        ////
//// toggle status.                                                  ////
////                                                                 ////
//// CCS has provided an example Windows program that reads / writes ////
//// to the HID device provided in this example.  This Windows       ////
//// example is called hiddemo.exe.                                  ////
////                                                                 ////
//// Normally this program sends and receives 2 bytes.  These        ////
//// sizes can be changed with USB_CONFIG_HID_TX_SIZE and            ////
//// USB_CONFIG_HID_RX_SIZE.  Changing these values will cause       ////
//// hiddemo.exe to not work.                                        ////
////                                                                 ////
//// This file is part of CCS's PIC USB driver code.  See USB.H      ////
//// for more documentation and a list of examples.                  ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// ABOUT HID:                                                      ////
////                                                                 ////
//// HID devices are useful because HID drivers are generally        ////
//// installed and supplied with modern operating systems.           ////
//// However, since HID is a general device with general drivers     ////
//// there are limitations to what a HID device can do:              ////
////                                                                 ////
//// - A report (message) can't be larger than 255 bytes.            ////
////  (A report or message != A packet)                              ////
////                                                                 ////
//// - On slow speed devices the max packet size is 8.               ////
////   On a full speed device the max packet size is 64.             ////
////                                                                 ////
//// - Data is obtained on the host / PC through polling the HID     ////
////   device (PC polls the PIC)                                     ////
////                                                                 ////
//// - On a full speed device, max polling rate is 1 transaction     ////
////   per 1ms. This is 64000 bytes per second (64 byte packets      ////
////   every 1ms).                                                   ////
////                                                                 ////
//// - On a slow speed device, max polling rate is 1 transaction     ////
////   per 10ms.  This is 800 bytes per second (8 byte packets every ////
////   10ms)                                                         ////
////                                                                 ////
//// - No guarantee that polls will happen at a guaranteed rate.     ////
////   If you want guaranteed transfer rate you must use Isochronous ////
////   transfers, which HID is not.                                  ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////                      WINDOWS DRIVERS:                           ////
////                                                                 ////
//// USB HID drivers come included with all Windows, starting        ////
//// with Windows 98 and up.  Windows 95 does not include USB        ////
//// support, unless you are running Windows 95 Gold (aka OSR2, or   ////
//// Windows 95b).                                                   ////
////                                                                 ////
//// If you plug in the USB device, and it is working, a "Found new  ////
//// device" window should pop-up.  The Add Device wizard then will  ////
//// install the HID drivers automatically.                          ////
////                                                                 ////
//// If you plug in the USB device and do not get this window then   ////
//// the device is not working, or USB does not work on your         ////
//// computer.  Goto the Device Manager (Right Click on my Computer, ////
//// it is located under one of the Tabs.  It is located under the   ////
//// Hardware tab of Windows2000), and make sure that "Universal     ////
//// Serial Bus controllers" is installed.  Open "Universal Serial   ////
//// Bus controllers" and you should see 1 or more "USB Root Hubs".  ////
//// If you see these then that's a good indication that USB is      ////
//// working on your computer.  If you see a question mark and       ////
//// an unkown USB device then this is quite possibly your USB       ////
//// device that is not working.                                     ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// NOTE ABOUT ENDPOINT BUFFER SIZE:                                ////
////                                                                 ////
//// Although this application sends and receives only 2 bytes from  ////
//// the PC, this demo defines USB_EP1_TX_SIZE and USB_EP1_RX_SIZE   ////
//// to 8 to allocate 8 bytes for these endpoint.  These constants   ////
//// are also used in the endpoint descriptor to specify the         ////
//// endpoint max size.  If you were concious of RAM savings you     ////
//// could redefine these to 2 (or even 1!), but you would lose      ////
//// throughput. The reason for throughput loss is that if you send  ////
//// a packet that is the same size as the max packet size then      ////
//// you need to send a 0 len packet to specify end of message       ////
//// marker.  The routines usb_puts() and usb_gets() send and        ////
//// receive multiple packet message, waiting for a 0 len packet     ////
//// or a packet that is smaller than max-packet size.               ////
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
//// June 20th, 2005: 18fxx5x initial release.                       ////
////                                                                 ////
//// March 22nd, 2005: Cleanup to work with PIC18Fxx5x               ////
////                                                                 ////
//// June 24th, 2004:  Cleanup to work with updated USB API          ////
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
#if __USB_PIC_PERIF__
 #if defined(__PCM__)
  #include <pic16f_usb.h>   //Microchip PIC16Fxxxx hardware layer for usb.c
 #elif defined(__PCH__)
  #include <pic18_usb.h>   //Microchip PIC18Fxx5x hardware layer for usb.c
 #elif defined(__PCD__)
  #include <pic24_usb.h>   //Microchip PIC24 hardware layer for usb.c
 #endif
#else
 #include <usbn960x.h>   //National 960x hardware layer for usb.c
#endif
#include <usb_desc_hid.h>   //USB Configuration and Device descriptors for this UBS device
#include <usb.c>        //handles usb setup tokens and get descriptor reports

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
      uart_printf(uart_putc, "\r\n\nUSB connected, waiting for enumeration...");
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

void display_array(unsigned int8 *p, unsigned int8 n)
{
   while(n--)
   {
      uart_printf(uart_putc, " 0x%X", *p++);
   }
}

void main(void)
{
   int8 out_data[USB_CONFIG_HID_TX_SIZE];
   int8 in_data[USB_CONFIG_HID_RX_SIZE];
   int8 send_timer=0;

   HW_INIT();
   
   LEDS_OFF();

   setup_adc_ports(HW_ADC_PORTS);
   setup_adc(HW_ADC_CONFIG);
   set_adc_channel(HW_ADC_CHANNEL);

   uart_printf(uart_putc, "\r\n\nCCS Vendor Specific HID Example");

  #ifdef __PCH__
   uart_printf(uart_putc, "\r\nPCH: v");
   uart_printf(uart_putc, __PCH__);
  #elif defined(__PCD__)
   uart_printf(uart_putc, "\r\nPCD: v");
   uart_printf(uart_putc, __PCD__);
  #else
   uart_printf(uart_putc, "\r\nPCM: v");
   uart_printf(uart_putc, __PCM__);
  #endif

   usb_init_cs();

  #if !(__USB_PIC_PERIF__)
   uart_printf(uart_putc, "\r\nUSBN: 0x%X", usbn_get_version());
  #endif
   uart_printf(uart_putc, "\r\n");
   
   while (TRUE)
   {
      usb_task();
      
      usb_debug_task();
      
      if (usb_enumerated())
      {
         if (!send_timer)
         {
            send_timer=250;
            out_data[0] = read_adc();
            out_data[1] = BUTTON_PRESSED();
            
            if (usb_put_packet(USB_HID_ENDPOINT, out_data, USB_CONFIG_HID_TX_SIZE, USB_DTS_TOGGLE))
            {
               uart_printf(uart_putc, "\r\n<-- Sending %u bytes:", USB_CONFIG_HID_TX_SIZE);
               display_array(out_data, USB_CONFIG_HID_TX_SIZE);
            }
         }
         
         if (usb_kbhit(USB_HID_ENDPOINT))
         {
            usb_get_packet(USB_HID_ENDPOINT, in_data, USB_CONFIG_HID_RX_SIZE);
            
            uart_printf(uart_putc, "\r\n--> Received %u bytes:", USB_CONFIG_HID_RX_SIZE);
            display_array(in_data, USB_CONFIG_HID_RX_SIZE);
            
           #if defined(LEDS_GREEN)
            if (in_data[0] && in_data[1])
            {
               LEDS_RED();
            }
            else if (in_data[0] || in_data[1])
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

         delay_ms(1);
      }
   }
}
