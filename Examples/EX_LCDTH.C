/////////////////////////////////////////////////////////////////////////
////                         EX_LCDTH.C                              ////
////                                                                 ////
////  This example demonstartes the use of the DS1621 chip.  The     ////
////  example reads the temperature and then sends it over RS232 and ////
////  to a LCD.                                                      ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////     Connect pin D0 to a switch.                                 ////
////     Make the following connections:                             ////
////          DS1621 pin     Protoboard                              ////
////              1            pin C0 (also use pull up resistor)    ////
////              2            pin C1 (also use pull up resistor)    ////
////              3            No connection                         ////
////              4            Gnd                                   ////
////              5            Gnd                                   ////
////              6            Gnd                                   ////
////              7            Gnd                                   ////
////              8            +5V                                   ////
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
#include <18F452.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#endif

#include <lcd.c>
#include <ds1621.c>

#define RESET_BUTTON    PIN_d0

int current_temp, max_temp, min_temp;


void reset_temp()  {

   current_temp = read_temp();
   min_temp=current_temp;
   max_temp=current_temp;
}


void main() {

   init_temp();
   lcd_init();
   delay_ms(6);

   reset_temp();

   while(TRUE)
   {
      current_temp = read_temp();

      if(input(RESET_BUTTON)==0)
         reset_temp();
      else if(current_temp>max_temp)
         max_temp=current_temp;
      else if(current_temp<min_temp)
         min_temp=current_temp;

      printf(lcd_putc,"\fCurrent Temp: %U F\nMin: %U Max: %U",                        current_temp,min_temp,max_temp);
      delay_ms(500);
   }
}
