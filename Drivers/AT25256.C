///////////////////////////////////////////////////////////////////////////
////   Library for an ATMEL25128 or ATMEL25256                         ////
////    ATMEL25128 has 16,384 (or 13bits of addressing) locations      ////
////    ATMEL25256 has 32,768 words of 8 bits                          ////
////                                                                   ////
////   Uses software (bit banging on the port pins)                    ////
////                                                                   ////
////                     Pin Layout                                    ////
////   ATMEL EEPROM pin     to      Microchip MCU Pin                  ////
////   ----------------             -----------------                  ////
////   1 (CS)                       PortB 0                            ////
////   2 (SO)                       PortC 4                            ////
////   3 (WP)                       +5V                                ////
////   4 (GND)                      GND                                ////
////   5 (SI)                       PortC 5                            ////
////   6 (SCK)                      PortC 3                            ////
////   7 (HOLD)                     +5V                                ////
////   8 (VCC)                      +5V                                ////
////                                                                   ////
////   init_ext_eeprom();    Call before the other functions are used  ////
////                                                                   ////
////   write_ext_eeprom(a, d);  Write the unsigned int8 d to the address a      ////
////                                                                   ////
////   d = read_ext_eeprom(a);   Read the unsigned int8 d from the address a    ////
////                                                                   ////
////   b = ext_eerpom_ready();  Returns TRUE if the eeprom is ready    ////
////                            to receive opcodes                     ////
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
#define EEPROM_SELECT PIN_B0
#define EEPROM_DI     PIN_C5
#define EEPROM_DO     PIN_C4
#define EEPROM_CLK    PIN_C3
#endif

#ifndef EEPROM_ADDRESS
#define EEPROM_ADDRESS long
#endif

#byte SSPSTAT=0x94
#byte SSPCON=0x14

void init_ext_eeprom() {
   int1 i;

   output_low(EEPROM_DI);
   output_low(EEPROM_CLK);
   output_high(EEPROM_SELECT);   //at25256 is cs active LOW
   i=input(EEPROM_DO);
}

BOOLEAN ext_eeprom_ready() {
   unsigned int8 cmd[1],i,data;

   cmd[0] = 0x05;         //rdsr opcode

   output_low(EEPROM_SELECT);

   for(i=1;i<=8;++i) {
      output_bit(EEPROM_DI, shift_left(cmd,1,0));
      output_high(EEPROM_CLK);      //data latches
      output_low(EEPROM_CLK);      //back to idle
   }

   for(i=1;i<=8;++i) {
        output_high(EEPROM_CLK);      //data latches
        shift_left(&data,1,input(EEPROM_DO));
        output_low(EEPROM_CLK);              //back to idle
   }
   output_high(EEPROM_SELECT);
   return (!(bit_test(data, 0)));
}


void write_ext_eeprom(EEPROM_ADDRESS address, unsigned int8 data) {

   unsigned int8 cmd[4];
   unsigned int8 i;
   cmd[2]=((address>>8)&(0xFF));   //address msb (16bit addressing)
   cmd[1]=address-cmd[2];      //address lsb

   do {} while (!(ext_eeprom_ready())); //wait until the eeprom is out of the previous write state, if applicable

   cmd[0]=0x06;
   output_low(EEPROM_SELECT); //cs is active low
   for(i=1;i<=8;++i) {
      output_bit(EEPROM_DI, shift_left(cmd,1,0));
      output_high(EEPROM_CLK);      //data latches
      output_low(EEPROM_CLK);      //back to idle
   }
   output_high(EEPROM_SELECT);

   cmd[0]=data;
   cmd[3]=0x02;      //write opcode
   output_low(EEPROM_SELECT);
   for(i=1;i<=32;++i) {
      output_bit(EEPROM_DI, shift_left(cmd,4,0));
      output_high(EEPROM_CLK);      //data latches
      output_low(EEPROM_CLK);      //back to idle
   }

   output_high(EEPROM_SELECT);
}


unsigned int8 read_ext_eeprom(EEPROM_ADDRESS address) {
   unsigned int8 i,data;
   unsigned int8 cmd[3];
   cmd[2]=0x03;            //read opcode
   cmd[1]=((address>>8)&(0xFF));
   cmd[0]=address-cmd[1];

   do {} while (!(ext_eeprom_ready())); //wait until the eeprom is out of the previous write state, if applicable
   output_low(EEPROM_SELECT);
   for(i=1;i<=24;++i) {
      output_bit(EEPROM_DI, shift_left(cmd,3,0));
      output_high(EEPROM_CLK);      //data latches
      output_low(EEPROM_CLK);      //back to idle
   }

   for(i=1;i<=8;++i) {
        output_high(EEPROM_CLK);      //data latches
        shift_left(&data,1,input(EEPROM_DO));
        output_low(EEPROM_CLK);              //back to idle
   }

   output_high(EEPROM_SELECT);

   return(data);
}
