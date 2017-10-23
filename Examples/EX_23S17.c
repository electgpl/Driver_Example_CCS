/////////////////////////////////////////////////////////////////////////
////                            EX_23S17.C                           ////
////                                                                 ////
////  This program interfaces a Microchip MCP23S17 Port Extension    ////
////  chip. The chip is being used to control 2-seven segment        ////
////  displays while only using 4 pins on the PIC. This example only ////
////  has one 23S17 on the SPI bus. If more are being used (up to 8) ////
////  then IO_MULTIPLE_DEVICES needs to be defined and an address    ////
////  field needs to be passed into each function. Refer to the      ////
////  driver for more documentation.                                 ////
////                                                                 ////
////  This example will only work for devices that support SPI.      ////
////  Change the device, clock, SPI, and RS232 if necessary for your ////
////  hardware.                                                      ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2009 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////


#include <18F4520.h>
#fuses NOWDT, HS
#use delay(clock=20M)
#use spi(MASTER, MODE=0, SPI1, FORCE_HW)

#define IO_CS_PIN		PIN_C2   //must be defined to use the driver

#include <23S17.c>

const int8 LED_MAP[10] = {0x7E,0x30,0x6D,0x79,0x33,0x5B,0x5F,0x70,0x7F,0x7B};

void main(){

	int8 counter = 0;
	
	delay_ms(1);

	IO_INIT();	//initializes the MCP23S17 chip.
	
	//setting both ports as outputs
	IO_SET_TRIS_A(0x00);
	IO_SET_TRIS_B(0x00);
	
	while(TRUE){
	
		IO_OUTPUT_A(LED_MAP[counter/10]^0x7F); //outputting the 10s place on port A
		IO_OUTPUT_B(LED_MAP[counter%10]^0x7F); //outputting the 1s place on port B
		
		if(counter == 99)
			counter = 0;
		else
			counter++;
		
		delay_ms(1000);
	
	}

}
