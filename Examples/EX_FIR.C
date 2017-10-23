/////////////////////////////////////////////////////////////////////////
////                             EX_FIR.C                            ////
////                                                                 ////
////  This program filters out a Square wave input using a FIR filter////
////  The MAC instruction of the dsPIC is used for this purpose      ////
////  The filter coeficients are calculated at the start of the      ////
////  code and stored in Data memory.                                ////
////  The FIR filter basically performs a convolution of the input   ////
////  signal and the filter kernel.                                  ////
////  The filter kernel is a symmetric sinc function sin(pi*x)/(pi*x)////
////  The square wave is stored in RAM and later moved to BankY.     ////
////  Circular buffer addressing is used to read the input samples   ////
////  and multiply it with the filter kernel.                        ////
////  The FIR filters are usually memory intensive, but produce      ////
////  satisfactory output.                                           ////
////                                                                 ////
////  This example will work only with the PCD Compiler.             ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2009 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#if defined(__PCD__)
#include <33FJ128GP306.h>
#fuses HS, NOWDT, NOPROTECT, PR
#use delay(clock=12000000)
#else 
#error This example works only for PCD. Please select the PCD Compiler
#endif



#include <math.h>
#include <filter.h>
#include <init_fir.c>
#include <fir.c>

void main() 
{

   int length = FILTER_SIZE-1;

   init_filter_coef();
   init_sq_wave();
   


   // Save the CORCON and MODCON Registers
   #ASM
   PUSH CORCON   
   PUSH MODCON
   #ENDASM
   setup_registers(&kernel[0], length);
   Filter_FIR(&Sq_wave[0],&Output[0],&kernel[0], BLOCK_LENGTH);
   // Restore the CORCON and MODCON Registers
   #ASM
   POP MODCON
   POP CORCON    
   #ENDASM

 
  while (TRUE) {
   }
}


