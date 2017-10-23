/////////////////////////////////////////////////////////////////////////
////                  ex_rtos_demo_9_basic_kernal.C                  ////
////                                                                 ////
////  This file demonstrates how to create a basic command line      ////
////  using the serial port without having to stop the RTOS          ////
////  operation.  This can also be considered a semi kernal for      ////
////  the RTOS.                                                      ////
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

#include <string.h>

// this character array will be used to take input from the prompt
char input [ 30 ];
// this will hold the current position in the array
int index;
// this will signal to the kernal that input is ready to be processed
int1 input_ready;
// different commands
char en1 [ ] = "enable1";
char en2 [ ] = "enable2";
char dis1 [ ] = "disable1";
char dis2 [ ] = "disable2";

#task(rate=1000ms,max=100ms)
void The_first_rtos_task ( );

#task(rate=1000ms,max=100ms)
void The_second_rtos_task ( );

#task(rate=500ms,max=100ms)
void The_kernal ( );

// serial interupt
#int_rda
void serial_interrupt ( )
{
   if(index<29) {
      input [ index ] = getc ( );   // get the value in the serial recieve reg
      putc ( input [ index ] );     // display it on the screen
      if(input[index]==0x0d){       // if the input was enter
         putc('\n');
         input [ index ] = '\0';    // add the null character
         input_ready=TRUE;          // set the input read variable to true
         index=0;                   // and reset the index
      }
      else if (input[index]==0x08){
         if ( index > 1 ) {
            putc(' ');
            putc(0x08);
            index-=2;
         }
      }
      index++;
   }
   else {
      putc ( '\n' );
      putc ( '\r' );
      input [ index ] = '\0';
      index = 0;
      input_ready = TRUE;
   }
}

void The_first_rtos_task ( ) {
   output_low(RED); delay_ms(50); output_high(RED);
}

void The_second_rtos_task ( ) {
   output_low(GREEN); delay_ms(20); output_high(GREEN);
}

void The_kernal ( ) {
   while ( TRUE ) {
      printf ( "INPUT:> " );
      while(!input_ready)
         rtos_yield ( );

      printf ( "%S\n\r%S\n\r", input , en1 );

      if ( !strcmp( input , en1 ) )
         rtos_enable ( The_first_rtos_task );
      else if ( !strcmp( input , en2 ) )
         rtos_enable ( The_second_rtos_task );
      else if ( !strcmp( input , dis1 ) )
         rtos_disable ( The_first_rtos_task );
      else if ( !strcmp ( input , dis2 ) )
         rtos_disable ( The_second_rtos_task );
      else
         printf ( "Error: unknown command\n\r" );

      input_ready=FALSE;
      index=0;
   }
}

void main ( ) {
   // initialize input variables
   index=0;
   input_ready=FALSE;
   // initialize interrupts
   enable_interrupts(int_rda);
   enable_interrupts(global);
   rtos_run();
}
