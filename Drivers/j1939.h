////////////////////////////////////////////////////////////////////////////////
////                                J1939.h                                 ////
////                                                                        ////
//// Prototypes, defines and global variable used with the J1939 Driver.    ////
////                                                                        ////
////////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2012 Custom Computer Services                ////
//// This source code may only be used by licensed users of the CCS         ////
//// C compiler.  This source code may only be distributed to other         ////
//// licensed users of the CCS C compiler.  No other use,                   ////
//// reproduction or distribution is permitted without written              ////
//// permission.  Derivative programs created using this software           ////
//// in object code form are not restricted in any way.                     ////
////////////////////////////////////////////////////////////////////////////////
#ifndef _J1939_H
#define _J1939_H

#include <stdint.h>

#ifndef USE_INTERNAL_CAN
#define USE_INTERNAL_CAN TRUE
#endif

#ifndef J1939_RECEIVE_BUFFERS
#define J1939_RECEIVE_BUFFERS    16
#endif

#if J1939_RECEIVE_BUFFERS < 2
#undef J1939_RECEIVE_BUFFERS
#define J1939_RECEIVE_BUFFERS    2  //required to be able to receive one RTS/CTS session and one BAM session at same time
#endif

#ifndef J1939_TRANSMIT_BUFFERS
#define J1939_TRANSMIT_BUFFERS   2
#endif

#if J1939_TRANSMIT_BUFFERS == 0
#undef J1939_TRANSMIT_BUFFERS
#define J1939_TRANSMIT_BUFFERS   1
#endif

////////////////////////////////////////////////////////////////////////////////  Global variables

//global variables containing unit's J1939 Address and Name
uint8_t g_MyJ1939Address;
uint8_t g_J1939Name[8];

//global J1939 tick variables
J1939_TICK_TYPE g_J1939CurrentClaimTick, g_J1939PreviousClaimTick;
J1939_TICK_TYPE g_J1939PreviousCannotClaimTick;
J1939_TICK_TYPE g_J1939CannotClaimDelay;

//J1939 PDU Structure
typedef struct _J1939_PDU_STRUCT {
   uint8_t SourceAddress;
   uint8_t DestinationAddress;   //Also Group Extension for PDU2 messages
   uint8_t PDUFormat;
   int1    DataPage;
   int1    ExtendedDataPage;     //Set to 0 for J1939
   uint8_t Priority:3;
   uint8_t unused7_5:3;          //unused bits don't do anything with them
} J1939_PDU_STRUCT;

//J1939 Message Structure
typedef struct _J1939_MESSAGE_STRUCT {
   J1939_PDU_STRUCT PDU;
   uint8_t Length;
   uint8_t Data[8];
} J1939_MESSAGE_STRUCT;

//global J1939 Receive and Transmit buffers
J1939_MESSAGE_STRUCT g_J1939ReceiveBuffer[J1939_RECEIVE_BUFFERS];
J1939_MESSAGE_STRUCT g_J1939XmitBuffer[J1939_TRANSMIT_BUFFERS];

//global J1939 variable for indexing J1939 Receive and Transmit buffers
static uint8_t g_J1939ReceiveNextIn;
static uint8_t g_J1939ReceiveNextOut;
static uint8_t g_J1939XmitNextIn;
static uint8_t g_J1939XmitNextOut;

//J1939 Flag structure
typedef struct _J1939_FLAGS_STRUCT {
   int1    AddressClaimed;       //Unit Successfully claimed an address
   int1    AddressClaimSent;     //Unit has sent a claim request
   int1    AddressNewClaim;      //Used to specify if claim request is for a new address
   int1    AddressCannotClaim;   //If not arbitrary address capable, is set if unit can't claim address
   uint8_t unused4_1:4;
   uint8_t ReceiveBufferCount;   //Keep track of number of stored messages in receive buffer
   uint8_t XmitBufferCount;      //Keep track of number of messages that still need transmitted
} J1939_FLAGS_STRUCT;

//global J1939 Flag structure variable
J1939_FLAGS_STRUCT g_J1939Flags;

//global variable used in generating pseudo-random 8-bit number
uint8_t rand_seed;

//////////////////////////////////////////////////////////////////////////////// J1939 Defines

//PDU Format Defines
#define J1939_PF_BROADCAST          254
#define J1939_PF_REQUEST            234
#define J1939_PF_ACK                232
#define J1939_PF_PROPRIETARY_A      239
#define J1939_PF_PROPRIETARY_B      255
#define J1939_PF_REQUEST2           201
#define J1939_PF_TRANSFER           202
#define J1939_PF_PT_CM              236
#define J1939_PF_PT_DT              235
#define J1939_PF_ADDR_CLAIMED       238
#define J1939_PF_ADDR_CANNOT_CLAIM  238

//PDU Default Priorities Defines
#define J1939_CONTROL_PRIORITY         3
#define J1939_REQUEST_PRIORITY         6
#define J1939_ACK_PRIORITY             6
#define J1939_PROPRIETARY_A_PRIORITY   6
#define J1939_PROPRIETARY_B_PRIORITY   6
#define J1939_REQUEST2_PRIORITY        6
#define J1939_TRANSFER_PRIORITY        6
#define J1939_TP_CM_PRIORITY           7
#define J1939_TP_DT_PRIORITY           7

//Defines used with Transport Protocol Messages (refer to J1939-21 for spec)
#define J1939_TP_CM_CTS          16
#define J1939_TP_CM_DTS          17
#define J1939_TP_CM_EOF          19
#define J1939_TP_CM_ABORT        255
#define J1939_TP_CM_BAM          32

//J1939 Address Defines
#define J1939_NULL_ADDRESS       254
#define J1939_GLOBAL_ADDRESS     255

//////////////////////////////////////////////////////////////////////////////// J1939 Baud Rate

#ifndef J1939_BAUD_RATE
#define J1939_BAUD_RATE 250000
#endif

#ifndef CAN_BRG_PRESCALAR
 #if USE_INTERNAL_CAN == TRUE
  #if getenv("CLOCK") == 8000000
   #if J1939_BAUD_RATE == 250000
    #if defined(__PCH__) || (getenv("DEVICE") == "DSPIC30F6010A") || (getenv("DEVICE") == "DSPIC30F6011A") || (getenv("DEVICE") == "DSPIC30F6012A") || (getenv("DEVICE") == "DSPIC30F6013A") || (getenv("DEVICE") == "DSPIC30F6014A") || (getenv("DEVICE") == "DSPIC30F6015") || \
        (getenv("DEVICE") == "DSPIC30F5011") || (getenv("DEVICE") == "DSPIC30F5013") || (getenv("DEVICE") == "DSPIC30F5015") || (getenv("DEVICE") == "DSPIC30F5016") || \
        (getenv("DEVICE") == "DSPIC30F4011") || (getenv("DEVICE") == "DSPIC30F4012") || (getenv("DEVICE") == "DSPIC30F4013")
     #define CAN_BRG_PRESCALAR         0
     #define CAN_BRG_PHASE_SEGMENT_1   6
     #define CAN_BRG_PHASE_SEGMENT_2   6
     #define CAN_BRG_PROPAGATION_TIME  0
     #define CAN_BRG_SYNCH_JUMP_WIDTH  0
    #else
     #define CAN_BRG_PRESCALAR         0
     #define CAN_BRG_PHASE_SEGMENT_1   2
     #define CAN_BRG_PHASE_SEGMENT_2   2
     #define CAN_BRG_PROPAGATION_TIME  0
     #define CAN_BRG_SYNCH_JUMP_WIDTH  0
    #endif
   #elif J1939_BAUD_RATE == 500000
    #if defined(__PCH__) || (getenv("DEVICE") == "DSPIC30F6010A") || (getenv("DEVICE") == "DSPIC30F6011A") || (getenv("DEVICE") == "DSPIC30F6012A") || (getenv("DEVICE") == "DSPIC30F6013A") || (getenv("DEVICE") == "DSPIC30F6014A") || (getenv("DEVICE") == "DSPIC30F6015") || \
        (getenv("DEVICE") == "DSPIC30F5011") || (getenv("DEVICE") == "DSPIC30F5013") || (getenv("DEVICE") == "DSPIC30F5015") || (getenv("DEVICE") == "DSPIC30F5016") || \
        (getenv("DEVICE") == "DSPIC30F4011") || (getenv("DEVICE") == "DSPIC30F4012") || (getenv("DEVICE") == "DSPIC30F4013")
     #define CAN_BRG_PRESCALAR         0
     #define CAN_BRG_PHASE_SEGMENT_1   2
     #define CAN_BRG_PHASE_SEGMENT_2   2
     #define CAN_BRG_PROPAGATION_TIME  0
     #define CAN_BRG_SYNCH_JUMP_WIDTH  0
    #else
     #error Increase Clock Speed for 500000 Kb/s
    #endif
   #else
    #error Please define BRG Prescalar, Phase Segments, Propagation Time and Synch Jump Width for J1939 Baud Rate and PIC Clock Speed
   #endif
  #elif getenv("CLOCK") == 16000000
   #if J1939_BAUD_RATE == 250000
    #if defined(__PCH__) || (getenv("DEVICE") == "DSPIC30F6010A") || (getenv("DEVICE") == "DSPIC30F6011A") || (getenv("DEVICE") == "DSPIC30F6012A") || (getenv("DEVICE") == "DSPIC30F6013A") || (getenv("DEVICE") == "DSPIC30F6014A") || (getenv("DEVICE") == "DSPIC30F6015") || \
        (getenv("DEVICE") == "DSPIC30F5011") || (getenv("DEVICE") == "DSPIC30F5013") || (getenv("DEVICE") == "DSPIC30F5015") || (getenv("DEVICE") == "DSPIC30F5016") || \
        (getenv("DEVICE") == "DSPIC30F4011") || (getenv("DEVICE") == "DSPIC30F4012") || (getenv("DEVICE") == "DSPIC30F4013")
     #define CAN_BRG_PRESCALAR         1
     #define CAN_BRG_PHASE_SEGMENT_1   6
     #define CAN_BRG_PHASE_SEGMENT_2   6
     #define CAN_BRG_PROPAGATION_TIME  0
     #define CAN_BRG_SYNCH_JUMP_WIDTH  0
    #else
     #define CAN_BRG_PRESCALAR         0
     #define CAN_BRG_PHASE_SEGMENT_1   6
     #define CAN_BRG_PHASE_SEGMENT_2   6
     #define CAN_BRG_PROPAGATION_TIME  0
     #define CAN_BRG_SYNCH_JUMP_WIDTH  0
    #endif
   #elif J1939_BAUD_RATE == 500000
    #if defined(__PCH__) || (getenv("DEVICE") == "DSPIC30F6010A") || (getenv("DEVICE") == "DSPIC30F6011A") || (getenv("DEVICE") == "DSPIC30F6012A") || (getenv("DEVICE") == "DSPIC30F6013A") || (getenv("DEVICE") == "DSPIC30F6014A") || (getenv("DEVICE") == "DSPIC30F6015") || \
        (getenv("DEVICE") == "DSPIC30F5011") || (getenv("DEVICE") == "DSPIC30F5013") || (getenv("DEVICE") == "DSPIC30F5015") || (getenv("DEVICE") == "DSPIC30F5016") || \
        (getenv("DEVICE") == "DSPIC30F4011") || (getenv("DEVICE") == "DSPIC30F4012") || (getenv("DEVICE") == "DSPIC30F4013")
     #define CAN_BRG_PRESCALAR         0
     #define CAN_BRG_PHASE_SEGMENT_1   6
     #define CAN_BRG_PHASE_SEGMENT_2   6
     #define CAN_BRG_PROPAGATION_TIME  0
     #define CAN_BRG_SYNCH_JUMP_WIDTH  0
    #else
     #define CAN_BRG_PRESCALAR         0
     #define CAN_BRG_PHASE_SEGMENT_1   2
     #define CAN_BRG_PHASE_SEGMENT_2   2
     #define CAN_BRG_PROPAGATION_TIME  0
     #define CAN_BRG_SYNCH_JUMP_WIDTH  0
    #endif
   #else
    #error Please define BRG Prescalar, Phase Segments, Propagation Time and Synch Jump Width for J1939 Baud Rate and PIC Clock Speed
   #endif
  #elif getenv("CLOCK") == 20000000
   #if J1939_BAUD_RATE == 250000
    #if defined(__PCH__) || (getenv("DEVICE") == "DSPIC30F6010A") || (getenv("DEVICE") == "DSPIC30F6011A") || (getenv("DEVICE") == "DSPIC30F6012A") || (getenv("DEVICE") == "DSPIC30F6013A") || (getenv("DEVICE") == "DSPIC30F6014A") || (getenv("DEVICE") == "DSPIC30F6015") || \
        (getenv("DEVICE") == "DSPIC30F5011") || (getenv("DEVICE") == "DSPIC30F5013") || (getenv("DEVICE") == "DSPIC30F5015") || (getenv("DEVICE") == "DSPIC30F5016") || \
        (getenv("DEVICE") == "DSPIC30F4011") || (getenv("DEVICE") == "DSPIC30F4012") || (getenv("DEVICE") == "DSPIC30F4013")
     #define CAN_BRG_PRESCALAR         3
     #define CAN_BRG_PHASE_SEGMENT_1   3
     #define CAN_BRG_PHASE_SEGMENT_2   3
     #define CAN_BRG_PROPAGATION_TIME  0
     #define CAN_BRG_SYNCH_JUMP_WIDTH  0
    #else
     #define CAN_BRG_PRESCALAR         1
     #define CAN_BRG_PHASE_SEGMENT_1   3
     #define CAN_BRG_PHASE_SEGMENT_2   3
     #define CAN_BRG_PROPAGATION_TIME  0
     #define CAN_BRG_SYNCH_JUMP_WIDTH  0
    #endif
   #elif J1939_BAUD_RATE == 500000
    #if defined(__PCH__) || (getenv("DEVICE") == "DSPIC30F6010A") || (getenv("DEVICE") == "DSPIC30F6011A") || (getenv("DEVICE") == "DSPIC30F6012A") || (getenv("DEVICE") == "DSPIC30F6013A") || (getenv("DEVICE") == "DSPIC30F6014A") || (getenv("DEVICE") == "DSPIC30F6015") || \
        (getenv("DEVICE") == "DSPIC30F5011") || (getenv("DEVICE") == "DSPIC30F5013") || (getenv("DEVICE") == "DSPIC30F5015") || (getenv("DEVICE") == "DSPIC30F5016") || \
        (getenv("DEVICE") == "DSPIC30F4011") || (getenv("DEVICE") == "DSPIC30F4012") || (getenv("DEVICE") == "DSPIC30F4013")
     #define CAN_BRG_PRESCALAR         1
     #define CAN_BRG_PHASE_SEGMENT_1   3
     #define CAN_BRG_PHASE_SEGMENT_2   3
     #define CAN_BRG_PROPAGATION_TIME  0
     #define CAN_BRG_SYNCH_JUMP_WIDTH  0
    #else
     #define CAN_BRG_PRESCALAR         0
     #define CAN_BRG_PHASE_SEGMENT_1   3
     #define CAN_BRG_PHASE_SEGMENT_2   3
     #define CAN_BRG_PROPAGATION_TIME  0
     #define CAN_BRG_SYNCH_JUMP_WIDTH  0
    #endif
   #else
    #error Please define BRG Prescalar, Phase Segments, Propagation Time and Synch Jump Width for J1939 Baud Rate and PIC Clock Speed
   #endif
  #elif getenv("CLOCK") == 32000000
   #if J1939_BAUD_RATE == 250000
    #if defined(__PCH__) || (getenv("DEVICE") == "DSPIC30F6010A") || (getenv("DEVICE") == "DSPIC30F6011A") || (getenv("DEVICE") == "DSPIC30F6012A") || (getenv("DEVICE") == "DSPIC30F6013A") || (getenv("DEVICE") == "DSPIC30F6014A") || (getenv("DEVICE") == "DSPIC30F6015") || \
        (getenv("DEVICE") == "DSPIC30F5011") || (getenv("DEVICE") == "DSPIC30F5013") || (getenv("DEVICE") == "DSPIC30F5015") || (getenv("DEVICE") == "DSPIC30F5016") || \
        (getenv("DEVICE") == "DSPIC30F4011") || (getenv("DEVICE") == "DSPIC30F4012") || (getenv("DEVICE") == "DSPIC30F4013")
     #define CAN_BRG_PRESCALAR         3
     #define CAN_BRG_PHASE_SEGMENT_1   6
     #define CAN_BRG_PHASE_SEGMENT_2   6
     #define CAN_BRG_PROPAGATION_TIME  0
     #define CAN_BRG_SYNCH_JUMP_WIDTH  0
    #else
     #define CAN_BRG_PRESCALAR         1
     #define CAN_BRG_PHASE_SEGMENT_1   6
     #define CAN_BRG_PHASE_SEGMENT_2   6
     #define CAN_BRG_PROPAGATION_TIME  0
     #define CAN_BRG_SYNCH_JUMP_WIDTH  0
    #endif
   #elif J1939_BAUD_RATE == 500000
    #if defined(__PCH__) || (getenv("DEVICE") == "DSPIC30F6010A") || (getenv("DEVICE") == "DSPIC30F6011A") || (getenv("DEVICE") == "DSPIC30F6012A") || (getenv("DEVICE") == "DSPIC30F6013A") || (getenv("DEVICE") == "DSPIC30F6014A") || (getenv("DEVICE") == "DSPIC30F6015") || \
        (getenv("DEVICE") == "DSPIC30F5011") || (getenv("DEVICE") == "DSPIC30F5013") || (getenv("DEVICE") == "DSPIC30F5015") || (getenv("DEVICE") == "DSPIC30F5016") || \
        (getenv("DEVICE") == "DSPIC30F4011") || (getenv("DEVICE") == "DSPIC30F4012") || (getenv("DEVICE") == "DSPIC30F4013")
     #define CAN_BRG_PRESCALAR         1
     #define CAN_BRG_PHASE_SEGMENT_1   6
     #define CAN_BRG_PHASE_SEGMENT_2   6
     #define CAN_BRG_PROPAGATION_TIME  0
     #define CAN_BRG_SYNCH_JUMP_WIDTH  0
    #else
     #define CAN_BRG_PRESCALAR         0
     #define CAN_BRG_PHASE_SEGMENT_1   6
     #define CAN_BRG_PHASE_SEGMENT_2   6
     #define CAN_BRG_PROPAGATION_TIME  0
     #define CAN_BRG_SYNCH_JUMP_WIDTH  0
    #endif
   #else
    #error Please define BRG Prescalar, Phase Segments, Propagation Time and Synch Jump Width for J1939 Baud Rate and PIC Clock Speed
   #endif
  #elif getenv("CLOCK") == 40000000
   #if J1939_BAUD_RATE == 250000
    #if defined(__PCH__) || (getenv("DEVICE") == "DSPIC30F6010A") || (getenv("DEVICE") == "DSPIC30F6011A") || (getenv("DEVICE") == "DSPIC30F6012A") || (getenv("DEVICE") == "DSPIC30F6013A") || (getenv("DEVICE") == "DSPIC30F6014A") || (getenv("DEVICE") == "DSPIC30F6015") || \
        (getenv("DEVICE") == "DSPIC30F5011") || (getenv("DEVICE") == "DSPIC30F5013") || (getenv("DEVICE") == "DSPIC30F5015") || (getenv("DEVICE") == "DSPIC30F5016") || \
        (getenv("DEVICE") == "DSPIC30F4011") || (getenv("DEVICE") == "DSPIC30F4012") || (getenv("DEVICE") == "DSPIC30F4013")
     #define CAN_BRG_PRESCALAR         4
     #define CAN_BRG_PHASE_SEGMENT_1   6
     #define CAN_BRG_PHASE_SEGMENT_2   6
     #define CAN_BRG_PROPAGATION_TIME  0
     #define CAN_BRG_SYNCH_JUMP_WIDTH  0
    #else
     #define CAN_BRG_PRESCALAR         3
     #define CAN_BRG_PHASE_SEGMENT_1   3
     #define CAN_BRG_PHASE_SEGMENT_2   3
     #define CAN_BRG_PROPAGATION_TIME  0
     #define CAN_BRG_SYNCH_JUMP_WIDTH  0
    #endif
   #elif J1939_BAUD_RATE == 500000
    #if defined(__PCH__) || (getenv("DEVICE") == "DSPIC30F6010A") || (getenv("DEVICE") == "DSPIC30F6011A") || (getenv("DEVICE") == "DSPIC30F6012A") || (getenv("DEVICE") == "DSPIC30F6013A") || (getenv("DEVICE") == "DSPIC30F6014A") || (getenv("DEVICE") == "DSPIC30F6015") || \
        (getenv("DEVICE") == "DSPIC30F5011") || (getenv("DEVICE") == "DSPIC30F5013") || (getenv("DEVICE") == "DSPIC30F5015") || (getenv("DEVICE") == "DSPIC30F5016") || \
        (getenv("DEVICE") == "DSPIC30F4011") || (getenv("DEVICE") == "DSPIC30F4012") || (getenv("DEVICE") == "DSPIC30F4013")
     #define CAN_BRG_PRESCALAR         1
     #define CAN_BRG_PHASE_SEGMENT_1   7
     #define CAN_BRG_PHASE_SEGMENT_2   7
     #define CAN_BRG_PROPAGATION_TIME  2
     #define CAN_BRG_SYNCH_JUMP_WIDTH  0
    #else
     #define CAN_BRG_PRESCALAR         0
     #define CAN_BRG_PHASE_SEGMENT_1   7
     #define CAN_BRG_PHASE_SEGMENT_2   7
     #define CAN_BRG_PROPAGATION_TIME  2
     #define CAN_BRG_SYNCH_JUMP_WIDTH  0
    #endif
   #else
    #error Please define BRG Prescalar, Phase Segments, Propagation Time and Synch Jump Width for J1939 Baud Rate and PIC Clock Speed
   #endif
  #else
   #error Please define BRG Prescalar, Phase Segments, Propagation Time and Synch Jump Width for J1939 Baud Rate and PIC Clock Speed
  #endif
 #else
  #if J1939_BAUD_RATE == 250000
   #define CAN_BRG_PRESCALAR         3
   #define CAN_BRG_PHASE_SEGMENT_1   3
   #define CAN_BRG_PHASE_SEGMENT_2   3
   #define CAN_BRG_PROPAGATION_TIME  0
   #define CAN_BRG_SYNCH_JUMP_WIDTH  0
   #error/warning This assumes the External CAN chip is clocked with a 20MHz crystal
  #elif J1939_BAUD_RATE == 500000
   #define CAN_BRG_PRESCALAR         1
   #define CAN_BRG_PHASE_SEGMENT_1   3
   #define CAN_BRG_PHASE_SEGMENT_2   3
   #define CAN_BRG_PROPAGATION_TIME  0
   #define CAN_BRG_SYNCH_JUMP_WIDTH  0
   #error/warning This assumes the External CAN chip is clocked with a 20MHz crystal
  #else
   #error Please define BRG Prescalar, Phase Segments, Propagation Time and Synch Jump Width for J1939 Baud Rate and PIC Clock Speed
  #endif
 #endif
#endif

//////////////////////////////////////////////////////////////////////////////// Prototypes

void J1939Init(void);
#separate
void J1939ReceiveTask(void);
#separate
void J1939XmitTask(void);
int1 J1939Kbhit(void);
int1 J1939GetMessage(J1939_PDU_STRUCT &PDU, uint8_t *Data, uint8_t &Length);
int1 J1939PutMessage(J1939_PDU_STRUCT PDU, uint8_t *Data, uint8_t Bytes);
void J1939RequestAddress(uint8_t address);
void J1939ClaimAddress(void);
int1 J1939CheckName(uint8_t *data);
void J1939HandleAddressRequest(J1939_PDU_STRUCT PDU);
void J1939LoadReceiveBuffer(J1939_PDU_STRUCT ReceivedPDU,uint8_t *Data,uint8_t length);
void J1939HandleAddressClaim(J1939_PDU_STRUCT ReceivedPDU, uint8_t *Name);
void J1939SetCANFilter(uint8_t address);
uint8_t xor8(void);

#endif
