/////////////////////////////////////////////////////////////////////////
////                      Ex_Sync_Master.C                           ////
////                                                                 ////
////  This example demonstrates Master-Slave communication using the ////
////  UART. The  SYNC_MASTER parameter makes the RS232 line a        ////
///   a synchronous master, making the receive pin the clock out     ////
////  and the data pin the data in/out.                              ////
////  Load the program onto one PIC chip (Master) and load the       ////
////  Ex_Sync_Slave.c program onto another PIC. Connect the 2 UARTS  ////
////  Connect C6 from one to C6 to the other.                        ////
////  Connect C7 from one to C7 to the other.                        ////
////  Make sure both the chips are powered from the same source, i.e ////
////  they share common VDD and GND                                  ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2006 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////



#include <18f4520.h>
#fuses HS,NOLVP,NOWDT
#use delay(clock=20000000)
#use rs232(baud=119200, bits=8, UART1, SYNC_MASTER)

void main()
{
   while(true)
   {
      putc('g');
      delay_ms(1000);
      putc('y');
      delay_ms(1000);
      putc('r');
      delay_ms(1000);
   }
}
