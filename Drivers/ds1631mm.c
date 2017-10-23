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
#define DAL_SCL PIN_B0
#define DAL_SDA PIN_B1
#endif

#define read_temp    read_full_temp // for backwards compatability

#use i2c(multi_master, sda=DAL_SDA, scl=DAL_SCL, force_hw)

void temp_config(BYTE data) {
	byte state;
	do
	{
	   i2c_start();
   	state = i2c_write(0x90);
		if(state == 0)			//bus is ours
		{
   	   state = i2c_write(0xac);
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

void init_temp() {
	byte state;
	output_float(DAL_SDA);
	output_float(DAL_SCL);
	do
	{
	   i2c_start();
	   state = i2c_write(0x90);
		if(state == 0)
		{
			state = i2c_write(0x51);
			if(state == 0)
			{
				i2c_stop();
				temp_config(0xc);
			}
		}
		if(state == 1)
		{
			i2c_stop();
		}
	}while(state != 0);
}

signed long read_full_temp() {  // Returns hundreths of degrees F (-67 to 257)
   signed int  datah, datal;
   signed long data;
	byte state;

	do
	{
	   i2c_start();
		state = i2c_write(0x90);
		if(state == 0)
		{
			state = i2c_write(0xaa);
			if(state == 0)
			{
				i2c_start();
				state = i2c_write(0x91);
				if(state == 0)
				{
					datah=i2c_read(1);
					datal=i2c_read(0);
					i2c_stop();
				}
			}
		}
		if(state == 1)
		{
			i2c_stop();
		}
	}while(state != 0);

	data=(signed long)datah*100;
	data=data+(((datal >> 4 )*(long)100)/16);
	data=data*9;
	data = (data / 5) + 3200;

	return(data);
}
