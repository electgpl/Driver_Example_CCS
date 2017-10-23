/////////////////////////////////////////////////////////////////////////
////                         EX_EXPIO.C                              ////
////                                                                 ////
////     This program shows how to use the 74165.C and 74595.C       ////
////  libraries for extended input and output.                       ////
////                                                                 ////
////     When button 1 is pushed, LED 1 will toggle green.  Button   ////
////  2 will toggle LED 2.  However, when both buttons are pushed,   ////
////  LED 3 will toggle green.                                       ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////               74165 pin    Protoboard                           ////
////                  11           Button 1                          ////
////                  12           Button 2                          ////
////                  13           gnd                               ////
////                  14           gnd                               ////
////                  15           gnd                               ////
////                  3            gnd                               ////
////                  4            gnd                               ////
////                  5            gnd                               ////
////                  6            gnd                               ////
////                  8            gnd                               ////
////                  16           +5V                               ////
////                  9            pin B5                            ////
////                  1            pin B3                            ////
////                  2            pin B4                            ////
////                                                                 ////
////               74595 pin    Protoboard                           ////
////                  15           LED 1                             ////
////                  1            LED 2                             ////
////                  2            LED 3                             ////
////                  3            gnd                               ////
////                  4            gnd                               ////
////                  5            gnd                               ////
////                  6            gnd                               ////
////                  7            gnd                               ////
////                  8            gnd                               ////
////                  13           gnd                               ////
////                  16           +5V                               ////
////                  10           +5V                               ////
////                  14           pin B2                            ////
////                  11           pin B1                            ////
////                  12           pin B0                            ////
////                                                                 ////
////  This example will work with the PCB, PCM and PCH compilers.    ////
////  The following conditional compilation lines are used to        ////
////  include a valid device for each compiler.  Change the device   ////
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
#include <16C56.h>
#fuses HS,NOWDT,NOPROTECT
#use delay(clock=20000000)

#elif defined(__PCM__)
#include <16F877.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)

#elif defined(__PCH__)
#include <18F458.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#endif

#include <74595.c>
#include <74165.c>


void main() {
   BYTE data;
   do {
      read_expanded_inputs (&data);

      data |= 0xF8;                           //Force the unused input bits on
      data -= (!(data&0x01)&!(data&0x02))<<2; //Turn on bit 2 it both inputs are
                                              //toggled
      write_expanded_outputs (&data);
   } while (TRUE);
}
