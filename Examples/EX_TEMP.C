/////////////////////////////////////////////////////////////////////////
////                         EX_TEMP.C                               ////
////                                                                 ////
////  Reads temperature using the DS1621 and sends it over the RS232 ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCB        pin A2 to RS232 RX, pin A3 to RS232 TX           ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////                                                                 ////
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
#include <18F458.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#endif

#define DAL_SCL   PIN_B0
#define DAL_SDA   PIN_B1

#include <ds1621.c>


void main() {
   BYTE value;

   init_temp();

   do {
      value = read_temp();
      printf("%u\r\n",value);
      delay_ms(1000);
   } while (TRUE);
}
