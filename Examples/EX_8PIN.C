/////////////////////////////////////////////////////////////////////////
////                             EX_8PIN.C                           ////
////  This program shows how to use the pins on a PIC12C508/9 or a   ////
////  PIC12C671/2 for general I/O.  When run, the program will run up////
////  and down the LEDs.  If you hold down the push button, the      ////
////  program will reverse its direction.                            ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////             12C508/671 pin   Protoboard                         ////
////                  1                +5V                           ////
////                  2                LED 5                         ////
////                  3                LED 4                         ////
////                  4                Push Button                   ////
////                  5                LED 3                         ////
////                  6                LED 2                         ////
////                  7                LED 1                         ////
////                  8                gnd                           ////
////                                                                 ////
////  This example will work with the PCB and PCM compilers.  The    ////
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


#if defined(__PCB__)
#include <12C508.h>
#fuses INTRC,NOWDT,NOPROTECT, NOMCLR
#use delay(clock=4000000)
#define GP0 PIN_B0
#define GP1 PIN_B1
#define GP2 PIN_B2
#define GP3 PIN_B3
#define GP4 PIN_B4
#define GP5 PIN_B5

#elif defined(__PCM__)
#include <12C671.h>
#fuses INTRC,NOWDT,NOPROTECT, NOMCLR,NOLVP
#use delay(clock=4000000)
#define GP0 PIN_A0
#define GP1 PIN_A1
#define GP2 PIN_A2
#define GP3 PIN_A3
#define GP4 PIN_A4
#define GP5 PIN_A5

#endif


void cycle_forward (void)
{
   output_low (GP5);
   output_high (GP0);
   delay_ms (70);
   output_low (GP0);
   output_high (GP1);
   delay_ms (70);
   output_low (GP1);
   output_high (GP2);
   delay_ms (70);
   output_low (GP2);
   output_high (GP4);
   delay_ms (70);
   output_low (GP4);
   output_high (GP5);
   delay_ms (70);
}

void cycle_backward (void)
{
   output_low (GP0);
   output_high (GP5);
   delay_ms (70);
   output_low (GP5);
   output_high (GP4);
   delay_ms (70);
   output_low (GP4);
   output_high (GP2);
   delay_ms (70);
   output_low (GP2);
   output_high (GP1);
   delay_ms (70);
   output_low (GP1);
   output_high (GP0);
   delay_ms (70);
}

void main() {
#if defined(__PCB__)
   setup_counters (RTCC_INTERNAL,RTCC_DIV_2);
                               // See .h file for other special
                               // options for setup_counters
#else
   setup_counters (RTCC_INTERNAL,RTCC_DIV_2);
#endif


   while (TRUE) {
      if (!input (GP3))
		   cycle_forward ();
      else
		   cycle_backward ();
   }
}

