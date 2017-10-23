////////////////////////////////////////////////////////////////////////////////
////                          external_eeprom.c                             ////
////                                                                        ////
////       Utilities to write various data types to external eeprom         ////
////////////////////////////////////////////////////////////////////////////////
////                                                                        ////
////   void write_int1_ext_eeprom(address, int8 bitPosition, int1 data)     ////
////     Call to write one bit of data                                      ////
////                                                                        ////
////   int1 read_int1_ext_eeprom(address, int8 bitPosition)                 ////
////     Call to read one bit of data                                       ////
////                                                                        ////
////                                                                        ////
////   void write_int16_ext_eeprom(address, int16 data)                     ////
////     Call to write a 16 bit integer                                     ////
////                                                                        ////
////   int16 read_int16_ext_eeprom(address)                                 ////
////     Call to read a 16 bit integer                                      ////
////                                                                        ////
////                                                                        ////
////   void write_int32_ext_eeprom(address, int32 data)                     ////
////     Call to write a 32 bit integer                                     ////
////                                                                        ////
////   int32 read_int32_ext_eeprom(address)                                 ////
////     Call to read a 32 bit integer                                      ////
////                                                                        ////
////                                                                        ////
////   void write_float_ext_eeprom(address, float data)                     ////
////     Call to write a floating point number                              ////
////                                                                        ////
////   float read_float_ext_eeprom(address)                                 ////
////     Call to read a floating point number                               ////
////                                                                        ////
////////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2004 Custom Computer Services               ////
//// This source code may only be used by licensed users of the CCS C       ////
//// compiler.  This source code may only be distributed to other licensed  ////
//// users of the CCS C compiler.  No other use, reproduction or            ////
//// distribution is permitted without written permission. Derivative       ////
//// programs created using this software in object code form are not       ////
//// restricted in any way.                                                 ////
////////////////////////////////////////////////////////////////////////////////


#ifndef EXTERNAL_EEPROM_UTILITIES
#define EXTERNAL_EEPROM_UTILITIES

// Used to adjust the address range
#ifndef EEPROM_ADDRESS
#define EEPROM_ADDRESS  int8
#endif

// Purpose:    Write one bit to external eeprom
// Inputs:     1) An eeprom address
//             2) The bit position (LSB == 0)
//             3) The bit to write
// Outputs:    None
void write_int1_ext_eeprom(EEPROM_ADDRESS address, int8 bitPosition, int1 data)
{
   int8 stored_data;

   stored_data = read_ext_eeprom(address);

   if(data)
   {
      bit_set(stored_data, bitPosition);
   }
   else
   {
      bit_clear(stored_data, bitPosition);
   }

   write_ext_eeprom(address, stored_data);
}


// Purpose:    Read one bit from external eeprom
// Inputs:     1) An eeprom address
//             2) The bit position (LSB == 0)
// Outputs:    The bit read from external eeprom
int1 read_int1_ext_eeprom(EEPROM_ADDRESS address, int8 bitPosition)
{
   return bit_test(read_ext_eeprom(address), bitPosition);
}


// Purpose:    Write a 16 bit number to external eeprom
// Inputs:     1) An eeprom address
//             2) The 16 bit number to write to external eeprom
// Outputs:    None
void write_int16_ext_eeprom(EEPROM_ADDRESS address, int16 data)
{
   int8 i;
   for(i = 0; i < 2; ++i)
   {
     write_ext_eeprom(address + i, *((int8 *)(&data) + i));
     
   }
}


// Purpose:    Read a 16 bit number from external eeprom
// Inputs:     An eeprom address
// Outputs:    The 16 bit number read from external eeprom
int16 read_int16_ext_eeprom(EEPROM_ADDRESS address)
{
   int8  i;
   int16 data;

   for(i = 0; i < 2; ++i)
   {
      *((int8 *)(&data) + i) = read_ext_eeprom(address + i);
   }

   return(data);
}


// Purpose:    Write a 32 bit integer to external eeprom
// Inputs:     1) An eeprom address
//             2) The 32 bit number to write to external eeprom
// Outputs:    None
void write_int32_ext_eeprom(EEPROM_ADDRESS address, int32 data)
{
   int8 i;

   for(i = 0; i < 4; ++i)
   {
     write_ext_eeprom(address + i, *((int8 *)(&data) + i));
   }
}


// Purpose:    Read a 32 bit integer from external eeprom
// Inputs:     An eeprom address
// Outputs:    The 32 bit integer read from external eeprom
int32 read_int32_ext_eeprom(EEPROM_ADDRESS address)
{
   int8  i;
   int32 data;

   for(i = 0; i < 4; ++i)
   {
      *((int8 *)(&data) + i) = read_ext_eeprom(address + i);
   }

   return data;
}


// Purpose:    Write a floating point number to external eeprom
// Inputs:     1) An eeprom address. Four eeprom locations will be used.
//             2) The floating point number to write to external eeprom
// Outputs:    None
void write_float_ext_eeprom(EEPROM_ADDRESS address, float data)
{
   int8 i;

   for(i = 0; i < 4; ++i)
   {
     write_ext_eeprom(address + i, *((int8 *)(&data) + i));
   }
}


// Purpose:    Read a floating point number from external eeprom
// Inputs:     An eeprom address
// Outputs:    The floating point number read from the external eeprom
float read_float_ext_eeprom(EEPROM_ADDRESS address)
{
   int8 i;
   float data;

   for(i = 0; i < 4; ++i)
   {
     *((int8 *)(&data) + i) = read_ext_eeprom(address + i);
   }

   return data;
}

#endif
