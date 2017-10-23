///////////////////////////////////////////////////////////////////////////
////   Library for a mcp41010-i/p                                      ////
////                                                                   ////
////   set_pot (int data);  Sets pot to new_value                      ////
////                                                                   ////
////   shutdown_pot ();  shutdown pot to save power                    ////
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

#define CS PIN_B4
#define SCLK PIN_B2
#define SI PIN_B1

set_pot (int data) {
   BYTE i;
   BYTE cmd[2];

   cmd[0] = data;
   cmd[1] = 0x11;

   output_low(SCLK);
   output_low(CS);

   for(i=1;i<=16;++i) {
      output_bit(SI, shift_left(cmd,2,0));

      output_high(SCLK);
      output_low(SCLK);
   }
   output_high(CS);
}

shutdown_pot () {
   BYTE i;
   BYTE cmd[2];

   cmd[0] = 0;
   cmd[1] = 0x21;

   output_low(SCLK);
   output_low(CS);

   for(i=1;i<=16;++i) {
      output_bit(SI, shift_left(cmd,2,0));

      output_high(SCLK);
      output_low(SCLK);
   }
   output_high(CS);
}
