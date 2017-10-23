////////////////////////////////////////////////////////////////////////////////
////                          internal_eeprom.c                             ////
////                                                                        ////
////       Utilities to write various data types to internal eeprom         ////
////////////////////////////////////////////////////////////////////////////////
////                                                                        ////
////   void write_int1_eeprom(address, int8 bitPosition, int1 data)         ////
////     Call to write one bit of data                                      ////
////                                                                        ////
////   int1 read_int1_eeprom(address, int8 bitPosition)                     ////
////     Call to read one bit of data                                       ////
////                                                                        ////
////                                                                        ////
////   void write_int16_eeprom(address, int16 data)                         ////
////     Call to write a 16 bit integer                                     ////
////                                                                        ////
////   void write_int16_eeprom(address, int16 data)                         ////
////     Call to read a 16 bit integer                                      ////
////                                                                        ////
////                                                                        ////
////   void write_int32_eeprom(address, int32 data)                         ////
////     Call to write a 32 bit integer                                     ////
////                                                                        ////
////   int16 read_int32_eeprom(address)                                     ////
////     Call to read a 32 bit integer                                      ////
////                                                                        ////
////                                                                        ////
////   void write_float_eeprom(address, float data)                         ////
////     Call to write a 32 bit floating point number                       ////
////                                                                        ////
////   float read_float_eeprom(address)                                     ////
////     Call to read a 32 bit floating point number                        ////
////                                                                        ////
////  PCD only:                                                             ////
////   void write_int48_eeprom(address, int48 data)                         ////
////     Call to write a 48 bit integer                                     ////
////                                                                        ////
////   int48 read_int48_eeprom(address)                                     ////
////     Call to read a 48 bit integer                                      ////
////                                                                        ////
////   void write_int64_eeprom(address, int64 data)                         ////
////     Call to write a 64 bit integer                                     ////
////                                                                        ////
////   int64 read_int64_eeprom(address)                                     ////
////     Call to read a 64 bit integer                                      ////
////                                                                        ////
////   void write_float48_eeprom(address, float48 data)                     ////
////     Call to write a 48 bit floating point number                       ////
////                                                                        ////
////   float48 read_float48_eeprom(address)                                 ////
////     Call to read a 48 bit floating point number                        ////
////                                                                        ////
////   void write_float64_eeprom(address, float64 data)                     ////
////     Call to write a 64 bit floating point number                       ////
////                                                                        ////
////   float64 read_float64_eeprom(address)                                 ////
////     Call to read a 64 bit floating point number                        ////
////                                                                        ////
////////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2012 Custom Computer Services               ////
//// This source code may only be used by licensed users of the CCS C       ////
//// compiler.  This source code may only be distributed to other licensed  ////
//// users of the CCS C compiler.  No other use, reproduction or            ////
//// distribution is permitted without written permission. Derivative       ////
//// programs created using this software in object code form are not       ////
//// restricted in any way.                                                 ////
////////////////////////////////////////////////////////////////////////////////


#ifndef INTERNAL_EEPROM_UTILITIES
#define INTERNAL_EEPROM_UTILITIES

// Used to adjust the address range
#ifndef INT_EEPROM_ADDRESS
#if getenv("DATA_EEPROM") > 255
#define INT_EEPROM_ADDRESS  unsigned int16
#else
#define INT_EEPROM_ADDRESS  unsigned int8
#endif
#endif

#ifndef INT_EEPROM_DATA_SIZE
#ifdef (__PCD__)
#define INT_EEPROM_DATA_SIZE unsigned int16
#else
#define INT_EEPROM_DATA_SIZE unsigned int8
#endif

////////////////////////////////////////////////////////////////////////////////
//// Internal EEPROM Functions
////////////////////////////////////////////////////////////////////////////////

// Purpose:    Write one bit to internal eeprom
// Inputs:     1) An eeprom address
//             2) The bit position (LSB == 0)
//             3) The bit to write
// Outputs:    None
void write_int1_eeprom(INT_EEPROM_ADDRESS address, unsigned int8 bitPosition, int1 data)
{
   INT_EEPROM_DATA_SIZE stored_data;

   stored_data = read_eeprom(address);

   if(data)
   {
      bit_set(stored_data, bitPosition);
   }
   else
   {
      bit_clear(stored_data, bitPosition);
   }

   write_eeprom(address, stored_data);
}


// Purpose:    Read one bit from internal eeprom
// Inputs:     1) An eeprom address
//             2) The bit position (LSB == 0)
// Outputs:    The bit read from internal eeprom
int1 read_int1_eeprom(INT_EEPROM_ADDRESS address, unsigned int8 bitPosition)
{
   return bit_test(read_eeprom(address), bitPosition);
}


// Purpose:    Write a 16 bit number to internal eeprom
// Inputs:     1) An eeprom address. Two eeprom locations will be used.
//             2) The 16 bit number to write to internal eeprom
// Outputs:    None
void write_int16_eeprom(INT_EEPROM_ADDRESS address, unsigned int16 data)
{
   #ifdef __PCD__
   write_eeprom(address,data);
   #else
   int8 i;

   for(i = 0; i < 2; ++i)
   {
     write_eeprom(address + i, *((int8 *)(&data) + i));
   }
   #endif
}


// Purpose:    Read a 16 bit number from internal eeprom
// Inputs:     An eeprom address
// Outputs:    The 16 bit number read from internal eeprom
unsigned int16 read_int16_eeprom(INT_EEPROM_ADDRESS address)
{
   #ifdef __PCD__
   return(read_eeprom(address));
   #else
   int8  i;
   int16 data;

   for(i = 0; i < 2; ++i)
   {
     *((int8 *)(&data) + i) = read_eeprom(address + i);
   }

   return(data);
   #endif
}


// Purpose:    Write a 32 bit integer to internal eeprom
// Inputs:     1) An eeprom address. Four eeprom locations will be used.
//             2) The 32 bit number to write to internal eeprom
// Outputs:    None
void write_int32_eeprom(INT_EEPROM_ADDRESS address, unsigned int32 data)
{
   #ifdef __PCD__
   write_eeprom(address, &data, 4);
   #else
   int8 i;
   
   for(i = 0; i < 4; i++)
     write_eeprom(address + i, *((int8 *)(&data) + i));
   #endif
}


// Purpose:    Read a 32 bit integer from internal eeprom
// Inputs:     An eeprom address
// Outputs:    The 32 bit integer read from internal eeprom
unsigned int32 read_int32_eeprom(INT_EEPROM_ADDRESS address)
{
   #ifdef __PCD__
   return(read_eeprom(address, 4));
   #else
   int8  i;
   int32 data;
   
   for(i = 0; i < 4; i++)
     *((int8 *)(&data) + i) = read_eeprom(address + i);

   return(data);
   #endif
}


// Purpose:    Write a 32 bit floating point number to internal eeprom
// Inputs:     1) An eeprom address. Four eeprom locations will be used.
//             2) The floating point number to write to internal eeprom
// Outputs:    None
void write_float_eeprom(INT_EEPROM_ADDRESS address, float32 data)
{
   #ifdef __PCD__
   write_eeprom(address, &data, 4);
   #else
   int8 i;
   
   for(i = 0; i < 4; i++)
     write_eeprom(address + i, *((int8 *)(&data) + i));
   #endif
}


// Purpose:    Read a 32 bit floating point number from internal eeprom
// Inputs:     An eeprom address
// Outputs:    The floating point number read from the internal eeprom
float32 read_float_eeprom(INT_EEPROM_ADDRESS address)
{
   float32 data;

   #ifdef __PCD__
   read_eeprom(address, &data, 4);
   
   return(data);
   #else
   int8 i;
   
   for(i = 0; i < 4; i++)
     *((int8 *)(&data) + i) = read_eeprom(address + i);

   return(data);
   #endif
}

#ifdef __PCD__
// Purpose:    Write a 48 bit integer to internal eeprom
// Inputs:     1) An eeprom address. Six eeprom locations will be used.
//             2) The 48 bit number to write to internal eeprom
// Outputs:    None
void write_int48_eeprom(INT_EEPROM_ADDRESS address, unsigned int48 data)
{
   write_eeprom(address, &data, 6);
}


// Purpose:    Read a 48 bit integer from internal eeprom
// Inputs:     An eeprom address
// Outputs:    The 48 bit integer read from internal eeprom
unsigned int48 read_int48_eeprom(INT_EEPROM_ADDRESS address)
{
   return(read_eeprom(address, 6));
}


// Purpose:    Write a 64 bit integer to internal eeprom
// Inputs:     1) An eeprom address. Eight eeprom locations will be used.
//             2) The 64 bit number to write to internal eeprom
// Outputs:    None
void write_int64_eeprom(INT_EEPROM_ADDRESS address, unsigned int64 data)
{
   write_eeprom(address, &data, 8);
}


// Purpose:    Read a 64 bit integer from internal eeprom
// Inputs:     An eeprom address
// Outputs:    The 64 bit integer read from internal eeprom
unsigned int64 read_int64_eeprom(INT_EEPROM_ADDRESS address)
{
   return(read_eeprom(address, 8));
}


// Purpose:    Write a 48 bit floating point number to internal eeprom
// Inputs:     1) An eeprom address. Six eeprom locations will be used.
//             2) The floating point number to write to internal eeprom
// Outputs:    None
void write_float48_eeprom(INT_EEPROM_ADDRESS address, float48 data)
{
   write_eeprom(address, &data, 6);
}


// Purpose:    Read a 48 bit floating point number from internal eeprom
// Inputs:     An eeprom address
// Outputs:    The floating point number read from the internal eeprom
float48 read_float48_eeprom(INT_EEPROM_ADDRESS address)
{
   float48 data;

   read_eeprom(address, &data, 6);
   
   return(data);
}


// Purpose:    Write a 64 bit floating point number to internal eeprom
// Inputs:     1) An eeprom address. Eight eeprom locations will be used.
//             2) The floating point number to write to internal eeprom
// Outputs:    None
void write_float64_eeprom(INT_EEPROM_ADDRESS address, float64 data)
{
   write_eeprom(address, &data, 8);
}


// Purpose:    Read a 64 bit floating point number from internal eeprom
// Inputs:     An eeprom address
// Outputs:    The floating point number read from the internal eeprom
float64 read_float64_eeprom(INT_EEPROM_ADDRESS address)
{
   float64 data;

   read_eeprom(address, &data, 8);
   
   return(data);
}

#endif
   

#endif
