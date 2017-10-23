/////////////////////////////////////////////////////////////////////////
////                           EX_PWM.C                              ////
////                                                                 ////
////  This program will show how to use the built in PIC PWM.        ////
////  The program takes an analog input and uses the digital         ////
////  value to set the duty cycle.  The frequency is set by          ////
////  the user over the RS-232.                                      ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////      Connect a scope to pin C2                                  ////
////      Connect pin A0 to output of the POT                        ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////                                                                 ////
////  This example will work with the PCM, and PCH compilers.  The   ////
////  following conditional compilation lines are used to include a  ////
////  valid device for each compiler.  Change the device, clock and  ////
////  RS232 pins for your hardware if needed.                        ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2013 Custom Computer Services         ////
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
#use delay(clock=10000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7, BRGH1OK)

#elif defined(__PCH__)
#include <18F458.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=10000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7, BRGH1OK)
#endif

void main(void)
{

   /* PWM EXAMPLE */

   char selection;
   unsigned int8 value;

   printf("\r\nFrequency:\r\n");
   printf("    1) 19.5 khz\r\n");
   printf("    2) 4.9 khz\r\n");
   printf("    3) 1.2 khz\r\n");

   do {
     selection=getc();
   } while((selection<'1')||(selection>'3'));

   setup_ccp1(CCP_PWM);   // Configure CCP1 as a PWM

          //   The cycle time will be (1/clock)*4*t2div*(period+1)
          //   In this program clock=10000000 and period=127 (below)
          //   For the three possible selections the cycle time is:
          //     (1/10000000)*4*1*128 =  51.2 us or 19.5 kHz
          //     (1/10000000)*4*4*128 = 204.8 us or 4.9 kHz
          //     (1/10000000)*4*16*128= 819.2 us or 1.2 kHz

   switch(selection) {
     case '1' : setup_timer_2(T2_DIV_BY_1, 127, 1);
                break;
     case '2' : setup_timer_2(T2_DIV_BY_4, 127, 1);
                break;
     case '3' : setup_timer_2(T2_DIV_BY_16, 127, 1);
                break;
   }

  setup_port_a(ALL_ANALOG);
  setup_adc(adc_clock_internal);
  set_adc_channel( 0 );
  printf("%c\r\n",selection);

  while( TRUE ) {
    value=read_adc();

    printf("%2X\r",value);

    set_pwm1_duty(value);          // This sets the time the pulse is
                                   // high each cycle.  We use the A/D
                                   // input to make an easy demo.
                                   // the high time will be:
                                   //  if value is LONG INT:
                                   //    value*(1/clock)*t2div
                                   //  if value is INT:
                                   //    value*4*(1/clock)*t2div
                                   // for example a value of 30 and t2div
                                   // of 1 the high time is 12us
                                   // WARNING:  A value too high or low will
                                   //           prevent the output from
                                   //           changing.
  }

}
