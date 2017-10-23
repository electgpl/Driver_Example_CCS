/////////////////////////////////////////////////////////////////////////
////                          EX_ENCRY.C                             ////
////                                                                 ////
////  This program shows how to implement two serial ports to        ////
////  transfer data between the ports and to encrypt/decrypt         ////
////  the data on one side for a secure communications link.         ////
////                                                                 ////
////  +------+    +-----+             +-----+    +------+            ////
////  |  PC  |    | PIC |             | PIC |    |  PC  |            ////
////  |  A   |====|  A  |=============|  B  |====|  B   |            ////
////  +------+    +-----+    secure   +-----+    +------+            ////
////                          wire                                   ////
////                                                                 ////
////                                                                 ////
////  This example will work with the AFL. The                       ////
////  following conditional compilation lines are used to include a  ////
////  valid device for each compiler.  Change the device, clock and  ////
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

#elif defined(__PCH__)
#include <18F452.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#endif

#define BUFFER_SIZE 32

///////////////////////////////////////////////////////////////// PORT 1
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

byte buffer1[BUFFER_SIZE];
byte next_in1 = 0;
byte next_out1 = 0;

#int_rda
void serial_isr1() {
   int t;

   buffer1[next_in1]=getc();
   t=next_in1;
   next_in1=(next_in1+1) % BUFFER_SIZE;
   if(next_in1==next_out1)
     next_in1=t;           // Buffer full !!
}

#define bkbhit1 (next_in1!=next_out1)

byte bgetc1() {
   byte c;

   while(!bkbhit1) ;
   c=buffer1[next_out1];
   next_out1=(next_out1+1) % BUFFER_SIZE;
   return(c);
}

void putc1(char c) {
   putc(c);
}


///////////////////////////////////////////////////////////////// PORT 2
#use rs232(baud=9600, xmit=PIN_B1, rcv=PIN_B0)

byte buffer2[BUFFER_SIZE];
byte next_in2 = 0;
byte next_out2 = 0;

#int_ext
void serial_isr2() {
   int t;

   buffer2[next_in2]=getc();
   t=next_in2;
   next_in2=(next_in2+1) % BUFFER_SIZE;
   if(next_in2==next_out2)
     next_in2=t;           // Buffer full !!
}

#define bkbhit2 (next_in2!=next_out2)

byte bgetc2() {
   byte c;

   while(!bkbhit2) ;
   c=buffer2[next_out2];
   next_out2=(next_out2+1) % BUFFER_SIZE;
   return(c);
}

void putc2(char c) {
   putc(c);
}

///////////////////////////////////////////////////////////////////////////////////////

void main() {
   char c;

   enable_interrupts(global);
   enable_interrupts(int_rda);
   enable_interrupts(int_ext);

   printf(putc2,"\r\n\Running...\r\n");


   do {
      if(bkbhit1) {
		   c = bgetc1();
			putc2(c);
		}
      if(bkbhit2) {
		   c = bgetc2();
			putc1(c);
		}
   } while (TRUE);
}
