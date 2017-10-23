/////////////////////////////////////////////////////////////////////////
////                             EX_CNI.C                            ////
////                                                                 ////
////  This program is an example to setup the Change Notification    ////
////  Interrupt for the dsPIC30F5011 chip                            ////
////                                                                 ////
////  The code will enable the Change Notification Interrupt on the  ////
////  RC14 pin on the 30F5011 chip (Pin 48).                         ////
////                                                                 ////
////  The code will toggle the LED on PIN_D9 and service the         ////
////  the interrupt routine when a signal changes on the RC14 pin    ////
////  The interrupt service routine has a printf statement that will ////
////  indicate that the code has entered the interrupt               ////
////  This will be more visible on a LED.                            ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////     Insert jumpers from a LED to pin D9.                        ////
////     PCD        UART1                                            ////
////                                                                 ////
////  This example will work only for the PCD Compiler               ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2007 Custom Computer Services         ////
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
#include <30F5011.h>
#fuses HS, NOWDT, NOPROTECT
#use delay(clock=20000000)
#use rs232(baud=9600, UART1)

#int_CNI
void CCNI_Interrupt()
{
  printf("\n\rPin Changed");
}


void main() {

   printf("\n\rRESET");

   enable_interrupts(INTR_CN_PIN | PIN_C14);
   enable_interrupts(INTR_GLOBAL);
   while (TRUE) {
     output_high(PIN_D9);
     delay_ms(500);
     output_low(PIN_D9);
     delay_ms(500);
   }
}
