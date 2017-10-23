/////////////////////// Driver to generate CRC //////////////////////////
////                                                                 ////
////  generate_8bit_crc(data, length, pattern)                       ////
////        Generates 8 bit crc from the data using the pattern.     ////
////                                                                 ////
////  generate_16bit_crc(data, length, pattern)                      ////
////        Generates 16 bit crc from the data using the pattern.    ////
////                                                                 ////
////  generate_32bit_crc(data, length, pattern)                      ////
////        Generates 32 bit crc from the data using the pattern.    ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#define CRC_16    0x8005      //bit pattern (1)1000 0000 0000 0101
#define CRC_CCITT 0x1021      //bit pattern (1)0001 0000 0010 0001
#define CRC_32    0x04C11DB7  //bit pattern (1)0000 0100 1100 0001 0001 1101 1011 0111


int generate_8bit_crc(char* data, int16 length, int pattern)
{
   int   *current_data;
   int   crc_byte;
   int16 byte_counter;
   int   bit_counter;

   current_data = data;
   crc_byte = *current_data++;

   for(byte_counter=0; byte_counter < (length-1); byte_counter++)
   {
      for(bit_counter=0; bit_counter < 8; bit_counter++)
      {
         if(!bit_test(crc_byte,7))
         {
            crc_byte <<= 1;
            bit_test(*current_data, 7 - bit_counter) ?
               bit_set(crc_byte,0) : bit_clear(crc_byte,0);
            continue;
         }
         crc_byte <<= 1;
         bit_test(*current_data, 7 - bit_counter) ?
            bit_set(crc_byte,0) : bit_clear(crc_byte,0);
         crc_byte ^= pattern;
      }
      current_data++;
   }
   for(bit_counter=0; bit_counter < 8; bit_counter++)
   {
      if(!bit_test(crc_byte,7))
      {
         crc_byte <<= 1;
         continue;
      }
      crc_byte <<= 1;
      crc_byte ^= pattern;
   }
   return crc_byte;
}


int16 generate_16bit_crc(char* data, int16 length, int16 pattern)
{
   int   *current_data;
   int16 crc_Dbyte;
   int16 byte_counter;
   int   bit_counter;

   current_data = data + 2;
   crc_Dbyte =  make16(data[0], data[1]);

   for(byte_counter=0; byte_counter < (length-2); byte_counter++)
   {
      for(bit_counter=0; bit_counter < 8; bit_counter++)
      {
         if(!bit_test(crc_Dbyte,15))
         {
            crc_Dbyte <<= 1;
            bit_test(*current_data, 7 - bit_counter) ?
               bit_set(crc_Dbyte,0) : bit_clear(crc_Dbyte,0);
            continue;
         }
         crc_Dbyte <<= 1;
         bit_test(*current_data, 7 - bit_counter) ?
            bit_set(crc_Dbyte,0) : bit_clear(crc_Dbyte,0);
         crc_Dbyte ^= pattern;
      }
      current_data++;
   }

   for(bit_counter=0; bit_counter < 16; bit_counter++)
   {
      if(!bit_test(crc_Dbyte,15))
      {
         crc_Dbyte <<= 1;
         continue;
      }
      crc_Dbyte <<= 1;
      crc_Dbyte ^= pattern;
   }

   return crc_Dbyte;
}

int32 generate_32bit_crc(char* data, int16 length, int32 pattern)
{
   int   *current_data;
   int32 crc_Dbyte;
   int16 byte_counter;
   int   bit_counter;

   current_data = data + 4;
   crc_Dbyte =  make32(data[0], data[1], data[2], data[3]);

   for(byte_counter=0; byte_counter < (length-4); byte_counter++)
   {
      for(bit_counter=0; bit_counter < 8; bit_counter++)
      {
         if(!bit_test(crc_Dbyte,31))
         {
         crc_Dbyte <<= 1;
         bit_test(*current_data, 7 - bit_counter) ?
            bit_set(crc_Dbyte,0) : bit_clear(crc_Dbyte,0);
            continue;
         }
         crc_Dbyte <<= 1;
         bit_test(*current_data, 7 - bit_counter) ?
            bit_set(crc_Dbyte,0) : bit_clear(crc_Dbyte,0);
         crc_Dbyte ^= pattern;
      }
      current_data++;
   }

   for(bit_counter=0; bit_counter < 32; bit_counter++)
   {
      if(!bit_test(crc_Dbyte,31))
      {
         crc_Dbyte <<= 1;
         continue;
      }
      crc_Dbyte <<= 1;
      crc_Dbyte ^= pattern;
   }

   return crc_Dbyte;
}
