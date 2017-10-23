/////////////////////////////////////////////////////////////////////////
////                         EX_TGETC2.C                             ////
////                                                                 ////
////  This program echoes the key presses of the user and times-     ////
////  out after a specified amount of time.  This uses the timeout   ////
////  mechanism that is internal to #use rs232(), for an example of  ////
////  how to write your own timeout method see ex_tgetc.c            ////
////                                                                 ////
////  When using #use rs232()'s timeout mechanism, if a character    ////
////  is not received within the specifed time the return value      ////
////  of getc() will be 0, and RS232_ERRORS will also be 0.          ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCB        pin A2 to RS232 RX, pin A3 to RS232 TX           ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////                                                                 ////
////  This example will work with the PCB, PCM and PCH compilers.    ////
////  The following conditional compilation lines are used to        ////
////  include a valid device for each compiler.  Change the device,  ////
////  clock and RS232 pins for your hardware if needed.              ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2007 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#define  KEYHIT_DELAY   500     // in milliseconds

///////////// end configuration /////////////////////////////////////////

#if defined(__PCB__)
#include <16C56.h>
#fuses HS,NOWDT,NOPROTECT
#use delay(clock=20M)
#use rs232(baud=9600, xmit=PIN_A3, rcv=PIN_A2, timeout=KEYHIT_DELAY)

#elif defined(__PCM__)
#include <16F877A.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20M)
#use rs232(baud=9600, uart1, errors, timeout=KEYHIT_DELAY)

#elif defined(__PCH__)
#include <18F4580.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20M)
#use rs232(baud=9600, uart1, errors, timeout=KEYHIT_DELAY)
#endif

void main()
{
   int status;
   char value;

   while(TRUE)
   {
      status=1;
      printf("\r\nStart typing:\r\n");
      while(!kbhit());

      do
      {
         value=getc();
         if (value)
            putc(value);
      } while (RS232_ERRORS); //RS232_ERRORS will be 0 when there is a timeout
      printf("\r\nToo slow!\r\n");
   }
}
