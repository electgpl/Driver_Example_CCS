/////////////////////////////////////////////////////////////////////////
////                     EX_CENTER_ALIGNED_PWM.C                     ////
////                                                                 ////
////  This program will show how to setup and use the Output Compare ////
////  with Dedicated Timers Module in Edge-Aligned PWM mode. The     ////
////  program takes an analog input and uses the digital value to    ////
////  set the duty cycle.  The frequency is set by the user over the ////
////  RS-232.                                                        ////
////                                                                 ////
////  Configure the PIC24FV32KA302 as follows:                       ////
////     Connect a scope to pin B7                                   ////
////     Connect pin A0 to output of the POT                         ////
////     Connect pin B0 to RS232 TX, pin B1 to RS232 RX              ////
////                                                                 ////
////  This example will work with the PCD compilers. The following   ////
////  conditional compilation lines are used to include a valid      ////
////  device for each compiler.  Change the device, clock and RS232  ////
////  pins for your hardware if needed.                              ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2012 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#include <24FV32KA302.h>
#fuses NOWDT
#device ADC=12
#use delay(internal=16MHz)
#use rs232(baud=9600, UART2)

void main(void) 
{

   /* PWM EXAMPLE FOR 16-BIT PICS */
   
   char selection;
   unsigned int16 value;
   unsigned int16 duty;

   printf("\r\nFrequency:\r\n");
   printf("    1) 125 hz\r\n");
   printf("    2) 15.63  hz\r\n");
   printf("    3) 1.95  hz\r\n");
   
   do {
     selection=getc();
   } while((selection<'1')||(selection>'3'));

          //   The cycle time will be (1/clock)*2*t2div*(period+1)
          //   In this program clock=16000000 and period=64000 (below)
          //   For the three possible selections the cycle time is:
          //     (1/16000000)*2*1*64000  =  8 ms or 125 hz
          //     (1/20000000)*2*8*64000  = 64 ms or 15.63  hz
          //     (1/20000000)*2*64*64000 = 0.512 ms or 1.95  hz

   switch(selection) {
     case '1' : setup_timer2(TMR_INTERNAL | TMR_DIV_BY_1, 64000);
                break;
     case '2' : setup_timer2(TMR_INTERNAL | TMR_DIV_BY_8, 64000);
                break;
     case '3' : setup_timer2(TMR_INTERNAL | TMR_DIV_BY_64, 64000);
                break;
   }

  setup_adc_ports(sAN0, VSS_VDD);
  setup_adc(ADC_CLOCK_INTERNAL | ADC_TAD_MUL_31);
  set_adc_channel(0);
  printf("%c\r\n",selection);
  
  setup_compare(1, COMPARE_PWM_EDGE | COMPARE_TIMER2 | COMPARE_TRIG_SYNC_TIMER2);
  
  while(TRUE) 
  {
    value=read_adc();
    duty = ((unsigned int32)value * 100) / 4096;
    
    printf("\rADC Duty: %3u%%",duty);
    
    /* Set the duty cycle using the adc reading.  */
    set_pwm_duty(1,((unsigned int32)value*64000)/4096);
  }

}
