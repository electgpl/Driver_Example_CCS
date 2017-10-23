///////////////////////////////////////////////////////////////////////////
////   Library for a MicroChip PIC12CE67X EEPROM                       ////
////                                                                   ////
////   init_ext_eeprom();    Call before the other functions are used  ////
////                                                                   ////
////   write_ext_eeprom(a, d);  Write the byte d to the address a      ////
////                                                                   ////
////   d = read_ext_eeprom(a);   Read the byte d from the address a    ////
////                                                                   ////
////   The main program may define eeprom_sda, eeprom_scl              ////
////   and eeprom_vdd to override the defaults below.                  ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
////////////////////////////////////////////////////////////////////////////

#ifndef EEPROM_SDA
   #define EEPROM_SDA  46
#endif

#ifndef EEPROM_SCL
   #define EEPROM_SCL  47
#endif

#use i2c(MASTER, SDA=EEPROM_SDA, SCL=EEPROM_SCL, NO_STRETCH, NOFLOAT_HIGH)

#define EEPROM_ADDRESS int
#define EEPROM_SIZE    16

void init_ext_eeprom() {
   output_high(EEPROM_SDA);
   output_high(EEPROM_SCL);
}


void write_ext_eeprom(int address, BYTE data) {
   i2c_start();
   i2c_write(0xa0);
   i2c_write(address);
   i2c_write(data);
   i2c_stop();
   delay_ms(11);
}


BYTE read_ext_eeprom(int address) {
   BYTE data;

   i2c_start();
   i2c_write(0xa0);
   i2c_write(address);
   i2c_start();
   i2c_write(0xa1);
   data=i2c_read(0);
   i2c_stop();
   return(data);
}
