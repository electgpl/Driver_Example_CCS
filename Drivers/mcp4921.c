//////////////// Driver for MCP4921 D/A Converter ///////////////////////
////                                                                 ////
////  init_dac()    Call this before using a MCP4921 DAC             ////
////                                                                 ////
////  write_dac( long int )   Writes command and 12 bit resolution   ////
////                          to the MCP4921 which converts it to a  ////
////                          voltage                                ////
////                          0-4095 input represents 0-vref V out.  ////
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

#ifndef EEPROM_SELECT

#define DAC_CS     PIN_B0
#define DAC_CLK    PIN_B1
#define DAC_DI     PIN_B2
#define DAC_LDAC   PIN_B4

#endif

void init_dac()
{
   output_high(DAC_CS);
   output_high(DAC_LDAC);
   output_high(DAC_CLK);
   output_high(DAC_DI);
}

void write_dac(int16 data) {
   BYTE cmd[3];
   BYTE i;

   cmd[0]=data;
   cmd[1]=(data>>8);
   cmd[2]=0x03;                           
   
   output_high(DAC_LDAC);
   output_low(DAC_CLK);
   output_low(DAC_CS);

   for(i=0; i<=23; ++i)
   {
      if(i<4 || (i>7 && i<12))
         shift_left(cmd,3,0);
      else
      {
         output_bit(DAC_DI, shift_left(cmd,3,0));

         output_high(DAC_CLK);
         output_low(DAC_CLK);
      }
   }
   output_high(DAC_CS);

   output_low(DAC_LDAC);
   delay_us(10);
   
   output_HIGH(DAC_LDAC);
}
