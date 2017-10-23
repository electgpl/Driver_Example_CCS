/////////////////////////////////////////////////////////////////////////
////                     ex_rtos_demo4_messages.C                    ////
////                                                                 ////
////  This file demonstrates how to use the real time operating      ////
////  systems messaging functions.                                   ////
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
#use rtos(timer=0,minor_cycle=500ms)

int8 count;

// each task will now be given a two byte queue
#task(rate=1000ms,max=100ms,queue=2)
void The_first_rtos_task ( );

#task(rate=500ms,max=100ms,queue=2)
void The_second_rtos_task ( );

void The_first_rtos_task ( ) {
   // the function rtos_msg_poll will return the number of messages in the
   // current tasks queue
   // always make sure to check that their is a message or else the read
   // function will hang
   if(rtos_msg_poll ( )>0){
      // the function rtos_msg_read, reads the first value in the queue
      printf("messages recieved by task1 : %i\n\r",rtos_msg_read ( ));
      // the funciton rtos_msg_send, sends the value given as the
      // second parameter to the function given as the first
      rtos_msg_send(The_second_rtos_task,count);
      count++;
   }
}

void The_second_rtos_task ( ) {
   rtos_msg_send(The_first_rtos_task,count);
   if(rtos_msg_poll ( )>0){
      printf("messages recieved by task2 : %i\n\r",rtos_msg_read ( ));
      count++;
   }
}

void main ( ) {
   count=0;
   rtos_run();
}
