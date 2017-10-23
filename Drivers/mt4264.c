///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                            mt4264.h                               ////
////                                                                   ////
//// This Library provides definitions that allow a PIC processor to   ////
//// access a 64k dynamic memory device                                ////
////                                                                   ////
//// Device drivers developed on standard pin out                      ////
////                                                                   ////
////                       ________________                            ////
////                       |              |                            ////
////                   NC  |              | Vss                        ////
////                       |              | ___                        ////
////                   DI  |              | CAS                        ////
////                   __  |              |                            ////
////                   WE  |              | DO                         ////
////                  ___  |              |                            ////
////                  RAS  |              | A6                         ////
////                       |              |                            ////
////                   A0  |              | A3                         ////
////                       |              |                            ////
////                   A2  |              | A4                         ////
////                       |              |                            ////
////                   A1  |              | A5                         ////
////                       |              |                            ////
////                  Vcc  |              | A7                         ////
////                       |              |                            ////
////                       ________________                            ////
////                                                                   ////
////                                                                   ////
////  Functions:                                                       ////
////                                                                   ////
////     DynMemInit : initializes the dynamic memory for use           ////
////                                                                   ////
////     Refresh : interrupt service routine that refreshes the        ////
////               dynamic memory approcamatly onece every 4ms         ////
////                                                                   ////
////     BitRead : reads a bit of dynamic memory                       ////
////                                                                   ////
////     BitWrite : writes a bit to dynamic memory                     ////
////                                                                   ////
////     ByteRead : Reads a byte (8 bits) from dynamic memory          ////
////                                                                   ////
////     ByteWrite : Writes a byte (8 bits) to dynamic memory          ////
////                                                                   ////
////     WordRead : Reads a word (16 bits) from dynamic memory         ////
////                                                                   ////
////     WordWrite : Writes a word (16 bits) to dynamic memory         ////
////                                                                   ////
////     DWordRead : Reads a double word (32 bits) from dynamic        ////
////                 memory                                            ////
////                                                                   ////
////     DWordWrite : Writes a double word (32 bits) to dynamic        ////
////                  memory                                           ////
////                                                                   ////
////     FloatRead : Reads a float (32 bits) from dynamic memory       ////
////                                                                   ////
////     FloatWrite : Writes a float (32 bits) to dynamic memory       ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////     notes:                                                        ////
////                                                                   ////
////   - Dynamic memory, by nature, must be refreshed periodicaly.     ////
////     an interrupt on timer1 is used for this purpose.              ////
////                                                                   ////
////   - It's a good idea to have a 0.1uF capacitor between +5V        ////
////     and ground.  This helps to maintain the integrity of the      ////
////     data.                                                         ////
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

#include <mt4264.h>

// DynMemInit initialized the dynamic memory for reading and writting
void DynMemInit ( )
{
   int i;

   setup_timer_1(T1_INTERNAL | T1_DIV_BY_1);
   enable_interrupts(int_timer1);
   enable_interrupts(global);
   set_timer1(0xC800);

   // 8 cycles containing an RAS clock
   for(i=0;i<8;i++)
      BitRead(0,0);
}

// the timer will interrupt every 4ms inorder to refresh the
// memory
#int_timer1
void Refresh ( )
{
   unsigned int16 i;
   // to output, CAS must be high, then each row is output followed
   // by an RAS cycle
   output_high(RAS);
   output_high(CAS);
   for(i=0;i<0x100;i++)
   {
      addressByte(i);
      output_low(RAS);
      output_high(RAS);
   }
   // set the timer up so that it will role over in 4ms
   set_timer1(0xC800);
}


// BitRead will return the bit located at the address desired
//    rowAddress = the row number of the bit
//    colAddress = the column number of the bit
//
// Events needed to read one
//    1. output row address to the chip
//    2. latch row address by bringing RAS low
//    3. bring write enable high
//    4. output column address to the chip
//    5. latch row address by bringing CAS low
//    6. read data in
//    7. bring CAS high
//    8. bring RAS high
int1 BitRead ( int rowAddress, int colAddress )
{
   int1 data;

   disable_interrupts(global);
   disable_interrupts(int_timer0);

   output_high(RAS);
   output_high(CAS);

   addressByte(rowAddress);
   #asm nop #endasm
   output_low(RAS);

   output_high(WE);
   addressByte(colAddress);
   #asm nop #endasm
   output_low(CAS);

   data = input(D0);

   output_high(CAS);
   output_high(RAS);

   enable_interrupts(int_timer0);
   enable_interrupts(global);

   return data;
}

// BitWrite will write value ath the address desired
//    rowAddress = the row number of the bit
//    colAddress = the column number of the bit
//    value      = the value to write to the specified bit
//
// Events needed to write one bit
//    1. output row address to the chip
//    2. latch row addres by bringing RAS low
//    3. bring write enable low
//    4. write to input pin
//    5. output col address to the chip
//    6. bring CAS high
//    7. bring RAS high
void BitWrite ( int rowAddress, int colAddress, int1 value )
{
   disable_interrupts(global);
   disable_interrupts(int_timer0);

   output_high (RAS);
   output_high (CAS);

   addressByte(rowAddress);
   #asm nop #endasm
   output_low(RAS);
   output_low(WE);
   output_bit(DI,value);
   #asm nop #endasm
   addressByte(colAddress);
   #asm nop #endasm
   output_low(CAS);

   output_high(CAS);
   output_high(RAS);

   enable_interrupts(int_timer0);
   enable_interrupts(global);
}

// ByteRead will return an array of bytes starting at address
//    address    = starting address to read from, range from 0x0000 to 0xffff
//    returnByte = a poniter to an array that will hold the read bytes
//    bytes      = the number of bytes to be read into the array
void ByteRead ( int32 address, int8 * returnByte, int bytes )
{
   int16 rowAddress, colAddress;
   int32 bits;

   // determine row and col addresses
   rowAddress=address/0x20;
   colAddress=(address&0x0000001F)*8;

   // determine number of bits to shift in
   bits=bytes*8;

   while(bits>0) {
      // shift in one bit at a time
      shift_right(returnByte,1,BitRead(rowAddress,colAddress));

      // increment rowAddress and colAddress accordingly
      if(++colAddress==0x100) {
         rowAddress+=0x01;
         colAddress=0x00;
      }
      // increment returnByte accordingly
      if(--bits%8==0)
         returnByte++;
   }
}

// ByteWrite will write an array of bytes starting at address
//    address  = starting address to write too, range from 0x0000 to 0x1fff
//    dataByte = a pointer to an array that holds the bytes to be written
//    bytes    = the number of bytes to be written into the array
void ByteWrite ( int32 address, int8 * dataByte, int bytes )
{
   int   bufferByte;
   int16 rowAddress, colAddress;
   int32 bits;

   // determine row and col addresses
   rowAddress=address/0x20;
   colAddress=(address&0x0000001F)*8;

   // determine number of bits to shift in
   bits=bytes*8;

   // bufferByte will keep the actual data from being channged
   bufferByte=*dataByte;

   while(bits>0) {

      // shift out one bit at a time
      BitWrite(rowAddress,colAddress,shift_right(&bufferByte,1,0));

      // increment rowAddress and colAddress accordingly
      if(++colAddress==0x100) {
         rowAddress+=0x01;
         colAddress=0x00;
      }

      // increment dataByte accordingly
      if(--bits%8==0) {
         dataByte++;
         bufferByte=*dataByte;
      }
   }
}

// WordRead will return an array of words starting at address
//    address  = starting address to read from, range from 0x0000 to 0x0fff
//    dataWord = a pointer to an array that will hold the read words
//    words    = the number of words to be read into the array
void WordRead ( int32 address, int16 * returnWord, int words )
{
   int16 rowAddress, colAddress;
   int32 bits;

   // determine row and col addresses
   rowAddress=address/0x10;
   colAddress=(address&0x0000000F)*10;

   // determine number of bits to shift in
   bits=words*16;

   while(bits>0) {
      // shift in one bit at a time
      shift_right(returnWord,2,BitRead(rowAddress,colAddress));

      // increment rowAddress and colAddress accordingly
      if(++colAddress==0x100) {
         rowAddress+=0x01;
         colAddress=0x00;
      }
      // increment returnByte accordingly
      if(--bits%16==0)
         returnWord++;
   }
}


// WordWrite will write an array of words starting at address
//    address = starting address to write too, range from 0x0000 to 0x0fff
//    dataWord = a pointer to an array that holds the words to be written
//    words = the number of words to be written into the array.
void WordWrite ( int32 address, int16 * dataWord, int words )
{
   int16 bufferWord, rowAddress, colAddress;
   int32 bits;

   // determine row and col addresses
   rowAddress=address/0x10;
   colAddress=(address&0x0000000F)*10;

   // determine number of bits to shift in
   bits=words*16;

   // bufferByte will keep the actual data from being channged
   bufferWord=*dataWord;

   while(bits>0) {
      // shift out one bit at a time
      BitWrite(rowAddress,colAddress,shift_right(&bufferWord,2,0));

      // increment rowAddress and colAddress accordingly
      if(++colAddress==0x100) {
         rowAddress+=0x01;
         colAddress=0x00;
      }

      // increment dataByte accordingly
      if(--bits%16==0) {
         dataWord++;
         bufferWord=*dataWord;
      }
   }
}

// DWordRead will return an array of dwords starting at address
//    address = starting address to read from, range from 0x0000 to 0x07ff
//    datadWord = a pointer to n array that will hold the read dwords
//    words = the number of dwords to be read into the array
void DWordRead ( int32 address, int32 * returndWord, int dwords )
{
   int16 rowAddress, colAddress;
   int32 bits;

   // determine row and col addresses
   rowAddress=address%8*20;
   colAddress=address/8;

   // determine number of bits to shift in
   bits=dwords*32;

   while(bits>0) {
      // shift in one bit at a time
      shift_right(returndWord,4,BitRead(rowAddress,colAddress));

      // increment rowAddress and colAddress accordingly
      if(++colAddress==0x100) {
         rowAddress+=0x10;
         colAddress=0x00;
      }
      // increment returnByte accordingly
      if(--bits%32==0)
         returndWord++;
   }
}

// DWordWrite will write an array of dwords starting at address
//    address = starting adderss to read from, range from 0x0000 to 0x07ff
//    datadWord = a pointer to an array that holds the dwords to be written
//    dwords = the number of dwords to be read into the array
void DWordWrite ( int32 address, int32 * datadWord, int dwords )
{
   int16 rowAddress, colAddress;
   int32 bufferDWord;
   int32 bits;

   // determine row and col addresses
   rowAddress=address%8*20;
   colAddress=address/8;

   // determine number of bits to shift in
   bits=dwords*32;

   // bufferByte will keep the actual data from being channged
   bufferDWord=*datadWord;

   while(bits>0) {
      // shift out one bit at a time
      BitWrite(rowAddress,colAddress,shift_right(&bufferDWord,4,0));

      // increment rowAddress and colAddress accordingly
      if(++colAddress==0x100) {
         rowAddress+=0x01;
         colAddress=0x00;
      }

      // increment dataByte accordingly
      if(--bits%32==0) {
         datadWord++;
         bufferDWord=*datadWord;
      }
   }
}

// FloatRead will return an array of floats starting at address
//    address = starting address to read from, range from 0x0000 to 0x07ff
//    datafloat = a pointer to an array that will hold the read floats
//    floats = the number of floats to be read into the array
void FloatRead ( int32 address, float * returnFloat, int floats )
{
   int16 rowAddress, colAddress;
   int32 bits;

   // determine row and col addresses
   rowAddress=address%8*20;
   colAddress=address/8;

   // determine number of bits to shift in
   bits=floats*32;

   while(bits>0) {
      // shift in one bit at a time
      shift_right(returnFloat,4,BitRead(rowAddress,colAddress));

      // increment rowAddress and colAddress accordingly
      if(++colAddress==0x100) {
         rowAddress+=0x10;
         colAddress=0x00;
      }
      // increment returnByte accordingly
      if(--bits%32==0)
         returnFloat++;
   }
}

// FloatWrite will write an array of floats starting at address
//    address = starting adderss to read from, range from 0x0000 to 0x07ff
//    dataFloat = a pointer to an array that holds the floats to be written
//    floats = the number of floats to be read into the array
void FloatWrite ( int32 address, float * dataFloat, int floats )
{
   int16 rowAddress, colAddress;
   float bufferFloat;
   int32 bits;

   // determine row and col addresses
   rowAddress=address%8*20;
   colAddress=address/8;

   // determine number of bits to shift in
   bits=floats*32;

   // bufferByte will keep the actual data from being channged
   bufferFloat=*dataFloat;

   while(bits>0) {
      // shift out one bit at a time
      BitWrite(rowAddress,colAddress,shift_right(&bufferFloat,4,0));

      // increment rowAddress and colAddress accordingly
      if(++colAddress==0x100) {
         rowAddress+=0x01;
         colAddress=0x00;
      }

      // increment dataByte accordingly
      if(--bits%32==0) {
         dataFloat++;
         bufferFloat=*dataFloat;
      }
   }
}

typemod <, ByteRead, ByteWrite, 0x0000, 0x1fff> dynMem;
