//////////////////////////////////////////////////////////////////////////////////////////
////                                modbus_phy_layer.h                                ////
////                                                                                  ////
////      Physical layer of the MODBUS protocol driver for serial communications.     ////
////                                                                                  ////
////  Refer to documentation at http://www.modbus.org for more information on MODBUS. ////
////                                                                                  ////
//////////////////////////////////////////////////////////////////////////////////////////
////                                                                                  ////
//// Revision history:                                                                ////
////  July 20, 2011       Seperated Physical Layer functions definitions into this    ////
////                      file from modbus.c                                          ////
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

#ifndef MODBUS_PHY_LAYER_H
#define MODBUS_PHY_LAYER_H

#if (MODBUS_TYPE == MODBUS_TYPE_MASTER)
#if (MODBUS_SERIAL_TYPE == MODBUS_RTU)
#define MODBUS_SERIAL_WAIT_FOR_RESPONSE()\
{\
   modbus_timeout_enabled = 0;\
   if(address)\
   {\
      while(!modbus_kbhit() && --modbus_serial_wait)\
         delay_us(1);\
      if(!modbus_serial_wait)\
         modbus_rx.error=TIMEOUT;\
   }\
   modbus_serial_wait = MODBUS_SERIAL_TIMEOUT;\
}
#else
#define MODBUS_SERIAL_WAIT_FOR_RESPONSE()\
{\
   if(address)\
   {\
      while(!modbus_kbhit() && --modbus_serial_wait)\
         delay_us(1);\
      if(!modbus_serial_wait)\
         modbus_rx.error=TIMEOUT;\
   }\
   modbus_serial_wait = MODBUS_SERIAL_TIMEOUT;\
}
#endif
#endif

#if (MODBUS_SERIAL_INT_SOURCE != MODBUS_INT_EXT)
   #if defined(__PCD__)
      #if (MODBUS_SERIAL_INT_SOURCE == MODBUS_INT_RDA)
         #word TXSTA=getenv("SFR:U1STA")
         #bit TRMT=TXSTA.8
      #elif (MODBUS_SERIAL_INT_SOURCE == MODBUS_INT_RDA2)
         #word TXSTA=getenv("SFR:U2STA")
         #bit TRMT=TXSTA.8
      #elif (MODBUS_SERIAL_INT_SOURCE == MODBUS_INT_RDA3)
         #word TXSTA=getenv("SFR:U3STA")
         #bit TRMT=TXSTA.8
      #elif (MODBUS_SERIAL_INT_SOURCE == MODBUS_INT_RDA4)
         #word TXSTA=getenv("SFR:U4STA")
         #bit TRMT=TXSTA.8
      #endif
   #else
      #if (MODBUS_SERIAL_INT_SOURCE == MODBUS_INT_RDA)
         #if getenv("sfr_valid:TXSTA")
            #byte TXSTA=getenv("sfr:TXSTA")
         #elif getenv("sfr_valid:TXSTA1")
            #byte TXSTA=getenv("sfr:TXSTA1")
         #else
            #byte TXSTA=getenv("sfr:TX1STA")
         #endif
      #elif (MODBUS_SERIAL_INT_SOURCE == MODBUS_INT_RDA2)
         #if getenv("sfr_valid:TXSTA2")
            #byte TXSTA=getenv("sfr:TXSTA2")
         #else
            #byte TXSTA=getenv("sfr:TX2STA")
         #endif
     #elif (MODBUS_SERIAL_INT_SOURCE == MODBUS_INT_RDA3)
         #if getenv("sfr_valid:TXSTA3")
            #byte TXSTA=getenv("sfr:TXSTA3")
         #else
            #byte TXSTA=getenv("sfr:TX3STA")
         #endif
     #elif (MODBUS_SERIAL_INT_SOURCE == MODBUS_INT_RDA4)
         #if getenv("sfr_valid:TXSTA4")
            #byte TXSTA=getenv("sfr:TXSTA4")
         #else
            #byte TXSTA=getenv("sfr:TX4STA")
         #endif
      #endif
      #bit TRMT=TXSTA.1
   #endif
#endif

#if (MODBUS_SERIAL_INT_SOURCE != MODBUS_INT_EXT)
#define WAIT_FOR_HW_BUFFER()\
{\
   while(!TRMT);\
}
#endif

int1 modbus_serial_new=0;

/********************************************************************
These exceptions are defined in the MODBUS protocol.  These can be
used by the slave to communicate problems with the transmission back
to the master who can also use these to easily check the exceptions.
The first exception is the only one that is not part of the protocol
specification.  The TIMEOUT exception is returned when no slave
responds to the master's request within the timeout period.
********************************************************************/
typedef enum _exception{ILLEGAL_FUNCTION=1,ILLEGAL_DATA_ADDRESS=2,
ILLEGAL_DATA_VALUE=3,SLAVE_DEVICE_FAILURE=4,ACKNOWLEDGE=5,SLAVE_DEVICE_BUSY=6,
MEMORY_PARITY_ERROR=8,GATEWAY_PATH_UNAVAILABLE=10,GATEWAY_TARGET_NO_RESPONSE=11,
TIMEOUT=12} exception;

/********************************************************************
These functions are defined in the MODBUS protocol.  These can be
used by the slave to check the incomming function.  See
ex_modbus_slave.c for example usage.
********************************************************************/
typedef enum _function{FUNC_READ_COILS=0x01,FUNC_READ_DISCRETE_INPUT=0x02,
FUNC_READ_HOLDING_REGISTERS=0x03,FUNC_READ_INPUT_REGISTERS=0x04,
FUNC_WRITE_SINGLE_COIL=0x05,FUNC_WRITE_SINGLE_REGISTER=0x06,
FUNC_READ_EXCEPTION_STATUS=0x07,FUNC_DIAGNOSTICS=0x08,
FUNC_GET_COMM_EVENT_COUNTER=0x0B,FUNC_GET_COMM_EVENT_LOG=0x0C,
FUNC_WRITE_MULTIPLE_COILS=0x0F,FUNC_WRITE_MULTIPLE_REGISTERS=0x10,
FUNC_REPORT_SLAVE_ID=0x11,FUNC_READ_FILE_RECORD=0x14,
FUNC_WRITE_FILE_RECORD=0x15,FUNC_MASK_WRITE_REGISTER=0x16,
FUNC_READ_WRITE_MULTIPLE_REGISTERS=0x17,FUNC_READ_FIFO_QUEUE=0x18} function;

/********************************************************************
Our receive struct.  This is used when receiving data as a master or
slave.  Once a message is sent to you with your address, you should
begin processing that message.  Refer to ex_modbus_slave.c to see
how to properly use this structure.
********************************************************************/
struct
{
   unsigned int8 address;
   unsigned int8 len;                       //number of bytes in the message received
   function func;                           //the function of the message received
   exception error;                         //error recieved, if any
   unsigned int8 data[MODBUS_SERIAL_RX_BUFFER_SIZE]; //data of the message received
} modbus_rx;

//////////////////////////////////////////////////////////////////////////////////////////
////  For Custom Commands                                                             ////
////                                                                                  ////
////  modbus_serial_send_start(address,func)                                          ////
////    - Setup serial line to begin sending.  Once this is called, you can send data ////
////      using modbus_serial_putc().  Should only be used for custom commands.       ////
////                                                                                  ////
////  modbus_serial_send_stop()                                                       ////
////    - Must be called to finalize the send when modbus_serial_send_start is used.  ////
////                                                                                  ////
////  modbus_serial_putc(unsigned int8 c)                                             ////
////    - Sends a character onto the serial line                                      ////
////                                                                                  ////
//////////////////////////////////////////////////////////////////////////////////////////

// Purpose:    Send a message over the RS485 bus
// Inputs:     1) The destination address
//             2) The number of bytes of data to send
//             3) A pointer to the data to send
//             4) The length of the data
// Outputs:    TRUE if successful
//             FALSE if failed
// Note:       Format:  source | destination | data-length | data | checksum
void modbus_serial_send_start(unsigned int8 to, unsigned int8 func);

// Purpose:    Sends a message stop
// Inputs:     none
// Outputs:    None
void modbus_serial_send_stop();

// Purpose:    Puts a character onto the serial line
// Inputs:     Character
// Outputs:    None
void modbus_serial_putc(unsigned int8 c);

//////////////////////////////////////////////////////////////////////////////////////////
////  For Init                                                                        ////
//////////////////////////////////////////////////////////////////////////////////////////

// Purpose:    Enable data reception
// Inputs:     None
// Outputs:    None
void RCV_ON(void);

#endif //MODBUS_PHY_LAYER_H
