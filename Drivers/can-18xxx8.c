/////////////////////////////////////////////////////////////////////////
////                        can-18xxx8.c                             ////
//// CAN Library routines for Microchip's PIC18Cxx8 and 18Fxx8 line  ////
////                                                                 ////
//// This library provides the following functions:                  ////
////  (for more information on these functions see the comment       ////
////   header above each function)                                   ////
////                                                                 ////
////    can_init - Configures the PIC18xxx8 CAN peripheral           ////
////                                                                 ////
////    can_set_baud - Sets the baud rate control registers          ////
////                                                                 ////
////    can_set_mode - Sets the CAN module into a specific mode      ////
////                                                                 ////
////    can_set_id - Sets the standard and extended ID               ////
////                                                                 ////
////    can_get_id - Gets the standard and extended ID               ////
////                                                                 ////
////    can_putd - Sends a message/request with specified ID         ////
////                                                                 ////
////    can_getd - Returns specified message/request and ID           ////
////                                                                 ////
////    can_kbhit - Returns true if there is data in one of the      ////
////                receive buffers                                  ////
////                                                                 ////
////    can_tbe - Returns true if the transmit buffer is ready to    ////
////              send more data                                     ////
////                                                                 ////
////    can_abort - Aborts all pending transmissions                 ////
////                                                                 ////
//// PIN_B3 is CANRX, and PIN_B2 is CANTX.  You will need a CAN      ////
//// transceiver to connect these pins to CANH and CANL bus lines.    ////
////                                                                 ////
//// CCS provides an example, ex_can.c, which shows how to use this  ////
//// library.                                                        ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// Version History                                                 ////
////                                                                 ////
////  Jul 27 04 - can_init() uses CAN_USE_EXTENDED_ID instead of     ////
////              setting all RX filters to extended.                ////
////                                                                 ////
////  Feb 24 04 - can_get_id() fixed for EID<18:20>.                 ////
////                                                                 ////
////  Mar 30 09 - added CANTX2 code for PIC18F6585/8585/6680/8680    ////
////                                                                 ////
////  May 18 10 - changed variable types to unsigned incase compiled ////
////              with #device ANSI, which makes variables signed    ////
////                                                                 ////
////  Apr 20 11 - updated for new PIC18FxxK80 chips                  ////
////                                                                 ////
////  Jan 10 13 - updated can_init() function to setup correct tris  ////
////              register for PIC being used.                       ////
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

#include <can-18xxx8.h>

#if CAN_DO_DEBUG
 #define can_debug printf
#else
 #define can_debug
#endif


//macros
#define can_kbhit()                 (RXB0CON.rxful || RXB1CON.rxful)
#define can_tbe()                   (!TXB0CON.txreq || !TXB1CON.txreq || !TXB2CON.txreq)
#define can_abort()                 (CANCON.abat=1)


////////////////////////////////////////////////////////////////////////
//
// can_init()
//
// Initializes PIC18xxx8 CAN peripheral.  Sets the RX filter and masks so the
// CAN peripheral will receive all incoming IDs.  Configures both RX buffers
// to only accept valid messages (as opposed to all messages, or all
// extended message, or all standard messages).  Also sets the tri-state
// setting of B2 to output, and B3 to input (apparently the CAN peripheral
// doesn't keep track of this)
//
// The constants (CAN_USE_RX_DOUBLE_BUFFER, CAN_ENABLE_DRIVE_HIGH,
// CAN_ENABLE_CAN_CAPTURE) are given a default define in the can-18xxx8.h file.
// These default values can be overwritten in the main code, but most
// applications will be fine with these defaults.
//
//////////////////////////////////////////////////////////////////////////////
void can_init(void) {
   can_set_mode(CAN_OP_CONFIG);   //must be in config mode before params can be set
   can_set_baud();

   RXB0CON=0;
   RXB0CON.rxm=CAN_RX_VALID;
   RXB0CON.rxb0dben=CAN_USE_RX_DOUBLE_BUFFER;
   RXB1CON=RXB0CON;

   CIOCON.endrhi=CAN_ENABLE_DRIVE_HIGH;
   CIOCON.cancap=CAN_ENABLE_CAN_CAPTURE;
   CIOCON.tx2src=CAN_CANTX2_SOURCE;       //added 3/30/09 for PIC18F6585/8585/6680/8680
   CIOCON.tx2en=CAN_ENABLE_CANTX2;        //added 3/30/09 for PIC18F6585/8585/6680/8680

   can_set_id(RX0MASK, CAN_MASK_ACCEPT_ALL, CAN_USE_EXTENDED_ID);  //set mask 0
   can_set_id(RX0FILTER0, 0, CAN_USE_EXTENDED_ID);  //set filter 0 of mask 0
   can_set_id(RX0FILTER1, 0, CAN_USE_EXTENDED_ID);  //set filter 1 of mask 0

   can_set_id(RX1MASK, CAN_MASK_ACCEPT_ALL, CAN_USE_EXTENDED_ID);  //set mask 1
   can_set_id(RX1FILTER2, 0, CAN_USE_EXTENDED_ID);  //set filter 0 of mask 1
   can_set_id(RX1FILTER3, 0, CAN_USE_EXTENDED_ID);  //set filter 1 of mask 1
   can_set_id(RX1FILTER4, 0, CAN_USE_EXTENDED_ID);  //set filter 2 of mask 1
   can_set_id(RX1FILTER5, 0, CAN_USE_EXTENDED_ID);  //set filter 3 of mask 1

  #if (getenv("DEVICE") == "PIC18F6585") || (getenv("DEVICE") == "PIC18LF6585") || \
      (getenv("DEVICE") == "PIC18F6680") || (getenv("DEVICE") == "PIC18LF6680") || \
      (getenv("DEVICE") == "PIC18F8585") || (getenv("DEVICE") == "PIC18LF8585") || \
      (getenv("DEVICE") == "PIC18F8680") || (getenv("DEVICE") == "PIC18LF8680") || \
      (getenv("DEVICE") == "PIC18C658") || (getenv("DEVICE") == "PIC18C858")
   set_tris_g((*getenv("SFR:TRISG") & 0xFE) | 0x04);  //G2 in, G0 out
   if(CAN_ENABLE_CANTX2)
    bit_clear(*getenv("SFR:TRISG"), 1);  //G1 out
  #elif getenv("FUSE_SET:CANE")
   set_tris_e((*getenv("SFR:TRISE") & 0xDF) | 0x10);  //E4 in, E5 out
  #elif getenv("FUSE_SET:CANC")
   set_tris_c((*getenv("SFR:TRISC") & 0xBF) | 0x80);  //C7 in, C6 out
  #else
   set_tris_b((*getenv("SFR:TRISB") & 0xFB) | 0x08);  //B3 in, B2 out
  #endif

   can_set_mode(CAN_OP_NORMAL);
}

////////////////////////////////////////////////////////////////////////
//
// can_set_baud()
//
// Configures the baud rate control registers.  All the defines here
// are defaulted in the can-18xxx8.h file.  These defaults can, and
// probably should, be overwritten in the main code.
//
// Current defaults are set to work with Microchip's MCP250xxx CAN
// Developers Kit if this PIC is running at 20Mhz.
//
////////////////////////////////////////////////////////////////////////
void can_set_baud(void) {
   BRGCON1.brp=CAN_BRG_PRESCALAR;
   BRGCON1.sjw=CAN_BRG_SYNCH_JUMP_WIDTH;

   BRGCON2.prseg=CAN_BRG_PROPAGATION_TIME;
   BRGCON2.seg1ph=CAN_BRG_PHASE_SEGMENT_1;
   BRGCON2.sam=CAN_BRG_SAM;
   BRGCON2.seg2phts=CAN_BRG_SEG_2_PHASE_TS;

   BRGCON3.seg2ph=CAN_BRG_PHASE_SEGMENT_2;
   BRGCON3.wakfil=CAN_BRG_WAKE_FILTER;
}

void can_set_mode(CAN_OP_MODE mode) {
   CANCON.reqop=mode;
   while( (CANSTAT.opmode) != mode );
}


////////////////////////////////////////////////////////////////////////
//
// can_set_id()
//
// Configures the xxxxEIDL, xxxxEIDH, xxxxSIDL and xxxxSIDH registers to
// configure the defined buffer to use the specified ID
//
//   Parameters:
//     addr - pointer to first byte of ID register, starting with xxxxEIDL.
//            For example, a pointer to RXM1EIDL
//     id - ID to set buffer to
//     ext - Set to TRUE if this buffer uses an extended ID, FALSE if not
//
////////////////////////////////////////////////////////////////////////
void can_set_id(unsigned int* addr, unsigned int32 id, int1 ext) {
   int *ptr;

   ptr=addr;

   if (ext) {  //extended
      //eidl
      *ptr=make8(id,0); //0:7

      //eidh
      ptr--;
      *ptr=make8(id,1); //8:15

      //sidl
      ptr--;
      *ptr=make8(id,2) & 0x03;   //16:17
      *ptr|=(make8(id,2) << 3) & 0xE0; //18:20
      *ptr|=0x08;


      //sidh
      ptr--;
      *ptr=((make8(id,2) >> 5) & 0x07 ); //21:23
      *ptr|=((make8(id,3) << 3) & 0xF8);//24:28
   }
   else {   //standard
      //eidl
      *ptr=0;

      //eidh
      ptr--;
      *ptr=0;

      //sidl
      ptr--;
      *ptr=(make8(id,0) << 5) & 0xE0;

      //sidh
      ptr--;
      *ptr=(make8(id,0) >> 3) & 0x1F;
      *ptr|=(make8(id,1) << 5) & 0xE0;
   }
}

////////////////////////////////////////////////////////////////////////
//
// can_get_id()
//
// Returns the ID of the specified buffer.  (The opposite of can_set_id())
// This is used after receiving a message, to see which ID sent the message.
//
//   Parameters:
//     addr - pointer to first byte of ID register, starting with xxxxEIDL.
//            For example, a pointer to RXM1EIDL
//     ext - Set to TRUE if this buffer uses an extended ID, FALSE if not
//
//   Returns:
//     The ID of the buffer
//
////////////////////////////////////////////////////////////////////////
unsigned int32 can_get_id(unsigned int * addr, int1 ext) {
   unsigned int32 ret;
   unsigned int * ptr;

   ret=0;
   ptr=addr;

   if (ext) {
      ret=*ptr;  //eidl

      ptr--;     //eidh
      ret|=((unsigned int32)*ptr << 8);

      ptr--;     //sidl
      ret|=((unsigned int32)*ptr & 0x03) << 16;
      ret|=((unsigned int32)*ptr & 0xE0) << 13;

      ptr--;     //sidh
      ret|=((unsigned int32)*ptr << 21);

   }
   else {
      ptr-=2;    //sidl
      ret=((unsigned int32)*ptr & 0xE0) >> 5;

      ptr--;     //sidh
      ret|=((unsigned int32)*ptr << 3);
   }

   return(ret);
}

////////////////////////////////////////////////////////////////////////
//
// can_putd()
//
// Puts data on a transmit buffer, at which time the CAN peripheral will
// send when the CAN bus becomes available.
//
//    Parameters:
//       id - ID to transmit data as
//       data - pointer to data to send
//       len - length of data to send
//       priority - priority of message.  The higher the number, the
//                  sooner the CAN peripheral will send the message.
//                  Numbers 0 through 3 are valid.
//       ext - TRUE to use an extended ID, FALSE if not
//       rtr - TRUE to set the RTR (request) bit in the ID, false if NOT
//
//    Returns:
//       If successful, it will return TRUE
//       If un-successful, will return FALSE
//
////////////////////////////////////////////////////////////////////////
int1 can_putd(unsigned int32 id, unsigned int * data, unsigned int len, unsigned int priority, int1 ext, int1 rtr) {
   unsigned int i;
   unsigned int * txd0;
   unsigned int port;

   txd0=&TXRXBaD0;

    // find empty transmitter
    //map access bank addresses to empty transmitter
   if (!TXB0CON.txreq) {
      CANCON.win=CAN_WIN_TX0;
      port=0;
   }
   else if (!TXB1CON.txreq) {
      CANCON.win=CAN_WIN_TX1;
      port=1;
   }
   else if (!TXB2CON.txreq) {
      CANCON.win=CAN_WIN_TX2;
      port=2;
   }
   else {
      #if CAN_DO_DEBUG
         can_debug("\r\nCAN_PUTD() FAIL: NO OPEN TX BUFFERS\r\n");
      #endif
      return(0);
   }

   //set priority.
   TXBaCON.txpri=priority;

   //set tx mask
   can_set_id(TXRXBaID, id, ext);

   //set tx data count
   TXBaDLC=len;
   TXBaDLC.rtr=rtr;

    for (i=0; i<len; i++) {
      *txd0=*data;
      txd0++;
      data++;
    }

   //enable transmission
   TXBaCON.txreq=1;

   CANCON.win=CAN_WIN_RX0;

   #if CAN_DO_DEBUG
            can_debug("\r\nCAN_PUTD(): BUFF=%U ID=%LX LEN=%U PRI=%U EXT=%U RTR=%U\r\n", port, id, len, priority, ext, rtr);
            if ((len)&&(!rtr)) {
               data-=len;
               can_debug("  DATA = ");
               for (i=0;i<len;i++) {
                  can_debug("%X ",*data);
                  data++;
               }
               can_debug("\r\n");
            }
   #endif

   return(1);
}

////////////////////////////////////////////////////////////////////////
//
// can_getd()
//
// Gets data from a receive buffer, if the data exists
//
//    Returns:
//      id - ID who sent message
//      data - pointer to array of data
//      len - length of received data
//      stat - structure holding some information (such as which buffer
//             received it, ext or standard, etc)
//
//    Returns:
//      Function call returns a TRUE if there was data in a RX buffer, FALSE
//      if there was none.
//
////////////////////////////////////////////////////////////////////////
int1 can_getd(unsigned int32 & id, unsigned int * data, unsigned int & len, struct rx_stat & stat)
{
    unsigned int i;
    unsigned int * ptr;

    if (RXB0CON.rxful) {
        CANCON.win=CAN_WIN_RX0;
        stat.buffer=0;

        CAN_INT_RXB0IF=0;

        stat.err_ovfl=COMSTAT.rx0ovfl;
        COMSTAT.rx0ovfl=0;

        if (RXB0CON.rxb0dben) {
         stat.filthit=RXB0CON.filthit0;
        }
    }
    else if ( RXB1CON.rxful )
    {
        CANCON.win=CAN_WIN_RX1;
        stat.buffer=1;

        CAN_INT_RXB1IF=0;

        stat.err_ovfl=COMSTAT.rx1ovfl;
        COMSTAT.rx1ovfl=0;

        stat.filthit=RXB1CON.filthit;
    }
    else {
      #if CAN_DO_DEBUG
         can_debug("\r\nFAIL ON CAN_GETD(): NO MESSAGE IN BUFFER\r\n");
      #endif
      return (0);
    }

    len = RXBaDLC.dlc;
    stat.rtr=RXBaDLC.rtr;

    stat.ext=TXRXBaSIDL.ext;
    id=can_get_id(TXRXBaID,stat.ext);

    ptr = &TXRXBaD0;
    for ( i = 0; i < len; i++ ) {
        *data = *ptr;
        data++;
        ptr++;
    }

    // return to default addressing
    CANCON.win=CAN_WIN_RX0;

    stat.inv=CAN_INT_IRXIF;
    CAN_INT_IRXIF = 0;

    if (stat.buffer) {
      RXB1CON.rxful=0;
    }
    else {
      RXB0CON.rxful=0;
    }

    #if CAN_DO_DEBUG
       can_debug("\r\nCAN_GETD(): BUFF=%U ID=%LX LEN=%U OVF=%U ", stat.buffer, id, len, stat.err_ovfl);
       can_debug("FILT=%U RTR=%U EXT=%U INV=%U", stat.filthit, stat.rtr, stat.ext, stat.inv);
       if ((len)&&(!stat.rtr)) {
          data-=len;
          can_debug("\r\n    DATA = ");
          for (i=0;i<len;i++) {
            can_debug("%X ",*data);
            data++;
          }
       }
       can_debug("\r\n");
    #endif

    return(1);
}



