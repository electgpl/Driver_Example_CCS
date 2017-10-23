/////////////////////////////////////////////////////////////////////////
////                          pic24_usb.h                            ////
////                                                                 ////
//// Hardware layer for CCS's USB library.  This hardware layer      ////
//// supports the USB peripheral on the PIC24 family chips.          ////
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

#IFNDEF __PIC24_USB_H__
#DEFINE __PIC24_USB_H__

#DEFINE __USB_HARDWARE__

//let the USB Stack know that we are using a PIC with internal USB peripheral
#DEFINE __PIC__   1

#ifndef USB_USE_FULL_SPEED
 #define USB_USE_FULL_SPEED   TRUE
#endif

#ifndef USB_MAX_EP0_PACKET_LENGTH
   #if USB_USE_FULL_SPEED==FALSE
      //slow speed requires 8byte max packet size for endpoint 0
      #DEFINE USB_MAX_EP0_PACKET_LENGTH   8
   #else
      //for full speed you can still use 8bytes, but 64 will be faster
      #DEFINE USB_MAX_EP0_PACKET_LENGTH   64
   #endif
#endif

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
int1 usb_flush_in(unsigned int8 endpoint, unsigned int16 len, USB_DTS_BIT tgl);

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
void usb_flush_out(unsigned int8 endpoint, USB_DTS_BIT tgl);

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
unsigned int16 usb_rx_packet_size(unsigned int8 endpoint);

#ENDIF

