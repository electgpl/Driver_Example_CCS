/////////////////////////////////////////////////////////////////////////
////                        EX_ADMM10.C                              ////
////                                                                 ////
////  This program displays the min and max of 30 A/D samples over   ////
////  the RS-232 interface.  The process is repeated forever.        ////
////  This version uses a 10 bit ADC.                                ////
////                                                                 ////
////  Configure the CCS prototype card as follows for PCM and PCH:   ////
////     Insert jumper from output of POT to pin A5                  ////
////     Use a 10K POT to vary the voltage.                          ////
////                                                                 ////
////  Configure the CCS prototype card as follows for PCD:           ////
////     Use the POT labeled B0 or connect a POT to B0               ////
////     Use a 10K POT to vary the voltage.                          ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////     PCD        pin F2 to RS232 RX, pin F3 to RS232 TX           ////
////                                                                 ////
////  This example will work with the PCM and PCH compilers.  The    ////
////  following conditional compilation lines are used to include a  ////
////  valid device for each compiler.  Change the device, clock and  ////
////  RS232 pins for your hardware if needed.                        ////
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
#device ADC=16
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCH__)
#include <18F452.h>
#device ADC=16
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCD__)
#include <30F2010.h>
#device ADC=16
#fuses HS,NOWDT,NOPROTECT
#use delay(clock=20000000)
#use rs232(baud=9600, UART1A)
#endif

void main() {

   unsigned int8 i;
   unsigned int16 value, min, max;

   printf("Sampling:");

   setup_port_a( ALL_ANALOG );
   setup_adc( ADC_CLOCK_INTERNAL );
   set_adc_channel( 0 );

   do {
      min=0xffff;
      max=0;
      for(i=0; i<=30; ++i) {
         delay_ms(100);
         value = Read_ADC();
         if(value<min)
            min=value;
         if(value>max)
            max=value;
      }
      printf("\n\rMin: %4LX  Max: %4LX\n\r",min,max);

   } while (TRUE);
}
