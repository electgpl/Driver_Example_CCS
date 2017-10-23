//////////////////////////////////////////////////////////////////////////////////////////
////                            modbus_phy_layer_tcpip.c                              ////
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

#ifndef MODBUS_PHY_LAYER_TCPIP_C
#define MODBUS_PHY_LAYER_TCPIP_C

#include <modbus_phy_layer_tcpip.h>

#if (MODBUS_TYPE == MODBUS_TYPE_CLIENT)
/*
ModbusLoadTxBufferStart
Purpose: To load the MPAB Header and function code into the TX Buffer
Input:   function    func     The function code to send with message
         int8        address  The Unit Identifier to send with message
         int16       length   The number of bytes of the message
Output:  void
*/
void ModbusLoadTxBufferStart(function func, unsigned int8 address,unsigned int16 length)
{
   modbus_tx.MBAPHeader.TransactionIdentifier++;
   modbus_tx.MBAPHeader.ProtocolIdentifier = 0;
   modbus_tx.MBAPHeader.Length = length;
   modbus_tx.MBAPHeader.UnitIdentifier = address;
   modbus_tx.func = func;
}

/*
modbus_putc
Purpose: To load a byte of data into the TX Buffer
Input:   int8        data     The byte of data to load into the buffer
         int1        new      If TRUE it resets the index and loads the data into index 0, If FALSE loads data into next index location
Output:  void
*/
void modbus_putc(unsigned int8 data,int1 new = FALSE)
{
   static int8 i;
   
   if(new)
      i = 0;
      
   modbus_tx.data[i++] = data;
}

#else
/*
ModbusLoadTxBufferStart
Purpose: To load the MPAB Header and function code into the TX Buffer
Input:   function    func        The function code to send with message
         MBAP_HEADER MBAPHeader  Contains the Transaction Identifier and Unit Identifier of MBAP Header to send with message
         int16       length      The number of bytes of the message
Output:  void
*/
void MobusLoadTxBufferStart(function func, MBAP_HEADER MBAPHeader, unsigned int16 length)
{
   modbus_tx[MBAPHeader.WhichSocket].MBAPHeader.TransactionIdentifier = MBAPHeader.TransactionIdentifier;
   modbus_tx[MBAPHeader.WhichSocket].MBAPHeader.ProtocolIdentifier = 0;
   modbus_tx[MBAPHeader.WhichSocket].MBAPHeader.Length = length;
   modbus_tx[MBAPHeader.WhichSocket].MBAPHeader.UnitIdentifier = MBAPHeader.UnitIdentifier;
   modbus_tx[MBAPHeader.WhichSocket].func = func;
}

/*
modbus_putc
Purpose: To load a byte of data into the TX Buffer
Input:   int8        which    Selects which TX Buffer to load data into
         int8        data     The byte of data to load into the buffer
         int1        new      If TRUE it resets the index and loads the data into index 0, If FALSE loads data into next index location
Output:  void
*/
void modbus_putc(unsigned int8 which, unsigned int8 data, int1 new = FALSE)
{
   static int8 i;
   
   if(new)
      i = 0;
      
   modbus_tx[which].data[i++] = data;
}

#endif

#endif
