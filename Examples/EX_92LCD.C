/////////////////////////////////////////////////////////////////////////
////                             EX_92LCD.C                          ////
////                                                                 ////
////  This program displays 0-9999 on a LCD connected directly to a  ////
////  923/924 chip.                                                  ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////     Use the 92x Adaptor with the 92x lcd module plugged into it.////
////                                                                 ////
////  This example will work with the PCM compiler.  The following   ////
////  conditional compilation lines are used to include a valid      ////
////  device for the compiler.  Change the device and clock for your ////
////  hardware if needed.                                            ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////


#if defined(__PCM__)
#include <16C924.h>
#fuses HS,NOWDT,PUT
#use delay(clock=20000000)
#endif


/////////////////////////////////////////////////////////////////////////////////////////
//                                 LCD Configuration                                   //
/////////////////////////////////////////////////////////////////////////////////////////
// Digit segments    A        B        C        D        E        F        G        DP
//                   b7       b6       b5       b4       b3       b2       b1       b0
#define DIGIT4    COM1+26, COM1+17, COM2+17, COM3+26, COM2+25, COM1+25, COM2+26, COM3+17
#define DIGIT3    COM1+24, COM1+16, COM2+16, COM3+24, COM2+23, COM1+23, COM2+24, COM3+16
#define DIGIT2    COM1+22, COM1+19, COM2+19, COM3+22, COM2+21, COM1+21, COM2+22, COM3+19
#define DIGIT1    COM1+20, COM1+18, COM2+18, COM3+20, COM2+28, COM1+28, COM2+20, COM3+18
//
//         character         0    1    2    3    4    5    6    7    8    9
byte const Digit_Map[10] = {0xFC,0x60,0xDA,0xF2,0x66,0xB6,0xBE,0xE0,0xFE,0xE6};

#define BLANK 0
#define DASH 11
/////////////////////////////////////////////////////////////////////////////////////////


byte lcd_pos;

void lcd_putc(char c) {
   byte segments;

   if(c=='\f')
     lcd_pos=0;
   else {
      if((c>='0')&&(c<='9'))
         segments=Digit_Map[c-'0'];
      else
         segments=BLANK;
      switch(lcd_pos) {
        case 1 : lcd_symbol(segments,DIGIT4); break; // fill 1000s place
        case 2 : lcd_symbol(segments,DIGIT3); break; // fill  100s place
        case 3 : lcd_symbol(segments,DIGIT2); break; // fill   10s place
        case 4 : lcd_symbol(segments,DIGIT1); break; // fill    1s place
      }
   }
   lcd_pos++;
}


void main() {
   long number = 0;

   setup_lcd(LCD_MUX14|LCD_STOP_ON_SLEEP,2,ALL_LCD_PINS);

   while(TRUE) {
      printf(lcd_putc,"\f%4lu",number);
      if(number++==10000)
        number=0;
      delay_ms(100);
   }
}
