/////////////////////////////////////////////////////////////////////////
////                       EX_MOTOR_PWM.C                            ////
////                                                                 ////
////  This program will show how to use the built in MOTOR PWM       ////
////  functions available on dsPIC33FJxxxMCxxx PICs.  This program   ////
////  sets up four PWM pairs for a total of 8 PWM outputs. PWM pairs ////
////  1, 2, 3 are setup to operate in complementary mode, and PWM    ////
////  pair 4 is setup to operate in independent mode.                ////
////                                                                 ////
////  PWM pair 1's duty cycle is setup to be adjustable by using the ////
////  analog reading of AN0 to set the duty. PWM pair 2, 3, and 4    ////
////  duty cycle is set to a fixed 25%, 50%, and 75%.                ////
////                                                                 ////
////  PWM pair 1 and 4 have an active and inactive dead-time set to  ////
////  100 instructions, 10 us with a 20 MHz input clock.  PWM pair 2 ////
////  and 4 have an active and inactive dead-time set to 30          ////
////  instructions, 3 us with a 20 MHz input clock.                  ////
////                                                                 ////
////  PWM pair 1 also has fault A enabled to cause the 1L to go low  ////
////  and 1H to go high if a fault occurs, line goes low, on Fault   ////
////  A, PIN_D8 on the dsPIC33FJ128MC706.                            ////
////                                                                 ////
////  This example will work with the PCD compiler.  Change the      ////
////  device, clock and RS232 pins for your hardware if needed.      ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2010 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#include <33FJ128MC706.h>
#fuses PR,HS,NOWDT
#device ADC=12
#use delay(clock=20MHz)
#use rs232(UART1,baud=9600)

void main(void)
{
   unsigned int16 CurrADCReading, PrevADCReading;

   delay_ms(100);

   printf("\n\rCCS EX_MOTOR_PWM.c");
   
   // Enable PWM module 1 in Free Running Mode with a
   // prescalar of 4.  For a period of approximately
   // 13.1 ms with a 20 MHz input clock.
   setup_motor_pwm(1,MPWM_FREE_RUN,4,0,0x7FFF);
                                                
   // Enable 1L and 1H in complementary mode using Fault A and                                            
   // an active and inactive dead-time of 100 instructions,
   // 10 us for a 20 MHz input clock.  If Fault A goes low, 1L
   // goes inactive (low) and 1H goes active (high).
   set_motor_unit(1,1,MPWM_ENABLE | MPWM_FAULTA_LI_HA,100,100);
   
   // Enable 2L and 2H in complementary mode with an active
   // and inactive dead-time of 30 instructions.
   set_motor_unit(1,2,MPWM_ENABLE,30,30); 
   
   // Enable 2L and 2H in complementary mode with an active
   // and inactive dead-time of 30 instructions.
   set_motor_unit(1,3,MPWM_ENABLE,30,30);
   
   //Enable 4L and 4H in independent mode with an active and
   //inactive dead-time of 100 instructions.
   set_motor_unit(1,4,MPWM_ENABLE | MPWM_INDEPENDENT,100,100);
                                                                  
   set_motor_pwm_duty(1,1,0x7FFF);  //Set pin pair 1 to a 50% duty cycle.
   set_motor_pwm_duty(1,2,0x3FFF);  //Set pin pair 2 to a 25% duty cycle.
   set_motor_pwm_duty(1,3,0x7FFF);  //Set pin pair 3 to a 50% duty cycle.
   set_motor_pwm_duty(1,4,0xBFFF);  //Set pin pair 4 to a 75% duty cycle.
   
   setup_adc(ADC_CLOCK_INTERNAL | ADC_TAD_MUL_31);
   setup_adc_ports(sAN0,VSS_VDD);
   set_adc_channel(0);
   
   PrevADCReading = 0x7FFF;
   
   while(1)
   {
      CurrADCReading = read_adc() << 4;
      
      if(CurrADCReading != PrevADCReading)
      {
         set_motor_pwm_duty(1,1,CurrADCReading);
         PrevADCReading = CurrADCReading;
      }
   }
}
