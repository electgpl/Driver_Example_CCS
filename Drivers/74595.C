///////////////////////////////////////////////////////////////////////////
////   Library for a 74595 Expanded Output Chip                        ////
////                                                                   ////
////   Any number of these chips may be connected in serise to get     ////
////   8 additional outputs per chip.  The cost is 3 I/O pins for      ////
////   any number of chips.                                            ////
////                                                                   ////
////   write_expanded_outputs(eo);  Writes the array eo to the chips   ////
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

#IFNDEF EXP_OUT_ENABLE

#define EXP_OUT_ENABLE  PIN_B0
#define EXP_OUT_CLOCK   PIN_B1
#define EXP_OUT_DO      PIN_B2
#define NUMBER_OF_74595 1

#ENDIF


void write_expanded_outputs(BYTE* eo) {
  BYTE i;

  output_low(EXP_OUT_CLOCK);
  output_low(EXP_OUT_ENABLE);

  for(i=1;i<=NUMBER_OF_74595*8;++i) {  // Clock out bits from the eo array
    if((*(eo+(NUMBER_OF_74595-1))&0x80)==0)
      output_low(EXP_OUT_DO);
    else
      output_high(EXP_OUT_DO);
   shift_left(eo,NUMBER_OF_74595,0);
   output_high(EXP_OUT_CLOCK);
   output_low(EXP_OUT_CLOCK);
  }
  output_high(EXP_OUT_ENABLE);
  output_low(EXP_OUT_ENABLE);

}
