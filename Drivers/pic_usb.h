////
//// This driver is for old PIC16C745 and PIC16C765 devices and is no
//// longer supported by CCS.
//// For 16F USB support, see pic16f_usb.h
////
/////////////////////////////////////////////////////////////////////////
////                            pic_usb.c                            ////
////                                                                 ////
////  Microchip PIC16C7x5 Hardware layer for CCS's PIC USB driver    ////
////                                                                 ////
//// This file is part of CCS's PIC USB driver code.  See USB.H      ////
//// for more documentation and a list of examples.                  ////
////                                                                 ////
////   *************************  NOTE  **************************   ////
////  Need a pullup resistor (1.5K) connecting Vusb (C3) to D- (C4). ////
////  PIC16C7x5 will not work as a USB device without this pull-up   ////
////  resistor.                                                      ////
////                                                                 ////
////  PIC16C7x5 must run at 24Mhz.  You can use a 24Mhz oscillator,  ////
////  or a 6Mhz oscillator if you use enable the HS 4x PLL.  The     ////
////  USB peripheral is dependent on an accurate clock, so use an    ////
////  oscillator and not a crystal.                                  ////
////                                                                 ////
////   *************************  NOTE  **************************   ////
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
////   06-30-05: usb_tbe() added                                     ////
////             The way endpoint 0 DTS is set has been changed.     ////
////                                                                 ////
//// June 20th, 2005:                                                ////
////    Cleanup (18Fxx5x project).                                   ////
////    Functions now use newer USB_DTS_BIT enum.                    ////
////    Method of which endpoints are configured (see                ////
////      usb_set_configured()) changed to use new config constants  ////
////      (see usb_ep_tx_type[], usb_ep_tx_size[], etc) in usb.h.    ////
////    Method of which code determines a valid configuration, to    ////
////      prevent using invalid endpoints and memory, changed.       ////
////    USB_MAX_ENDPOINTS define removed.                            ////
////    usb_ep0_rx_buffer[] and usb_ep0_tx_buffer[] defined here,    ////
////      instead of usb.h, since these are hardware dependent.      ////
////    Usb_Buffer constant removed, replaced with                   ////
////      USB_GENERAL_BUFFER_START.                                  ////
////    usb_kbhit() added.                                           ////
////    Upon reception of an OUT token on endpoint 1 or 2, a global  ////
////      boolean is set TRUE.  (see usb_kbhit())                    ////
////    usb_init_ep0_setup() added.                                  ////
////    usb_attached() added.                                        ////
////    usb_task() added.                                            ////
////    usb_flush_packet_0() added.                                  ////
////    usb_init_cs() added.                                         ////
////    usb_attach() added.                                          ////
////    usb_detach() added.                                          ////
////    USB_PIC16C7X5_SMALLER_STALL configuration option removed.    ////
////    USB_endpoint_in_stalled[] and USB_endpoint_out_stalled[]     ////
////      removed.                                                   ////
////    Put Packet, Get Packet and Token Done interrupt cleaned up.  ////
////                                                                 ////
//// June 24th, 2004:                                                ////
////    Optimization and cleanup.                                    ////
////    The way error counter in PIC16C7x5 is defined has changed.   ////
////    USB_PIC16C7X5_SMALLER_STALL added to change between two      ////
////       stall/unstall routine.  Will default to smallest routine  ////
////       but may be more unstable.                                 ////
////    Will now auto-configure endpoint configuration based upon    ////
////       USB_EPx_TX_ENABLE, USB_EPx_RX_ENABLE and USB_EPx_RX_SIZE. ////
////    USB_ISR_HANDLE_TOKDNE option added to change the way TOK_DNE ////
////       interrupt is handled (either handle by interrupt or       ////
////       user polling).                                            ////
////    usb_stall_ep(), usb_unstall_ep(), and usb_endpoint_stalled() ////
////       don't have direction as a parameter.  Will get direction  ////
////       from bit7 of the endpoint.                                ////
////                                                                 ////
//// May 25th, 2004: Typo in usb_get_packet() prototype              ////
////                                                                 ////
//// June 20th, 2003: Minor cleanup                                  ////
////                                                                 ////
//// October 28th, 2002: Fixed typos                                 ////
////                                                                 ////
//// October 25th, 2002: Changed IN Endpoints to initialize to DATA1 ////
////                     after device configuration                  ////
////                                                                 ////
//// September 12th, 2002: Fixed a problem with usb_put_packet()     ////
////                       not sending packets or sending packets    ////
////                       with all zeros.                           ////
////                                                                 ////
//// August 28th, 2002: Fixed a problem with data toggle sync when   ////
////                    sending data to PC (host).                   ////
////                                                                 ////
//// August 2nd, 2002: Initial Public Release                        ////
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

#IFNDEF __PIC16_USB_H__
#DEFINE __PIC16_USB_H__

#DEFINE __USB_HARDWARE__

#ifdef USB_USE_FULL_SPEED
 #undef USB_USE_FULL_SPEED
#endif

#define USB_USE_FULL_SPEED FALSE

#define USB_MAX_EP0_PACKET_LENGTH   8

#DEFINE __PIC__   1	//let the USB Stack know that we are using a PIC16C7x5

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
int1 usb_flush_in(int8 endpoint, int8 len, USB_DTS_BIT tgl);

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

#ENDIF
