/////////////////////////////////////////////////////////////////////////
////                          EX_SISR.C                              ////
////                                                                 ////
////  This program shows how to implement an interrupt service       ////
////  routine to buffer up incomming serial data.                    ////
////                                                                 ////
////  If the PIC does not have an RDA interrupt pin, B0 may be used  ////
////  with the INT_EXT.                                              ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////     PCD        UART1A                                           ////
////                                                                 ////
////  This example will work with the PCM, PCD, and PCH compilers.   ////
////  The following conditional compilation lines are used to include////
////  a valid device for each compiler.  Change the device, clock and////
////  RS232 pins for your hardware if needed.                        ////
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

#elif defined(__PCH__)
#include <18F452.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCD__)
#include <30F2010.h>
#fuses HS, NOWDT, NOPROTECT
#use delay(clock=20000000)
#use rs232(baud=9600, UART1A)
#endif


#define BUFFER_SIZE 32
BYTE buffer[BUFFER_SIZE];
BYTE next_in = 0;
BYTE next_out = 0;


#int_rda
void serial_isr() {
   int t;

   buffer[next_in]=getc();
   t=next_in;
   next_in=(next_in+1) % BUFFER_SIZE;
   if(next_in==next_out)
     next_in=t;           // Buffer full !!
}

#define bkbhit (next_in!=next_out)

BYTE bgetc() {
   BYTE c;

   while(!bkbhit) ;
   c=buffer[next_out];
   next_out=(next_out+1) % BUFFER_SIZE;
   return(c);
}

void main() {

   enable_interrupts(int_rda);
   #if defined(__PCD__)
   enable_interrupts(intr_global);
   #else
   enable_interrupts(global);
   #endif

   printf("\r\n\Running...\r\n");

               // The program will delay for 10 seconds and then display
               // any data that came in during the 10 second delay

   do {
      delay_ms(10000);
      printf("\r\nBuffered data => ");
      while(bkbhit)
        putc( bgetc() );
   } while (TRUE);
}
