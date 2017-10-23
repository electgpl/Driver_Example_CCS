/////////////////////////////////////////////////////////////////////////
////                           EX_LCDKB.C                            ////
////                                                                 ////
////  This program uses both the KBD.C and LCD.C drivers to allow    ////
////  keypad entry and LCD display.  All keys are echoed except *    ////
////  that will clear the display.  Either the kbd_getc or lcd_putc  ////
////  may be replaced with getc or putc to use just one device with  ////
////  the RS-232.                                                    ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////     Plug in both the LCD and KEYPAD.                            ////
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

#elif defined(__PCM__)
#include <16F877.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)

#elif defined(__PCH__)
#include <18F458.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#endif

#include <lcd.c>
#include <kbd.c>


void main() {
   char k;

   lcd_init();
   kbd_init();

   lcd_putc("\fReady...\n");

   while (TRUE) {
      k=kbd_getc();
      if(k!=0)
        if(k=='*')
          lcd_putc('\f');
        else
          lcd_putc(k);
   }
}
