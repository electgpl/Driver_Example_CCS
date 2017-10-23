////////////////////////////////////////////////////////////////////////////
////   Library for the ADT7301 Digital Temperature Sensor               ////
////                                                                    ////
////   init_temp();          Call before the other functions are used   ////
////                                                                    ////
////   d = read_full_temp(); Read the temp in degrees F                 ////
////                                                                    ////
////   d = read_full_tempC(); Read the temp in degrees C                ////
////                                                                    ////
////                      Pin Layout - SOT 23                           ////
////   -----------------------------------------------------------      ////
////   |                                                         |      ////
////   | 1: VSS    GND             | 6: DOUT  Serial Data Output |      ////
////   |                           |    __                       |      ////
////   | 2: DIN  Serial Data Input | 5: CS    Chip Select Input  |      ////
////   |                           |                             |      ////
////   | 3: VDD    +5V             | 4: SCLK  Serial Clock Input |      ////
////   |                                                         |      ////
////   -----------------------------------------------------------      ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2006 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C   ////
//// compiler.  This source code may only be distributed to other       ////
//// licensed users of the CCS C compiler.  No other use, reproduction  ////
//// or distribution is permitted without written permission.           ////
//// Derivative programs created using this software in object code     ////
//// form are not restricted in any way.                                ////
////////////////////////////////////////////////////////////////////////////


#ifndef AD7301_C
#define AD7301_C

#ifndef  ADT_DIN
#define  ADT_DIN   PIN_B0
#define  ADT_SCLK  PIN_B1
#define  ADT_DOUT  PIN_B2
#define  ADT_CS    PIN_B3
#endif

/////////////////////////////
////                     ////
//// Function Prototypes ////
////                     ////
/////////////////////////////

/*
int16 read_ADT()
This will read in values from the DOUT line on the ADT7301
Param: none
Returns: 14 data bits from the AD7301
*/
int16 read_ADT();


/*
int getTempF()
This will take the value read in from the ADT7301 and convert the value to Fahrenheit
Param: none
Returns: The temperature read from the ADT7301 in Fahrenheit
*/
int read_full_temp();

/*
int getTempC()
This will take the value read in from the ADT7301 and convert the value to Celsius
Param: none
Returns: The temperature read from the ADT7301 in Celsius
*/
int read_full_tempC();

//////////////////////////////////
////                          ////
//// Function Implementations ////
////                          ////
//////////////////////////////////

// Only for Compatibility!!!
void init_temp(void)
{
}

/*
int16 read_ADT()
This will read in values from the DOUT line on the AD7301
Param: none
Returns: 14 data bits from the AD7301
*/
int16 read_ADT()
{
   int i;
   int16 retval = 0;

   output_high(ADT_SCLK);
   output_low(ADT_CS);

   // ignore leading zero
   output_low(ADT_SCLK);
   delay_us(1);
   output_high(ADT_SCLK);

   // read in the 14 data bits
   for(i = 0; i < 15; i++)
   {
      output_low(ADT_SCLK);
      shift_left(&retval, 2, input(ADT_DOUT));
      output_high(ADT_SCLK);
   }

   output_high(ADT_CS);
   return retval;
}

/*
int read_full_temp()
This will take the value read in from the ADT7301 and convert the value to Fahrenheit
Param: none
Returns: The temperature read from the ADT7301 in Fahrenheit
*/
int read_full_temp()
{
   int data;

   // get the temperature in Celsius
   data = read_full_tempC();

   // check for negative value
   if(data & 0x80)
   {
      // 2's complement; get absolute value
      data = ~data;
      data++;

      // convert to Fahrenheit, notice the "- 32" because the data is actually negative
      data = (data * 1.8) - 32;

      // 2's complement; return to positive value
      data = ~data;
      data++;
   }

   else
      // convert the temperature to Fahrenheit the easy way
      data = (data * 1.8) + 32;

   return data;
}

/*
int read_full_tempC()
This will take the value read in from the ADT7301 and convert the value to Celsius
Param: none
Returns: The temperature read from the ADT7301 in Celsius
*/
int read_full_tempC()
{
   // read in the value from the chip
   int16 data;
   data = read_ADT();

   // check to see if the read value is negative
   if(data & 0x2000)
      data -= 16384;

   // convert from the ADT's bit code to Celsius
   data /= 32;

   return data;
}

#endif
