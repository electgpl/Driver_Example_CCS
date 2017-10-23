/////////////////////////////////////////////////////////////////////////
////                         EX_PWM_PCD.C                            ////
////                                                                 ////
////  This program will show how to use the built in PIC PWM.        ////
////  The program takes an analog input and uses the digital         ////
////  value to set the duty cycle.  The frequency is set by          ////
////  the user over the RS-232.                                      ////
////                                                                 ////
////  Configure the CCS PCD 30F2010 development board as follows:    ////
////      Connect a scope to pin D0                                  ////
////                                                                 ////
////  This example will work with the PCD compiler.  Change the      ////
////  device, clock and RS232 pins for your hardware if needed.      ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2013 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#include <30F2010.h>
#fuses HS,NOWDT
#device ADC=10
#use delay(clock=20000000)
#use rs232(baud=9600, UART1A)

void main(void)
{

   /* PWM EXAMPLE FOR 16-BIT PICS */

   char selection;
   unsigned int16 value;

   printf("\r\nFrequency:\r\n");
   printf("    1) 76.37 hz\r\n");
   printf("    2) 9.55  hz\r\n");
   printf("    3) 1.19  hz\r\n");

   do {
     selection=getc();
   } while((selection<'1')||(selection>'3'));

          //   The cycle time will be (1/clock)*4*t2div*(period+1)
          //   In this program clock=20000000 and period=65472 (below)
          //   For the three possible selections the cycle time is:
          //     (1/20000000)*4*1*65473  =  13.1 ms or 76.37 Hz
          //     (1/20000000)*4*8*65473  = 104.7 ms or 9.55  Hz
          //     (1/20000000)*4*64*65473 = 838.1 ms or 1.19  Hz

   switch(selection) {
     case '1' : setup_timer2(TMR_INTERNAL | TMR_DIV_BY_1, 0xFFC0);
                break;
     case '2' : setup_timer2(TMR_INTERNAL | TMR_DIV_BY_8, 0xFFC0);
                break;
     case '3' : setup_timer2(TMR_INTERNAL | TMR_DIV_BY_64, 0xFFC0);
                break;
   }

  setup_adc_ports(sAN0);
  setup_adc(adc_clock_internal);
  set_adc_channel(0);
  printf("%c\r\n",selection);

  setup_compare(1, COMPARE_PWM | COMPARE_TIMER2);

  while(TRUE)
  {
    value=read_adc();

    printf("%3LX\r",value);

    /* set the duty cycle using the adc reading.  the adc reading is in 10 bits,
      the duty cycle is in 16 bits, multiplying by 64 brings adc up to 16 bits */
    set_pwm_duty(1,value * (unsigned int16)64); // This sets the time the pulse is
                                                // high each cycle.  We use the A/D
                                                // input to make an easy demo.
                                                // The high time will be:
                                                //   value*64*4*(1/clock)*t2div
                                                // For example if value = 200,
                                                // clock=20MHz and t2div=1 then
                                                //  value*64*4*(1/clock)*t2div = 2.56 ms
                                                // WARNING:  A value too high or low will
                                                //           prevent the output from
                                                //           changing.
  }

}

