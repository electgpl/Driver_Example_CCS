////////////////////////////////////////////////////////////////////////////
////   Library for a Dallas 1621 Temperature chip                       ////
////                                                                    ////
////   init_temp();          Call before the other functions are used   ////
////                                                                    ////
////   d = read_high_temp(); Read the temperature in degrees F (0-255)  ////
////                                                                    ////
////   d = read_low_temp();  Read the temp in degrees F (-67 to 127)    ////
////                                                                    ////
////   d = read_full_temp(); Read the temp in degrees F (-67 to 257)    ////
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


#ifndef DAL_SCL
#define DAL_SCL PIN_B0
#define DAL_SDA PIN_B1
#endif

#define read_temp    read_high_temp // for backwards compatability

#use i2c(master,sda=DAL_SDA, scl=DAL_SCL)


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
   i2c_write(0xee);
   i2c_stop();
   temp_config(8);
}


BYTE read_high_temp() {        // Returns degrees F (0-255)
   BYTE datah,datal;
   signed long data;

   i2c_start();
   i2c_write(0x90);
   i2c_write(0xaa);
   i2c_start();
   i2c_write(0x91);
   datah=i2c_read();
   datal=i2c_read(0);
   i2c_stop();

   data=datah;
   data=data*9;
   if(bit_test(datal,7))
   {
      if(data < 0)
          data -= 4;
      else
         data += 4;
   }
   data=(data/5)+32;

   if(data < 0)
      data = 0;
   else if(data > 255)
      data = 255;

   return((int)data);
}


signed int read_low_temp() {  // Returns degrees F (-67 to 127)
   signed int datah, datal;
   signed long data;

   i2c_start();
   i2c_write(0x90);
   i2c_write(0xaa);
   i2c_start();
   i2c_write(0x91);
   datah=i2c_read();
   datal=i2c_read(0);
   i2c_stop();

   data=datah;
   data=data*9;
   if(bit_test(datal,7))
   {
      if(data < 0)
          data -= 4;
      else
         data += 4;
   }
   data = (data / 5) + 32;

   if(data > 127)
      data = 127;

   return((int)data);
}


signed long read_full_temp() {  // Returns degrees F (-67 to 257)
   signed int datah, datal;
   signed long data;

   i2c_start();
   i2c_write(0x90);
   i2c_write(0xaa);
   i2c_start();
   i2c_write(0x91);
   datah=i2c_read();
   datal=i2c_read(0);
   i2c_stop();

   data=datah;
   data=data*9;
   if(bit_test(datal,7))
   {
      if(data < 0)
          data -= 4;
      else
         data += 4;
   }
   data = (data / 5) + 32;

   return(data);
}
