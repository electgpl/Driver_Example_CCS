/////////////////////////////////////////////////////////////////////////
////                           EX_MCR.C                              ////
////                                                                 ////
//// This example program demonstrates the use of a magnetic card    ////
//// reader and MCR.C. The program calls mcr_read() and passes in    ////
//// pointers to two arrays. The mcr_read() will fill the arrays     ////
//// data read by the card reader. The data is then displayed and    ////
//// any errors are reported.                                        ////
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

#if defined(__PCM__)
#include <16F877.h>
#device *=16
#fuses HS,NOLVP,NOWDT,NOPROTECT
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCH__)
#include <18F452.h>
#fuses HS,NOLVP,NOWDT,NOPROTECT
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#endif

// Included for card read functionality
#include "mcr.c"

void main()
{
   int index;
   int error;
   char Track1[79];
   char Track2[40];

   while(TRUE)
   {
      printf("\r\n\nWaiting to read magnetic card...\r\n");

      // Read a card and get any errors
      error = mcr_read(Track1, Track2);

      // Display the data in track 1
      index=0;
      printf("Track1 Data:\r\n");

      // Loop until the end sentinel (?) is displayed.
      // This loop does not display the LRC character.
      do
      {
         // Display the character and increment the index
         putc(Track1[index++]);
      }while(Track1[index-1] != '?' && index<79);


      // Display the data in track 2
      index=0;
      printf("\r\nTrack2 Data:\r\n");

      // Loop until the end sentinel (?) is displayed.
      // This loop does not display the LRC character.
      do
      {
         // Display the character and increment the index
         putc(Track2[index++]);
      }while(Track2[index-1] != '?' && index<40);

      // Check for card reading errors
      if(error & MCR_ERR_PARITY1)
      {
         printf("\r\nTrack 1 parity error");
      }
      if(error & MCR_ERR_PARITY2)
      {
         printf("\r\nTrack 2 parity error");
      }
      if(error & MCR_ERR_LRC1)
      {
         printf("\r\nTrack 1 LRC error");
      }
      if(error & MCR_ERR_LRC2)
      {
         printf("\r\nTrack 2 LRC error");
      }
   }
}
