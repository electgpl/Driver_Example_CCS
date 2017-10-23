/////////////////////////////////////////////////////////////////////////////
////                              AD7705.C                               ////
////                     Driver for analog device AD7705                 ////
////                                                                     ////
//// adc_init()                    Call after power up                   ////
////                                                                     ////
//// read_adc_value(channel)Read adc value from the specified channel    ////
////                                                                     ////
//// adc_disable()    Disables the adc conversion                   ////
/////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services             ////
//// This source code may only be used by licensed users of the CCS C    ////
//// compiler.  This source code may only be distributed to other        ////
//// licensed users of the CCS C compiler.  No other use, reproduction   ////
//// or distribution is permitted without written permission.            ////
//// Derivative programs created using this software in object code      ////
//// form are not restricted in any way.                                 ////
/////////////////////////////////////////////////////////////////////////////
//// Driver routines for the AD7705 chip
//Assuming a 2.4576 crystal ocsillator is used between MCLK IN and MCLK OUT

//connection pins to the PIC
#define ADC_DRDY  PIN_B1
#define ADC_DO    PIN_B2
#define ADC_DI    PIN_B3
#define ADC_RESET PIN_B4
#define ADC_CS    PIN_B5
#define ADC_CLK   PIN_B6

//Operation modes
#define ADC_NORMAL 0x00
#define ADC_SELF 0x40
#define ADC_ZERO_SCALE 0x80
#define ADC_FULL_SCALE 0xc0

//Gain settings
#define ADC_GAIN_1 0x00
#define ADC_GAIN_2 0x08
#define ADC_GAIN_4 0x10
#define ADC_GAIN_8 0x18
#define ADC_GAIN_16 0x20
#define ADC_GAIN_32 0x28
#define ADC_GAIN_64 0x30
#define ADC_GAIN_128 0x38

//Polar operations
#define ADC_BIPOLAR 0x04
#define ADC_UNIPOLAR 0x00

//update rates
#define ADC_50 0x04
#define ADC_60 0x05
#define ADC_250 0x06
#define ADC_500 0x07
void write_adc_byte(BYTE data)
{
   BYTE i;

   output_low(ADC_CS);
   for(i=1;i<=8;++i) {
      output_low(ADC_CLK);
      output_bit(ADC_DI, shift_left(&data,1,0));
      output_high(ADC_CLK);
   }
   output_high(ADC_CS);
}


long int read_adc_word()
{
   BYTE i;
   long data;

   output_low(ADC_CS);
   for(i=1;i<=16;++i) {
      output_low(ADC_CLK);
      output_high(ADC_CLK);
      shift_left(&data,2,input(ADC_DO));
   }
   output_high(ADC_CS);
   return data;
}


//setup the device paramaters(mode, gainsetting, polar operation and output rate)
void setup_adc_device(int calmode, int gainsetting, int operation, int rate)
{
    write_adc_byte( 0x20 );//Communications Register set to write of clock register
   write_adc_byte( rate );//Clock Register info here
     write_adc_byte( 0x10 );//Communications Register set to write of setup register
   write_adc_byte( calmode|gainsetting|operation);//Setup Register info here
}

//initailaization routine

void adc_init()
{
   output_low(ADC_RESET);
   output_high(ADC_CLK);
   output_high(ADC_CS);   //Set low to AD7715 chip select low pin
   output_high(ADC_RESET);   //Set high to AD7715 reset low pin
   setup_adc_device(ADC_SELF,ADC_GAIN_1,ADC_BIPOLAR,ADC_50);
   delay_ms(3000);
}

//read an adc  value from the specified channel
long int read_adc_value(int1 ch)
{
   long int value=0;
   while (input(ADC_DRDY) );// Changed 1.16.07 Wait for line to go low before data capture
   if(ch)
      write_adc_byte(0x38);//communications register set to read of data register of channel 1
   else
      write_adc_byte(0x39);//communications register set to read of data register of channel 0

   value=read_adc_word();
   while (!input(ADC_DRDY)); // Changed 1.16.07   - Wait for line to come back up
   return value;
}

//disable the a/d conversion
void adc_disable()
{
    write_adc_byte( 0x20 );//Communications Register set to write of clock register
    write_adc_byte( 0x10 );//Clock Register info here
}

//Convert the value read to volts
float convert_to_volts(long data){
   return ((float)data*2.5/0xffff);
}
