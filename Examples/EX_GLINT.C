/////////////////////////////////////////////////////////////////////////
////                         EX_GLINT.C                              ////
////                                                                 ////
////  This example shows how to use the global interrupt to execute  ////
////  interrupts quickly and efficently.  It also shows how to use   ////
////  the #asm and #endasm directives.                               ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////                                                                 ////
////  This example will work with the PCM and PCH compilers.         ////
////  The following conditional compilation lines are used to        ////
////  include a valid device for each compiler.  Change the device,  ////
////  clock and RS232 pins for your hardware if needed.              ////
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
#include <18F452.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#endif

long counter;

#if defined(__PCM__)
   int save_w;
   #locate save_w=0x7f
   int save_status;
   #locate save_status=0x20

   #byte status = 3
   #bit zero_flag = status.2
   #bit t0if = 0xb.2
#elif defined(__PCH__)
   int save_w;
   #locate save_w=0x80
   int save_status;
   #locate save_status=0xFF

   #byte status = 0xFD8
   #bit zero_flag = status.2
   #bit t0if = 0xFF2.2
#endif



#INT_GLOBAL
void isr()  {
   #asm
   //store current state of processor
   MOVWF save_w
   SWAPF status,W
   BCF   status,5
   BCF   status,6
   MOVWF save_status
   // Save anything else your code may change
   // You need to do something with PCLATH if you have GOTOs

   // remember to check to see what interrupt fired if using more than one!!

   // code for isr
   BCF   t0if
   INCF  counter,F
   BTFSC zero_flag
   INCF  (&counter+1),F

   // restore processor and return from interrupt
   SWAPF save_status,W
   MOVWF status
   SWAPF save_w,F
   SWAPF save_w,W
   #endasm
}

void main()   {

   printf("\r\nStarting the counter.\r\n\n");
   counter = 0;

   setup_counters(RTCC_INTERNAL,RTCC_DIV_32);
   enable_interrupts(INT_RTCC);
   enable_interrupts(GLOBAL);

   while(TRUE)
   {
      printf("The counter value is currently:  %5lu\r\n", counter);
      delay_ms(1000);
   }
}
