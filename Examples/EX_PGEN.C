/////////////////////////////////////////////////////////////////////////
////                             EX_PEGN.C                           ////
////                                                                 ////
////  This program is used for the PGEN board sold with the exp kit. ////
////  The board has two 10 pos switches one controls the high time   ////
////  and the other the low time of the pulses.  If a switch is in   ////
////  the 0 position it will wait for the button to be pressed and   ////
////  then issue a single pulse.                                     ////
////                                                                 ////
////  This example will work with the PCB, PCM and PCH compilers.    ////
////  The following conditional compilation lines are used to        ////
////  include a valid device for each compiler.  Change the device,  ////
////  clock and RS232 pins for your hardware if needed.              ////
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

#elif defined(__PCM__)
#include <16F877.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)

#elif defined(__PCH__)
#include <18F452.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#endif

#BYTE port_b = 6

void wait(BYTE time) {
   switch(time) {
     case 0 : break;
     case 1 : delay_us(100);  break;
     case 2 : delay_ms(1);    break;
     case 3 : delay_ms(10);   break;
     case 4 : delay_ms(25);   break;
     case 5 : delay_ms(50);   break;
     case 6 : delay_ms(100);  break;
     case 7 : delay_ms(250);  break;
     case 8 : delay_ms(500);  break;
     case 9 : delay_ms(1000); break;
   }
}

void read_selections(int & sw1, int & sw2) {
      sw1 = (~(port_b >> 4))&15;
      sw2 = (~(port_b & 15))&15;
}

void main() {
   BYTE high_time, low_time;
#define one_shot_selected ((high_time==0)||(low_time==0))

   while (TRUE) {

      read_selections(high_time, low_time);

      if(high_time==0)
        output_high(pin_a0);
      else
        output_low(pin_a0);

      while ((one_shot_selected) && input(pin_a1))
         read_selections(high_time, low_time);

      output_high(pin_a0);
      wait(high_time);
      output_low(pin_a0);
      wait(low_time);

      if( one_shot_selected )
        delay_ms(1500);
   }
}
