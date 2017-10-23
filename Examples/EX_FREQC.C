/////////////////////////////////////////////////////////////////////////
////                             EX_FREQC.C                          ////
////  Counts the 'zero' crossings of an input signal for 1 second,   ////
////  therefore counting the frequency of the signal.  The input     ////
////  signal's frequency is then displayed over the serial port.     ////
////                                                                 ////
////  The 1s delay is created by generating a perfect 5,000,000      ////
////  cycles (it takes 0.2us for one cycle at 20Mhz).  The trick is  ////
////  that conditional statements have different values of cycles    ////
////  depending on a TRUE or FALSE jump.                             ////
////                                                                 ////
////  The input signal is to be supplied to pin C0                   ////
////                                                                 ////
////  This example will work with the PCM compiler.  The             ////
////  following conditional compilation lines are used to include a  ////
////  valid device for each compiler.  Change the device and clock   ////
////  for your hardware if needed.                                   ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#include <16F877.h>
#fuses HS,NOWDT,NOLVP
#use delay(clock=20000000)    //one instruction=0.2us
#use rs232(baud=9600, xmit=PIN_c6, rcv=PIN_c7)
#bit t1_overflow=0x0C.0

// #bit t1_overflow=0xF9E.0  (PIC18, Reminder)

void main() {
   int cycles8, cycles;
   int32 freq;
   long freqc_high;
   long freqc_low;

   while (TRUE) {
      cycles8=0;
      cycles=0;
      freqc_high=0;
      t1_overflow=0;
      set_timer1(0);
      setup_timer_1(T1_EXTERNAL|T1_DIV_BY_1);
/* ___ wait one second ___  */
      while (cycles!=0xFF) { //true=3, false=4
       cycles8=0; //1 cycle
       //start inner loop
       while (cycles8!=0xFF) { //true=3, false=4
         if (t1_overflow)//true=2,false=3             //----|
            {t1_overflow=0;freqc_high++;}//6 cycles   //    |
         else                                         //    |-- 8 cycles
            {delay_cycles(5);}                        //----|
         delay_cycles(62); //x
         cycles8++; //1
 ///2 cycles to jump to top
 //math: end inner loop
 //math: total inner loop=((3+8+x+1+2)*255 + 4)*255
 //math: if x=62.87781 then inner loops takes 5mil instructions
 //math: if x=62 then inner loop takes 4942920, have to fill 57080 cycles
  }
 delay_cycles(216);		//y
 cycles++;		 	///1 cycle
 ///2 cylces to jump to top
 //math: outer=(3+1+y+1+2)*255+4=57080
 //math: y=(57080-4)/255)-(3+1+0+0+1+2)
 //math: if y=216.827450980392156862745098039216 then outer loop cylces is 57080
 //math: if y=216 then outer loop cycles is off by 211 cycles.  z=211
}
      delay_cycles(211);	//z
/* ___ end waiting 1 second ___ */
      setup_timer_1(T1_DISABLED);	//turn of counter to prevent corruption while grabbing value
      if (t1_overflow)				//check one last time for overflow
          freqc_high++;
      freqc_low=get_timer1();		//get timer1 value as the least sign. 16bits of freq counter
      freq=make32(freqc_high,freqc_low);	//use new make32 function to join lsb and msb
      printf("%LU Hz\r\n",freq);		//and print frequency

   }
}
