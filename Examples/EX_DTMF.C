/////////////////////////////////////////////////////////////////////////
////                           EX_DTMF.C                             ////
////                                                                 ////
////  This program simulates a touch-tone telephone.  The user can   ////
////  push keys on the keypad and the dtmf tones will be heard over  ////
////  the speaker.  If the user places the receiver of a telephone   ////
////  over the speaker, and then dials a number on the keypad, the   ////
////  telephone will recognize the dtmf tones and place the call.    ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////     Plug in the KEYPAD.                                         ////
////     Connect a R-2R ladder to Port D as shown below.             ////
////     Connect the negative wire of the speaker to ground.         ////
////     Connect the positive wire of the speaker to the output of   ////
////        the R-2R ladder.                                         ////
////                                                                 ////
////  PIN D0  PIN D1  PIN D2  PIN D3  PIN D4  PIN D5  PIN D6  PIN D7 ////
////    |       |       |       |       |       |       |       |    ////
////   2R      2R      2R      2R      2R      2R      2R      2R    ////
////    |       |       |       |       |       |       |       |    ////
////    *---R---*---R---*---R---*---R---*---R---*---R---*---R---*    ////
////    |                                                       |    ////
////   2R                                                       |    ////
////    |                R = 100 ohms                         OUTPUT ////
////   GND              2R = 200 ohms                                ////
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
#use delay(clock=20000000)    // must be 20 MHz

#elif defined(__PCM__)
#include <16F877.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)    // must be 20 MHz

#elif defined(__PCH__)
#include <18F452.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)    // must be 20 MHz
#endif

#include <kbd.c>

CONST unsigned int SINE_WAVE[200] = {
128,132,136,139,143,147,150,154,158,161,165,169,172,176,179,
182,186,189,192,195,199,202,204,207,210,213,215,218,220,223,
225,227,229,231,233,235,237,238,240,241,242,243,244,245,246,
247,247,247,248,248,248,248,248,247,247,247,246,245,244,243,
242,241,240,238,237,235,233,231,229,227,225,223,220,218,215,
213,210,207,204,202,199,195,192,189,186,182,179,176,172,169,
165,161,158,154,150,147,143,139,136,132,128,124,120,117,113,
109,106,102, 98, 95, 91, 87, 84, 80, 77, 74, 70, 67, 64, 61,
 57, 54, 52, 49, 46, 43, 41, 38, 36, 33, 31, 29, 27, 25, 23,
 21, 19, 18, 16, 15, 14, 13, 12, 11, 10,  9,  9,  9,  8,  8,
  8,  8,  8,  9,  9,  9, 10, 11, 12, 13, 14, 15, 16, 18, 19,
 21, 23, 25, 27, 29, 31, 33, 36, 38, 41, 43, 46, 49, 52, 54,
 57, 61, 64, 67, 70, 74, 77, 80, 84, 87, 91, 95, 98,102,106,
109,113,117,120,124};

int index1,index2,inc1,inc2;


#INT_RTCC
void wave_generator() {
   int wave = 0;

   set_rtcc(25);           // when clock is 20MHz, interrupts every 100us

   wave = ((long)SINE_WAVE[index1]+(long)SINE_WAVE[index2])/2;
   output_d(wave);

   index1 += inc1;
   index2 += inc2;

   if(index1 >= 200)
      index1 -= 200;

   if(index2 >= 200)
      index2 -= 200;
}

#define DTMF_ROW1   14  // for 700 Hz, increment this many times every 100us
#define DTMF_ROW2   15  // for 750 Hz, increment this many times every 100us
#define DTMF_ROW3   17  // for 850 Hz, increment this many times every 100us
#define DTMF_ROW4   19  // for 950 Hz, increment this many times every 100us
#define DTMF_COLA   24  // for 1200 Hz, increment this many times every 100us
#define DTMF_COLB   27  // for 1350 Hz, increment this many times every 100us
#define DTMF_COLC   30  // for 1500 Hz, increment this many times every 100us

void generate_dtmf_tone(char keypad, long duration)  {

   index1=0;
   index2=0;
   inc1=0;
   inc2=0;
   if((keypad=='1')||(keypad=='2')||(keypad=='3'))
      inc1=DTMF_ROW1;
   else if((keypad=='4')||(keypad=='5')||(keypad=='6'))
      inc1=DTMF_ROW2;
   else if((keypad=='7')||(keypad=='8')||(keypad=='9'))
      inc1=DTMF_ROW3;
   else if((keypad=='*')||(keypad=='0')||(keypad=='#'))
      inc1=DTMF_ROW4;

   if((keypad=='1')||(keypad=='4')||(keypad=='7')||(keypad=='*'))
      inc2=DTMF_COLA;
   else if((keypad=='2')||(keypad=='5')||(keypad=='8')||(keypad=='0'))
      inc2=DTMF_COLB;
   else if((keypad=='3')||(keypad=='6')||(keypad=='9')||(keypad=='#'))
      inc2=DTMF_COLC;

   setup_counters(RTCC_INTERNAL,RTCC_DIV_2);
   enable_interrupts(INT_RTCC);
   enable_interrupts(GLOBAL);

   while(duration-- > 0)
   {
      delay_ms(1);
   }
   disable_interrupts(INT_RTCC);
   output_d(0);
}


void main()  {
   char k;

   kbd_init();

   while(TRUE)
   {
      k=kbd_getc();
      if(k!=0)
         generate_dtmf_tone(k, 100);
   }
}
