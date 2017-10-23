///////////////////////////////////////////////////////////////////////////
////   Library for a MicroChip 25C160                                  ////
////                                                                   ////
////   init_ext_eeprom();    Call before the other functions are used  ////
////                                                                   ////
////   write_ext_eeprom(a, d);  Write the byte d to the address a      ////
////                                                                   ////
////   d = read_ext_eeprom(a);   Read the byte d from the address a    ////
////                                                                   ////
////   b = ext_eeprom_ready();  Returns TRUE if the eeprom is ready    ////
////                            to receive opcodes                     ////
////                                                                   ////
////   The main program may define EEPROM_SELECT, EEPROM_DI, EEPROM_DO ////
////   and EEPROM_CLK to override the defaults below.                  ////
////                                                                   ////
////                                                                   ////
////                      Pin Layout                                   ////
////   -----------------------------------------------                 ////
////   |    __                                       |                 ////
////   | 1: CS   EEPROM_SELECT | 8: VCC   +5V        |                 ////
////   |                       |    ____             |                 ////
////   | 2: SO   EEPROM_DO     | 7: HOLD  +5V        |                 ////
////   |    __                 |                     |                 ////
////   | 3: WP   +5V           | 6: SCK   EEPROM_CLK |                 ////
////   |                       |                     |                 ////
////   | 4: VSS  GND           | 5: SI    EEPROM_DI  |                 ////
////   -----------------------------------------------                 ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2003 Custom Computer Services          ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#ifndef EEPROM_SELECT

#define EEPROM_SELECT PIN_B0
#define EEPROM_CLK    PIN_B1
#define EEPROM_DI     PIN_B2
#define EEPROM_DO     PIN_B4

#endif

#define EEPROM_ADDRESS long int
#define EEPROM_SIZE    2048

void init_ext_eeprom() {
   output_high(EEPROM_SELECT);
   output_low(EEPROM_DI);
   output_low(EEPROM_CLK);
}

BOOLEAN ext_eeprom_ready() {
   BYTE cmd[1], i, data;

   cmd[0] = 0x05;			         //rdsr opcode

   output_low(EEPROM_SELECT);

   for(i=1; i<=8; ++i) {
      output_bit(EEPROM_DI, shift_left(cmd,1,0));
      output_high(EEPROM_CLK);	//data latches
      output_low(EEPROM_CLK);		//back to idle
   }

   for(i=1; i<=8; ++i) {
        output_high(EEPROM_CLK);	//data latches
        shift_left(&data,1,input(EEPROM_DO));
        output_low(EEPROM_CLK);  //back to idle
   }
   output_high(EEPROM_SELECT);
   return !bit_test(data, 0);
}

void write_ext_eeprom(EEPROM_ADDRESS address, BYTE data) {
   BYTE cmd[4];
   BYTE i;
   BYTE wren;
   wren=0x06;
   cmd[0]=data;
   cmd[1]=address;
   cmd[2]=(address>>8);
   cmd[3]=0x02;

   // Wait until the eeprom is done with a previous write
   while(!ext_eeprom_ready());

   output_low(EEPROM_SELECT);
   for(i=0; i<8; ++i)
   {
      output_bit(EEPROM_DI, shift_left(&wren,1,0));
      output_high(EEPROM_CLK);
      output_low(EEPROM_CLK);
   }
   output_high(EEPROM_SELECT);
   output_low(EEPROM_SELECT);
   for(i=0; i<32; ++i)
   {
      output_bit(EEPROM_DI, shift_left(cmd,4,0));
      output_high(EEPROM_CLK);
      output_low(EEPROM_CLK);
   }
   output_high(EEPROM_SELECT);
}

BYTE read_ext_eeprom(EEPROM_ADDRESS address) {
   BYTE cmd[3];
   BYTE i,data;
   cmd[0]=address;
   cmd[1]=(address>>8);
   cmd[2]=0x03;

   // Wait until the eeprom is done with a previous write
   while(!ext_eeprom_ready());

   output_low(EEPROM_SELECT);
   for(i=0; i<24; ++i)
   {
      output_bit(EEPROM_DI, shift_left(cmd,3,0));
      output_high(EEPROM_CLK);
      output_low(EEPROM_CLK);
   }
   for(i=0; i<8; ++i)
   {
      shift_left(&data,1,input(EEPROM_DO));
      output_high(EEPROM_CLK);
      output_low(EEPROM_CLK);
   }
   output_high(EEPROM_SELECT);
   return(data);
}
