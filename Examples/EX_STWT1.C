/////////////////////////////////////////////////////////////////////////
////                           EX_STWT1.C                            ////
////                                                                 ////
////  This program uses timer 1 and interrupts to keep a             ////
////  real time seconds counter.  A simple stop watch function is    ////
////  then implemented.                                              ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////                                                                 ////
////  This example will work with the PCM and PCH compilers.  The    ////
////  following conditional compilation lines are used to include a  ////
////  valid device for each compiler.  Change the device, clock and  ////
////  RS232 pins for your hardware if needed.                        ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2004 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////


#if defined(__PCM__)
#include <16F877a.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCH__)
#include <18F452.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#endif

#define INTS_PER_SECOND 19         // (20000000/(4*4*65536))

int8 seconds;      // A running seconds counter
int8 int_count;    // Number of interrupts left before a second has elapsed


#INT_TIMER1                        // This function is called every time
void clock_isr() {                 // timer 1 overflows (65535->0), which is
                                   // approximately 19 times per second for
    if(--int_count==0) {           // this program.
      ++seconds;
      int_count = INTS_PER_SECOND;
    }
}


void main() {
   int_count = INTS_PER_SECOND;
   setup_timer_1(T1_INTERNAL | T1_DIV_BY_4);
   set_timer1(0);
   enable_interrupts(INT_TIMER1);
   enable_interrupts(GLOBAL);

   while(TRUE) {
      printf("Press any key to begin.\n\r");
      getc();
      int_count = INTS_PER_SECOND;
      seconds = 0;
      printf("Press any key to stop.\n\r");
      getc();
      printf("%u seconds.\n\r", seconds);
   }
}
