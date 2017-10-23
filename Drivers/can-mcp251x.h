/////////////////////////////////////////////////////////////////////////
////                        can-mcp251x.h                            ////
////                                                                 ////
//// Prototypes, definitions, defines and macros used for and with   ////
//// the CCS CAN library for the MCP2510 (and compatible) CAN IO     ////
//// expanders.                                                      ////
////                                                                 ////
//// (see can-mcp251x.c)                                             ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// Version History                                                 ////
////                                                                 ////
////  Apr 20 04 - Fixed a compiling problem                          ////
////                                                                 ////
////  Jul 17 09 - Updated for PCD Compiler                           ////
////                                                                 ////
////  May 20 10 - Changed all variable types to unsigned for PCD and ////
////              if #device ANSI is used with PCH or PCM            ////
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

#ifndef __CCS_CANMCP2510_LIB_DEFINES__
#define __CCS_CANMCP2510_LIB_DEFINES__

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

#ifndef CAN_USE_RX_DOUBLE_BUFFER
 #define CAN_USE_RX_DOUBLE_BUFFER TRUE   //if buffer 0 overflows, do NOT use buffer 1 to put buffer 0 data
#endif

#ifndef CAN_ENABLE_DRIVE_HIGH
 #define CAN_ENABLE_DRIVE_HIGH 0
#endif

#ifndef CAN_ENABLE_CAN_CAPTURE
 #define CAN_ENABLE_CAN_CAPTURE 0
#endif

enum CAN_OP_MODE {CAN_OP_CONFIG=4, CAN_OP_LISTEN=3, CAN_OP_LOOPBACK=2, CAN_OP_SLEEP=1, CAN_OP_NORMAL=0};

//can control
struct struct_CANCTRL {
   int  clkpre:2; //0:1 //clkout pin prescalar
   int1 clken; //2   //clkout pin enable
   int1 void3; //3
   int1 abat;  //4   //abort all pending transmissions
   CAN_OP_MODE reqop:3; //5:7 //request operation mode
};
#define CANCTRL   0x0F  //or 1f, or 2f, or 3f ... or 7f

enum CAN_INT_CODE {CAN_INT_RX1=7, CAN_INT_RX0=6, CAN_INT_TX2=5, CAN_INT_TX1=4, CAN_INT_TX0=3, CAN_INT_WAKEUP=2, CAN_INT_ERROR=1, CAN_INT_NO=0};

//can status register READ-ONLY
struct struct_CANSTAT {
   int1 void0;   //0
   CAN_INT_CODE icode:3;   //1:3   //interrupt code
   int1 void4;   //4
   CAN_OP_MODE opmode:3;   //5:7   //operation mode status
};
#define CANSTAT 0x0E //or 1e, or 2e ... or 7e

//error flag register
struct struct_EFLG {
   int1 ewarn;      //0 //error warning
   int1 rxwar;      //1 //receiver warning
   int1 txwar;      //2 //transmitter warning
   int1 rxep;   //3 //receive error passive flag
   int1 txep;   //4 //transmit error passive flag
   int1 txbo;   //5   //bus off error flag
   int1 rx0ovr;   //6   //receive buffer 0 overflow
   int1 rx1ovr;   //7   //receive buffer 1 overflow
};
#define EFLG   0x2D

enum INTERRUPT { RX0=0x01,RX1=0x02,TX0=0x04,TX1=0x08,TX2=0x10,ERR=0x20,WAK=0x40,IVR=0x80};

//interrupt enable register
struct struct_CANINTE {
   int1 rx0ie; //0   //receive buffer 0 full interrupt enable
   int1 rx1ie; //1   //receive buffer 1 full interrupt enable
   int1 tx0ie; //2   //transmit buffer 0 empty interrupt enable
   int1 tx1ie; //3   //transmit buffer 1 empty interrupt enable
   int1 tx2ie; //4   //transmit buffer 2 empty interrupt enable
   int1 errie; //5   //error interrupt enable
   int1 wakie; //6   //wakeup interrupt  enable
   int1 merre; //7   //message error interrupt enable
};
#define CANINTE   0x2B

//interrupt enable register
struct struct_CANINTF {
   int1 rx0if; //0   //receive buffer 0 full interrupt flag
   int1 rx1if; //1   //receive buffer 1 full interrupt flag
   int1 tx0if; //2   //transmit buffer 0 empty interrupt flag
   int1 tx1if; //3   //transmit buffer 1 empty interrupt flag
   int1 tx2if; //4   //transmit buffer 2 empty interrupt flag
   int1 errif; //5   //error interrupt flag
   int1 wakif; //6   //wakeup interrupt flag
   int1 merrf; //7   //message error interrupt flag
};
#define CANINTF   0x2C


//error counters
#define TEC    0x1C
#define REC    0x1D

//baud rate control register 1
struct struct_CNF1 {
   int brp:6;   //0:5   //baud rate prescalar
   int sjw:2;   //6:7   //synchronized jump width
};
#define CNF1   0x2A

//baud rate control register 2
struct struct_CNF2 {
   int prseg:3; //0:2 //propagation time select
   int phseg1:3; //3:5 //phase segment 1
   int1 sam; //6 //sample of the can bus line
   int1 btlmode; //7 //phase segment 2 bit time length
};
#define CNF2   0x29

//baud rate control register 3
struct struct_CNF3 {
   int phseg2:3;     //0:2 //phase segment 2 time select
   int void543:3;    //3:5
   int1 wakfil;      //6   //selects can bus line filter for wake-up
   int1 sof;         //7
};
#define CNF3   0x28
//can i/o control register

//transmit buffer n control register
struct txbNctrl_struct {
   int  txpri:2;   //0:1   //transmit priority bits
   int1 void2; //2
   int1 txreq;   //3   //transmit request status (clear to request message abort)
   int1 txerr;   //4   //transmission error detected
   int1 mloa;   //5   //message lost arbitration
   int1 abtf;   //6   //message was aborted / or transmitted successfully
   int1 void7;
};
#define TXB0CTRL  0x30
#define TXB1CTRL  0x40
#define TXB2CTRL  0x50

//TXnRTS pin control and status register
struct struct_TXRTSCTRL {
   int1 b0rtsm; //0  //1=request message trans, 0=digital
   int1 b1rtsm; //1  //1=request message trans, 0=digital
   int1 b2rtsm; //2  //1=request message trans, 0=digital
   int1 b0rts; //3   //reads as tx2rts when in digital, 0 when in rts
   int1 b1rts; //4   //reads as tx2rts when in digital, 0 when in rts mode
   int1 b2rts; //5  //reads as tx2rts when in digital, 0 when in rts mode
   int  void67:2; //6:7
};
#define TXRTSCTRL 0x0D

//transmit buffer n standard identifier
#define TXB0SIDH 0x31
#define TXB0SIDL 0x32
#define TXB1SIDH 0x41
#define TXB1SIDL 0x42
#define TXB2SIDH 0x51
#define TXB2SIDL 0x52

//transmit buffer n extended identifier
#define TXB0EIDH 0x33
#define TXB0EIDL 0x34
#define TXB1EIDH 0x43
#define TXB1EIDL 0x44
#define TXB2EIDH 0x53
#define TXB2EIDL 0x54

//transmit buffer n data byte m
#define TXB0D0 0x36
#define TXB0D7 0x3D

#define TXB1D0 0x46
#define TXB1D7 0x4D

#define TXB2D0 0x56
#define TXB2D7 0x5D

//transmit buffer n data length
struct txbNdlc_struct {
   int dlc:4;   //0:3
   int void54:2; //4:5
   int1 rtr; //6 //transmission frame remote transmission
   int1 void7; //7
};
#define TXB0DLC 0x35
#define TXB1DLC 0x45
#define TXB2DLC 0x55

//#byte TXBaDLC=0xF65  //txbXdlc when in the access bank


//transmit error count register
#byte TXERRCNT=0xF76


enum CAN_RX_MODE {CAN_RX_ALL=3, CAN_RX_EXT=2, CAN_RX_STD=1, CAN_RX_VALID=0};

//receive buffer 0 control register
struct struct_RXB0CTRL {
   int1 filhit0;   //0 //filter hit
   int1 bukt1;   //1 //read only copy of bukt bit (used internally by mcp2510)
   int1 bukt;   //2 //rollover enable
   int1 rxrtr;   //3 //receive remote transfer request
   int1 void4;   //4
   CAN_RX_MODE rxm:2;   //5:6 //receiver buffer mode
   int1 void7;   //7 //receive full status
};
#define RXB0CTRL  0x60

//receive buffer 1 control register
struct struct_RXB1CTRL {
   int filhit0:3;   //0:2
   int1 rxrtr;   //3 //receive remote transfer request
   int1 void4;   //4
   CAN_RX_MODE rxm:2;   //5:6 //receive buffer mode
   int1 void7;   //7
};
#define RXB1CTRL 0x70

//RXnBF pint control and status register
struct struct_BFPCTRL {
   int1  b0bfm; //0   //1=pin is interrupt when message loaded into rxb0, 0=digital
   int1  b1bfm; //1   //1=pin is interrupt when message loaded into rxb1, 0=digital
   int1  b0bfe; //2   //rx0bf pin function enable
   int1  b1bfe; //3   //rx1bf pin function enable
   int1  b0bfs; //4   //rx0bf pin state
   int1  b1bfs; //5   //rx1bf pin state
};

//receive buffer n standard identifier
#define   RXB0SIDH  0x61
#define   RXB0SIDL  0x62

#define   RXB1SIDH  0x71
#define   RXB1SIDL  0x72

//receive buffer n extended identifier
#define   RXB0EID8  0x63
#define   RXB0EID0  0x64

#define   RXB1EID8  0x73
#define   RXB1EID0  0x74

struct struct_TXRXBaSIDL {
   int void012:3; //0:2
   int1 ext;      //3 //extended id
   int1 srr;      //4 //substitute remove request bit
   int void567:3; //5:7
};

//receive buffer n data length code register
struct rxbNdlc_struct {
   int dlc:4;   //0:3 //data length code
   int1 rb0;   //4   //reserved
   int1 rb1;   //5   //reserved
   int1 rtr;   //6   //receiver remote transmission request bit
   int1 void7;   //7
};
#define   RXB0DLC   0x65
#define   RXB1DLC   0x75

//receive buffer n data field byte m register
#define RXB0D0    0x66
#define RXB0D7    0x6D

#define RXB1D0    0x76
#define RXB1D7    0x7D


//receive acceptance filter n standard identifier
#define RXF0SIDH  0x00
#define RXF0SIDL  0x01
#define RXF1SIDH  0x04
#define RXF1SIDL  0x05
#define RXF2SIDH  0x08
#define RXF2SIDL  0x09
#define RXF3SIDH  0x10
#define RXF3SIDL  0x11
#define RXF4SIDH  0x14
#define RXF4SIDL  0x15
#define RXF5SIDH  0x18
#define RXF5SIDL  0x19

//receive acceptance filter n extended identifier
#define RXF0EIDH  0x02
#define RXF0EIDL  0x03
#define RXF1EIDH  0x06
#define RXF1EIDL  0x07
#define RXF2EIDH  0x0a
#define RXF2EIDL  0x0b
#define RXF3EIDH  0x12
#define RXF3EIDL  0x13
#define RXF4EIDH  0x16
#define RXF4EIDL  0x17
#define RXF5EIDH  0x1a
#define RXF5EIDL  0x1b

//receive acceptance mask n standard identifier mask
#define RXM0SIDH  0x20
#define RXM0SIDL  0x21
#define RXM1SIDH  0x24
#define RXM1SIDL  0x25

//receive acceptance mask n extended identifier mask
#define RXM0EIDH  0x22
#define RXM0EIDL  0x23
#define RXM1EIDH  0x26
#define RXM1EIDL  0x27

#define RX0MASK       RXM0EIDL   //rxm0eidl
#define RX1MASK       RXM1EIDL   //rxm1eidl
#define RX0FILTER0    RXF0EIDL   //rxf0eidl
#define RX0FILTER1    RXF1EIDL   //rxf1eidl
#define RX1FILTER2    RXF2EIDL   //rxf2eidl
#define RX1FILTER3    RXF3EIDL   //rxf3eidl
#define RX1FILTER4    RXF4EIDL   //rxf4eidl
#define RX1FILTER5    RXF5EIDL   //rxf5eidl
#define RXB0ID        RXB0EIDL   //rxb0eidl
#define RXB1ID        RXB1EIDL   //rxb1eidl
#define TXB0ID        TXB0EIDL   //txb0eidl
#define TXB1ID        TXB1EIDL   //txb1eidl
#define TXB2ID        TXB2EIDL   //tsb2eidl

//value to put in mask field to accept all incoming id's
#define CAN_MASK_ACCEPT_ALL   0


//PROTOTYPES and MACROS

struct rx_stat {
   int1 err_ovfl;
   int filthit:3;
   int1 buffer;
   int1 rtr;
   int1 ext;
   int1 inv;
};

void  can_init(void);
void  can_set_baud(void);
void  can_set_mode(CAN_OP_MODE mode);
void can_set_id(unsigned int8 addr, unsigned int32 id, int1 ext);
unsigned int32 can_get_id(unsigned int8 addr, int1 ext);
int1  can_putd(unsigned int32 id, unsigned int8 * data, unsigned int8 len, unsigned int8 priority, int1 ext, int1 rtr);
int1  can_getd(unsigned int32 & id, unsigned int8 * data, unsigned int8 & len, struct rx_stat & stat);
int1 can_kbhit(void);
int1 can_tbe(void);
void can_abort(void);
void can_enable_interrupts(INTERRUPT settings);
void can_disable_interrupts(INTERRUPT settings);
void can_clear_interrupt(INTERRUPT settings);

void mcp2510_init();
void mcp2510_command(unsigned int8 command);
void mcp2510_write(unsigned int8 address, unsigned int8 data);
unsigned int8 mcp2510_status(void);
unsigned int8 mcp2510_read(unsigned int8 address);
void mcp2510_bit_modify(unsigned int8 address, unsigned int8 mask, unsigned int8 data);

#endif
