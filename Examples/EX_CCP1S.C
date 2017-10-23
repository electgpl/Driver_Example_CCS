/////////////////////////////////////////////////////////////////////////
////                           EX_CCP1S.C                            ////
////                                                                 ////
////  This program will show how to use the built in CCP or IC/OC    ////
////  to generate a single pulse of a predefined time in response    ////
////  to a pushbutton.                                               ////
////                                                                 ////
////  Configure the CCS prototype card as follows for PCM and PCH:   ////
////      Connect a scope to pin C2                                  ////
////      Connect pushbutton to pin B0                               ////
////  Configure the CCS prototype card as follows for PCD:           ////
////      Connect a scope to pin D0                                  ////
////      Use pushbutton on pin B1                                   ////
////                                                                 ////
////  This example will work with the PCM, PCH, and PCD compilers.   ////
////  The following conditional compilation lines are used to        ////
////  include a valid device for each compiler.  Change the device   ////
////  and clock for your hardware if needed.                         ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2008 Custom Computer Services         ////
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

#elif defined(__PCD__)
#include <30f2010.h>
#fuses HS,NOWDT,NOPROTECT
#use delay(clock=20000000)
#endif

void main()
{
   #if defined(__PCD__)
   setup_compare(1, COMPARE_CLR_ON_MATCH);      // Configure CCP1 in COMPARE mode
   setup_timer2(TMR_INTERNAL);                  // Set up timer to instruction clk
   
   while(TRUE)
   {
      while(input(PIN_B1));                     // Wait for keypress
      setup_compare(1, COMPARE_SET_ON_MATCH);   // Configure OC1 to set
      set_timer2(0);

      set_compare_time(1, 500);                 // Set high time limit                                              
                                                // to 100 us
                                                // limit is time/(clock/4)
                                                // 500 = .0001*(20000000/4)

      setup_compare(1, COMPARE_CLR_ON_MATCH);   // Configure OC1 to pull
                                                // pin C2 low on a match
                                                // with timer1

      delay_ms(1000);                           // Debounce - Permit only one pulse/sec
   }
      
   #else
   setup_ccp1(CCP_COMPARE_CLR_ON_MATCH);        // Configure CCP1 in COMPARE mode
   setup_timer_1(T1_INTERNAL);                  // Set up timer to instruction clk

   while(TRUE)
   {
       while(input(PIN_B0)) ;                   // Wait for keypress

       setup_ccp1(CCP_COMPARE_SET_ON_MATCH);    // Configure CCP1 to set
       CCP_1=0;                                 // C2 high now
       set_timer1(0);

       CCP_1 = 500;                             // Set high time limit
                                                // to 100 us
                                                // limit is time/(clock/4)
                                                // 500 = .0001*(20000000/4)

       setup_ccp1(CCP_COMPARE_CLR_ON_MATCH);    // Configure CCP1 in COMPARE
                                                // mode and to pull pin C2
                                                // low on a match with timer1

       delay_ms(1000);                          // Debounce - Permit only one pulse/sec
   }
   #endif
}
