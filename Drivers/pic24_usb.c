/////////////////////////////////////////////////////////////////////////
////                          pic24_usb.c                            ////
////                                                                 ////
//// Hardware layer for CCS's USB library.  See pic24_usb.h more     ////
//// documentation about the PIC24 hardware layer.                   ////
////                                                                 ////
//// This file is part of CCS's PIC USB driver code.  See USB.H      ////
//// for more documentation and a list of examples.                  ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// Version History:                                                ////
////                                                                 ////
//// July 23rd, 2012:                                                ////
////  Added __STALL_ON_UEP_ERRATA__                                  ////
////                                                                 ////
//// July 9th, 2012:                                                 ////
////  Removed the clearing of the UOWN bit inside token done ISR     ////
////     for non-setup packets on endpoint 0 or all other            ////
////     endpoints.  This really only affects people using           ////
////     usb_put_packet() or usb_get_packet() inside of other        ////
////     interrupts.                                                 ////
////                                                                 ////
//// May 9th, 2012:                                                  ////
////  Update to work with latest PCD compilers.                      ////
////                                                                 ////
//// Mar 7th, 2012:                                                  ////
////  Added 24EP/33EP family support.                                ////
////                                                                 ////
//// Oct 10th, 2010:                                                 ////
////  Added USB_ISR_POLLING support.  Define this and interrupts     ////
////     will not be used.  usb_task() must be called periodically   ////
////     in your main loop.  If it is not called faster than once    ////
////     per millisecond, USB may not work.                          ////
////                                                                 ////
//// Nov 13th, 2009:                                                 ////
////  usb_disable_endpoint() won't touch BD status registers for     ////
////     endpoints that aren't allocated.                            ////
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

#IFNDEF __PIC24_USB_C__
#DEFINE __PIC24_USB_C__

#INCLUDE <usb.h>
#include <stdint.h>

#define __STALL_ON_UEP_ERRATA__

#if getenv("BIT_VALID:USB1IE")
   #bit USBIE=getenv("BIT:USB1IE")
#else
   #bit USBIE=getenv("BIT:USBIE")
#endif

#if (getenv("SFR_VALID:U1BDTP2"))
   //newer style PIC24EP/dsPIC33EP chips
   #define __PIC24EP_USB__
#endif

#if USB_EP15_TX_SIZE || USB_EP15_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  15
#elif USB_EP14_TX_SIZE || USB_EP14_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  14
#elif USB_EP13_TX_SIZE || USB_EP13_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  13
#elif USB_EP12_TX_SIZE || USB_EP12_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  12
#elif USB_EP11_TX_SIZE || USB_EP11_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  11
#elif USB_EP10_TX_SIZE || USB_EP10_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  10
#elif USB_EP9_TX_SIZE || USB_EP9_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  9
#elif USB_EP8_TX_SIZE || USB_EP8_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  8
#elif USB_EP7_TX_SIZE || USB_EP7_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  7
#elif USB_EP6_TX_SIZE || USB_EP6_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  6
#elif USB_EP5_TX_SIZE || USB_EP5_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  5
#elif USB_EP4_TX_SIZE || USB_EP4_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  4
#elif USB_EP3_TX_SIZE || USB_EP3_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  3
#elif USB_EP2_TX_SIZE || USB_EP2_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  2
#elif USB_EP1_TX_SIZE || USB_EP1_RX_SIZE
 #define USB_LAST_DEFINED_ENDPOINT  1
#else
 #define USB_LAST_DEFINED_ENDPOINT  0
#endif

#define USB_CONTROL_REGISTER_SIZE   ((USB_LAST_DEFINED_ENDPOINT+1)*8)

#define USB_DATA_BUFFER_NEEDED (USB_EP0_TX_SIZE+USB_EP0_RX_SIZE+USB_EP1_TX_SIZE+\
                           USB_EP1_RX_SIZE+USB_EP2_TX_SIZE+USB_EP2_RX_SIZE+\
                           USB_EP3_TX_SIZE+USB_EP3_RX_SIZE+USB_EP4_TX_SIZE+\
                           USB_EP4_RX_SIZE+USB_EP5_TX_SIZE+USB_EP5_RX_SIZE+\
                           USB_EP6_TX_SIZE+USB_EP6_RX_SIZE+USB_EP7_TX_SIZE+\
                           USB_EP7_RX_SIZE+USB_EP8_TX_SIZE+USB_EP8_RX_SIZE+\
                           USB_EP9_TX_SIZE+USB_EP9_RX_SIZE+USB_EP10_TX_SIZE+\
                           USB_EP10_RX_SIZE+USB_EP11_TX_SIZE+USB_EP11_RX_SIZE+\
                           USB_EP12_TX_SIZE+USB_EP12_RX_SIZE+USB_EP13_TX_SIZE+\
                           USB_EP13_RX_SIZE+USB_EP14_TX_SIZE+USB_EP14_RX_SIZE+\
                           USB_EP15_TX_SIZE+USB_EP15_RX_SIZE)

#if ((USB_DATA_BUFFER_NEEDED+USB_CONTROL_REGISTER_SIZE) > getenv("RAM"))
 #error You are trying to allocate more memory for endpoints than the PIC can handle
#endif

#if defined(__PIC24EP_USB__)
#define __PIC24_NINC_BIT__ 0
typedef union
{
   struct
   {
      unsigned int8 :2;  //reserved bits 0 and 1 of word 1
      unsigned int8 pid:4;
   };
   struct
   {
      unsigned int8 :2;     //reserved bits 0 and 1 of word 1
      unsigned int8 bstall:1;
      unsigned int8 dtsen:1;
      unsigned int8 ninc:1;
      unsigned int8 keep:1;
      unsigned int8 data01:1;
      unsigned int8 uown:1;
      unsigned int8: 8; //reserved bits 8 to 15 of word1
      unsigned int16 count;
   };
} usb_bd_stat_t;
typedef unsigned int32 usb_bd_ptr_t;
#else
#if defined(__PIC24_NINC_BIT__)
#undef __PIC24_NINC_BIT__
#endif
#define __PIC24_NINC_BIT__ 0
//pic24f
typedef union
{
   struct
   {
      unsigned int16 count:10;
      unsigned int8 bstall:1;
      unsigned int8 dtsen:1;
      unsigned int8 :2;
      unsigned int8 data01:1;
      unsigned int8 uown:1;
   };
   struct
   {
      unsigned int16 :10;  //reserved bits 0 and 1 of word 1
      unsigned int8 pid:4;
   };
} usb_bd_stat_t;
typedef unsigned int16 usb_bd_ptr_t;
#endif


/*
   Options that USB_PING_PONG_MODE can be set to.  It must be set
   to one of these choices.

   If you use USB_PING_PONG_MODE_ON, you can also define USB_PING_PONG_EVEN_ONLY
   where BDT is allocated for full ping-pong buffering but the peripheral
   only uses the EVEN buffers.
*/
#define USB_PING_PONG_MODE_OFF         0     //no ping pong.  Not supported on EP chips.
#define USB_PING_PONG_MODE_E0_RX       1     //ping pong endpoint 0 RX (OUT) only.  Not supported on EP chips.
#define USB_PING_PONG_MODE_ON          2     //ping pong all endpoints
#define USB_PING_PONG_MODE_ON_BUT_E0   3     //ping pong on all endpoints but E0.  Not supported on EP chips.

/*
   NOTE - PING PONG MODE IS NOT SUPPORTED BY CCS!

   I would not mess with this as many things may break.

   On PIC24EP/dsPIC33EP chips, ping pong is enabled but always using the
   EVEN buffers.  You cannot truly disable ping-pong buffering on these
   chips.
*/
#if !defined(USB_PING_PONG_MODE)
   #if defined(__PIC24EP_USB__)
      #define USB_PING_PONG_MODE USB_PING_PONG_MODE_ON
      #define USB_PING_PONG_EVEN_ONLY
   #else
      #define USB_PING_PONG_MODE USB_PING_PONG_MODE_OFF
   #endif
#endif

#if ((USB_PING_PONG_MODE==USB_PING_PONG_MODE_E0_RX)||(USB_PING_PONG_MODE==USB_PING_PONG_MODE_ON))
   #define USB_EP0_RX_BUFFERS 2
#else
   #define USB_EP0_RX_BUFFERS 1
#endif

#if (USB_PING_PONG_MODE==USB_PING_PONG_MODE_ON)
   #define USB_EP0_TX_BUFFERS 2
#else
   #define USB_EP0_TX_BUFFERS 1
#endif

#if ((USB_PING_PONG_MODE==USB_PING_PONG_MODE_ON)||(USB_PING_PONG_MODE==USB_PING_PONG_MODE_ON_BUT_E0))
   #define USB_EPN_TXRX_BUFFERS 2
#else
   #define USB_EPN_TXRX_BUFFERS 1
#endif

typedef struct
{
   usb_bd_stat_t stat;
   usb_bd_ptr_t ptr;
} usb_pd_buffer_t;

struct
{
   usb_pd_buffer_t ep0out[USB_EP0_RX_BUFFERS];  //pic rx
   usb_pd_buffer_t ep0in[USB_EP0_TX_BUFFERS];   //pic tx
  #if (USB_LAST_DEFINED_ENDPOINT != 0)
   usb_pd_buffer_t epn[USB_LAST_DEFINED_ENDPOINT][2][USB_EPN_TXRX_BUFFERS];
  #endif
} g_UsbBdt;
#locate g_UsbBdt=0x1200

//dir 0 = rx(out), 1 = tx(in)
uint8_t * UsbEpBdXGetStPtr(uint8_t ep, int1 dir)
{
   uint8_t *ptr;

   // only supports no ping pong or even only ping pong

   if (ep == 0)
   {
      if (dir)
         ptr = &g_UsbBdt.ep0in[0].stat;
      else
         ptr = &g_UsbBdt.ep0out[0].stat;
   }
   else
   {
      ptr = &g_UsbBdt.epn[ep-1][dir][0].stat;
   }

   return(ptr);
}

void UsbEpBdXGetSt(uint8_t ep, int1 dir, usb_bd_stat_t *pDst)
{
   usb_bd_stat_t *pSrc;

   pSrc = UsbEpBdXGetStPtr(ep, dir);

  //#if defined(__PIC24EP_USB__)
  #if 0
   memcpy(pDst, pSrc, sizeof(usb_bd_stat_t));
  #else
   *pDst = *pSrc;
  #endif
}

void UsbEpBdXSetSt(uint8_t ep, int1 dir, usb_bd_stat_t *pSrc)
{
   usb_bd_stat_t *pDst;

   pDst = UsbEpBdXGetStPtr(ep, dir);

  //#if defined(__PIC24EP_USB__)
  #if 0
   memcpy(pDst, pSrc, sizeof(usb_bd_stat_t));
  #else
   *pDst = *pSrc;
  #endif
}

//dir 0 = rx(out), 1 = tx(in)
uint8_t * UsbEpBdXGetPtrPtr(uint8_t ep, int1 dir)
{
   uint8_t *ptr;

   ptr = UsbEpBdXGetStPtr(ep, dir);

   ptr += sizeof(usb_bd_stat_t);

   return(ptr);
}

void UsbEpBdXGetPtr(uint8_t ep, int1 dir, usb_bd_ptr_t *pDst)
{
   usb_bd_ptr_t *pSrc;

   pSrc = UsbEpBdXGetPtrPtr(ep, dir);

   memcpy(pDst, pSrc, sizeof(usb_bd_ptr_t));
}

void UsbEpBdXSetPtr(uint8_t ep, int1 dir, usb_bd_ptr_t *pSrc)
{
   usb_bd_ptr_t *pDst;

   pDst = UsbEpBdXGetPtrPtr(ep, dir);

   memcpy(pDst, pSrc, sizeof(usb_bd_ptr_t));
}

#if defined(__DO_DEBUG_USB)
usb_bd_stat_t EP_BDxST_I(uint8_t ep)
{
   usb_bd_stat_t ret;

   UsbEpBdXGetSt(ep, 1, &ret);

   return(ret);
}
usb_bd_stat_t EP_BDxST_O(uint8_t ep)
{
   usb_bd_stat_t ret;

   UsbEpBdXGetSt(ep, 0, &ret);

   return(ret);
}

usb_bd_ptr_t EP_BDxADR_I(uint8_t ep)
{
   usb_bd_ptr_t ret;

   UsbEpBdXGetPtr(ep, 1, &ret);

   return(ret);
}

usb_bd_ptr_t EP_BDxADR_O(uint8_t ep)
{
   usb_bd_ptr_t ret;

   UsbEpBdXGetPtr(ep, 0, &ret);

   return(ret);
}
#endif

union
{
   struct
   {
      unsigned int8 ep0_rx_buffer[USB_MAX_EP0_PACKET_LENGTH];
      unsigned int8 ep0_tx_buffer[USB_MAX_EP0_PACKET_LENGTH];

      //these buffer definitions needed for CDC library
     #if USB_EP1_RX_SIZE
      unsigned int8 ep1_rx_buffer[USB_EP1_RX_SIZE];
     #endif
     #if USB_EP1_TX_SIZE
      unsigned int8 ep1_tx_buffer[USB_EP1_TX_SIZE];
     #endif
     #if USB_EP2_RX_SIZE
      unsigned int8 ep2_rx_buffer[USB_EP2_RX_SIZE];
     #endif
     #if USB_EP2_TX_SIZE
      unsigned int8 ep2_tx_buffer[USB_EP2_TX_SIZE];
     #endif
   };
   unsigned int8 general[USB_DATA_BUFFER_NEEDED];
} g_USBDataBuffer;
#if defined(__DO_DEBUG_USB)
#define USB_DATA_BUFFER_LOCATION 0x1400
#locate g_USBDataBuffer=USB_DATA_BUFFER_LOCATION
#warning why not do a locate and change some debugs to actually look at this locate, incase pointers got corrupted (USB_MAX_EP0_PACKET_LENGTH, rx, tx)
#else
#define USB_DATA_BUFFER_LOCATION &g_USBDataBuffer.general[0]
#endif

#define usb_ep0_rx_buffer  g_USBDataBuffer.ep0_rx_buffer
#define usb_ep0_tx_buffer  g_USBDataBuffer.ep0_tx_buffer

//these buffer definitions needed for CDC library
#define usb_ep1_rx_buffer g_USBDataBuffer.ep1_rx_buffer
#define usb_ep1_tx_buffer g_USBDataBuffer.ep1_tx_buffer
#define usb_ep2_rx_buffer g_USBDataBuffer.ep2_rx_buffer
#define usb_ep2_tx_buffer g_USBDataBuffer.ep2_tx_buffer

//if you are worried that the PIC is not receiving packets because a bug in the
//DATA0/DATA1 synch code, you can set this to TRUE to ignore the DTS on
//receiving.
#ifndef USB_IGNORE_RX_DTS
 #define USB_IGNORE_RX_DTS FALSE
#endif

#ifndef USB_IGNORE_TX_DTS
 #define USB_IGNORE_TX_DTS FALSE
#endif

//if you enable this it will keep a counter of the 6 possible errors the
//pic can detect.  disabling this will save you ROM, RAM and execution time.
#if !defined(USB_USE_ERROR_COUNTER)
   #define USB_USE_ERROR_COUNTER FALSE
#endif

#if USB_USE_ERROR_COUNTER
   unsigned int ERROR_COUNTER[6];
#endif

//---pic24 memory locations
#word UOTGIR  =  getenv("SFR:U1OTGIR")
#word UOTGIE  =  getenv("SFR:U1OTGIE")
#word UOTGCON =  getenv("SFR:U1OTGCON")
#word UPWRC   =  getenv("SFR:U1PWRC")
#word UIR     =  getenv("SFR:U1IR")
#word UIE     =  getenv("SFR:U1IE")
#word UEIR    =  getenv("SFR:U1EIR")
#word UEIE    =  getenv("SFR:U1EIE")
#word USTAT   =  getenv("SFR:U1STAT")
#word UCON    =  getenv("SFR:U1CON")
#word UADDR   =  getenv("SFR:U1ADDR")
#word UBDTP1  =  getenv("SFR:U1BDTP1")
#if defined(__PIC24EP_USB__)
#word UBDTP2  =  getenv("SFR:U1BDTP2")
#word UBDTP3  =  getenv("SFR:U1BDTP3")
#endif
#word UFRML   =  getenv("SFR:U1FRML")
#word UFRMH   =  getenv("SFR:U1FRMH")
#word UCFG1   =  getenv("SFR:U1CNFG1")
#word UCFG2   =  getenv("SFR:U1CNFG2")
#define  UEP0_LOC getenv("SFR:U1EP0")

unsigned int16 UEP_SFR[16];
#locate UEP_SFR=UEP0_LOC

#define UEP(x) UEP_SFR[x]

#define UIE_BIT_URST    0
#define UIE_BIT_UERR    1
#define UIE_BIT_SOF     2
#define UIE_BIT_TRN     3
#define UIE_BIT_IDLE    4
#define UIE_BIT_STALL   7

#BIT UIE_SOF = UIE.UIE_BIT_SOF
#BIT UIE_STALL = UIE.UIE_BIT_STALL
#BIT UIE_IDLE = UIE.UIE_BIT_IDLE
#BIT UIE_TRN = UIE.UIE_BIT_TRN
#BIT UIE_UERR = UIE.UIE_BIT_UERR
#BIT UIE_URST = UIE.UIE_BIT_URST

#define UOTGIE_BIT_ACTV 4

#BIT UOTGIE_ACTV=UOTGIE.UOTGIE_BIT_ACTV

#BIT UIR_SOF = UIR.UIE_BIT_SOF
#BIT UIR_STALL = UIR.UIE_BIT_STALL
#BIT UIR_IDLE = UIR.UIE_BIT_IDLE
#BIT UIR_TRN = UIR.UIE_BIT_TRN
#BIT UIR_UERR = UIR.UIE_BIT_UERR
#BIT UIR_URST = UIR.UIE_BIT_URST

#BIT UOTGIR_ACTV=UOTGIR.UOTGIE_BIT_ACTV

#ifndef BIT
#define BIT(x) (1<<x)
#endif

#define __USB_UIF_RESET    BIT(UIE_BIT_URST)
#define __USB_UIF_ERROR    BIT(UIE_BIT_UERR)
#define __USB_UIF_TOKEN    BIT(UIE_BIT_TRN)
#define __USB_UIF_IDLE     BIT(UIE_BIT_IDLE)
#define __USB_UIF_STALL    BIT(UIE_BIT_STALL)
#define __USB_UIF_SOF      BIT(UIE_BIT_SOF)

#if USB_USE_ERROR_COUNTER
 #define STANDARD_INTS __USB_UIF_STALL|__USB_UIF_IDLE|__USB_UIF_TOKEN|__USB_UIF_ERROR|__USB_UIF_RESET
#else
 #define STANDARD_INTS __USB_UIF_STALL|__USB_UIF_IDLE|__USB_UIF_TOKEN|__USB_UIF_RESET
#endif

#bit UOTGCON_DPPULUP = UOTGCON.7
#bit UOTGCON_DPMULUP = UOTGCON.6

#bit UCON_SE0=UCON.6
#bit UCON_PKTDIS=UCON.5
#bit UCON_USBEN=UCON.0
#bit UCON_RESUME=UCON.2
#bit UCON_PPBRST=UCON.1

#bit UPWRC_USBPWR=UPWRC.0
#bit UPWRC_SUSPND=UPWRC.1
#define UCON_SUSPND UPWRC_SUSPND

//See UEPn (0xF70-0xF7F)
#define ENDPT_DISABLED   0x00   //endpoint not used
#define ENDPT_IN_ONLY   0x04    //endpoint supports IN transactions only
#define ENDPT_OUT_ONLY   0x08    //endpoint supports OUT transactions only
#define ENDPT_CONTROL   0x0C    //Supports IN, OUT and CONTROL transactions - Only use with EP0
#define ENDPT_NON_CONTROL 0x1C  //Supports both IN and OUT transactions

#define __UEP_EPHSK  0x01

//Define the states that the USB interface can be in
enum {USB_STATE_DETACHED=0, USB_STATE_ATTACHED=1, USB_STATE_POWERED=2, USB_STATE_DEFAULT=3,
    USB_STATE_ADDRESS=4, USB_STATE_CONFIGURED=5} usb_state=0;

#define USTAT_IN_E0        8
#define USTAT_OUT_SETUP_E0 0

#define __USB_UCFG1_UTEYE   0x80
#define __USB_UCFG1_UOEMON  0x40

/* wrong
#define __USB_UCFG2_PUVBUS  0x10 //microchip calls this USB_PULLUP_ENABLE
#define __USB_UCFG2_UTRDIS  0x01 //microchip calls this USB_EXTERNAL_TRANSCEIVER
#define __UCFG2_VAL_ENABLED__ __USB_UCFG2_PUVBUS
*/
#define __UCFG2_VAL_ENABLED__ 0

//#define __UCFG1_VAL_ENABLED__ USB_PING_PONG_MODE

#if defined(__PIC24EP_USB__)
   //newer pic24EP/dspic33EP chips
   #define __UCFG1_VAL_ENABLED__ 0
#else
   //older pic24/dsPIC33 chips
   #define __UCFG1_VAL_ENABLED__ USB_PING_PONG_MODE
#endif

#define __UCFG1_VAL_DISABLED__   0
#define __UCFG2_VAL_DISABLED__ 0

#if defined(USB_EXTERNAL_PULLUPS) && defined(USB_INTERNAL_PULLUPS)
 #error Can not use both internal and external pullups at the same time!
#endif

#if !defined(USB_EXTERNAL_PULLUPS) && !defined(USB_INTERNAL_PULLUPS)
 #define USB_INTERNAL_PULLUPS
#endif

unsigned int8 __setup_0_tx_size;

//interrupt handler, specific to PIC24 peripheral only
void usb_handle_interrupt(void);
void usb_isr_rst(void);
void usb_isr_uerr(void);
void usb_isr_sof(void);
void usb_isr_activity(void);
void usb_isr_uidle(void);
void usb_isr_tok_dne(void);
void usb_isr_stall(void);
void usb_init_ep0_setup(void);

#if defined(USB_ISR_POLLING)
void usb_isr(void);
#endif


//following functions standard part of CCS PIC USB driver, and used by usb.c
unsigned int16 usb_get_packet_buffer(unsigned int8 endpoint, unsigned int8 *ptr, unsigned int16 max);

static void usb_clear_trn(void);

static void usb_clear_isr_flag(unsigned int16 *sfr, unsigned int8 bit);
static void usb_clear_isr_reg(unsigned int16 *sfr);

//// BEGIN User Functions:

// see usb_hw_layer.h for documentation
int1 usb_kbhit(unsigned int8 en)
{
   usb_bd_stat_t stat;
   UsbEpBdXGetSt(en, 0, &stat);
   //return((UEP(en) != ENDPT_DISABLED) && ((EP_BDxST_O(en) & __BDST_UOWN) == 0));
   return((UEP(en) != ENDPT_DISABLED) && !stat.uown);
}

// see usb_hw_layer.h for documentation
int1 usb_tbe(unsigned int8 en)
{
   usb_bd_stat_t stat;
   UsbEpBdXGetSt(en, 1, &stat);
   //return((UEP(en)!=ENDPT_DISABLED) && ((EP_BDxST_I(en) & __BDST_UOWN) == 0));
   return((UEP(en)!=ENDPT_DISABLED) && !stat.uown);
}

// see usb_hw_layer.h for documentation
void usb_detach(void)
{
   UCON = 0;  //disable USB hardware
   UIE = 0;   //disable USB interrupts
   usb_clear_isr_reg(&UEIR);
   usb_clear_isr_reg(&UIR);
   UOTGIE = 0;
   UPWRC_USBPWR = 1;
   UCFG1 = __UCFG1_VAL_DISABLED__;
   UCFG2 = __UCFG2_VAL_DISABLED__;
   UCON_PPBRST = 1;

   //set_tris_g(get_tris_g() | 0x0C); //set d+/d- pins to input //pin_g2 and pin_g3
   usb_state=USB_STATE_DETACHED;
   UADDR = 0;

   memset(&UEP_SFR[0], 0x00, sizeof(UEP_SFR));

   memset(&g_UsbBdt, 0x00, sizeof(g_UsbBdt));

   UBDTP1 = &g_UsbBdt >> 8;
   debug_usb(debug_putc, "\r\nDETACH p=%02X", UBDTP1);
  #if defined(__PIC24EP_USB__)
   //newer 24ep/33ep chips
   UBDTP2 = &g_UsbBdt >> 16;
   UBDTP3 = &g_UsbBdt >> 24;
   debug_usb(debug_putc, "%02X%02X", UBDTP2, UBDTP3);
  #endif

  #if !defined(USB_PING_PONG_EVEN_ONLY)
   UCON_PPBRST = 0;
  #endif

   debug_usb(debug_putc, " %LX ", &g_UsbBdt);


   usb_token_reset();              //clear the chapter9 stack
   UCON_PKTDIS=0;
   //__usb_kbhit_status=0;

   /*
   UCON = 0;  //disable USB hardware
   UIE = 0;   //disable USB interrupts
   usb_state = USB_STATE_DETACHED;
   */
   UOTGCON_DPPULUP = 0;
   UOTGCON_DPMULUP = 0;
  #if defined(USB_INTERNAL_PULLUPS)
   #if USB_USE_FULL_SPEED
    UOTGCON_DPPULUP = 1;
   #else
    UOTGCON_DPMULUP = 1;
   #endif
  #endif
}

// see usb_hw_layer.h for documentation
void usb_init_cs(void)
{
   usb_detach();
}

// see usb_hw_layer.h for documentation
static void usb_attach(void)
{
   usb_token_reset();

   UCON = 0;
  #if defined(USB_PING_PONG_EVEN_ONLY)
   UCON_PPBRST = 1;
  #endif
   UIE = 0;                                // Mask all USB interrupts
   UCFG1 = __UCFG1_VAL_ENABLED__;
   UCFG2 = __UCFG2_VAL_ENABLED__;

   while (!UCON_USBEN) {UCON_USBEN = 1;}  // Enable module & attach to bus

   usb_state = USB_STATE_ATTACHED;      // Defined in usbmmap.c & .h
}

// see usb_hw_layer.h for documentation
void usb_task(void) {
  #if defined(USB_ISR_POLLING)
   if (interrupt_active(INT_USB))
   {
      usb_isr();
   }
  #endif

  #if defined(USB_CDC_DELAYED_FLUSH)
   if (usb_enumerated())
   {
      usb_cdc_flush_out_buffer();
   }
  #endif

   if (usb_attached()) {
      if (UCON_USBEN==0) {
         debug_usb(debug_putc, "\r\n\nUSB TASK: ATTACH");
         usb_attach();
         //delay_ms(50);
      }
   }
   else {
      if (UCON_USBEN==1)  {
         debug_usb(debug_putc, "\r\n\nUSB TASK: DE-ATTACH");
         usb_detach();
      }
   }

   if ((usb_state == USB_STATE_ATTACHED)&&(!UCON_SE0))
   {
      usb_clear_isr_reg(&UIR);
      UIE=0;
     #if !defined(USB_ISR_POLLING)
      enable_interrupts(INT_USB);
      enable_interrupts(INTR_GLOBAL);
     #endif
      UIE=__USB_UIF_IDLE | __USB_UIF_RESET;  //enable IDLE and RESET USB interrupt
      usb_state=USB_STATE_POWERED;
      debug_usb(debug_putc, "\r\n\nUSB TASK: POWERED");
   }
}

// see usb_hw_layer.h for documentation
void usb_init(void)
{
   usb_init_cs();

   do
   {
      usb_task();
   } while (usb_state != USB_STATE_POWERED);
}

// see pic24_usb.h for documentation
int1 usb_flush_in(unsigned int8 endpoint, unsigned int16 len, USB_DTS_BIT tgl)
{
   usb_bd_stat_t newBD, oldBD;

   memset(&newBD, 0x00, sizeof(newBD));

   debug_usb(debug_putc, "\r\nPUT e%U t%U l%LU s%08LX a%08LX uc%02X ", endpoint, tgl, len, (unsigned int32)EP_BDxST_I(endpoint), (unsigned int32)EP_BDxADR_I(endpoint), UCON);

   if (usb_tbe(endpoint))
   {
      newBD.count = len;

     #if defined(__DO_DEBUG_USB)
      debug_display_ram(len, USB_DATA_BUFFER_LOCATION + USB_MAX_EP0_PACKET_LENGTH);
     #else
      debug_display_ram(len, EP_BDxADR_I(endpoint));
     #endif

     #if USB_IGNORE_TX_DTS
      newBD.uown = TRUE;
     #else
      if (tgl == USB_DTS_TOGGLE)
      {
         UsbEpBdXGetSt(endpoint, 1, &oldBD);
         tgl = oldBD.data01;
         tgl = !tgl;
         debug_usb(debug_putc, "dT ");
      }
      else if (tgl == USB_DTS_USERX)
      {
         UsbEpBdXGetSt(endpoint, 0, &oldBD);
         tgl = oldBD.data01;
         debug_usb(debug_putc, "dR%us%08lxe%u ", tgl, (unsigned int32)EP_BDxST_O(endpoint), endpoint);
      }

      debug_usb(debug_putc, "d%U ", tgl);

      newBD.data01 = tgl;
      newBD.dtsen = TRUE;
      newBD.uown = TRUE;
     #endif

     #if __PIC24_NINC_BIT__
      newBD.ninc = __PIC24_NINC_BIT__;
     #endif

      UsbEpBdXSetSt(endpoint, 1, &newBD);

      debug_usb(debug_putc, " %08LX %U %08LX ", (unsigned int32)newBD, endpoint, (unsigned int32)EP_BDxST_I(endpoint));

      //debug_display_ram(16, (void*)UBDTP1 << 8);
      debug_display_ram(16, 0x1200);

      return(1);
   }
    else
    {
         debug_usb(debug_putc,"\r\nPUT_ERR1 ");
         debug_display_ram(16, (void*)UBDTP1 << 8);
    }
   return(0);
}

// see usb_hw_layer.h for documentation.
int1 usb_put_packet(unsigned int8 endpoint, unsigned int8 * ptr, unsigned int16 len, USB_DTS_BIT tgl)
{
   usb_bd_ptr_t bdPtr;

   if (usb_tbe(endpoint))
   {
      UsbEpBdXGetPtr(endpoint, 1, &bdPtr);
      memcpy(bdPtr, ptr, len);

      return(usb_flush_in(endpoint, len, tgl));
   }
   else
   {
      debug_usb(debug_putc,"\r\nPUT_ERR2 %U %02X ", endpoint, UEP(endpoint));
      debug_display_ram(16, (void*)UBDTP1 << 8);
     #if defined(__DO_DEBUG_USB)
      if (endpoint == 0)
      {
         for(len=0;len<250;len++)
         {
            delay_ms(1);
            uart_task();
         }
      }
     #endif
   }

   return(0);
}

/// END User Functions


/// BEGIN Hardware layer functions required by USB.C

// see pic24_usb.h for documentation
void usb_flush_out(unsigned int8 endpoint, USB_DTS_BIT tgl)
{
   usb_bd_stat_t newBD, oldBD;

   memset(&newBD, 0x00, sizeof(newBD));

  #if __PIC24_NINC_BIT__
   newBD.ninc = __PIC24_NINC_BIT__;
  #endif

     #if USB_IGNORE_RX_DTS
      if (tgl == USB_DTS_STALL)
      {
         debug_usb(debug_putc, '*');

         newBD.uown = TRUE;
         newBD.bstall = TRUE;

         UsbEpBdXSetSt(endpoint, 1, &newBD);
         UsbEpBdXSetSt(endpoint, 0, &newBD);
         return;
      }
      else
      {
         newBD.uown = TRUE;
      }
     #else
      //oldBD = g_UsbBdt.ep[endpoint].out;
      if (tgl == USB_DTS_TOGGLE)
      {
         UsbEpBdXGetSt(endpoint, 0, &oldBD);
         tgl = oldBD.data01;
         tgl = !tgl;
      }
      if (tgl == USB_DTS_STALL)
      {
         newBD.uown = TRUE;
         newBD.bstall = TRUE;

         UsbEpBdXSetSt(endpoint, 1, &newBD);
      }
      else if (tgl == USB_DTS_DATA1)
      {
         newBD.uown = TRUE;
         newBD.data01 = TRUE;
         newBD.dtsen = TRUE;
      }
      else //if (tgl == USB_DTS_DATA0)
      {
         newBD.uown = TRUE;
         newBD.data01 = FALSE;
         newBD.dtsen = TRUE;
      }
     #endif

   //bit_clear(__usb_kbhit_status,endpoint);

   newBD.count = usb_ep_rx_size[endpoint];

   UsbEpBdXSetSt(endpoint, 0, &newBD);
}

// see pic24_usb.h for documentation
unsigned int16 usb_rx_packet_size(unsigned int8 endpoint)
{
   usb_bd_stat_t stat;
   UsbEpBdXGetSt(endpoint, 0, &stat);
   return(stat.count);
   //return(EP_BDxST_O(endpoint) & 0x03FF);
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
unsigned int16 usb_get_packet_buffer(unsigned int8 endpoint, unsigned int8 *ptr, unsigned int16 max)
{
   unsigned int16 i;
   usb_bd_ptr_t bdPtr;

   i = usb_rx_packet_size(endpoint);

   //printf("\r\nUSB GET PACKET ST:%LX %U I=%LX MAX=%LX %X%X ", EP_BDxST_O(endpoint), endpoint, i, max, al[0], al[1]);

   if (i<max) {max=i;}

   if (max)
   {
      UsbEpBdXGetPtr(endpoint, 0, &bdPtr);
      memcpy(ptr, bdPtr, max);
   }

   return(max);
}

// see usb_hw_layer.h
unsigned int16 usb_get_packet(unsigned int8 endpoint, unsigned int8 * ptr, unsigned int16 max)
{
   max=usb_get_packet_buffer(endpoint,ptr,max);
   usb_flush_out(endpoint, USB_DTS_TOGGLE);

   return(max);
}

// see usb_hw_layer.h
void usb_stall_ep(unsigned int8 endpoint)
{
   usb_bd_stat_t newBD;
   int1 direction;

   memset(&newBD, 0x00, sizeof(newBD));

   newBD.uown = TRUE;
   newBD.bstall = TRUE;

  #if __PIC24_NINC_BIT__
   newBD.ninc = __PIC24_NINC_BIT__;
  #endif

   direction = bit_test(endpoint,7);
   endpoint &= 0x7F;

   UsbEpBdXSetSt(endpoint, direction, &newBD);
}

// see usb_hw_layer.h for documentation
void usb_unstall_ep(unsigned int8 endpoint)
{
   usb_bd_stat_t newBD;
   int1 direction;

   direction = bit_test(endpoint,7);
   endpoint &= 0x7F;

   memset(&newBD, 0x00, sizeof(newBD));

   if (direction)
   {
      newBD.uown = TRUE;
     #if !USB_IGNORE_RX_DTS
      newBD.dtsen = TRUE;
     #endif

     #if __PIC24_NINC_BIT__
      newBD.ninc = __PIC24_NINC_BIT__;
     #endif

      UsbEpBdXSetSt(endpoint, 1, &newBD);
   }
   else
   {
      UsbEpBdXSetSt(endpoint, 0, &newBD);
   }

  #if defined(__STALL_ON_UEP_ERRATA__)
   bit_clear(UEP(endpoint), 1);
  #endif
}

// see usb_hw_layer.h for documentation
int1 usb_endpoint_stalled(unsigned int8 endpoint)
{
   int1 direction;
   usb_bd_stat_t bd;

   direction = bit_test(endpoint,7);
   endpoint &= 0x7F;

   UsbEpBdXGetSt(endpoint, direction, &bd);

   return(bd.uown && bd.bstall);
}

// see usb_hw_layer.h for documentation
void usb_set_address(unsigned int8 address)
{
   UADDR = address;

   if (address)
      usb_state = USB_STATE_ADDRESS;
   else
      usb_state = USB_STATE_POWERED;
}

// see usb_hw_layer.h for documentation
void usb_set_configured(unsigned int8 config)
{
   unsigned int8 en;
   usb_bd_ptr_t addy;
   unsigned int16 new_uep;
   unsigned int16 len;
   usb_bd_stat_t newBD;

   if (config == 0)
   {  //if config=0 then set addressed state
      usb_state = USB_STATE_ADDRESS;
      usb_disable_endpoints();
   }
   else
   {
      usb_state = USB_STATE_CONFIGURED; //else set configed state
      addy = USB_DATA_BUFFER_LOCATION + (2*USB_MAX_EP0_PACKET_LENGTH); //skip ep0
      for (en=1; en<16; en++)
      {
         usb_disable_endpoint(en);
         new_uep = 0;
         if (usb_ep_rx_type[en] != USB_ENABLE_DISABLED)
         {
            memset(&newBD, 0x00, sizeof(newBD));

            new_uep = ENDPT_OUT_ONLY;

            len = usb_ep_rx_size[en];

            UsbEpBdXSetPtr(en, 0, &addy);

            addy += len;

            newBD.uown = TRUE;
            #if !USB_IGNORE_RX_DTS
               newBD.dtsen = TRUE;
            #endif


           #if __PIC24_NINC_BIT__
            newBD.ninc = __PIC24_NINC_BIT__;
           #endif

            newBD.count = len;

            UsbEpBdXSetSt(en, 0, &newBD);
         }
         if (usb_ep_tx_type[en] != USB_ENABLE_DISABLED)
         {
            memset(&newBD, 0x00, sizeof(newBD));

           #if __PIC24_NINC_BIT__
            newBD.ninc = __PIC24_NINC_BIT__;
           #endif

            new_uep |= ENDPT_IN_ONLY;

            UsbEpBdXSetPtr(en, 1, &addy);
            addy += usb_ep_tx_size[en];

            newBD.data01 = TRUE;

            UsbEpBdXSetSt(en, 1, &newBD);
         }

         if (new_uep == (ENDPT_IN_ONLY | ENDPT_OUT_ONLY))
            new_uep = ENDPT_NON_CONTROL;

         if (usb_ep_tx_type[en] != USB_ENABLE_ISOCHRONOUS)
            new_uep |= __UEP_EPHSK;

         UEP(en) = new_uep;
      }
   }
}

/// END Hardware layer functions required by USB.C


/// BEGIN USB Interrupt Service Routine

static void usb_clear_trn(void)
{
   usb_clear_isr_flag(&UIR, UIE_BIT_TRN);
   delay_cycles(6);
}

/*******************************************************************************
/* usb_handle_interrupt()
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
#if !defined(USB_ISR_POLLING)
#int_usb
#endif
void usb_isr(void)
{
   unsigned int8 TRNAttempts = 0;

  #if !defined(USB_ISR_POLLING)
   debug_putc('.');
  #endif

   if (usb_state==USB_STATE_DETACHED) return;   //should never happen, though

  #if defined(USB_ISR_POLLING)
   if (((UIR&UIE)!= 0) || ((UOTGIR&UOTGIE)!=0))
  #else
   if (UIR || UOTGIR)
  #endif
   {
     #if defined(USB_ISR_POLLING)
      debug_putc('.');
      #if 0
      printf(debug_putc, "%02x%02x ", UIR&UIE, UOTGIR&UOTGIE);
      #endif
     #endif

      //debug_usb(debug_putc,"\r\n\n[%X%X%U] ",(int8)UIR, (int8)UOTGIR, UCON_SUSPND);
      if (UOTGIR_ACTV && UOTGIE_ACTV) {usb_isr_activity();}  //activity detected.  (only enable after sleep)

      if (UCON_SUSPND) return;

      if (UIR_STALL && UIE_STALL) {usb_isr_stall();}        //a stall handshake was sent

      if (UIR_UERR && UIE_UERR) {usb_isr_uerr();}          //error has been detected

      if (UIR_URST && UIE_URST) {usb_isr_rst();}        //usb reset has been detected

      if (UIR_IDLE && UIE_IDLE) {usb_isr_uidle();}        //idle time, we can go to sleep
      if (UIR_SOF && UIE_SOF) {usb_isr_sof();}
      //if (UIR_STALL && UIE_STALL) {usb_isr_stall();}        //a stall handshake was sent

      do
      {
         if (UIR_TRN && UIE_TRN) {
            usb_isr_tok_dne();
            //UIR_TRN=0;    // clear the token done interrupt., 0x190.3
         }    //a token has been detected (majority of isrs)
         else
            break;
      }
      while (TRNAttempts++ < 4);
     #if defined(USB_CDC_ISR)
      //has to be done here, can't be done until TRN is empty.
      if (usb_cdc_kbhit())
      {
         USB_CDC_ISR();
      }
     #endif
   }
}

//SOF interrupt not handled.  user must add this depending on application
void usb_isr_sof(void)
{
   debug_usb(debug_putc,"\r\nSOF");
   usb_clear_isr_flag(&UIR, UIE_BIT_SOF);
}

// see usb_hw_layer.h for documentation
void usb_disable_endpoint(unsigned int8 en)
{
   usb_bd_stat_t stat;

   UEP(en)=ENDPT_DISABLED;

   if (usb_endpoint_is_valid(en))
   {
      //better ping-pong support needed here
      //clear state, deque if necessary
      memset(&stat, 0, sizeof(usb_bd_stat_t));
      UsbEpBdXSetSt(en, 0, &stat);
      UsbEpBdXSetSt(en, 1, &stat);
   }
}

// see usb_hw_layer.h for documentation
void usb_disable_endpoints(void)
{
   unsigned int8 i;

   for (i=1;i<16;i++)
      usb_disable_endpoint(i);

   //__usb_kbhit_status=0;
}

/*******************************************************************************
/* usb_isr_rst()
/*
/* Summary: The host (computer) sent us a RESET command.  Reset USB device
/*          and token handler code to initial state.
/*
/********************************************************************************/
void usb_isr_rst(void) {
   debug_usb(debug_putc,"R");

   usb_clear_isr_reg(&UEIR);
   usb_clear_isr_reg(&UIR);
   UEIE = 0x9F;
   UIE = STANDARD_INTS;

   UADDR=0;

   usb_token_reset();

   usb_disable_endpoints();

   UEP(0) = ENDPT_CONTROL | __UEP_EPHSK;

   while (UIR_TRN)
   {
      usb_clear_trn();
   }

   UCON_PKTDIS=0; //SIE token and packet processing enabled

   usb_init_ep0_setup();

   usb_state=USB_STATE_DEFAULT; //put usb mcu into default state
}

/*****************************************************************************
/* usb_init_ep0_setup()
/*
/* Summary: Configure EP0 to receive setup packets
/*
/*****************************************************************************/
void usb_init_ep0_setup(void)
{
   usb_bd_stat_t newBD;
   usb_bd_ptr_t bdPtr;

   memset(&newBD, 0x00, sizeof(newBD));
   newBD.count = USB_MAX_EP0_PACKET_LENGTH;
   newBD.uown = TRUE;
  #if __PIC24_NINC_BIT__
   newBD.ninc = __PIC24_NINC_BIT__;
  #endif
  #if !USB_IGNORE_RX_DTS
   newBD.dtsen = TRUE;
  #endif

   bdPtr = USB_DATA_BUFFER_LOCATION;
   UsbEpBdXSetPtr(0, 0, &bdPtr);

   UsbEpBdXSetSt(0, 0, &newBD);

   memset(&newBD, 0x00, sizeof(usb_bd_stat_t));
   UsbEpBdXSetSt(0, 1, &newBD);

   bdPtr = USB_DATA_BUFFER_LOCATION + (usb_bd_ptr_t)USB_MAX_EP0_PACKET_LENGTH;
   UsbEpBdXSetPtr(0, 1, &bdPtr);

   debug_usb(debug_putc, "\r\nEP0 SETUP uc=%02X is=%08LX ia=%08LX os=%08LX oa=%08LX ", UCON, (unsigned int32)EP_BDxST_I(0), (unsigned int32)EP_BDxADR_I(0), (unsigned int32)EP_BDxST_O(0), EP_BDxADR_O(0));
}

/*******************************************************************************
/* usb_isr_uerr()
/*
/* Summary: The USB peripheral had an error.  If user specified, error counter
/*          will incerement.  If having problems check the status of these 8 bytes.
/*
/* NOTE: This code is not enabled by default.
/********************************************************************************/
void usb_isr_uerr(void) {
#if USB_USE_ERROR_COUNTER
   unsigned int ints;
#endif

   debug_usb(debug_putc,"E %02X ",UEIR);

#if USB_USE_ERROR_COUNTER

   ints=UEIR & UEIE; //mask off the flags with the ones that are enabled

   if ( bit_test(ints,0) ) { //increment pid_error counter
      debug_usb(debug_putc,"PID ");
      ERROR_COUNTER[0]++;
   }

   if ( bit_test(ints,1) ) {  //increment crc5 error counter
      debug_usb(debug_putc,"CRC5 ");
      ERROR_COUNTER[1]++;
   }

   if ( bit_test(ints,2) ) {  //increment crc16 error counter
      debug_usb(debug_putc,"CRC16 ");
      ERROR_COUNTER[2]++;
   }

   if ( bit_test(ints,3) ) {  //increment dfn8 error counter
      debug_usb(debug_putc,"DFN8 ");
      ERROR_COUNTER[3]++;
   }

   if ( bit_test(ints,4) ) {  //increment bto error counter
      debug_usb(debug_putc,"BTO ");
      ERROR_COUNTER[4]++;
   }

   if ( bit_test(ints,7) ) { //increment bts error counter
      debug_usb(debug_putc,"BTS ");
      ERROR_COUNTER[5]++;
   }
#endif

   usb_clear_isr_reg(&UEIR);
   usb_clear_isr_flag(&UIR, UIE_BIT_UERR);
}

/*******************************************************************************
/* usb_isr_uidle()
/*
/* Summary: USB peripheral detected IDLE.  Put the USB peripheral to sleep.
/*
/********************************************************************************/
void usb_isr_uidle(void)
{
   debug_usb(debug_putc,"I");

   UOTGIE_ACTV=1;   //enable activity interrupt flag.
   usb_clear_isr_flag(&UIR, UIE_BIT_IDLE);
   //UCON_SUSPND=1; //set suspend. we are now suspended
}


/*******************************************************************************
/* usb_isr_activity()
/*
/* Summary: USB peripheral detected activity on the USB device.  Wake-up the USB
/*          peripheral.
/*
/********************************************************************************/
void usb_isr_activity(void)
{
   debug_usb(debug_putc,"A");

   UCON_SUSPND=0; //turn off low power suspending
   UOTGIE_ACTV=0; //clear activity interupt enabling
   usb_clear_isr_flag(&UOTGIR, UOTGIE_BIT_ACTV);
}

/*******************************************************************************
/* usb_isr_stall()
/*
/* Summary: Stall handshake detected.
/*
/********************************************************************************/
void usb_isr_stall(void)
{
   debug_usb(debug_putc,"S");


   if (bit_test(UEP(0),1)) {
      usb_init_ep0_setup();
      bit_clear(UEP(0),1);
   }
   usb_clear_isr_flag(&UIR, UIE_BIT_STALL);
}

/*******************************************************************************
/* usb_isr_tok_dne()
/*
/* Summary: A Token (IN/OUT/SETUP) has been received by the USB peripheral.
/*          If a setup token on EP0 was received, run the chapter 9 code and
/*          handle the request.
/*          If an IN token on EP0 was received, continue transmitting any
/*          unfinished requests that may take more than one packet to transmit
/*          (if necessary).
/*          If an OUT token on any other EP was received, mark that EP as ready
/*          for a usb_get_packet().
/*          Does not handle any IN or OUT tokens on EP0.
/*
/********************************************************************************/
void usb_isr_tok_dne(void)
{
   usb_bd_stat_t stat;
   unsigned int8 en;
   int1 tgl;

   en=USTAT>>4;

   debug_usb(debug_putc,"T ");
   debug_usb(debug_putc,"us%02X uc%02X ", USTAT, UCON);

   if ((USTAT & 0x00F8) == USTAT_OUT_SETUP_E0)
   {
      //new out or setup token in the buffer
      unsigned int16 pidKey;

      debug_usb(debug_putc,"%08LX ", (unsigned int32)EP_BDxST_O(0));

      UsbEpBdXGetSt(0, 0, &stat);

      pidKey = stat.pid;  //save PID

      //clear pid, prevent bdstal/pid confusion
      stat.uown = 0;
      stat.pid = 0;

      UsbEpBdXSetSt(0, 0, &stat);

      usb_clear_trn();

      if (pidKey == PID_SETUP)
      {
         // return the in buffer to us (dequeue any pending requests)
         memset(&stat, 0x00, sizeof(usb_bd_stat_t));
         UsbEpBdXSetSt(0, 1, &stat);


         debug_usb(debug_putc, "%08LX ", (unsigned int32)EP_BDxST_O(0));
         debug_display_ram(EP_BDxST_O(0).count, usb_ep0_rx_buffer);

         usb_isr_tok_setup_dne();

         UCON_PKTDIS=0;       // UCON,PKT_DIS ; Assuming there is nothing to dequeue, clear the packet disable bit

         //if setup_0_tx_size==0xFF - stall ep0 (unhandled request)
         //if setup_0_tx_size==0xFE - get EP0OUT ready for a data packet, leave EP0IN alone
         //else setup_0_tx_size=size of response, get EP0OUT ready for a setup packet, mark EPOIN ready for transmit
         if (__setup_0_tx_size==0xFF)
            usb_flush_out(0,USB_DTS_STALL);
         else
         {
            UsbEpBdXGetSt(0, 0, &stat);
            tgl = stat.data01;
            tgl = !tgl;
            usb_flush_out(0,USB_DTS_TOGGLE);
            if (__setup_0_tx_size!=0xFE)
            {
               usb_flush_in(0, __setup_0_tx_size, tgl);
               //usb_flush_in(0, __setup_0_tx_size, USB_DTS_USERX);
            }
         }
         //why was this here?
         //UCON_PKTDIS=0;       // UCON,PKT_DIS ; Assuming there is nothing to dequeue, clear the packet disable bit
      }
      else if (pidKey == PID_OUT)
      {
         usb_isr_tok_out_dne(0);
         usb_flush_out(0,USB_DTS_TOGGLE);
         if ((__setup_0_tx_size!=0xFE)&&(__setup_0_tx_size!=0xFF))
               usb_flush_in(0,__setup_0_tx_size,USB_DTS_DATA1);   //send response (usually a 0len)
         debug_display_ram(16, (void*)UBDTP1 << 8);
      }
   }
   else if ((USTAT & 0x00F8) == USTAT_IN_E0)    //pic -> host transfer completed
   {
      /*
      //clear up any BDSTAL confusion
      UsbEpBdXGetSt(0, 1, &stat);
      stat.uown = FALSE;
      stat.count = 0;
      stat.pid = 0;
      UsbEpBdXSetSt(0, 1, &stat);
      */


      usb_clear_trn();
      __setup_0_tx_size=0xFF;
      usb_isr_tok_in_dne(0);
      if (__setup_0_tx_size!=0xFF)
         usb_flush_in(0,__setup_0_tx_size,USB_DTS_TOGGLE);
      //else
      //   usb_init_ep0_setup();
   }
   else
   {
      if (!bit_test(USTAT, 3))
      {
         /*
         //clear up any BDSTAL confusion
         UsbEpBdXGetSt(en, 0, &stat);
         stat.uown = FALSE;
         stat.pid = 0;
         UsbEpBdXSetSt(en, 0, &stat);
         */

         usb_clear_trn();
         usb_isr_tok_out_dne(en);
      }
      else
      {
         /*
         //clear up any BDSTAL confusion
         UsbEpBdXGetSt(en, 1, &stat);
         stat.uown = FALSE;
         stat.count = 0;
         stat.pid = 0;
         UsbEpBdXSetSt(en, 1, &stat);
         */

         usb_clear_trn();
         usb_isr_tok_in_dne(en);
      }
   }
}

// see usb_hw_layer.h for documentation
void usb_request_send_response(unsigned int8 len) {__setup_0_tx_size=len;}
void usb_request_get_data(void) {__setup_0_tx_size=0xFE;}
void usb_request_stall(void) {__setup_0_tx_size=0xFF;}


/// END USB Interrupt Service Routine

static void usb_clear_isr_flag(unsigned int16 *sfr, unsigned int8 bit)
{
   *sfr = (int16)1 << bit;
}

static void usb_clear_isr_reg(unsigned int16 *sfr)
{
   *sfr = 0xFFFF;
}

#ENDIF

