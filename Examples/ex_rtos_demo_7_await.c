/////////////////////////////////////////////////////////////////////////
////                    ex_rtos_demo_7_await.C                       ////
////                                                                 ////
////  This file demonstrates how to use the real time operating      ////
////  systems rtos_await function                                    ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#include <18F452.h>
#use delay(clock=20000000)
#use rs232(baud=9600,xmit=PIN_C6,rcv=PIN_C7)
#use rtos(timer=0,minor_cycle=100ms)

#define RED PIN_B5
#define GREEN PIN_A5

int8 count;

#task(rate=1000ms,max=100ms,queue=2)
void The_first_rtos_task ( );

#task(rate=1000ms,max=100ms,queue=2)
void The_second_rtos_task ( );

void The_first_rtos_task ( ) {
   // rtos_await simply waits for the given expression to be true
   // if it is not true, it acts like an rtos_yield and passes the system
   // to the next task
   rtos_await(count==10);
   output_low(GREEN); delay_ms(20); output_high(GREEN);
   count=0;
}

void The_second_rtos_task ( ) {
   output_low(RED); delay_ms(20); output_high(RED);
   count++;
}

void main ( ) {
   count=0;
   rtos_run();
}
