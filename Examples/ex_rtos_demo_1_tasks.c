/////////////////////////////////////////////////////////////////////////
////                     ex_rtos_demo1_tasks.C                       ////
////                                                                 ////
////  This file demonstrates how to use the ral time operating       ////
////  system to schedule tasks and how to use the rtos_run function  ////
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

#include <18F458.h>
#use delay(clock=20000000)
#use rs232(baud=9600,xmit=PIN_C6,rcv=PIN_C7)
// this tells the compiler that the rtos functionality will be needed, that
// timer0 will be used as the timing device, and that the minor cycle for
// all tasks will be 100 miliseconds
#use rtos(timer=0,minor_cycle=100ms)

// each function that is to be an operating system task must have the #task
// preprocessor directive located above it.
// in this case, the task will run every second, its maximum time to run must
// be less than or equal to the minor cycle, and there is no need for a
// queue at this point, so no memory will be reserved.
#task(rate=1000ms,max=100ms)
// the function can be called anything that a standard function can be called
void The_first_rtos_task ( )
{
   printf("1\n\r");
}

#task(rate=500ms,max=100ms)
void The_second_rtos_task ( )
{
   printf("\t2!\n\r");
}

#task(rate=100ms,max=100ms)
void The_third_rtos_task ( )
{
   printf("\t\t3\n\r");
}

// main is still the entry point for the program
void main ( )
{
   // rtos_run begins the loop which will call the task functions above at the
   // schedualed time
   rtos_run ( );
}
