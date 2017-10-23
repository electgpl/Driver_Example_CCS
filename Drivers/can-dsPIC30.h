/////////////////////////////////////////////////////////////////////////
////                        can-dsPIC30.h                            ////
////                                                                 ////
//// Prototypes, definitions, defines and macros used for and with   ////
//// the CCS CAN library for Microchip's dsPIC30F401x, dsPIC30F501x  ////
////   and dsPIC30F601xA lines                                       ////
////                                                                 ////
//// (see can-dsPIC30.c)                                             ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2008 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#ifndef __CCS_CANdsPIC30_LIB_DEFINES__
#define __CCS_CANdsPIC30_LIB_DEFINES__

#ifndef CAN_DO_DEBUG
 #define CAN_DO_DEBUG FALSE
#endif

#IFNDEF CAN_USE_EXTENDED_ID
  #define CAN_USE_EXTENDED_ID         TRUE
#ENDIF

#IFNDEF CAN_BRG_SYNCH_JUMP_WIDTH
  #define CAN_BRG_SYNCH_JUMP_WIDTH  0  //synchronized jump width (def: 1 x Tq)
#ENDIF

#IFNDEF CAN_BRG_PRESCALAR
  #define CAN_BRG_PRESCALAR  4  //baud rate generator prescalar (def: 4) ( Tq = (2 x (PRE + 1))/Fosc )
#ENDIF

#ifndef CAN_BRG_SEG_2_PHASE_TS
 #define CAN_BRG_SEG_2_PHASE_TS   TRUE //phase segment 2 time select bit (def: freely programmable)
#endif

#ifndef CAN_BRG_SAM
 #define CAN_BRG_SAM 0 //sample of the can bus line (def: bus line is sampled 1 times prior to sample point)
#endif

#ifndef CAN_BRG_PHASE_SEGMENT_1
 #define CAN_BRG_PHASE_SEGMENT_1  5 //phase segment 1 (def: 6 x Tq)
#endif

#ifndef CAN_BRG_PROPAGATION_TIME
 #define CAN_BRG_PROPAGATION_TIME 2 //propagation time select (def: 3 x Tq)
#endif

#ifndef CAN_BRG_WAKE_FILTER
 #define CAN_BRG_WAKE_FILTER FALSE   //selects can bus line filter for wake up bit
#endif

#ifndef CAN_BRG_PHASE_SEGMENT_2
 #define CAN_BRG_PHASE_SEGMENT_2 5 //phase segment 2 time select (def: 6 x Tq)
#endif

#if getenv("SFR_VALID:C2CTRL")
   #IFNDEF CAN2_BRG_SYNCH_JUMP_WIDTH
     #define CAN2_BRG_SYNCH_JUMP_WIDTH  0  //synchronized jump width (def: 1 x Tq)
   #ENDIF
   
   #IFNDEF CAN2_BRG_PRESCALAR
     #define CAN2_BRG_PRESCALAR  4  //baud rate generator prescalar (def: 4) ( Tq = (2 x (PRE + 1))/Fosc )
   #ENDIF
   
   #ifndef CAN2_BRG_SEG_2_PHASE_TS
    #define CAN2_BRG_SEG_2_PHASE_TS   TRUE //phase segment 2 time select bit (def: freely programmable)
   #endif
   
   #ifndef CAN2_BRG_SAM
    #define CAN2_BRG_SAM 0 //sample of the can bus line (def: bus line is sampled 1 times prior to sample point)
   #endif
   
   #ifndef CAN2_BRG_PHASE_SEGMENT_1
    #define CAN2_BRG_PHASE_SEGMENT_1  5 //phase segment 1 (def: 6 x Tq)
   #endif
   
   #ifndef CAN2_BRG_PROPAGATION_TIME
    #define CAN2_BRG_PROPAGATION_TIME 2 //propagation time select (def: 3 x Tq)
   #endif
   
   #ifndef CAN2_BRG_WAKE_FILTER
    #define CAN2_BRG_WAKE_FILTER FALSE   //selects can bus line filter for wake up bit
   #endif
   
   #ifndef CAN2_BRG_PHASE_SEGMENT_2
    #define CAN2_BRG_PHASE_SEGMENT_2 5 //phase segment 2 time select (def: 6 x Tq)
   #endif
#endif

#ifndef CAN_USE_RX_DOUBLE_BUFFER
 #define CAN_USE_RX_DOUBLE_BUFFER TRUE   //if buffer 0 overflows, do NOT use buffer 1 to put buffer 0 data
#endif

#ifndef CAN_ENABLE_CAN_CAPTURE
 #define CAN_ENABLE_CAN_CAPTURE 0
#endif

////////////////////////////////////////////////////////////////////////////////
/////////////////////// CAN Control & Status Register //////////////////////////
////////////////////////////////////////////////////////////////////////////////

enum CAN_OP_MODE {   CAN_OP_LISTEN_ALL=7,   
                     CAN_OP_CONFIG=4,
                     CAN_OP_LISTEN=3,
                     CAN_OP_LOOPBACK=2,
                     CAN_OP_DISABLE=1,
                     CAN_OP_NORMAL=0 };

enum CAN_INT_CODE {   CAN_INT_WAKEUP=7,
                     CAN_INT_RX0=6,
                     CAN_INT_RX1=5,
                     CAN_INT_TX0=4,
                     CAN_INT_TX1=3,
                     CAN_INT_TX2=2,
                     CAN_INT_ERROR=1,
                     CAN_INT_NO=0};
                     
// CAN Control & Status register configurations
struct CiCTRL_struct{ 
   int1 void0;             //0
   CAN_INT_CODE icode:3;   //1:3    interrupt code
   int1 void4;             //4
   CAN_OP_MODE opmode:3;   //5:7    operation mode status
   CAN_OP_MODE reqop:3;    //8:10   request can operation mode bits
   int1 cancks;            //11     can master clock
   int1 abat;               //12     abort all pending transmissions
   int1 csidl;              //13     stop in idle mode
   int1 void14;            //14
   int1 cancap;            //15     can message receive capture enable
};
struct CiCTRL_struct C1CTRL;

#word C1CTRL=getenv("SFR:C1CTRL")

#if getenv("SFR_VALID:C2CTRL")
   struct CiCTRL_struct C2CTRL;
   #word C2CTRL=getenv("SFR:C2CTRL")
#endif

////////////////////////////////////////////////////////////////////////////////
//////////////////////// CAN Transmit Buffer Registers /////////////////////////
////////////////////////////////////////////////////////////////////////////////

enum CAN_TXPRI_CODE  {  CAN_TXPRI_HIGHEST=3,
                        CAN_TXPRI_HIGH=2,
                        CAN_TXPRI_LOW=1,
                        CAN_TXPRI_LOWEST=0 };

// Transmit Buffer Status and Control Register
struct CiTXnCON_struct {
   CAN_TXPRI_CODE txpri:2; //0:1    message transmission priority
   int1 void2;             //2
   int1 txreq;             //3      message send request bit
   int1 txerr;             //4      error detected during transmission bit
   int1 txlarb;            //5      message lost arbitration bit
   int1 txabt;             //6      massage aborted bit 
   int void15_7:9;         //7:15
};
struct CiTXnCON_struct C1TX0CON;
struct CiTXnCON_struct C1TX1CON;
struct CiTXnCON_struct C1TX2CON;

#word C1TX0CON=getenv("SFR:C1TX0CON")
#word C1TX1CON=getenv("SFR:C1TX1CON")
#word C1TX2CON=getenv("SFR:C1TX2CON")

#if getenv("SFR_VALID:C2TX0CON")
   struct CiTXnCON_struct C2TX0CON;
   struct CiTXnCON_struct C2TX1CON;
   struct CiTXnCON_struct C2TX2CON;
   #word C2TX0CON=getenv("SFR:C2TX0CON")
   #word C2TX1CON=getenv("SFR:C2TX1CON")
   #word C2TX2CON=getenv("SFR:C2TX2CON")
#endif

// Transmit Buffer n Standard Identifier
#word C1TX0SID=getenv("SFR:C1TX0SID")
#word C1TX1SID=getenv("SFR:C1TX1SID")
#word C1TX2SID=getenv("SFR:C1TX2SID")

#if getenv("SFR_VALID:C2TX0SID")
   #word C2TX0SID=getenv("SFR:C2TX0SID")
   #word C2TX1SID=getenv("SFR:C2TX1SID")
   #word C2TX2SID=getenv("SFR:C2TX2SID")
#endif

// Transmit Buffer n Extended Identifier
#word C1TX0EID=getenv("SFR:C1TX0EID")
#word C1TX1EID=getenv("SFR:C1TX1EID")
#word C1TX2EID=getenv("SFR:C1TX2EID")

#if getenv("SFR_VALID:C2TX0EID")
   #word C2TX0EID=getenv("SFR:C2TX0EID")
   #word C2TX1EID=getenv("SFR:C2TX1EID")
   #word C2TX2EID=getenv("SFR:C2TX2EID")
#endif

#word C1TX0=getenv("SFR:C1TX0DLC")
#word C1TX1=getenv("SFR:C1TX1DLC")
#word C1TX2=getenv("SFR:C1TX2DLC")

#if getenv("SFR_VALID:C2TX0DLC")
   #word C2TX0=getenv("SFR:C2TX0DLC")
   #word C2TX1=getenv("SFR:C2TX1DLC")
   #word C2TX2=getenv("SFR:C2TX2DLC")
#endif

// Transmit Buffer n Data Field Word m
#word C1TX0B1=getenv("SFR:C1TX0B1")
#word C1TX0B2=getenv("SFR:C1TX0B2")
#word C1TX0B3=getenv("SFR:C1TX0B3")
#word C1TX0B4=getenv("SFR:C1TX0B4")
#word C1TX1B1=getenv("SFR:C1TX1B1")
#word C1TX1B2=getenv("SFR:C1TX1B2")
#word C1TX1B3=getenv("SFR:C1TX1B3")
#word C1TX1B4=getenv("SFR:C1TX1B4")
#word C1TX2B1=getenv("SFR:C1TX2B1")
#word C1TX2B2=getenv("SFR:C1TX2B2")
#word C1TX2B3=getenv("SFR:C1TX2B3")
#word C1TX2B4=getenv("SFR:C1TX2B4")

#if getenv("SFR_VALID:C2TX0B1")
   #word C2TX0B1=getenv("SFR:C2TX0B1")
   #word C2TX0B2=getenv("SFR:C2TX0B2")
   #word C2TX0B3=getenv("SFR:C2TX0B3")
   #word C2TX0B4=getenv("SFR:C2TX0B4")
   #word C2TX1B1=getenv("SFR:C2TX1B1")
   #word C2TX1B2=getenv("SFR:C2TX1B2")
   #word C2TX1B3=getenv("SFR:C2TX1B3")
   #word C2TX1B4=getenv("SFR:C2TX1B4")
   #word C2TX2B1=getenv("SFR:C2TX2B1")
   #word C2TX2B2=getenv("SFR:C2TX2B2")
   #word C2TX2B3=getenv("SFR:C2TX2B3")
   #word C2TX2B4=getenv("SFR:C2TX2B4")
#endif

////////////////////////////////////////////////////////////////////////////////
///////////////////////// CAN Receive Buffer Registers /////////////////////////
////////////////////////////////////////////////////////////////////////////////

enum CAN_FILHIT_CODE {  RXF5=5, 
                        RXF4=4,
                        RXF3=3,
                        RXF2=2,
                        RXF1=1,        // only possible when DBEN bit is set
                        RXF0=0 };      // only possible when DBEN bit is set

// Receive Buffer 0 Status & Control Register
struct CiRX0CON_struct {
   int1 filhit0;     //0   indicates which acceptance filter enabled the message reception bit
   int1 jtoff;       //1   jump table offset bit
   int1 dben;        //2   receive buffer 0 double buffer enable bit
   int1 rxrtrro;     //3   received remote transer request bit
   int void654:3;    //4:6 
   int1 rxful;       //7   receive full status bit
   int void15_8:8;   //8:15
};
struct CiRX0CON_struct C1RX0CON;

#word C1RX0CON=getenv("SFR:C1RX0CON")

#if getenv("SFR_VALID:C2RX0CON")
   struct CiRX0CON_struct C2RX0CON;
   #word C2RX0CON=getenv("SFR:C2RX0CON")
#endif

// Receive Buffer 1 Status & Control Register
struct CiRX1CON_struct {
   CAN_FILHIT_CODE filhit:3;  //0:2 indicates which acceptance filter enabled the message reception bits
   int1 rxrtrro;              //3   received remote transfer request bit
   int void654:3;             //4:6 
   int1 rxful;                //7   receive full status bit
   int void8_15:8;            //8:15
};
struct CiRX1CON_struct C1RX1CON;

#word C1RX1CON=getenv("SFR:C1RX1CON")

#if getenv("SFR_VALID:C2RX1CON")
   struct CiRX1CON_struct C2RX1CON;
   #word C2RX1CON=getenv("SFR:C2RX1CON")
#endif

// Receive Buffer n Standard Identifier
struct CiRXnSID_struct {
   int1 rxide;                //0      extended identifier flag bit
   int1 srr;                  //1      substitute remote request bit
   int sid:11;                //2:12   standard identifier bits
   int void15_13:3;           //13:15
};
struct CiRXnSID_struct C1RX0SID;
struct CiRXnSID_struct C1RX1SID;

#word C1RX0SID=getenv("SFR:C1RX0SID")
#word C1RX1SID=getenv("SFR:C1RX1SID")

#if getenv("SFR_VALID:C2RX0SID")
   struct CiRXnSID_struct C2RX0SID;
   struct CiRXnSID_struct C2RX1SID;
   #word C2RX0SID=getenv("SFR:C2RX0SID")
   #word C2RX1SID=getenv("SFR:C2RX1SID")
#endif

//Receive Buffer n Extended Indentifier
#word C1RX0EID=getenv("SFR:C1RX0EID")
#word C1RX1EID=getenv("SFR:C1RX1EID")

#if getenv("SFR_VALID:C2RX0EID")
   #word C2RX0EID=getenv("SFR:C2RX0EID")
   #word C2RX1EID=getenv("SFR:C2RX1EID")
#endif

// Receive Buffer n Data Field Word m
#word C1RX0B1=getenv("C1RX0B1")
#word C1RX0B2=getenv("C1RX0B2")
#word C1RX0B3=getenv("C1RX0B3")
#word C1RX0B4=getenv("C1RX0B4")
#word C1RX1B1=getenv("C1RX1B1")
#word C1RX1B2=getenv("C1RX1B2")
#word C1RX1B3=getenv("C1RX1B3")
#word C1RX1B4=getenv("C1RX1B4")

#if getenv("SFR_VALID:C2RX0B1")
   #word C2RX0B1=getenv("C2RX0B1")
   #word C2RX0B2=getenv("C2RX0B2")
   #word C2RX0B3=getenv("C2RX0B3")
   #word C2RX0B4=getenv("C2RX0B4")
   #word C2RX1B1=getenv("C2RX1B1")
   #word C2RX1B2=getenv("C2RX1B2")
   #word C2RX1B3=getenv("C2RX1B3")
   #word C2RX1B4=getenv("C2RX1B4")
#endif

// Receive Buffer n Data Length Control
struct CiRXnDLC_struct {
   int dlc:4;        //0:3    data length code bits
   int1 rb0;         //4      reserved bit 0
   int void765:3;    //5:7
   int1 rb1;         //8      reserved bit 1
   int1 rxrtr;       //9      receive remote transmission request control bit
   int eid:6;        //10:15  extended identifier bits
};
struct CiRXnDLC_struct C1RX0;
struct CiRXnDLC_struct C1RX1;

#word C1RX0=getenv("SFR:C1RX0DLC")
#word C1RX1=getenv("SFR:C1RX1DLC")

#if getenv("SFR_VALID:C2RX0DLC")
   struct CiRXnDLC_struct C2RX0;
   struct CiRXnDLC_struct C2RX1;
   #word C2RX0=getenv("SFR:C2RX0DLC")
   #word C2RX1=getenv("SFR:C2RX1DLC")
#endif

////////////////////////////////////////////////////////////////////////////////
//////////////////////// CAN Message Acceptance Filters ////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Acceptance Filter n Standard Indentifier
#word C1RXF0SID=getenv("SFR:C1RXF0SID")
#word C1RXF1SID=getenv("SFR:C1RXF1SID")
#word C1RXF2SID=getenv("SFR:C1RXF2SID")
#word C1RXF3SID=getenv("SFR:C1RXF3SID")
#word C1RXF4SID=getenv("SFR:C1RXF4SID")
#word C1RXF5SID=getenv("SFR:C1RXF5SID")

#if getenv("SFR_VALID:C2RXF0SID")
   #word C2RXF0SID=getenv("SFR:C2RXF0SID")
   #word C2RXF1SID=getenv("SFR:C2RXF1SID")
   #word C2RXF2SID=getenv("SFR:C2RXF2SID")
   #word C2RXF3SID=getenv("SFR:C2RXF3SID")
   #word C2RXF4SID=getenv("SFR:C2RXF4SID")
   #word C2RXF5SID=getenv("SFR:C2RXF5SID")
#endif

// Acceptance Filter n Extended Identifer High
#word C1RXF0EIDH=getenv("SFR:C1RXF0EIDH")
#word C1RXF1EIDH=getenv("SFR:C1RXF1EIDH")
#word C1RXF2EIDH=getenv("SFR:C1RXF2EIDH")
#word C1RXF3EIDH=getenv("SFR:C1RXF3EIDH")
#word C1RXF4EIDH=getenv("SFR:C1RXF4EIDH")
#word C1RXF5EIDH=getenv("SFR:C1RXF5EIDH")

#if getenv("SFR_VALID:C2RXF0EIDH")
   #word C2RXF0EIDH=getenv("SFR:C2RXF0EIDH")
   #word C2RXF1EIDH=getenv("SFR:C2RXF1EIDH")
   #word C2RXF2EIDH=getenv("SFR:C2RXF2EIDH")
   #word C2RXF3EIDH=getenv("SFR:C2RXF3EIDH")
   #word C2RXF4EIDH=getenv("SFR:C2RXF4EIDH")
   #word C2RXF5EIDH=getenv("SFR:C2RXF5EIDH")
#endif
   
// Acceptance Filter n Entended Indenter Low
#word C1RXF0=getenv("SFR:C1RXF0EIDL")
#word C1RXF1=getenv("SFR:C1RXF1EIDL")
#word C1RXF2=getenv("SFR:C1RXF2EIDL")
#word C1RXF3=getenv("SFR:C1RXF3EIDL")
#word C1RXF4=getenv("SFR:C1RXF4EIDL")
#word C1RXF5=getenv("SFR:C1RXF5EIDL")

#if getenv("SFR_VALID:C2RXF0EIDL")
   #word C2RXF0=getenv("SFR:C2RXF0EIDL")
   #word C2RXF1=getenv("SFR:C2RXF1EIDL")
   #word C2RXF2=getenv("SFR:C2RXF2EIDL")
   #word C2RXF3=getenv("SFR:C2RXF3EIDL")
   #word C2RXF4=getenv("SFR:C2RXF4EIDL")
   #word C2RXF5=getenv("SFR:C2RXF5EIDL")
#endif
   
////////////////////////////////////////////////////////////////////////////////
///////////////////// CAN Acceptance Filter Mask Registers /////////////////////
////////////////////////////////////////////////////////////////////////////////

// Acceptance Filter Mask n Standard Identifier
#word C1RXM0SID=getenv("SFR:C1RXM0SID")
#word C1RXM1SID=getenv("SFR:C1RXM1SID")

#if getenv("SFR_VALID:C2RXM0SID")
   #word C2RXM0SID=getenv("SFR:C2RXM0SID")
   #word C2RXM1SID=getenv("SFR:C2RXM1SID")
#endif
   
// Acceptance Filter Mask n Extended Identifier High
#word C1RXM0EIDH=getenv("SFR:C1RXM0EIDH")
#word C1RXM1EIDH=getenv("SFR:C1RXM1EIDH")

#if getenv("SFR_VALID:C2RXM0EIDH")
   #word C2RXM0EIDH=getenv("SFR:C2RXM0EIDH")
   #word C2RXM1EIDH=getenv("SFR:C2RXM1EIDH")
#endif
   
// Acceptance Filter Mask n Extended Identifier Low
#word C1RXM0=getenv("SFR:C1RXM0EIDL")
#word C1RXM1=getenv("SFR:C1RXM1EIDL")

#if getenv("SFR_VALID:C2RXM0EIDL")
   #word C2RXM0=getenv("SFR:C2RXM0EIDL")
   #word C2RXM1=getenv("SFR:C2RXM1EIDL")
#endif
   
////////////////////////////////////////////////////////////////////////////////
//////////////////////////// CAN Baud Rate Registers ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct CiCFG1_struct {
   int brp:6;        //0:5    baud rate prescaler bits
   int sjw:2;        //6:7    synchronized jump width bits
   int void15_8:8;    //8:15
};
struct CiCFG1_struct C1CFG1;

#word C1CFG1=getenv("SFR:C1CFG1")

#if getenv("SFR_VALID:C2CFG1")
   struct CiCFG1_struct C2CFG1;
   #word C2CFG1=getenv("SFR:C2CFG1")
#endif
   
struct CiCFG2_struct {
   int prseg:3;      //0:2    propagation time segment bits
   int seg1ph:3;     //3:5    phase buffer segment 1 bits
   int1 sam;         //6      sample of the CAN bus line bit
   int1 seg2phts;    //7      phase segment 2 time select bit
   int seg2ph:3;     //8:10   phase buffer segement 2 bits
   int void131211:3; //11:13
   int1 wakfil;      //14     select CAN bus line filter for wake-up bit
   int1 void15;      //15
};
struct CiCFG2_struct C1CFG2;

#word C1CFG2=getenv("SFR:C1CFG2")

#if getenv("SFR_VALID:C2CFG2")
   struct CiCFG2_struct C2CFG2;
   #word C2CFG2=getenv("SFR:C2CFG2")
#endif

////////////////////////////////////////////////////////////////////////////////
//////////////////////// CAN Module Error Count Register ///////////////////////
////////////////////////////////////////////////////////////////////////////////

// Transmit/Receive Error Count
#word C1EC=getenv("SFR:C1EC")

#if getenv("SFR_VALID:C2EC")
   #word C2EC=getenv("SFR:C2EC")
#endif

////////////////////////////////////////////////////////////////////////////////
//////////////////////////// CAN Interrupt Registers ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

enum INTERRUPT { RX0=0x01,RX1=0x02,TX0=0x04,TX1=0x08,TX2=0x10,ERR=0x20,WAK=0x40,IVR=0x80};

// Interrupt Enable Register
struct CiINTE_struct {
   int1 rx0ie;       //0      receive buffer 0 interrupt enable bit
   int1 rx1ie;       //1      receive buffer 1 interrupt enable bit
   int1 tx0ie;       //2      transmit buffer 0 interrupt enable bit
   int1 tx1ie;       //3      transmit buffer 1 interrupt enable bit
   int1 tx2ie;       //4      transmit buffer 2 interrupt enable bit
   int1 errie;       //5      error interrupt enable bit
   int1 wakie;       //6      bus wake up activity interrupt enable bit
   int1 ivrie;       //7      invalid message received interrupt enable bit
   int void15_8:8;   //8:15
};
struct CiINTE_struct C1INTE;

#word C1INTE=getenv("SFR:C1INTE")

#if getenv("SFR_VALID:C2INTE")
   struct CiINTE_struct C2INTE;
   #word C2INTE=getenv("SFR:C2INTE")
#endif

// Interrupt Flag Register
struct CiINTF_struct {
   int1 rx0if;       //0      receive buffer 0 interrupt flag bit
   int1 rx1if;       //1      receive buffer 1 interrupt flag bit
   int1 tx0if;       //2      transmit buffer 0 interrput flag bit
   int1 tx1if;       //3      transmit buffer 1 interrput flag bit
   int1 tx2if;       //4      transmit buffer 2 interrput flag bit
   int1 effif;       //5      error interrupt flag bit
   int1 wakif;       //6      bus wake-up activity interrupt flag bit
   int1 ivrif;       //7      invalid message received interrupt flag bit
   int1 ewarn;       //8      transmitter or receiver is in error state, warning bit
   int1 rxwar;       //9      receiver in error state, warning bit
   int1 txwar;       //10     transmitter in error state, warning bit
   int1 rxep;        //11     receiver in error state, bus passive bit
   int1 txep;        //12     transmitter in error state, bus passive bit
   int1 txbo;        //13     transmitter in error state, bus off bit
   int1 rx1ovr;      //14     receive buffer 1 overflowed bit
   int1 rx0ovr;      //15     receive buffer 0 overflowed bit
};
struct CiINTF_struct C1INTF;

#word C1INTF=getenv("SFR:C1INTF")

#if getenv("SFR_VALID:C2INTF")
   struct CiINTF_struct C2INTF;
   #word C2INTF=getenv("SFR:C2INTF")
#endif
   
//value to put in mask field to accept all incoming id's
#define CAN_MASK_ACCEPT_ALL   0

//PROTOTYPES

struct rx_stat {
   uint8_t filthit;      // fillter that allowed the frame into the buffer
   uint8_t buffer;      // receive buffer
   int1 err_ovfl;      // buffer overflow
   int1 rtr;         // rtr requested
   int1 ext;         // extended id
   int1 inv;         // invalid id?
};

void can_set_baud(void);
void can_set_mode(CAN_OP_MODE mode);
void can_set_id(uint16_t* addr, uint32_t id, int1 ext);
void can_set_transmit_id(uint16_t* addr, uint32_t id, int1 ext);
uint32_t can_get_id(uint16_t *addr, int1 ext);
int1 can_putd(uint32_t id, uint8_t *data, uint8_t len, uint8_t priority, int1 ext, int1 rtr);
int1 can_getd(uint32_t &id, uint8_t *data, uint8_t &len, struct rx_stat &stat);

#if getenv("SFR_VALID:C2CTRL")
   void can2_set_baud(void);
   void can2_set_mode(CAN_OP_MODE mode);
   void can2_set_id(uint16_t* addr, uint32_t id, int1 ext);
   int1 can2_putd(uint32_t id, uint8_t *data, uint8_t len, uint8_t priority, int1 ext, int1 rtr);
   int1 can2_getd(uint32_t &id, uint8_t *data, uint8_t &len, struct rx_stat &stat);
#endif

#endif
