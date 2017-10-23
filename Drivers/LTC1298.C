//////////// Driver for LTC1298 A/D Converter /////////////////////////////
////                                                                   ////
////  adc_init()                                 Call after power up   ////
////                                                                   ////
////  value = read_analog( channel )             Read a analog channel ////
////                                             channel is 0 or 1     ////
////                                                                   ////
////  convert_to_volts( value,  string )         Fills in string with  ////
////                                             the true voltage in   ////
////                                             the form 0.000        ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#ifndef ADC_CS

#define ADC_CLK  PIN_B0
#define ADC_DOUT PIN_B1
#define ADC_DIN  PIN_B2
#define ADC_CS   PIN_B3

#endif



void adc_init() {
   output_high(ADC_CS);
}

void write_adc_byte(BYTE data_byte, BYTE number_of_bits) {
   BYTE i;

   delay_us(2);
   for(i=0; i<number_of_bits; ++i) {
      if((data_byte & 1)==0)
        output_low(ADC_DIN);
      else
        output_high(ADC_DIN);
      data_byte=data_byte>>1;
      output_high(ADC_CLK);
      delay_us(50);
      output_low(ADC_CLK);
      delay_us(50);
   }
}


BYTE read_adc_byte(BYTE number_of_bits) {
   BYTE i,data;

   data=0;
   for(i=0;i<number_of_bits;++i) {
      output_high(ADC_CLK);
      delay_us(50);
      shift_left(&data,1,input(ADC_DOUT));
      output_low(ADC_CLK);
      delay_us(50);
   }
   return(data);
}


long int read_analog( BYTE channel ) {
   int l;
   long int h;

   delay_us(200);

   output_low(ADC_CLK);
   output_high(ADC_DIN);
   output_low(ADC_CS);

   if(channel==0)
     channel=0x1b;
   else
     channel=0x1f;
   write_adc_byte( channel, 5);

   h=read_adc_byte(8);
   l=read_adc_byte(4)<<4;
   output_high(ADC_CS);
   return((h<<8)|l);
}

void convert_to_volts( long int data, char volts[6]) {
   BYTE i;
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
