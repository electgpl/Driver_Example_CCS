/////////////////////////////////////////////////////////////////////////
////                      Ex_Sync_Slave.C                            ////
////                                                                 ////
////  This example demonstrates Master-Slave communication using the ////
////  UART. The  SYNC_SLAVE parameter makes the RS232 line a         ////
///   a synchronous slave, making the receive pin the clock out      ////
////  and the data pin the data in/out.                              ////
////  Load the program onto one PIC chip (Slave) and load the        ////
////  Ex_Sync_Master.c program onto another PIC. Connect the 2 UARTS ////
////  Connect C6 from Master PIC to C6 of Slave PIC                  ////
////  Connect C7 from Master PIC to C7 of Slave PIC                  ////
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
#include <utility.c>
#use rs232(bits=8, UART1, SYNC_SLAVE)

void main()
{
   char data;
while(true)
   {
   while(true)
          {
             data = getc();
             switch(data){
             case 'g':output_high(PIN_A1);
                      output_low(PIN_A2);
                      output_low(PIN_A3);
             break;
             case 'y':output_low(PIN_A1);
                      output_high(PIN_A2);
                      output_low(PIN_A3);

             break;
             case 'r':output_low(PIN_A1);
                      output_low(PIN_A2);
                      output_high(PIN_A3);

             break;
              default:output_low(PIN_A1);
                      output_low(PIN_A2);
                      output_low(PIN_A3);
             break;

                           }
           }
    }
}
