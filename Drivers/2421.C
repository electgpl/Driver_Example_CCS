///////////////////////////////////////////////////////////////////////////
////   Library for a MicroChip 24C21                                   ////
////                                                                   ////
////   init_ext_eeprom();    Call before the other functions are used  ////
////                                                                   ////
////   write_ext_eeprom(a, d);  Write the byte d to the address a      ////
////                                                                   ////
////   d = read_ext_eeprom(a);  Read the byte d from the address a     ////
////                                                                   ////
////   The main program may define EEPROM_SDA                          ////
////   and EEPROM_SCL to override the defaults below.                  ////
////                                                                   ////
////                            Pin Layout                             ////
////   -----------------------------------------------------------     ////
////   |                                                         |     ////
////   | 1: NC   Not Connected | 8: VCC   +5V                    |     ////
////   |                       |                                 |     ////
////   | 2: NC   Not Connected | 7: VCLK  +5V                    |     ////
////   |                       |                                 |     ////
////   | 3: NC   Not Connected | 6: SCL   EEPROM_SCL and Pull-Up |     ////
////   |                       |                                 |     ////
////   | 4: VSS  GND           | 5: SDA   EEPROM_SDA and Pull-Up |     ////
////   -----------------------------------------------------------     ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2006 Custom Computer Services          ////
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

#use i2c(master, sda=EEPROM_SDA, scl=EEPROM_SCL)

#define EEPROM_ADDRESS BYTE
#define EEPROM_SIZE    128


/*
void init_ext_eep()
This will take initialize the Clock and Data pins on the EEPROM
Param: none
Returns: none
*/
void init_ext_eep()
{
   output_float(EEPROM_SCL);
   output_float(EEPROM_SDA);
   output_low(EEPROM_SCL);
   output_high(EEPROM_SDA);
   output_high(EEPROM_SCL);
}

/*
void write_ext_eeprom(EEPROM_ADDRESS address, BYTE data)
This will write a byte of data to the EEPROM
PARAM  EEPROM_ADDRESS : The address where data is to be written
PARAM data: The byte of data to be written
Returns: none
*/
void write_ext_eeprom(EEPROM_ADDRESS address, BYTE data) {
   i2c_start();
   i2c_write(0xa0);
   i2c_write(address);
   i2c_write(data);
   i2c_stop();
   delay_ms(10);
}

/*
BYTE read_ext_eeprom(EEPROM_ADDRESS address)
This will read a byte of data from the EEPROM
PARAM  EEPROM_ADDRESS : The read address of the EEPROM
Returns: Data byte
*/
BYTE read_ext_eeprom(EEPROM_ADDRESS address) {
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
