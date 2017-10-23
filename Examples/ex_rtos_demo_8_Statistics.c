/////////////////////////////////////////////////////////////////////////
////                   ex_rtos_demo_8_Statistics.C                   ////
////                                                                 ////
////  This file demonstrates how to use the real time operating      ////
////  systems statistics featuers                                    ////
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
#use rtos(timer=0,minor_cycle=100ms,statistics)

// This structure must be defined inorder to retrieve the statistical
// information
struct rtos_stats {
   int32 task_total_ticks;       // number of ticks the task has used
   int16 task_min_ticks;         // the minimum number of ticks used
   int16 task_max_ticks;         // the maximum number of ticks ueed
   int16 hns_per_tick;           // us = (ticks*hns_per_tic)/10
};

#task(rate=1000ms,max=100ms)
void The_first_rtos_task ( );

#task(rate=1000ms,max=100ms)
void The_second_rtos_task ( );

void The_first_rtos_task ( ) {
   struct rtos_stats stats;
   rtos_stats(The_second_rtos_task,&stats);
   printf ( "\n\r" );
   printf ( "task_total_ticks : %Lius\n\r" ,
            (int32)(stats.task_total_ticks)*stats.hns_per_tick );
   printf ( "task_min_ticks   : %Lius\n\r" ,
            (int32)(stats.task_min_ticks)*stats.hns_per_tick );
   printf ( "task_max_ticks   : %Lius\n\r" ,
            (int32)(stats.task_max_ticks)*stats.hns_per_tick );
   printf ("\n\r");
}

void The_second_rtos_task ( ) {
   int i, count = 0;

   while(TRUE) {
      if(rtos_overrun(the_second_rtos_task)) {
         printf("The Second Task has Overrun\n\r\n\r");
         count=0;
      }
      else
        count++;

      for(i=0;i<count;i++)
         delay_ms(50);

      rtos_yield();
   }
}

void main ( ) {
   rtos_run ( );
}
