///////////////////////////////////////////////////////////////////////////////
////                                                                       ////
////                              ZMD44102.H                               ////
////                                                                       ////
////                                                                       ////
//// zmd_init() - Initializes the hardware.  Does not inializes the API.   ////
////              Normally you want to call ZMac_Init() instead.           ////
////                                                                       ////
//// ZMac_Init() - Initializes the hardware and the API.                   ////
////                                                                       ////
//// zmd_force_sleep() - If the GPD pin is connected, will use the GPD     ////
////     pin to put the radio to sleep.                                    ////
////                                                                       ////
//// zmd_force_wake() - If the GPD pin is connected, will deassert the GPD ////
////     pin to stop it from sleeping                                      ////
////                                                                       ////
//// zmd_reset() - Performs a hard reset of the unit by toggling the       ////
////     reset PIN.                                                        ////
////                                                                       ////
//// zmd_set_maccontrol(om) - Sets the operating mode of the radio.        ////
////     Normally you don't want to do this, let the API determine the     ////
////     operating mode needed.                                            ////
////                                                                       ////
//// zmd_storeLQI(doStoreAck?) - Tell the ZMD44102 radio to store or not   ////
////     store LQI at the end of each packet.  Normally ZMac_Init() will   ////
////     turn this off.                                                    ////
////                                                                       ////
//// lqi=zmd_getLQI() - If LQI is appended to the end of the packet, use   ////
////     this to read the LQI.  You must call this after ZMac_DiscardRX()  ////
////     if LQI is enabled.                                                ////
////                                                                       ////
//// ZPhy_SetChannel(channel) - Configure the ZMD44102 to use this 900MHz  ////
////     channel.  Channel 0 is for Europe, Channel 1-10 are for North     ////
////     America.                                                          ////
////                                                                       ////
//// channel=ZPhy_GetChannel() - Get the current channel being used.       ////
////                                                                       ////
//// ZPhy_SetTX(ZPHY_TX_MODE, ZPHY_TX_POWER) - Configure the PHY layer     ////
////     TX settings (testing mode, power, etc).                           ////
////     See ZPHY_TX_MODE and ZPHY_TX_POWER.  Max power is the default.    ////
////                                                                       ////
//// ZMac_SetTX(ZMAC_TX_MODE, ZMAC_TX_SLOTTED) - Configure the MAC         ////
////     layer TX settings (slotted mode, collision detection).            ////
////     Collision detection is enabled and slotted mode is disabled       ////
////     by default.                                                       ////
////                                                                       ////
//// ZMac_SetMyShortAddress(shortAddress) - Set this to be our short       ////
////     16bit address.  This is the address that will be used for         ////
////     transmitting packets.  This is also the address used by the       ////
////     receive filter.                                                   ////
////                                                                       ////
//// ZMac_SetMyLongAddress(longAddress) - Set this to be our long          ////
////     64bit address.  This is the address that will be used for         ////
////     transmitting packets.  This is also the address used by the       ////
////     receive filter.                                                   ////
////                                                                       ////
//// ZMac_SetMyPanId(*longAddress) - Set this to be our PAN ID             ////
////     address.  This is the address that will be used for               ////
////     transmitting packets.  This is also the address used by the       ////
////     receive filter.                                                   ////
////                                                                       ////
//// ZMac_UseMyShortAddress() - Tell the API to use the short address for  ////
////     future transmitted packets.  The receive filter will use both     ////
////     short and long address.                                           ////
////                                                                       ////
//// ZMac_UseMyLongAddress() - Tell the API to use the long address for    ////
////     future transmitted packets.  The receive filter will use both     ////
////     short and long address.                                           ////
////                                                                       ////
//// shortAddress=ZMac_GetMyShortAddress() - Returns our current short     ////
////     address.                                                          ////
////                                                                       ////
//// ZMac_GetMyPanId() - Returns our current PAN id.                       ////
////                                                                       ////
//// ZMac_GetMyLongAddress(*longAddress) - Saves our current long to       ////
////     longAddress.                                                      ////
////                                                                       ////
//// ec=ZMac_StartListen() - Turns on the receiver, indefinately.          ////
////                                                                       ////
//// ec=ZMac_StopListen() - Turns off the receiver.                        ////
////                                                                       ////
//// ZMac_StartPromiscous() - Turns off address filtering and lets ACK     ////
////     packets be stored into the receive buffer.  Useful if you want    ////
////     to packet sniff.                                                  ////
////                                                                       ////
//// ZMac_StopPromiscous() - Returns the address filtering to normal.      ////
////                                                                       ////
//// ZMac_IsRXReady() - Returns TRUE if there a packet in the receive      ////
////     buffer.                                                           ////
////                                                                       ////
//// count=ZMac_GetHeader(*header) - Reads the 802.15.4 MAC header and     ////
////     saves to the pointer, and returns the number of bytes in the      ////
////     802.15.4 payload.  Don't call this until ZMac_IsRxReady()         ////
////     returns TRUE.                                                     ////
////                                                                       ////
//// c=ZMac_Getc() - Reads and returns the next char in the receive        ////
////     buffer.  Should not be called until after ZMac_GetHeader() has    ////
////     been called.                                                      ////
////                                                                       ////
//// ZMac_Getw(*w) - Reads and saves the next word to the w pointer.       ////
////     Should not be called until after ZMac_GetHeader() has been        ////
////     called.                                                           ////
////                                                                       ////
//// ZMac_Getd(*ptr, len) - Reads len bytes from the receive buffer and    ////
////     saves them to ptr.  Should not be called until after              ////
////     ZMac_GetHeader() has been called.                                 ////
////                                                                       ////
//// ec=ZMac_DiscardRX() - Discards the current packet in the receive      ////
////     buffer.  Must be called for each packet, even if you read all the ////
////     data in the receive buffer.                                       ////
////                                                                       ////
//// ec=ZMac_DiscardRXAll() - Discards ALL packets in the receive buffer.  ////
////                                                                       ////
//// ZMac_IsTXReady() - Returns TRUE if the radio is ready to transmit a   ////
////     packet.                                                           ////
////                                                                       ////
//// ZMac_PutHeader(*remoteNode, frameCon) - Puts an 802.15.4 header into  ////
////     the transmit buffer.  remoteNode is the destination address       ////
////     of the packet.  The source address of the packet will be our      ////
////     address.  frameCon is the most significant byte of the frame      ////
////     control word (except for IntraPan, which is done depending on     ////
////     the remoteNode address).  This should not be called until         ////
////     ZMac_IsTXReady() returns TRUE.  The sequence number increments    ////
////     after each call.                                                  ////
////                                                                       ////
//// ZMac_Putc(c) - Puts the specified char into the TX FIFO.  Should      ////
////     not be called until after ZMac_IsTXReady() returns TRUE and       ////
////     after ZMac_PutHeader() prepared the packet.                       ////
////                                                                       ////
//// ZMac_Putd(*ptr, len) - Puts len bytes from ptr into the TX FIFO.      ////
////     Should not be called until after ZMac_IsTXReady() returns TRUE    ////
////     and after ZMac_PutHeader() prepared the packet.                   ////
////                                                                       ////
//// ec=ZMac_FlushTx(waitForAck) - Marks the packet in the transmit        ////
////     buffer as ready to transmit.  If using CSMA, it will wait until   ////
////     after the collision detection has passed.  If waitForAck is TRUE  ////
////     then it will wait to see if the radio received an ACK.  The radio ////
////     has it's own internal timeout mechanism for waiting for an ACK.   ////
////     Returns 0 if success, non-zero if failure (collision, no ack,     ////
////      etc.)                                                            ////
////                                                                       ////
////  ec=ZMac_DiscardTX() - Discards any data in the transmit buffer, so   ////
////     you can start a new packet.                                       ////
////                                                                       ////
//// --------------------------------------------------------------------- ////
////                                                                       ////
//// The following functions take the above API to give you simpler        ////
//// functions for reading and writing a packet:                           ////
////                                                                       ////
//// ec=ZMac_PutPacket(*remoteNode, *ptr, len) - Sends a packet, len bytes ////
////     from pointer, to the remoteNode.  Returns 0 if sent successfully, ////
////     non-zero if error (collision, no ack, etc).                       ////
////                                                                       ////
//// ec=ZMac_FlushPacket(*ptr, count, waitForAck) - If ZMac_PutPacket()    ////
////     returns an error, you can use this to attempt to retransmit       ////
////     the packet.  This is similar to ZMac_PutPacket(), but will not    ////
////     retransmit the header or increment the sequence number.           ////
////                                                                       ////
//// count=ZMac_GetPacket(*remoteNode, *ptr, max, t) - Turns on the        ////
////     receiver and waits t milliseconds for an incoming packet.  If     ////
////     a packet is received, remoteNode is updated and the payload (up   ////
////     to max bytes) is saved to ptr.  count returns the number of bytes ////
////     saved to ptr.  The receiver is disabled when done.                ////
////                                                                       ////
///////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2006 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////


#include <string.h>

#ifndef ZMD_PIN_RSN
 #define ZMD_PIN_ALE       PIN_E2
 #define ZMD_PIN_RD        PIN_E0
 #define ZMD_PIN_WR        PIN_E1
 #define zmd_read_port     input_d
 #define zmd_write_port    output_d
 #define zmd_float_port    input_d

 #define ZMD_PIN_SS        PIN_C2
 #define ZMD_PIN_MOSI      PIN_C4
 #define ZMD_PIN_MISO      PIN_C5
 #define ZMD_PIN_SCLK      PIN_C3

 #define ZMD_PIN_IRQ       PIN_B0
 #define ZMD_PIN_GPD       PIN_B1
 #define ZMD_PIN_RSN       PIN_C1
#endif

#ifndef ZMD_USE_PARALLEL
#define ZMD_USE_PARALLEL   FALSE
#endif


typedef enum
{
   ZPHY_TX_MODE_NORMAL = 0,
   ZPHY_TX_MODE_CARRIER = 2,  //generate a carrier wave
   ZPHY_TX_MODE_MODULATED_CARRIER = 4  //generated a modulated carrier wave
} ZPHY_TX_MODE;

typedef enum
{
   ZMAC_ADDRESS_MODE_NONE = 0x00,
   ZMAC_ADDRESS_MODE_SHORT = 0x08,
   ZMAC_ADDRESS_MODE_LONG = 0x0C
} ZMAC_ADDRESS_MODE;

typedef enum
{
   ZPHY_TX_POWER_MAX = 0,
   ZPHY_TX_POWER_0DBM = 0,
   ZPHY_TX_POWER_neg7DBM = 1,
   ZPHY_TX_POWER_neg14DBM = 2,
   ZPHY_TX_POWER_neg21DBM = 3,
   ZPHY_TX_POWER_MIN = 3
} ZPHY_TX_POWER;

typedef enum
{
   ZMAC_TX_MODE_GTS = 0,
   ZMAC_TX_MODE_CSMA = 1,
   ZMAC_TX_MODE_DIRECT = 2
} ZMAC_TX_MODE;

typedef enum
{
   ZMAC_TX_SLOTTED_DISABLE = 0,
   ZMAC_TX_SLOTTED_ENABLE = 1,
   ZMAC_TX_SLOTTED_ENABLE_WITH_ACK = 0x11
} ZMAC_TX_SLOTTED;

typedef union
{
   int8 b[2];
   int16 w;
} ZMAC_SHORT_ADDRESS;

typedef union
{
   int8 b[8];
   int16 w[4];
} ZMAC_LONG_ADDRESS;

typedef struct {
   ZMAC_SHORT_ADDRESS panId;
   ZMAC_SHORT_ADDRESS shortAddress;
   ZMAC_LONG_ADDRESS longAddress;
   ZMAC_ADDRESS_MODE addressMode;
} ZMAC_NODE_INFO;

typedef struct {
   int frameType:3;          //bit 0:2
   int securityEnabled:1;    //bit 3
   int framePending:1;       //bit 4
   int ackRequested:1;       //bit 5
   int intraPan:1;            //bit 6
   int reserved:1;            //bit 7
} ZMAC_MHRFC1TX;

#define ZMAC_FCON_TYPE_BEACON       0x00
#define ZMAC_FCON_TYPE_DATA         0x01
#define ZMAC_FCON_TYPE_ACK          0x02
#define ZMAC_FCON_TYPE_CMD          0x03

#define ZMAC_FCON_SECURITY_YES      0x08
#define ZMAC_FCON_SECURITY_NO       0x00

#define ZMAC_FCON_FRAME_PENDING_YES 0x10
#define ZMAC_FCON_FRAME_PENDING_NO  0x00

#define ZMAC_FCON_ACK_REQUESTED_YES 0x20
#define ZMAC_FCON_ACK_REQUESTED_NO  0x00

#define ZMAC_FCON_INTRAPAN_YES      0x40
#define ZMAC_FCON_INTRAPAN_NO       0x40

typedef struct {
   int reserved:2;            //bit 0:1
   ZMAC_ADDRESS_MODE destAddressMode:2;   //bit 2:3
   int reserved2:2;           //bit 4:5
   ZMAC_ADDRESS_MODE sourceAddressMode:2; //bit 6:7
} ZMAC_MHRFC2TX;

typedef struct {
   ZMAC_MHRFC1TX frameCon;
   int8 seq;
   ZMAC_NODE_INFO sourceNode;
   ZMAC_NODE_INFO destNode;
} ZMAC_HEADER;

//write to a file register on the zmd
void zmd_write_reg(int8 address, int8 data) {
   #if ZMD_USE_PARALLEL
      output_high(ZMD_PIN_ALE);
      output_high(ZMD_PIN_WR);
      output_high(ZMD_PIN_RD);
      zmd_write_port(address);
     #if (getenv("CLOCK")>20000000)
      delay_cycles(3);
     #endif
      output_low(ZMD_PIN_ALE);
      zmd_write_port(data);
      output_low(ZMD_PIN_WR);
      delay_us(1);
      output_high(ZMD_PIN_WR);
      output_high(ZMD_PIN_ALE);
   #else
      int8 i,cmd[3];
      cmd[2]=1;
      cmd[1]=address;
      cmd[0]=data;
      output_low(ZMD_PIN_SCLK);
      output_low(ZMD_PIN_SS);
      for (i=0; i<24; i++) {
         output_bit(ZMD_PIN_MOSI, shift_left(&cmd[0],3,0));
         output_high(ZMD_PIN_SCLK);
         delay_us(1);
         output_low(ZMD_PIN_SCLK);
         delay_us(1);
      }
      output_high(ZMD_PIN_SS);
   #endif
}

//read a file register on the zmd
int8 zmd_read_reg(int8 address) {
   int8 data;
   #if ZMD_USE_PARALLEL
      output_high(ZMD_PIN_ALE);
      output_high(ZMD_PIN_WR);
      output_high(ZMD_PIN_RD);
      zmd_write_port(address);
     #if (getenv("CLOCK")>20000000)
      delay_cycles(3);
     #endif
      output_low(ZMD_PIN_ALE);
      output_low(ZMD_PIN_RD);
      delay_us(1);
      data=zmd_read_port();
      output_high(ZMD_PIN_RD);
      output_high(ZMD_PIN_ALE);
   #else
      int8 i,cmd[2];
      cmd[1]=0x81;
      cmd[0]=address;
      output_low(ZMD_PIN_SCLK);
      output_low(ZMD_PIN_SS);
      for (i=0; i<16; i++) {
         output_bit(ZMD_PIN_MOSI, shift_left(&cmd[0],2,0));
         output_high(ZMD_PIN_SCLK);
         delay_us(1);
         output_low(ZMD_PIN_SCLK);
         delay_us(1);
      }
      output_high(ZMD_PIN_SS);
      delay_us(2);
      output_low(ZMD_PIN_SS);
      for (i=0; i<8; i++) {
         shift_left(&data,1,input(ZMD_PIN_MISO));
         output_high(ZMD_PIN_SCLK);
         delay_us(1);
         output_low(ZMD_PIN_SCLK);
         delay_us(1);
      }
      output_high(ZMD_PIN_SS);
   #endif
   return(data);
}

#if defined(ZMD_PIN_GPD)
 #define zmd_force_sleep()   output_high(ZMD_PIN_GPD)
 #define zmd_force_wake()    output_low(ZMD_PIN_GPD)
#else
 #define zmd_force_sleep()
 #define zmd_force_wake()
#endif

void zmd_reset(void) {
   output_low(ZMD_PIN_RSN);
   delay_ms(10);
   output_high(ZMD_PIN_RSN);
   delay_ms(1);
}

//ZIGBEE ZMD44102 REGISTERS
//irq control registers
#define ZMD_IRQreason      0xAC  //Interrupt Request Reason [7:0]
#define ZMD_IRQmask        0xAD  //Interrupt Condition Mask [22:0]
#define ZMD_IRQmask1       0xAD
#define ZMD_IRQmask2       0xAE
#define ZMD_IRQmask3       0xAF
#define ZMD_IRQmask4       0xB0

//phy registers
#define ZMD_RPCC           0x00  //PHY Current Channel [7:0]
#define ZMD_RTXM           0x05  //PHY Transmitter Mode Register [7:0]
#define ZMD_RAGCM          0x0D  //AGC Mode Register[3:0]

//mac operating control
#define ZMD_macControl     0xA0  //MAC Control Commands [4:0]
#define ZMD_macControlT    0xA1  //MAC Control Commands [4:0]
#define ZMD_macOpMode      0xA2  //MAC Operating Mode [3:0]

//mac fifo register
#define ZMD_WrTxFifo       0x80  //Transmit FIFO Write Access [7:0]
#define ZMD_RdRxFifo       0x81  //Receive FIFO Read Access [7:0]
#define ZMD_macFifoStatus  0xA9  //MAC TX/RX FIFO Status [3:0]

//mac tx control
#define ZMD_macTxConfig    0xB1  //MAC Transmitter Configuration [3:0]
#define ZMD_macTxStatus    0xA3  //MAC Transmit Status [6:0]
#define ZMD_msduLengthTx   0x60  //Transmit Frame MSDU Length [6:0]
#define ZMD_mhrFc1Tx       0x61
#define ZMD_mhrFc2Tx       0x62
#define ZMD_mhrSqnNbTx     0x63  //Transmit Frame MAC Header Sequence Number Field
#define ZMD_mhrDstPanIdTx  0x64  //Tx Frame MAC Header Dest. PAN Identifier Field
#define ZMD_mhrDstAddr16_1Tx 0x66  //Tx Frame MAC Header 16bit Dest. Address Field [15:0]
#define ZMD_mhrDstAddr64_1Tx 0x68  //Tx Frame MAC Header 64bit Dest. Address Field [63:0]
#define ZMD_mhrSrcPanId1Tx  0x70  //Transmit Frame MAC Header Source PAN Identifier Field [15:0]
#define ZMD_mhrSrcAddr16_1Tx 0x72  //Transmit Frame MAC Header 16bit Source Address Field [15:0]
#define ZMD_mhrSrcAddr64_1Tx 0x74  //Transmit Frame MAC Header 64bit Source Address Field [63:0]

//mac rx control
#define ZMD_RxMode         0x08
#define ZMD_macRxStatus    0xA4  //MAC Receive Status [7:0]
#define ZMD_macRxConfig    0xB2
#define ZMD_macFramePend   0x88  //Frame Pending Number

//mac frame filter control
#define ZMD_macFilterConfig 0xB6 //MAC Frame Filter Configuration [3:0] 95
#define ZMD_macFilterStatus 0xAA //MAC Frame Filter Status [2:0] 72
#define ZMD_macPanId1 0x89  //Reference MAC PAN Identifier 0x89-0x8A [15:0]
#define ZMD_macShortAddr1 0x8B //Reference MAC Short Address 0x8B-0x8C [15:0] 96
#define ZMD_aExtendedAddr1 0x8D //aExtendedAddr – Reference Extended Address 0x8D-0x94 [15:0] 97

#define ZMD_macCoordSrcAddr   0x95
#define ZMD_macAutoBcTrStatus 0xA7
#define ZMD_macAutoBcTxStatus 0xA8

//spi registers
#define ZMD_SPIconfig      0xFB  //SPI Configuration [5:0]

//recommended default register setup
#define ZMD_MTPcontrol     0x37  //Multiple Times Programmable (MTP) Memory Control [3:0]

//dropped registers???
#define ZMD_ED0            0x22  //Energy Detection Value Offset[7:0]

//macControl settings
#define ZMD_mc_NoCmd          0x00
#define ZMD_mc_Sleep          0x01
#define ZMD_mc_TxRxOff        0x02
#define ZMD_mc_TxOn           0x03
#define ZMD_mc_RxOn           0x04
#define ZMD_mc_ScanOn         0x05
#define ZMD_mc_ScanOff        0x06
#define ZMD_mc_ScanCont       0x07
#define ZMD_mc_BcTrOn         0x08
#define ZMD_mc_BcTrSleep      0x09
#define ZMD_mc_BcTrOff        0x0A
#define ZMD_mc_BcOk           0x0B
#define ZMD_mc_BcFail         0x0C
#define ZMD_mc_AutoBcTxOn     0x0D
#define ZMD_mc_AutoBcTxOff    0x0E
#define ZMD_mc_TxFifoFlush    0x13
#define ZMD_mc_RxFifoFlush    0x14
#define ZMD_mc_CmdError       0x1F

//macOpMode settings
#define ZMD_om_Idle          0x00
#define ZMD_om_PdnSleep      0x01
#define ZMD_om_Tx            0x02
#define ZMD_om_RxIdle        0x03
#define ZMD_om_RxActive      0x04
#define ZMD_om_ScanTx        0x05
#define ZMD_om_ScanRx        0x06
#define ZMD_om_BcTr          0x07
#define ZMD_om_AutoBcTx      0x08
#define ZMD_om_PhySwitch     0x09

typedef enum {
   ZMD_EC_OK = 0,
   ZMD_EC_INVALID_CMD = 1, //ZMD responded with invalid command after a write to macControl
   ZMD_EC_CMD_TO = 2,       //ZMD timedout waiting for macControl execution
   ZMD_EC_RX_FIFO_EMPTY = 3,  //Application tried to read mac payload when there was no data in the RX fifo
   ZMD_EC_RX_BAD_ADDRESS = 4,  //802.15.4 packet received wasn't addressed to us
   ZMD_EC_RX_NOT_DATA = 5,     //Packet in RX FIFO wasn't a Data/CMD
   ZMD_EC_TX_TIMEOUT = 6,       //timeout waiting for TX IRQ
   ZMD_EC_TX_GTSfail = 7,   //GTS transmission feailed due unavailable slot
   ZMD_EC_TX_CAPfail = 8,  //transmission failed due to channel access failure because of CAP end
   ZMD_EC_TX_Chbusy = 9,    //transmission failed due to channel access failure because of busy channel
   ZMD_EC_TX_NOIRQ = 10,    //we got a TX IRQ but macTxStatus isn't valid
   ZMD_EC_FIFO_ERROR = 11,
   ZMD_EC_UNKNOWN_INT = 12,
   ZMD_EC_TX_BUSY = 13, //ZMD is still busy with the last frame transmit
   ZMD_EC_TX_SFEnd = 14,  //Transmit frame failed because super frame ended
   ZMD_EC_RX_BUSY = 15, //ZMD is still busy acquiring or frame reception
   ZMD_EC_RX_ACK = 16,  //received an ack packet
   ZMD_EC_RX_ACKTimeOut = 17, //timeout waiting for ack
   ZMD_EC_RX_ACKFramePend = 18,  //received an ack packet with the frame pending bit set
   ZMD_EC_RX_POLLNoData = 19, //no data frame received after receiving an ack with the frame pending bit set
   ZMD_EC_RX_POLLCAPend = 20, //no pending data received within the current cap, will continue in the next cap
   ZMD_EC_RX_DATA = 21, //frame without the ack requested received
   ZMD_EC_RX_DATAAck = 22  //frame with an ack requested received
} ZMD_EC;

//write to macControl register and wait for command execution
//to is timeout in ms
ZMD_EC __zmd_set_maccontrol(int8 new_maccontrol, int16 to) {
   int8 resp;
   zmd_write_reg(ZMD_macControl,new_maccontrol);

   do {
      resp=zmd_read_reg(ZMD_macControl);
      if (resp==0x1F) {return(ZMD_EC_INVALID_CMD);}
      if (resp) {
         delay_ms(1);
         to--;
      }
   } while (resp && to);

   if (resp)
      return(ZMD_EC_CMD_TO);
   else
      return(ZMD_EC_OK);
}

#define zmd_set_maccontrol(x) __zmd_set_maccontrol(x,50)
#define zmd_putc(x) zmd_write_reg(ZMD_WrTxFifo, x)
#define zmd_getc() zmd_read_reg(ZMD_RdRxFifo)

void zmd_storeLQI(int1 storeLQI)
{
   int8 scratch;

   scratch=zmd_read_reg(ZMD_macRxConfig);

   if (storeLQI)
      bit_set(scratch,4);
   else
      bit_clear(scratch,4);

   zmd_write_reg(ZMD_macRxConfig, scratch);
}

//call this after ZMac_DiscardRX()
int16 zmd_getLQI(void)
{
   union
   {
      char b[2];
      int16 w;
   } lqi;

   lqi.b[0]=zmd_getc();
   lqi.b[1]=zmd_getc();
   return(lqi.w);
}

void zmd_init(void)
{
  #if defined(ZMD_PIN_SS)
   output_high(ZMD_PIN_SS);
  #endif

  #if defined(ZMD_PIN_ALE)
   output_high(ZMD_PIN_ALE);
  #endif

  #if defined(ZMD_PIN_WR)
   output_high(ZMD_PIN_WR);
  #endif

  #if defined(ZMD_PIN_RD)
   output_high(ZMD_PIN_RD);
  #endif

  #if !ZMD_USE_PARALLEL
   output_low(ZMD_PIN_SCLK);
  #endif

   zmd_force_wake();
   zmd_reset();

  #if ZMD_USE_PARALLEL
   zmd_write_reg(ZMD_SPIconfig,0);
  #else
   zmd_write_reg(ZMD_SPIconfig,0x20);
  #endif

   zmd_set_maccontrol(ZMD_om_Idle);
}

ZMAC_NODE_INFO g_MyZMACNode;
int g_ZMacTxFrameLen;
int g_ZMacRxFrameRemaining;
int g_ZMacTxSqNum;
int g_macFramePend=0;

//Set the RF channel.
//if channel=0, sets RxUsMode (bit 6 of 0x08) to 0
//if channel!=0, sets RxUsMode to 1
void ZPhy_SetChannel(int8 channel)
{
   int8 rxmode;

   zmd_write_reg(ZMD_RPCC, channel);

   rxmode=zmd_read_reg(ZMD_RxMode);

   if (channel==0)
      bit_clear(rxmode,6);
   else
      bit_set(rxmode,6);

   zmd_write_reg(ZMD_RxMode, rxmode);
}

#define ZPhy_GetChannel()  (zmd_read_reg(ZMD_RPCC))

//Set Phy TX Mode
void ZPhy_SetTX(ZPHY_TX_MODE mode, ZPHY_TX_POWER power)
{
   struct
   {
      int txmode:3;
      int txiomode:1;
      int sel_pa_lp:2;
      int reserved:2;
   } rtxm;

   rtxm=zmd_read_reg(ZMD_RTXM);
   rtxm.txmode=mode;
   rtxm.sel_pa_lp=power;

   zmd_write_reg(ZMD_RTXM, (int8)rtxm);
}

//Set MAC TX Mode
void ZMac_SetTX(ZMAC_TX_MODE mode, ZMAC_TX_SLOTTED slotted)
{
   struct
   {
      int EnableSlottedMode:1;
      int TxOption:2;
      int GoToRx:1;
      int EnableSlottedAck:1;
      int BatteryLifeExtension:1;
      int DirectFIFOAccess:1;
      int Reserved:1;
   } macTxConfig;

   macTxConfig=zmd_read_reg(ZMD_macTxConfig);
   (int8)macTxConfig &= 0xE8;
   macTxConfig.TxOption = mode;
   (int8)macTxConfig |= slotted;
   zmd_write_reg(ZMD_macTxConfig, (int8)macTxConfig);
}

//Set our 16bit address (this goes into the MAC receive filter, and the MAC src
//address)
void ZMac_SetMyShortAddress(int16 shortAddress)
{
   zmd_write_reg(ZMD_macShortAddr1, make8(shortAddress,0));
   zmd_write_reg(ZMD_macShortAddr1 + 1, make8(shortAddress,1));

   zmd_write_reg(ZMD_mhrSrcAddr16_1Tx, make8(shortAddress,0));
   zmd_write_reg(ZMD_mhrSrcAddr16_1Tx + 1, make8(shortAddress,1));

   g_MyZMACNode.shortAddress=shortAddress;
}


//Set our 64bit address (this goes into the MAC receive filter)
void ZMac_SetMyLongAddress(int8 *longAddress)
{
   int8 i;

   for (i=0;i<8;i++)
   {
      zmd_write_reg(ZMD_aExtendedAddr1 + i, *longAddress);
      zmd_write_reg(ZMD_mhrSrcAddr64_1Tx + i, *longAddress);
      longAddress++;
   }

   memcpy(&g_MyZMACNode.longAddress.b[0], longAddress, 8);
}

//Set our PanId (this goes into the MAC receive filter)
void ZMac_SetMyPanId(int16 panId)
{
   zmd_write_reg(ZMD_macPanId1, make8(panId,0));
   zmd_write_reg(ZMD_macPanId1 + 1, make8(panId,1));

   zmd_write_reg(ZMD_mhrSrcPanId1Tx, make8(panId,0));
   zmd_write_reg(ZMD_mhrSrcPanId1Tx + 1, make8(panId,1));

   g_MyZMACNode.panId=panId;
}

#define ZMac_UseMyShortAddress() (g_MyZMACNode.addressMode=ZMAC_ADDRESS_MODE_SHORT)
#define ZMac_UseMyLongAddress() (g_MyZMACNode.addressMode=ZMAC_ADDRESS_MODE_LONG)

#define ZMac_GetMyShortAddress() (g_MyZMACNode.shortAddress.w)
#define ZMac_GetMyPanId() (g_MyZMACNode.panId.w)

//Get our 64bit address
void ZMac_GetMyLongAddress(int8 *longAddress)
{
   int8 i;
   for (i=0;i<8;i++)
   {
      *longAddress=zmd_read_reg(ZMD_aExtendedAddr1 + i);
      longAddress++;
   }
}

//Put c into the TX FIFO
void ZMac_Putc(char c)
{
   g_ZMacTxFrameLen++;
   zmd_putc(c);
}

//Put len number of chars from c
void ZMac_Putd(char *c, int len)
{
   while(len--)
   {
      ZMac_Putc(*c);
      c++;
   }
}
//Goto RX mode.
//will stay in RX mode even after a receiving a packet (macRxConfig.ContRx=1),
//(and disable storing of acks in buffer)
//and will go back to listen mode after TX a packet (macTxConfig.GotoRx=1)
ZMD_EC ZMac_StartListen(void)
{
   int8 scratch;

   scratch=zmd_read_reg(ZMD_macRxConfig);
   bit_set(scratch,2);  //contrx
   bit_clear(scratch,3);   //storeack
   zmd_write_reg(ZMD_macRxConfig, scratch);

   scratch=zmd_read_reg(ZMD_macTxConfig);
   bit_set(scratch,3);
   zmd_write_reg(ZMD_macTxConfig, scratch);

   return(zmd_set_maccontrol(ZMD_mc_RxOn));
}

//Stop RX mode.
ZMD_EC ZMac_StopListen(void)
{
   char scratch;

   scratch=zmd_read_reg(ZMD_macRxConfig);
   bit_clear(scratch,2);
   zmd_write_reg(ZMD_macRxConfig, scratch);

   scratch=zmd_read_reg(ZMD_macTxConfig);
   bit_clear(scratch,3);
   zmd_write_reg(ZMD_macTxConfig, scratch);

   return(zmd_set_maccontrol(ZMD_mc_TxRxOff));
}

void ZMac_StartPromiscous(void)
{
   char scratch;

   scratch=zmd_read_reg(ZMD_macRxConfig);
   bit_clear(scratch,1);   //disable autoack
   bit_set(scratch,3);   //storeack
   zmd_write_reg(ZMD_macRxConfig, scratch);

   zmd_write_reg(ZMD_macFilterConfig, 1);
}

void ZMac_StopPromiscous(void)
{
   char scratch;

   scratch=zmd_read_reg(ZMD_macRxConfig);
   bit_set(scratch,1);   //reenable autoack
   bit_clear(scratch,3);   //storeack
   zmd_write_reg(ZMD_macRxConfig, scratch);

   zmd_write_reg(ZMD_macFilterConfig, 7);
}

//returns TRUE if the MAC is idle
int1 ZMac_IsTXReady(void)
{
   int8 state;

   state=zmd_read_reg(ZMD_macOpMode);

   return((state==ZMD_om_Idle)||(state==ZMD_om_RxIdle)||(state==ZMD_om_RxActive));
}

//Returns TRUE if there a packet in the FIFO
int1 ZMac_IsRXReady(void)
{
   if (zmd_read_reg(ZMD_macFramePend))
      g_macFramePend++;
   return(g_macFramePend);
}

//Get a char from the RX FIFO
char ZMac_Getc(void)
{
   char c;
   if (g_ZMacRxFrameRemaining)
   {
      g_ZMacRxFrameRemaining--;
      c=zmd_getc();
   }
   return(c);
}

void ZMac_Getw(int8 *w)
{
   *w++=ZMac_Getc();
   *w=ZMac_Getc();
}

//Gets chars from the RX FIFO
void ZMac_Getd(char *c, int len)
{
   while(len--)
   {
      *c=ZMac_Getc();
      c++;
   }
}


ZMD_EC ZMac_DiscardRX(void)
{
   while(g_ZMacRxFrameRemaining)
      ZMac_Getc();
   if (g_macFramePend)
      g_macFramePend--;
   return(0);
}

//Discards all frames in the RX fifo
ZMD_EC ZMac_DiscardRXAll(void)
{
   int8 scratch;

   g_macFramePend=0;

   do {
      scratch=zmd_read_reg(ZMD_macFramePend);
   } while (scratch);

   return(zmd_set_maccontrol(ZMD_mc_RxFifoFlush));
}

//Discards the current packet in the TX FIFO
ZMD_EC ZMac_DiscardTX(void)
{
   g_ZMacTxFrameLen=0;
   return(zmd_set_maccontrol(ZMD_mc_TxFifoFlush));
}

//
void ZMac_PutHeader(ZMAC_NODE_INFO *remoteNode, int8 frameCon)
{
   int8 mhr2;
   char *ptr, i;

   ZMac_DiscardTX();

   mhr2=remoteNode->addressMode;

   if ((int8)mhr2 == ZMAC_ADDRESS_MODE_SHORT)
   {
      zmd_write_reg(ZMD_mhrDstAddr16_1Tx + 0, remoteNode->shortAddress.b[0]);
      zmd_write_reg(ZMD_mhrDstAddr16_1Tx + 1, remoteNode->shortAddress.b[1]);
   }
   else
   {
      ptr=&remoteNode->longAddress.b[0];

      for (i=0; i<8; i++)
         zmd_write_reg(ZMD_mhrDstAddr64_1Tx + i, *ptr++);
   }

   zmd_write_reg(ZMD_mhrDstPanIdTx + 0, remoteNode->panId.b[0]);
   zmd_write_reg(ZMD_mhrDstPanIdTx + 1, remoteNode->panId.b[1]);

   zmd_write_reg(ZMD_mhrFc1Tx, frameCon);

   mhr2 |= (g_MyZMACNode.addressMode << 4);
   zmd_write_reg(ZMD_mhrFc2Tx, mhr2);

   zmd_write_reg(ZMD_mhrSqnNbTx, g_ZMacTxSqNum++);
}

void ZMac_GetAddress(ZMAC_NODE_INFO *node, ZMAC_ADDRESS_MODE addressMode)
{

   node->addressMode=addressMode;
   if (addressMode==ZMAC_ADDRESS_MODE_SHORT)
   {
      ZMac_Getw(&node->shortAddress);
   }
   else
   {
      ZMac_Getd(&node->longAddress, 8);
   }
}

//if header is null, it will not save header
int ZMac_GetHeader(ZMAC_HEADER *header)
{
   ZMAC_NODE_INFO node;
   int8 mhr1, mhr2, seq;

   g_ZMacRxFrameRemaining=zmd_getc();
   mhr1=ZMac_Getc();
   mhr2=ZMac_Getc();
   seq=ZMac_Getc();

   if (header)
   {
      header->seq=seq;
      header->frameCon=mhr1;
   }

   if ( (mhr1 & 0x07) == ZMAC_FCON_TYPE_ACK )
      return(0);

   if (mhr2 & 0x0C)
   {
      ZMac_Getw(&node.panId);
      ZMac_GetAddress(&node, mhr2 & 0x0C);

      if (header)
      {
         memcpy(&header->destNode, &node, sizeof(node));
      }
   }

   if (!bit_test(mhr1,6))
   {
      ZMac_Getw(&node.panId);
   }
   else
   {
      node.panId=ZMac_GetMyPanId();
   }

   ZMac_GetAddress(&node, (mhr2 & 0xC0)>>4);

   if (header)
      memcpy(&header->sourceNode, &node, sizeof(node));

   return(g_ZMacRxFrameRemaining);
}


ZMD_EC ZMac_LastRxStatus(void)
{
   int8 status;

   status=zmd_read_reg(ZMD_macRxStatus);

   //if (status & 0x03)
   //   return(ZMD_EC_RX_BUSY);

   switch(status & ~0x03)
   {
      case 0x10:  return(ZMD_EC_RX_ACK);
      case 0x14:  return(ZMD_EC_RX_ACKTimeOut);
      case 0x18:  return(ZMD_EC_RX_ACKFramePend);
      case 0x20:  return(ZMD_EC_RX_POLLNoData);
      case 0x24:  return(ZMD_EC_RX_POLLCAPend);
      case 0x80:  return(ZMD_EC_RX_DATA);
      case 0xC0:  return(ZMD_EC_RX_DATAAck);
   }

   return(ZMD_EC_RX_BUSY);
}

ZMD_EC ZMac_LastTxStatus(void)
{
   int8 status;

   status=zmd_read_reg(ZMD_macTxStatus);

   //if (status & 0x07)
   //   return(ZMD_EC_TX_BUSY);

   switch(status & ~0x07)
   {
      case 0x08:  return(ZMD_EC_OK);
      case 0x10:  return(ZMD_EC_TX_Chbusy);
      case 0x20:  return(ZMD_EC_TX_CAPfail);
      case 0x40:  return(ZMD_EC_TX_GTSfail);
      case 0x80:  return(ZMD_EC_TX_SFEnd);
   }

   return(ZMD_EC_TX_BUSY);
}

//if using long address you don't care about pan.
//if using short address, both short address and pan need to match.
int1 ZMac_AddressMatch(ZMAC_NODE_INFO *pnodeA, ZMAC_NODE_INFO *pnodeB)
{
   int addressMode;
   int1 ret=FALSE;

   addressMode=pnodeA->addressMode;

   if (addressMode == pnodeB->addressMode)
   {
      if (addressMode == ZMAC_ADDRESS_MODE_SHORT)
      {
         if (pnodeA->panId.w == pnodeB->panId.w)
            if (pnodeA->shortAddress.w == pnodeB->shortAddress.w)
               ret=TRUE;
      }
      else if (addressMode == ZMAC_ADDRESS_MODE_SHORT)
      {
         if (memcmp(&pnodeA->longAddress, &pnodeB->longAddress, sizeof(ZMAC_LONG_ADDRESS))==0)
            ret=TRUE;
      }
   }

   return(ret);
}

ZMD_EC ZMac_FlushTx(int1 waitForAck)
{
   ZMD_EC ec;
   int irq;

   zmd_read_reg(ZMD_IRQreason);

   zmd_write_reg(ZMD_msduLengthTx, g_ZMacTxFrameLen);
   ec=zmd_set_maccontrol(ZMD_mc_TxOn);
   if (ec) return(ec);

   do {
      irq=zmd_read_reg(ZMD_IRQreason);
      if (bit_test(irq,2))
      {
         ec=ZMac_LastTxStatus();
         break;
      }
   } while (TRUE);
   if (ec) return(ec);

   if (waitForAck)
   {
      do
      {
         irq=zmd_read_reg(ZMD_IRQreason);
         if (bit_test(irq,1))
         {
            ec=ZMac_LastRxStatus();
            break;
         }
      } while (TRUE);

      if (ec==ZMD_EC_RX_ACK)
         ec=0;
   }

   return(ec);
}

ZMD_EC ZMac_FlushPacket(char *ptr, int8 count, int1 waitForAck)
{
   ZMac_DiscardTX();
   ZMac_Putd(ptr,count);
   return(ZMac_FlushTx(waitForAck));
}

ZMD_EC ZMac_PutPacket(ZMAC_NODE_INFO *remoteNode, char *ptr, int count)
{
   char frameCon;

   if (!ZMac_IsTXReady())
      return(ZMD_EC_TX_BUSY);

   frameCon =  ZMAC_FCON_TYPE_DATA | ZMAC_FCON_SECURITY_NO |
               ZMAC_FCON_FRAME_PENDING_NO | ZMAC_FCON_ACK_REQUESTED_YES |
               ZMAC_FCON_INTRAPAN_NO;

   if (remoteNode->panId.w == ZMac_GetMyPanId())
      frameCon |= ZMAC_FCON_INTRAPAN_YES;

   ZMac_PutHeader(remoteNode, frameCon);

   return(ZMac_FlushPacket(ptr, count, frameCon & ZMAC_FCON_ACK_REQUESTED_YES));
}

int ZMac_GetPacket(ZMAC_NODE_INFO *remoteNode, char *ptr, int max, int16 to)
{
   ZMAC_HEADER header;
   int count=0;
   char c;

   ZMac_DiscardRXAll();
   ZMac_StartListen();

   while(to--)
   {
      if (ZMac_IsRXReady())
      {
         ZMac_GetHeader(&header);
         if (remoteNode)
            memcpy(remoteNode,&header.sourceNode,sizeof(ZMAC_NODE_INFO));
         while(g_ZMacRxFrameRemaining)
         {
            c=ZMac_Getc();
            if (max)
            {
               *ptr++=c;
               max--;
               count++;
            }
         }
         break;
      }
      else
         delay_ms(1);
   }

   ZMac_StopListen();
   ZMac_DiscardRX();

   return(count);
}

//Init stack/functions
void ZMac_Init(void)
{
   zmd_init();

   memset(&g_MyZMACNode,0,sizeof(g_MyZMACNode));

   ZMac_SetMyPanId(0xFFFF);
   ZMac_SetMyShortAddress(0xFFFF);
   ZMac_UseMyLongAddress();

   zmd_storeLQI(FALSE);
}

