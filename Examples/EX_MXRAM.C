/////////////////////////////////////////////////////////////////////////
////                           EX_MXRAM.C                            ////
////                                                                 ////
////  This program it not intended to be executed but does show how  ////
////  to directly use all the RAM on a 877 chip (or any chip with    ////
////  RAM over 255).  For another method that takes less ROM see     ////
////  EX_PSP.C                                                       ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////                                                                 ////
////  This example will work with the PCB and PCH compilers.         ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#if defined( __PCB__)
#include <16C57.H>
#device *=8                     // This allows auto variables over location 0x1F
#fuses HS,NOWDT,NOPROTECT
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_A3, rcv=PIN_A2)
#define BUFFSIZE 16

#elif defined(__PCM__)
#include <16F877.H>
#device *=16                    // This allows auto variables over location 0xFF
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#define BUFFSIZE 64
#endif


byte buffer1[BUFFSIZE],buffer2[BUFFSIZE],buffer3[BUFFSIZE];
// Three buffers are allocated because C requires arrays elements to be in
// consecutive memory locations and the PIC memory has holes.


// The following allows reads and writes to the three arrays like they are
// one large array.


void write_buffer(char * index, char value) {
   if(index<BUFFSIZE)
    buffer1[index]=value;
 else if(index<(BUFFSIZE*2))
    buffer2[index%BUFFSIZE]=value;
 else
    buffer3[index%BUFFSIZE]=value;
}

char read_buffer(char * index) {
   if(index<BUFFSIZE)
    return(buffer1[index]);
 else if(index<(BUFFSIZE*2))
    return(buffer2[index%BUFFSIZE]);
 else
    return(buffer3[index%BUFFSIZE]);
}



void main() {
   char * i;
   char c;

   do {
       i=0;
       printf("\r\nEnter string: ");
       do{
         c=getc();
          write_buffer(i++,c);
       } while(c!='\r');

       i=0;
       printf("\r\nString from buffer: ");
       do{
         c=read_buffer(i++);
         putc(c);
       } while(c!='\r');
 } while(1);
}
