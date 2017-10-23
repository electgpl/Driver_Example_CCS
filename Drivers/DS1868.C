///////////////////////////////////////////////////////////////////////////
////   Library for a Dalas DS1868                                      ////
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
#define NUM_POTS 2

BYTE pots[2];

set_pot (int pot_num, int new_value) {
   BYTE i;
   BYTE cmd[3];

   if (pot_num >= NUM_POTS)
      return;

   pots[pot_num] = new_value;

   cmd[0]=pots[0];
   cmd[1]=pots[1];
   cmd[2]=0;

   for(i=1;i<=7;i++)
     shift_left(cmd,3,0);

   output_high(RST1);
   delay_us(2);

   for(i=1;i<=17;i++) {
      output_bit(DI, shift_left(cmd,3,0));
      delay_us(2);
      output_high(CLK);
      delay_us(2);
      if(i==17)
         output_low(RST1);
      output_low(CLK);
      delay_us(2);
   }
}

init_pots ()
{
   set_pot (0,0);
   set_pot (1,0);
}


