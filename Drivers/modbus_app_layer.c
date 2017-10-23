//////////////////////////////////////////////////////////////////////////////////////////
////                                modbus_app_layer.c                                ////
////                                                                                  ////
////    Application layer of the MODBUS protocol driver for serial communications.    ////
////                                                                                  ////
////  Refer to documentation at http://www.modbus.org for more information on MODBUS. ////
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

#ifndef MODBUS_APP_LAYER_C
#define MODBUS_APP_LAYER_C

#include <modbus_app_layer.h>

//////////////////////////////////////////////////////////////////////////////////////////
//// Shared Api                                                                       ////
//////////////////////////////////////////////////////////////////////////////////////////

// Purpose:    Initialize RS485 communication. Call this before
//             using any other RS485 functions.
// Inputs:     None
// Outputs:    None
void modbus_init()
{
   output_low(MODBUS_SERIAL_ENABLE_PIN);

   RCV_ON();

   #if defined(__PCD__)
      enable_interrupts(INTR_GLOBAL);
   #else
      enable_interrupts(GLOBAL);
   #endif
}

// Purpose:    Get a message from the RS485 bus and store it in a buffer
// Inputs:     None
// Outputs:    TRUE if a message was received
//             FALSE if no message is available
// Note:       Data will be filled in at the modbus_rx struct:
int1 modbus_kbhit()
{
   #if(MODBUS_SERIAL_TYPE == MODBUS_RTU)
   modbus_check_timeout();
   #endif

   if(!modbus_serial_new)
     return FALSE;
   else if(modbus_rx.func & 0x80)           //did we receive an error?
   {
      modbus_rx.error = modbus_rx.data[0];  //if so grab the error and return true
      modbus_rx.len = 1;
   }
   modbus_serial_new=FALSE;
   return TRUE;
}

#if (MODBUS_TYPE==MODBUS_TYPE_MASTER)
//////////////////////////////////////////////////////////////////////////////////////////
//// Master API                                                                       ////
//////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************
The following functions are defined in the MODBUS protocol.  Please
refer to http://www.modbus.org for the purpose of each of these.
All functions take the slaves address as their first parameter.
Each function returns the exception code received from the response.
The function will return 0 if there were no errors in transmission.
********************************************************************/

/*
read_coils
Input:     int8       address            Slave Address
           int16      start_address      Address to start reading from
           int16      quantity           Amount of addresses to read
Output:    exception                     0 if no error, else the exception
*/
exception modbus_read_coils(unsigned int8 address, unsigned int16 start_address, unsigned int16 quantity)
{
   modbus_serial_send_start(address, FUNC_READ_COILS);

   modbus_serial_putc(make8(start_address,1));
   modbus_serial_putc(make8(start_address,0));

   modbus_serial_putc(make8(quantity,1));
   modbus_serial_putc(make8(quantity,0));

   modbus_serial_send_stop();

   MODBUS_SERIAL_WAIT_FOR_RESPONSE();

   return modbus_rx.error;
}

/*
read_discrete_input
Input:     int8       address            Slave Address
           int16      start_address      Address to start reading from
           int16      quantity           Amount of addresses to read
Output:    exception                     0 if no error, else the exception
*/
exception modbus_read_discrete_input(unsigned int8 address, unsigned int16 start_address, unsigned int16 quantity)
{
   modbus_serial_send_start(address, FUNC_READ_DISCRETE_INPUT);

   modbus_serial_putc(make8(start_address,1));
   modbus_serial_putc(make8(start_address,0));

   modbus_serial_putc(make8(quantity,1));
   modbus_serial_putc(make8(quantity,0));

   modbus_serial_send_stop();

   MODBUS_SERIAL_WAIT_FOR_RESPONSE();

   return modbus_rx.error;
}

/*
read_holding_registers
Input:     int8       address            Slave Address
           int16      start_address      Address to start reading from
           int16      quantity           Amount of addresses to read
Output:    exception                     0 if no error, else the exception
*/
exception modbus_read_holding_registers(unsigned int8 address, unsigned int16 start_address, unsigned int16 quantity)
{
   modbus_serial_send_start(address, FUNC_READ_HOLDING_REGISTERS);

   modbus_serial_putc(make8(start_address,1));
   modbus_serial_putc(make8(start_address,0));

   modbus_serial_putc(make8(quantity,1));
   modbus_serial_putc(make8(quantity,0));

   modbus_serial_send_stop();

   MODBUS_SERIAL_WAIT_FOR_RESPONSE();

   return modbus_rx.error;
}

/*
read_input_registers
Input:     int8       address            Slave Address
           int16      start_address      Address to start reading from
           int16      quantity           Amount of addresses to read
Output:    exception                     0 if no error, else the exception
*/
exception modbus_read_input_registers(unsigned int8 address, unsigned int16 start_address, unsigned int16 quantity)
{
   modbus_serial_send_start(address, FUNC_READ_INPUT_REGISTERS);

   modbus_serial_putc(make8(start_address,1));
   modbus_serial_putc(make8(start_address,0));

   modbus_serial_putc(make8(quantity,1));
   modbus_serial_putc(make8(quantity,0));

   modbus_serial_send_stop();

   MODBUS_SERIAL_WAIT_FOR_RESPONSE();

   return modbus_rx.error;
}

/*
write_single_coil
Input:     int8       address            Slave Address
           int16      output_address     Address to write into
           int1       on                 true for on, false for off
Output:    exception                     0 if no error, else the exception
*/
exception modbus_write_single_coil(unsigned int8 address, unsigned int16 output_address, int1 on)
{
   modbus_serial_send_start(address, FUNC_WRITE_SINGLE_COIL);

   modbus_serial_putc(make8(output_address,1));
   modbus_serial_putc(make8(output_address,0));

   if(on)
       modbus_serial_putc(0xFF);
   else
       modbus_serial_putc(0x00);

   modbus_serial_putc(0x00);

   modbus_serial_send_stop();

   MODBUS_SERIAL_WAIT_FOR_RESPONSE();

   return modbus_rx.error;
}

/*
write_single_register
Input:     int8       address            Slave Address
           int16      reg_address        Address to write into
           int16      reg_value          Value to write
Output:    exception                     0 if no error, else the exception
*/
exception modbus_write_single_register(unsigned int8 address, unsigned int16 reg_address, unsigned int16 reg_value)
{
   modbus_serial_send_start(address, FUNC_WRITE_SINGLE_REGISTER);

   modbus_serial_putc(make8(reg_address,1));
   modbus_serial_putc(make8(reg_address,0));

   modbus_serial_putc(make8(reg_value,1));
   modbus_serial_putc(make8(reg_value,0));

   modbus_serial_send_stop();

   MODBUS_SERIAL_WAIT_FOR_RESPONSE();

   return modbus_rx.error;
}

/*
read_exception_status
Input:     int8       address            Slave Address
Output:    exception                     0 if no error, else the exception
*/
exception modbus_read_exception_status(unsigned int8 address)
{
   modbus_serial_send_start(address, FUNC_READ_EXCEPTION_STATUS);
   modbus_serial_send_stop();

   MODBUS_SERIAL_WAIT_FOR_RESPONSE();

   return modbus_rx.error;
}

/*
diagnostics
Input:     int8       address            Slave Address
           int16      sub_func           Subfunction to send
           int16      data               Data to send, changes based on subfunction
Output:    exception                     0 if no error, else the exception
*/
exception modbus_diagnostics(unsigned int8 address, unsigned int16 sub_func, unsigned int16 data)
{
   modbus_serial_send_start(address, FUNC_DIAGNOSTICS);

   modbus_serial_putc(make8(sub_func,1));
   modbus_serial_putc(make8(sub_func,0));

   modbus_serial_putc(make8(data,1));
   modbus_serial_putc(make8(data,0));

   modbus_serial_send_stop();

   MODBUS_SERIAL_WAIT_FOR_RESPONSE();

   return modbus_rx.error;
}

/*
get_comm_event_couter
Input:     int8       address            Slave Address
Output:    exception                     0 if no error, else the exception
*/
exception modbus_get_comm_event_counter(unsigned int8 address)
{
   modbus_serial_send_start(address, FUNC_GET_COMM_EVENT_COUNTER);
   modbus_serial_send_stop();

   MODBUS_SERIAL_WAIT_FOR_RESPONSE();

   return modbus_rx.error;
}

/*
get_comm_event_log
Input:     int8       address            Slave Address
Output:    exception                     0 if no error, else the exception
*/
exception modbus_get_comm_event_log(unsigned int8 address)
{
   modbus_serial_send_start(address, FUNC_GET_COMM_EVENT_LOG);
   modbus_serial_send_stop();

   MODBUS_SERIAL_WAIT_FOR_RESPONSE();

   return modbus_rx.error;
}

/*
write_multiple_coils
Input:     int8       address            Slave Address
           int16      start_address      Address to start at
           int16      quantity           Amount of coils to write to
           int1*      values             A pointer to an array holding the values to write
Output:    exception                     0 if no error, else the exception
*/
exception modbus_write_multiple_coils(unsigned int8 address, unsigned int16 start_address, unsigned int16 quantity,
                           unsigned int8 *values)
{
   unsigned int8 i,count;

   count = (unsigned int8)((quantity/8));

   if(quantity%8)
      count++;

   modbus_serial_send_start(address, FUNC_WRITE_MULTIPLE_COILS);

   modbus_serial_putc(make8(start_address,1));
   modbus_serial_putc(make8(start_address,0));

   modbus_serial_putc(make8(quantity,1));
   modbus_serial_putc(make8(quantity,0));

   modbus_serial_putc(count);

   for(i=0; i < count; ++i)
      modbus_serial_putc(values[i]);

   modbus_serial_send_stop();

   MODBUS_SERIAL_WAIT_FOR_RESPONSE();

   return modbus_rx.error;
}

/*
write_multiple_registers
Input:     int8       address            Slave Address
           int16      start_address      Address to start at
           int16      quantity           Amount of coils to write to
           int16*     values             A pointer to an array holding the data to write
Output:    exception                     0 if no error, else the exception
*/
exception modbus_write_multiple_registers(unsigned int8 address, unsigned int16 start_address, unsigned int16 quantity,
                           unsigned int16 *values)
{
   unsigned int8 i,count;

   count = quantity*2;

   modbus_serial_send_start(address, FUNC_WRITE_MULTIPLE_REGISTERS);

   modbus_serial_putc(make8(start_address,1));
   modbus_serial_putc(make8(start_address,0));

   modbus_serial_putc(make8(quantity,1));
   modbus_serial_putc(make8(quantity,0));

   modbus_serial_putc(count);

   for(i=0; i < quantity; ++i)
   {
      modbus_serial_putc(make8(values[i],1));
      modbus_serial_putc(make8(values[i],0));
   }

   modbus_serial_send_stop();

   MODBUS_SERIAL_WAIT_FOR_RESPONSE();

   return modbus_rx.error;
}

/*
report_slave_id
Input:     int8       address            Slave Address
Output:    exception                     0 if no error, else the exception
*/
exception modbus_report_slave_id(unsigned int8 address)
{
   modbus_serial_send_start(address, FUNC_REPORT_SLAVE_ID);
   modbus_serial_send_stop();

   MODBUS_SERIAL_WAIT_FOR_RESPONSE();

   return modbus_rx.error;
}

/*
read_file_record
Input:     int8                address            Slave Address
           int8                byte_count         Number of bytes to read
           read_sub_request*   request            Structure holding record information
Output:    exception                              0 if no error, else the exception
*/
exception modbus_read_file_record(unsigned int8 address, unsigned int8 byte_count,
                            modbus_read_sub_request *request)
{
   unsigned int8 i;

   modbus_serial_send_start(address, FUNC_READ_FILE_RECORD);

   modbus_serial_putc(byte_count);

   for(i=0; i < (byte_count/7); i+=7)
   {
      modbus_serial_putc(request->reference_type);
      modbus_serial_putc(make8(request->file_number, 1));
      modbus_serial_putc(make8(request->file_number, 0));
      modbus_serial_putc(make8(request->record_number, 1));
      modbus_serial_putc(make8(request->record_number, 0));
      modbus_serial_putc(make8(request->record_length, 1));
      modbus_serial_putc(make8(request->record_length, 0));
      request++;
   }

   modbus_serial_send_stop();

   MODBUS_SERIAL_WAIT_FOR_RESPONSE();

   return modbus_rx.error;
}

/*
write_file_record
Input:     int8                address            Slave Address
           int8                byte_count         Number of bytes to read
           read_sub_request*   request            Structure holding record/data information
Output:    exception                              0 if no error, else the exception
*/
exception modbus_write_file_record(unsigned int8 address, unsigned int8 byte_count,
                            modbus_write_sub_request *request)
{
   unsigned int8 i, j=0;

   modbus_serial_send_start(address, FUNC_WRITE_FILE_RECORD);

   modbus_serial_putc(byte_count);

   for(i=0; i < byte_count; i+=(7+(j*2)))
   {
      modbus_serial_putc(request->reference_type);
      modbus_serial_putc(make8(request->file_number, 1));
      modbus_serial_putc(make8(request->file_number, 0));
      modbus_serial_putc(make8(request->record_number, 1));
      modbus_serial_putc(make8(request->record_number, 0));
      modbus_serial_putc(make8(request->record_length, 1));
      modbus_serial_putc(make8(request->record_length, 0));

      for(j=0; (j < request->record_length) &&
            (j < MODBUS_SERIAL_RX_BUFFER_SIZE-8); j+=2)
      {
         modbus_serial_putc(make8(request->data[j], 1));
         modbus_serial_putc(make8(request->data[j], 0));
      }
      request++;
   }

   modbus_serial_send_stop();

   MODBUS_SERIAL_WAIT_FOR_RESPONSE();

   return modbus_rx.error;
}

/*
mask_write_register
Input:     int8       address            Slave Address
           int16      reference_address  Address to mask
           int16      AND_mask           A mask to AND with the data at reference_address
           int16      OR_mask            A mask to OR with the data at reference_address
Output:    exception                              0 if no error, else the exception
*/
exception modbus_mask_write_register(unsigned int8 address, unsigned int16 reference_address,
                           unsigned int16 AND_mask, unsigned int16 OR_mask)
{
   modbus_serial_send_start(address, FUNC_MASK_WRITE_REGISTER);

   modbus_serial_putc(make8(reference_address,1));
   modbus_serial_putc(make8(reference_address,0));

   modbus_serial_putc(make8(AND_mask,1));
   modbus_serial_putc(make8(AND_mask,0));

   modbus_serial_putc(make8(OR_mask, 1));
   modbus_serial_putc(make8(OR_mask, 0));

   modbus_serial_send_stop();

   MODBUS_SERIAL_WAIT_FOR_RESPONSE();

   return modbus_rx.error;
}

/*
read_write_multiple_registers
Input:     int8       address                Slave Address
           int16      read_start             Address to start reading
           int16      read_quantity          Amount of registers to read
           int16      write_start            Address to start writing
           int16      write_quantity         Amount of registers to write
           int16*     write_registers_value  Pointer to an aray us to write
Output:    exception                         0 if no error, else the exception
*/
exception modbus_read_write_multiple_registers(unsigned int8 address, unsigned int16 read_start,
                                    unsigned int16 read_quantity, unsigned int16 write_start,
                                    unsigned int16 write_quantity,
                                    unsigned int16 *write_registers_value)
{
   unsigned int8 i;

   modbus_serial_send_start(address, FUNC_READ_WRITE_MULTIPLE_REGISTERS);

   modbus_serial_putc(make8(read_start,1));
   modbus_serial_putc(make8(read_start,0));

   modbus_serial_putc(make8(read_quantity,1));
   modbus_serial_putc(make8(read_quantity,0));

   modbus_serial_putc(make8(write_start, 1));
   modbus_serial_putc(make8(write_start, 0));

   modbus_serial_putc(make8(write_quantity, 1));
   modbus_serial_putc(make8(write_quantity, 0));

   modbus_serial_putc((unsigned int8)(2*write_quantity));

   for(i=0; i < write_quantity ; i+=2)
   {
      modbus_serial_putc(make8(write_registers_value[i], 1));
      modbus_serial_putc(make8(write_registers_value[i+1], 0));
   }

   modbus_serial_send_stop();

   MODBUS_SERIAL_WAIT_FOR_RESPONSE();

   return modbus_rx.error;
}

/*
read_FIFO_queue
Input:     int8       address           Slave Address
           int16      FIFO_address      FIFO address
Output:    exception                    0 if no error, else the exception
*/
exception modbus_read_FIFO_queue(unsigned int8 address, unsigned int16 FIFO_address)
{
   modbus_serial_send_start(address, FUNC_READ_FIFO_QUEUE);

   modbus_serial_putc(make8(FIFO_address, 1));
   modbus_serial_putc(make8(FIFO_address, 0));

   modbus_serial_send_stop();

   MODBUS_SERIAL_WAIT_FOR_RESPONSE();

   return modbus_rx.error;
}

#else
//////////////////////////////////////////////////////////////////////////////////////////
//// Slave API                                                                        ////
//////////////////////////////////////////////////////////////////////////////////////////

/********************************************************************
The following slave functions are defined in the MODBUS protocol.
Please refer to http://www.modbus.org for the purpose of each of
these.  All functions take the slaves address as their first
parameter.
********************************************************************/

/*
read_coils_rsp
Input:     int8       address            Slave Address
           int8       byte_count         Number of bytes being sent
           int8*      coil_data          Pointer to an array of data to send
Output:    void
*/
void modbus_read_coils_rsp(unsigned int8 address, unsigned int8 byte_count, unsigned int8* coil_data)
{
   unsigned int8 i;

   modbus_serial_send_start(address, FUNC_READ_COILS);

   modbus_serial_putc(byte_count);

   for(i=0; i < byte_count; ++i)
   {
      modbus_serial_putc(*coil_data);
      coil_data++;
   }

   modbus_serial_send_stop();
}

/*
read_discrete_input_rsp
Input:     int8       address            Slave Address
           int8       byte_count         Number of bytes being sent
           int8*      input_data         Pointer to an array of data to send
Output:    void
*/
void modbus_read_discrete_input_rsp(unsigned int8 address, unsigned int8 byte_count,
                                    unsigned int8 *input_data)
{
   unsigned int8 i;

   modbus_serial_send_start(address, FUNC_READ_DISCRETE_INPUT);

   modbus_serial_putc(byte_count);

   for(i=0; i < byte_count; ++i)
   {
      modbus_serial_putc(*input_data);
      input_data++;
   }

   modbus_serial_send_stop();
}

/*
read_holding_registers_rsp
Input:     int8       address            Slave Address
           int8       byte_count         Number of bytes being sent
           int8*      reg_data           Pointer to an array of data to send
Output:    void
*/
void modbus_read_holding_registers_rsp(unsigned int8 address, unsigned int8 byte_count,
                                        unsigned int16 *reg_data)
{
   unsigned int8 i;

   modbus_serial_send_start(address, FUNC_READ_HOLDING_REGISTERS);

   modbus_serial_putc(byte_count);

   for(i=0; i < byte_count; i+=2)
   {
      modbus_serial_putc(make8(*reg_data,1));
      modbus_serial_putc(make8(*reg_data,0));
      reg_data++;
   }

   modbus_serial_send_stop();
}

/*
read_input_registers_rsp
Input:     int8       address            Slave Address
           int8       byte_count         Number of bytes being sent
           int8*      input_data         Pointer to an array of data to send
Output:    void
*/
void modbus_read_input_registers_rsp(unsigned int8 address, unsigned int8 byte_count,
                                        unsigned int16 *input_data)
{
   unsigned int8 i;

   modbus_serial_send_start(address, FUNC_READ_INPUT_REGISTERS);

   modbus_serial_putc(byte_count);

   for(i=0; i < byte_count; i+=2)
   {
      modbus_serial_putc(make8(*input_data,1));
      modbus_serial_putc(make8(*input_data,0));
      input_data++;
   }

   modbus_serial_send_stop();
}

/*
write_single_coil_rsp
Input:     int8       address            Slave Address
           int16      output_address     Echo of output address received
           int16      output_value       Echo of output value received
Output:    void
*/
void modbus_write_single_coil_rsp(unsigned int8 address, unsigned int16 output_address,
                                    unsigned int16 output_value)
{
   modbus_serial_send_start(address, FUNC_WRITE_SINGLE_COIL);

   modbus_serial_putc(make8(output_address,1));
   modbus_serial_putc(make8(output_address,0));

   modbus_serial_putc(make8(output_value,1));
   modbus_serial_putc(make8(output_value,0));

   modbus_serial_send_stop();
}

/*
write_single_register_rsp
Input:     int8       address            Slave Address
           int16      reg_address        Echo of register address received
           int16      reg_value          Echo of register value received
Output:    void
*/
void modbus_write_single_register_rsp(unsigned int8 address, unsigned int16 reg_address,
                                        unsigned int16 reg_value)
{
   modbus_serial_send_start(address, FUNC_WRITE_SINGLE_REGISTER);

   modbus_serial_putc(make8(reg_address,1));
   modbus_serial_putc(make8(reg_address,0));

   modbus_serial_putc(make8(reg_value,1));
   modbus_serial_putc(make8(reg_value,0));

   modbus_serial_send_stop();
}

/*
read_exception_status_rsp
Input:     int8       address            Slave Address
Output:    void
*/
void modbus_read_exception_status_rsp(unsigned int8 address, unsigned int8 data)
{
   modbus_serial_send_start(address, FUNC_READ_EXCEPTION_STATUS);
   modbus_serial_send_stop();
}

/*
diagnostics_rsp
Input:     int8       address            Slave Address
           int16      sub_func           Echo of sub function received
           int16      data               Echo of data received
Output:    void
*/
void modbus_diagnostics_rsp(unsigned int8 address, unsigned int16 sub_func, unsigned int16 data)
{
   modbus_serial_send_start(address, FUNC_DIAGNOSTICS);

   modbus_serial_putc(make8(sub_func,1));
   modbus_serial_putc(make8(sub_func,0));

   modbus_serial_putc(make8(data,1));
   modbus_serial_putc(make8(data,0));

   modbus_serial_send_stop();
}

/*
get_comm_event_counter_rsp
Input:     int8       address            Slave Address
           int16      status             Status, refer to MODBUS documentation
           int16      event_count        Count of events
Output:    void
*/
void modbus_get_comm_event_counter_rsp(unsigned int8 address, unsigned int16 status,
                                        unsigned int16 event_count)
{
   modbus_serial_send_start(address, FUNC_GET_COMM_EVENT_COUNTER);

   modbus_serial_putc(make8(status, 1));
   modbus_serial_putc(make8(status, 0));

   modbus_serial_putc(make8(event_count, 1));
   modbus_serial_putc(make8(event_count, 0));

   modbus_serial_send_stop();
}

/*
get_comm_event_counter_rsp
Input:     int8       address            Slave Address
           int16      status             Status, refer to MODBUS documentation
           int16      event_count        Count of events
           int16      message_count      Count of messages
           int8*      events             Pointer to event data
           int8       events_len         Length of event data in bytes
Output:    void
*/
void modbus_get_comm_event_log_rsp(unsigned int8 address, unsigned int16 status,
                                    unsigned int16 event_count, unsigned int16 message_count,
                                    unsigned int8 *events, unsigned int8 events_len)
{
   unsigned int8 i;

   modbus_serial_send_start(address, FUNC_GET_COMM_EVENT_LOG);

   modbus_serial_putc(events_len+6);

   modbus_serial_putc(make8(status, 1));
   modbus_serial_putc(make8(status, 0));

   modbus_serial_putc(make8(event_count, 1));
   modbus_serial_putc(make8(event_count, 0));

   modbus_serial_putc(make8(message_count, 1));
   modbus_serial_putc(make8(message_count, 0));

   for(i=0; i < events_len; ++i)
   {
      modbus_serial_putc(*events);
      events++;
   }

   modbus_serial_send_stop();
}

/*
write_multiple_coils_rsp
Input:     int8       address            Slave Address
           int16      start_address      Echo of address to start at
           int16      quantity           Echo of amount of coils written to
Output:    void
*/
void modbus_write_multiple_coils_rsp(unsigned int8 address, unsigned int16 start_address,
                                        unsigned int16 quantity)
{
   modbus_serial_send_start(address, FUNC_WRITE_MULTIPLE_COILS);

   modbus_serial_putc(make8(start_address,1));
   modbus_serial_putc(make8(start_address,0));

   modbus_serial_putc(make8(quantity,1));
   modbus_serial_putc(make8(quantity,0));

   modbus_serial_send_stop();
}

/*
write_multiple_registers_rsp
Input:     int8       address            Slave Address
           int16      start_address      Echo of address to start at
           int16      quantity           Echo of amount of registers written to
Output:    void
*/
void modbus_write_multiple_registers_rsp(unsigned int8 address, unsigned int16 start_address,
                                            unsigned int16 quantity)
{
   modbus_serial_send_start(address, FUNC_WRITE_MULTIPLE_REGISTERS);

   modbus_serial_putc(make8(start_address,1));
   modbus_serial_putc(make8(start_address,0));

   modbus_serial_putc(make8(quantity,1));
   modbus_serial_putc(make8(quantity,0));

   modbus_serial_send_stop();
}

/*
report_slave_id_rsp
Input:     int8       address            Slave Address
           int8       slave_id           Slave Address
           int8       run_status         Are we running?
           int8*      data               Pointer to an array holding the data
           int8       data_len           Length of data in bytes
Output:    void
*/
void modbus_report_slave_id_rsp(unsigned int8 address, unsigned int8 slave_id, unsigned int1 run_status,
                              unsigned int8 *data, unsigned int8 data_len)
{
   unsigned int8 i;

   modbus_serial_send_start(address, FUNC_REPORT_SLAVE_ID);

   modbus_serial_putc(data_len+2);
   modbus_serial_putc(slave_id);

   if(run_status)
    modbus_serial_putc(0xFF);
   else
    modbus_serial_putc(0x00);

   for(i=0; i < data_len; ++i)
   {
      modbus_serial_putc(*data);
      data++;
   }

   modbus_serial_send_stop();
}

/*
read_file_record_rsp
Input:     int8                     address            Slave Address
           int8                     byte_count         Number of bytes to send
           read_sub_request_rsp*    request            Structure holding record/data information
Output:    void
*/
void modbus_read_file_record_rsp(unsigned int8 address, unsigned int8 byte_count,
                                    modbus_read_sub_request_rsp *request)
{
   unsigned int8 i=0,j;

   modbus_serial_send_start(address, FUNC_READ_FILE_RECORD);

   modbus_serial_putc(byte_count);

   while(i < byte_count);
   {
      modbus_serial_putc(request->record_length);
      modbus_serial_putc(request->reference_type);

      for(j=0; (j < request->record_length); j+=2)
      {
         modbus_serial_putc(make8(request->data[j], 1));
         modbus_serial_putc(make8(request->data[j], 0));
      }

      i += (request->record_length)+1;
      request++;
   }

   modbus_serial_send_stop();
}

/*
write_file_record_rsp
Input:     int8                     address            Slave Address
           int8                     byte_count         Echo of number of bytes sent
           write_sub_request_rsp*   request            Echo of Structure holding record information
Output:    void
*/
void modbus_write_file_record_rsp(unsigned int8 address, unsigned int8 byte_count,
                                    modbus_write_sub_request_rsp *request)
{
   unsigned int8 i, j=0;

   modbus_serial_send_start(address, FUNC_WRITE_FILE_RECORD);

   modbus_serial_putc(byte_count);

   for(i=0; i < byte_count; i+=(7+(j*2)))
   {
      modbus_serial_putc(request->reference_type);
      modbus_serial_putc(make8(request->file_number, 1));
      modbus_serial_putc(make8(request->file_number, 0));
      modbus_serial_putc(make8(request->record_number, 1));
      modbus_serial_putc(make8(request->record_number, 0));
      modbus_serial_putc(make8(request->record_length, 1));
      modbus_serial_putc(make8(request->record_length, 0));

      for(j=0; (j < request->record_length); j+=2)
      {
         modbus_serial_putc(make8(request->data[j], 1));
         modbus_serial_putc(make8(request->data[j], 0));
      }
   }

   modbus_serial_send_stop();
}

/*
mask_write_register_rsp
Input:     int8        address            Slave Address
           int16       reference_address  Echo of reference address
           int16       AND_mask           Echo of AND mask
           int16       OR_mask            Echo or OR mask
Output:    void
*/
void modbus_mask_write_register_rsp(unsigned int8 address, unsigned int16 reference_address,
                           unsigned int16 AND_mask, unsigned int16 OR_mask)
{
   modbus_serial_send_start(address, FUNC_MASK_WRITE_REGISTER);

   modbus_serial_putc(make8(reference_address,1));
   modbus_serial_putc(make8(reference_address,0));

   modbus_serial_putc(make8(AND_mask,1));
   modbus_serial_putc(make8(AND_mask,0));

   modbus_serial_putc(make8(OR_mask, 1));
   modbus_serial_putc(make8(OR_mask, 0));

   modbus_serial_send_stop();
}

/*
read_write_multiple_registers_rsp
Input:     int8        address            Slave Address
           int16*      data               Pointer to an array of data
           int8        data_len           Length of data in bytes
Output:    void
*/
void modbus_read_write_multiple_registers_rsp(unsigned int8 address, unsigned int8 data_len,
                                                unsigned int16 *data)
{
   unsigned int8 i;

   modbus_serial_send_start(address, FUNC_READ_WRITE_MULTIPLE_REGISTERS);

   modbus_serial_putc(data_len*2);

   for(i=0; i < data_len*2; i+=2)
   {
      modbus_serial_putc(make8(data[i], 1));
      modbus_serial_putc(make8(data[i], 0));
   }

   modbus_serial_send_stop();
}

/*
read_FIFO_queue_rsp
Input:     int8        address            Slave Address
           int16       FIFO_len           Length of FIFO in bytes
           int16*      data               Pointer to an array of data
Output:    void
*/
void modbus_read_FIFO_queue_rsp(unsigned int8 address, unsigned int16 FIFO_len, unsigned int16 *data)
{
   unsigned int8 i;
   unsigned int16 byte_count;

   byte_count = ((FIFO_len*2)+2);

   modbus_serial_send_start(address, FUNC_READ_FIFO_QUEUE);

   modbus_serial_putc(make8(byte_count, 1));
   modbus_serial_putc(make8(byte_count, 0));

   modbus_serial_putc(make8(FIFO_len, 1));
   modbus_serial_putc(make8(FIFO_len, 0));

   for(i=0; i < FIFO_len; i+=2)
   {
      modbus_serial_putc(make8(data[i], 1));
      modbus_serial_putc(make8(data[i], 0));
   }

   modbus_serial_send_stop();
}

/*
read_FIFO_queue_rsp
Input:     int8        address            Slave Address
           int16       func               function to respond to
           exception   error              exception response to send
Output:    void
*/
void modbus_exception_rsp(unsigned int8 address, unsigned int16 func, exception error)
{
   modbus_serial_send_start(address, func|0x80);
   modbus_serial_putc(error);
   modbus_serial_send_stop();
}

#endif
#endif //MODBUS_APP_LAYER_C
