/////////////////////////////////////////////////////////////////////////
////                           EX_SINE.C                             ////
////                                                                 ////
////  This program demonstrates digital to analog conversion by      ////
////  using the RTCC (timer0) and the Max517 chip to generate a      ////
////  sine wave with a period of 10Hz.                               ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////     Connect Max517 pin 1 to scope (output)                      ////
////     Also make the following connections:                        ////
////          Max517 pin     Protoboard                              ////
////              2            gnd                                   ////
////              3            pin C1 (also use pull up resistor)    ////
////              4            pin C0 (also use pull up resistor)    ////
////              5            +5V                                   ////
////              6            +5V                                   ////
////              7            +5V                                   ////
////              8            +5V                                   ////
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

#include <max517.c>


BYTE CONST SINE_WAVE[200] = {
128,132,136,139,143,147,150,154,158,161,165,169,172,176,179,
182,186,189,192,195,199,202,204,207,210,213,215,218,220,223,
225,227,229,231,233,235,237,238,240,241,242,243,244,245,246,
247,247,247,248,248,248,248,248,247,247,247,246,245,244,243,
242,241,240,238,237,235,233,231,229,227,225,223,220,218,215,
213,210,207,204,202,199,195,192,189,186,182,179,176,172,169,
165,161,158,154,150,147,143,139,136,132,128,124,120,117,113,
109,106,102,98,95,91,87,84,80,77,74,70,67,64,61,57,54,52,49,
46,43,41,38,36,33,31,29,27,25,23,21,19,18,16,15,14,13,12,11,
10,9,9,9,8,8,8,8,8,9,9,9,10,11,12,13,14,15,16,18,19,21,23,
25,27,29,31,33,36,38,41,43,46,49,52,54,57,61,64,67,70,74,77,
80,84,87,91,95,98,102,106,109,113,117,120,124};

BYTE sine_index;

#int_rtcc
void isr() {

   set_rtcc(102);         // frequency of interrrupt = (clock/(4*divisor))/(256-reload)
                          //                 2029 hz = (20000000/(4*16))/(256-102)

   write_dac(SINE_WAVE[sine_index]);

   if(++sine_index==200) {
      sine_index=0;
   }
}


void main() {

  setup_counters( RTCC_INTERNAL, RTCC_DIV_16);
  enable_interrupts(INT_RTCC);
  enable_interrupts(GLOBAL);

  while (TRUE) ;
}
