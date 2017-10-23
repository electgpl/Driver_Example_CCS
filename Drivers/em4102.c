/////////////////////////////////////////////////////////////////////////
////                            EM4102.c                             ////
////                                                                 ////
//// This file contains drivers for a 4102 RF transponder            ////
////                                                                 ////
//// int1 read_4102(int8* data)                                      ////
////   - Call this funtion to read a 4102 transponder                ////
////   - Pass in a pointer to a 5 byte array                         ////
////   - The first byte will have the customer code and the last     ////
////     four bytes will contain the ID number                       ////
////                                                                 ////
////   - Returns FALSE if a parity check error occurred              ////
////   - Returns TRUE if a transponder was read successfully         ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2004 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////


#ifndef TRANSPONDER_4102_DRIVERS
#define TRANSPONDER_4102_DRIVERS

//#define UNIVERSAL_FORMAT

// Function Prototypes
int1 read_4102(int8* data);
int1 header_search_4102();
int1 decode_data_4102(int8* data);


// Purpose:       Reads the ID number and data number
// Inputs:        A pointer to a 5 byte array to fill
//                * The first byte will have the ID
//                * The last 4 bytes will have the data
// Outputs:       TRUE if read successful, FALSE if read failed
int1 read_4102(int8* data)
{
   int8 i;

   RF_get(sizeof(RFbuffer)*8, TRUE);    // Fill the buffer with data

   for(i=0; i<2; ++i)
   {
      while(!END_OF_RFBUFFER)
      {
         if(header_search_4102())       // Try to find 9 consecutive 1s
         {
            if(decode_data_4102(data))  // Try to decode the data after the header
            {
               RFbuffer_fill(0xAA);     // Prevents false detection
               
               #ifdef UNIVERSAL_FORMAT
                  i=data[1];
                  data[1]=data[3];
                  data[3]=i;
                  i=data[0];
                  data[0]=data[4];
                  data[4]=i;
               #endif
               
               return TRUE;             // Return sucessful read
            }
         }
      }
      RFbuffer_invert();                // Invert the buffer because the
   }                                    // Manchester encoded data could have
                                        // been read starting at the wrong edge

   RFbuffer_fill(0xAA);                 // Prevents false detection
   return FALSE;                        // Return error
}


// Purpose:       Search for the header consisting of 9 ones
// Inputs:        None
// Outputs:       TRUE if the header was found, FALSE if it was not found
int1 header_search_4102()
{
   int bitCounter = 0;

   // Loops until 9 consecutive 1s are found
   // or the end of the receive buffer is reached
   while(!END_OF_RFBUFFER)
   {
      if(RFbuffer_getBit() == 1)
      {
         if(++bitCounter == 9)
         {
            return TRUE;
         }
      }
      else
      {
         bitCounter = 0;
      }
   }

   return FALSE;
}


// Purpose:       Decodes the ID number and data number
// Inputs:        A pointer to a 5 byte array to fill
//                * The first byte will have the ID
//                * The last 4 bytes will have the data
// Outputs:       TRUE if read successful, FALSE if read failed
int1 decode_data_4102(int8* data)
{
   int1 bit = 0;
   int8 count = 1;
   int1 parity = 0;
   int1 colParity1 = 0;
   int1 colParity2 = 0;
   int1 colParity3 = 0;
   int1 colParity4 = 0;

   // Loop until 40 bits of data and 10 bits of parity are received
   for(count=1; count <= 50; ++count)
   {
      bit = RFbuffer_getBit();
      if(END_OF_RFBUFFER)
      {
         return FALSE;
      }

      if(count % 5 == 0)
      {
         // Check for row parity
         if(parity != bit)
         {
            return FALSE;
         }
         parity = 0;
      }
      else
      {
         // Store a bit of data
         #ifdef UNIVERSAL_FORMAT
         shift_left(data, 5, bit);
         #else 
         shift_right(data, 5, bit);
         #endif
         
         // Calculate row parity
         parity ^= bit;

         // Calculate column parity
         switch (count % 5)
         {
            case 1: colParity1 ^= bit; break;
            case 2: colParity2 ^= bit; break;
            case 3: colParity3 ^= bit; break;
            case 4: colParity4 ^= bit; break;
         }
      }
   }

   // Check for column parity
   if(colParity1 != RFbuffer_getBit() || colParity2 != RFbuffer_getBit() ||
      colParity3 != RFbuffer_getBit() || colParity4 != RFbuffer_getBit()   )
   {
      return FALSE;
   }

   // Check for stop bit
   if(RFbuffer_getBit() != 0)
   {
      return FALSE;
   }

   // Prevents reading all zeros for customer ID and tag ID
   for(count=0; count<5 && data[count] == 0; ++count);
   if(count == 5)
   {
      return FALSE;
   }

   // Return TRUE if no errors in decoding received transponder data
   return TRUE;
}

#endif
