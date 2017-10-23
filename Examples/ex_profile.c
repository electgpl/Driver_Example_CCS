/////////////////////////////////////////////////////////////////////////
////                           EX_PROFILE.C                          ////
////                                                                 ////
////  This program shows how use the IDE profile tool to time        ////
////  functions and to output real-time time from your program.      ////
////                                                                 ////
////  Click built-and-run in the IDE to compile and start the        ////
////  profile tool.  To start click on the setup page (left side)    ////
////  and for statistics check the "Show profileout counts"          ////
////      for data msgs  check the "Highlight changed lines"         ////
////      for call sequ check all the boxes                          ////
////  Click RUN to start the program running and change the page     ////
////  view to see the data.                                          ////
////                                                                 ////
////  If you are using a slower clock try the following line instead:////
////        #use profile(TIMER1, baud=19200)                         ////
////                                                                 ////
////  This example uses the TIMER1 feature for the best timing data. ////
////  You can remove that option if you use TIMER1 for something     ////
////  else.                                                          ////
////                                                                 ////
////  This example will work with the PCB,PCM,PCH and PCD compilers. ////
////  The following conditional compilation lines are used to        ////
////  include a valid device for each compiler.  Change the device,  ////
////  clock for your hardware if needed.                             ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 2013,2013 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#if defined(__PCB__)
#include <16F59.h>
#elif defined(__PCM__)
#include <16F887.h>
#elif defined(__PCH__)
#include <18F4520.h>
#endif

#use delay(crystal=20mhz)

#use profile(TIMER1)

#define INTS_PER_SECOND 125        // (20000000/(4*16*250*10))

int16 seconds;  
int8  minutes;
int8  int_count;    

#profile functions,parameters,profileout

#INT_TIMER2                       
void clock_isr() {               
                                   
    if(--int_count==0) {        
      ++seconds;
      int_count = INTS_PER_SECOND;
    }
}


int8 seconds_to_minutes(int16 secs) {     // Use the call sequence view to
   return secs / 60;                      // see this parameter value passed in  
}

void seconds_changed(void) {
    minutes = seconds_to_minutes(seconds);
    profileout(seconds);                  // This shows the three basic profileout
                                          // formats.  They show up under both statistics
    if((seconds % 10)==0) {               // and data messages.
       profileout("Ten second mark");
       profileout("Minutes is ",minutes);
    }
}


void main(void) {
   int16 last_second=0;
   
   int_count = INTS_PER_SECOND;
   setup_timer_2(T2_DIV_BY_16, 250, 10);
   set_timer2(0);
   enable_interrupts(INT_TIMER2);
   enable_interrupts(GLOBAL);
   
   profileout("Starting");

   while(true) {
      if(seconds!=last_second) {      // The processing time for this called
          seconds_changed();          // function is shown in the Statistics view. 
          last_second=seconds;
      }
   }
}

