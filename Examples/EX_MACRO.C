/////////////////////////////////////////////////////////////////////////
////                           EX_MACRO.C                            ////
////                                                                 ////
////  This program it not intended to be executed but does show a    ////
////  number of variations of the #define pre-processor directive.   ////
////                                                                 ////
////  This example will work with the PCB, PCM and PCH, and PCD      ////
////  compilers.                                                     ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#define BUFFERSIZE 10
#define BUFFER_EMPTY (next_in==next_out)

#define min(x,y) ((x<y)?x:y)
#define max(x,y) ((x>y)?x:y)

#define forever while(1);

#define MHZ(x)  x##000000

#ifdef  __PCD__
#define NORMAL_RS232 baud=9600, UART1A
#else
#ifndef __PCB__
#define NORMAL_RS232 baud=9600, xmit=PIN_C6, rcv=PIN_C7
#else
#define NORMAL_RS232 baud=9600, xmit=PIN_B1, rcv=PIN_B0
#endif
#endif

#define set_options(value)   {#ASM         \
                              MOVLW  value \
                              OPTION       \
                              #ENDASM}

#define debug(x)   printf("%s variable value is %d\r\n",#x,x);

#define TOSTRING(s)   #s

#define DEVICE_FILE_FOR(chip)  TOSTRING(chip##.h)

#ifdef __pcb__
#define IDLE  {if(kbhit()) isr();}
#else
#define IDLE  ;
#endif

#ifdef __PCD__
#include DEVICE_FILE_FOR(30F2010)
#fuses HS,NOPROTECT
#use delay(clock=MHZ(20))
#use RS232(NORMAL_RS232)
#else
#include DEVICE_FILE_FOR(16C74)
#fuses HS,NOPROTECT
#use delay(clock=MHZ(20))
#use RS232(NORMAL_RS232)
#endif

unsigned int8 buffer[BUFFERSIZE];
unsigned int8 next_in,next_out;

#ifndef __pcb__
#int_rda
#endif
void isr() {
   buffer[next_in]=getc();
   next_in=(next_in+1)%BUFFERSIZE;
}

void main() {

    unsigned int8 x,largest;

    set_options(0x34);

    #ifndef __pcb__
    enable_interrupts(INT_RDA);
    enable_interrupts(GLOBAL);
    #endif

    next_in=next_out=0;

    largest=0;

    do {
       while(BUFFER_EMPTY)
      IDLE;

       x=buffer[next_out];
       next_in=(next_out+1)%BUFFERSIZE;

       largest = max(largest,x);

       debug(next_in);
       debug(next_out)

    } forever;
}
