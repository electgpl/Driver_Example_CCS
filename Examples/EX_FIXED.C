/////////////////////////////////////////////////////////////////////////
////                          EX_FIXED.C                             ////
////                                                                 ////
////  This program shows how to display a LONG INT in a fixed        ////
////  point format.                                                  ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCB        pin A2 to RS232 RX, pin A3 to RS232 TX           ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////     PCD        pin F2 to RS232 RX, pin F3 to RS232 TX           ////
////                                                                 ////
////  This example will work with the PCB, PCM and PCH compilers.    ////
////  The following conditional compilation lines are used to        ////
////  include a valid device for each compiler.  Change the device,  ////
////  clock and RS232 pins for your hardware if needed.              ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2008 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////


#if defined(__PCB__)
#include <16C56.h>
#fuses HS,NOWDT,NOPROTECT
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_A3, rcv=PIN_A2)

#elif defined(__PCM__)
#include <16F877.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCH__)
#include <18F452.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCD__)
#include <30F2010.h>
#fuses HS,NOWDT
#use delay(clock=20000000)
#use rs232(baud=9600, UART1A)
#endif

#include <input.c>


void print_fp_2(unsigned int16 value) {
   byte i,digit;
   unsigned int16 divisor;

   divisor=10000;
   for(i=0;i<5;++i) {
      digit = value/divisor;
      value-=digit*divisor;
      divisor/=10;
      putc(digit+'0');
      if(i==2)
        putc('.');
   }
}

void print_as_volts(unsigned int16 value) {    // This function shows how to
   byte i,digit;                         // display a fixed point number.
   unsigned int16 divisor;                     // In this example we assume that
                                         // 0xffff is 5 volts

   divisor=13107;         // 65535 / 5
   for(i=0;i<5;++i) {
      digit = value/divisor;
      value-=digit*divisor;
      divisor/=10;
      putc(digit+'0');
      if(i==0)
        putc('.');
   }
}



void main() {
   unsigned int16 value;

   do {
      printf("\r\n\nHex value: ");
      value=gethex();
      value=value<<8|gethex();

      printf("\r\nAs dollers:  ");
      print_fp_2(value);

      printf("\r\nAs Volts:  ");
      print_as_volts(value);

   } while (TRUE);
}
