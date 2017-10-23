////////////////////////////////////////////////////////////////////////////
////   Library for the PCF8570 SRAM                                     ////
////                                                                    ////
////   write_ext_sram(a, d);  Write the byte d to the address a         ////
////                                                                    ////
////   d = read_ext_sram(a);  Read the byte d from the address a        ////
////                                                                    ////
////   The main program may define sram_sda                             ////
////   and sram_scl to override the defaults below.                     ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services            ////
//// This source code may only be used by licensed users of the CCS C   ////
//// compiler.  This source code may only be distributed to other       ////
//// licensed users of the CCS C compiler.  No other use, reproduction  ////
//// or distribution is permitted without written permission.           ////
//// Derivative programs created using this software in object code     ////
//// form are not restricted in any way.                                ////
////////////////////////////////////////////////////////////////////////////

#ifndef SRAM_SDA
#define SRAM_SDA  PIN_B1
#define SRAM_SCL  PIN_B0
#endif

#use i2c(master, sda=SRAM_SDA, scl=SRAM_SCL)

void write_ext_sram(BYTE address, BYTE data)
{
   i2c_start();
   i2c_write(0xa0);
   i2c_write(address);
   i2c_write(data);
   i2c_stop();
}


BYTE read_ext_sram(BYTE address)
{
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

