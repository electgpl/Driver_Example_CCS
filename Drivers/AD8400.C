///////////////////////////////////////////////////////////////////////////
////   Library for a AD8400                                            ////
////                                                                   ////
////   init_pots ();    Sets all of the pots to 0                      ////
////                                                                   ////
////   set_pot (pot_num, new_value);  Sets pot pot_num to new_value    ////
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

#define RST1 PIN_B0
#define CLK PIN_B1
#define DI PIN_B2
#define NUM_POTS 1

BYTE pots;

set_pot (int pot_num, int new_value) {
   BYTE i;
   BYTE cmd[2];

   pots = new_value;
   cmd[0] = pots;
   cmd[1] = 0;

   for(i=1;i<=6;i++)
     shift_left(cmd,2,0);

   output_low(CLK);
   output_low(RST1);

   delay_us(2);
   for(i=1;i<=10;++i) {
      output_bit(DI, shift_left(cmd,2,0));
      delay_us(2);
      output_high(CLK);
      delay_us(2);
      output_low(CLK);
      delay_us(2);
   }
   output_high(RST1);
}

void init_pots ()
{
   pots = 0;
}
