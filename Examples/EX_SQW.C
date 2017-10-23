/////////////////////////////////////////////////////////////////////////
////                             EX_SQW.C                            ////
////                                                                 ////
////  This program displays a message over the RS-232 and waits for  ////
////  any keypress to continue.  The program will then begin a 1khz  ////
////  square wave over I/O pin B0.                                   ////
////                                                                 ////
////  Comment out the printf's and getc to eliminate the RS232 and   ////
////  just output a square wave.                                     ////
////                                                                 ////
////  Change both delay_us to delay_ms to make the frequency 1 hz.   ////
////  This will be more visable on a LED.                            ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////     Insert jumpers from a LED to pin B0.                        ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCB        pin A2 to RS232 RX, pin A3 to RS232 TX           ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////     PCD        UART1A                                           ////
////                                                                 ////
////  This example will work with the PCB, PCM, PCD, and PCH         ////
////  compilers. The following conditional compilation lines are     ////
////  used to include a valid device for each compiler.  Change the  ////
////  device, clock and RS232 pins for your hardware if needed.      ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services         ////
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
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCB__)
#include <16C56.h>
#fuses HS,NOWDT,NOPROTECT
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_A3, rcv=PIN_A2)

#elif defined(__PCH__)
#include <18F458.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCD__)
#include <30F2010.h>
#fuses HS, NOWDT, NOPROTECT
#use delay(clock=20000000)
#use rs232(baud=9600, UART1A)
#endif


void main() {

   printf("Press any key to begin\n\r");
   getc();
   printf("1 khz signal activated\n\r");

   while (TRUE) {
     output_high(PIN_B0);
     delay_us(500);
     output_low(PIN_B0);
     delay_us(500);
   }
}
