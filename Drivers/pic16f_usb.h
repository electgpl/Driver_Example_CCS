/////////////////////////////////////////////////////////////////////////
////                          pic16f_usb.h                           ////
////                                                                 ////
//// Hardware layer for CCS's USB library.  This hardware layer      ////
//// supports the USB peripheral on the enhanced PIC family chips.   ////
//// Current supported families are:                                 ////
////     PIC16F1454/1455/1459                                        ////
////                                                                 ////
//// This file is part of CCS's PIC USB driver code.  See USB.H      ////
//// for more documentation and a list of examples.                  ////
////                                                                 ////
//// See the section labeled EXTRA USER FUNCTIONS for functions      ////
//// that are specific to only this HW peripheral that you may       ////
//// find useful to use in your application.                         ////
////                                                                 ////
//// This driver lets you use endpoints 0 thru 15.  Endpoint 0 is    ////
//// the setup/control endpoint and is always anabled.               ////
//// You can use any endpoint, but to save RAM it is recommended     ////
//// that you use lower number endpoints and in order.  For example, ////
//// use endpoints 1, 2 and 3 INSTEAD OF using endpoints 1, 8        ////
//// and 15.                                                         ////
////                                                                 ////
////  USB_MAX_EP0_PACKET_LENGTH will be set to 8 regardless          ////
////  of USB speed, to save RAM.                                     ////
////                                                                 ////
////   ************** NOTE ABOUT HW REQUIREMENTS ****************    ////
////  If you are not using internal pullups, you will need to put    ////
////  an internal pullup resistor on D+ or D+ depending on if you    ////
////  want to use slow speed or full speed.  This code configures    ////
////  the device to use internal pullups, see usb_init() if you      ////
////  want to change that.                                           ////
////                                                                 ////
////  You need approximately 470nF cap on Vusb, even if you are      ////
////  using the internal 3.3V USB regulator.                         ////
////                                                                 ////
////  To run at full speed, you must use the oscillator              ////
////  configuration (PLLx) to set the PLL divide to 4MHz.  You can   ////
////  configure the MCU clock to any speed (up to 48MHz) but the     ////
////  PLL must run at 4Mhz to provide the USB peripheral with a      ////
////  96MHz clock.  See the datasheet for details.                   ////
////                                                                 ////
////  To run at slow speed you must configure your MCU to run at     ////
////  24Mhz.  See the datasheet for details.                         ////
////                                                                 ////
////   ****************  NOTE ABOUT INTERRUPTS  ******************   ////
//// This driver uses INT_USB.  It requires INT_USB to interrupt the ////
//// PIC when an event has happened on the USB Bus.  Therfore        ////
//// this code enables interrupts.  A user modification can be made  ////
//// to poll the USB interrupt flag instead of relying on an         ////
//// interrupt.                                                      ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// Version History:                                                ////
////                                                                 ////
//// Feb 15, 2013:                                                   ////
////  Enhanced PIC16 support added.                                  ////
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

#IFNDEF __PIC16F_USB_H__
#DEFINE __PIC16F_USB_H__

#DEFINE __USB_HARDWARE__

//let the USB Stack know that we are using a PIC with internal USB peripheral
#DEFINE __PIC__   1

#ifndef USB_USE_FULL_SPEED
 #define USB_USE_FULL_SPEED   1
#endif

#define USB_MAX_EP0_PACKET_LENGTH   8

#if (!USB_USE_FULL_SPEED && (USB_MAX_EP0_PACKET_LENGTH!=8))
 #error Slow speed devices must use a max packet size of 8 for endpoint 0!
#endif

#if USB_MAX_EP0_PACKET_LENGTH < 8
 #error Max Endpoint 0 length can't be less than 8!
#endif

#if USB_MAX_EP0_PACKET_LENGTH > 64
 #error Max Endpoint 0 length can't be greater than 64!
#endif

#include <usb_hw_layer.h>

//////////////////////// EXTRA USER FUNCTIONS /////////////////////////////////

/**************************************************************
/* usb_flush_in()
/*
/* Input: endpoint - which endpoint to mark for transfer
/*        len - length of data that is being tramsferred
/*        tgl - Data toggle synchronization for this packet
/*
/* Output: TRUE if success, FALSE if error (we don't control the endpoint)
/*
/* Summary: Marks the endpoint ready for transmission.  You must
/*          have already loaded the endpoint buffer with data.
/*          (IN is PIC -> PC).
/*          This routine is useful if you want to setup an endpoint by
/*          writing to the buffer directly.
/*          This routine is not necessary if you use usb_put_packet().
/***************************************************************/
int1 usb_flush_in(int8 endpoint, int16 len, USB_DTS_BIT tgl);

/**************************************************************
/* usb_flush_out()
/*
/* Input: endpoint - which endpoint to mark for release
/*        tgl - Data toggle synchronization to expect in the next packet
/*
/* Output: NONE
/*
/* Summary: Clears the previously received packet, and then marks this
/*          endpoint's receive buffer as ready for more data.
/*          (OUT is PC -> PIC).  This routine is useful if you do not
/*          want to use usb_get_packet(), instead you want to handle with
/*          the endpoint buffer directly.  Also see usb_rx_packet_size().
/*          This routine is not necessary if you use usb_get_packet().
/***************************************************************/
void usb_flush_out(int8 endpoint, USB_DTS_BIT tgl);

/**************************************************************
/* usb_rx_packet_size()
/*
/* Input: endpoint - which endpoint to mark to check
/*
/* Output: Returns the number of bytes in the endpoint's receive buffer.
/*
/* Summary: Read the number of data stored in the receive buffer.  When you
/*    have handled the data, use usb_flush_out() to clear the buffer.  The
/*    result may be invalid if usb_kbhit() returns false for this endpoint.
/*    This routine is not necessary if you use usb_get_packet().
/***************************************************************/
int16 usb_rx_packet_size(int8 endpoint);

#ENDIF
