///////////////////////////////////////////////////////////////////////////
////     Drivers to read the object sensors on the PICROBOT            ////
////                                                                   ////
////  void init_objectSensors()                                        ////
////     Call before any other object sensor functions                 ////
////                                                                   ////
////  int8 read_leftObjectSensor()                                     ////
////     Call to read the value on the left object sensor              ////
////                                                                   ////
////  int8 read_rightObjectSensor()                                    ////
////     Call to read the value on the right object sensor             ////
////                                                                   ////
////  void read_objectSensors(int8& left, int8& right)                 ////
////     Call to read the current voltage on both object sensors       ////
////     The two input parameters will contain the current reading     ////
////     after the function returns.                                   ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2003 Custom Computer Services          ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////


#ifndef OBJECT_SENSORS
#define OBJECT_SENSORS

#ifndef LEFT_ADC_CHANNEL
#define LEFT_ADC_CHANNEL   1
#endif

#ifndef RIGHT_ADC_CHANNEL
#define RIGHT_ADC_CHANNEL  2
#endif


// Purpose:    Initialize the analog to digital converter
// Inputs:     None
// Outputs:    None
void init_objectSensors()
{
   setup_adc(ADC_CLOCK_INTERNAL);
   setup_adc_ports(ALL_ANALOG);
}


// Purpose:    Read the ADC value from the left sensor
// Inputs:     None
// Outputs:    The value read from the left sensor's ADC port
#if getenv("ADC_RESOLUTION") == 8
int8 read_leftObjectSensor()
#else
int16 read_leftObjectSensor()
#endif
{
   // Set the ADC to read the left object sensor
   set_adc_channel(LEFT_ADC_CHANNEL);

   // Allow the voltage to stabilize
   delay_ms(1);

   // Read the value from the left sensor
   return read_adc();
}


// Purpose:    Read the ADC value from the right sensor
// Inputs:     None
// Outputs:    The value read from the right sensor's ADC port
#if getenv("ADC_RESOLUTION") == 8
int8 read_rightObjectSensor()
#else
int16 read_rightObjectSensor()
#endif
{
   // Set the ADC to read the right object sensor
   set_adc_channel(RIGHT_ADC_CHANNEL);

   // Allow the voltage to stabilize
   delay_ms(1);

   // Read the value from the right sensor
   return read_adc();
}


// Purpose:    Read the ADC values from both sensors
// Inputs:     1) A reference to the location to store the left measurement
//             2) A reference to the location to store the right measurement
// Outputs:    None
#if getenv("ADC_RESOLUTION") == 8
void read_objectSensors(int8& leftSensor, int8& rightSensor)
#else
void read_objectSensors(int16& leftSensor, int16& rightSensor)
#endif
{
   // Set the ADC to read the left object sensor
   set_adc_channel(LEFT_ADC_CHANNEL);

   // Allow the voltage to stabilize
   delay_ms(1);

   // Read the value from the left sensor
   leftSensor = read_adc();


   // Set the ADC to read the right object sensor
   set_adc_channel(RIGHT_ADC_CHANNEL);

   // Allow the voltage to stabilize
   delay_ms(1);

   // Read the value from the right sensor
   rightSensor = read_adc();
}

#endif
