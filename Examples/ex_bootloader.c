///////////////////////////////////////////////////////////////////////////
////                      EX_BOOTLOADER.C                              ////
////                                                                   ////
////  This program is an example stand alone bootloader.               ////
////                                                                   ////
////  This program must be loaded into a target chip using a device    ////
////  programmer.  Afterwards this program may be used to load new     ////
////  versions of the application program.                             ////
////                                                                   ////
////  This bootloader is designed to detect pin B5 low on reset.  It   ////
////  will then use the RS232 link to download a new program.          ////
////  Otherwise the application program is started.                    ////
////                                                                   ////
////  Use an RS232 link and the SIOW.EXE program to load a new HEX     ////
////  file into the target chip.                                       ////
////                                                                   ////
////  This example will work with the PCM and PCH compilers.  The      ////
////  following conditional compilation lines are used to include a    ////
////  valid device for each compiler.  Change the device, clock and    ////
////  RS232 pins for your hardware if needed.                          ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2013 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS    ////
//// C compiler.  This source code may only be distributed to other    ////
//// licensed users of the CCS C compiler.  No other use,              ////
//// reproduction or distribution is permitted without written         ////
//// permission.  Derivative programs created using this software      ////
//// in object code form are not restricted in any way.                ////
///////////////////////////////////////////////////////////////////////////

#if defined(__PCM__)
#include <16F887.h>
#fuses NOWDT
#use delay(crystal=20MHz)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#define PUSH_BUTTON PIN_A4

#elif defined(__PCH__)
#include <18F45K22.h>
#fuses NOWDT
#use delay(crystal=16MHz)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#define PUSH_BUTTON PIN_A4
#endif

#define _bootloader

#include <bootloader.h>
#include <loader.c>

#if defined(__PCM__)
 #org LOADER_END+1,LOADER_END+2
#elif defined(__PCH__)
 #org LOADER_END+2,LOADER_END+4
#endif
void application(void) {
  while(TRUE);
}

void main(void) {

   if(!input(PUSH_BUTTON))
   {
      printf("\r\nBootloader Version 1.0\r\n");
   
      // Let the user know it is ready to accept a download
      printf("\r\nWaiting for download...");
      load_program();
   }

   application();
}

#int_global
void isr(void) {
   jump_to_isr(LOADER_END+5*(getenv("BITS_PER_INSTRUCTION")/8));
}


