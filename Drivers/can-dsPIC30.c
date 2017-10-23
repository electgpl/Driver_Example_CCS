///////////////////////////////////////////////////////////////////////////
////                        can-dsPIC30.c                              ////
//// CAN Library routines for Microchip's dsPIC30F401x, dsPIC30F501x   ////
////   and dsPIC30F601xA lines                                         ////
////                                                                   ////
//// This library provides the following functions:                    ////
////     (for more information on these functions see the comment      ////
////      header above each function)                                  ////
////                                                                   ////
////    can_init - Configures the dsPIC30F CAN1 peripheral             ////
////    can2_init - Configures the dsPIC30F CAN2 peripheral            ////
////                                                                   ////
////    can_set_baud - Sets the baud rate control registers for CAN1   ////
////    can2_set_baud - Sets the baud rate control registers for CAN2  ////
////                                                                   ////
////    can_set_mode - Sets the CAN1 module into a specific mode       ////
////    can2_set_mode - Sets the CAN2 module into a specific mode      ////
////                                                                   ////
////    can_set_id - Sets the standard and extended filter and filter  ////
////        mask ID's for CAN1 module                                  ////
////    can2_set_id - Sets the standard and extended filter and filter ////
////        mask ID's for CAN2 module                                  ////
////                                                                   ////
////    can_set_transmit_id - Sets the standard and extended transmit  ////
////        buffer ID for CAN1 and CAN2 modules                        ////
////                                                                   ////
////    can_get_id - Gets the standard and extended ID                 ////
////                                                                   ////
////    can_putd - Sends a message/request with specified ID for       ////
////        CAN1 module                                                ////
////    can2_putd - Sends a message/request with specified ID for      ////
////        CAN2 module                                                ////
////                                                                   ////
////    can_getd - Returns specifid message/request and ID for CAN1    ////
////        module                                                     ////
////    can2_getd - Returns specifid message/request and ID for CAN2   ////
////        module                                                     ////
////                                                                   ////
////    can_kbhit - Returns true if there is data in one of the        ////
////        receive buffers for CAN1 module                            ////
////    can2_kbhit - Returns true if there is data in one of the       ////
////        receive buffers for CAN2 module                            ////
////                                                                   ////
////    can_tbe - Returns true if the transmit buffer is ready to      ////
////        send more data for CAN1 module                             ////
////    can2_tbe - Returns true if the transmit buffer is ready to     ////
////        send more data for CAN2 module                             ////
////                                                                   ////
////    can_abort - Aborts all pending transmissions for CAN1 module   ////
////    can2_abort - Aborts all pending transmissions for CAN2 module  ////
////                                                                   ////
////    can_enable_interrupts - enables specified interrupt source     ////
////        for #INT_CAN1                                              ////
////    can2_enable_interrupts - enables specified interrupt source    ////
////        for #INT_CAN2                                              ////
////                                                                   ////
////    can_disable_interrupts - disables specified interrupt source   ////
////        for #INT_CAN1                                              ////
////    can2_disable_interrupts - disables specified interrupts source ////
////        for #INT_CAN2                                              ////
////                                                                   ////
////    PIN_F0 is C1RX, PIN_F1 is C1TX, PIN_G0 is C2RX, and PIN_G1 is  ////
////    C2TX.  You will need a CAN transeiver to connect these pins to ////
////    CANH and CANL bus lines.                                       ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2008 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS    ////
//// C compiler.  This source code may only be distributed to other    ////
//// licensed users of the CCS C compiler.  No other use,              ////
//// reproduction or distribution is permitted without written         ////
//// permission.  Derivative programs created using this software      ////
//// in object code form are not restricted in any way.                ////
///////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <can-dsPIC30.h>

#if CAN_DO_DEBUG
 #define can_debug printf
#else
 #define can_debug
#endif


//macros CAN1
#define can_kbhit()                 (C1RX0CON.rxful || C1RX1CON.rxful)
#define can_tbe()                   (!C1TX0CON.txreq || !C1TX1CON.txreq || !C1TX2CON.txreq)
#define can_abort()                 (C1CTRL.abat=1)

//macros CAN2
#if getenv("SFR_VALID:C2CTRL")
   #define can2_kbhit()                 (C2RX0CON.rxful || C2RX1CON.rxful)
   #define can2_tbe()                   (!C2TX0CON.txreq || !C2TX1CON.txreq || !C2TX2CON.txreq)
   #define can2_abort()                 (C2CTRL.abat=1)
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_init()
// can2_init()
//
// Initializes dsPIC CAN peripheral.  Sets the RX filter and masks so the
// CAN peripheral will receive all incoming IDs.  Configures both RX buffers
// to only accept valid valid messages (as opposed to all messages, or all
// extended message, or all standard messages).
//
// The constants (CAN_USE_RX_DOUBLE_BUFFER and CAN_ENABLE_CAN_CAPTURE)
// are given a default define in the can-dsPIC.h file.  These default
// values can be overwritten in the main code, but most applications
// will be fine with these defaults.
//
////////////////////////////////////////////////////////////////////////////////
void can_init(void) {
   uint16_t *ptr;
   can_set_mode(CAN_OP_CONFIG);   //must be in config mode before params can be set
   can_set_baud();
   
   C1CTRL.cancks=0;

   ptr = &C1RX0CON;
   //C1RX0CON=0;
   *ptr = 0;
   C1RX0CON.dben=CAN_USE_RX_DOUBLE_BUFFER;
  
   C1CTRL.cancap=CAN_ENABLE_CAN_CAPTURE;
   
   can_set_id(&C1RXM0, CAN_MASK_ACCEPT_ALL, CAN_MASK_ACCEPT_ALL);  //set mask 0
   can_set_id(&C1RXF0, 0, CAN_USE_EXTENDED_ID);  //set filter 0 of mask 0
   can_set_id(&C1RXF1, 0, CAN_USE_EXTENDED_ID);  //set filter 1 of mask 0

   can_set_id(&C1RXM1, CAN_MASK_ACCEPT_ALL, CAN_MASK_ACCEPT_ALL);  //set mask 1
   can_set_id(&C1RXF2, 0, CAN_USE_EXTENDED_ID);  //set filter 0 of mask 1
   can_set_id(&C1RXF3, 0, CAN_USE_EXTENDED_ID);  //set filter 1 of mask 1
   can_set_id(&C1RXF4, 0, CAN_USE_EXTENDED_ID);  //set filter 2 of mask 1
   can_set_id(&C1RXF5, 0, CAN_USE_EXTENDED_ID);  //set filter 3 of mask 1

   can_set_mode(CAN_OP_NORMAL);
}

#if getenv("SFR_VALID:C2CTRL")
   void can2_init(void) {
      uint16_t *ptr;
      can2_set_mode(CAN_OP_CONFIG);   //must be in config mode before params can be set
      can2_set_baud();
      
      C2CTRL.cancks=0;
   
      ptr = &C2RX0CON;
      //C2RX0CON=0;
      *ptr = 0;
      C2RX0CON.dben=CAN_USE_RX_DOUBLE_BUFFER;
     
      C2CTRL.cancap=CAN_ENABLE_CAN_CAPTURE;
      
      can2_set_id(&C2RXM0, CAN_MASK_ACCEPT_ALL, CAN_MASK_ACCEPT_ALL);  //set mask 0
      can2_set_id(&C2RXF0, 0, CAN_USE_EXTENDED_ID);  //set filter 0 of mask 0
      can2_set_id(&C2RXF1, 0, CAN_USE_EXTENDED_ID);  //set filter 1 of mask 0
   
      can2_set_id(&C2RXM1, CAN_MASK_ACCEPT_ALL, CAN_MASK_ACCEPT_ALL);  //set mask 1
      can2_set_id(&C2RXF2, 0, CAN_USE_EXTENDED_ID);  //set filter 0 of mask 1
      can2_set_id(&C2RXF3, 0, CAN_USE_EXTENDED_ID);  //set filter 1 of mask 1
      can2_set_id(&C2RXF4, 0, CAN_USE_EXTENDED_ID);  //set filter 2 of mask 1
      can2_set_id(&C2RXF5, 0, CAN_USE_EXTENDED_ID);  //set filter 3 of mask 1
   
      can2_set_mode(CAN_OP_NORMAL);
   }
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_set_baud()
// can2_set_baud()
//
// Configures the baud rate control registers.  All the defines here
// are defaulted in the can-dsPIC.h file.  These defaults can, and
// probably should, be overwritten in the main code.
//
// Current defaults are set to work with Microchip's MCP250xxx CAN
// Developers Kit if this PIC is running at 20Mhz.
//
////////////////////////////////////////////////////////////////////////////////
void can_set_baud(void) {
   C1CFG1.brp=CAN_BRG_PRESCALAR;
   C1CFG1.sjw=CAN_BRG_SYNCH_JUMP_WIDTH;

   C1CFG2.prseg=CAN_BRG_PROPAGATION_TIME;
   C1CFG2.seg1ph=CAN_BRG_PHASE_SEGMENT_1;
   C1CFG2.sam=CAN_BRG_SAM;
   C1CFG2.seg2phts=CAN_BRG_SEG_2_PHASE_TS;

   C1CFG2.seg2ph=CAN_BRG_PHASE_SEGMENT_2;
   C1CFG2.wakfil=CAN_BRG_WAKE_FILTER;
}

#if getenv("SFR_VALID:C2CTRL")
   void can2_set_baud(void) {
      C2CFG1.brp=CAN2_BRG_PRESCALAR;
      C2CFG1.sjw=CAN2_BRG_SYNCH_JUMP_WIDTH;
   
      C2CFG2.prseg=CAN2_BRG_PROPAGATION_TIME;
      C2CFG2.seg1ph=CAN2_BRG_PHASE_SEGMENT_1;
      C2CFG2.sam=CAN2_BRG_SAM;
      C2CFG2.seg2phts=CAN2_BRG_SEG_2_PHASE_TS;
   
      C2CFG2.seg2ph=CAN2_BRG_PHASE_SEGMENT_2;
      C2CFG2.wakfil=CAN2_BRG_WAKE_FILTER;
   }
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_set_mode()
// can2_set_mode()
//
// This function first sets the three bits (reqop2:reqop0) to the mode given 
// through the argument.
//
// The reqop bits do not immediatly change the mode of operation, the
// opmode bits in the C1CTRL1 or C2CTRL1 register (opmode2:opmode0) must change 
// to reflect the actuall change in mode, therefore a while statement is 
// used to check if the C1CTRL1 or C2CTRL1 opmode bits have changed to reflect 
// the change in mode.
//
////////////////////////////////////////////////////////////////////////////////
void can_set_mode(CAN_OP_MODE mode) {
   C1CTRL.reqop=mode;
   while( (C1CTRL.opmode) != mode );
}

#if getenv("SFR_VALID:C2CTRL")
   void can2_set_mode(CAN_OP_MODE mode) {
      C2CTRL.reqop=mode;
      while( (C2CTRL.opmode) != mode );
   }
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_set_id()
// can2_set_id()
//
// Configures the xxxxEIDL, xxxxEIDH, and xxxxSID registers to
// configure the defined buffer to use the specified ID
//
//   Paramaters:
//     addr - pointer to first byte of ID register, starting with xxxxEIDL.
//            For example, a pointer to C1RXM1
//     id - ID to set buffer to
//     ext - Set to TRUE if this buffer uses an extended ID, FALSE if not
//
////////////////////////////////////////////////////////////////////////////////
void can_set_id(uint16_t* addr, uint32_t id, int1 ext) {
   uint16_t *ptr;
   uint16_t temp;

   ptr=addr;
   
   if (ext) {  //extended
      //eidl
      temp=((id & 0x3F) << 10) & 0xFC00;
      *ptr=temp;                                   //0:5

      //eidh
      ptr-=1;
      temp=((id & 0x3FFC0) >> 6) & 0x0FFF;
      *ptr=temp;                                   //6:17

      //sid
      ptr-=1;
      temp=(uint16_t)(((id & 0x1FFC0000) >> 16) & 0x1FFC) | ((uint16_t)ext & 0x0001);
      *ptr=temp;                                  //18:28
   }
   else {   //standard
      //eidl
      *ptr=0;

      //eidh
      ptr-=1;
      *ptr=0;

      //sid
      ptr-=1;
      temp=(uint16_t)(((id & 0x7FF) << 2) & 0x1FFC) | ((uint16_t)ext & 0x0001);
      *ptr=temp;                                  //0:10
   }
}

#if getenv("SFR_VALID:C2CTRL")
   void can2_set_id(uint16_t* addr, uint32_t id, int1 ext) {
      uint16_t *ptr;
      uint16_t temp;
   
      ptr=addr;
      
      if (ext) {  //extended
         //eidl
         temp=((id & 0x3F) << 10) & 0xFC00;
         *ptr=temp;                                   //0:5
   
         //eidh
         ptr-=1;
         temp=((id & 0x3FFC0) >> 6) & 0x0FFF;
         *ptr=temp;                                   //6:17
   
         //sid
         ptr-=1;
         temp=(uint16_t)(((id & 0x1FFC0000) >> 16) & 0x1FFC) | ((uint16_t)ext & 0x0001);
         *ptr=temp;                                  //18:28
      }
      else {   //standard
         //eidl
         *ptr=0;
   
         //eidh
         ptr-=1;
         *ptr=0;
   
         //sid
         ptr-=1;
         temp=(uint16_t)(((id & 0x7FF) << 2) & 0x1FFC) | ((uint16_t)ext & 0x0001);
         *ptr=temp;                                  //0:10
      }
   }
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_set_transmit_id
//
// Configures the CxTXxDLC, CxTXxEID, and CxTXxSID registers to
// configure the defined buffer to use the specified ID
//
//   Paramaters:
//     addr - pointer to first byte of ID register, starting with CxTXxDLC.
//            For example, a pointer to C1TX1
//     id - ID to set buffer to
//     ext - Set to TRUE if this buffer uses an extended ID, FALSE if not
//
////////////////////////////////////////////////////////////////////////////////
void can_set_transmit_id(uint16_t* addr, uint32_t id, int1 ext) {
   uint16_t *ptr;
   uint16_t temp,temp1;
   ptr=addr;
   
   if (ext) {  //extended
      //eidl
      *ptr&=0x0278;
      temp=(((id & 0x3F) <<10) & 0xFC00);
      *ptr|=temp;                                  //0:5

      //eidh
      ptr-=1;
      temp=(((id & 0x3FC0) >> 6) & 0x00FF);
      temp1=(((id & 0x3C000) >> 2 ) & 0xF000);
      *ptr=(temp1 | temp);                                   //6:17
                  
      //sid
      ptr-=1;
      *ptr&=0x0003;
      temp=(((id & 0xFC0000) >> 16) & 0xFC);
      temp1=(((id & 0x1F000000) >> 13) & 0xF800);
      *ptr|=(temp | temp1);                                  //18:28
   }
   else {   //standard
      //dlc
      *ptr&=0x0278;

      //eid
      ptr-=1;
      *ptr=0;

      //sid
      ptr-=1;
      *ptr&=0x0003;
      temp=((id & 0x3F) << 2) & 0x00FC;
      temp1=((id & 0x7C0) << 5) & 0xF800;
      *ptr|=(temp | temp1);                                  //0:10
   }
}

////////////////////////////////////////////////////////////////////////////////
//
// can_get_id()
//
// Returns the ID of the specified buffer. (The opposite of can_set_transmit_id())
// This is used after receiving a message, to see which ID sent the message.
//
//   Paramaters:
//     addr - pointer to first byte of ID register, starting with xxxxEIDL.
//            For example, a pointer to C1RX0
//     ext - Set to TRUE if this buffer uses an extended ID, FALSE if not
//
//   Returns:
//     The ID of the buffer
//
////////////////////////////////////////////////////////////////////////////////
uint32_t can_get_id(uint16_t *addr, int1 ext) {
   uint32_t ret;
   uint16_t *ptr;

   ret=0;
   ptr=addr;

   if (ext) {
      ret=(*ptr >> 10) & 0x3F;  //eidl

      ptr-=1;     //eidh
      ret|=((uint32_t)*ptr << 6);

      ptr-=1;     //sid
      ret|=((uint32_t)*ptr & 0x1ffc) << 16;
   }
   else {
      ptr-=2;    //sid
      ret=((*ptr & 0x1ffc) >> 2) & 0x7FF;
   }

   return(ret);
}

////////////////////////////////////////////////////////////////////////////////
//
// can_putd()
// can2_putd()
//
// Puts data on a transmit buffer, at which time the CAN peripheral will
// send when the CAN bus becomes available.
//
//    Paramaters:
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
////////////////////////////////////////////////////////////////////////////////
int1 can_putd(uint32_t id, uint8_t *data, uint8_t len, uint8_t priority, int1 ext, int1 rtr) {
   uint16_t i;
   uint16_t *ptr;
   uint16_t port;

    // find emtpy transmitter
    //map access bank addresses to empty transmitter
   if (!C1TX0CON.txreq) 
   {
      ptr=&C1TX0SID;
      port=0;
   }
   else if (!C1TX1CON.txreq) 
   {
      ptr=&C1TX1SID;
      port=1;
   }
   else if (!C1TX2CON.txreq) 
   {
      ptr=&C1TX2SID;
      port=2;
   }
   else {
      #if CAN_DO_DEBUG
         can_debug("\r\nCAN_PUTD() FAIL: NO OPEN TX BUFFERS\r\n");
      #endif
      return(0);
   }
   
   *ptr=(((uint16_t)rtr << 1) | ((uint16_t)ext & 0x0001));
   ptr+=2;
   
   *ptr=(((uint16_t)rtr << 9) | ((uint16_t)len << 3));
   can_set_transmit_id(ptr,id,ext);
   ptr++;
   
   for(i=0;i<len;i+=2)
   {
      if (i+1==len)
         *ptr=make16(0,data[i]);
      else
         *ptr=make16(data[i+1],data[i]);
      ptr++;
   }
   
   switch(port)
   {
      case 0:
         C1TX0CON.txpri=priority;
         C1TX0CON.txreq=1;
         break;
      case 1:
         C1TX1CON.txpri=priority;
         C1TX1CON.txreq=1;
         break;
      case 2:
         C1TX2CON.txpri=priority;
         C1TX2CON.txreq=1;
         break;
   }         

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

#if getenv("SFR_VALID:C2CTRL")
   int1 can2_putd(uint32_t id, uint8_t *data, uint8_t len, uint8_t priority, int1 ext, int1 rtr) {
      uint16_t i;
      uint16_t *ptr;
      uint16_t port;
   
       // find emtpy transmitter
       //map access bank addresses to empty transmitter
      if (!C2TX0CON.txreq) 
      {
         ptr=&C2TX0SID;
         port=0;
      }
      else if (!C2TX1CON.txreq) 
      {
         ptr=&C2TX1SID;
         port=1;
      }
      else if (!C2TX2CON.txreq) 
      {
         ptr=&C2TX2SID;
         port=2;
      }
      else {
         #if CAN_DO_DEBUG
            can_debug("\r\nCAN2_PUTD() FAIL: NO OPEN TX BUFFERS\r\n");
         #endif
         return(0);
      }
      
      *ptr=(((uint16_t)rtr << 1) | ((uint16_t)ext & 0x0001));
      ptr+=2;
      
      *ptr=(((uint16_t)rtr << 9) | ((uint16_t)len << 3));
      can_set_transmit_id(ptr,id,ext);
      ptr++;
      
      for(i=0;i<len;i+=2)
      {
         if (i+1==len)
            *ptr=make16(0,data[i]);
         else
            *ptr=make16(data[i+1],data[i]);
         ptr++;
      }
      
      switch(port)
      {
         case 0:
            C2TX0CON.txpri=priority;
            C2TX0CON.txreq=1;
            break;
         case 1:
            C2TX1CON.txpri=priority;
            C2TX1CON.txreq=1;
            break;
         case 2:
            C2TX2CON.txpri=priority;
            C2TX2CON.txreq=1;
            break;
      }         
   
      #if CAN_DO_DEBUG
               can_debug("\r\nCAN2_PUTD(): BUFF=%U ID=%LX LEN=%U PRI=%U EXT=%U RTR=%U\r\n", port, id, len, priority, ext, rtr);
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
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_getd()
// can2_getd()
//
// Gets data from a receive buffer, if the data exists
//
//    Returns:
//      id - ID who sent message
//      data - pointer to array of data
//      len - length of received data
//      stat - structure holding some information (such as which buffer
//             recieved it, ext or standard, etc)
//
//    Returns:
//      Function call returns a TRUE if there was data in a RX buffer, FALSE
//      if there was none.
//
////////////////////////////////////////////////////////////////////////////////
int1 can_getd(uint32_t &id, uint8_t *data, uint8_t &len, struct rx_stat &stat)
{
   uint16_t i;
   uint16_t *ptr;
   int1 ext;
   
   if (C1RX0CON.rxful) 
   {
      stat.buffer=0;
      C1INTF.rx0if=0;
      stat.err_ovfl=C1INTF.rx0ovr;
      C1INTF.rx0ovr=0;
      if (C1RX0CON.dben) 
      {
         stat.filthit=C1RX0CON.filhit0;
      }
      len=C1RX0.dlc;
      stat.rtr=C1RX0CON.rxrtrro;
      
      ext=C1RX0SID.rxide;
      stat.ext=ext;
      
      ptr=&C1RX0;
   }
   else if (C1RX1CON.rxful)
   {
      stat.buffer=1;
      C1INTF.rx1if=0;
      stat.err_ovfl=C1INTF.rx1ovr;
      C1INTF.rx1ovr=0;
      stat.filthit=C1RX1CON.filhit;
      len=C1RX1.dlc;
      stat.rtr=C1RX1CON.rxrtrro;
      
      ext=C1RX1SID.rxide;
      stat.ext=ext;
      
      ptr=&C1RX1;
   }
   else {
      #if CAN_DO_DEBUG
         can_debug("\r\nFAIL ON CAN_GETD(): NO MESSAGE IN BUFFER\r\n");
      #endif
      return (0);
   }
    
   id=can_get_id(ptr,ext);
    
   ptr++;
   for(i=0;i<len;i+=2)
   {
      data[i]=make8(*ptr,0);
      if (i+1!=len)
         data[i+1]=make8(*ptr,1);
      ptr++;
   }
   
   stat.inv=C1INTF.ivrif;
   C1INTF.ivrif=0;
   
   if (stat.buffer==0) 
   {
      C1RX0CON.rxful=0;
   }
   else 
   {
      C1RX1CON.rxful=0;
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

#if getenv("SFR_VALID:C2CTRL")
   int1 can2_getd(uint32_t &id, uint8_t *data, uint8_t &len, struct rx_stat &stat)
   {
      uint16_t i;
      uint16_t *ptr;
      int1 ext;
      
      if (C2RX0CON.rxful) 
      {
         stat.buffer=0;
         C2INTF.rx0if=0;
         stat.err_ovfl=C2INTF.rx0ovr;
         C2INTF.rx0ovr=0;
         if (C2RX0CON.dben) 
         {
            stat.filthit=C2RX0CON.filhit0;
         }
         len=C2RX0.dlc;
         stat.rtr=C2RX0CON.rxrtrro;
         
         ext=C2RX0SID.rxide;
         stat.ext=ext;
         
         ptr=&C2RX0;
      }
      else if (C2RX1CON.rxful)
      {
         stat.buffer=1;
         C2INTF.rx1if=0;
         stat.err_ovfl=C2INTF.rx1ovr;
         C2INTF.rx1ovr=0;
         stat.filthit=C2RX1CON.filhit;
         len=C2RX1.dlc;
         stat.rtr=C2RX1CON.rxrtrro;
         
         ext=C2RX1SID.rxide;
         stat.ext=ext;
         
         ptr=&C2RX1;
      }
      else {
         #if CAN_DO_DEBUG
            can_debug("\r\nFAIL ON CAN2_GETD(): NO MESSAGE IN BUFFER\r\n");
         #endif
         return (0);
      }
       
      id=can_get_id(ptr,ext);
       
      ptr++;
      for(i=0;i<len;i+=2)
      {
         data[i]=make8(*ptr,0);
         if (i+1!=len)
            data[i+1]=make8(*ptr,1);
         ptr++;
      }
      
      stat.inv=C2INTF.ivrif;
      C2INTF.ivrif=0;
      
      if (stat.buffer==0) 
      {
         C2RX0CON.rxful=0;
      }
      else 
      {
         C2RX1CON.rxful=0;
      }
   
      #if CAN_DO_DEBUG
         can_debug("\r\nCAN2_GETD(): BUFF=%U ID=%LX LEN=%U OVF=%U ", stat.buffer, id, len, stat.err_ovfl);
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
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_enable_interrupts()
// can2_enable_interrupts()
//
//    Selects which interrupt sources to use with #INT_CAN1 and #INT_CAN2 
//    interrupts 
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
////////////////////////////////////////////////////////////////////////////////

void can_enable_interrupts(INTERRUPT setting)
{
   uint16_t temp;
   uint16_t ptr;
   
   ptr = &C1INTE;
   
   //temp=C1INTE;
   //C1INTE=(temp | setting);
   temp = *ptr;
   *ptr = (temp | setting);
}

#if getenv("SFR_VALID:C2INTE")
   void can2_enable_interrupts(INTERRUPT setting)
   {
      uint16_t temp;
      uint16_t *ptr;
      
      ptr = &C2INTE;
      
      //temp=C2INTE;
      //C2INTE=(temp | setting);
      temp = *ptr;
      *ptr = (temp | setting);
   }
#endif

////////////////////////////////////////////////////////////////////////////////
//
// can_disable_interrupts()
// can2_disable_interrupts()
//
//    Disables specifed interrupt sources that are use with #INT_CAN1 and 
//    #INT_CAN2 interrupts 
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
////////////////////////////////////////////////////////////////////////////////

void can_disable_interrupts(INTERRUPT setting)
{
   uint16_t temp;
   uint16_t *ptr;
   
   ptr = &C1INTE;
   
   //temp=C1INTE;
   temp = *ptr;
   temp&=(setting^0xFF);
   
   //C1INTE=temp;
   *ptr = temp;
}

#if getenv("SFR_VALID:C2INTE")
   void can2_disable_interrupts(INTERRUPT setting)
   {
      uint16_t temp;
      uint16_t *ptr;
      
      ptr = &C2INTE;
      
      //temp=C2INTE;
      temp = *ptr;
      temp&=(setting^0xFF);
      
      //C2INTE=temp;
      *ptr = temp;
   }
#endif
