/////////////////////////////////////////////////////////////////////////
////                          EX_SRAM.C                              ////
////                                                                 ////
////  This program uses the PCF8570.C SRAM driver to read and write  ////
////  to an external (2 wire) serial SRAM.                           ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////     Insert jumpers from:                                        ////
////           PCF8570      Protoboard                               ////
////               1            gnd                                  ////
////               2            gnd                                  ////
////               3            gnd                                  ////
////               4            gnd                                  ////
////               5            pin B7                               ////
////               6            pin B6                               ////
////               7            gnd                                  ////
////               8            +5V                                  ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCB        pin A2 to RS232 RX, pin A3 to RS232 TX           ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////                                                                 ////
////  The 68HC68R1.C or 68HC68R2.C drivers may be used in place of   ////
////  the PCF8570.C driver.                                          ////
////  This example will work with the PCB, PCM and PCH compilers.    ////
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
#include <pcf8570.c>


void main() {

   BYTE value,cmd;
   BYTE address;

   do {
      do {
         printf("\r\nRead or Write: ");
         cmd=getc();
         cmd=toupper(cmd);
         putc(cmd);
      } while ( (cmd!='R') && (cmd!='W') );

      printf("\n\rLocation: ");

      address = gethex();

      if(cmd=='R')
         printf("\r\nValue: %X\r\n",READ_EXT_SRAM( address ) );

      if(cmd=='W') {
         printf("\r\nNew value: ");
         value = gethex();
         printf("\n\r");
         WRITE_EXT_SRAM( address, value );
      }
   } while (TRUE);

}
