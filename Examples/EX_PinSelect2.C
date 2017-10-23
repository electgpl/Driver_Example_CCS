/////////////////////////////////////////////////////////////////////////
////                          EX_PinSelect2.C                        ////
////                                                                 ////
////  This program will show how to use the Peripheral pin select    ////
////  directive to setup the I/O ports for the PIC24FJ64GA004 family ////
////  This example will run on the Explorer16 demo board with a      ////
////  PIC24FJ64GA004 PIM                                             ////
////                                                                 ////
////  Hardware UART 2 is setup to talk to the PC through pins C9,C3. ////
////  Pin B4 is configured as the Extrnal Interrupt 1 pin            ////
////  Pin B8 is configured as the Input Capture 4 pin                ////
////                                                                 ////
////  This example will work only with the  PCD compilers.           ////
////  To change the device, simply include another device using the  ////
////  #include directive                                             ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2007 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////



#include <24FJ64GA004.h>
#fuses HS,NOWDT,NOPROTECT,PR 
#use delay(clock=8000000)
#use rs232(baud=9600, UART1)


/* Following lines of code will do the following:
   Setup Pin C9 as UART 2 Transmit pin
   Setup Pin C3 as UART 2 Receive Pin
   Setup Pin B4 as the Extrnal Interrupt 1 pin
   Setup Pin B8 as the Input Capture 4 pin
   The compiler will automatically generate code for setting the RPINRx
   and the RPORx registers, and lock the I/O pin select bit after it is done
   setting up these registers
*/
#pin_select U2TX = PIN_C9 
#pin_select U2RX = PIN_C3

#pin_select INT1 = PIN_B4   // Setup External Interrupt INT_EXT1
#pin_select IC4  =  PIN_B8  // setup Input Capture  INT_IC4

#use rs232(baud=9600, UART2)


#INT_EXT1 Level = 4
void external_interrupt1(void)
{
   printf("\n\r EXT1 Trigger");
}


#INT_IC4 Level = 4
void Input_Capture4(void)
{
   printf("\n\r IC4 Trigger");
}
void main()
{
   printf("\n\rRESET");
   
   // Following setup required for Input Capture 
   setup_timer2(TMR_INTERNAL);    // Start timer 2
   setup_capture(4,CAPTURE_RE |INTERRUPT_EVERY_CAPTURE | CAPTURE_TIMER2 );   
   
   enable_interrupts(INTR_GLOBAL);
   enable_interrupts(INT_EXT1);
   enable_interrupts(INT_IC4);
   
    while(1)
    {
    }
}


