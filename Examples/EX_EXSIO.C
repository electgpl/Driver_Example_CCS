/////////////////////////////////////////////////////////////////////////
////                EX_EXSIO - SC28L19x UART Test program            ////
////                                                                 ////
//// This program shows how to use an external UART on the PIC.      ////
//// It will identify ports 0 and 1 on startup then transfer data    ////
//// from port 0 to 1 and from 1 to 0.                               ////
////                                                                 ////
////  This example will work with the PCB, PCM and PCH compilers.    ////
////  The following conditional compilation lines are used to        ////
////  include a valid device for each compiler.  Change the device,  ////
////  clock and RS232 pins for your hardware if needed.              ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services         ////
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

#elif defined(__PCM__)
#include <16F877.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)

#elif defined(__PCH__)
#include <18F452.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#endif

#include "sc28l19x.c"

void putc0(char c) {
   ext_uart_putc(0,c);
}

void putc1(char c) {
   ext_uart_putc(1,c);
}


void putc2(char c) {
   ext_uart_putc(2,c);
}


void main()  {
   set_tris_a(0x0f);
   set_tris_b(0);
   set_tris_c(0);
   set_tris_d(0xff);
   set_tris_e(2);

   ext_uart_init(0, BAUD_9600, PARITY_NONE, BITS_8);
   ext_uart_init(1, BAUD_9600, PARITY_NONE, BITS_8);

   putc0("\r\nYou are port zero.\r\n");
   putc1("\r\nYou are port one.\r\n");

   while( TRUE ) {
      if(ext_uart_kbhit(0)) {
         putc1(ext_uart_getc(0));
      }
      if(ext_uart_kbhit(1)) {
         putc0(ext_uart_getc(1));
      }
   }
}

