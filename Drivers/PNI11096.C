///////////////////////////////////////////////////////////////////////////
////   Library for a PNI11096                                          ////
////     3-Axis Magneto-Inductive Sensor Driver and Controller         ////
////     with SPI Serial Interface                                     ////
////                                                                   ////
////   init_magneto();                                                 ////
////     Call before the read function is used                         ////
////                                                                   ////
////   signed int16 read_magneto(AXIS | DIVISION_RATIO);               ////
////     Call to read a value from an axis                             ////
////                                                                   ////
////                                                                   ////
////   The main program may define MAG_SS, MAG_DRDY, MAG_RESET,        ////
////   MAG_CLK, MAG_MISO, and MAG_MOSI to override the defaults below. ////
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

#ifndef PNI11096
#define PNI11096


#ifndef MAG_SS

#define MAG_SS       PIN_B2
#define MAG_DRDY     PIN_B4
#define MAG_RESET    PIN_B5
#define MAG_CLK      PIN_C3
#define MAG_MISO     PIN_C4
#define MAG_MOSI     PIN_C5

#endif

////////////////////////////////////////////////////////////////////////////////
// Use one of the following defines to specify which axis to read
#define MAG_XAXIS       0x01
#define MAG_YAXIS       0x02
#define MAG_ZAXIS       0x03

// The following division ratios can be ORed with an axis.
// The default is 32, but the recommended is 512.
#define MAG_DIV_32      0x00
#define MAG_DIV_64      0x10
#define MAG_DIV_128     0x20
#define MAG_DIV_256     0x30
#define MAG_DIV_512     0x40
#define MAG_DIV_1024    0x50
#define MAG_DIV_2048    0x60
#define MAG_DIV_4096    0x70

// The following diagnostic commands can be ORed with an axis
#define MAG_SHOW_CLK    0x80  // * Show the clock on DHST
                              //   (SOIC PIN_27, DIE PIN_25, MLF PIN_24)
#define MAG_CONTINUOUS  0x04  // * Continuously run the oscillator on an axis
#define MAG_OSC_DIR     0x08  // * Change the oscillator direction
                              //   (Only works when MAG_CONTINUOUS is used)
////////////////////////////////////////////////////////////////////////////////

// Purpose:    Initialize the pins used by the magneto-inductive sensor
void init_magneto()
{
   output_high(MAG_SS);
   output_low(MAG_RESET);
   output_low(MAG_CLK);
}

// Purpose:    Read the magnetic intensity and direction for a given axis
// Inputs:     The axis to read ORed with any settings
// Outputs:    A signed 16 bit integer representing
//             magnetic intensity and direction
signed int16 read_magneto(int8 axis)
{
   int i;
   signed int16 data;

   output_low(MAG_CLK);         // Ensure the clock is low
   output_low(MAG_SS);          // Select the sensor
   delay_us(1);
   output_high(MAG_RESET);      // Reset the sensor
   delay_us(1);
   output_low(MAG_RESET);

   for(i=0; i<8; ++i)
   {
      output_bit(MAG_MOSI, shift_left(&axis, 1, 0));
      delay_us(1);

      output_high(MAG_CLK);     // Clock out a bit of data
      delay_us(1);
      output_low(MAG_CLK);
   }

   while(!input(MAG_DRDY));     // Wait for the measurement

   for(i=0; i<16; ++i)          // Read the measurement
   {
      output_high(MAG_CLK);     // Clock in a bit of data
      delay_us(1);
      shift_left(&data, 2, input(MAG_MISO));
      output_low(MAG_CLK);
      delay_us(1);
   }

   output_high(MAG_SS);         // Deselect the sensor

   return data;
}

#endif
