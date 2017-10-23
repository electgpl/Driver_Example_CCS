/////////////////////////////////////////////////////////////////////////
////                             EX_Capture.C                        ////
////                                                                 ////
////  This example uses the Input Capture Module to measure the      ////
////  period of an input signal.                                     ////
////                                                                 ////
////  The code will enable the IC1 interrupt on the RD8 pin of the   ////
////  24FJ128GA006 chip (Pin 42).                                    ////
////                                                                 ////
////  The Input Capture Module is setup to capture the Timer 2 value ////
////  on ever falling edge of the input signal.  The module is also  ////
////  setup to generate an interrupt on every second capture event.  ////
////  The interrupt service routine reads both capture times from    ////
////  the input capture FIFO buffer and sets a flag indicating that  ////
////  new capture times are available.  Then in main when the flag   ////
////  is set the new values are used to calculate the signal's       ////
////  period and it is diplayed using the UART1 module.              ////
////                                                                 ////
////  The minimum and maximum periods that this example can measure  ////
////  depends of the setup of the timer used with the input capture  ////
////  module and the PIC's clock frequency.  This example setups up  ////
////  the timer 2 module with a prescalar of 64, with a clock        ////
////  frequency of 20MHz this example can measure periods of the     ////
////  input signal from 6.4us to 0.419424 seconds in increments of   ////
////  6.4 us.  Adjust the timer's prescalar and clock frequency to   ////
////  measure shorter or longer periods.                             ////
////                                                                 ////
////  This example also has the option of generating a PWM signal    ////
////  using the Output Compare Module that the Input Capture Module  ////
////  can measure.  When the define GENERATE_PWM_SIGNAL is used the  ////
////  example will generate a PWM that changes period every 5        ////
////  seconds for a total of ten different periods that can be       ////
////  messured by the Input Capture Module.                          ////
////                                                                 ////
////  Configure the CCS prototype board as follows:                  ////
////     When using the define GENERATE_PWM_SIGNAL:                  ////
////        Insert a jumper from pin D8 to pin D0                    ////
////                                                                 ////
////     When not using the define GENERATE_PWM_SIGNAL:              ////
////        Connect the input signal to pin D8                       ////
////                                                                 ////
////  This example will work only for the PCD Compiler               ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2011 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////


#if !defined(__PCD__)
#error This example will only compile for the PCD 24 bit compiler
#endif
#include <24FJ128GA006.h>
#fuses NOWDT, NOPROTECT
#use delay(crystal=20000000)
#use rs232(baud=57600, UART1)

#define GENERATE_PWM_SIGNAL

#ifdef GENERATE_PWM_SIGNAL

typedef unsigned int16 TICK_TYPE;
#define TICKS_PER_SECOND 1

TICK_TYPE Timer1Overflow = 0;

#INT_TIMER1
void timer1_isr(void)
{
   Timer1Overflow++;
}

TICK_TYPE GetTickDifference(TICK_TYPE current, TICK_TYPE previous)
{
   return(current - previous);
}

TICK_TYPE GetTick(void)
{
   return(Timer1Overflow);
}
#endif

unsigned int16 FirstCapture, SecondCapture;
int1 NewCapture = FALSE;

#int_IC1
void IC1_Interrupt(void)
{
   unsigned int16 dummy;

   if(!NewCapture)
   {
      FirstCapture = get_capture(1,FALSE);   //Read first capture event.
      SecondCapture = get_capture(1,FALSE);  //Read second capture event.
      NewCapture = TRUE;
   }
   else
   {
      dummy = get_capture(1,TRUE);  //The TRUE causes the last capture event to 
                                    //be retrieved from the capture FIFO buffer  
                                    //and to clear out any previous captures. 
                                    //This is used to keep the FIFO buffer from 
                                    //overflowing while calculating and 
                                    //displaying the time.  If an overflow 
                                    //occurs it will cause any further Input
                                    //Capture interrupts form occuring.
   }  
}

unsigned int16 GetCaptureDifference(unsigned int16 first, unsigned int16 second)
{
   return(second - first);
}

#ifdef GENERATE_PWM_SIGNAL
//The following are values used to generate 10 different period pwm signals for 
//the Input Capture to  measure.  With a 20MHz clock the periods are 64us, 640us,
//6400us, 12800us, 32000us, 64000us, 89600us, 128000us, 320000us, and 384000us.
unsigned int16 OC1Period[10] =   {9, 99, 999, 1999, 4999, 9999, 13999, 19999, 49999, 59999};
unsigned int16 OC1Duty[10] =     {5, 50, 500, 1000, 2500, 5000, 7000,  10000, 20000, 30000};
#endif

void main()
{
   TICK_TYPE CurrentTick, PreviousTick;
   unsigned int16 Time;
   unsigned int8 i = 0;

   printf("\n\rRESET\n\n\r");
   
   //The following sets up the Input Capture 1 unit to capture timer 2 on the 
   //falling edge, high to low, of the input signal, and to generate an
   //interrupt on every second capture event.
   SETUP_CAPTURE(1, CAPTURE_FE | INTERRUPT_SECOND_CAPTURE | CAPTURE_TIMER2);  
   setup_timer2(TMR_INTERNAL|TMR_DIV_BY_64,0xFFFF);   //enable Timer 2
   
   enable_interrupts(INT_IC1);      //enable 
   enable_interrupts(INTR_GLOBAL);
   
  #ifdef GENERATE_PWM_SIGNAL
   //The following sets up the Output Compare 1 unit to generate a pwm signal 
   //that this example can use to measure. 
   setup_compare(1, COMPARE_PWM | COMPARE_TIMER3);
   setup_timer3(TMR_INTERNAL|TMR_DIV_BY_64,OC1Period[i]);   //enable Timer3
   set_pwm_duty(1,OC1Duty[i]);
   
   setup_timer1(TMR_INTERNAL|TMR_DIV_BY_256,39062);   //interrupt every second
   enable_interrupts(INT_TIMER1);      //enable Timer 1 interrupt
   
   PreviousTick = CurrentTick = GetTick();
  #endif
   
   while (TRUE) 
   {
     #ifdef GENERATE_PWM_SIGNAL
      CurrentTick = GetTick();
      
      if(GetTickDifference(CurrentTick, PreviousTick) >= 5*TICKS_PER_SECOND)  //change PWM period every 5 seconds
      {
         if(++i >= 10)
            i = 0;
            
         setup_timer3(TMR_INTERNAL | TMR_DIV_BY_64,OC1Period[i]);
         set_pwm_duty(1,OC1Duty[i]);
         
         PreviousTick = CurrentTick;
      }
     #endif
   
      if(NewCapture)
      {
         Time = GetCaptureDifference(FirstCapture, SecondCapture);
         
         printf("\r\rPeriod: %8.1w microseconds",((unsigned int32)Time*64));
         
         NewCapture = FALSE;
      }
   }
}
