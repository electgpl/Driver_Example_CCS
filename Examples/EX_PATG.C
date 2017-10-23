/////////////////////////////////////////////////////////////////////////
////                         EX_PATG.C                               ////
////                                                                 ////
////  This program will output multiple square waves from the pins   ////
////  of port b.  The waves can be differnt frequencies, and can be  ////
////  seen by hooking up a scope to any of the pins.                 ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////     Connect any of pins B0 through B7 to a scope.               ////
////                                                                 ////
////  This example will work with the PCM and PCH compilers.  The    ////
////  following conditional compilation lines are used to include a  ////
////  valid device for each compiler.  Change the device and clock   ////
////  for your hardware if needed.                                   ////
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

#elif defined(__PCH__)
#include <18F452.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#endif

#define NUM_OUTPUTS  7

//NOTE:  periods MUST be multiples of 400
//Periods are in microseconds
#define  PERIOD_0 400
#define  PERIOD_1 800
#define  PERIOD_2 1600
#define  PERIOD_3 2000
#define  PERIOD_4 20000
#define  PERIOD_5 64000
#define  PERIOD_6 2000000


const long wave_period[NUM_OUTPUTS] = {
            PERIOD_0/400, PERIOD_1/400, PERIOD_2/400, PERIOD_3/400,
            PERIOD_4/400, PERIOD_5/400, PERIOD_6/400};

long counter[NUM_OUTPUTS] = {0,0,0,0,0,0,0};

int port_b_image;


// This interrupt is used to output the waveforms.  The interrupt
// is automatically called ever 200us.
#INT_TIMER1
void wave_timer() {
   int i;

   set_timer1(0xFC4F);                       // sets timer to interrupt in 200us
   output_b(port_b_image);                   // outputs the waveform

   for(i=0; i<NUM_OUTPUTS; i++)              // sets up next output for each pin
   {
      if((++counter[i]) == wave_period[i])   // if counter is expired
      {
         counter[i] = 0;                     // reset counter
         if(bit_test(port_b_image,i))        // and set pin as needed
            bit_clear(port_b_image,i);
         else
            bit_set(port_b_image,i);
      }
   }
}


void main()   {

   setup_timer_1(T1_INTERNAL|T1_DIV_BY_1);   // setup interrupts
   enable_interrupts(INT_TIMER1);
   enable_interrupts(GLOBAL);

   port_b_image=0;                           // initialize variable
   output_b(port_b_image);

   while(TRUE);                              // loop forever
}
