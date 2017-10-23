///////////////////////////////////////////////////////////////////////////
////   Library for a Dallas 1624 Temperature and memory chip           ////
////                                                                   ////
////   init()       Call before any other functions are called         ////
////                                                                   ////
////   init_temp();          Call before the read temp function is used////
////                                                                   ////
////   d = read_temp();      Read the temerature in farenheit          ////
////                                                                   ////
////   write_ext_eeprom(int address,int val) writes val to the eeprom  ////
////                                                                   ////
////   dat=read_ext_eeprom(int address) reads value from the address   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////


#ifndef DAL_SCL
#define DAL_SCL PIN_C0
#define DAL_SDA PIN_C1
#endif

#use i2c(master, sda=DAL_SDA, scl=DAL_SCL)


void temp_config()
{
   i2c_start();
   i2c_write(0x90);
   i2c_write(0xac);
   i2c_write(0x01);
   i2c_stop();
}

void init()
{
   output_high(DAL_SDA);
   output_high(DAL_SCL);
}


void init_temp() {
   temp_config();
   delay_ms(50);
   i2c_start();
   i2c_write(0x90);
   i2c_write(0xee);
   i2c_stop();
}


float read_temp()        ////// Returns in farenheit
{
   int datah,datal;
   float dval,fval;
   i2c_start();
   i2c_write(0x90);
   i2c_write(0xaa);
   i2c_start();
   i2c_write(0x91);
   datah=i2c_read();
   datal=i2c_read(0);
   i2c_stop();
   dval=datah+.03125*shift_right(&datal,1,0);
   fval=(dval*1.8)+32;
   return(fval);
}


int read_ext_eeprom(int address)
{
   int data;
   i2c_start();
   i2c_write(0x90);
   i2c_write(0x17);
   i2c_write(address);
   i2c_start();
   i2c_write(0x91);
   data=i2c_read();
   i2c_stop();
   return data;


}


void write_ext_eeprom(int address, int data)
{
   i2c_start();
   i2c_write(0x90);
   i2c_write(0x17);
   i2c_write(address);
   i2c_write(data);
   i2c_stop();
   delay_ms(50);
}
