/////////////////////////////////////////////////////////////////////////
////                           EX_QSORT.C                            ////
////                                                                 ////
////  This program shows how to use the stdlib function qsort.       ////
////  This is also a clasic example of how to implement pointers     ////
////  to functions.                                                  ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////                                                                 ////
////  This example will work with the PCM and PCH compilers.  The    ////
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
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCH__)
#include <18F458.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#endif

#include <stdlib.h>

#define maxchars 6
#define items 5

char data[items][maxchars] = {"ONE","TWO","THREE","FOUR","FIVE"};


signed int assending(char * p1,char * p2) {
   int i;

   i=0;
   while((i<maxchars)&&(*p1==*p2)) {
     ++i;
     ++p1;
     ++p2;
   }
   if(i==maxchars)
     return 0;
   else if(*p1<*p2)
     return -1;
   else
     return 1;
}

signed int desending(char * p1,char * p2) {
   int i;

   i=0;
   while((i<maxchars)&&(*p1==*p2)) {
     ++i;
     ++p1;
     ++p2;
   }
   if(i==maxchars)
     return 0;
   else if(*p1>*p2)
     return -1;
   else
     return 1;
}



void main() {
   int i;
   char c;
   _Cmpfun compare;

   while( TRUE ) {
      do {
        printf("\r\n\r\nSort Assending or Desending (A,D): ");
        c=toupper(getc());
      } while ((c!='A')&&(c!='D'));

      if(c == 'A')
        compare=assending;
      else
        compare=desending;

      qsort(data, items, maxchars, compare);
      for(i=0; i<items; ++i)
        printf("\r\n%s",data[i]);
   }
}

