/////////////////////////////////////////////////////////////////////////
////                          EX_TANK.C                              ////
////                                                                 ////
////  This example uses a trig function to calculate the volume of   ////
////  a fluid in a tank.  The tank is in the shape of a large        ////
////  propane tank.  The ends are half of an ellipsoid and the       ////
////  main body part is a horizontal cylinder.                       ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////                                                                 ////
////  This example will work with the PCM and PCH compilers.  The    ////
////  following conditional compilation lines are used to include a  ////
////  valid device for each compiler.  Change the device, clock and  ////
////  RS232 pins for your hardware if needed.                        ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////


#if defined(__PCM__)
#include <16F877.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCH__)
#include <18F458.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#endif

#include <math.h>
#include <input.c>
#include <stdlib.h>

#define  NUM_LENGTH 8


main()   {

   float volume, cap_vol, tube_vol, diameter, length=1, cap_length,
         height, offset_height, tube_length, temp1;
   char  string[NUM_LENGTH];
   short error=FALSE;

   while(TRUE)
   {
      do {
         if(error)
            printf("\r\nERROR MADE, PLEASE ENTER VALID NUMBERS!!");
         printf("\r\n\n***NOTE:  ALL DIMENSIONS ARE IN METERS***\r\n");
         printf(  "Enter dimensions of tank...
                   \r\n     Overall length:  ");
         get_string(string,NUM_LENGTH);
         length = atof(string);

         printf(  "\r\n     Cap length:  ");
         get_string(string,NUM_LENGTH);
         cap_length = atof(string);

         printf(  "\r\n     Diameter of tank:  ");
         get_string(string,NUM_LENGTH);
         diameter = atof(string);

         printf("\r\n\nEnter the height of the fluid:  ");
         get_string(string,NUM_LENGTH);
         height = atof(string);

         tube_length = length - 2*cap_length;
         if(height>diameter||length<=0||cap_length<0||height<0||tube_length<0)
            error = TRUE;
         else
            error = FALSE;
      }
      while(error);

      offset_height = height - diameter/2;

      temp1 = pwr(offset_height,3) - 3*diameter*diameter*offset_height/4
               - pwr(diameter,3)/4;
      cap_vol = -PI*cap_length*temp1/(3*diameter);

      temp1 = sqrt(diameter*diameter/4-offset_height*offset_height);
      temp1 = PI/2 + 4*offset_height*temp1/(diameter*diameter)
               + asin(2*offset_height/diameter);
      tube_vol = tube_length*diameter*diameter*temp1/4;

      volume = tube_vol + 2*cap_vol;

      printf("\r\n\nTotal volume of water is:  %f gallons\n\n", volume*264.2);
   }
}
