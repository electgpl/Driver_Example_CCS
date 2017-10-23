/////////////////////////////////////////////////////////////////////////
////                          EX_EXTEE.C                             ////
////                                                                 ////
////  This program uses the 24xx or 93xx external EEPROM drivers to  ////
////  read and write to an external serial EEPROM.                   ////
////                                                                 ////
////  Change the #include <2416.C> to any of the other drivers to    ////
////  test other parts.  Note each driver defines EEPROM_ADDRESS     ////
////  indicate 8 or 16 bit addresses.                                ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////     EEPROM connections will vary depending on the device,       ////
////     for the 93C56 use the 9356.C driver and the default         ////
////     connections:                                                ////
////               9356 pin    Protoboard                            ////
////                  1            pin B7                            ////
////                  2            pin B6                            ////
////                  3            pin B5                            ////
////                  4            pin B4                            ////
////                  5            gnd                               ////
////                  6            gnd                               ////
////                  7            gnd                               ////
////                  8            +5V                               ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCB        pin A2 to RS232 RX, pin A3 to RS232 TX           ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////     PCD        UART1A                                           ////
////                                                                 ////
////  This example will work with the PCB, PCM, PCH, and PCD         ////
////  compilers.                                                     ////
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

#elif defined(__PCD__)
#include <30F2010.h>
#fuses HS,NOWDT,NOPROTECT
#use delay(clock=20000000)
#use rs232(baud=9600, UART1A)
#endif

#include "input.c"
#include "2416.c"


void main() {

   BYTE value, cmd;
   EEPROM_ADDRESS address;

   init_ext_eeprom();

   do {
      do {
         printf("\r\nRead or Write: ");
         cmd=getc();
         cmd=toupper(cmd);
         putc(cmd);
      } while ( (cmd!='R') && (cmd!='W') );

      printf("\n\rLocation: ");

#if sizeof(EEPROM_ADDRESS)==1
      address = gethex();
#else
#if EEPROM_SIZE>0xfff
      address = gethex();
#else
      address = gethex1();
#endif
      address = (address<<8)+gethex();
#endif

      if(cmd=='R')
         printf("\r\nValue: %X\r\n",READ_EXT_EEPROM( address ) );

      if(cmd=='W') {
         printf("\r\nNew value: ");
         value = gethex();
         printf("\n\r");
         WRITE_EXT_EEPROM( address, value );
      }
   } while (TRUE);
}
