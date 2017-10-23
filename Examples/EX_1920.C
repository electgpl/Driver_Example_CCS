/////////////////////////////////////////////////////////////////////////
////                             EX_1920.C                           ////
////                                                                 ////
////  This program interfaces a Dallas DS1920 touch memory device.   ////
////  It will display the temperature reading of the device it       ////
////  "touches" with pin B0.                                         ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////     Connect pin B0 through a 4.7K resistor to 5V                ////
////     Connect the touch device to ground and then "Touch" to B0   ////
////     See additional connections below.                           ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCB        pin A2 to RS232 RX, pin A3 to RS232 TX           ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////                                                                 ////
////  The device requires 2 seconds of contact to charge enough power////
////  for the temperature conversion.                                ////
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
#include <18F452.h>
#fuses HS,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#endif

#include <touch.c>
#include <string.h>
#include <stdlib.h>

void main() {
   byte buffer[2];

   printf("\r\nTemperature requires at least 2 seconds of contact.");
   printf("\r\nWaiting for a touch device...\r\n");

   while (TRUE) {
     if(touch_present()) {
        touch_write_byte(0xCC);
        touch_write_byte (0x44);
        output_high(TOUCH_PIN);
        delay_ms(2000);

        touch_present();
        touch_write_byte(0xCC);
        touch_write_byte (0xBE);
        buffer[0] = touch_read_byte();
        buffer[1] = touch_read_byte();

        printf ("\r\nTemperature: %c%3.1f C", (buffer[1])?'-':' ', (float)buffer[0]/2);
        delay_ms (1000);
     }
  }
}
