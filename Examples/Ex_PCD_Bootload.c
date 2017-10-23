///////////////////////////////////////////////////////////////////////////
////                       EX_PCD_BOOTLOAD.c                           ////
////                                                                   ////
////  This program is an example application program that can be       ////
////  be loaded, recompiled and reloaded by a standalone bootloader.   ////
////                                                                   ////
////  Before loading this program the bootloader (EX_PCD_BOOTLOADER.c) ////
////  must be compiled and loaded into the target chip.                ////
////                                                                   ////
////  Use an RS232 link and the SIOW.EXE program to load this HEX file ////
////  into the target chip.  See ex_pcd_bootloader.c for more          ////
////  information.                                                     ////
////                                                                   ////
////  This example will work with the PCD compiler.  The following     ////
////  conditional compilation lines are used to include a valid device ////
////  for the compiler.  Change the device, clock and RS232 pins for   ////
////  your hardware if needed.                                         ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2013 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS    ////
//// C compiler.  This source code may only be distributed to other    ////
//// licensed users of the CCS C compiler.  No other use,              ////
//// reproduction or distribution is permitted without written         ////
//// permission.  Derivative programs created using this software      ////
//// in object code form are not restricted in any way.                ////
///////////////////////////////////////////////////////////////////////////

//These defines control the chip family that this program will compile for.
//The setup for 4 different chips, one from each of these chip families,
//is listed below.
//#define DSPIC33FJ
//#define DSPIC30F
#define PIC24FJ
//#define PIC24HJ

#if defined(DSPIC33FJ)
   #include <33FJ128GP706.h>
   #fuses NOWDT
   #use delay(crystal=12MHz)
   #use rs232(BAUD=9600,UART2)
#elif defined(DSPIC30F)
   #include <30F4012.h>
   #fuses NOWDT
   #use delay(crystal=20M)
   #use rs232(BAUD=9600,UART1A)
#elif defined(PIC24FJ)
   #include <24FJ128GA006.h>
   #fuses NOWDT
   #use delay(crystal=20MHz)
   #use rs232(BAUD=9600,UART1)
#elif defined(PIC24HJ)
   #include <24HJ256GP610.h>
   #fuses NOWDT
   #use delay(crystal=20MHz)
   #use rs232(Baud=9600,UART1)
#endif

//This is a necessary include file.  It reserves space so that the
//bootloader is not overwritten.
#include <pcd_bootloader.h>

void main()
{
   delay_ms(100);

   printf("\r\nApplication Version 1.0\r\n");
   printf("\r\nYou can put whatever code you want here.");
   printf("\r\nSo enjoy!");

   while(TRUE);
}
