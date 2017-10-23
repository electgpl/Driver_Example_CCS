///////////////////////////////////////////////////////////////////////////
////   Library for a MicroChip 24LC08B                                 ////
////                                                                   ////
////   init_ext_eeprom();    Call before the other functions are used  ////
////                                                                   ////
////   write_ext_eeprom(a, d);  Write the byte d to the address a      ////
////                                                                   ////
////   d = read_ext_eeprom(a);  Read the byte d from the address a     ////
////                                                                   ////
////   b = ext_eeprom_ready();  Returns TRUE if the eeprom is ready    ////
////                            to receive opcodes                     ////
////                                                                   ////
////   The main program may define EEPROM_SDA                          ////
////   and EEPROM_SCL to override the defaults below.                  ////
////                                                                   ////
////                            Pin Layout                             ////
////   -----------------------------------------------------------     ////
////   |                                                         |     ////
////   | 1: NC   Not Connected | 8: VCC   +5V                    |     ////
////   |                       |                                 |     ////
////   | 2: NC   Not Connected | 7: WP    GND                    |     ////
////   |                       |                                 |     ////
////   | 3: NC   Not Connected | 6: SCL   EEPROM_SCL and Pull-Up |     ////
////   |                       |                                 |     ////
////   | 4: VSS  GND           | 5: SDA   EEPROM_SDA and Pull-Up |     ////
////   -----------------------------------------------------------     ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2003 Custom Computer Services          ////
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

#use i2c(multi_master, sda=EEPROM_SDA, scl=EEPROM_SCL, force_hw)

#define EEPROM_ADDRESS long int
#define EEPROM_SIZE    1024

void init_ext_eeprom() {
   output_float(EEPROM_SCL);
   output_float(EEPROM_SDA);
}

void write_ext_eeprom(long int address, BYTE data) {
	byte state;
	do
	{
	   i2c_start();
		state = i2c_write((0xa0|(BYTE)(address>>7))&0xfe);
		if(state == 0)			//bus is ours
		{
			state = i2c_write(address);
			if(state == 0)
			{
				state = i2c_write(data);
				i2c_stop();
			}
		}
		if(state == 1)			//if no acknowledge, release before retry
		{
			i2c_stop();
		}
	}while(state != 0);
}

int read_ext_eeprom(long int address) {
	byte data, state;
	do
	{
	   i2c_start();
		state = i2c_write((0xa0|(BYTE)(address>>7))&0xfe);
		if(state == 0)
		{
			state = i2c_write(address);
			if(state == 0)
			{
				i2c_start();
				state = i2c_write((0xa0|(BYTE)(address>>7))|1);
				if(state == 0)
				{
					data=i2c_read(0);
					i2c_stop();
				}
			}
		}
		if(state == 1)
		{
			i2c_stop();
		}
	}while(state != 0);
	return(data);
}
