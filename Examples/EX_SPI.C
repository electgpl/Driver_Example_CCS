/////////////////////////////////////////////////////////////////////////
////                          EX_SPI.C                               ////
////                                                                 ////
////  This program uses the 9356SPI external EEPROM driver to read   ////
////  and write to an external serial EEPROM using SPI.              ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////               9356 pin    Protoboard                            ////
////                  1            pin B0                            ////
////                  2            pin C3                            ////
////                  3            pin C5                            ////
////                  4            pin C4                            ////
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
////  This example will work with the PCB, PCM, PCD, and PCH         ////
////  compilers. The following conditional compilation lines are used////
////  to include a valid device for each compiler.  Change the       ////
////  device, clock and RS232 pins for your hardware if needed.      ////
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
#include <18F458.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCD__)
#include <30F2010.h>
#fuses HS, NOWDT, NOPROTECT
#use delay(clock=20000000)
#use rs232(baud=9600, UART1A)
#endif

#include <input.c>
#include <9356.c>

void main() {

   byte value,cmd;
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

      address = gethex();

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
