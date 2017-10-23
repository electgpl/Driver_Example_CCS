/////////////////////////////////////////////////////////////////////////
////                           EX_PinSelect.C                        ////
////                                                                 ////
////  This program will show how to use the Peripheral pin select    ////
////  directive to setup the I/O ports for the PIC24FJ64GA004 family ////
////  This example will run on the Explorer16 demo board with a      ////
////  PIC24FJ64GA004 PIM                                             ////
////                                                                 ////
////  Hardware UART 2 is setup to talk to the PC through pins C9,C3. ////
////  Pin B7 is used as the output Compare output pin to test the    ////
////  output of the PWM                                              ////
////                                                                 ////
////  This example is the modified Ex_pwm example that runs on the   ////
////  24FJ64GA004 chip on a Explorer 16 board.                       ////
////  The program takes an analog input from the pot connected to    ////
////  the pin C1 (AN7) and uses this value to set the duty cycle.    ////
////  The frequency is set by the user over the RS-232.              ////
////                                                                 ////
////  Configure the Explorer 16 board as follows:                    ////
////  Connect Jumper JP1 for the Pot connection                      ////
////  Connect 9 pin serial cable from Explorer16 board to PC         ////
////                                                                 ////
////                                                                 ////
////  This example will work only with the  PCD compilers.           ////
////  To change the device, simply include another device using the  ////
////  #include directive                                             ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2007 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////


#include <24FJ64GA004.h>
#fuses HS,NOWDT,PR
#device ADC=10
#use delay(clock=8000000)

/* The following directives will do the following:
   Setup Pin C9 as UART 2 Transmit pin
   Setup Pin C3 as UART 2 Receive Pin
   Setup Pin B7 as the Output Compare 1 pin
   The compiler will automatically generate code for setting the RPINRx
   and the RPORx registers, and lock the I/O pin select bit after it is done
   setting up these registers
*/ 
#pin_select U2TX = PIN_C9 
#pin_select U2RX = PIN_C3
#pin_select OC1 = PIN_B7

#use rs232(baud=9600, UART2)

void main(void) 
{  

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
          //   In this program clock=20000000 and period=127 (below)
          //   For the three possible selections the cycle time is:
          //     (1/8000000)*2*1*65473  =  16.36 ms or 61.09 hz
          //     (1/8000000)*2*8*65473  = 130.9 ms or 7.63  hz
          //     (1/8000000)*2*64*65473 = 1047 ms or 0.954  hz

   switch(selection) {
     case '1' : setup_timer2(TMR_INTERNAL | TMR_DIV_BY_1, 0xFFC0);
                break;
     case '2' : setup_timer2(TMR_INTERNAL | TMR_DIV_BY_8, 0xFFC0);
                break;
     case '3' : setup_timer2(TMR_INTERNAL | TMR_DIV_BY_64, 0xFFC0);
                break;
   }


   setup_adc_ports(sAN7);
   setup_adc(adc_clock_internal);
   // The pot is connected to AN7 - Pin C1
   set_adc_channel(7);

   printf("%c\r\n",selection);
   setup_compare(1, COMPARE_PWM | COMPARE_TIMER2);
  
   while(TRUE) 
  {
    value=read_adc();
   
    /* set the duty cycle using the adc reading.  the adc reading is in 10 bits,
      the duty cylce is in 16 bits, multiplying by 64 brings adc up to 16 bits */
    set_pwm_duty(1,value * (int16)64); // This sets the time the pulse is
                                       // high each cycle.  We use the A/D
                                       // input to make a easy demo.
                                       // the high time will be:
                                       // value is int16, clock = 20MHz, t2div = 1
                                       // This will give period of 13.1 ms
                                       // For example if value = 200, then
                                       //  value*64*4*(1/clock)*t2div = 2.56 ms
                                       // This will give an duty cycle of about 20 %
                                       // WARNING:  A value too high or low will
                                       //           prevent the output from
                                       //           changing.
                                   
  }

}

