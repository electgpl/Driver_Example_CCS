///////////////////////////////////////////////////////////////////////////
////   Library for a 74165 Expanded Input Chip                         ////
////                                                                   ////
////   Any number of these chips may be connected in series to get     ////
////   8 additional inputs per chip.  The cost is 3 I/O pins for       ////
////   any number of chips.                                            ////
////                                                                   ////
////    read_expanded_inputs(ei);  Reads the array ei from the chips   ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#IFNDEF EXP_IN_ENABLE

#define EXP_IN_ENABLE   PIN_B3
#define EXP_IN_CLOCK    PIN_B4
#define EXP_IN_DI       PIN_B5
#define NUMBER_OF_74165 1

#ENDIF


void read_expanded_inputs(BYTE *ei) {
  BYTE i;

  output_high(EXP_IN_CLOCK);
  output_low(EXP_IN_ENABLE);      // Latch all inputs
  output_high(EXP_IN_ENABLE);

  for(i=1;i<=NUMBER_OF_74165*8;++i) {      // Clock in bits to the ei structure
    shift_left(ei,NUMBER_OF_74165,input(EXP_IN_DI));
    output_low(EXP_IN_CLOCK);
    output_high(EXP_IN_CLOCK);
  }
  output_low(EXP_IN_ENABLE);
}

