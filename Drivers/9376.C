///////////////////////////////////////////////////////////////////////////
////   Library for a MicroChip 93AA76C configured for a x8 org           ////
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

#define EEPROM_ADDRESS long int
#define EEPROM_SIZE    1024

void init_ext_eeprom() {
   BYTE cmd[2];
   BYTE i;

   cmd[0]=0;
   cmd[1]=0x26;

   output_low(EEPROM_DI);
   output_low(EEPROM_CLK);
   output_low(EEPROM_SELECT);

   for(i=1;i<=2;++i)
      shift_left(cmd,2,0);
   output_high(EEPROM_SELECT);
   for(i=1;i<=14;++i) {
      output_bit(EEPROM_DI, shift_left(cmd,2,0));
      output_high(EEPROM_CLK);
      output_low(EEPROM_CLK);
   }
   output_low(EEPROM_DI);
   output_low(EEPROM_SELECT);
}

void write_ext_eeprom(EEPROM_ADDRESS address, BYTE data) {
   BYTE cmd[4];
   BYTE i;

   cmd[0]=data;
   cmd[1]=address;
   cmd[2]=((address>>8)&0x3);
   cmd[3]=0x05;

   for(i=1;i<=5;++i)
      shift_left(cmd,4,0);
   output_high(EEPROM_SELECT);
   for(i=1;i<=27;++i)
   {
      if(i<4 || i>8)
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

BYTE read_ext_eeprom(EEPROM_ADDRESS address) {
   BYTE cmd[4];
   BYTE i,data;

   cmd[0]=0;
   cmd[1]=address;
   cmd[2]=((address>>8)&0x3);
   cmd[3]=0x06;

   for(i=1;i<=5;++i)
      shift_left(cmd,4,0);
   output_high(EEPROM_SELECT);
   for(i=1;i<=27;++i)
   {
      if(i<4 || i>8)
      {
         output_bit(EEPROM_DI, shift_left(cmd,4,0));
         output_high(EEPROM_CLK);
         output_low(EEPROM_CLK);
      }
      else
         shift_left(cmd,4,0);

      if(i>19)
         shift_left(&data,1,input(EEPROM_DO));

   }
   output_low(EEPROM_SELECT);
   return(data);
}
