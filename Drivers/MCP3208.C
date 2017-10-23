////////////////// Driver for MCP3208 A/D Converter ////////////////////////
////																	////
////  adc_init()														////
////		Call after power up											////
////																	////
////  value = read_analog_mcp( channel, mode )							////
////		Read an analog channel										////
////		0 through 7 and select										////
////		differential (0) or											////
////		single (1) mode												////
////																	////
////  value = read_analog( channel )									////
////		Read an analog channel										////
////		0 through 7 in	single mode									////
////																	////
////  convert_to_volts( value,  string )								////
////		Fills in string with										////
////		the true voltage in											////
////		the form 0.000												////
////																	////
////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services            ////
//// This source code may only be used by licensed users of the CCS C   ////
//// compiler.  This source code may only be distributed to other       ////
//// licensed users of the CCS C compiler.  No other use, reproduction  ////
//// or distribution is permitted without written permission.           ////
//// Derivative programs created using this software in object code     ////
//// form are not restricted in any way.                                ////
////////////////////////////////////////////////////////////////////////////


#ifndef MCP3208_CS

#define MCP3208_CLK  PIN_B0
#define MCP3208_DOUT PIN_B1
#define MCP3208_DIN  PIN_B2
#define MCP3208_CS   PIN_B3

#endif


void adc_init() {
   output_high(MCP3208_CS);
}


void write_adc_byte(BYTE data_byte, BYTE number_of_bits) {
   BYTE i;

   delay_us(2);
   for(i=0; i<number_of_bits; ++i) {
      output_low(MCP3208_CLK);
      if((data_byte & 1)==0)
			output_low(MCP3208_DIN);
      else
			output_high(MCP3208_DIN);
      data_byte=data_byte>>1;
      delay_us(50);
      output_high(MCP3208_CLK);
      delay_us(50);
   }
}


BYTE read_adc_byte(BYTE number_of_bits) {
   BYTE i,data;

   data=0;
   for(i=0;i<number_of_bits;++i) {
      output_low(MCP3208_CLK);
      delay_us(50);
      shift_left(&data,1,input(MCP3208_DOUT));
      output_high(MCP3208_CLK);
      delay_us(50);
   }
   return(data);
}


long int read_analog_mcp(BYTE channel, BYTE mode) {
   int l;
   long int h;
	BYTE ctrl_bits;

   delay_us(200);

	if(mode!=0)
		mode=1;

   output_low(MCP3208_CLK);
   output_high(MCP3208_DIN);
   output_low(MCP3208_CS);

	if(channel==1)					// Change so MSB of channel #
		ctrl_bits=4;				//		is in LSB place
	else if(channel==3)
		ctrl_bits=6;
	else if(channel==4)
		ctrl_bits=1;
	else if(channel==6)
		ctrl_bits=3;
	else
		ctrl_bits=channel;

	ctrl_bits=ctrl_bits<<1;

	if(mode==1)						// In single mode
		ctrl_bits |= 1;
	else								// In differential mode
		ctrl_bits &= 0xfe;

	ctrl_bits=ctrl_bits<<1;		// Shift so LSB is start bit
	ctrl_bits |= 1;

   write_adc_byte( ctrl_bits, 7);	// Send the control bits

   h=read_adc_byte(8);
   l=read_adc_byte(4)<<4;

   output_high(MCP3208_CS);

   return((h<<8)|l);
}


long int read_analog( BYTE channel )	// Auto specifies single mode
{
	return read_analog_mcp( channel, 1);
}


void convert_to_volts( long int data, char volts[6]) {
   BYTE i, d, div_h, div_l;
   long int temp,div;

   div=0x3330;

   for(i=0;i<=4;i++) {
     temp=data/div;
     volts[i]=(BYTE)temp+'0';
     if(i==0) {
       volts[1]='.';
       i++;
     }
     temp=div*(BYTE)temp;
     data=data-temp;
     div=div/10;
   }
   volts[i]='\0';
}
