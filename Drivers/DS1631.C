////////////////////////////////////////////////////////////////////////////
////   Library for a Dallas 1631 Temperature chip                       ////
////                                                                    ////
////   init_temp();          Call before the other functions are used   ////
////                                                                    ////
////   d = read_full_temp(); Read the temp in degrees F (-67 to 257)    ////
////                                                                    ////
////   This driver returns a high resolution value.  For lower          ////
////   resolutions use the DS1621.C driver as it is compatible with     ////
////   the DS1631.                                                      ////
////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C   ////
//// compiler.  This source code may only be distributed to other       ////
//// licensed users of the CCS C compiler.  No other use, reproduction  ////
//// or distribution is permitted without written permission.           ////
//// Derivative programs created using this software in object code     ////
//// form are not restricted in any way.                                ////
////////////////////////////////////////////////////////////////////////////


#ifndef DAL_SCL
#if defined(__PCD__)
#define DAL_SCL PIN_D0
#define DAL_SDA PIN_D1
#else
#define DAL_SCL PIN_B0
#define DAL_SDA PIN_B1
#endif
#endif

#define read_temp    read_full_temp // for backwards compatability

#use i2c(master, sda=DAL_SDA, scl=DAL_SCL)


void temp_config(BYTE data) {

   i2c_start();
   i2c_write(0x90);
   i2c_write(0xac);
   i2c_write(data);
   i2c_stop();
}


void init_temp() {
   output_high(DAL_SDA);
   output_high(DAL_SCL);
   i2c_start();
   i2c_write(0x90);
   i2c_write(0x51);
   i2c_stop();
   temp_config(0xc);
}


signed long read_full_temp() {  // Returns hundreths of degrees F (-67 to 257)
   signed int  datah, datal;
   signed long data;

   i2c_start();
   i2c_write(0x90);
   i2c_write(0xaa);
   i2c_start();
   i2c_write(0x91);
   datah=i2c_read();
   datal=i2c_read(0);
   i2c_stop();

   data=(signed long)datah*100;
   data=data+(((datal >> 4 )*(long)50)/16);
   data=data*9;
   data = (data / 5) + 3200;

   return(data);
}
