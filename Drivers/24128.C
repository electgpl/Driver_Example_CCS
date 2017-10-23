///////////////////////////////////////////////////////////////////////////
////   Library for a 24C128 serial EEPROM                              ////
////                                                                   ////
////   init_ext_eeprom();    Call before the other functions are used  ////
////                                                                   ////
////   write_ext_eeprom(a, d);  Write the byte d to the address a      ////
////                                                                   ////
////   d = read_ext_eeprom(a);   Read the byte d from the address a    ////
////                                                                   ////
////   The main program may define eeprom_sda                          ////
////   and eeprom_scl to override the defaults below.                  ////
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

#ifndef EEPROM_SDA

#define EEPROM_SDA  PIN_B1
#define EEPROM_SCL  PIN_B0

#endif

#define hi(x)  (*((int8 *)&x+1))

#use i2c(master, sda=EEPROM_SDA, scl=EEPROM_SCL)

#define EEPROM_ADDRESS long int
#define EEPROM_SIZE   16384

void init_ext_eeprom()
{
    int i;

   output_float(EEPROM_SCL);
   output_float(EEPROM_SDA);
   delay_us(4);
   for (i=0;i<3;i++) {
     i2c_start();
     i2c_write(0xa0);
     i2c_write(0xff);
     i2c_write(0xff);
     if(i==2)
       i2c_write(0x2);
     else
       i2c_write(0x2+i);
     i2c_stop();
  }
}


void write_ext_eeprom(long int address, BYTE data)
{
   i2c_start();
   i2c_write(((0xa0)|(hi(address)>>5))&0xfe);
   i2c_write(hi(address)&0x3f);
   i2c_write(address);
   i2c_write(data);
   i2c_stop();
   delay_ms(10);
}


BYTE read_ext_eeprom(long int address) {
   BYTE data;

   i2c_start();
   i2c_write(((0xa0)|(hi(address)>>5))&0xfe);
   i2c_write(hi(address)&0x3f);
   i2c_write(address);
   i2c_start();
   i2c_write((0xa1)|(hi(address)>>5));
   data=i2c_read(0);
   i2c_stop();
   return(data);
}
