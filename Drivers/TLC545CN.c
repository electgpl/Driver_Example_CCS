//////////////// Driver for TLC545CN A/D Converter //////////////////////
////                                                                 ////
////  init_ext_adc()                      Call after power up        ////
////                                                                 ////
////  value = read_ext_adc()              Converts to digital number ////
////                                      and sends to MCU           ////
////                                                                 ////
////  value = read_ext_adc()              Converts digital number    ////
////                                      to voltage when vref- = 0  ////
////                                      and vref+ = 5V             ////
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

#define ADC_DO    PIN_B0
#define ADC_DI    PIN_B1
#define ADC_CS    PIN_B2
#define ADC_CLK   PIN_B4

void init_ext_adc()
{
   delay_us(1);
   output_high(ADC_CS);
   output_low(ADC_CLK);
}

BYTE read_ext_adc(BYTE address)
{
   BYTE i,data,junk=0,addr;
   addr=address;

   for(i=1;i<=3;++i)
   {
       shift_left(&address,1,0);
   }

   output_low(ADC_CS);
   for(i=1;i<=8;++i)
   {
      output_bit(ADC_DI, shift_left(&address,1,0));

      output_high(ADC_CLK);
      output_low(ADC_CLK);
   }
   output_high(ADC_CS);

   delay_us(10);
   
   output_low(ADC_CS);
   for(i=1;i<=8;++i)
   {
      output_bit(ADC_DI, shift_left(&addr,1,0));
      shift_left(&data,1,input(ADC_DO));        

      output_high(ADC_CLK);
      output_low(ADC_CLK);
   }
   output_high(ADC_CS);

   return(data);
}

float convert_to_volts(BYTE data)
{
   return ((float)data*5.0/255);
}
