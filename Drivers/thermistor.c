/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////                          thermistor.c                           ////
////                                                                 ////
//// Library containing routines for converting and ADC reading into ////
//// tenth degrees Fahrenheit.  Math is performed using floating     ////
//// point variables and the log() routine.                          ////
////                                                                 ////
//// The library works with a simple voltage divider network where a ////
//// thermistor is working as a voltage divider with another         ////
//// resistor.  This library will work with two configurations:      ////
//// a.)where the thermistor is pulling the voltage divider to the   ////
////   ADC reference voltage and the resistor is pulling the voltage ////
////   divider to ground.                                            ////
//// b.) the thermistor is pulling the voltage divider to ground and ////
////   the resistor is pulling the voltage divider up to the ADC     ////
////   reference voltage.                                            ////
////                                                                 ////
//// thermistor_config(t0, r0, b, isPullup, rdiv, bits)              ////
////     Must be called before any other routines are used.  This    ////
////     configures the thermistor libary for your configuration.    ////
////     This only needs to be called once.                          ////
////     t0 - Temperature (celsius) at known temperature, usually    ////
////        specified in the datasheet for your theremistor.  This   ////
////        is usually 25 degrees celsius.                           ////
////     r0 - Resistance value of thermistor at t0 temperature.      ////
////        This will be documented in the datasheet/specifications  ////
////        of your thermistor.                                      ////
////     b - Beta constant of your thermistor.  This will be         ////
////        documented in the datasheet/specifications of your       ////
////        thermistor.                                              ////
////     isPullup - set to TRUE if the thermistor is the pull-up     ////
////        of the voltage divider.  set to FALSE if the thermistor  ////
////        is the pulldown of the voltage divider.                  ////
////     rdiv - The resistance value of the other resistor in the    ////
////        voltage divider network.                                 ////
////     bits - the number of bits of ADC resolution being passed to ////
////        the termistor library.                                   ////
////                                                                 ////
//// temp = thermistor_convert(adc)                                  ////
////     Convert the adc reading into tenths degrees fahrenheit.     ////
////     Value is signed.                                            ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2011 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#ifndef __THERMISTOR_C__
#define __THERMISTOR_C__

static float g_ThermoBdivT0;   //stored in kelvin
static float g_Thermor0;
static float g_Thermob;
static int1 g_ThermoisPullup;
static unsigned int16 g_Thermordiv;
static unsigned int32 g_Thermofullscale;

#define KELVIN_TO_C  ((float)272.15)

#include <math.h>

/*
   library makes the assumption that the pull-up voltage is the same 
   voltage as the ADC reference voltage.
*/
void thermistor_config(signed int16 t0, unsigned int16 r0, unsigned int16 b, int1 isPullup, unsigned int16 rdiv, unsigned int8 bits)
{
   g_Thermor0 = (float)r0;
   g_Thermob = (float)b;
   g_ThermoBdivT0 = g_Thermob / ((float)t0 + KELVIN_TO_C);
   g_ThermoisPullup = isPullup;
   g_Thermordiv = rdiv;
   g_Thermofullscale = (unsigned int32)1 << (unsigned int32)bits;
}

signed int16 thermistor_convert(unsigned int16 adc)
{
   unsigned int32 r; //in tenths
   float l, t;
   signed int16 res;
   
   if (g_ThermoisPullup)
   {
      r = g_Thermofullscale * (unsigned int32)g_Thermordiv;
      r -= (unsigned int32)adc * g_Thermordiv; 
      r /= (unsigned int32)adc;
   }
   else
   {
      r = (unsigned int32)adc * (unsigned int32)g_Thermordiv;
      r /= (g_Thermofullscale - (unsigned int32)adc);
   }
      
   l = log((float)r/g_Thermor0);
   
   t = g_Thermob / (l + g_ThermoBdivT0);  //we now have temp in kelvin

   t -= KELVIN_TO_C;
   t *= (float)18;   //9/5 in tenths
   
   res = (signed int32)t;
   
   res += (signed int32)320;
   
   return(res);
}

#endif
