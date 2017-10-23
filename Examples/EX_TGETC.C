/////////////////////////////////////////////////////////////////////////
////                         EX_TGETC.C                              ////
////                                                                 ////
////  This program echoes the key presses of the user and times-     ////
////  out after a specified amount of time.                          ////
////                                                                 ////
////  #use rs232() also has a timeout method, for an example of      ////
////  this see ex_tgetc2.c                                           ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCB        pin A2 to RS232 RX, pin A3 to RS232 TX           ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////     PCD        UART1A                                           ////
////                                                                 ////
////  This example will work with the PCB, PCM, PCD, and PCH         ////
////  compilers. The following conditional compilation lines are     ////
////  used to include a valid device for each compiler.  Change the  ////
////  device, clock and RS232 pins for your hardware if needed.      ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////


#if defined(__PCB__)
#include <16C56.h>
#fuses HS,NOWDT,NOPROTECT
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_A3, rcv=PIN_A2)

#elif defined(__PCM__)
#include <16F877A.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCH__)
#include <18F452.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCD__)
#include <30F2010.h>
#fuses HS,NOWDT
#use delay(clock=20000000)
#use rs232(baud=9600, UART1A)
#endif

#include <input.c>

#define  KEYHIT_DELAY   500     // in milliseconds


char timed_getc() {
   unsigned int16 timeout;
   char retval;

   timeout=0;
   while(!kbhit() && (++timeout< (KEYHIT_DELAY*100)))
      delay_us(10);
   if(kbhit())
      retval = getc();
   else
      retval = 0;
   return(retval);
}

void main()
{
   unsigned int8 status;
   char value;

   while(TRUE)
   {
      status=1;
      printf("\r\nStart typing:\r\n");
      while(!kbhit());

      while(status==1)
      {
         value=timed_getc();
         if(value==0)
            status=0;
         else
         {
            status=1;
            putc(value);
         }
      }
      printf("\r\nToo slow!\r\n");
   }
}
