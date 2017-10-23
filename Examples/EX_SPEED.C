/////////////////////////////////////////////////////////////////////////
////                            EX_SPEED.C                           ////
////                                                                 ////
////  This example uses two photo diodes (PN334PA-ND) to time the    ////
////  motion of an  object across the diodes.  By entering in the    ////
////  distance between the diodes, the program can calculate the     ////
////  speed of the object in miles per hour.  NOTE:  Wrapping the    ////
////  diodes in paper to make a tube will greatly help channel the   ////
////  detectors and will improve the results.                        ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////     GND to short end of both photo diodes                       ////
////     Pin A2 to long end of left photo diode                      ////
////     Pin A3 to long end of right photo diode                     ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////                                                                 ////
////  This example will work with the PCM and PCH compilers.  The    ////
////  following conditional compilation lines are used to include a  ////
////  valid device for each compiler.  Change the device, clock and  ////
////  RS232 pins for your hardware if needed.                        ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2006 Custom Computer Services         ////
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
#include <18F452.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#endif

#include <stdlib.h>

// The calibration offset is how much lower the A/D converter must get below
// the reference value in order for it to trigger.  1 = 0.0195V
#define CALIBRATION_OFFSET    2


void main()   {
   int ref1,ref2;
   long time;
   float distance,mph;
   char s_dist[6];


   printf("\r\n\n\nThis program times movement across the two sensors.\r\n");
   printf("Note that this must take place in less than 800ms.\r\n\n");

   // Setup ADC and Timer1
   setup_adc_ports(ALL_ANALOG);
   setup_adc(ADC_CLOCK_INTERNAL);
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_8);

   printf("Type in the distance in inches between the two sensors and push enter.\r\n");
   gets(s_dist);

   // get distance between sensors
   distance = atof(s_dist);
   printf("\r\ndistance = %f\r\n",distance);

   while(TRUE)
   {
      // Calibrate the sensors
      printf("\r\nCalibrating the sensors, please don't cover them!\r\n");

      set_adc_channel(2);
      delay_ms(500);
      ref2=read_adc();

      set_adc_channel(1);
      delay_ms(1);
      ref1=read_adc();

      printf("Calibration done.\r\n");
      printf("Sweep your hand from the left to the right across the sensors.\r\n");

      set_adc_channel(2);
      delay_ms(1);

      // wait for the first sensor to be triggered
      while(read_adc()>(ref2-CALIBRATION_OFFSET))
         delay_us(5);
      set_timer1(0);                      // start counting
      set_adc_channel(1);                 // switch to other sensor
      delay_us(10);

      // wait for the second sensor to be triggered
      while(read_adc()>(ref1-CALIBRATION_OFFSET))
         delay_us(5);
      time=get_timer1();                  // get the time

      mph = 0.0000016*(float)time/3600.0;  // calculate the time in hours
      mph = distance/(63360.0*mph);

      printf("Your speed was %3.2f mph.\r\n",mph);

      delay_ms(1000);                     // wait until diodes are uncovered again
   }
}


