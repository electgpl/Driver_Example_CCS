////////////////////////////////////////////////////////////////////////////
////   Library for a LM75CIM3 Temperature chip                          ////
////                                                                    ////
////   init_temp();          Call before the other functions are used   ////
////                                                                    ////
////   d = read_full_temp(); Read the temp in degrees F                 ////
////                                                                    ////
////   NOTE:  DIVIDE result by 10 to get true temperature reading       ////
////                                                                    ////
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

#define read_temp    read_full_temp // for backwards compatability

#use i2c(master,sda=DAL_SDA, scl=DAL_SCL)

void init_temp() {
   output_high(DAL_SDA);
   output_high(DAL_SCL);
   i2c_start();
   i2c_write(0x90);
   i2c_write(0x00);
   i2c_write(0x00);
   i2c_stop();
}

signed long read_full_temp() {
   signed long datah, datal;
   signed long data;

   i2c_start();
   i2c_write(0x90);
   i2c_write(0x00);
   i2c_start();
   i2c_write(0x91);
   datah=i2c_read();
   datal=i2c_read(0);
   i2c_stop();

   data=(signed long)datah*10;

   if(bit_test(datal,7))
   {
      if(data < 0)
      {
          data -= 5;
      }
      else
      {
         data += 5;
      }
   }

   data=(data)*90;
   data=(data/50)+320;

   return(data);
}
