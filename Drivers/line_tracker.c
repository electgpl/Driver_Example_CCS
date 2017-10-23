///////////////////////////////////////////////////////////////////////////
////   Library for the line tracking sensors on the Picrobot           ////
////                                                                   ////
////   init_lineTracker();                                             ////
////     Call before the other functions are used                      ////
////                                                                   ////
////   int1 lt_check(int8 position);                                   ////
////     Call to test if a sensor is over black or white               ////
////        Inputs:                                                    ////
////           LT_LEFT, LT_RIGHT, or LT_CENTER                         ////
////        Outputs:                                                   ////
////           0 if white, 1 if black                                  ////
////                                                                   ////
////   int8 or int16 lt_measure(int8 position)                         ////
////     Call to measure the sensor value                              ////
////        Inputs:                                                    ////
////           LT_LEFT, LT_RIGHT, or LT_CENTER                         ////
////        Outputs:                                                   ////
////           The value measured by the A to D converter              ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2004 Custom Computer Services          ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
#ifndef LINE_TRACKER
#define LINE_TRACKER

// Used to select which sensor to read
// The numbers coorespond to an ADC channel
#ifndef LT_LEFT
#define LT_LEFT      5
#endif

#ifndef LT_RIGHT
#define LT_RIGHT     7
#endif

#ifndef LT_CENTER
#define LT_CENTER    6
#endif
///////////////////////////////////////////////////////////////////////////


// Purpose:    Initialize the line tracking sensors
// Inputs:     None
// Outputs:    None
void init_lineTracker()
{
   setup_adc(ADC_CLOCK_INTERNAL);
   setup_adc_ports(ALL_ANALOG);
}


// Purpose:    Check if a sensors sees a reflective surface
// Inputs:     LT_LEFT, LT_RIGHT, or LT_CENTER
// Outputs:    0 if reflective surface, 1 if not reflective surface
//             (0 if white, 1 if black)
int1 lt_check(int8 position)
{
   set_adc_channel(position);

   // Allow the voltage to stabilize
   delay_ms(4);

   #if getenv("ADC_RESOLUTION") == 8
   if(read_adc() < 120)
   #else
   if(read_adc() < 700)
   #endif
   {
      return 0;
   }

   return 1;
}


// Purpose:    Measure the sensor value from the A to D converter
// Inputs:     LT_LEFT, LT_RIGHT, or LT_CENTER
// Outputs:    An 8 or 16 bit integer with the value from the A to D converter
#if getenv("ADC_RESOLUTION") == 8
int8 lt_measure(int8 position)
#else
int16 lt_measure(int8 position)
#endif
{
   set_adc_channel(position);
   delay_ms(4);
   return read_adc();
}

#endif
