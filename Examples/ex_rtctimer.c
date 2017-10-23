///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                           ex_rtctimer.c                           ////
////                                                                   ////
//// Uses the Timer2 interrupt to create a real time clock.  API       ////
//// follows the standard C time.h library, with a few minor           ////
//// exceptions (see timer.h for documentation).  The rtctimer.c       ////
//// library provides the time base in which a tick timer is           ////
//// incremented on timer2 overflows.  SetTimer() needs to be run      ////
//// first to iniatialize the RTC to the current date.                 ////
////                                                                   ////
//// First, the example will get the current time from the user to     ////
//// initialize the RTC.  Then, every second it will display the       ////
//// current time over the serial port.                                ////
////                                                                   ////
//// Optionally, the time can be displayed on an LCD.  Uncomment       ////
//// _USE_LCD_ to turn on this feature.                                ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2011 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS    ////
//// C compiler.  This source code may only be distributed to other    ////
//// licensed users of the CCS C compiler.  No other use,              ////
//// reproduction or distribution is permitted without written         ////
//// permission.  Derivative programs created using this software      ////
//// in object code form are not restricted in any way.                ////
///////////////////////////////////////////////////////////////////////////

#if defined(__PCM__)
#include <16F877A.h>
#fuses HS,NOWDT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCH__)
#include <18F4580.h>
#fuses HS,NOWDT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCD__)
#include <24FJ128GA006.h>
#fuses PR,HS,NOWDT
#use delay(clock=20000000)
#use rs232(UART1,baud=9600)
#endif

//#define _USE_LCD_       //uncomment this if you want to use an LCD

//we are using the rtctimer.c library, in which a counter is incremented
//every time the timer2 interrupt occurs (timer2 overflow).  the time math
//needs to know what rate the timer2 interrupt occurs.  this definition
//must match the rate the timer2 is configured for.
#define CLOCKS_PER_SECOND  1000

///////////////// end configuration ///////////////////////

#include <time.h>       //standard C time library
#include <rtctimer.c>   //library for time.h that uses timer2 as time base

#include <stdlib.h>
#include <input.c>      //needed for the rs232 input routines

#if defined(_USE_LCD_)
#include <lcd.c>        //LCD display
#endif

void InitTime(void)
{
   struct_tm t;
   
   //tm_year is years since 1900.
   printf("\r\nYear (0-99): ");
   t.tm_year = (unsigned int16)get_int() + (unsigned int16)100;   //add 100 to put is into 2000
   
   printf("\r\nMonth (1-12): ");
   t.tm_mon = get_int() - 1;
   
   printf("\r\nDay (1-31): ");
   t.tm_mday = get_int() - 1;
   
   printf("\r\nHour (0-23): ");
   t.tm_hour = get_int();
   
   printf("\r\nMinute (0-59): ");
   t.tm_min = get_int();
   
   SetTime(&t);
   
   printf("\r\n\n");
}

void main(void)
{  
   char tString[32];
   unsigned int8 i = 0;
   time_t tTime = 0;
   
   /* Setup the LCD */
  #if defined(_USE_LCD_)
   lcd_init();
  #endif
  
   printf("\r\n\nex_rtctimer\r\n\n");
  
   InitTime();

   /* Setup timer 2 PCM and PCH
    * On a 4 Mhz clock, this will trigger a timer2 interrupt every 1.0 ms
    * For time.h to work properly, Timer2 must overflow every millisecond
    * OverflowTime = 4 * (1/OscFrequency) * Prescale * Period * Postscale 
    * For 4 Mhz: .001 seconds  = 4 * (1/4000000 seconds) * 4 * 250 * 1
    *
    * Setup timer 2 PCD
    * OverflowTime = 2 * (1/OscFrequency) * Prescale * Period
    * For 4 MHz: .001 seconds = 2 * (1/4000000 seconds) * 1 * 2000
    */
    
  #if getenv("CLOCK")==4000000)
   #ifdef __PCD__
    setup_timer2(TMR_INTERNAL | TMR_DIV_BY_1, 2000);
   #else
    setup_timer_2(T2_DIV_BY_1,250,4);
   #endif
  #elif getenv("CLOCK")==20000000)
   #ifdef __PCD__
    setup_timer2(TMR_INTERNAL | TMR_DIV_BY_1, 10000);
   #else
    setup_timer_2(T2_DIV_BY_4,250,5);
   #endif
  #else
   #error Configure TIMER2 so it interrupts at a rate defined by CLOCKS_PER_SECOND
  #endif
   
   /* Enable the timer 2 interrupt, or it will not fire */
   enable_interrupts(INT_TIMER2);
   /* Enable interrupts globally too, otherwise no interrupt will fire */
   #ifdef __PCD__
    enable_interrupts(INTR_GLOBAL);
   #else
    enable_interrupts(GLOBAL);
   #endif
   
   while(1)
   {
      /* Get the time */
      tTime = time(NULL);
      
      /* Get the string representation of the time */
      ctime(&tTime, tString);
      
      /* Print the time to RS-232 */
      printf("It is currently: %s\n\r", tString);
      
      /* Print the time to an LCD */
     #if defined(_USE_LCD_)
      lcd_gotoxy(1,1);
      i = 3;   //skip the first 3 characters, not enough space on LCD.
      while(tString[i] != '\0')
      {
         lcd_putc(tString[i]);
         i++;
      }
     #endif
      
      delay_ms(1000);
   }
}
