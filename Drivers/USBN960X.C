/////////////////////////////////////////////////////////////////////////
////                      usbn960x.c                                 ////
////                                                                 ////
//// National USBN960x Hardware layer for CCS's PIC USB driver.  See ////
//// usbn960x.h for more documentation.                              ////
////                                                                 ////
//// This file is part of CCS's PIC USB driver code.  See USB.H      ////
//// for more documentation and a list of examples.                  ////
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

#IFNDEF __USBN960X_C__
#DEFINE __USBN960X_C__

#define debug_usb(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z)
#define debug_putc

//#define debug_usb printf
//#define debug_putc putc_tbe
//#define debug_putc UART1
//void debug_putc(char c) {
//   if ((__usb_cdc_state==USB_CDC_OUT_LINECODING)||temporary_debug)
//      putc_tbe(c);
//}

#include <usb.h>

#if (( (USB_EP1_TX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP2_TX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP3_TX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP4_TX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP5_TX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP6_TX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP7_TX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP8_TX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP9_TX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP10_TX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP11_TX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP12_TX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP13_TX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP14_TX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP15_TX_ENABLE!=USB_ENABLE_DISABLED) ) > 3)
   #error You defined more TX endpoints than the USBN960x is capable
#endif

#if (( (USB_EP1_RX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP2_RX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP3_RX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP4_RX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP5_RX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP6_RX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP7_RX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP8_RX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP9_RX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP10_RX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP11_RX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP12_RX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP13_RX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP14_RX_ENABLE!=USB_ENABLE_DISABLED) + (USB_EP15_RX_ENABLE!=USB_ENABLE_DISABLED) ) > 3)
   #error You defined more RX endpoints than the USBN960x is capable
#endif

//usbn9603/9604 register set
#define     USBN_MCNTRL    0x00
#define     USBN_CCONF     0x01
//          reserved       0x02
#define     USBN_RID       0x03
#define     USBN_FAR       0x04
#define     USBN_NFSR      0x05
#define     USBN_MAEV      0x06
#define     USBN_MAMSK     0x07
#define     USBN_ALTEV     0x08
#define     USBN_ALTMSK    0x09
#define     USBN_TXEV      0x0A
#define     USBN_TXMSK     0x0B
#define     USBN_RXEV      0x0C
#define     USBN_RXMSK     0x0D
#define     USBN_NAKEV     0x0E
#define     USBN_NAKMSK    0x0F
#define     USBN_FWEV      0x10
#define     USBN_FWMSK     0x11
#define     USBN_FNH       0x12
#define     USBN_FNL       0x13
#define     USBN_DMACNTRL  0x14
#define     USBN_DMAEV     0x15
#define     USBN_DMAMSK    0x16
#define     USBN_MIR       0x17
#define     USBN_DMACNT    0x18
#define     USBN_DMAERR    0x19
//          reserved       0x1A
#define     USBN_WKUP      0x1B
//          reserved       0x1C
//          reserved       0x1D
//          reserved       0x1E
//          reserved       0x1F
#define     USBN_EPC0      0x20
#define     USBN_TXD0      0x21
#define     USBN_TXS0      0x22
#define     USBN_TXC0      0x23
//          reserved       0x24
#define     USBN_RXD0      0x25
#define     USBN_RXS0      0x26
#define     USBN_RXC0      0x27
#define     USBN_EPC1      0x28
#define     USBN_TXD1      0x29
#define     USBN_TXS1      0x2A
#define     USBN_TXC1      0x2B
#define     USBN_EPC2      0x2C
#define     USBN_RXD1      0x2D
#define     USBN_RXS1      0x2E
#define     USBN_RXC1      0x2F
#define     USBN_EPC3      0x30
#define     USBN_TXD2      0x31
#define     USBN_TXS2      0x32
#define     USBN_TXC2      0x33
#define     USBN_EPC4      0x34
#define     USBN_RXD2      0x35
#define     USBN_RXS2      0x36
#define     USBN_RXC2      0x37
#define     USBN_EPC5      0x38
#define     USBN_TXD3      0x39
#define     USBN_TXS3      0x3A
#define     USBN_TXC3      0x3B
#define     USBN_EPC6      0x3C
#define     USBN_RXD3      0x3D
#define     USBN_RXS3      0x3E
#define     USBN_RXC3      0x3F

//CCS turns on the following interrupts:
#ifndef USBN_ALTMSK_ENABLES
   #define USBN_ALTMSK_ENABLES   0xC0
#endif
#ifndef USBN_TXMSK_ENABLES
   #define USBN_TXMSK_ENABLES    0xFF  //interrupt on any change on TX interrupt (IN packet or OVR)
#endif
#ifndef USBN_RXMSK_ENABLES
   #define USBN_RXMSK_ENABLES    0xFF  //interrupt on any change on RX interrupt (OUT/SETUP packet or OVR)
#endif
#ifndef USBN_NAKMSK_ENABLES
   #define USBN_NAKMSK_ENABLES   0x10  //interrupt only on NAK on EP0 OUT/SETUP packets.
   //#define USBN_NAKMSK_ENABLES   0x00  //interrupt only on NAK on EP0 OUT/SETUP packets.
#endif
#ifndef USBN_FWMSK_ENABLES
   #define USBN_FWMSK_ENABLES    0x00  //dont interrupt at all for frame events
#endif
#ifndef USBN_MAMSK_ENABLES
   #define USBN_MAMSK_ENABLES    0xD6  //interrupt on ALT, NAK, TX and RX events
   //#define USBN_MAMSK_ENABLES    0xC6  //interrupt on ALT, TX and RX events
#endif

unsigned int8  usbn_current_alt_mask;  //we will modify what ALT masks depending if we are idle or not

//NFSR STATES
#define     USBN_NFSR_RESET        0
#define     USBN_NFSR_RESUME       1
#define     USBN_NFSR_OPERATIONAL  2
#define     USBN_NFSR_SUSPEND      3

unsigned int16 __usb_kbhit_status;

unsigned int8 epc_to_endpoint[7]={0,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

char usb_ep0_rx_buffer[8];
char usb_ep0_tx_buffer[8];

// *** protoypes of user functions
void usb_flush_out(int8 endpoint, USB_DTS_BIT tgl);
int8 usbn_get_version(void);

// *** prototypes used by usb.c (token handling)
unsigned int8 usb_get_packet_status(int8 endpoint, int8 * ptr, unsigned int8 max, int8 *rxstatus);


// *** prototypes (USB ISR)
void usb_check_warn(void);
void usb_check_alt(void);
void usb_check_alt_reset(void);
void usb_check_txev(void);
void usb_check_frame(void);
void usb_check_nak(void);
void usb_check_uld(void);
void usb_check_rxev(void);

// *** prototype of USBN960x specific functions used only in this file
void usb_enable_endpoint(int8 endpoint, int8 epc, int1 iso);
int8 usb_find_epc(int8 endpoint);
unsigned int16 usb_get_packet_buffer_status(int8 endpoint, int8 *ptr, unsigned int16 max, int16 *rxstatus);
void usb_reset(void);
void usbn_write(int8 address, int8 data);
int8 usbn_read(int8 address);

//DONT CHANGE THE FOLLOWING 3 CONSTANT ARRAYS
const char USBN_EPCx[]={USBN_EPC0,USBN_EPC1,USBN_EPC2,USBN_EPC3,USBN_EPC4,USBN_EPC5,USBN_EPC6}; //epc_address[x] finds the constant USBN_EPCx
const char USBN_TX_FIFOx[]={USBN_TXD0, USBN_TXD1, 0,         USBN_TXD2, 0,         USBN_TXD3, 0};          //ecp0,ecp1,ecp3,ecp5 have tx_fifos
const char USBN_RX_FIFOx[]={USBN_RXD0, 0,         USBN_RXD1, 0,         USBN_RXD2, 0,         USBN_RXD3};  //ecp0,ecp2,ecp4,ecp6 have rx_fifos


/// BEGIN User Functions

void usb_init_ep0_setup(void) {
   usbn_write(USBN_EPC0,0);
   usbn_write(USBN_TXC0, 0x08);   //flush fifos
   usbn_write(USBN_RXC0, 0x08);
   usbn_write(USBN_RXC0, 0x01);    //enable rx0
}

unsigned int8 usbn_is_attached=FALSE;

unsigned int8 __setup_0_tx_size;

// see usb_hw_layer.h for documentation
void usb_request_send_response(unsigned int8 len) {__setup_0_tx_size=len;}
void usb_request_get_data(void) {__setup_0_tx_size=0xFE;}
void usb_request_stall(void) {__setup_0_tx_size=0xFF;}

// see usb_hw_layer.h for documentation
void usb_attach(void) {
   debug_usb(debug_putc,"\r\nUSB ATTACH");
   usbn_is_attached=TRUE;
   usb_reset();
   delay_ms(10);
   usbn_write(USBN_MCNTRL, 0x04); //vge
   delay_ms(2);
   usbn_write(USBN_MCNTRL, 0xCC); //VGE, NAT, active low
   ext_int_edge(H_TO_L);
   enable_interrupts(INT_EXT);
   enable_interrupts(GLOBAL);
}

// see usb_hw_layer.h for documentation
void usb_detach() {
   debug_usb(debug_putc,"\r\nUSB DETACH");
   usbn_is_attached=FALSE;
   usbn_write(USBN_MCNTRL,0);
   disable_interrupts(INT_EXT);
}

// see usb_hw_layer.h for documentation
void usb_task()
{
  #if defined(USB_CDC_DELAYED_FLUSH)
   if (usb_enumerated())
   {
      usb_cdc_flush_out_buffer();
   }
  #endif

   if (usb_attached()) {
      if (!usbn_is_attached) {
         usb_attach();
      }
   }
   else {
      if (usbn_is_attached)  {
         usb_detach();
      }
   }
}

// see usb_hw_layer.h for documentation
void usb_init_cs(void) {
   debug_usb(debug_putc,"\r\nUSB INIT (cs)");
   output_high(USBN_CS);
   output_float(USBN_A0);
   output_high(USBN_RD);
   output_high(USBN_WR);
   usbn_bus_float();

   __usb_kbhit_status=0;

   output_high(USBN_RS);
   delay_ms(100);
   output_low(USBN_RS);
   delay_ms(10);
   output_high(USBN_RS);
   delay_ms(10);

   usbn_current_alt_mask=USBN_ALTMSK_ENABLES & 0x7F;  //make sure resume is off

   //turn on masks
   usbn_write(USBN_ALTMSK, usbn_current_alt_mask);
   usbn_write(USBN_TXMSK, USBN_TXMSK_ENABLES);
   usbn_write(USBN_RXMSK, USBN_RXMSK_ENABLES);

   usbn_write(USBN_NAKMSK, USBN_NAKMSK_ENABLES);
   usbn_write(USBN_FWMSK, USBN_FWMSK_ENABLES);
   usbn_write(USBN_MAMSK, USBN_MAMSK_ENABLES);

   usbn_write(USBN_EPC0, 0);
   usbn_write(USBN_FAR, 0x80);

   usbn_write(USBN_WKUP,0x0C);

   usbn_write(0x1F,0x40);             //3.3V regulator workaround
}

// see usb_hw_layer.h for documentation
void usb_init(void) {
   debug_usb(debug_putc,"\r\nUSB INIT");
   usb_init_cs();
   usb_attach();
}

// see usb_hw_layer.h for documentation
int1 usb_tbe(int8 endpoint)
{
   unsigned int8 epc;
   unsigned int8 address;
   unsigned int8 status;

   epc=usb_find_epc(endpoint | 0x80);

   if (epc != 0xFF) {
      address=USBN_TX_FIFOx[epc]+2;  //address = USBN_TXCx
      status=usbn_read(address); //TXCx
      if (!bit_test(status,0) || !endpoint) { //make sure last isnt set
         return(TRUE);
      }
   }
   return(FALSE);
}

// see usb_hw_layer.h for documentation
int1 usb_put_packet(int8 endpoint, int8 * ptr, unsigned int16 len, USB_DTS_BIT tgl)
{
   unsigned int8 epc, address, status;

      debug_usb(debug_putc,"\r\nPUT %U ", endpoint);

      epc=usb_find_epc(endpoint | 0x80);

      if (epc != 0xFF) {
         debug_usb(debug_putc,"[%U] - ", len);
         address=USBN_TX_FIFOx[epc];  //address = USBN_TXDx
         address+=2; //TXCx
         status=usbn_read(address); //TXCx
         if ((!bit_test(status,1))||(!endpoint)) { //make sure last isnt set
            if (!endpoint) {  //if endpoint 0 we need to disable rx_en on RXC0
               usbn_write(USBN_RXC0, 0x08);
            }
            status=(status & 0x04) | 0x08;   //flush, but keep toggle
            usbn_write(address,status); //FLUSH
               address-=2; //TXDx
               while (len != 0) {
                  usbn_write(address, *ptr);
                  debug_usb(debug_putc,"%X", *ptr);
                  ptr++;
                  len--;
               }
               status=usbn_read(address+2) & 0x04;  //TXCx
               if (endpoint) {status |= 0x02;}         //set LAST bit
               if (tgl==USB_DTS_TOGGLE) {status ^= 0x04;}   //toggle TOGGLE bit (TOGGLE DATAx)
               else if (tgl==USB_DTS_DATA1) {status |= 0x04;}  //set toggle (DATA1)
               else if (tgl==USB_DTS_DATA0) {status &= ~0x04;} //clear toggle (DATA0)
               status |= 1;   //set TX_EN
               usbn_write(address+2,status);
               debug_usb(debug_putc, " t=%X TXC=%X",tgl,status);
               if (!endpoint) {
                  usbn_write(USBN_RXC0, 0);
               }
               return(1);
         }
      }

   return(0);
}


/*******************************************************************************
/* usbn_get_version()
/*
/* Summary: Stictly for debugging, this function gets the revision number of the
/*          USBN960x you are connected to.  Good to see if you're connection
/*          to the USBN960x is good.
/*
/********************************************************************************/
int8 usbn_get_version(void) {
   return(usbn_read(USBN_RID));
}

/// END User Functions


/// BEGIN Hardware layer functions required by USB.C

// see usb_hw_layer.h
int1 usb_kbhit(int8 en)
{
   return(bit_test(__usb_kbhit_status,en));
}

/*******************************************************************************
/* usb_get_packet_status(endpoint, *ptr, max, *rxstatus)
/*
/* Input: endpoint - endpoint to get data from
/*        ptr - where to save data to local PIC RAM
/*        max - max amount of data to receive from buffer
/*        rxstatus - pointer to location to save rxstatus (set to NULL if you
/*                       don't want to save)
/*
/* Output: the amount of data taken from the buffer. If no data is available in the buffer,
/*         or there was an error, will return FALSE.
/*
/* Summary: Gets a packet of data from the USBN buffer and puts into local PIC RAM.
/*          You could poll usb_get_packet() until it doesn't return 0, or you could
/*          poll usb_epX_rx_status.rx (where X is endpoint number) until .rx = 1.
/*          usb_kbhit(endpoint) (located in usb.c) will poll usb_epX_rx_status.rx,
/*          but is easier to read.
/*
/* NOTE: DOES NOT CHECK IF ENDPOINT IS VALID, USB_FIND_EPC() DOES
/********************************************************************************/
unsigned int8 usb_get_packet_status(int8 endpoint, int8 * ptr, unsigned int8 max, int8 *rxstatus) {
   unsigned int8 rx;
   debug_usb(debug_putc,"\r\nGET %U ", endpoint);
   rx=usb_get_packet_buffer_status(endpoint, ptr, max, rxstatus);
   usb_flush_out(endpoint, 0);
   return(rx);
}

/*******************************************************************************
/* usb_get_packet_buffer(endpoint, *ptr, max)
/*
/* Input: endpoint - endpoint to get data from
/*        ptr - where to save data to local PIC RAM
/*        max - max amount of data to receive from buffer
/*
/* Output: the amount of data taken from the buffer.
/*
/* Summary: Gets a packet of data from the USB buffer and puts into local PIC RAM.
/*          Does not mark the endpoint as ready for more data.  Once you are
/*          done with data, call usb_flush_out() to mark the endpoint ready
/*          to receive more data.
/*
/********************************************************************************/
#define usb_get_packet_buffer(e,p,m) usb_get_packet_buffer_status(e,p,m,0)
unsigned int16 usb_get_packet_buffer_status(int8 endpoint, int8 *ptr, unsigned int16 max, int16 *rxstatus) {
   unsigned int8 epc, address, rx=0, status, len, i;

      epc=usb_find_epc(endpoint);
      if (epc!=0xFF) {
         address=USBN_RX_FIFOx[epc];     //RXDx
         status=usbn_read(address+1);  //RXSx
         if (rxstatus) {*rxstatus=status;}
         if (bit_test(status,7) && endpoint) {  //ERROR!
            usbn_write(address + 2, 0x08);  //FLUSH
            usbn_write(address + 2, 0x01);  //enable
         }
         else if (bit_test(status,4)) {
               len=usbn_read(address+1) & 0x0F;  //RXSx
               while (len) {
                  i=usbn_read(address);
                  if (rx < max) {
                     rx++;
                     *ptr=i;
                     ptr++;
                  }
                  len=usbn_read(address+1) & 0x0F;  //RXSx
               }
               if (!endpoint) {
                  usbn_write(address + 2, 0x08);  //FLUSH
                  usbn_write(address + 2, 0x01);  //enable
               }
         }
      }
   return(rx);
}

/**************************************************************
/* usb_flush_out()
/*
/* Input: endpoint - which endpoint to mark for transfer
/*        tgl - Data toggle synchronization to expect in the next packet
/*              (tgl not used on USBN960x)
/*
/* Output: NONE
/*
/* Summary: Clears the previously received packet, and then marks this
/*          endpoint's receive buffer as ready for more data.
/*          (OUT is PC -> PIC)
/***************************************************************/
void usb_flush_out(int8 endpoint, USB_DTS_BIT tgl) {
   unsigned int8 epc, address, status;

   debug_usb(debug_putc,"\r\nGET %U ", endpoint);

      epc=usb_find_epc(endpoint);
      if (epc!=0xFF) {
         address=USBN_RX_FIFOx[epc];     //RXDx
         status=usbn_read(USBN_RXMSK);
         switch(epc) {
            case 0:  status|=1;
                     break;
            case 2:  status|=2;
                     break;
            case 4:  status|=4;
                     break;
            case 6:  status|=8;
                     break;
         }
         //reenable mask for this interrupt
         usbn_write(USBN_RXMSK,status);
         bit_clear(__usb_kbhit_status, endpoint);


         //flush and re-enable interrupts
         if (endpoint) {
            usbn_write(address+2, 0x08);  //RXCx
            usbn_write(address+2, 0x01);  //RXCx
         }
      }
}

// see usb_hw_layer.h for documentation
unsigned int16 usb_get_packet(int8 endpoint, int8 * ptr, unsigned int16 max)
{
   return(usb_get_packet_status(endpoint, ptr, max, 0));
}

// see usb_hw_layer.h for documentation
void usb_stall_ep(int8 endpoint)
{
   unsigned int8 epc, address, value;

   debug_usb(debug_putc," [STALL %U]",endpoint);

      epc=usb_find_epc(endpoint);
      if (epc!=0xFF) {

         //set STALL
         address=USBN_EPCx[epc];
         value=usbn_read(address);
         value |= 0x80;
         usbn_write(address,value);
      }
}

// see usb_hw_layer.h for documentation
void usb_unstall_ep(int8 endpoint)
{
   unsigned int8 epc, address, value;


      epc=usb_find_epc(endpoint);

   debug_usb(debug_putc," [UNSTALL %U %U]",endpoint,epc);

      if (epc!=0xFF) {
         address=USBN_EPCx[epc];
         value=usbn_read(address);
         value &= 0x7F;
         usbn_write(address,value);
      }
}

// see usb_hw_layer.h for documentation
int1 usb_endpoint_stalled(int8 endpoint) {
   unsigned int8 epc, address, value;

      epc=usb_find_epc(endpoint);
      if (epc!=0xFF) {
         address=USBN_EPCx[epc];
         value=usbn_read(address);
         return(bit_test(value,7));
      }


   return(1);
}

// see usb_hw_layer.h for documentation
void usb_set_address(int8 address) {
   debug_usb(debug_putc," [SA %U]",address);
   usbn_write(USBN_EPC0, usbn_read(USBN_EPC0) | 0x40);
   usbn_write(USBN_FAR, address | 0x80);
}

// see usb_hw_layer.h for documentation
void usb_set_configured(int8 config) {
   unsigned int8 i;
   unsigned int8 txepc=1;
   unsigned int8 rxepc=2;
   unsigned int8 type;

   debug_usb(debug_putc," [CONFIG %U]",config);

   if (config) {
      for (i=1;i<16;i++) {
         type=usb_ep_tx_type[i];
         if ((type!=USB_ENABLE_DISABLED)&&(txepc<=5)) {
            usb_enable_endpoint(i,txepc,type==USB_ENABLE_ISOCHRONOUS);
            txepc+=2;
         }
         type=usb_ep_rx_type[i];
         if ((type!=USB_ENABLE_DISABLED)&&(rxepc<=6)) {
            usb_enable_endpoint(i,rxepc,type==USB_ENABLE_ISOCHRONOUS);
            rxepc+=2;
         }
      }
   }
   else {
      usb_disable_endpoint(1);
      usb_disable_endpoint(2);
      usb_disable_endpoint(3);
      usb_disable_endpoint(4);
      usb_disable_endpoint(5);
      usb_disable_endpoint(6);
   }
}

/// END Hardware layer functions required by USB.C


/// BEGIN USB Interrupt Service Routine

/*******************************************************************************
/* usb_isr()
/*
/* Summary: Checks the Main Event interrupt register, and acts upon USB interrupts.
/*          ALT interrupt happens for the misc interrupts (reset, idle, wakeup, etc.)
/*          NAK interrupt happens when the USBN device generated a NAK to an IN or OUT/SETUP token
/*          TX interrupt happens when an IN token has happend (PIC -> HOST)
/*          RX interrupt happens when an OUT/SETUP token has happened. (HOST -> PIC).
/*          Majority of USB token handling is done through the RX event.
/*
/*          If you choose to use a polling method instead of by interrupt then call
/*          this function on any free time.  Use great care if you choose to poll:
/*          some USB requests require a response within a few milli-seconds.
/*
/********************************************************************************/
#bit USBIE=getenv("BIT:INT0IE")
#int_ext NOCLEAR
void usb_isr(void) {
   unsigned int8 maev;

   clear_interrupt(INT_EXT);
   do {
      maev=usbn_read(USBN_MAEV) & usbn_read(USBN_MAMSK);
      if (maev) {
         if (bit_test(maev,6)) {usb_check_rxev();}    //OUT/SETUP tokens to us                        //0x40
         if (bit_test(maev,1)) {usb_check_alt();}     //Resets, Idles, Suspends, etc.                 //0x02
         if (bit_test(maev,2)) {usb_check_txev();}    //IN token was processed (set up new data?)     //0x04
         if (bit_test(maev,4)) {usb_check_nak();}     //PC tried to send us data, but 960x nak'd it   //0x10
         //if (bit_test(maev,0)) {usb_check_warn();}    //TODO: CCS does not provide this function      //0x01
         //if (bit_test(maev,3)) {usb_check_frame();}   //TODO: CCS does not provide this function      //0x08
         //if (bit_test(maev,5)) {usb_check_uld();}     //TODO: CCS does not provide this function      //0x20
      }
   } while (maev);
}

/*******************************************************************************
/* usb_check_warn()
/*
/* Summary: The WARN event happens when a USBN buffer reaches a desired state.
/*          This requires the user to setup the WARN setting for each buffer.
/*          This capability not provided by CCS.
/*
/********************************************************************************/
void usb_check_warn(void) {
    debug_usb(debug_putc,"\r\nI{W}");
   //TODO: add code to handle when FIFOs get near full / empty
   //This interrupt not used by CCS
}

/*******************************************************************************
/* usb_check_frame()
/*
/* Summary: Handles a FRAME event interrupt.  CCS does not handle this interrupt.
/*          Only relevant to users using isochronous.
/*
/********************************************************************************/
void usb_check_frame(void) {
   //TODO: not used by CCS
}

/*******************************************************************************
/* usb_check_uld()
/*
/* Summary:  Handles ULD interrupt.  Not provided by CCS.
/*
/********************************************************************************/
void usb_check_uld(void) {
   debug_usb(debug_putc,"\r\nI{U}");
   //TODO: not used by CCS
}

/*******************************************************************************
/* usb_check_alt()
/*
/* Summary:  This interrupt happens when an RESET, SUSPEND or RESUME event has happened.
/*            RESET - USBN device is reset, USB token handling code is put into initial state
/*            SUSPEND - Resume interrupt is enabled, USBN device put into suspended mode
/*            RESUME - Suspend interrupt is enabled, USBN device put into operational state, EP0 reception enabled
/*
/********************************************************************************/
void usb_check_alt(void) {
   unsigned int8 status;

   status=usbn_read(USBN_ALTEV) & usbn_current_alt_mask;

   if (bit_test(status,2)) {debug_usb(debug_putc,"\r\nI{A%X DMA}", status);} // DMA.  //Not used by CCS
   else if (bit_test(status,6)) {usb_check_alt_reset();} // RESET
   else if ((bit_test(status,5)) || (bit_test(status,4))) { // SD5 or SD3
      debug_usb(debug_putc,"\r\nI{A%X SD5}", status);
      usbn_current_alt_mask=(usbn_current_alt_mask & 0xCF) | 0x80; //make sure resume interrupt is on, sd3/sd5 is off
      usbn_write(USBN_ALTMSK, usbn_current_alt_mask);
      usbn_write(USBN_NFSR, USBN_NFSR_SUSPEND);
   }
   else if (bit_test(status,7)) { // RESUME
      debug_usb(debug_putc,"\r\nI{A%X RES}", status);
      usbn_current_alt_mask=usbn_current_alt_mask & 0x7F; //TURN ON ALL EXCEPT RESUME
      usbn_write(USBN_ALTMSK, usbn_current_alt_mask);
      usbn_write(USBN_NFSR, USBN_NFSR_OPERATIONAL);
      usbn_write(USBN_RXC0, usbn_read(USBN_RXC0) | 0x01);  //enable reception
   }
   else if (bit_test(status,3)) {debug_usb(debug_putc,"\r\nI{A%X EOP}", status);} // EOP   //resume sequence has been acknowledged and completed by host
   else if (bit_test(status,1)) {debug_usb(debug_putc,"\r\nI{A%X WK}", status);} // WKUP
}

/*******************************************************************************
/* usb_check_alt_reset()
/*
/* Summary: This Alt event happens when the Host sends a reset signal.  We reset the USBN and
/*          put the USB token handling code back into initial state.
/*
/********************************************************************************/
void usb_check_alt_reset(void) {
            usbn_write(USBN_EPC0, usbn_read(USBN_EPC0) & ~0x80); //CLEAR_STALL_EP0
            usbn_write(USBN_NFSR, USBN_NFSR_RESET);

            usbn_write(USBN_FAR, 0x80);   //set default address

            usbn_write(USBN_EPC0, 0x00);         //enable ep0 only

            usb_reset();

            usbn_write(USBN_NFSR, USBN_NFSR_OPERATIONAL);
            usbn_write(USBN_FAR, 0x80);   //default un-addressed address
            usbn_write(USBN_EPC0, 0x40);     //default address
            usbn_write(USBN_RXC0, 0x01);  //enable reception
}

/*******************************************************************************
/* usb_check_nak()
/*
/* Summary: Handles a NAK event.  This interrupt happens when the USBN960x device
/*          generates a NAK.  We only handle the RX NAK, which happens when the Host
/*          tried sending us an OUT/SETUP (host -> pic) packet, but either the buffer
/*          is full or EP0 reception is disabled or EP0 is transmitting.  In all cases
/*          we flush and reset the EP0 buffer.
/*
/*          CCS does not provide code to handle NAKs on the other endpoints.
/*
/********************************************************************************/
void usb_check_nak(void)
{
   unsigned int8 status;

   status=usbn_read(USBN_NAKEV) & USBN_NAKMSK_ENABLES;
   debug_usb(debug_putc,"\r\nI{N%X}",status);
   if (bit_test(status,1)) {} //TODO: not handled by CCS
   if (bit_test(status,2)) {} //TODO: not handled by CCS
   if (bit_test(status,3)) {} //TODO: not handled by CCS
   if (bit_test(status,4) || bit_test(status,0)) {
         usbn_write(USBN_TXC0, 0x08);  //flush TX FIFO, disable TX
         usbn_write(USBN_RXC0, 0x08);  //flush RX FIFO, disable RX
         usbn_write(USBN_TXC0, 0x00);  //disable TX
         usbn_write(USBN_RXC0, 0x01);  //enable RX
         USB_stack_status.dev_req=NONE;       //previous token request state.  init at none
   }
   if (bit_test(status,5)) {} //TODO: not handled by CCS
   if (bit_test(status,6)) {} //TODO: not handled by CCS
   if (bit_test(status,7)) {} //TODO: not handled by CCS
}

/*******************************************************************************
/* usb_check_txev()
/*
/* Summary: The TX interrupt happens whenever an IN token has been processed.  (PIC -> HOST).
/*          Since the IN token has been processed that means the TX buffer is empty.
/*          CCS only handles TX event when EP0 is empty, meaning it's time to send the next packet
/*          of the message if there is one.
/*
/*          CCS does not handle an interrupt on EPC1-EPC3 because we suggest polling, using
/*          repetitive usb_put_packet() calls.  usb_put_packet() will return true if data was put into buffer,
/*          false if the buffer was not ready for data (because it was still putting out the last packet).
/*
/********************************************************************************/
void usb_check_txev(void) {
   unsigned int8 status, txs;

   status=usbn_read(USBN_TXEV) & USBN_TXMSK_ENABLES;

   debug_usb(debug_putc,"\r\nI");
   debug_usb(debug_putc,"{T%X}",status);

   if (status & 0xF0) {
      //TODO: not used by CCS
   }
   if (bit_test(status,0)) {  //sent data out of EP0
      //printf(putc_tbe,"*T");
      txs=usbn_read(USBN_TXS0);
      if (bit_test(txs,6)) {   //ACK
            __setup_0_tx_size=0xFF;
            usb_isr_tok_in_dne(0);
            if ((__setup_0_tx_size!=0xFF)&&(__setup_0_tx_size!=0xFE)) {
               //printf(putc_tbe,"p");
               if (__setup_0_tx_size) {
                  usb_put_packet(0,usb_ep0_tx_buffer,__setup_0_tx_size,USB_DTS_TOGGLE);
               }
               else {
                  //0len packet
                  txs=usbn_read(USBN_TXC0) & 0x04;
                  usbn_write(USBN_TXC0, 0x08);
                  usbn_write(USBN_TXC0, 0x01 | (txs^0x04));
               }
            }
            else {
               //printf(putc_tbe,"r");
               usbn_write(USBN_TXC0, 0x08);
               usbn_write(USBN_RXC0, 0x01);  //enable reception
            }
      }
      else {
         usbn_write(USBN_RXC0, 0x01);
      }
   }
   if (bit_test(status,1)) {
      txs=usbn_read(USBN_TXS1);
      usbn_write(USBN_TXC1, (usbn_read(USBN_TXC1) & 0x04) | 0x08);
      usb_isr_tok_in_dne(epc_to_endpoint[1]);
   }
   if (bit_test(status,2)) {
      txs=usbn_read(USBN_TXS2);
      usbn_write(USBN_TXC2, (usbn_read(USBN_TXC2) & 0x04) | 0x08);
      usb_isr_tok_in_dne(epc_to_endpoint[3]);
   }
   if (bit_test(status,3)) {
      txs=usbn_read(USBN_TXS3);
      usbn_write(USBN_TXC3, (usbn_read(USBN_TXC3) & 0x04) | 0x08);
      usb_isr_tok_in_dne(epc_to_endpoint[5]);
   }
}

/*******************************************************************************
/* usb_check_rxev()
/*
/* Summary: Handles the RX interrupt.  This happens when an OUT/SETUP token has happened (HOST -> PIC)
/*          and there is data in the RX buffers.
/*
/*          If there is data in the EP0 buffer and it is a SETUP token, it is processed by the
/*          SETUP token handler.  The SETUP token handler is the bulk of this code.
/*
/*          If there is data in the EP1-3 buffers then data is placed into RAM on the PIC.
/*          (See usb_isr_tok_out_dne(endpoint)).
/*
/*          TODO: Not provided by CCS - handle non SETUP data on EP0.
/*
/********************************************************************************/
void usb_check_rxev(void)
{
   unsigned int8 status, rxstatus, len, i;
   unsigned int8 en;

   status=usbn_read(USBN_RXEV) & USBN_RXMSK_ENABLES;

   debug_usb(debug_putc,"\r\nI");
   debug_usb(debug_putc,"{R%X}",status);

   if (bit_test(status,0)) {
      rxstatus=usbn_read(USBN_RXS0);
      len=usbn_read(USBN_RXS0) & 0xF;
      for (i=0;i<len;i++) {
         usb_ep0_rx_buffer[i]=usbn_read(USBN_RXD0);
      }
      usbn_write(USBN_TXC0, 0x08);
      usbn_write(USBN_RXC0, 0x08);

      if (bit_test(rxstatus,6)) { //setup
         usbn_write(USBN_EPC0, usbn_read(USBN_EPC0) & ~0x80);//clear stall

         usb_isr_tok_setup_dne();

            //if setup_0_tx_size==0xFF - stall ep0 (unhandled request)
            //if setup_0_tx_size==0xFE - get EP0OUT ready for a data packet, leave EP0IN alone
            //else setup_0_tx_size=size of response, get EP0OUT ready for a setup packet, mark EPOIN ready for transmit
            if (__setup_0_tx_size==0xFF) {
               //printf(putc_tbe,"s");
               //usbn_write(USBN_RXC0, 0x08);  //flush RX FIFO
               usb_stall_ep(0);
               usbn_write(USBN_TXC0, 0x01);
               //usb_put_packet(0,0,0,USB_DTS_DATA1);
            }
            else if (__setup_0_tx_size!=0xFE) {
               //printf(putc_tbe,"a");
               if (__setup_0_tx_size) {
                  usb_put_packet(0,usb_ep0_tx_buffer,__setup_0_tx_size,USB_DTS_DATA1);
               }
               else {
                  //send 0len
                  usbn_write(USBN_TXC0, 0x08);
                  usbn_write(USBN_TXC0, 0x05);
               }
            }
            else {
               //usbn_write(USBN_RXC0, 0x08);  //flush RX FIFO
               //usbn_write(USBN_RXC0, 0x01);  //flush RX FIFO
               //printf(putc_tbe,"w");
               //usbn_write(USBN_RXC0, 0x09);  //flush RX fifo and enable reception
               usbn_write(USBN_RXC0, 0x01);  //flush RX fifo and enable reception
            }
      }
      else {
         __setup_0_tx_size=0xFE;
         usb_isr_tok_out_dne(0);
            if (__setup_0_tx_size==0xFF) {
               //printf(putc_tbe,"s");
               usb_stall_ep(0);
               usbn_write(USBN_TXC0, 0x01);
            }
            else if (__setup_0_tx_size!=0xFE) {
               //printf(putc_tbe,"a");
               if (__setup_0_tx_size) {
                  if (bit_test(rxstatus,4)) {
                     usb_put_packet(0,usb_ep0_tx_buffer,__setup_0_tx_size,USB_DTS_DATA0);
                  }
                  else {
                     usb_put_packet(0,usb_ep0_tx_buffer,__setup_0_tx_size,USB_DTS_DATA1);
                  }
               }
               else {
                  //send 0len
                  usbn_write(USBN_TXC0, 0x08);
                  if (bit_test(rxstatus,4)) {
                     usbn_write(USBN_TXC0, 0x05);
                  }
                  else {
                     usbn_write(USBN_TXC0, 0x01);
                  }
               }
            }
            else {
               //printf(putc_tbe,"w");
               usbn_write(USBN_RXC0, 0x01);  //flush RX fifo and enable reception
            }
      }
   }
   if (bit_test(status,1)) {
      usbn_write(USBN_RXMSK, usbn_read(USBN_RXMSK) & 0xFD); //unmask RXFIFO1 interrupt. will be remasked when you get the packet
      en = epc_to_endpoint[2];
      bit_set(__usb_kbhit_status, en);
      usb_isr_tok_out_dne(en);
   }
   if (bit_test(status,2)) {
      usbn_write(USBN_RXMSK, usbn_read(USBN_RXMSK) & 0xFB); //unmask RXFIFO2 interrupt
      en = epc_to_endpoint[4];
      bit_set(__usb_kbhit_status, en);
      usb_isr_tok_out_dne(en);
   }
   if (bit_test(status,3)) {
      usbn_write(USBN_RXMSK, usbn_read(USBN_RXMSK) & 0xF7); //unmask RXFIFO2 interrupt
      en = epc_to_endpoint[6];
      bit_set(__usb_kbhit_status, en);
      usb_isr_tok_out_dne(en);
   }
}

/// END USB Interrupt Service Routine


/// BEGIN USBN960x Functions only used in this File

/*******************************************************************************
/* usbn_write(address, data)
/*
/* Input: address - address of USBN960x register to write to
/*        data - data to write to USBN960x
/*
/* Summary: Writes a byte of memory to specified address on USBN960x
/*
/********************************************************************************/
void usbn_write(int8 address, int8 data) {
   output_high(USBN_RD);
   output_high(USBN_WR);

   usbn_bus_out(address);
   output_high(USBN_A0);
   output_low(USBN_CS);
   output_low(USBN_WR);
   output_high(USBN_WR);
   output_high(USBN_CS);
   output_float(USBN_A0);

   usbn_bus_out(data);
   output_low(USBN_A0);
   output_low(USBN_CS);
   output_low(USBN_WR);
   output_high(USBN_WR);
   output_high(USBN_CS);
   output_float(USBN_A0);
   usbn_bus_float();
}

/*******************************************************************************
/* usbn_read(address)
/*
/* Input: address - address of USBN960x register to read
/* Output: data (byte) in specified address.
/*
/* Summary: Reads a byte of memory from specified address on USBN960x
/*
/********************************************************************************/
int8 usbn_read(int8 address) {
   unsigned int8 data;

   output_high(USBN_RD);
   output_high(USBN_WR);

   usbn_bus_out(address);
   output_high(USBN_A0);
   output_low(USBN_CS);
   output_low(USBN_WR);
   output_high(USBN_WR);
   output_high(USBN_CS);
   output_float(USBN_A0);
   usbn_bus_float();

   output_low(USBN_A0);
   output_low(USBN_CS);
   output_low(USBN_RD);
   data=usbn_bus_in();
   output_high(USBN_RD);
   output_high(USBN_CS);
   output_float(USBN_A0);
   usbn_bus_float();

   return(data);
}

/*******************************************************************************
/* usb_find_epc(endpoint)
/*
/* Input: endpoint - endpoint to find USBN address.  bit 7: direction of endpoint.  0 == OUT or CONTROL, 1 == IN
/*
/* Output: returns the USBN960x endpoint control number (epc)
/*
/* Summary: The USBN960x has 7 channels (EP0, EP1-3 TX, and EP1-3 RX), but
/*          EP1-3 TX/RX can be mapped to any endpoint from 1-15.  This function
/*          looks up what endpoint is mapped to what channel.
/*
/********************************************************************************/
int8 usb_find_epc(int8 endpoint) {
   unsigned int8 i;

   if ((endpoint & 0x7F)==0) {return(0);}

   if (bit_test(endpoint,7))
      i=1;
   else
      i=2;

   endpoint&=0x7F;   //mask off direction bit
   while (i<8) {
      if (epc_to_endpoint[i]==endpoint)
         return(i);
      i+=2;
   }
   return(0xFF);  //not found
}

/*******************************************************************************
/* usb_reset()
/*
/* Summary: Resetsthe USB token handler code, resets the USBN device (software reset, not hardware),
/*          clears all the USBN buffers, clears the local EP0 buffer, enables reception
/*          on EP0.
/*
/********************************************************************************/
void usb_reset(void) {
   debug_usb(debug_putc,"\r\nUSBN RESET");

   usb_token_reset();

   usbn_write(USBN_FAR, 0);
   usbn_write(USBN_NFSR, USBN_NFSR_RESET);

   usbn_write(USBN_TXC1, 0x08);
   usbn_write(USBN_TXC2, 0x08);
   usbn_write(USBN_TXC3, 0x08);

   usbn_write(USBN_RXC1, 0x08);
   usbn_write(USBN_RXC2, 0x08);
   usbn_write(USBN_RXC3, 0x08);
}

/*******************************************************************************
/* usb_enable_endpoint(endpoint,iso)
/*
/* Input: endpoint - endpoint to enable
/*        epc - Endpoint Control Number to assign this endpoint. [1-6]  (odd=TX/IN, even=RX/OUT)
/*        iso - TRUE if the endpoint is isochronous, FALSE if it is a bulk/interrupt endpoint.
/*
/* Summary: Enables endpoint for receiving / transmitting.
/*
/* NOTE: DOESNT CHECK IF ENDPOINT IS VALID.  USB_FIND_EPC WILL.
/********************************************************************************/
void usb_enable_endpoint(int8 endpoint, int8 epc, int1 iso) {
   unsigned int8 address;
   unsigned int8 value;

   debug_usb(debug_putc," [ENABLE EP %U->%U",endpoint, epc);
   if (iso)
      debug_usb(debug_putc," ISO");
   debug_putc(']');

   if (epc) {
      epc_to_endpoint[epc]=endpoint;

      address=USBN_EPCx[epc];
      value=endpoint & 0x0F;
      value|=0x10;
      if (iso) {value |= 0x20;}
      usbn_write(address,value);

      if (!bit_test(epc,0)) {
         debug_putc('*');
         address=USBN_RX_FIFOx[epc] + 2;  //RXCn
         //value=usbn_read(address);
         //value |= 1; //enable reception
         //usbn_write(address, value);
         usbn_write(address, 0x05);
      }
   }
}

// see usb_hw_layer.h for documentation
// there is a bug here - the usb_hw_layer.h assumes that the paramter of this function
// is the endpoint (1..15), but this routine for the usb960x is using the epc (0..6).
// this bug can be ignored because the USB stack doesn't call usb_disable_endpoint().
void usb_disable_endpoint(int8 en)
{
   unsigned int8 address;

   if (en) {
      //clear EP_EN
      address=USBN_EPCx[en];
      usbn_write(address, 0);

      //clear TX_EN or RX_EN
      if (bit_test(en,0)) { //if EPC is odd, we are dealing with TX FIFO
         address=USBN_TX_FIFOx[en];
      }
      else {   //if EPC is even, we are delaing with RX FIFO
         address=USBN_RX_FIFOx[en];
      }
      address+=2;
      usbn_write(address, 0x08);

      epc_to_endpoint[en]=0xFF;
   }
}


/// END USBN960x Functions only used in this File

#ENDIF
