//////////////// Driver for MAX517 D/A Converter ////////////////////////
////                                                                 ////
////  write_dac( int )   Writes a byte to the MAX517 chip which      ////
////                     converts it to a voltage.                   ////
////                     0-255 input represents 0-5V out.            ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#ifndef MAX517_SDA

#define MAX517_SDA    PIN_C0
#define MAX517_CLK    PIN_C1

#endif

#use i2c(master, sda=MAX517_SDA, scl=MAX517_CLK, FAST)


void write_dac(int data_byte) {

 i2c_start();
 i2c_write(0x5e);           // Send the address of the device
 i2c_write(0);
 i2c_write(data_byte);      // Send the data to the device
 i2c_stop();
}

