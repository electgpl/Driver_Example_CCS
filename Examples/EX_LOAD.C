/////////////////////////////////////////////////////////////////////////
////                          EX_LOAD.C                              ////
////                                                                 ////
////  This program uses the loader driver to show how you can        ////
////  read and write to an internal program memory.                  ////
////                                                                 ////
////  The LOADER.C will take an Intel 8-bit Hex file over RS232      ////
////  and modify the flash program memory with the new code.         ////
////                                                                 ////
////  After each good line, the loader sends an ACK character.  The  ////
////  driver uses XON/XOFF flow control.  Also, any buffer on the PC ////
////  UART must be turned off, or to its lowest setting, otherwise it////
////  will miss data.                                                ////
////                                                                 ////
////  Use an RS232 link and the SIOW.EXE program to load a new HEX   ////
////  file into the target chip.                                     ////
////                                                                 ////
////  This example will work with the PCM and PCH compilers.  The    ////
////  following conditional compilation lines are used to include a  ////
////  valid device for each compiler.  Change the device, clock and  ////
////  RS232 pins for your hardware if needed.                        ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2013 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#if defined(__PCM__)
#include <16F887.h>
#fuses NOWDT
#use delay(crystal=20MHz)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCH__)
#include <18F45K22.h>
#fuses NOWDT
#use delay(crystal=16MHz)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#endif

#include <loader.c>

void main()
{
   // Clears out the rs232 buffer
   delay_ms (1000);
   while (kbhit())
   {
      getc();
   }

   // Loop until 'L' is pressed
   do
   {
      printf("\r\nSoftware Version A!\r\n");
      printf("Press L to download new software.\r\n");
   } while (getc() != 'L');

   // Let the user know it is ready to accept a download
   printf("\r\nWaiting for download...");

   // Load the program
   load_program();
}
