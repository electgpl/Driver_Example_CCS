///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                             d41256.h                              ////
////                                                                   ////
//// This Library provides definitions that allow a PIC processor to   ////
//// access a 256k dynamic memory device                               ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2003 Custom Computer Services          ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

//// addressing is performed over port b
#define addressByte(x) output_b(x)

//// DI, WE, RAS, CAS, D0, and A9 will be covered by port c
#define DI  PIN_C0
#define WE  PIN_C1
#define RAS PIN_C2
#define CAS PIN_C3
#define D0  PIN_C4
#define A9  PIN_C5

// the timer will interrupt every 4ms inorder to refresh the
// memory
#int_timer1
void Refresh ( );

// DynMem64Init initialized the dynamic memory for reading and writting
void DynMemInit ( );

// BitRead will return the bit located at the address desired
//    rowAddress = the row number of the bit
//    colAddress = the column number of the bit
int1 BitRead ( int rowAddress, int colAddress );

// BitWrite will write value ath the address desired
//    rowAddress = the row number of the bit
//    colAddress = the column number of the bit
//    value      = the value to write to the specified bit
void BitWrite ( int rowAddress, int colAddress, int1 value );

// ByteRead will return an array of bytes starting at address
//    address    = starting address to read from, range from 0x0000 to 0x1fff
//    returnByte = a poniter to an array that will hold the read bytes
//    bytes      = the number of bytes to be read into the array
void ByteRead ( int32 address, int8 * returnByte, int bytes );

// ByteWrite will write an array of bytes starting at address
//    address  = starting address to write too, range from 0x0000 to 0x1fff
//    dataByte = a pointer to an array that holds the bytes to be written
//    bytes    = the number of bytes to be written into the array
void ByteWrite ( int32 address, int8 * dataByte, int bytes );

// WordRead will return an array of words starting at address
//    address  = starting address to read from, range from 0x0000 to 0x0fff
//    dataWord = a pointer to an array that will hold the read words
//    words    = the number of words to be read into the array
void WordRead ( int32 address, int16 * returnWord, int words );

// WordWrite will write an array of words starting at address
//    address = starting address to write too, range from 0x0000 to 0x0fff
//    dataWord = a pointer to an array that holds the words to be written
//    words = the number of words to be written into the array.
void WordWrite ( int32 address, int16 * dataWord, int words );

// DWordRead will return an array of dwords starting at address
//    address = starting address to read from, range from 0x0000 to 0x07ff
//    datadWord = a pointer to n array that will hold the read dwords
//    words = the number of dwords to be read into the array
void DWordRead ( int32 address, int32 * returndWord, int dwords );

// DWordWrite will write an array of dwords starting at address
//    address = starting adderss to read from, range from 0x0000 to 0x07ff
//    datadWord = a pointer to an array that holds the dwords to be written
//    dwords = the number of dwords to be read into the array
void DWordWrite ( int32 address, int32 * datadWord, int dwords );

// FloatRead will return an array of floats starting at address
//    address = starting address to read from, range from 0x0000 to 0x07ff
//    datafloat = a pointer to an array that will hold the read floats
//    floats = the number of floats to be read into the array
void FloatRead ( int32 address, float * returnFloat, int floats );

// FloatWrite will write an array of floats starting at address
//    address = starting adderss to read from, range from 0x0000 to 0x07ff
//    dataFloat = a pointer to an array that holds the floats to be written
//    floats = the number of floats to be read into the array
void FloatWrite ( int32 address, float * dataFloat, int floats );
