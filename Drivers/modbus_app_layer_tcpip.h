//////////////////////////////////////////////////////////////////////////////////////////
////                            modbus_app_layer_tcpip.h                              ////
////                                                                                  ////
////    Application layer of the MODBUS protocol driver for TCP/IP communications.    ////
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

#ifndef MODBUS_APP_LAYER_TCPIP_H
#define MODBUS_APP_LAYER_TCPIP_H

#include <modbus_phy_layer_tcpip.h>

//////////////////////////////////////////////////////////////////////////////////////////
//// Shared API                                                                       ////
////                                                                                  ////
////  modbus_init()                                                                   ////
////    - Initialize modbus TCP/IP communication system.                              ////
////                                                                                  ////
////  modbus_kbhit()                                                                  ////
////    - Used to check if a packet has been received.                                ////
////                                                                                  ////
////  modbus_getd()                                                                   ////
////    - Used to retrieve packet from RX buffer.                                     ////
////                                                                                  ////
////  ModbusTask()                                                                    ////
////    - Maintains modbus TCP/IP connection by calling ModBusTCPTask(), which calls  ////
////      ModbusConnectedTask() to send and recieve modbus messages.  Must be called  ////
////      once during each iteration of your main loop, and it must be called after   ////
////      StackTask().                                                                ////
////                                                                                  ////
//////////////////////////////////////////////////////////////////////////////////////////

/*modbus_init
Input:         none
Output:        void
*/
void modbus_init(void);

/*modbus_kbhit
Input:         none
Output:        int1     TRUE if there is new data in RX buffer to retrieve, FALSE if none
*/
int1 modbus_kbhit(void);

#if (MODBUS_TYPE == MODBUS_TYPE_CLIENT)
/*
modbus_getd
Input:         int8        &unit_identier variable to return Unit Identifier to
               function    &func          variable to return Function Code to
               int8        &length        variable to return number of data bytes to
               int8        *data          pointer to return data to
Output:        exception                  0 if no error, else the exception
*/
exception modbus_getd(unsigned int8 &unit_identifier, function &func, unsigned int8 &length, unsigned int8 *data);

#else
/*
modbus_get
Input:         MBAP_HEADER &MBAPHeader    structure to return MBAP Header to
               function    &func          variable to return Function Code to
               int8        &length        variable to return number of data bytes to
               int8        *data          pointer to return data to
Output         int1                       TRUE if new data was retrieved from RX buffer, FALSE if no new data was in RX buffer
*/
int1 modbus_getd(MBAP_HEADER &MBAPHeader, function &func, unsigned int8 &length, unsigned int8 *data);

#endif

/*ModbusTask
Input:         none
Output:        void
*/
void ModbusTask(void);

#if (MODBUS_TYPE==MODBUS_TYPE_CLIENT)

//////////////////////////////////////////////////////////////////////////////////////////
//// Client Api                                                                       ////
////                                                                                  ////
////  All client API functions return 0 on success.                                   ////
////                                                                                  ////
////  int1 modbus_read_coils(address,start_address,quantity)                          ////
////    - Wrapper for function 0x01(read coils) in the MODBUS specification.          ////
////                                                                                  ////
////  int1 modbus_read_discrete_input(address,start_address,quantity)                 ////
////    - Wrapper for function 0x02(read discret input) in the MODBUS specification.  ////
////                                                                                  ////
////  int1 modbus_read_holding_registers(address,start_address,quantity)              ////
////    - Wrapper for function 0x03(read holding regs) in the MODBUS specification.   ////
////                                                                                  ////
////  int1 modbus_read_input_registers(address,start_address,quantity)                ////
////    - Wrapper for function 0x04(read input regs) in the MODBUS specification.     ////
////                                                                                  ////
////  int1 modbus_write_single_coil(address,output_address,on)                        ////
////    - Wrapper for function 0x05(write single coil) in the MODBUS specification.   ////
////                                                                                  ////
////  int1 modbus_write_single_register(address,reg_address,reg_value)                ////
////    - Wrapper for function 0x06(write single reg) in the MODBUS specification.    ////
////                                                                                  ////
////  int1 modbus_write_multiple_coils(address,start_address,quantity,*values)        ////
////    - Wrapper for function 0x0F(write multiple coils) in the MODBUS specification.////
////    - Special Note: values is a pointer to an int8 array, each byte represents 8  ////
////                    coils.                                                        ////
////                                                                                  ////
////  int1 modbus_write_multiple_registers(address,start_address,quantity,*values)    ////
////    - Wrapper for function 0x10(write multiple regs) in the MODBUS specification. ////
////    - Special Note: values is a pointer to an int16 array                         ////
////                                                                                  ////
////  int1 modbus_read_file_record(address,byte_count,*request)                       ////
////    - Wrapper for function 0x14(read file record) in the MODBUS specification.    ////
////                                                                                  ////
////  int1 modbus_write_file_record(address,byte_count,*request)                      ////
////    - Wrapper for function 0x15(write file record) in the MODBUS specification.   ////
////                                                                                  ////
////  int1 modbus_mask_write_register(address,reference_address,AND_mask,OR_mask)     ////
////    - Wrapper for function 0x16(read coils) in the MODBUS specification.          ////
////                                                                                  ////
////  int1 modbus_read_write_multiple_registers(address,read_start,read_quantity,     ////
////                            write_start,write_quantity, *write_registers_value)   ////
////    - Wrapper for function 0x17(read write mult regs) in the MODBUS specification.////
////                                                                                  ////
////  int1 modbus_read_FIFO_queue(address,FIFO_address)                               ////
////    - Wrapper for function 0x18(read FIFO queue) in the MODBUS specification.     ////
////                                                                                  ////
//////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************
The following structs are used for read/write_sub_request.  These
functions take in one of these structs.
Please refer to the MODBUS protocol specification if you do not
understand the members of the structure.
********************************************************************/
typedef struct _modbus_read_sub_request
{
   unsigned int8 reference_type;
   unsigned int16 file_number;
   unsigned int16 record_number;
   unsigned int16 record_length;
} modbus_read_sub_request;

typedef struct _modbus_write_sub_request
{
   unsigned int8 reference_type;
   unsigned int16 file_number;
   unsigned int16 record_number;
   unsigned int16 record_length;
   unsigned int16 data[MODBUS_BUFFER_SIZE-8];
} modbus_write_sub_request;

/********************************************************************
The following functions are defined in the MODBUS protocol.  Please
refer to http://www.modbus.org for the purpose of each of these.
All functions take the Unit Identifier as their first parameter.
The function will return 1 if there was an error when attempting to 
load the TX buffer, otherwilse it will return 0 if successful.
********************************************************************/

/*
read_coils
Input:     int8       address            Unit Identifier
           int16      start_address      Address to start reading from
           int16      quantity           Amount of addresses to read
Output:    int1                          0 if successful placed in TCP TX buffer, 1 if unsuccessful
*/
int1 modbus_read_coils(unsigned int8 address, unsigned int16 start_address, unsigned int16 quantity);

/*
read_discrete_input
Input:     int8       address            Unit Identifier
           int16      start_address      Address to start reading from
           int16      quantity           Amount of addresses to read
Output:    int1                          0 if successful placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_read_discrete_input(unsigned int8 address, unsigned int16 start_address, unsigned int16 quantity);

/*
read_holding_registers
Input:     int8       address            Unit Identifier
           int16      start_address      Address to start reading from
           int16      quantity           Amount of addresses to read
Output:    int1                          0 if successful placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_read_holding_registers(unsigned int8 address, unsigned int16 start_address, unsigned int16 quantity);

/*
read_input_registers
Input:     int8       address            Unit Identifier
           int16      start_address      Address to start reading from
           int16      quantity           Amount of addresses to read
Output:    int1                          0 if successful placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_read_input_registers(unsigned int8 address, unsigned int16 start_address, unsigned int16 quantity);

/*
write_single_coil
Input:     int8       address            Unit Identifier
           int16      output_address     Address to write into
           int1       on                 true for on, false for off
Output:    int1                          0 if successful placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_write_single_coil(unsigned int8 address, unsigned int16 output_address, int1 on);

/*
write_single_register
Input:     int8       address            Unit Identifier
           int16      reg_address        Address to write into
           int16      reg_value          Value to write
Output:    int1                          0 if successful placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_write_single_register(unsigned int8 address, unsigned int16 reg_address, unsigned int16 reg_value);

/*
write_multiple_coils
Input:     int8       address            Unit Identifier
           int16      start_address      Address to start at
           int16      quantity           Amount of coils to write to
           int1*      values             A pointer to an array holding the values to write
Output:    int1                          0 if successful placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_write_multiple_coils(unsigned int8 address, unsigned int16 start_address, unsigned int16 quantity, unsigned int8 *values);

/*
write_multiple_registers
Input:     int8       address            Unit Identifier
           int16      start_address      Address to start at
           int16      quantity           Amount of registers to write to
           int16*     values             A pointer to an array holding the data to write
Output:    int1                          0 if successful placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_write_multiple_registers(unsigned int8 address, unsigned int16 start_address, unsigned int16 quantity, unsigned int16 *values);

/*
read_file_record
Input:     int8                address            Unit Identifier
           int8                byte_count         Number of bytes to read
           read_sub_request*   request            Structure holding record information
Output:    int1                                   0 if successful placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_read_file_record(unsigned int8 address, unsigned int8 byte_count, modbus_read_sub_request *request);

/*
write_file_record
Input:     int8                address            Unit Identifier
           int8                byte_count         Number of bytes to read
           read_sub_request*   request            Structure holding record/data information
Output:    int1                                   0 if successful placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_write_file_record(unsigned int8 address, unsigned int8 byte_count, modbus_write_sub_request *request);

/*
mask_write_register
Input:     int8       address            Unit Identifier
           int16      reference_address  Address to mask
           int16      AND_mask           A mask to AND with the data at reference_address
           int16      OR_mask            A mask to OR with the data at reference_address
Output:    int1                          0 if successful placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_mask_write_register(unsigned int8 address, unsigned int16 reference_address, unsigned int16 AND_mask, unsigned int16 OR_mask);

/*
read_write_multiple_registers
Input:     int8       address                Unit Identifier
           int16      read_start             Address to start reading
           int16      read_quantity          Amount of registers to read
           int16      write_start            Address to start writing
           int16      write_quantity         Amount of registers to write
           int16*     write_registers_value  Pointer to an aray us to write
Output:    int1                              0 if successful placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_read_write_multiple_registers(unsigned int8 address, unsigned int16 read_start,
                                    unsigned int16 read_quantity, unsigned int16 write_start,
                                    unsigned int16 write_quantity,
                                    unsigned int16 *write_registers_value);
                                    
/*
read_FIFO_queue
Input:     int8       address           Unit Identifier
           int16      FIFO_address      FIFO address
Output:    int1                         0 if successful placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_read_FIFO_queue(unsigned int8 address, unsigned int16 FIFO_address);

#else

//////////////////////////////////////////////////////////////////////////////////////////
//// Server API                                                                       ////
////                                                                                  ////
////  int1 modbus_read_coils_rsp(MBAPHeader,byte_count,*coil_data)                    ////
////    - Wrapper to respond to 0x01(read coils) in the MODBUS specification.         ////
////                                                                                  ////
////  int1 modbus_read_discrete_input_rsp(MBAPHeader,byte_count,*input_data)          ////
////    - Wrapper to respond to 0x02(read discret input) in the MODBUS specification. ////
////                                                                                  ////
////  int1 modbus_read_holding_registers_rsp(MBAPHeader,byte_count,*reg_data)         ////
////    - Wrapper to respond to 0x03(read holding regs) in the MODBUS specification.  ////
////                                                                                  ////
////  int1 modbus_read_input_registers_rsp(MBAPHeader,byte_count,*input_data)         ////
////    - Wrapper to respond to 0x04(read input regs) in the MODBUS specification.    ////
////                                                                                  ////
////  int1 modbus_write_single_coil_rsp(MBAPHeader,output_address,output_value)       ////
////    - Wrapper to respond to 0x05(write single coil) in the MODBUS specification.  ////
////                                                                                  ////
////  int1 modbus_write_single_register_rsp(MBAPHeader,reg_address,reg_value)         ////
////    - Wrapper to respond to 0x06(write single reg) in the MODBUS specification.   ////
////                                                                                  ////
////  int1 modbus_write_multiple_coils_rsp(MBAPHeader,start_address,quantity)         ////
////    - Wrapper to respond to 0x0F(write multiple coils) in the MODBUS specification////
////                                                                                  ////
////  int1 modbus_write_multiple_registers_rsp(MBAPHeader,start_address,quantity)     ////
////    - Wrapper to respond to 0x10(write multiple regs) in the MODBUS specification.////
////                                                                                  ////
////  int1 modbus_read_file_record_rsp(MBAPHeader,byte_count,*request)                ////
////    - Wrapper to respond to 0x14(read file record) in the MODBUS specification.   ////
////                                                                                  ////
////  int1 modbus_write_file_record_rsp(MBAPHeader,byte_count,*request)               ////
////    - Wrapper to respond to 0x15(write file record) in the MODBUS specification.  ////
////                                                                                  ////
////  int1 modbus_mask_write_register_rsp(MBAPHeader,reference_address,AND_mask,      ////
////                                                                   OR_mask)       ////
////    - Wrapper to respond to 0x16(read coils) in the MODBUS specification.         ////
////                                                                                  ////
////  int1 modbus_read_write_multiple_registers_rsp(MBAPHeader,*data,data_len)        ////
////    - Wrapper to respond to 0x17(read write mult regs) in the MODBUS specification////
////                                                                                  ////
////  int1 modbus_read_FIFO_queue_rsp(MBAPHeader,FIFO_len,*data)                      ////
////    - Wrapper to respond to 0x18(read FIFO queue) in the MODBUS specification.    ////
////                                                                                  ////
////  int1 modbus_exception_rsp(MBAPHeader,func,error)                                ////
////    - Wrapper to send an exception response.  See exception list below.           ////
////                                                                                  ////
//// Exception List:                                                                  ////
////  ILLEGAL_FUNCTION, ILLEGAL_DATA_ADDRESS, ILLEGAL_DATA_VALUE,                     ////
////  SLAVE_DEVICE_FAILURE, ACKNOWLEDGE, SLAVE_DEVICE_BUSY, MEMORY_PARITY_ERROR,      ////
////  GATEWAY_PATH_UNAVAILABLE, GATEWAY_TARGET_NO_RESPONSE                            ////
////                                                                                  ////
//////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************
The following structs are used for read/write_sub_request_rsp.  These
functions take in one of these structs.  Please refer to the MODBUS
protocol specification if you do not understand the members of the
structure.
********************************************************************/
typedef struct _modbus_read_sub_request_rsp
{
   unsigned int8 record_length;
   unsigned int8 reference_type;
   unsigned int16 data[((MODBUS_BUFFER_SIZE)/2)-3];
} modbus_read_sub_request_rsp;

typedef struct _modbus_write_sub_request_rsp
{
   unsigned int8 reference_type;
   unsigned int16 file_number;
   unsigned int16 record_number;
   unsigned int16 record_length;
   unsigned int16 data[((MODBUS_BUFFER_SIZE)/2)-8];
} modbus_write_sub_request_rsp;

/********************************************************************
The following server functions are defined in the MODBUS protocol.
Please refer to http://www.modbus.org for the purpose of each of
these.
********************************************************************/

/*
read_coils_rsp
Input:     MPAB_HEADER  MBAPHeader       Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int8         byte_count       Number of bytes being sent
           int8*        coil_data        Pointer to an array of data to send
Output:    int1                          0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_read_coils_rsp(MBAP_HEADER MBAPHeader, unsigned int8 byte_count, unsigned int8* coil_data);

/*
read_discrete_input_rsp
Input:     MPAB_HEADER  MBAPHeader       Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int8         byte_count       Number of bytes being sent
           int8*        input_data       Pointer to an array of data to send
Output:    int1                          0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_read_discrete_input_rsp(MBAP_HEADER MBAPHeader, unsigned int8 byte_count, unsigned int8 *input_data);

/*
read_holding_registers_rsp
Input:     MPAB_HEADER  MBAPHeader       Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int8         byte_count       Number of bytes being sent
           int16*       reg_data         Pointer to an array of data to send
Output:    int1                          0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_read_holding_registers_rsp(MBAP_HEADER MBAPHeader, unsigned int8 byte_count, unsigned int16 *reg_data);

/*
read_input_registers_rsp
Input:     MPAB_HEADER  MBAPHeader       Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int8         byte_count       Number of bytes being sent
           int16*       input_data       Pointer to an array of data to send
Output:    int1                          0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_read_input_registers_rsp(MBAP_HEADER MBAPHeader, unsigned int8 byte_count, unsigned int16 *input_data);


/*
write_single_coil_rsp
Input:     MPAB_HEADER  MBAPHeader       Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int16        output_address   Echo of output address received
           int16        output_value     Echo of output value received
Output:    int1                          0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_write_single_coil_rsp(MBAP_HEADER MBAPHeader, unsigned int16 output_address, unsigned int16 output_value);

/*
write_single_register_rsp
Input:     MPAB_HEADER  MBAPHeader       Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int16        reg_address      Echo of register address received
           int16        reg_value        Echo of register value received
Output:    int1                          0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_write_single_register_rsp(MBAP_HEADER MBAPHeader, unsigned int16 reg_address, unsigned int16 reg_value);

/*
write_multiple_coils_rsp
Input:     MPAB_HEADER  MBAPHeader       Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int16        start_address    Echo of address to start at
           int16        quantity         Echo of amount of coils written to
Output:    int1                          0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_write_multiple_coils_rsp(MBAP_HEADER MBAPHeader, unsigned int16 start_address, unsigned int16 quantity);

/*
write_multiple_registers_rsp
Input:     MPAB_HEADER  MBAPHeader       Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int16        start_address    Echo of address to start at
           int16        quantity         Echo of amount of registers written to
Output:    int1                          0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_write_multiple_registers_rsp(MBAP_HEADER MBAPHeader, unsigned int16 start_address, unsigned int16 quantity);

/*
read_file_record_rsp
Input:     MPAB_HEADER              MBAPHeader        Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int8                     byte_count        Number of bytes to send
           read_sub_request_rsp*    request           Structure holding record/data information
Output:    int1                                       0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_read_file_record_rsp(MBAP_HEADER MBAPHeader, unsigned int8 byte_count, modbus_read_sub_request_rsp *request);

/*
write_file_record_rsp
Input:     MPAB_HEADER              MBAPHeader        Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int8                     byte_count        Echo of number of bytes sent
           write_sub_request_rsp*   request           Echo of Structure holding record information
Output:    int1                                       0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_write_file_record_rsp(MBAP_HEADER MBAPHeader, unsigned int8 byte_count, modbus_write_sub_request_rsp *request);

/*
mask_write_register_rsp
Input:     MPAB_HEADER  MBAPHeader        Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int16        reference_address Echo of reference address
           int16        AND_mask          Echo of AND mask
           int16        OR_mask           Echo or OR mask
Output:    int1                           0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_mask_write_register_rsp(MBAP_HEADER MBAPHeader, unsigned int16 reference_address, unsigned int16 AND_mask, unsigned int16 OR_mask);

/*
read_write_multiple_registers_rsp
Input:     MPAB_HEADER  MBAPHeader        Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int8         data_len          Length of data in bytes
           int16*       data              Pointer to an array of data
Output:    int1                           0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_read_write_multiple_registers_rsp(MBAP_HEADER MBAPHeader, unsigned int8 data_len, unsigned int16 *data);

/*
read_FIFO_queue_rsp
Input:     MPAB_HEADER  MBAPHeader        Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int16        FIFO_len          Length of FIFO in bytes
           int16*       data              Pointer to an array of data
Output:    int1                           0 if successfully placed in TX buffer, 1 if unsuccessfulvoid
*/
int1 modbus_read_FIFO_queue_rsp(MBAP_HEADER MBAPHeader, unsigned int16 FIFO_len, unsigned int16 *data);

/*
modbus_exception_rsp
Input:     MPAB_HEADER  MBAPHeader        Received MPAB Header used to create MPAB Header of response and which socket to respond to
           function     func              function to respond to
           exception    error             exception response to send
Output:    int1                           0 if successfully placed in TX buffer, 1 if unsuccessfulvoid
*/
int1 modbus_exception_rsp(MBAP_HEADER MBAPHeader, function func, exception error);

#endif

#if (MODBUS_TYPE == MODBUS_TYPE_CLIENT)
/*
ModbusServerAddrInit
Input:               none
Output:              void
*/
void ModbusServerAddrInit(void);

#endif

/*
ModbusTCPTask
Input:         none
Output:        void
*/
void ModbusTCPTask(void);

#if (MODBUS_TYPE == MODBUS_TYPE_CLIENT)
/*
ModbusConnectedTask
Input:               TCP_SOCKET  socket   Socket to receive or transmit message on
Output:              void
*/
void ModbusConnectedTask(TCP_SOCKET socket);

#else
/*
ModbusConnectedTask
Input:               TCP_SOCKET  socket   Socket to receive or transmit message on
                     int8        which    Which TX/RX buffer to transmit/receive message from/on
Output:              void
*/
void ModbusConnectedTask(TCP_SOCKET socket,unsigned int8 which);

#endif

#if (MODBUS_TYPE == MODBUS_TYPE_CLIENT)
IP_ADDR server;
#endif

#if (MODBUS_TYPE == MODBUS_TYPE_CLIENT)
typedef enum _MODBUS_TCP_STATE {
   MODBUS_TCP_STATE_CONNECT=0, MODBUS_TCP_STATE_CONNECT_WAIT=1,
   MODBUS_TCP_STATE_CONNECTED=2, MODBUS_TCP_STATE_DISCONNECT=3,
   MODBUS_TCP_STATE_FORCE_DISCONNECT=4
} MODBUS_TCP_STATE;

typedef enum _MODBUS_STATE {
   MODBUS_IDLE = 0, MODBUS_SEND_MESSAGE, MODBUS_WAIT_FOR_RESPONSE
} MODBUS_STATE;

#else
typedef enum _MODBUS_TCP_STATE {
   MODBUS_TCP_STATE_NEW=0, MODBUS_TCP_STATE_LISTENING=1,
   MODBUS_TCP_STATE_CONNECTED=2, MODBUS_TCP_STATE_DISCONNECT=3,
   MODBUS_TCP_STATE_FORCE_DISCONNECT=4
} MODBUS_TCP_STATE;

typedef enum _MODBUS_STATE {
   MODBUS_IDLE = 0, MODBUS_GET_MESSAGE, MODBUS_SEND_RESPONSE
} MODBUS_STATE;

#endif

typedef enum _MODBUS_RX_STATE {
   MODBUS_RX_TRANSACTION_IDENTIFIER = 0, MODBUS_RX_PROTOCOL_IDENTIFER, MODBUS_RX_LENGTH,
   MODBUS_RX_UNIT_IDENTIFIER, MODBUS_RX_FUNCTION_CODE, MODBUS_RX_DATA
} MODBUS_RX_STATE;

#endif
