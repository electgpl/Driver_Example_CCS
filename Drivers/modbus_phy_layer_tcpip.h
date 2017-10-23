//////////////////////////////////////////////////////////////////////////////////////////
////                            modbus_phy_layer_tcpip.h                              ////
////                                                                                  ////
////      Physical layer of the MODBUS protocol driver for TCP/IP communications.     ////
////                                                                                  ////
////  Refer to documentation at http://www.modbus.org for more information on MODBUS. ////
////                                                                                  ////
//////////////////////////////////////////////////////////////////////////////////////////
////                                                                                  ////
//// Revision history:                                                                ////
////  November 1, 2011    Added TCP/IP protocol to MODBUS driver.                     ////
////                                                                                  ////
//////////////////////////////////////////////////////////////////////////////////////////
////                (C) Copyright 1996, 2013 Custom Computer Services                 ////
////        This source code may only be used by licensed users of the CCS            ////
////        C compiler.  This source code may only be distributed to other            ////
////        licensed users of the CCS C compiler.  No other use,                      ////
////        reproduction or distribution is permitted without written                 ////
////        permission.  Derivative programs created using this software              ////
////        in object code form are not restricted in any way.                        ////
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef MODBUS_PHY_LAYER_TCPIP_H
#define MODBUS_PHY_LAYER_TCPIP_H

#if (MODBUS_TYPE == MODBUS_TYPE_CLIENT)
int1 modbus_rx_new = FALSE;   //Flag to indicating if new message is waiting in RX buffer to receive
int1 modbus_tx_new = FALSE;   //Flag to indicate if new message is waiting in TX buffer to send
#else
static int1 modbus_rx_new[MODBUS_LISTEN_SOCKETS];  //Flag to indicating if new message is waiting in RX buffer to receive
static int1 modbus_tx_new[MODBUS_LISTEN_SOCKETS];  //Flag to indicate if new message is waiting in TX buffer to send
#endif

/********************************************************************
These exceptions are defined in the MODBUS protocol.  These can be
used by the server to communicate problems with the transmission back
to the client who can also use these to easily check the exceptions.  
The first exception is the only one that is not part of the protocol 
specification.  The TIMEOUT exception is returned when no server 
responds to the client's request within the timeout period.
********************************************************************/
typedef enum _exception{ILLEGAL_FUNCTION=1,ILLEGAL_DATA_ADDRESS=2, 
ILLEGAL_DATA_VALUE=3,SLAVE_DEVICE_FAILURE=4,ACKNOWLEDGE=5,SLAVE_DEVICE_BUSY=6, 
MEMORY_PARITY_ERROR=8,GATEWAY_PATH_UNAVAILABLE=10,GATEWAY_TARGET_NO_RESPONSE=11,
TIMEOUT=12} exception;

/********************************************************************
These functions are defined in the MODBUS protocol.  These can be
used by the server to check the incomming function.
********************************************************************/
typedef enum _function{FUNC_READ_COILS=0x01,FUNC_READ_DISCRETE_INPUT=0x02,
FUNC_READ_HOLDING_REGISTERS=0x03,FUNC_READ_INPUT_REGISTERS=0x04,
FUNC_WRITE_SINGLE_COIL=0x05,FUNC_WRITE_SINGLE_REGISTER=0x06,
FUNC_WRITE_MULTIPLE_COILS=0x0F,FUNC_WRITE_MULTIPLE_REGISTERS=0x10,
FUNC_READ_FILE_RECORD=0x14,FUNC_WRITE_FILE_RECORD=0x15,
FUNC_MASK_WRITE_REGISTER=0x16,FUNC_READ_WRITE_MULTIPLE_REGISTERS=0x17,
FUNC_READ_FIFO_QUEUE=0x18} function;

/********************************************************************
Our RX and TX buffers.  These are used when receiving and transmitting 
data as a client or server.
********************************************************************/

#if (MODBUS_TYPE == MODBUS_TYPE_CLIENT)
typedef struct _MBAP_HEADER {
   unsigned int16 TransactionIdentifier;     //Transaction Identifier of message
   unsigned int16 ProtocolIdentifier;        //Protocol Identifer of message Modbus Protocol = 0
   unsigned int16 Length;                    //Number of bytes in message including Unit Identifier
   unsigned int8  UnitIdentifier;            //Unit Identifier of message
} MBAP_HEADER;

struct {
   unsigned int8  address;                   //Unit Identifier of received message
   unsigned int16 len;                       //Number of data bytes in the received message
   function func;                            //The function of the received message
   exception error;                          //Error recieved, if any
   unsigned int8 data[MODBUS_BUFFER_SIZE];   //Data of the received message
} modbus_rx;

struct {
   MBAP_HEADER MBAPHeader;                   //MBAP Header of transmit message
   function func;                            //The function of the transmit message
   unsigned int8 data[MODBUS_BUFFER_SIZE];   //Data of the transmit mesage
} modbus_tx;

#else
typedef struct _MBAP_HEADER {
   unsigned int16 TransactionIdentifier;     //Transaction Identifier of message
   unsigned int16 ProtocolIdentifier;        //Protocol Identifer of message Modbus Protocol = 0
   unsigned int16 Length;                    //Number of bytes in message including Unit Identifier
   unsigned int8  UnitIdentifier;            //Unit Identifier of message
   unsigned int8  WhichSocket;               //Not part of MBAP Header, added to determine which socket the server is to send response to
} MBAP_HEADER;

struct {
   MBAP_HEADER MBAPHeader;                   //MBAP Header of received Message
   unsigned int16 len;                       //Number of data bytes in the received message
   function func;                            //The function of the received message
   unsigned int8 data[MODBUS_BUFFER_SIZE];   //Data of the received message
} modbus_rx[MODBUS_LISTEN_SOCKETS];

struct {
   MBAP_HEADER MBAPHeader;                   //MBAP Header of transmit message
   function func;                            //The function of the transmit message
   unsigned int8 data[MODBUS_BUFFER_SIZE];   //Data of the transmit mesage
} modbus_tx[MODBUS_LISTEN_SOCKETS];

#endif

#if (MODBUS_TYPE == MODBUS_TYPE_CLIENT)
/*
ModbusLoadTxBufferStart
Purpose: To load the MPAB Header and function code into the TX Buffer
Input:   function    func     The function code to send with message
         int8        address  The Unit Identifier to send with message
         int16       length   The number of bytes of the message
Output:  void
*/
void ModbusLoadTxBufferStart(function func, unsigned int8 address,unsigned int16 length);

/*
modbus_putc
Purpose: To load a byte of data into the TX Buffer
Input:   int8        data     The byte of data to load into the buffer
         int1        new      If TRUE it resets the index and loads the data into index 0, If FALSE loads data into next index location
Output:  void
*/
void modbus_putc(unsigned int8 data,int1 new = FALSE);

#else
/*
ModbusLoadTxBufferStart
Purpose: To load the MPAB Header and function code into the TX Buffer
Input:   function    func        The function code to send with message
         MBAP_HEADER MBAPHeader  Contains the Transaction Identifier and Unit Identifier of MBAP Header to send with message
         int16       length      The number of bytes of the message
Output:  void
*/
void MobusLoadTxBufferStart(function func, MBAP_HEADER MBAPHeader, unsigned int16 length);

/*
modbus_putc
Purpose: To load a byte of data into the TX Buffer
Input:   int8        which    Selects which TX Buffer to load data into
         int8        data     The byte of data to load into the buffer
         int1        new      If TRUE it resets the index and loads the data into index 0, If FALSE loads data into next index location
Output:  void
*/
void modbus_putc(unsigned int8 which, unsigned int8 data, int1 new = FALSE);

#endif

#endif
