////
//// This driver is for old PIC16C745 and PIC16C765 devices and is no
//// longer supported by CCS.
//// For 16F USB support, see pic16f_usb.c
////
/////////////////////////////////////////////////////////////////////////
////                            pic_usb.c                            ////
////                                                                 ////
////  Microchip PIC16C7x5 Hardware layer for CCS's PIC USB driver    ////
////  See pic_usb.h for documentation.                               ////
////                                                                 ////
//// This file is part of CCS's PIC USB driver code.  See USB.H      ////
//// for more documentation and a list of examples.                  ////
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

#IFNDEF __PIC16_USB_C__
#DEFINE __PIC16_USB_C__

#define debug_usb(a,b,c,d,e,f,g,h,i,k,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z)
#define debug_putc

#INCLUDE <usb.h>

//if you enable this it will keep a counter of the 8 possible errors the pic can detect.
//disabling this will save you ROM, RAM and execution time.
#ifndef USB_USE_ERROR_COUNTER
   #define USB_USE_ERROR_COUNTER FALSE
#endif

//if you enable this, the token handler is processed in the USB interrupt.
//if you disable this, the token hanlder must be polled.  The USB interrupt handles everything else.
//disabling this will save you RAM and ROM since a large portion of code will be taken out of the ISR.
//enabling this will give you more reliable code.
//the function that must be polled very quickly (within 10ms) if you disable this is usb_isr_tok_dne()
#ifndef USB_ISR_HANDLE_TOKDNE
   #define USB_ISR_HANDLE_TOKDNE TRUE
#endif

//if you are worried that the PIC is not receiving packets because a bug in the
//DATA0/DATA1 synch code, you can set this to TRUE to ignore the DTS on
//receiving.
#ifndef USB_IGNORE_RX_DTS
 #define USB_IGNORE_RX_DTS FALSE
#endif

#if USB_USE_ERROR_COUNTER
   int ERROR_COUNTER[8];
#endif

#IFNDEF STANDARD_INTS  //standard ints is what goes into UIE, EXCEPT FOR ACTIVITY BIT
   #if  USB_ISR_HANDLE_TOKDNE   //when set to true, the isr will handle TOK_DNE
      #if USB_USE_ERROR_COUNTER
         #define STANDARD_INTS   0x3B
      #else
         #define STANDARD_INTS   0x39
      #endif
   #else
      #if USB_USE_ERROR_COUNTER
         #define STANDARD_INTS   0x33
      #else
         #define STANDARD_INTS   0x31
      #endif
   #endif // TOK_DNE
#ENDIF

#define USB_BUFFER_NEEDED (USB_EP1_TX_SIZE+USB_EP1_RX_SIZE+USB_EP2_TX_SIZE+USB_EP2_RX_SIZE)

#if (USB_EP3_RX_SIZE + USB_EP4_RX_SIZE + USB_EP5_RX_SIZE + USB_EP6_RX_SIZE + USB_EP7_RX_SIZE + USB_EP8_RX_SIZE + USB_EP9_RX_SIZE + USB_EP10_RX_SIZE + USB_EP11_RX_SIZE + USB_EP12_RX_SIZE + USB_EP13_RX_SIZE + USB_EP14_RX_SIZE + USB_EP15_RX_SIZE)
   #error The PIC16C7x5 only has endpoints 0 through 2
#endif

#if (USB_EP3_TX_SIZE + USB_EP4_TX_SIZE + USB_EP5_TX_SIZE + USB_EP6_TX_SIZE + USB_EP7_TX_SIZE + USB_EP8_TX_SIZE + USB_EP9_TX_SIZE + USB_EP10_TX_SIZE + USB_EP11_TX_SIZE + USB_EP12_TX_SIZE + USB_EP13_TX_SIZE + USB_EP14_TX_SIZE + USB_EP15_TX_SIZE)
   #error The PIC16C7x5 only has endpoints 0 through 2
#endif

#if (USB_BUFFER_NEEDED > 24)
   #error You defined more endpoint buffer space than the PIC16C7x5 has memory for
#endif


//---pic16c7xx memory locations
#byte UIR     =  0x190
#byte UIE     =  0x191
#byte UEIR    =  0x192
#byte UEIE    =  0x193
#byte USTAT   =  0x194
#byte UCTRL   =  0x195
#bit UCTRL_DEVATT=UCTRL.3
#bit UCTRL_PKTDIS=UCTRL.4
#bit UCTRL_SE0=UCTRL.5
#byte UADDR   =  0x196
#byte USWSTAT =  0x197

#define  UEP0_LOC 0x198
#define UEP(x) *(UEP0_LOC+x)

#define BD0STAT_LOC 0x1A0
#define BD0CNT_LOC 0x1A1
#define BD0ADRL_LOC 0x1A2

#define EP_BDxST_O(x)    *(BD0STAT_LOC + x*8)
#define EP_BDxCNT_O(x)    *(BD0CNT_LOC + x*8)
#define EP_BDxADR_O(x)   *(BD0ADRL_LOC + x*8)
#define EP_BDxST_I(x)    *(BD0STAT_LOC + 4 + x*8)
#define EP_BDxCNT_I(x)    *(BD0CNT_LOC + 4 + x*8)
#define EP_BDxADR_I(x)   *(BD0ADRL_LOC + 4 + x*8)

//start of input/output buffer for endpoints.  ends at 0x1DF
//we will use 0x1B8:0x1BF for Endpoint 0 OUT/SETUP,
//0x1C0:0x1C7 for Endpoint 0 IN.
//0x1C8:0x1DF will be reserved for endpoints 1 and 2.
#define USB_GENERAL_BUFFER_START 0x1B8

#define __USB_UIF_RESET    0x01
#define __USB_UIF_ERROR    0x02
#define __USB_UIF_ACTIVE   0x04
#define __USB_UIF_TOKEN    0x08
#define __USB_UIF_IDLE     0x10
#define __USB_UIF_STALL    0x20

#BIT UIR_USB_RST = UIR.0
#BIT UIR_UERR  =  UIR.1
#BIT UIR_ACTIVITY = UIR.2
#BIT UIR_TOK_DNE = UIR.3
#BIT UIR_UIDLE = UIR.4
#BIT UIR_STALL = UIR.5

#BIT UIE_USB_RST = UIE.0
#BIT UIE_UERR  =  UIE.1
#BIT UIE_ACTIVITY = UIE.2
#BIT UIE_TOK_DNE = UIE.3
#BIT UIE_UIDLE = UIE.4
#BIT UIE_STALL = UIE.5

//See UEPn (0x198-0x19A)
#define ENDPT_DISABLED	0x00   //endpoint not used
#define ENDPT_IN_ONLY	0x02    //endpoint supports IN transactions only
#define ENDPT_OUT_ONLY	0x04    //endpoint supports OUT transactions only
#define ENDPT_CONTROL	0x06    //Supports IN, OUT and CONTROL transactions - Only use with EP0
#define ENDPT_NON_CONTROL 0x0E  //Supports both IN and OUT transactions

//Define the states that the USB interface can be in
//See USWST (0x197)
#define	USWST_POWERED_STATE	0x00
#define	USWST_DEFAULT_STATE	0x01
#define	USWST_ADDRESS_STATE	0x02
#define	USWST_CONFIG_STATE	0x03

enum {USB_STATE_DETACHED=0, USB_STATE_ATTACHED=1, USB_STATE_POWERED=2, USB_STATE_DEFAULT=3,
    USB_STATE_ADDRESS=4, USB_STATE_CONFIGURED=5} usb_state=0;

//--BDendST has their PIDs upshifed 2
#define USB_PIC_PID_IN       0x24  //device to host transactions
#define USB_PIC_PID_OUT      0x04  //host to device transactions
#define USB_PIC_PID_SETUP    0x34  //host to device setup transaction

//global variables that we need.

char usb_ep0_rx_buffer[USB_MAX_EP0_PACKET_LENGTH];
#locate usb_ep0_rx_buffer=USB_GENERAL_BUFFER_START

char usb_ep0_tx_buffer[USB_MAX_EP0_PACKET_LENGTH];
#locate usb_ep0_tx_buffer=USB_GENERAL_BUFFER_START + 8

int8 __setup_0_tx_size;

//interrupt handler, specific to PIC16C765 peripheral only
void usb_isr();
void usb_isr_rst();
void usb_isr_stall(void);
#if USB_USE_ERROR_COUNTER
void usb_isr_uerr();
#endif
void usb_isr_activity();
void usb_isr_uidle();
void usb_isr_tok_dne();

//following functions standard part of CCS PIC USB driver, and used by usb.c
void usb_wrongstate();


//// BEGIN User Functions:

int8 __usb_kbhit_status;

// see usb_hw_layer.h for documentation
int1 usb_kbhit(int8 en)
{
   return(__usb_kbhit_status,en);
}

// see usb_hw_layer.h for documentation
void usb_detach(void) {  //done
   usb_state=USB_STATE_DETACHED;
   USWSTAT=0;     //default to powered state
   UCTRL=0;  //disable USB hardware
   UIE=0;   //disable USB interrupts
   usb_token_reset();              //clear the chapter9 stack
   __usb_kbhit_status=0;
}

// see usb_hw_layer.h for documentation
void usb_attach(void) {
   usb_state = USB_STATE_ATTACHED;
   USWSTAT=0;     //default to powered state
   UCTRL = 0;
   UIR=0;
   UIE=__USB_UIF_IDLE | __USB_UIF_RESET;  //enable IDLE and RESET USB interrupt
   UCTRL_DEVATT = 1;                     // Enable module & attach to bus
}

// see usb_hw_layer.h for documentation
void usb_task(void) {
   if (usb_attached()) {
      if (UCTRL_DEVATT==0) {
         usb_attach();
      }
   }
   else {
      if (UCTRL_DEVATT==1)  {
         usb_detach();
      }
   }

   if ((usb_state == USB_STATE_ATTACHED)&&(!UCTRL_SE0)) {
      usb_state=USB_STATE_POWERED;
      enable_interrupts(INT_USB);
      enable_interrupts(GLOBAL);
   }
}

// see usb_hw_layer.h for documentation
void usb_init_cs(void) {
      USTAT=0;
      USWSTAT=0;     //default to powered state
      UADDR=0;
      usb_state = USB_STATE_DETACHED;
      usb_token_reset();              //clear the chapter9 stack
      __usb_kbhit_status=0;
}

// see usb_hw_layer.h for documentation
void usb_init(void) {
   usb_init_cs();
   usb_attach();
   usb_state=USB_STATE_POWERED;
   enable_interrupts(INT_USB);
   enable_interrupts(GLOBAL);
}

// see pic_usb.h for documentation
int1 usb_flush_in(int8 endpoint, int8 len, USB_DTS_BIT tgl) {
   int8 i;

   //debug_usb(debug_putc,"\r\nPUT %X %U %U",endpoint, tgl, len);

   i=EP_BDxST_I(endpoint);
   if (!bit_test(i,7)) {

      EP_BDxCNT_I(endpoint)=len;

      //debug_display_ram(len, EP_BDxADR_I(endpoint));

      if (tgl == USB_DTS_TOGGLE) {
         i=EP_BDxST_I(endpoint);
         if (bit_test(i,6))
            tgl=USB_DTS_DATA0;  //was DATA1, goto DATA0
         else
            tgl=USB_DTS_DATA1;  //was DATA0, goto DATA1
      }
      else if (tgl == USB_DTS_USERX) {
         i=EP_BDxST_O(endpoint);
         if (bit_test(i,6))
            tgl=USB_DTS_DATA1;
         else
            tgl=USB_DTS_DATA0;
      }
      
      if (tgl == USB_DTS_DATA1) {
         i=0xC8;  //DATA1, UOWN
      }
      else if (tgl == USB_DTS_DATA0) {
         i=0x88; //DATA0, UOWN
      }

      EP_BDxST_I(endpoint)=i;//save changes

      return(1);
   }
   return(0);
}

// see usb_hw_layer.h for documentation
int1 usb_tbe(int8 en)
{
   int8 st;
   st=EP_BDxST_I(en);
   if (!bit_test(st,7))
      return(TRUE);
   return(FALSE);
}

// see usb_hw_layer.h for documentation
int1 usb_put_packet(int8 endpoint, int8 * ptr, unsigned int16 len, USB_DTS_BIT tgl)
{
   int8 i;
   int8 * buff_add;

   #if (sizeof(buff_add) != 2)
    #error USB Library requires 16bit pointers to be enabled
   #endif

   if (!bit_test(EP_BDxST_I(endpoint),7)) {

      buff_add=EP_BDxADR_I(endpoint) + (int16)0x100;

      for (i=0;i<len;i++) {
         *buff_add=*ptr;
         buff_add++;
         ptr++;
      }

      return(usb_flush_in(endpoint, len, tgl));
    }
    return(0);
}

/// END User Functions


/// BEGIN Hardware layer functions required by USB.C

// see usb_hw_layer.h documentation
void usb_request_send_response(unsigned int8 len) {__setup_0_tx_size=len;}
void usb_request_get_data(void) {__setup_0_tx_size=0xFE;}
void usb_request_stall(void) {__setup_0_tx_size=0xFF;}

/*****************************************************************************
/* usb_init_ep0_setup()
/*
/* Summary: Configure EP0 to receive setup packets
/*
/*****************************************************************************/
void usb_init_ep0_setup(void) {
    EP_BDxCNT_O(0) = USB_MAX_EP0_PACKET_LENGTH;
    EP_BDxADR_O(0) = USB_GENERAL_BUFFER_START;
   #if USB_IGNORE_RX_DTS
    EP_BDxST_O(0) = 0x80; //give control to SIE, data toggle synch off
   #else
    EP_BDxST_O(0) = 0x88; //give control to SIE, DATA0, data toggle synch on
   #endif

    EP_BDxST_I(0) = 0;
    EP_BDxADR_I(0) = USB_GENERAL_BUFFER_START + USB_MAX_EP0_PACKET_LENGTH;
}

// see pic_usb.h for documentation
void usb_flush_out(int8 endpoint, USB_DTS_BIT tgl) {
   int8 i;

      i=EP_BDxST_O(endpoint);
      if (tgl == USB_DTS_TOGGLE) {
         if (bit_test(i,6))
            tgl=USB_DTS_DATA0;  //was DATA1, goto DATA0
         else
            tgl=USB_DTS_DATA1;  //was DATA0, goto DATA1
      }
      if (tgl == USB_DTS_STALL) {
         i=0x84;
         EP_BDxST_I(endpoint)=0x84; //stall both in and out endpoints
      }
      else if (tgl == USB_DTS_DATA1) {
         i=0xC8;  //DATA1, UOWN
      }
      else if (tgl == USB_DTS_DATA0) {
         i=0x88; //DATA0, UOWN
      }

   bit_clear(__usb_kbhit_status,endpoint);
   EP_BDxCNT_O(endpoint)=usb_ep_rx_size[endpoint];
   EP_BDxST_O(endpoint)=i;
}

// see usb_hw_layer.h for documentation
unsigned int16 usb_get_packet(int8 endpoint, int8 * ptr, unsigned int16 max)
{
   int8 * al;
   int8 i;

   #if (sizeof(al) != 2)
    #error USB Library requires 16bit pointers to be enabled
   #endif

   al=EP_BDxADR_O(endpoint) + (int16)0x100;
   i=EP_BDxCNT_O(endpoint);

   if (i<max) {max=i;}

   i=0;

   while (i<max) {
       *ptr=*al;
       ptr++;
       al++;
       i++;
   }

   usb_flush_out(endpoint, USB_DTS_TOGGLE);

   return(max);
}

// see usb_hw_layer.h for documentation
void usb_stall_ep(int8 endpoint) 
{
   int1 direction;
   direction=bit_test(endpoint,7);
   endpoint&=0x7F;
   if (direction) {
      EP_BDxST_I(endpoint)=0x84;
   }
   else {
      EP_BDxST_O(endpoint)=0x84;
   }
}

// see usb_hw_layer.h for documentation
void usb_unstall_ep(int8 endpoint) 
{
   int1 direction;
   direction=bit_test(endpoint,7);
   endpoint&=0x7F;
   if (direction) {
      #if USB_IGNORE_RX_DTS
      EP_BDxST_I(endpoint)=0x80;
      #else
      EP_BDxST_I(endpoint)=0x88;
      #endif
   }
   else {
      EP_BDxST_O(endpoint)=0x00;
   }
}

// see usb_hw_layer.h for documentation
int1 usb_endpoint_stalled(int8 endpoint) 
{
   int1 direction;
   int8 st;
   direction=bit_test(endpoint,7);
   endpoint&=0x7F;
   if (direction) {
      st=EP_BDxST_I(endpoint);
   }
   else {
      st=EP_BDxST_O(endpoint);
   }
   return(bit_test(st,7) && bit_test(st,2));
}

// see usb_hw_layer.h for documentation
void usb_set_address(int8 address) 
{
   UADDR=address;
   if (address) {
      usb_state=USB_STATE_ADDRESS;
      USWSTAT=USWST_ADDRESS_STATE;
   }
   else {
      usb_state=USB_STATE_POWERED;
      USWSTAT=USWST_DEFAULT_STATE;
   }
}

// see usb_hw_layer.h for documentation
void usb_set_configured(int8 config) 
{
   int8 en;
   int16 addy;
   int8 new_uep;

   if (config==0) {
      //if config=0 then set addressed state
      USWSTAT=USWST_ADDRESS_STATE;
      usb_state=USB_STATE_ADDRESS;
      UEP(1)=ENDPT_DISABLED;
      UEP(2)=ENDPT_DISABLED;
   }
   else {
      USWSTAT=USWST_CONFIG_STATE; //else set configed state
      usb_state=USB_STATE_CONFIGURED; //else set configed state
      addy=(int16)USB_GENERAL_BUFFER_START + (int16)16;  //first 16 bytes used for endpoint 0
      for (en=1;en<3;en++) {
         new_uep=0;
         if (usb_ep_rx_type[en]!=USB_ENABLE_DISABLED) {
            new_uep=0x04;
            EP_BDxCNT_O(en)=usb_ep_rx_size[en];
            EP_BDxADR_O(en)=addy;
            addy+=usb_ep_rx_size[en];
           #if USB_IGNORE_RX_DTS
            EP_BDxST_O(en)=0x80;
           #else
            EP_BDxST_O(en)=0x88;
           #endif
         }
         if (usb_ep_tx_type[en]!=USB_ENABLE_DISABLED) {
            new_uep|=0x02;
            EP_BDxADR_I(en)=addy;
            addy+=usb_ep_tx_size[en];
            EP_BDxST_I(en)=0x40;
         }
         if (new_uep==0x06) {new_uep=0x0E;}
         UEP(en)=new_uep;
      }
   }
}


/// END Hardware layer functions required by USB.C


/// BEGIN USB Interrupt Service Routine

/*******************************************************************************
/* usb_isr()
/*
/* Summary: Checks the interrupt, and acts upon event.  Processing finished
/*          tokens is the majority of this code, and is handled by usb.c
/*
/* NOTE: If you wish to change to a polling method (and not an interrupt method),
/*       then you must call this function rapidly.  If there is more than 10ms
/*       latency the PC may think the USB device is stalled and disable it.
/*       To switch to a polling method, remove the #int_usb line above this fuction.
/*       Also, goto usb_init() and remove the code that enables the USB interrupt.
/********************************************************************************/
#int_usb
void usb_isr() {
      if (UIR_USB_RST && UIE_USB_RST) {usb_isr_rst();}        //usb reset has been detected
      if (UIR_UERR && UIE_UERR) {
        #if USB_USE_ERROR_COUNTER
         usb_isr_uerr();
        #else
        UIR_UERR=0;
        #endif
      }          //error has been detected
      if (UIR_ACTIVITY && UIE_ACTIVITY) {usb_isr_activity();}  //activity detected.  (only enable after sleep)
      if (UIR_UIDLE && UIE_UIDLE) {usb_isr_uidle();}        //idle time, we can go to sleep
     #if USB_ISR_HANDLE_TOKDNE
      if (UIR_TOK_DNE && UIE_TOK_DNE) {usb_isr_tok_dne();}    //a token has been detected (majority of isrs)
     #endif
      if (UIR_STALL && UIE_STALL) {usb_isr_stall();}        //a stall handshake was sent
}

/*******************************************************************************
/* usb_isr_rst()
/*
/* Summary: The host (computer) sent us a RESET command.  Reset USB device
/*          and token handler code to initial state.
/*
/********************************************************************************/
void usb_isr_rst() {
   usb_token_reset();

   UIR_TOK_DNE=0;    //do this 4 times to clear out the ustat fifo
   UIR_TOK_DNE=0;
   UIR_TOK_DNE=0;
   UIR_TOK_DNE=0;

   usb_init_ep0_setup();

   UADDR=0;          //set USB Address to 0

   UEP(0)=ENDPT_CONTROL; //endpoint 0 is a control pipe and requires an ACK
   UEP(1)=ENDPT_DISABLED; //turn on endpoint 1 is an IN/OUT pipe.
   UEP(2)=ENDPT_DISABLED;

   UIE=STANDARD_INTS;         //enable all interrupts except activity
   #if USB_USE_ERROR_COUNTER
      UEIE=0xFF;        //enable all error interrupts
   #endif

   USWSTAT=USWST_DEFAULT_STATE; //put usb mcu into default state
   usb_state=USB_STATE_DEFAULT; //put usb mcu into default state
   UIR_USB_RST=0;    //clear reset flag
}

/*******************************************************************************
/* usb_isr_uerr()
/*
/* Summary: The USB peripheral had an error.  If user specified, error counter
/*          will incerement.  I having problems check the status of these 8 bytes.
/*
/* NOTE: This code is not enabled by default.
/********************************************************************************/
#if USB_USE_ERROR_COUNTER
void usb_isr_uerr() {
   int ints;

   ints=UEIR & UEIE; //mask off the flags with the ones that are enabled
   if ( bit_test(ints,0) ) {ERROR_COUNTER[0]++;}   //increment pid_error counter
   if ( bit_test(ints,1) ) {ERROR_COUNTER[1]++;}   //increment crc5 error counter
   if ( bit_test(ints,2) ) {ERROR_COUNTER[2]++;}   //increment crc16 error counter
   if ( bit_test(ints,3) ) {ERROR_COUNTER[3]++;}   //increment dfn8 error counter
   if ( bit_test(ints,4) ) {ERROR_COUNTER[4]++;}   //increment bto error counter
   if ( bit_test(ints,5) ) {ERROR_COUNTER[5]++;}   //increment wrt error counter
   if ( bit_test(ints,6) ) {ERROR_COUNTER[6]++;}   //increment own error counter
   if ( bit_test(ints,7) ) {ERROR_COUNTER[7]++;}   //increment bts error counter

   UEIR=0;           //clear flags
   UIR_UERR=0;
}
#endif

/*******************************************************************************
/* usb_isr_uidle()
/*
/* Summary: USB peripheral detected IDLE.  Put the USB peripheral to sleep.
/*
/********************************************************************************/
void usb_isr_uidle() {
   UIR_UIDLE=0;
   bit_set(UCTRL,1); //set suspend. we are now suspended
   bit_clear(UIR,2); //clear activity interept flag
   bit_set(UIE,2);   //enable activity interrupt flag. (we are now suspended until we get an activity interrupt. nice)
}


/*******************************************************************************
/* usb_isr_activity()
/*
/* Summary: USB peripheral detected activity on the USB device.  Wake-up the USB
/*          peripheral.
/*
/********************************************************************************/
void usb_isr_activity() {
   bit_clear(UIE,2);  //clear activity interupt enabling
   UIR_ACTIVITY=0;
   bit_clear(UCTRL,1); //turn off low power suspending
   bit_clear(UIR,4);  //clear idle flag
   bit_set(UIE,4);    //turn on idle interrupts
}

/*******************************************************************************
/* usb_isr_stall()
/*
/* Summary: Stall handshake detected.
/*
/********************************************************************************/
void usb_isr_stall(void) {
   if (bit_test(UEP(0),0)) {
      usb_init_ep0_setup();
      bit_clear(UEP(0),0);
   }
   UIR_STALL=0;
}

/*******************************************************************************
/* usb_isr_tok_dne()
/*
/* Summary: A Token (IN/OUT/SETUP) has been received by the USB peripheral.
/*          Information about token is received, and sent to usb.c's token handling
/*          code.
/*
/********************************************************************************/
void usb_isr_tok_dne() {
   int8 en;

   if (USTAT==0) {   //new out or setup token in the buffer
      if ((EP_BDxST_O(0) & 0x3C)==USB_PIC_PID_SETUP) { //setup PID
         __setup_0_tx_size=0xFF;

         EP_BDxST_I(0)=0;   // return the in buffer to us (dequeue any pending requests)

         usb_isr_tok_setup_dne();

            //if setup_0_tx_size==0xFF - stall ep0 (unhandled request)
            //if setup_0_tx_size==0xFE - get EP0OUT ready for a data packet, leave EP0IN alone
            //else setup_0_tx_size=size of response, get EP0OUT ready for a setup packet, mark EPOIN ready for transmit
            if (__setup_0_tx_size==0xFF)
               usb_flush_out(0,USB_DTS_STALL);
            else {
               usb_flush_out(0,USB_DTS_TOGGLE);
               if (__setup_0_tx_size!=0xFE) {
                  usb_flush_in(0,__setup_0_tx_size,USB_DTS_USERX);
               }
            }


         UCTRL_PKTDIS=0;       // UCON,PKT_DIS ; Assuming there is nothing to dequeue, clear the packet disable bit
      }
      else if ((EP_BDxST_O(0) & 0x3C)==USB_PIC_PID_OUT) {
            usb_isr_tok_out_dne(0);
            usb_flush_out(0,USB_DTS_TOGGLE);
            if (__setup_0_tx_size!=0xFE) {   //send 0 len ack
               usb_flush_in(0,__setup_0_tx_size,USB_DTS_USERX);
            }
      }
   }

   else if (USTAT==4) {   //pic -> host transfer completed
         __setup_0_tx_size=0xFF;
         usb_isr_tok_in_dne(0);
         if (__setup_0_tx_size!=0xFF)
            usb_flush_in(0,__setup_0_tx_size,USB_DTS_TOGGLE);
         else
            usb_init_ep0_setup();
   }

   else {
      if (!bit_test(USTAT,2)) {
         en=USTAT>>3;
         bit_set(__usb_kbhit_status, en);         
         usb_isr_tok_out_dne(en);
      }
      else {
         usb_isr_tok_in_dne(en);
      }
   }
   UIR_TOK_DNE=0;
}

/// END USB Interrupt Service Routine

#ENDIF
