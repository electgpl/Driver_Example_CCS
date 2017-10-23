/////////////////////////////////////////////////////////////////////////
////                          ex_EXTDynMem.c                         ////
////                                                                 ////
////  This program uses the mt4264 and the d41256 dynamic memory     ////
////  device drivers to read and write data to an external dynamic   ////
////  memory chip.                                                   ////
////                                                                 ////
////  Change the include file to either the mt4264.c file for a 64k  ////
////  by 1 bit dynamic memory chip or the d41256.c file for a 256k   ////
////  by 1 bit dynamic memory chip.                                  ////
////                                                                 ////
////  Configure your developement board as follows:                  ////
////     Connections:                                                ////
////        Pins:                   Protoboard:                      ////
////        *note that pin one is only used on the 256k chip for     ////
////         address bit 9.  on the 64k chip, it is not connected    ////
////          1.......................pin_c5                         ////
////          2.......................pin_c0                         ////
////          3.......................pin_c1                         ////
////          4.......................pin_c2                         ////
////          5.......................pin_b0                         ////
////          6.......................pin_b2                         ////
////          7.......................pin_b1                         ////
////          8.......................Vcc(+5V)                       ////
////          9.......................pin_b7                         ////
////         10.......................pin_b5                         ////
////         11.......................pin_b4                         ////
////         12.......................pin_b3                         ////
////         13.......................pin_b6                         ////
////         14.......................pin_c4                         ////
////         15.......................pin_c3                         ////
////         16.......................Vss(gnd)                       ////
////                                                                 ////
////  This example will compile for the PCB, PCM, and PCH compilers  ////
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


#if defined(__PCB__)
#include <16C56.h>
#fuses HS,NOWDT,NOPROTECT
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_A3, rcv=PIN_A2)

#elif defined(__PCM__)
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

#include <input.c>
#include <d41256.c>

void main ( void )
{
   // dynMem is a type mode that reads and writes bytes to the dynamic memory
   int dynMem input_string [ 100 ];
   int buffer, index, i;

   // initialization routine must be called
   DynMemInit ( );

   index = 0;

   printf ( "Input String : " );
   while ( buffer != 0x0d )
   {
      if ( kbhit() )
      {
         buffer = getc ( );
         putc ( buffer );
         input_string [ index ] = buffer;
         index++;
      }
   }

   printf ( "\n\rBuffer reads : " );

   for ( i = 0 ; i < index + 1 ; i++ )
   {
      buffer = input_string [ i ];
      putc ( buffer );
   }
}
