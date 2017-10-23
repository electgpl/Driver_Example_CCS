/////////////////////////////////////////////////////////////////////////
////                          EX_ADMM.C                              ////
////                                                                 ////
////  This program displays the min and max of 30 A/D samples over   ////
////  the RS-232 interface.  The process is repeated forever.        ////
////                                                                 ////
////  If required configure the CCS prototype card as follows:       ////
////     Insert jumper from output of POT to pin A5                  ////
////     Use a 10K POT to vary the voltage.                          ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////     PCD        none                                             ////
////                                                                 ////
////  This example will work with the PCM, PCH, and PCD compilers.   ////
////  The following conditional compilation lines are used to        ////
////  include a valid device for each compiler.  Change the device,  ////
////  clock and RS232 pins for your hardware if needed.              ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2007 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////


#if defined(__PCM__)                           // Preprocessor directive that chooses the compiler
#include <16F877.h>                           // Preprocessor directive that selects the chip
#fuses HS,NOWDT,NOPROTECT,NOLVP                // Preprocessor directive that defines the chip fuses 
#use delay(clock=20000000)                     // Preprocessor directive that specifies clock speed
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7) // Preprocessor directive that includes RS232 libraries

#elif defined(__PCH__)
#include <18F452.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCD__)
#include <30F2010.h>
#fuses HS,NOWDT
#device ADC=8
#use delay(clock=20000000)
#use rs232(baud=9600, UART1A)
#endif

void main() { 

   unsigned int8 i, value, min, max;

   printf("Sampling:");                      // Printf function included in RS232 library

   #ifdef __PCD__
   setup_adc_ports(sAN0);                    // Built-in A/D setup function
   #else
   setup_adc_ports(AN0);
   #endif
   
   setup_adc(ADC_CLOCK_INTERNAL);            // Built-in A/D setup function
   set_adc_channel(0);                       // Built-in A/D setup function

   do {
      min=255;
      max=0;
      for(i=0; i<=30; ++i) {
         delay_ms(100);                      // Built-in delay function
         value = read_adc();                 // Built-in A/D read function
         if(value<min)
            min=value;
         if(value>max)
            max=value;
      }
      
      printf("\r\nMin: %2X  Max: %2X\n\r",min,max);     
   } while (TRUE);
}
