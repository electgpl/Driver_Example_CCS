/////////////////////////////////////////////////////////////////////////
////                      usbn960x.h                                 ////
////                                                                 ////
//// National USBN960x Hardware layer for CCS's PIC USB driver.      ////
////                                                                 ////
//// This example USB peripheral layer was written and tested with   ////
//// CCS's USB Full Speed demo board.  When using your own design,   ////
//// either wire your USB960x the same as our demo board or change   ////
//// the port and pin definitions below.                             ////
////                                                                 ////
//// Hardware only supports 3 endpoints (not counting endpoint 0).   ////
////                                                                 ////
//// The following definitions must be made in order to communicate  ////
//// with the USBN960x device (default config is made in this        ////
//// file if not specified by the user):                             ////
////     usbn_bus_out(x)                                             ////
////     usbn_bus_in()                                               ////
////     usbn_bus_float()                                            ////
////     usbn_bus_control()                                          ////
////     USBN_INT       PIN_B0                                       ////
////     USBN_RS        PIN_C1                                       ////
////     USBN_CS        PIN_C2                                       ////
////     USBN_RD        PIN_C3                                       ////
////     USBN_WR        PIN_C4                                       ////
////     USBN_A0        PIN_C5                                       ////
////                                                                 ////
//// This file is part of CCS's PIC USB driver code.  See USB.H      ////
//// for more documentation and a list of examples.                  ////
////                                                                 ////
////   *************************  NOTE  **************************   ////
//// This driver uses INT_EXT.  It requires INT_EXT to interrupt the ////
//// PIC when an event has happened on the USBN960x.  Because of     ////
//// this code enables interrupts.  A user modification can be made  ////
//// to poll the USBN960x device instead of relying on an interrupt. ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// Version History:                                                ////
////                                                                 ////
//// March 5th, 2009:                                                ////
////   Cleanup for Wizard.                                           ////
////   PIC24 Initial release.                                        ////
////                                                                 ////
//// 11-03-05:                                                       ////
////   The way EP0 is flushed and enabled in the TX and RX           ////
////      interrupts and put and get packet routines completely      ////
////      overhauled.                                                ////
////   TX and RX interrupt handler cleaned up.                       ////
////                                                                 ////
//// 10-27-05:                                                       ////
////   usb_get_packet_buffer() added.                                ////
////   usb_flush_out() added.                                        ////
////                                                                 ////
//// 06-30-05:                                                       ////
////    usb_tbe() added                                              ////
////    The way endpoint 0 DTS is set has been changed.              ////
////    usb_kbhit() moved back to usb.c                              ////
////    Fixed a bug where EP0 wasn't being stalled if an invalid     ////
////        setup request was made by the host.                      ////
////    Added usb_get_packet_status()                                ////
////                                                                 ////
//// June 20th, 2005:                                                ////
////    18Fxx5x initial release                                      ////
////    Functions now use newer USB_DTS_BIT enum.                    ////
////    usb_ep0_rx_buffer[] and usb_ep0_tx_buffer[] moved from usb.h ////
////    usb_kbhit() added.                                           ////
////    usb_attached() added.                                        ////
////    usb_task() added.                                            ////
////    usb_flush_packet_0() added.                                  ////
////    usb_init_cs() added.                                         ////
////    usb_attach() added.                                          ////
////    usb_detach() added.                                          ////
////                                                                 ////
//// June 24th, 2004:                                                ////
////    Optimization and cleanup.                                    ////
////    Some definitions may have changed.                           ////
////    usb_find_epc(), usb_stall_ep(), usb_unstall_ep(), and        ////
////       usb_endpoint_stalled() don't have direction as a          ////
////       parameter.  Will get direction from bit7 of the endpoint. ////
////                                                                 ////
//// October 27th, 2003: RX/TX - IN/OUT Backwards                    ////
////                             in usb_set_configured()             ////
////                                                                 ////
//// June 18th, 2003: Fixed a problem where PIC would miss interrupt ////
////                                                                 ////
//// May 6th, 2003: Fixed a potential read/write to registers        ////
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

#IFNDEF __USBN960X_H__
#DEFINE __USBN960X_H__

#DEFINE __USB_HARDWARE__

#ifndef USB_USE_FULL_SPEED
   #define USB_USE_FULL_SPEED TRUE
#endif

#define USB_MAX_EP0_PACKET_LENGTH   8

#define __USBN__ 1   //let other parts of the code know that we are running a National USB Part

//CCS USB demo board is connected to the USBN960x in this manner:
#ifndef usbn_out
   #define  usbn_bus_out(x)      output_d(x)
   #define  usbn_bus_in          input_d
   #define  usbn_bus_float()     set_tris_d(0xFF)
   #define  usbn_bus_control()   set_tris_d(0)
#endif

//CCS USB demo board is connected to the USBN960x in this manner:
#ifndef  USBN_CS
   #define  USBN_INT       PIN_B0   //leave at B0 if you want to use external interrupt
   #define  USBN_RS        PIN_C1   //reset
   #define  USBN_CS        PIN_C2
   #define  USBN_RD        PIN_C3
   #define  USBN_WR        PIN_C4
   #define  USBN_A0        PIN_C5
#endif

#include <usb_hw_layer.h>

//////////////////////// EXTRA USER FUNCTIONS /////////////////////////////////

 // NO EXTRA USER FUNCTIONS //

#ENDIF
