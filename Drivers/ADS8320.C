//////////////// Driver for ADS8320 A/D Converter ///////////////////////
////                                                                 ////
////  init_ext_adc()                      Call after power up        ////
////                                                                 ////
////  value = read_ext_adc()              Converts to digital number ////
////                                      and sends to MCU           ////
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


#ifndef ADS8320_CS

#define ADS8320_CLK  PIN_B0
#define ADS8320_DOUT PIN_B1
#define ADS8320_CS   PIN_B2

#endif


void init_ext_adc() {
   output_high(ADS8320_CS);
   output_high(ADS8320_CLK);
}


long read_ext_adc() {
   int i;
   long data;

   data=0;
   output_low(ADS8320_CS);
   for(i=0;i<=5;i++)             // take sample and send start bit
   {
      output_low(ADS8320_CLK);
      delay_us(1);
      output_high(ADS8320_CLK);
      delay_us(1);
   }
   for(i=0;i<16;++i) {           // send sample over spi
      output_low(ADS8320_CLK);
      delay_us(1);
      shift_left(&data,2,input(ADS8320_DOUT));
      output_high(ADS8320_CLK);
      delay_us(1);
   }

   output_high(ADS8320_CS);
   return(data);
}

float convert_to_volts(long data) {
   return ((float)data*5.0/0xffff);
}
