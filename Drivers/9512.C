///////////////////////////////////////////////////////////////////////////
////   Library for a MicroChip 95C12N                                  ////
////                                                                   ////
////   init_ext_eeprom();    Call before the other functions are used  ////
////                                                                   ////
////   write_ext_eeprom(a, d);  Write the byte d to the address a      ////
////                                                                   ////
////   d = read_ext_eeprom(a);   Read the byte d from the address a    ////
////                                                                   ////
////   The main program may define eeprom_select, eeprom_di, eeprom_do ////
////   and eeprom_clk to override the defaults below.                  ////
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
#ifndef EEPROM_SELECT

#define EEPROM_SELECT PIN_B4
#define EEPROM_CLK    PIN_B2
#define EEPROM_DI     PIN_B1
#define EEPROM_DO     PIN_B0

#endif

#define EEPROM_ADDRESS BYTE
#define EEPROM_SIZE    61

void init_ext_eeprom() {
   BYTE cmd[2];
   BYTE i;

   output_low(EEPROM_DI);
   output_low(EEPROM_CLK);
   output_low(EEPROM_SELECT);

   cmd[0]=0x30;
   cmd[1]=0x1;

   for(i=1;i<=6;++i)
      shift_left(cmd,2,0);
   output_high(EEPROM_SELECT);
   for(i=1;i<=10;++i) {
      output_bit(EEPROM_DI, shift_left(cmd,2,0));
      output_high(EEPROM_CLK);
      output_low(EEPROM_CLK);
   }
   output_low(EEPROM_DI);
   output_low(EEPROM_SELECT);
}

void write_ext_eeprom(EEPROM_ADDRESS address, long int data) {
   BYTE cmd[4];
   BYTE i;

   cmd[0]=(BYTE)data;
   cmd[1]=(data>>8);
   cmd[2]=address;
   cmd[3]=0x5;

   for(i=1;i<=4;++i)
      shift_left(cmd,4,0);
   output_high(EEPROM_SELECT);
   for(i=1;i<=28;++i)
   {
      if(i<5 || i>6)
      {
         output_bit(EEPROM_DI, shift_left(cmd,4,0));
         output_high(EEPROM_CLK);
         output_low(EEPROM_CLK);
      }
      else
         shift_left(cmd,4,0);
   }
   output_low(EEPROM_DI);
   output_low(EEPROM_SELECT);
   delay_ms(11);
}

long int read_ext_eeprom(EEPROM_ADDRESS address) {
   BYTE cmd[4];
   BYTE i;
   long int data;

   cmd[0]=0x00;
   cmd[1]=0x00;
   cmd[2]=address;
   cmd[3]=0x6;

   for(i=1;i<=4;++i)
      shift_left(cmd,4,0);
   output_high(EEPROM_SELECT);
   for(i=1;i<=28;++i)
   {
      if(i<5 || i>6)
      {
         output_bit(EEPROM_DI, shift_left(cmd,4,0));
         output_high(EEPROM_CLK);
         output_low(EEPROM_CLK);
      }
      else
         shift_left(cmd,4,0);

      if(i>12)
         shift_left(&data,2,input(EEPROM_DO));
   }
   output_low(EEPROM_SELECT);
   return(data);
}
