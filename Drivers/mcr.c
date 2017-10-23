/////////////////////////////////////////////////////////////////////////
////                           MCR.C                                 ////
////                                                                 ////
////  This file contains code to read tracks one and two from a      ////
////  magnetic card.                                                 ////
////                                                                 ////
////  int mcr_read(char* track1, char* track2)                       ////
////     * Call this function to read a card. It will wait until a   ////
////       card is present, then will read and decode the data.      ////
////       Decoding and parity checks are done while reading. LRC is ////
////       checked after the card is read. Only reading in one       ////
////       direction is supported.                                   ////
////     * Parameter notes                                           ////
////        - track1 should be 79 characters                         ////
////        - track2 should be 40 characters                         ////
////        - Pass a NULL pointer to not read a track                ////
////     * Returns an error code                                     ////
////        - 1: Parity error on track 1                             ////
////        - 2: Parity error on track 2                             ////
////        - 4: LRC error on track 1                                ////
////        - 8: LRC error on track 2                                ////
////       Multiple errors could occur, so check the cooresponding   ////
////       bit for a particular error.                               ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////  Pin connections for the OMRON V3A-6                            ////
////  1:       GND                                                   ////
////  2 & 3:   +5V                                                   ////
////  4 & 7:   MCR_CARD_PRESENT                                      ////
////  5:       MCR_STROBE2                                           ////
////  6:       MCR_DATA2                                             ////
////  8:       MCR_STROBE1                                           ////
////  9:       MCR_DATA1                                             ////
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


// Redefine the following pins in
// the main file as needed.
#ifndef MCR_CARD_PRESENT
#define MCR_CARD_PRESENT   PIN_B0
#define MCR_DATA1          PIN_B1
#define MCR_STROBE1        PIN_B2
#define MCR_DATA2          PIN_B4
#define MCR_STROBE2        PIN_B5
#endif

// The following define the error codes.
// To check for a specific error, check
// the cooresponding bit in the value
// returned by mcr_read().
#define MCR_ERR_PARITY1 1
#define MCR_ERR_PARITY2 2
#define MCR_ERR_LRC1    4
#define MCR_ERR_LRC2    8


// Date:       7/28/2003
int mcr_read(char* track1, char* track2)
{
   int   error    = 0;
   int1  dataBit  = 0;

   int1  ST1      = 1;
   int1  ST1_old  = 1;
   int1  firstOne1= 0;
   int1  parity1  = 0;
   int   count1   = 0;
   int   index1   = 0;
   int   LRC1     = 0;

   int1  ST2      = 1;
   int1  ST2_old  = 1;
   int1  firstOne2= 0;
   int1  parity2  = 0;
   int   count2   = 0;
   int   index2   = 0;
   int   LRC2     = 0;

   // Wait for card to be present
   while(input(MCR_CARD_PRESENT));

   // Loop until a card is not present
   while(!input(MCR_CARD_PRESENT))
   {
      // Check for NULL pointer and an index less than 79
      if(track1 != 0 && index1 < 79)
      {
         // Get strobe number one
         ST1 = input(MCR_STROBE1);

         // If the strobe was high and is now low (falling edge),
         // then data is present
         if(ST1 == 0 && ST1_old == 1)
         {
            ST1_old = 0;

            // Check if the first 1 was received
            if(firstOne1 == 1)
            {
               // Check if 6 bits of data were received
               if(count1 == 6)
               {
                  // Set the data bit count back to 0
                  count1 = 0;

                  // Shift the bits right by 2
                  *(track1+index1) >>= 2;

                  // Verify the LRC
                  if(*(track1+index1-1) == '?' && LRC1 != *(track1+index1))
                  {
                     error |= MCR_ERR_LRC1;
                  }
                  else
                  {
                     LRC1 ^= *(track1+index1);
                  }

                  // Add 0x20 to convert to ASCII
                  *(track1+index1) += 0x20;

                  // Check the parity bit. The parity on a space is not checked
                  // because of the trailing zeros after the LRC character
                  if(!input(MCR_DATA1) != parity1 && *(track1+index1) != ' ')
                  {
                     error |= MCR_ERR_PARITY1;
                  }

                  // Reset the parity check
                  parity1 = 1;

                  // Increment the index into the array
                  ++index1;
               }
               else
               {
                  // Get a bit of data from the card
                  dataBit = !input(MCR_DATA1);

                  // XOR the data with the parity bit
                  parity1 ^= dataBit;

                  // Store the new bit of data
                  shift_right(track1+index1, 1, dataBit);

                  // Increment the bit counter
                  ++count1;
               }
            }
            else
            {
               // Check if the first 1 has appeard on the data line
               if(!input(MCR_DATA1))
               {
                  // Set the first 1 received flag
                  firstOne1 = 1;

                  // Store the first 1
                  shift_right(track1, 1, 1);

                  // Increment the bit counter
                  ++count1;
               }
            }
         }
         else if(ST1 == 1)
         {
            ST1_old = 1;
         }
      }


      // Check for NULL pointer and an index less than 40
      if(track2 != 0 && index2 < 40)
      {
         // Get strobe number 2
         ST2 = input(MCR_STROBE2);

         // If the strobe was high and is now low (falling edge),
         // then data is present
         if(ST2 == 0 && ST2_old == 1)
         {
            ST2_old = 0;

            // Check if the first 1 was received
            if(firstOne2 == 1)
            {
               // Check if 4 bits of data were received
               if(count2 == 4)
               {
                  // Reset the bit counter back to 0
                  count2 = 0;

                  // Shift the bits right by 4
                  *(track2+index2) >>= 4;

                  // Verify the LRC
                  if(*(track2+index2-1) == '?' && LRC2 != *(track2+index2))
                  {
                     error |= MCR_ERR_LRC2;
                  }
                  else
                  {
                     LRC2 ^= *(track2+index2);
                  }
                  
                  // Add 0x30 to convert to ASCII
                  *(track2+index2) += 0x30;

                  // Check the parity bit. The parity on a 0 is not checked
                  // because of the trailing zeros after the LRC character
                  if(!input(MCR_DATA2) != parity2 && *(track2+index2) != '0')
                  {
                     error |= MCR_ERR_PARITY2;
                  }

                  // Reset the parity check
                  parity2 = 1;

                  // Increment the index into the array
                  ++index2;
               }
               else
               {
                  // Get a bit of data from the card
                  dataBit = !input(MCR_DATA2);

                  // XOR the data with the parity bit
                  parity2 ^= dataBit;

                  // Store the new bit of data
                  shift_right(track2+index2, 1, dataBit);

                  // Increment the bit counter
                  ++count2;
               }
            }
            else
            {
               // Check if the first 1 has appeard on the data line
               if(!input(MCR_DATA2))
               {
                  // Set the first 1 received flag
                  firstOne2 = 1;

                  // Store the first 1
                  shift_right(track2, 1, 1);

                  // Increment the bit counter
                  ++count2;
               }
            }
         }
         else if(ST2 == 1)
         {
            ST2_old = 1;
         }
      }
   }

   // Return any errors
   return error;
}
