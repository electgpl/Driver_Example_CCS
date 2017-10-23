/////////////////////////////////////////////////////////////////////////
////                        can-mcp251x.c                            ////
//// CAN Library routines for Microchip's MCP2510 (and compatible)   ////
//// CAN IO expanders.                                               ////
////                                                                 ////
//// This library provides the following functions:                  ////
////  (for more information on these functions see the comment       ////
////   header above each function)                                   ////
////                                                                 ////
////    can_init - Configures the MCP2510 CAN peripheral             ////
////                                                                 ////
////    can_set_baud - Sets the baud rate control registers          ////
////                                                                 ////
////    can_set_mode- Sets the CAN module into a specific mode      ////
////                                                                 ////
////    can_set_id - Sets the standard and extended ID               ////
////                                                                 ////
////    can_get_id - Gets the standard and extended ID               ////
////                                                                 ////
////    can_putd - Sends a message/request with specified ID         ////
////                                                                 ////
////    can_getd - Returns specified message/request and ID          ////
////                                                                 ////
////    can_kbhit - Returns true if there is data in one of the      ////
////                receive buffers                                  ////
////                                                                 ////
////    can_tbe - Returns true if the transmit buffer is ready to    ////
////              send more data                                     ////
////                                                                 ////
////    can_abort - Aborts all pending transmissions                 ////
////                                                                 ////
////    can_enable_interrupts - Enable specified CAN interrupts      ////
////                                                                 ////
////    can_disable_interrupts - Disable specified CAN interrupts    ////
////                                                                 ////
////    can_clear_interrupt - Clear specified CAN interrupt flag     ////
////                                                                 ////
//// You will need a CAN transceiver to connect CANRX and CANTX      ////
//// pins to CANH and CANL bus lines.                                ////
////                                                                 ////
//// CCS provides an example, ex_can_ccs_b.c, which shows how to use ////
//// this library with CCS's CAN Prototype board.                    ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// Version History                                                 ////
////                                                                 ////
////  Jul 27 04 - can_init() uses CAN_USE_EXTENDED_ID instead of     ////
////              setting all RX filters to extended.                ////
////                                                                 ////
////  Apr 20 04 - Fixed a compiling problem.                         ////
////                                                                 ////
////  Feb 24 04 - can_get_id() fixed for EID<18:20>.                 ////
////                                                                 ////
////  Jul 17 09 - Updated for PCD Compiler                           ////
////                                                                 ////
////  May 20 10 - Changed all variable types to unsigned for PCD and ////
////              if #device ANSI is used with PCH or PCM            ////
////                                                                 ////
////  Nov 10 10 - Modified SPI protocol to use #use SPI()            ////
////                                                                 ////
////  Aug 07 13 - Added can_enable_interrupts(),                     ////
////              can_disable_interrupts() and can_clear_interrupt() ////
////              functions.                                         ////
////                                                                 ////
////  Oct 24 13 - Updated can_set_mode() function so that it reads   ////
////              the CANSTAT instead of the CANCTRL to determine if ////
////              the mode as changed.                               ////
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

#include <can-mcp251x.h>

//IO pins connected to MCP2510
#ifndef EXT_CAN_CS
   #if defined(__PCD__)
      #define EXT_CAN_CS   PIN_B1
      #define EXT_CAN_SI   PIN_B2
      #define EXT_CAN_SO   PIN_B4
      #define EXT_CAN_SCK  PIN_E5
   #else
      #define EXT_CAN_CS   PIN_B1
      #define EXT_CAN_SI   PIN_C1
      #define EXT_CAN_SO   PIN_C0
      #define EXT_CAN_SCK  PIN_C3
   #endif
//   #define EXT_CAN_RESET   PIN_B5 //CCS library does not use this pin by default
//   #define EXT_CAN_TX0RTS  PIN_C4 //CCS library does not use this pin by default
//   #define EXT_CAN_TX1RTS  PIN_B4 //CCS library does not use this pin by default
//   #define EXT_CAN_TX2RTS  PIN_C2 //CCS library does not use this pin by default
#endif

#if CAN_DO_DEBUG
 #define can_debug printf
#else
 #define can_debug
#endif

////////////////////////////////////////////////////////////////////////
//
// can_init()
//
// Initializes MCP2510 CAN peripheral.  Sets the RX filter and masks so the
// CAN peripheral will receive all incoming IDs.  Configures both RX buffers
// to only accept valid messages (as opposed to all messages, or all
// extended message, or all standard messages).
//
// The constants (CAN_USE_RX_DOUBLE_BUFFER, CAN_ENABLE_DRIVE_HIGH,
// CAN_ENABLE_CAN_CAPTURE, etc) are given a default define in the can-mcp2510.h file.
// These default values can be overwritten in the main code, but most
// applications will be fine with these defaults.
//
//////////////////////////////////////////////////////////////////////////////
void can_init(void) {
   struct struct_RXB0CTRL b_rxb0ctrl;

   mcp2510_init();

   can_set_mode(CAN_OP_CONFIG);   //must be in config mode before params can be set
   can_set_baud();

   memset(&b_rxb0ctrl,0,1);
  // b_rxb0ctrl=0;
   b_rxb0ctrl.rxm=CAN_RX_VALID;
   b_rxb0ctrl.bukt=CAN_USE_RX_DOUBLE_BUFFER;
   mcp2510_write(RXB0CTRL, (unsigned int8)b_rxb0ctrl);
   mcp2510_write(RXB1CTRL, (unsigned int8)b_rxb0ctrl);

   //if you want to configure the TXnRTS pins, do it here.  default is off

   can_set_id(RX0MASK, CAN_MASK_ACCEPT_ALL, CAN_USE_EXTENDED_ID);  //set mask 0 (RX BUFFER 0)
   can_set_id(RX0FILTER0, 0, CAN_USE_EXTENDED_ID);  //set filter 0 of mask 0 (RX BUFFER 0)
   can_set_id(RX0FILTER1, 0, CAN_USE_EXTENDED_ID);  //set filter 1 of mask 0 (RX BUFFER 0)

   can_set_id(RX1MASK, CAN_MASK_ACCEPT_ALL, CAN_USE_EXTENDED_ID);  //set mask 1 (RX BUFFER 1)
   can_set_id(RX1FILTER2, 0, CAN_USE_EXTENDED_ID);  //set filter 0 of mask 1 (RX BUFFER 1)
   can_set_id(RX1FILTER3, 0, CAN_USE_EXTENDED_ID);  //set filter 1 of mask 1 (RX BUFFER 1)
   can_set_id(RX1FILTER4, 0, CAN_USE_EXTENDED_ID);  //set filter 2 of mask 1 (RX BUFFER 1)
   can_set_id(RX1FILTER5, 0, CAN_USE_EXTENDED_ID);  //set filter 3 of mask 1 (RX BUFFER 1)

   can_set_mode(CAN_OP_NORMAL);
}

////////////////////////////////////////////////////////////////////////
//
// can_set_baud()
//
// Configures the baud rate control registers.  All the defines here
// are defaulted in the can-mcp2510.h file.  These defaults can, and
// probably should, be overwritten in the main code.
//
// Current defaults are set to work with CCS's CAN Prototype board and
// Microchip's MCP250xxx CAN Developers Kit if this PIC is running at 20MHz.
//
////////////////////////////////////////////////////////////////////////
void can_set_baud(void) {
   struct struct_CNF1 new_CNF1;
   struct struct_CNF2 new_CNF2;
   struct struct_CNF3 new_CNF3;


   new_CNF1.brp=CAN_BRG_PRESCALAR;
   new_CNF1.sjw=CAN_BRG_SYNCH_JUMP_WIDTH;

   new_CNF2.prseg=CAN_BRG_PROPAGATION_TIME;
   new_CNF2.phseg1=CAN_BRG_PHASE_SEGMENT_1;
   new_CNF2.sam=CAN_BRG_SAM;
   new_CNF2.btlmode=CAN_BRG_SEG_2_PHASE_TS;

   new_CNF3.phseg2=CAN_BRG_PHASE_SEGMENT_2;
   new_CNF3.wakfil=CAN_BRG_WAKE_FILTER;
   new_CNF3.sof=0;

   mcp2510_write(CNF1, (unsigned int8)new_CNF1);
   mcp2510_write(CNF2, (unsigned int8)new_CNF2);
   mcp2510_write(CNF3, (unsigned int8)new_CNF3);
}

void can_set_mode(CAN_OP_MODE mode) {
   struct struct_CANCTRL old_CANCTRL;
   struct struct_CANSTAT new_CANSTAT;

   memset(&old_CANCTRL,mcp2510_read(CANCTRL),1);

   old_CANCTRL.reqop=mode;

   mcp2510_write(CANCTRL, (unsigned int8)old_CANCTRL);

   do {
      memset(&new_CANSTAT,mcp2510_read(CANSTAT),1);
      
   } while (new_CANSTAT.opmode != mode);
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
void can_set_id(unsigned int8 addr, unsigned int32 id, int1 ext) {
   unsigned int8 converted_id[4];
   unsigned int8 *ptr;

   ptr=&converted_id[3];   //3=eidl, 2=eidh, 1=sidl, 0=sidh

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

   //0=eidl, 1=eidh, 2=sidl, 3=sidh
   mcp2510_write(addr--, converted_id[3]);
   mcp2510_write(addr--, converted_id[2]);
   mcp2510_write(addr--, converted_id[1]);
   mcp2510_write(addr, converted_id[0]);
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
unsigned int32 can_get_id(unsigned int8 addr, int1 ext) {
   unsigned int32 ret;
   unsigned int8 * ptr;
   unsigned int8 converted_id[4];

   ptr=&converted_id[3];   //3=eidl, 2=eidh, 1=sidl, 0=sidh

   converted_id[3]=mcp2510_read(addr--);
   converted_id[2]=mcp2510_read(addr--);
   converted_id[1]=mcp2510_read(addr--);
   converted_id[0]=mcp2510_read(addr);

   ret=0;


   if (ext) {
      ret=(unsigned int32)*ptr;  //eidl

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
//       If unsuccessful, will return FALSE
//
////////////////////////////////////////////////////////////////////////
int1 can_putd(unsigned int32 id, unsigned int8 * data, unsigned int8 len, unsigned int8 priority, int1 ext, int1 rtr) {
   unsigned int8 i;
   unsigned int8 port;

   unsigned int8 TXRXBaD0;
   unsigned int8 TXBaCTRL;
   unsigned int8 TXRXBaEIDL;
   unsigned int8 TXBaDLC;

   struct txbNctrl_struct b_TXBaCTRL;
   struct rxbNdlc_struct b_TXBaDLC;
   struct txbNctrl_struct b_TXB0CTRL, b_TXB1CTRL, b_TXB2CTRL;

   //b_TXB0CTRL=mcp2510_read(TXB0CTRL);
   memset(&b_TXB0CTRL,mcp2510_read(TXB0CTRL),1);
   //b_TXB1CTRL=mcp2510_read(TXB1CTRL);
   memset(&b_TXB1CTRL,mcp2510_read(TXB1CTRL),1);
   //b_TXB2CTRL=mcp2510_read(TXB2CTRL);
   memset(&b_TXB2CTRL,mcp2510_read(TXB2CTRL),1);

    // find empty transmitter
    //map access bank addresses to empty transmitter
   if (!b_TXB0CTRL.txreq) {
      TXRXBaD0=TXB0D0;
      TXBaCTRL=TXB0CTRL;
      TXRXBaEIDL=TXB0EIDL;
      TXBaDLC=TXB0DLC;
      port=0;
   }
   else if (!b_TXB1CTRL.txreq) {
      TXRXBaD0=TXB1D0;
      TXBaCTRL=TXB1CTRL;
      TXRXBaEIDL=TXB1EIDL;
      TXBaDLC=TXB1DLC;
      port=1;
   }
   else if (!b_TXB2CTRL.txreq) {
      TXRXBaD0=TXB2D0;
      TXBaCTRL=TXB2CTRL;
      TXRXBaEIDL=TXB2EIDL;
      TXBaDLC=TXB2DLC;
      port=2;
   }
   else {
      #if CAN_DO_DEBUG
         can_debug("\r\nCAN_PUTD() FAIL: NO OPEN TX BUFFERS\r\n");
      #endif
      return(0);
   }

   //set priority.
   //b_TXBaCTRL=mcp2510_read(TXBaCTRL);
   memset(&b_TXBaCTRL,mcp2510_read(TXBaCTRL),1);
   
   b_TXBaCTRL.txpri=priority;
   mcp2510_write(TXBaCTRL, (unsigned int8)b_TXBaCTRL);

   //set tx mask
   can_set_id(TXRXBaEIDL, id, ext);

   //set tx data count
   //b_TXBaDLC=len;
   memset(&b_TXBaDLC,len,1);
   b_TXBaDLC.rtr=rtr;
   mcp2510_write(TXBaDLC, (unsigned int8)b_TXBaDLC);

   //write to buffer
    for (i=TXRXBaD0; i<(TXRXBaD0 + len); i++) {
      mcp2510_write(i,*data);
      data++;
    }

   //enable transmission
   //b_TXBaCTRL=mcp2510_read(TXBaCTRL);
   memset(&b_TXBaCTRL,mcp2510_read(TXBaCTRL),1);
   b_TXBaCTRL.txreq=1;
   mcp2510_write(TXBaCTRL, (unsigned int8)b_TXBaCTRL);

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
int1 can_getd(unsigned int32 & id, unsigned int8 * data, unsigned int8 & len, struct rx_stat & stat)
{
   unsigned int8 i;

   struct struct_RXB0CTRL b_RXB0CTRL;
   struct struct_RXB1CTRL b_RXB1CTRL;
   struct struct_EFLG b_EFLG;

   unsigned int8 RXBaDLC;
   struct rxbNdlc_struct b_RXBaDLC;

   unsigned int8 TXRXBaSIDL;
   struct struct_TXRXBaSIDL b_TXRXBaSIDL;


   unsigned int8 RXBaD0;
   struct struct_CANINTF b_CANINTF;

   //b_CANINTF=mcp2510_read(CANINTF);
   memset(&b_CANINTF,mcp2510_read(CANINTF),1);

   //b_RXB0CTRL=mcp2510_read(RXB0CTRL);
   memset(&b_RXB0CTRL,mcp2510_read(RXB0CTRL),1);
   //b_RXB1CTRL=mcp2510_read(RXB1CTRL);
   memset(&b_RXB1CTRL,mcp2510_read(RXB1CTRL),1);
   //b_EFLG=mcp2510_read(EFLG);
   memset(&b_EFLG,mcp2510_read(EFLG),1);

    if (b_CANINTF.rx0if) {
        stat.buffer=0;

        stat.err_ovfl=b_EFLG.rx0ovr;
        b_EFLG.rx0ovr=0;
        mcp2510_write(EFLG, (unsigned int8)b_EFLG);

        if (b_RXB0CTRL.bukt) {
         stat.filthit=b_RXB0CTRL.filhit0;
        }

        RXBaDLC=RXB0DLC;
        TXRXBaSIDL=RXB0SIDL;
        RXBaD0=RXB0D0;
    }
    else if (b_CANINTF.rx1if)
    {
        stat.buffer=1;

        stat.err_ovfl=b_EFLG.rx1ovr;
        b_EFLG.rx1ovr=0;
        mcp2510_write(EFLG, (unsigned int8)b_EFLG);


        stat.filthit=b_RXB1CTRL.filhit0;
        RXBaDLC=RXB1DLC;
        TXRXBaSIDL=RXB1SIDL;
        RXBaD0=RXB1D0;
    }
    else {
      #if CAN_DO_DEBUG
         can_debug("\r\nFAIL ON CAN_GETD(): NO MESSAGE IN BUFFER\r\n");
      #endif
      return (0);
    }

   //get count
    //b_RXBaDLC=mcp2510_read(RXBaDLC);
    memset(&b_RXBaDLC,mcp2510_read(RXBaDLC),1);
    len = b_RXBaDLC.dlc;
    stat.rtr=b_RXBaDLC.rtr;

   //was it extended or standard?
    //b_TXRXBaSIDL=mcp2510_read(TXRXBaSIDL);
    memset(&b_TXRXBaSIDL,mcp2510_read(TXRXBaSIDL),1);
    stat.ext=b_TXRXBaSIDL.ext;
    id=can_get_id(TXRXBaSIDL + 2,stat.ext);

   //get data
    for ( i = RXBaD0; i < (RXBaD0 + len); i++ ) {
         *data=mcp2510_read(i);
        data++;
    }

    stat.inv=b_CANINTF.merrf;
    if (b_CANINTF.merrf) {
      b_CANINTF.merrf=0;
    }

    if (stat.buffer) {
      b_CANINTF.rx1if=0;
    }
    else {
      b_CANINTF.rx0if=0;
    }
    mcp2510_write(CANINTF, (unsigned int8)b_CANINTF);

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

////////////////////////////////////////////////////////////////////////
//
// can_kbhit()
//
// Returns TRUE if there is data in the receive buffers
//
//////////////////////////////////////////////////////////////////////////////
int1 can_kbhit(void) {
   struct struct_CANINTF b_CANINTF;

   //b_CANINTF=mcp2510_read(CANINTF);
   memset(&b_CANINTF,mcp2510_read(CANINTF),1);
   if (b_CANINTF.rx0if || b_CANINTF.rx1if)
      {return(1);}

   return(0);
}

////////////////////////////////////////////////////////////////////////
//
// can_tbe()
//
// Returns TRUE if the transmit buffers are empty and ready to transmit data
//
//////////////////////////////////////////////////////////////////////////////
int1 can_tbe(void) {
   struct txbNctrl_struct b_TXB0CTRL, b_TXB1CTRL, b_TXB2CTRL;

   //b_TXB0CTRL=mcp2510_read(TXB0CTRL);
   memset(&b_TXB0CTRL,mcp2510_read(TXB0CTRL),1);
   //b_TXB1CTRL=mcp2510_read(TXB1CTRL);
   memset(&b_TXB1CTRL,mcp2510_read(TXB1CTRL),1);
   //b_TXB2CTRL=mcp2510_read(TXB2CTRL);
   memset(&b_TXB2CTRL,mcp2510_read(TXB2CTRL),1);

   if (!b_TXB0CTRL.txreq || !b_TXB1CTRL.txreq || !b_TXB2CTRL.txreq)
      {return(1);}

   return(0);
}

////////////////////////////////////////////////////////////////////////
//
// can_abort()
//
// Aborts all pending transmissions.
//
//////////////////////////////////////////////////////////////////////////////
void can_abort(void) {
   struct struct_CANCTRL b_CANCTRL;

   //b_CANCTRL=mcp2510_read(CANCTRL);
   memset(&b_CANCTRL,mcp2510_read(CANCTRL),1);
   b_CANCTRL.abat=1;
   mcp2510_write(CANCTRL, (unsigned int8)b_CANCTRL);

   delay_ms(5);
   b_CANCTRL.abat=0;
   mcp2510_write(CANCTRL, (unsigned int8)b_CANCTRL);
}

///////////////////////////////////////////////////////////////////////////////
//
// can_enable_interrupts()
//
//    Selects which interrupt sources causes the INT pin to be driven low. 
//
//    Parameters
//       setting - the selected interrupts 
//          enumerated as
//             RX0 - Receive Buffer 0 Interrupt
//             RX1 - Receive Buffer 1 Interrupt
//             TX0 - Transmit Buffer 0 Interrupt
//             TX1 - Transmit Buffer 1 Interrupt
//             TX2 - Transmit Buffer 2 Interrupt
//             ERR - Error Interrupt
//             WAK - Wake Up Activity Interrupt
//             IVR - Invalid Message Received Interrupt
//
//       You can or these together (via |) to enable more than one interrupt 
//       source
//
///////////////////////////////////////////////////////////////////////////////
void can_enable_interrupts(INTERRUPT settings)
{
   mcp2510_bit_modify(CANINTE, settings, settings);
}

///////////////////////////////////////////////////////////////////////////////
//
// can_disable_interrupts()
//
//    Disables selected interrupts from causing the INT pin to be driven low. 
//
//    Parameters
//       setting - the selected interrupts 
//          enumerated as
//             RX0 - Receive Buffer 0 Interrupt
//             RX1 - Receive Buffer 1 Interrupt
//             TX0 - Transmit Buffer 0 Interrupt
//             TX1 - Transmit Buffer 1 Interrupt
//             TX2 - Transmit Buffer 2 Interrupt
//             ERR - Error Interrupt
//             WAK - Wake Up Activity Interrupt
//             IVR - Invalid Message Received Interrupt
//
//       You can or these together (via |) to disable more than one interrupt 
//       source
//
///////////////////////////////////////////////////////////////////////////////
void can_disable_interrupts(INTERRUPT settings)
{
   mcp2510_bit_modify(CANINTE, settings, ~settings);
}

///////////////////////////////////////////////////////////////////////////////
//
// can_clear_interrupt()
//
//    Clears selected interrupt flags.
//
//    Parameters
//       setting - the selected interrupts
//          enumerated as
//             RX0 - Receive Buffer 0 Interrupt
//             RX1 - Receive Buffer 1 Interrupt
//             TX0 - Transmit Buffer 0 Interrupt
//             TX1 - Transmit Buffer 1 Interrupt
//             TX2 - Transmit Buffer 2 Interrupt
//             ERR - Error Interrupt
//             WAK - Wake Up Activity Interrupt
//             IVR - Invalid Message Received Interrupt
//
//       You can or these together (via |) to clear more than one interrupt
//       flag
//
///////////////////////////////////////////////////////////////////////////////
void can_clear_interrupt(INTERRUPT settings)
{
   mcp2510_bit_modify(CANINTF, settings, ~settings);
}


///////////////////
///
//
// SPI CODE
//
///
//////////////////

#ifndef USE_HARDWARE_SPI
   #use spi(MASTER,MODE=0,DI=EXT_CAN_SO,DO=EXT_CAN_SI,CLK=EXT_CAN_SCK,bits=8,stream=MCP2510)
#else
   #use spi(MASTER,MODE=0,DI=EXT_CAN_SO,DO=EXT_CAN_SI,CLK=EXT_CAN_SCK,bits=8,stream=MCP2510,FORCE_HW)
#endif

//data clocked in on rising edge
//data driven out on falling edge
unsigned int8 mcp2510_read(unsigned int8 address) {
   unsigned int8 data;

   output_low(EXT_CAN_CS);
   
   spi_xfer(MCP2510,0x03);
   spi_xfer(MCP2510,address);
   data = spi_xfer(MCP2510,0);
   
   output_high(EXT_CAN_CS);

   return(data);
}

unsigned int8 mcp2510_status(void) {
   unsigned int8 data;
   
   output_low(EXT_CAN_CS);
   
   spi_xfer(MCP2510,0xA0);
   data = spi_xfer(MCP2510,0);
   spi_xfer(MCP2510,0);
   
   output_high(EXT_CAN_CS);

   return(data);
}


void mcp2510_write(unsigned int8 address, unsigned int8 data) {
   output_low(EXT_CAN_CS);
   
   spi_xfer(MCP2510,0x02);
   spi_xfer(MCP2510,address);
   spi_xfer(MCP2510,data);
   
   output_high(EXT_CAN_CS);
}

void mcp2510_command(unsigned int8 command) {
   output_low(EXT_CAN_CS);
   
   spi_xfer(MCP2510,command);
   
   output_high(EXT_CAN_CS);
}

void mcp2510_init(void) {
   output_high(EXT_CAN_CS);

   #ifdef EXT_CAN_TX0RTS
    output_high(EXT_CAN_TX0RTS);
   #endif
   #ifdef EXT_CAN_TX1RTS
    output_high(EXT_CAN_TX1RTS);
   #endif
   #ifdef EXT_CAN_TX2RTS
    output_high(EXT_CAN_TX2RTS);
   #endif

  #ifdef EXT_CAN_TX0RTS
   output_high(EXT_CAN_RESET);
   output_low(EXT_CAN_RESET);
   output_high(EXT_CAN_RESET);
   delay_ms(5);
  #endif

   mcp2510_command(0xC0);   //reset
   delay_ms(5);
}

void mcp2510_bit_modify(unsigned int8 address, unsigned int8 mask, unsigned int8 data)
{
   output_low(EXT_CAN_CS);
   
   spi_xfer(MCP2510, 0x05);
   spi_xfer(MCP2510, address);
   spi_xfer(MCP2510, mask);
   spi_xfer(MCP2510, data);
   
   output_high(EXT_CAN_CS);
}
