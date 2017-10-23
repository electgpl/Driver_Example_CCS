///////////////////////////////////////////////////////////////////////////
////   Library for an electronic compass using the PNI11096            ////
////                                                                   ////
////   void compass_init();                                            ////
////     Call before the other functions are used                      ////
////                                                                   ////
////   void compass_calibrate();                                       ////
////     The compass must be calibrated before it is used.             ////
////     Calibration only needs to occur once.                         ////
////     Connect the RS232 port to a computer and follow instructions. ////
////     The calibration values will be saved to eeprom locations 0-4  ////
////     * Notes:                                                      ////
////      - Set ICD to "Erase when needed" to prevent the calibration  ////
////        values from being lost when reprogramming a chip.          ////
////      - Declination values can be found at the National            ////
////        Geophysical Data Center (NGDC) website.                    ////
////      - Current website address:                                   ////
////        http://www.ngdc.noaa.gov/cgi-bin/seg/gmag/declination1.pl  ////
////      - A declination in degrees east is a positive value.         ////
////      - A declination in degrees west is a negative value.         ////
////                                                                   ////
////   int16 compass_getAngle();                                       ////
////     Call to get the direction the compass is pointing             ////
////      North = 0 degrees      South = 180 degrees                   ////
////      East  = 270 degrees    West  = 90 degrees                    ////
////                                                                   ////
////  The main program may redefine the eeprom addresses used to save  ////
////  the compass calibration values. See defines below.               ////
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

#ifndef ELECTRONIC_COMPASS
#define ELECTRONIC_COMPASS


///////////////////////////////////////////////////////////////////////////
//// Calibration value eeprom addresses
///////////////////////////////////////////////////////////////////////////
#ifndef X_CALIBRATE_EEPROM_ADDRESS
#define X_CALIBRATE_EEPROM_ADDRESS  0
#endif
#ifndef Y_CALIBRATE_EEPROM_ADDRESS
#define Y_CALIBRATE_EEPROM_ADDRESS  2
#endif
#ifndef DECLINATION_EEPROM_ADDRESS
#define DECLINATION_EEPROM_ADDRESS  4
#endif
///////////////////////////////////////////////////////////////////////////


#include <stdlib.h>
#include <input.c>
#include <math.h>
#include <internal_eeprom.c>
#include <pni11096.c>


// Stores the calibration values
signed int16   xc          = 0;
signed int16   yc          = 0;
signed int8    declination = 0;


// Purpose:    Initializes the magneto sensor and loads calibration values
//             from eeprom. Call this before the other functions are used.
// Inputs:     None
// Outputs:    None
void compass_init()
{
   // Initialize the magneto to measure magnetic fields
   init_magneto();

   // Load saved calibration values
   xc = read_int16_eeprom(X_CALIBRATE_EEPROM_ADDRESS);
   yc = read_int16_eeprom(Y_CALIBRATE_EEPROM_ADDRESS);
   declination = read_eeprom(DECLINATION_EEPROM_ADDRESS);
}


// Purpose:    Find the direction the compass is pointing and return the angle
// Inputs:     None
// Outputs:    A 16 bit integer containing 0 to 359
//             North = 0 Degrees       South = 180 Degrees
//             East  = 270 Degrees     West  = 90 Degrees
int16 compass_getAngle()
{
   signed int16 angle;
   signed int16 x, y;

   // Get the magnitude of the x and y magnetic fields
   x = ((read_magneto(MAG_XAXIS | MAG_DIV_512)) - xc);
   y = ((read_magneto(MAG_YAXIS | MAG_DIV_512)) - yc);

   // Find the angle of the resultant vector
   angle = (atan((float)y/(float)x) * (180.0 / PI)) + declination;

   // Convert the angle to 0 to 360 degree scale
   if(x > 0)
   {
      angle += 180;
   }
   else if(x < 0 && angle < 0)
   {
      angle += 360;
   }

   return angle;
}


// Purpose:    Call this function to calibrate the compass. The calibration
//             values will be stored in eeprom locations 0 - 4. The compass
//             initialization function reads these values from eeprom, so the
//             compass only has to be calibrated once. Read the description
//             below for setting the declination.
// Inputs:     None
// Outputs:    None
void compass_calibrate()
{
   signed int32 x1, y1, x2, y2;

   // Prompt for north and store the angle
   printf("Point north, then press any key\n\r");
   getc();
   // Get the magnitude of the x and y magnetic fields
   x1 = read_magneto(MAG_XAXIS | MAG_DIV_512);
   y1 = read_magneto(MAG_YAXIS | MAG_DIV_512);

   // Prompt for south and store the angle
   printf("Point south, then press any key\n\r");
   getc();
   // Get the magnitude of the x and y magnetic fields
   x2 = read_magneto(MAG_XAXIS | MAG_DIV_512);
   y2 = read_magneto(MAG_YAXIS | MAG_DIV_512);

   // Calculate the calibration values
   xc = (x1 + x2) / 2;
   yc = (y1 + y2) / 2;


   // * Declination values can be found at the
   //   National Geophysical Data Center (NGDC) website.
   // * Current website address:
   //   http://www.ngdc.noaa.gov/cgi-bin/seg/gmag/declination1.pl
   // * A declination in degrees east is a positive value
   //   and a declination in degrees west is a negative value
   printf("Degress of declination (-West, +East): ");
   declination = get_int();
   printf("\n\r");

   // Store the calibration values to eeprom for later use
   write_int16_eeprom(X_CALIBRATE_EEPROM_ADDRESS, xc);
   write_int16_eeprom(Y_CALIBRATE_EEPROM_ADDRESS, yc);
   write_eeprom(DECLINATION_EEPROM_ADDRESS, declination);
}

#endif

