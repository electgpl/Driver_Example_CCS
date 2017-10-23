//////////////////////////////////////////////////////////////////////////////////////////
////                            modbus_app_layer_tcpip.c                              ////
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

#ifndef MODBUS_APP_LAYER_TCPIP_C
#define MODBUS_APP_LAYER_TCPIP_C

#include <modbus_app_layer_tcpip.h>
#include "tcpip\TCPIP Stack\tcp.h"
#include "tcpip\TCPIP Stack\tick.h"

#if (MODBUS_TYPE == MODBUS_TYPE_SERVER)
TCP_SOCKET socket[MODBUS_LISTEN_SOCKETS];
#endif

//////////////////////////////////////////////////////////////////////////////////////////
//// Shared API                                                                       ////
//////////////////////////////////////////////////////////////////////////////////////////

/*modbus_init
Input:         none
Output:        void
*/
void modbus_init(void)
{
  #if (MODBUS_TYPE == MODBUS_TYPE_CLIENT)
   ModbusServerAddrInit();
   modbus_tx.MBAPHeader.TransactionIdentifier = 0;
  #else
   unsigned int8 i;
   
   for(i=0;i<MODBUS_LISTEN_SOCKETS;i++)
      socket[i] = INVALID_SOCKET;
  #endif
}

#if (MODBUS_TYPE == MODBUS_TYPE_CLIENT)
/*modbus_kbhit
Input:         none
Output:        int1     TRUE if there is new data in RX buffer to retrieve, FALSE if none
*/
int1 modbus_kbhit(void)
{
   return(modbus_rx_new);
}

/*
modbus_getd
Input:         int8        &unit_identier variable to return Unit Identifier to
               function    &func          variable to return Function Code to
               int8        &length        variable to return number of data bytes to
               int8        *data          pointer to return data to
Output:        exception                  0 if no error, else the exception
*/
exception modbus_getd(unsigned int8 &unit_identifier, function &func, unsigned int8 &length, unsigned int8 *data)
{
   unsigned int8 i;
   
   if(modbus_rx.error)
   {
      if(modbus_rx.error != TIMEOUT)
      {
         unit_identifier = modbus_rx.address;
         func = modbus_rx.func;
      }
   }
   else
   {
      unit_identifier = modbus_rx.address;
      func = modbus_rx.func;
      length = modbus_rx.len;
      
      for(i=0;i<length;i++)
         *data++ = modbus_rx.data[i];
   }
   
   modbus_rx_new = FALSE;
   return(modbus_rx.error);
}

#else
/*modbus_kbhit
Input:         none
Output:        int1     TRUE if there is new data in RX buffer to retrieve, FALSE if none
*/
int1 modbus_kbhit(void)
{
   unsigned int8 i;
   
   for(i=0;i<MODBUS_LISTEN_SOCKETS;i++)
   {
      if(modbus_rx_new[i] == TRUE)
         return(TRUE);
   }
   
   return(FALSE);
}

/*
modbus_get
Input:         MBAP_HEADER &MBAPHeader    structure to return MBAP Header to
               function    &func          variable to return Function Code to
               int8        &length        variable to return number of data bytes to
               int8        *data          pointer to return data to
Output         int1                       TRUE if new data was retrieved from RX buffer, FALSE if no new data was in RX buffer
*/
int1 modbus_getd(MBAP_HEADER &MBAPHeader, function &func, unsigned int8 &length, unsigned int8 *data)
{
   unsigned int8 which,i;
   
   for(which=0;which<MODBUS_LISTEN_SOCKETS;which++)
   {
      if(modbus_rx_new[which] == TRUE)
         break;
   }
   
   if(which >= MODBUS_LISTEN_SOCKETS)
      return(FALSE);
      
   MBAPHeader.TransactionIdentifier = modbus_rx[which].MBAPHeader.TransactionIdentifier;
   MBAPHeader.ProtocolIdentifier = modbus_rx[which].MBAPHeader.ProtocolIdentifier;
   MBAPHeader.Length = modbus_rx[which].MBAPHeader.Length;
   MBAPHeader.UnitIdentifier = modbus_rx[which].MBAPHeader.UnitIdentifier;
   MBAPHeader.WhichSocket = which;
   
   func = modbus_rx[which].func;
   length = modbus_rx[which].len;
   
   for(i=0;i<length;i++)
      *data++ = modbus_rx[which].data[i];
      
   modbus_rx_new[which] = FALSE;
   return(TRUE);      
}

#endif

/*ModbusTask
Input:         none
Output:        void
*/
void ModbusTask(void)
{
   ModbusTCPTask();
}

#if (MODBUS_TYPE==MODBUS_TYPE_CLIENT)
//////////////////////////////////////////////////////////////////////////////////////////
//// CLIENT API                                                                       ////
//////////////////////////////////////////////////////////////////////////////////////////

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
Output:    int1                          0 if successfully placed in TCP TX buffer, 1 if unsuccessful
*/
int1 modbus_read_coils(unsigned int8 address, unsigned int16 start_address, unsigned int16 quantity)
{
   if(!modbus_tx_new)   //check to see if there is data still in tx buffer to send
   {
      if(quantity > 2000)
         return(TRUE);
   
      ModbusLoadTxBufferStart(FUNC_READ_COILS, address, 6);
      modbus_putc(make8(start_address,1),TRUE);
      modbus_putc(make8(start_address,0));
      modbus_putc(make8(quantity,1));
      modbus_putc(make8(quantity,0));
      
      modbus_tx_new = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
read_discrete_input
Input:     int8       address            Unit Identifier
           int16      start_address      Address to start reading from
           int16      quantity           Amount of addresses to read
Output:    int1                          0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_read_discrete_input(unsigned int8 address, unsigned int16 start_address, unsigned int16 quantity)
{
   if(!modbus_tx_new)   //check to see if there is data still in tx buffer to send
   {
      if(quantity > 2000)
         return(TRUE);
   
      ModbusLoadTxBufferStart(FUNC_READ_DISCRETE_INPUT, address, 6);
      modbus_putc(make8(start_address,1),TRUE);
      modbus_putc(make8(start_address,0));
      modbus_putc(make8(quantity,1));
      modbus_putc(make8(quantity,0));
      
      modbus_tx_new = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
read_holding_registers
Input:     int8       address            Unit Identifier
           int16      start_address      Address to start reading from
           int16      quantity           Amount of addresses to read
Output:    int1                          0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_read_holding_registers(unsigned int8 address, unsigned int16 start_address, unsigned int16 quantity)
{
   if(!modbus_tx_new)   //check to see if there is data still in tx buffer to send
   {
      if(quantity > 125)
         return(TRUE);
   
      ModbusLoadTxBufferStart(FUNC_READ_HOLDING_REGISTERS, address, 6);
      modbus_putc(make8(start_address,1),TRUE);
      modbus_putc(make8(start_address,0));
      modbus_putc(make8(quantity,1));
      modbus_putc(make8(quantity,0));
      
      modbus_tx_new = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
read_input_registers
Input:     int8       address            Unit Identifier
           int16      start_address      Address to start reading from
           int16      quantity           Amount of addresses to read
Output:    int1                          0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_read_input_registers(unsigned int8 address, unsigned int16 start_address, unsigned int16 quantity)
{
   if(!modbus_tx_new)   //check to see if there is data still in tx buffer to send
   {
      if(quantity > 125)
         return(TRUE);
   
      ModbusLoadTxBufferStart(FUNC_READ_INPUT_REGISTERS, address, 6);
      modbus_putc(make8(start_address,1),TRUE);
      modbus_putc(make8(start_address,0));
      modbus_putc(make8(quantity,1));
      modbus_putc(make8(quantity,0));
      
      modbus_tx_new = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
write_single_coil
Input:     int8       address            Unit Identifier
           int16      output_address     Address to write into
           int1       on                 true for on, false for off
Output:    int1                          0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_write_single_coil(unsigned int8 address, unsigned int16 output_address, int1 on)
{
   if(!modbus_tx_new)   //check to see if there is data still in tx buffer to send
   {
      ModbusLoadTxBufferStart(FUNC_WRITE_SINGLE_COIL, address, 6);
      modbus_putc(make8(output_address,1),TRUE);
      modbus_putc(make8(output_address,0));
      modbus_putc(0xFF);
      modbus_putc(0x00);
         
      modbus_tx_new = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
write_single_register
Input:     int8       address            Unit Identifier
           int16      reg_address        Address to write into
           int16      reg_value          Value to write
Output:    int1                          0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_write_single_register(unsigned int8 address, unsigned int16 reg_address, unsigned int16 reg_value)
{
   if(!modbus_tx_new)   //check to see if there is data still in tx buffer to send
   {
      ModbusLoadTxBufferStart(FUNC_WRITE_SINGLE_REGISTER, address, 6);
      modbus_putc(make8(reg_address,1),TRUE);
      modbus_putc(make8(reg_address,0));
      modbus_putc(make8(reg_value,1));
      modbus_putc(make8(reg_value,0));
      
      modbus_tx_new = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
write_multiple_coils
Input:     int8       address            Unit Identifier
           int16      start_address      Address to start at
           int16      quantity           Amount of coils to write to
           int1*      values             A pointer to an array holding the values to write
Output:    int1                          0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_write_multiple_coils(unsigned int8 address, unsigned int16 start_address, unsigned int16 quantity, unsigned int8 *values)
{
   unsigned int16 i,count;
   
   if(!modbus_tx_new)   //check to see if there is data still in tx buffer to send
   {
      if(quantity > 1968)
         return(TRUE);
   
      count = quantity/8;
   
      if(quantity%8)
         count++; 
   
      ModbusLoadTxBufferStart(FUNC_WRITE_MULTIPLE_COILS, address, 7 + count);
      modbus_putc(make8(start_address,1),TRUE);
      modbus_putc(make8(start_address,0));
      modbus_putc(make8(quantity,1));
      modbus_putc(make8(quantity,0));
      modbus_putc(count);
      for(i=5;i<(count+5);i++)
      {
         if(i >= MODBUS_BUFFER_SIZE)   //check to make sure the TX buffer wasn't overran
            return(TRUE);
            
         modbus_putc(*values++);
      }
      
      modbus_tx_new = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
write_multiple_registers
Input:     int8       address            Unit Identifier
           int16      start_address      Address to start at
           int16      quantity           Amount of registers to write to
           int16*     values             A pointer to an array holding the data to write
Output:    int1                          0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_write_multiple_registers(unsigned int8 address, unsigned int16 start_address, unsigned int16 quantity, unsigned int16 *values)
{
   unsigned int16 i,count;
   
   if(!modbus_tx_new)   //check to see if there is data still in tx buffer to send
   {
      if(quantity > 123)
         return(TRUE);
   
      count = quantity*2;
      
      ModbusLoadTxBufferStart(FUNC_WRITE_MULTIPLE_REGISTERS, address, 7 + count);
      modbus_putc(make8(start_address,1),TRUE);
      modbus_putc(make8(start_address,0));
      modbus_putc(make8(quantity,1));
      modbus_putc(make8(quantity,0));
      modbus_putc(count);
      for(i=5;i<(count+5);i+=2)
      {
         if(i >= (MODBUS_BUFFER_SIZE - 1))   //check to make sure the TX buffer won't be overran
            return(TRUE);
            
         modbus_putc(make8(*values,1));
         modbus_putc(make8(*values++,0));
      }
      
      modbus_tx_new = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
read_file_record
Input:     int8                address            Unit Identifier
           int8                byte_count         Number of bytes to read
           read_sub_request*   request            Structure holding record information
Output:    int1                                   0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_read_file_record(unsigned int8 address, unsigned int8 byte_count, modbus_read_sub_request *request)
{
   unsigned int16 i;
   
   if(!modbus_tx_new)   //check to see if there is data still in tx buffer to send
   {
      if(byte_count > 245)
         return(TRUE);
   
      ModbusLoadTxBufferStart(FUNC_READ_FILE_RECORD,address, 3 + (unsigned int16)byte_count);
      modbus_putc(byte_count,TRUE);
      for(i=1;i<(byte_count + 1);i+=7)
      {
         if(i >= (MODBUS_BUFFER_SIZE - 6))   //check to make sure the TX buffer won't be overran
            return(TRUE);
            
         modbus_putc(request->reference_type);
         modbus_putc(make8(request->file_number, 1));
         modbus_putc(make8(request->file_number, 0));
         modbus_putc(make8(request->record_number, 1));
         modbus_putc(make8(request->record_number, 0));
         modbus_putc(make8(request->record_length, 1));
         modbus_putc(make8(request->record_length, 0));
         request++;
      }
      
      modbus_tx_new = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
write_file_record
Input:     int8                address            Unit Identifier
           int8                byte_count         Number of bytes to read
           read_sub_request*   request            Structure holding record/data information
Output:    int1                                   0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_write_file_record(unsigned int8 address, unsigned int8 byte_count, modbus_write_sub_request *request)
{
   unsigned int16 i, j=0;

   if(!modbus_tx_new)   //check to see if there is data still in tx buffer to send
   {
      if(byte_count > 251)
         return(TRUE);
   
      ModbusLoadTxBufferStart(FUNC_WRITE_FILE_RECORD,address, 3 + (unsigned int16)byte_count);
      modbus_putc(byte_count,TRUE);
      
      for(i=1;i<(byte_count+1);i+=(7+(j*2)))
      {
         if(i >= (MODBUS_BUFFER_SIZE - 6 - (2 * request->record_length)))  //check to make sure the TX buffer won't be overran
            return(TRUE);

         modbus_putc(request->reference_type);
         modbus_putc(make8(request->file_number, 1));
         modbus_putc(make8(request->file_number, 0));
         modbus_putc(make8(request->record_number, 1));
         modbus_putc(make8(request->record_number, 0));
         modbus_putc(make8(request->record_length, 1));
         modbus_putc(make8(request->record_length, 0));

         for(j=0; (j < request->record_length) && (j < MODBUS_BUFFER_SIZE-8); j+=2)
         {
            modbus_putc(make8(request->data[j], 1));
            modbus_putc(make8(request->data[j], 0));
         }
         request++;
      }
      
      modbus_tx_new = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
mask_write_register
Input:     int8       address            Unit Identifier
           int16      reference_address  Address to mask
           int16      AND_mask           A mask to AND with the data at reference_address
           int16      OR_mask            A mask to OR with the data at reference_address
Output:    int1                          0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_mask_write_register(unsigned int8 address, unsigned int16 reference_address, unsigned int16 AND_mask, unsigned int16 OR_mask)
{
   if(!modbus_tx_new)   //check to see if there is data still in tx buffer to send
   {
      ModbusLoadTxBufferStart(FUNC_MASK_WRITE_REGISTER,address,8);
      
      modbus_putc(make8(reference_address,1),TRUE);
      modbus_putc(make8(reference_address,0));
      modbus_putc(make8(AND_mask,1));
      modbus_putc(make8(AND_mask,0));
      modbus_putc(make8(OR_mask, 1));
      modbus_putc(make8(OR_mask, 0));
      
      modbus_tx_new = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
read_write_multiple_registers
Input:     int8       address                Unit Identifier
           int16      read_start             Address to start reading
           int16      read_quantity          Amount of registers to read
           int16      write_start            Address to start writing
           int16      write_quantity         Amount of registers to write
           int16*     write_registers_value  Pointer to an aray us to write
Output:    int1                              0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_read_write_multiple_registers(unsigned int8 address, unsigned int16 read_start,
                                    unsigned int16 read_quantity, unsigned int16 write_start,
                                    unsigned int16 write_quantity,
                                    unsigned int16 *write_registers_value)
{
   unsigned int16 i,write_count;
   
   if(!modbus_tx_new)   //check to see if there is data still in tx buffer to send
   {
      if((read_quantity > 125) || (write_quantity > 121))
         return(TRUE);
         
      write_count = 2*write_quantity;
      
      ModbusLoadTxBufferStart(FUNC_READ_WRITE_MULTIPLE_REGISTERS,address,11 + write_count);
      
      modbus_putc(make8(read_start,1),TRUE);
      modbus_putc(make8(read_start,0));  
      modbus_putc(make8(read_quantity,1));
      modbus_putc(make8(read_quantity,0));
      modbus_putc(make8(write_start, 1));
      modbus_putc(make8(write_start, 0));
      modbus_putc(make8(write_quantity, 1));
      modbus_putc(make8(write_quantity, 0));
      modbus_putc(write_count);
   
      for(i=9; i < (write_count+9); i+=2)
      {
         if(i >= (MODBUS_BUFFER_SIZE - 1))   //check to make sure the TX buffer won't be overran
            return(TRUE);
      
         modbus_putc(make8(*write_registers_value, 1));
         modbus_putc(make8(*write_registers_value++, 0));
      }
   
      modbus_tx_new = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
read_FIFO_queue
Input:     int8       address           Unit Identifier
           int16      FIFO_address      FIFO address
Output:    int1                         0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_read_FIFO_queue(unsigned int8 address, unsigned int16 FIFO_address)
{
   if(!modbus_tx_new)   //check to see if there is data still in tx buffer to send
   {
      ModbusLoadTxBufferStart(FUNC_READ_FIFO_QUEUE,address,4);

      modbus_putc(make8(FIFO_address, 1),TRUE);
      modbus_putc(make8(FIFO_address, 0));
      
      modbus_tx_new = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

#else
//////////////////////////////////////////////////////////////////////////////////////////
//// Server API                                                                       ////
//////////////////////////////////////////////////////////////////////////////////////////

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
int1 modbus_read_coils_rsp(MBAP_HEADER MBAPHeader, unsigned int8 byte_count, unsigned int8* coil_data)
{
   unsigned int8 i;
   
   if(!modbus_tx_new[MBAPHeader.WhichSocket])
   {
      MobusLoadTxBufferStart(FUNC_READ_COILS, MBAPHeader, 3 + byte_count);
      
      modbus_putc(MBAPHeader.WhichSocket,byte_count,TRUE);

      for(i=1; i < (byte_count+1); i++)
      {
         if(i >= MODBUS_BUFFER_SIZE)   //check to make sure the TX buffer wasn't overran
            return(TRUE);
      
         modbus_putc(MBAPHeader.WhichSocket,*coil_data++);
      }
      
      modbus_tx_new[MBAPHeader.WhichSocket] = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
read_discrete_input_rsp
Input:     MPAB_HEADER  MBAPHeader       Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int8         byte_count       Number of bytes being sent
           int8*        input_data       Pointer to an array of data to send
Output:    int1                          0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_read_discrete_input_rsp(MBAP_HEADER MBAPHeader, unsigned int8 byte_count, unsigned int8 *input_data)
{
   unsigned int8 i;
   
   if(!modbus_tx_new[MBAPHeader.WhichSocket])
   {
      MobusLoadTxBufferStart(FUNC_READ_DISCRETE_INPUT, MBAPHeader, 3 + byte_count);
      
      modbus_putc(MBAPHeader.WhichSocket,byte_count,TRUE);
      
      for(i=1; i < (byte_count+1); i++)
      {
         if(i >= MODBUS_BUFFER_SIZE)   //check to make sure the TX buffer wasn't overran
            return(TRUE);
      
         modbus_putc(MBAPHeader.WhichSocket,*input_data++);
      }
   
      modbus_tx_new[MBAPHeader.WhichSocket] = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
read_holding_registers_rsp
Input:     MPAB_HEADER  MBAPHeader       Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int8         byte_count       Number of bytes being sent
           int16*       reg_data         Pointer to an array of data to send
Output:    int1                          0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_read_holding_registers_rsp(MBAP_HEADER MBAPHeader, unsigned int8 byte_count, unsigned int16 *reg_data)
{
   unsigned int8 i;
   
   if(!modbus_tx_new[MBAPHeader.WhichSocket])
   {
      MobusLoadTxBufferStart(FUNC_READ_HOLDING_REGISTERS,MBAPHeader,3+byte_count);
      
      modbus_putc(MBAPHeader.WhichSocket,byte_count,TRUE);
      
      for(i=1; i < (byte_count+1); i+=2)
      {
         if(i >= (MODBUS_BUFFER_SIZE-1))  //check to make sure the TX buffer wasn't overran
            return(TRUE);
      
         modbus_putc(MBAPHeader.WhichSocket,make8(*reg_data,1));
         modbus_putc(MBAPHeader.WhichSocket,make8(*reg_data++,0));
      }
   
      modbus_tx_new[MBAPHeader.WhichSocket] = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
read_input_registers_rsp
Input:     MPAB_HEADER  MBAPHeader       Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int8         byte_count       Number of bytes being sent
           int16*       input_data       Pointer to an array of data to send
Output:    int1                          0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_read_input_registers_rsp(MBAP_HEADER MBAPHeader, unsigned int8 byte_count, unsigned int16 *input_data)
{
   unsigned int8 i;
   
   if(!modbus_tx_new[MBAPHeader.WhichSocket])
   {
      MobusLoadTxBufferStart(FUNC_READ_INPUT_REGISTERS,MBAPHeader,3+byte_count);
      
      modbus_putc(MBAPHeader.WhichSocket,byte_count,TRUE);

      for(i=1; i < (byte_count+1); i+=2)
      {
         if(i >= (MODBUS_BUFFER_SIZE-1))  //check to make sure the TX buffer wasn't overran
            return(TRUE);
            
         modbus_putc(MBAPHeader.WhichSocket,make8(*input_data,1));
         modbus_putc(MBAPHeader.WhichSocket,make8(*input_data++,0));
      }
   
      modbus_tx_new[MBAPHeader.WhichSocket] = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
write_single_coil_rsp
Input:     MPAB_HEADER  MBAPHeader       Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int16        output_address   Echo of output address received
           int16        output_value     Echo of output value received
Output:    int1                          0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_write_single_coil_rsp(MBAP_HEADER MBAPHeader, unsigned int16 output_address, unsigned int16 output_value)
{
   if(!modbus_tx_new[MBAPHeader.WhichSocket])
   {
      MobusLoadTxBufferStart(FUNC_WRITE_SINGLE_COIL,MBAPHeader,6);
      
      modbus_putc(MBAPHeader.WhichSocket,make8(output_address,1),TRUE);
      modbus_putc(MBAPHeader.WhichSocket,make8(output_address,0));
   
      modbus_putc(MBAPHeader.WhichSocket,make8(output_value,1));
      modbus_putc(MBAPHeader.WhichSocket,make8(output_value,0));
   
      modbus_tx_new[MBAPHeader.WhichSocket] = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
write_single_register_rsp
Input:     MPAB_HEADER  MBAPHeader       Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int16        reg_address      Echo of register address received
           int16        reg_value        Echo of register value received
Output:    int1                          0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_write_single_register_rsp(MBAP_HEADER MBAPHeader, unsigned int16 reg_address, unsigned int16 reg_value)
{
   if(!modbus_tx_new[MBAPHeader.WhichSocket])
   {
      MobusLoadTxBufferStart(FUNC_WRITE_SINGLE_REGISTER,MBAPHeader,6);

      modbus_putc(MBAPHeader.WhichSocket,make8(reg_address,1),TRUE);
      modbus_putc(MBAPHeader.WhichSocket,make8(reg_address,0));
   
      modbus_putc(MBAPHeader.WhichSocket,make8(reg_value,1));
      modbus_putc(MBAPHeader.WhichSocket,make8(reg_value,0));
   
      modbus_tx_new[MBAPHeader.WhichSocket] = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
write_multiple_coils_rsp
Input:     MPAB_HEADER  MBAPHeader       Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int16        start_address    Echo of address to start at
           int16        quantity         Echo of amount of coils written to
Output:    int1                          0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_write_multiple_coils_rsp(MBAP_HEADER MBAPHeader, unsigned int16 start_address, unsigned int16 quantity)
{
  if(!modbus_tx_new[MBAPHeader.WhichSocket])
   {
      MobusLoadTxBufferStart(FUNC_WRITE_MULTIPLE_COILS,MBAPHeader,6);

      modbus_putc(MBAPHeader.WhichSocket,make8(start_address,1),TRUE);
      modbus_putc(MBAPHeader.WhichSocket,make8(start_address,0));
   
      modbus_putc(MBAPHeader.WhichSocket,make8(quantity,1));
      modbus_putc(MBAPHeader.WhichSocket,make8(quantity,0));
   
      modbus_tx_new[MBAPHeader.WhichSocket] = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
write_multiple_registers_rsp
Input:     MPAB_HEADER  MBAPHeader       Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int16        start_address    Echo of address to start at
           int16        quantity         Echo of amount of registers written to
Output:    int1                          0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_write_multiple_registers_rsp(MBAP_HEADER MBAPHeader, unsigned int16 start_address, unsigned int16 quantity)
{
   if(!modbus_tx_new[MBAPHeader.WhichSocket])
   {
      MobusLoadTxBufferStart(FUNC_WRITE_MULTIPLE_REGISTERS,MBAPHeader,6);

      modbus_putc(MBAPHeader.WhichSocket,make8(start_address,1),TRUE);
      modbus_putc(MBAPHeader.WhichSocket,make8(start_address,0));
   
      modbus_putc(MBAPHeader.WhichSocket,make8(quantity,1));
      modbus_putc(MBAPHeader.WhichSocket,make8(quantity,0));
   
      modbus_tx_new[MBAPHeader.WhichSocket] = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
read_file_record_rsp
Input:     MPAB_HEADER              MBAPHeader        Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int8                     byte_count        Number of bytes to send
           read_sub_request_rsp*    request           Structure holding record/data information
Output:    int1                                       0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_read_file_record_rsp(MBAP_HEADER MBAPHeader, unsigned int8 byte_count, modbus_read_sub_request_rsp *request)
{
   unsigned int8 i=1,j;

   if(!modbus_tx_new[MBAPHeader.WhichSocket])
   {
      MobusLoadTxBufferStart(FUNC_READ_FILE_RECORD,MBAPHeader,3 + byte_count);

      modbus_putc(MBAPHeader.WhichSocket,byte_count,TRUE);
   
      while(i < (byte_count+1))
      {
         if(i >= (MODBUS_BUFFER_SIZE - 1 - (request->record_length*2))) //check to make sure the TX buffer wasn't overran
            return(TRUE);
      
         modbus_putc(MBAPHeader.WhichSocket,request->record_length);
         modbus_putc(MBAPHeader.WhichSocket,request->reference_type);
   
         for(j=0; (j < request->record_length); j++)
         {
            modbus_putc(MBAPHeader.WhichSocket,make8(request->data[j], 1));
            modbus_putc(MBAPHeader.WhichSocket,make8(request->data[j], 0));
         }
   
         i += (((request->record_length)*2)+2);
         request++;
      }
   
      modbus_tx_new[MBAPHeader.WhichSocket] = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
write_file_record_rsp
Input:     MPAB_HEADER              MBAPHeader        Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int8                     byte_count        Echo of number of bytes sent
           write_sub_request_rsp*   request           Echo of Structure holding record information
Output:    int1                                       0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_write_file_record_rsp(MBAP_HEADER MBAPHeader, unsigned int8 byte_count, modbus_write_sub_request_rsp *request)
{
   unsigned int8 i, j=0;

   if(!modbus_tx_new[MBAPHeader.WhichSocket])
   {
      MobusLoadTxBufferStart(FUNC_WRITE_FILE_RECORD,MBAPHeader,3 + byte_count);

      modbus_putc(MBAPHeader.WhichSocket,byte_count,TRUE);
   
      for(i=1; i < (byte_count+1); i+=(7+(j*2)))
      {
         if(i >= (MODBUS_BUFFER_SIZE - 6 -((request->record_length)*2))) //check to make sure the TX buffer wasn't overran
            return(TRUE);
      
         modbus_putc(MBAPHeader.WhichSocket,request->reference_type);
         modbus_putc(MBAPHeader.WhichSocket,make8(request->file_number, 1));
         modbus_putc(MBAPHeader.WhichSocket,make8(request->file_number, 0));
         modbus_putc(MBAPHeader.WhichSocket,make8(request->record_number, 1));
         modbus_putc(MBAPHeader.WhichSocket,make8(request->record_number, 0));
         modbus_putc(MBAPHeader.WhichSocket,make8(request->record_length, 1));
         modbus_putc(MBAPHeader.WhichSocket,make8(request->record_length, 0));
   
         for(j=0; (j < request->record_length); j++)
         {
            modbus_putc(MBAPHeader.WhichSocket,make8(request->data[j], 1));
            modbus_putc(MBAPHeader.WhichSocket,make8(request->data[j], 0));
         }
      }
   
      modbus_tx_new[MBAPHeader.WhichSocket] = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
mask_write_register_rsp
Input:     MPAB_HEADER  MBAPHeader        Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int16        reference_address Echo of reference address
           int16        AND_mask          Echo of AND mask
           int16        OR_mask           Echo or OR mask
Output:    int1                           0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_mask_write_register_rsp(MBAP_HEADER MBAPHeader, unsigned int16 reference_address, unsigned int16 AND_mask, unsigned int16 OR_mask)
{
   if(!modbus_tx_new[MBAPHeader.WhichSocket])
   {
      MobusLoadTxBufferStart(FUNC_MASK_WRITE_REGISTER,MBAPHeader,8);

      modbus_putc(MBAPHeader.WhichSocket,make8(reference_address,1),TRUE);
      modbus_putc(MBAPHeader.WhichSocket,make8(reference_address,0));
   
      modbus_putc(MBAPHeader.WhichSocket,make8(AND_mask,1));
      modbus_putc(MBAPHeader.WhichSocket,make8(AND_mask,0));
   
      modbus_putc(MBAPHeader.WhichSocket,make8(OR_mask, 1));
      modbus_putc(MBAPHeader.WhichSocket,make8(OR_mask, 0));

      modbus_tx_new[MBAPHeader.WhichSocket] = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
read_write_multiple_registers_rsp
Input:     MPAB_HEADER  MBAPHeader        Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int8         data_len          Length of data in bytes
           int16*       data              Pointer to an array of data
Output:    int1                           0 if successfully placed in TX buffer, 1 if unsuccessful
*/
int1 modbus_read_write_multiple_registers_rsp(MBAP_HEADER MBAPHeader, unsigned int8 data_len, unsigned int16 *data)
{
   unsigned int8 i;

   if(!modbus_tx_new[MBAPHeader.WhichSocket])
   {
      MobusLoadTxBufferStart(FUNC_READ_WRITE_MULTIPLE_REGISTERS,MBAPHeader,3 + data_len);

      modbus_putc(MBAPHeader.WhichSocket,data_len,TRUE);
   
      for(i=1; i < (data_len+1); i+=2)
      {
         if(i >= (MODBUS_BUFFER_SIZE - 1))   //check to make sure the TX buffer wasn't overran
            return(TRUE);
            
         modbus_putc(MBAPHeader.WhichSocket,make8(data[i], 1));
         modbus_putc(MBAPHeader.WhichSocket,make8(data[i], 0));
      }
   
      modbus_tx_new[MBAPHeader.WhichSocket] = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
read_FIFO_queue_rsp
Input:     MPAB_HEADER  MBAPHeader        Received MPAB Header used to create MPAB Header of response and which socket to respond to
           int16        FIFO_len          Length of FIFO in bytes
           int16*       data              Pointer to an array of data
Output:    int1                           0 if successfully placed in TX buffer, 1 if unsuccessfulvoid
*/
int1 modbus_read_FIFO_queue_rsp(MBAP_HEADER MBAPHeader, unsigned int16 FIFO_len, unsigned int16 *data)
{
   unsigned int8 i;
   unsigned int16 byte_count;
   
   if(!modbus_tx_new[MBAPHeader.WhichSocket])
   {
      byte_count = ((FIFO_len*2)+2);
      
      MobusLoadTxBufferStart(FUNC_READ_FIFO_QUEUE,MBAPHeader,4+byte_count);

      modbus_putc(MBAPHeader.WhichSocket,make8(byte_count, 1),TRUE);
      modbus_putc(MBAPHeader.WhichSocket,make8(byte_count, 0));
   
      modbus_putc(MBAPHeader.WhichSocket,make8(FIFO_len, 1));
      modbus_putc(MBAPHeader.WhichSocket,make8(FIFO_len, 0));
   
      for(i=4; i < (FIFO_len+4); i+=2)
      {
         if(i >= (MODBUS_BUFFER_SIZE - 1))   //check to make sure the TX buffer wasn't overran
            return(TRUE);
         
         modbus_putc(MBAPHeader.WhichSocket,make8(data[i], 1));
         modbus_putc(MBAPHeader.WhichSocket,make8(data[i], 0));
      }
   
      modbus_tx_new[MBAPHeader.WhichSocket] = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

/*
modbus_exception_rsp
Input:     MPAB_HEADER  MBAPHeader        Received MPAB Header used to create MPAB Header of response and which socket to respond to
           function     func              function to respond to
           exception    error             exception response to send
Output:    int1                           0 if successfully placed in TX buffer, 1 if unsuccessfulvoid
*/
int1 modbus_exception_rsp(MBAP_HEADER MBAPHeader, function func, exception error)
{
   if(!modbus_tx_new[MBAPHeader.WhichSocket])
   {
      MobusLoadTxBufferStart(func|0x80,MBAPHeader,3);

      modbus_putc(MBAPHeader.WhichSocket,error,TRUE);
      
      modbus_tx_new[MBAPHeader.WhichSocket] = TRUE;
      return(FALSE);
   }
   else
      return(TRUE);
}

#endif

#if (MODBUS_TYPE == MODBUS_TYPE_CLIENT)
/*
ModbusServerAddrInit
Input:               none
Output:              void
*/
void ModbusServerAddrInit(void)
{
   server.v[0]=MODBUS_SERVER_ADDR_0;
   server.v[1]=MODBUS_SERVER_ADDR_1;
   server.v[2]=MODBUS_SERVER_ADDR_2;
   server.v[3]=MODBUS_SERVER_ADDR_3;
}

/*
ModbusTCPTask
Input:         none
Output:        void
*/
void ModbusTCPTask(void)
{
   static TICK lastTick;
   static TCP_SOCKET socket=INVALID_SOCKET;
   static MODBUS_TCP_STATE state = MODBUS_TCP_STATE_CONNECT;
   TICK currTick;
   int8 dis;
   static char ConnectServer[16];
   
   currTick=TickGet();
   
   switch (state) {
      case MODBUS_TCP_STATE_CONNECT:
         sprintf(ConnectServer,"%u.%u.%u.%u",server.v[0], server.v[1], server.v[2], server.v[3]);
         debug_printf("\n\rCONNECTING TO %s", ConnectServer);         
         socket = TCPOpen(ConnectServer, TCP_OPEN_RAM_HOST, MODBUS_SERVER_LISTEN_PORT, TCP_PURPOSE_DEFAULT);
         if (socket!=INVALID_SOCKET) {
            lastTick=TickGet();
            state=MODBUS_TCP_STATE_CONNECT_WAIT;
         }
         else {
            debug_printf("\n\rSOCKET ERROR");
         }
         break;

      case MODBUS_TCP_STATE_CONNECT_WAIT:
         if (TCPIsConnected(socket)) {
            state=MODBUS_TCP_STATE_CONNECTED;
            debug_printf("\r\nCONNECTED TO %s", ConnectServer);
            debug_printf("\r\nMY IP: %u.%u.%u.%u", AppConfig.MyIPAddr.v[0], AppConfig.MyIPAddr.v[1], AppConfig.MyIPAddr.v[2], AppConfig.MyIPAddr.v[3]);
         }
         else if (TickGetDiff(currTick, lastTick) > (TICKS_PER_SECOND * 10)) {
            state=MODBUS_TCP_STATE_FORCE_DISCONNECT;
         }
         break;

      case MODBUS_TCP_STATE_CONNECTED:
         if (TCPIsConnected(socket)) {
            ModbusConnectedTask(socket);
            dis = FALSE;
            if (dis) {
               state=MODBUS_TCP_STATE_DISCONNECT;
               lastTick=currTick;
            }
         }
         else {
            debug_printf("\n\rDISCONNECTED");
            state=MODBUS_TCP_STATE_CONNECT;
         }
         break;

      case MODBUS_TCP_STATE_DISCONNECT:
         debug_printf("\n\rDISCONNECT FROM ");
         debug_printf("%U.%U.%U.%U", server.v[0], server.v[1], server.v[2], server.v[3]);
         if (TCPIsPutReady(socket)) {
            state=MODBUS_TCP_STATE_FORCE_DISCONNECT;
         }
         else if (TickGetDiff(currTick, lastTick) > (TICKS_PER_SECOND * 10)) {
            state=MODBUS_TCP_STATE_FORCE_DISCONNECT;
         }
         break;

      case MODBUS_TCP_STATE_FORCE_DISCONNECT:
         TCPDisconnect(socket);
         state=MODBUS_TCP_STATE_CONNECT;
         break;
   }
}

/*
ModbusConnectedTask
Input:               TCP_SOCKET  socket   Socket to receive or transmit message on
Output:              void
*/
void ModbusConnectedTask(TCP_SOCKET socket)
{
   static MODBUS_STATE state = MODBUS_IDLE;
   static TICK lastTick;
   TICK currTick;
   unsigned int8 data;
   int8 count;
   MODBUS_RX_STATE rx_state;
   
   currTick = TickGet();
   
   switch(state)
   {
      case MODBUS_IDLE:
         if(modbus_tx_new)
            state = MODBUS_SEND_MESSAGE;
         
         TCPDiscard(socket);
         break;
      case MODBUS_SEND_MESSAGE:
         TCPDiscard(socket);
         
         if(TCPIsPutReady(socket))
         {
            TCPPut(socket,make8(modbus_tx.MBAPHeader.TransactionIdentifier,1));
            TCPPut(socket,make8(modbus_tx.MBAPHeader.TransactionIdentifier,0));
            TCPPut(socket,make8(modbus_tx.MBAPHeader.ProtocolIdentifier,1));
            TCPPut(socket,make8(modbus_tx.MBAPHeader.ProtocolIdentifier,0));
            TCPPut(socket,make8(modbus_tx.MBAPHeader.Length,1));
            TCPPut(socket,make8(modbus_tx.MBAPHeader.Length,0));
            TCPPut(socket,modbus_tx.MBAPHeader.UnitIdentifier);
            TCPPut(socket,modbus_tx.func);
            TCPPutArray(socket,modbus_tx.data,modbus_tx.MBAPHeader.Length-2);
            TCPFlush(socket);
            modbus_tx_new = FALSE;
            state = MODBUS_WAIT_FOR_RESPONSE;
            lastTick = currTick;
         }      
         break;
      case MODBUS_WAIT_FOR_RESPONSE:
         if(TCPIsGetReady(socket))
         {
            count = 0;
            rx_state = MODBUS_RX_TRANSACTION_IDENTIFIER;
            
            debug_printf("\n\n\r");
            
            while(TCPGet(socket,&data))
            {
               debug_printf("%X ",data);
               switch(rx_state)
               {
                  case MODBUS_RX_TRANSACTION_IDENTIFIER:
                     if(++count == 2)
                        rx_state = MODBUS_RX_PROTOCOL_IDENTIFER;
                     break;
                  case MODBUS_RX_PROTOCOL_IDENTIFER:
                     if(++count == 4)
                        rx_state = MODBUS_RX_LENGTH;
                     break;
                  case MODBUS_RX_LENGTH:
                     if (++count == 5)
                        modbus_rx.len = data;
                     else
                     {
                        modbus_rx.len = ((modbus_rx.len << 8) + data) - 2;    //subtact 2 off for Unit Identifier and Function Code
                        rx_state = MODBUS_RX_UNIT_IDENTIFIER;
                     }
                     break;
                  case MODBUS_RX_UNIT_IDENTIFIER:
                     modbus_rx.address = data;
                     rx_state = MODBUS_RX_FUNCTION_CODE;
                     break;
                  case MODBUS_RX_FUNCTION_CODE:
                     modbus_rx.func = data;
                     count = 0;
                     rx_state = MODBUS_RX_DATA;
                     break;
                  case MODBUS_RX_DATA:
                     if(count >= MODBUS_BUFFER_SIZE)
                        count = MODBUS_BUFFER_SIZE - 1;
                        
                     modbus_rx.data[count] = data;
                     count++;
                     break;
               }
            }
            
            debug_printf("\n\n\r");
            
            if(modbus_rx.func & 0x80)
               modbus_rx.error = modbus_rx.data[0];
            else
               modbus_rx.error = 0;
               
            modbus_rx_new = TRUE;            
            state = MODBUS_IDLE;
         }
         else if(TickGetDiff(currTick,lastTick) > (TICKS_PER_SECOND*MODBUS_SERVER_TIMEOUT))
         {
            modbus_rx.error = TIMEOUT;
            modbus_rx_new = TRUE;
            state = MODBUS_IDLE;
         }
         break;
   }
}

#else
/*
ModbusTCPTask
Input:         none
Output:        void
*/
void ModbusTCPTask(void)
{
   static TICK lastTick[MODBUS_LISTEN_SOCKETS];
   static MODBUS_TCP_STATE state[MODBUS_LISTEN_SOCKETS]={0};
   TICK currTick;
   unsigned int8 i;
   
   currTick=TickGet();

   for (i=0;i<MODBUS_LISTEN_SOCKETS;i++) 
   {
      switch(state[i])
      {
         case MODBUS_TCP_STATE_NEW:
            if (socket[i] == INVALID_SOCKET)
               socket[i]=TCPListen(MODBUS_SERVER_LISTEN_PORT);
            if (socket[i]!=INVALID_SOCKET)
            {
               state[i]=MODBUS_TCP_STATE_LISTENING;
               debug_printf("\n\rLISTENING ON ");
               debug_printf("%U.%U.%U.%U", MY_IP.v[0], MY_IP.v[1], MY_IP.v[2], MY_IP.v[3]);
            }
            else 
            {
               debug_printf("\n\rSOCKET ERROR");
            }
            break;

         case MODBUS_TCP_STATE_LISTENING:
            if (TCPIsConnected(socket[i])) 
            {
               state[i]=MODBUS_TCP_STATE_CONNECTED;
               debug_printf("\n\rCONNECTED!");
               lastTick[i]=currTick;
            }
            break;

         case MODBUS_TCP_STATE_CONNECTED:
            if (TCPIsConnected(socket[i])) 
            {
               if (TickGetDiff(currTick,lastTick[i]) > ((int16)TICKS_PER_SECOND * 300)) 
               {
                  state[i]=MODBUS_TCP_STATE_DISCONNECT;
                  debug_printf("\n\rTIMEOUT\n\rDISCONNECTING");
                  lastTick[i]=currTick;
               }
               else 
               {
                  ModbusConnectedTask(socket[i],i);
               }
            }
            else 
            {
               debug_printf("\n\rDISCONNECTED");
               state[i]=MODBUS_TCP_STATE_FORCE_DISCONNECT;
            }
            break;

         case MODBUS_TCP_STATE_DISCONNECT:
            if (TCPIsPutReady(socket[i]))
            {
               state[i]=MODBUS_TCP_STATE_FORCE_DISCONNECT;
            }
            else if (TickGetDiff(currTick, lastTick[i]) > (TICKS_PER_SECOND * 10)) {
               state[i]=MODBUS_TCP_STATE_FORCE_DISCONNECT;
            }
            break;

         case MODBUS_TCP_STATE_FORCE_DISCONNECT:
            TCPDisconnect(socket[i]);
            state[i]=MODBUS_TCP_STATE_NEW;
            break;
      }
   }
}

/*
ModbusConnectedTask
Input:               TCP_SOCKET  socket   Socket to receive or transmit message on
                     int8        which    Which TX/RX buffer to transmit/receive message from/on
Output:              void
*/
void ModbusConnectedTask(TCP_SOCKET socket,unsigned int8 which)
{
   static MODBUS_STATE state[MODBUS_LISTEN_SOCKETS];
   int1 execute = TRUE;
   unsigned int8 count;
   MODBUS_RX_STATE rx_state;
   unsigned int8 data;
   
   while(execute)
   {
      switch(state[which])
      {
         case MODBUS_IDLE:
            if(TCPIsGetReady(socket))
            {
               state[which] = MODBUS_GET_MESSAGE;
               continue;
            }
               
            if(modbus_tx_new[which])
            {
               state[which] = MODBUS_SEND_RESPONSE;
            }
            
            execute = FALSE;
            break;
         case MODBUS_GET_MESSAGE:
            count = 0;
            rx_state = MODBUS_RX_TRANSACTION_IDENTIFIER;
            
            debug_printf("\n\n\r");
            
            while(TCPGet(socket,&data))
            {
               debug_printf("%X ",data);
            
               switch(rx_state)
               {
                  case MODBUS_RX_TRANSACTION_IDENTIFIER:
                     if(count == 0)
                     {
                        modbus_rx[which].MBAPHeader.TransactionIdentifier = (unsigned int16)data << 8;
                        count++;
                     }
                     else
                     {
                        modbus_rx[which].MBAPHeader.TransactionIdentifier |= data;
                        count = 0;
                        rx_state = MODBUS_RX_PROTOCOL_IDENTIFER;
                     }
                     break;
                  case MODBUS_RX_PROTOCOL_IDENTIFER:
                     if(count == 0)
                     {
                        modbus_rx[which].MBAPHeader.ProtocolIdentifier = (unsigned int16)data << 8;
                        count++;
                     }
                     else
                     {
                        modbus_rx[which].MBAPHeader.ProtocolIdentifier |= data;
                        count = 0;
                        rx_state = MODBUS_RX_LENGTH;
                     }
                     break;
                  case MODBUS_RX_LENGTH:
                     if(count == 0)
                     {
                        modbus_rx[which].MBAPHeader.Length = (unsigned int16)data << 8;
                        count++;
                     }
                     else
                     {
                        modbus_rx[which].MBAPHeader.Length |= data;
                        modbus_rx[which].len = modbus_rx[which].MBAPHeader.Length - 2;  //subtact 2 off for Unit Identifier and Function Code
                        count = 0;
                        rx_state = MODBUS_RX_UNIT_IDENTIFIER;
                     }
                     break;
                  case MODBUS_RX_UNIT_IDENTIFIER:
                     modbus_rx[which].MBAPHeader.UnitIdentifier = data;
                     rx_state = MODBUS_RX_FUNCTION_CODE;
                     break;
                  case MODBUS_RX_FUNCTION_CODE:
                     modbus_rx[which].func = data;
                     rx_state = MODBUS_RX_DATA;
                     break;
                  case MODBUS_RX_DATA:
                     if(count >= MODBUS_BUFFER_SIZE)
                        count = MODBUS_BUFFER_SIZE - 1;
                        
                     modbus_rx[which].data[count] = data;
                     count++;
                     break;
               }
            }
            
            debug_printf("\n\n\r");
            
            modbus_rx[which].MBAPHeader.WhichSocket = which;
            modbus_rx_new[which] = TRUE;            
            state[which] = MODBUS_IDLE;
            execute = FALSE;
            break;
         case MODBUS_SEND_RESPONSE:
            TCPDiscard(socket);
            
            if(TCPIsPutReady(socket))
            {
               TCPPut(socket,make8(modbus_tx[which].MBAPHeader.TransactionIdentifier,1));
               TCPPut(socket,make8(modbus_tx[which].MBAPHeader.TransactionIdentifier,0));
               TCPPut(socket,make8(modbus_tx[which].MBAPHeader.ProtocolIdentifier,1));
               TCPPut(socket,make8(modbus_tx[which].MBAPHeader.ProtocolIdentifier,0));
               TCPPut(socket,make8(modbus_tx[which].MBAPHeader.Length,1));
               TCPPut(socket,make8(modbus_tx[which].MBAPHeader.Length,0));
               TCPPut(socket,modbus_tx[which].MBAPHeader.UnitIdentifier);
               TCPPut(socket,modbus_tx[which].func);
               TCPPutArray(socket,modbus_tx[which].data,modbus_tx[which].MBAPHeader.Length-2);
               TCPFlush(socket);
               modbus_tx_new[which] = FALSE;
               state[which] = MODBUS_IDLE;
            }
            
            execute = FALSE;
            break;
      }
   }
}

#endif

#endif
