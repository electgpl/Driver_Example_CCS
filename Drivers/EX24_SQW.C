/////////////////////////////////////////////////////////////////////////
////                             EX24_SQW.C                          ////
////                                                                 ////
////  This program displays a message over the RS-232 and waits for  ////
////  any keypress to continue.  The program will then begin a 1khz  ////
////  square wave over I/O pin B4.                                   ////
////                                                                 ////
////  Comment out the printf's and getc to eliminate the RS232 and   ////
////  just output a square wave.                                     ////
////                                                                 ////
////  Change both delay_us to delay_ms to make the frequency 1 hz.   ////
////  This will be more visable on a LED.                            ////
////                                                                 ////
////                                                                 ////
////  This example will work with the PCD compiler.                  ////
////  UART1A specifies the alternate UART pins Pin_C13, Pin_C14      ////
////  use UART1 to sprcify UART for pins Pin_F3, Pin_F2              ////
////  Change the device, clock and RS232 pins for your hardware if   ////
////  needed.                                                        ////
/////////////////////////////////////////////////////////////////////////

#include <30F2010.h>

//#device ICD=TRUE  // For using the debugger, un-comment

#use delay(crystal=20mhz)




#use rs232(baud=9600, UART1A)


void main() {

   printf("Press any key to begin\n\r");
   getc();
   printf("1 khz signal activated\n\r");

   while (TRUE) {
     output_high(PIN_B4);
     delay_us(500);
     output_low(PIN_B4);
     delay_us(500);
   }
}

