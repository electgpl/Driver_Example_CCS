/////////////////////////////////////////////////////////////////////////
////                           EX_STWT.C                             ////
////                                                                 ////
////  This program uses the RTCC (timer0) and interrupts to keep a   ////
////  real time seconds counter.  A simple stop watch function is    ////
////  then implemented.                                              ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////                                                                 ////
////  This example will work with the PCM, PCH and PCD compilers. The////
////  following conditional compilation lines are used to include a  ////
////  valid device for each compiler.  Change the device, clock and  ////
////  RS232 pins for your hardware if needed.                        ////
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
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCH__)
#include <18F452.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCD__)
#include <30f2010.h>
#fuses HS, NOWDT
#use delay(clock=20000000)
#use rs232(baud=9600, UART1A)
#endif

#if !defined(__PCD__)
#define INTS_PER_SECOND 76     // (20000000/(4*256*256))
#endif
#if defined (__PCD__)
#define INTS_PER_SECOND 100
#endif

BYTE seconds;      // A running seconds counter
BYTE int_count;    // Number of interrupts left before a second has elapsed

#if !defined(__PCD__)
#int_rtcc                          // This function is called every time
void clock_isr() {                 // the RTCC (timer0) overflows (255->0).
                                   // For this program this is apx 76 times
    if(--int_count==0) {           // per second.
      ++seconds;
      int_count=INTS_PER_SECOND;
    }

}
#endif

#if defined(__PCD__)
#int_timer1                          
void clock_isr()
{
    if(--int_count==0) {           // per second.
      ++seconds;
      int_count=INTS_PER_SECOND;
    }
}

#endif

void main() {

   int_count=INTS_PER_SECOND;
   
   #if !defined(__PCD__)
   set_timer0(0);
   setup_counters( RTCC_INTERNAL, RTCC_DIV_256 | RTCC_8_BIT);
   enable_interrupts(INT_RTCC);
   enable_interrupts(GLOBAL);
   #endif
   
   #if defined(__PCD__)
   setup_timer1(TMR_INTERNAL,50000);
   enable_interrupts(INT_TIMER1);
   enable_interrupts(INTR_GLOBAL);
   #endif

   do {

      printf("Press any key to begin.\n\r");
      getc();
      seconds=0;
      set_timer1(0);
      printf("Press any key to stop.\n\r");
      getc();
      printf("%u seconds.\n\r",seconds);

   } while (TRUE);

}
