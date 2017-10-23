///////////////////////////////////////////////////////////////////////////
////   Library for a 24XX1025 serial EEPROM                            ////
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
////                            Pin Layout                             ////
////   -----------------------------------------------------------     ////
////   |                                                          |    ////
////   | 1: A0   Address Input 0| 8: VCC   +5V                    |    ////
////   |                        |                                 |    ////
////   | 2: A1   Address Input 1| 7: WP    Write Protect          |    ////
////   |                        |                                 |    ////
////   | 3: A2   Connect to VCC | 6: SCL   EEPROM_SCL and Pull-Up |    ////
////   |                        |                                 |    ////
////   | 4: VSS  GND            | 5: SDA   EEPROM_SDA and Pull-Up |    ////
////   -----------------------------------------------------------     ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2006 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////                                                                                                                                                         

#ifndef EEPROM_SDA

#define EEPROM_SDA  PIN_C4
#define EEPROM_SCL  PIN_C3

#endif


#define EEPROM_ADDRESS int32
#define EEPROM_SIZE   0x1FFFF

#use i2c(master, sda=EEPROM_SDA, scl=EEPROM_SCL)

/////////////////////////////
////                     ////
//// Function Prototypes ////
////                     ////
/////////////////////////////

/*
void init_ext_eeprom()
This function needs to be called before other functions are used
Param: none
Returns: none
*/
void init_ext_eeprom();
/*
void write_ext_eeprom(int32 address, BYTE data);
This function writes a byte of data to the specified address in the EEPROM
Param: EEPROM_ADDRESS : The address where data is to be written (32 bit)
Param data: The byte of data to be written
Returns: none
*/

void write_ext_eeprom(EEPROM_ADDRESS address, BYTE data);
/*
BYTE read_ext_eeprom(EEPROM_ADDRESS address)
This will read a byte of data from the EEPROM
Param:  EEPROM_ADDRESS : The read address of the EEPROM
Returns: Data byte
*/
BYTE read_ext_eeprom(int32 address);


//////////////////////////////////////////////
////////IMPLEMENTATION////////////////////////
//////////////////////////////////////////////

void init_ext_eeprom()
{
   output_float(EEPROM_SCL);
   output_float(EEPROM_SDA);

}


void write_ext_eeprom(int32 address, BYTE data)
{
   int1 status;
   int  command;

   if (address>65535)command=0xa8; // Check if address is located in Block 0/1
   else command=0xa0;

   i2c_start();
   i2c_write(command);
   i2c_write(address>>8);
   i2c_write(address);
   i2c_write(data);

   i2c_stop();
   i2c_start();
   status=i2c_write(command);
   while(status==1)
   {
      i2c_start();
      status=i2c_write(command);
   }
}

BYTE read_ext_eeprom(int32 address) {
   BYTE data;
   int   command;

   if (address>65535)command=0xa8;
   else command=0xa0;

   i2c_start();
   i2c_write(command);
   i2c_write(address>>8);
   i2c_write(address);
   i2c_start();
   i2c_write(command+1);
   data=i2c_read(0);
   i2c_stop();
   return(data);
}


