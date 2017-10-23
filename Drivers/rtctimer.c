/*
   This library is the older timebase that CCS provided for time.h.  It 
   is provided for backwards compatiblity with older projects.
   
   It is strongly suggested moving to rtcticks.c instead, as it uses
   the #use timer().  The #use timer() library makes it very easy to
   change between PIC architectures and PIC clock speeds.
*/
#warning Not maintained, try to use rtcticks.c instead.  See comments above.

///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                            rtctimer.c                             ////
////                                                                   ////
//// Using the timer2 overflow interrupt, provides a time base and     ////
//// the functions needed for a time.h compatable driver.  See time.h  ////
//// for documentation of the time.h library.                          ////
////                                                                   ////
//// TimeInit() initializes TIMER2 and enables interrupts.             ////
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

#ifndef __RTCTIMER_C__
#define __RTCTIMER_C__

#include <time.c>

#define CLOCKS_PER_SECOND 1000 // overflow every 1 ms

/* Globals & Resources */
static clock_t clock_ticks;
static time_t calendar_time; //seconds since Jan 1, 1970 00:00:00

/* Returns processor time used by program (clock ticks of ISR) 
 */
clock_t clock()
{
   return clock_ticks;
}

/* Returns the current calendar time.
 * If a timer is specified, it will be set to the current calendar time.
 */
time_t time(time_t * timer)
{
   if(timer != NULL)
   {
      *timer = calendar_time;
   }
   
   return calendar_time;
}

/* Sets the current calendar time (in seconds) from the given time
 */
void SetTime(struct_tm * nTime)
{   
   calendar_time = mktime(nTime);  
}

/* Sets the current calendar time (in seconds) from the given time
 */
void SetTimeSec(time_t sTime)
{   
   calendar_time = sTime; 
}

/* Returns the current time as a struct_tm
 */
void GetTime(struct_tm *pRetTm)
{
   struct_tm *lTime;
   
   lTime = localtime(&calendar_time);
   
   memcpy(pRetTm, lTime, sizeof(struct_tm));
}

/* Initializes the timer to interrupt at a rate of CLOCKS_PER_SECOND.
 * The default is 1000 clocks per second, or 1ms per interrupt.
 * If your clock frequency is not listed (4MHz,20MHz,40MHz), you 
 * must add support for your device.
 */
void TimeInit(void)
{
  // 4 MHz Clock
  #if getenv("CLOCK")==4000000)
   #ifdef __PCD__
    setup_timer2(TMR_INTERNAL | TMR_DIV_BY_1, 2000);
   #else
    setup_timer_2(T2_DIV_BY_1,250,4);
   #endif
  // 20 MHz Clock
  #elif getenv("CLOCK")==20000000)
   #ifdef __PCD__
    setup_timer2(TMR_INTERNAL | TMR_DIV_BY_1, 10000);
   #else
    setup_timer_2(T2_DIV_BY_4,250,5);
   #endif
  // 40 MHz Clock
  #elif getenv("CLOCK")==40000000)
   #ifdef __PCD__
    setup_timer2(TMR_INTERNAL | TMR_DIV_BY_1, 20000);
   #else
    setup_timer_2(T2_DIV_BY_4,250,10);
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
}

/* Timer2 Overflow interrupt routine to update the current time.
 * CLOCKS_PER_SECOND must match the Timer2 Overflow rate 
 * to accurately measure time.
 */
#int_timer2
void updateClock()
{
 #if CLOCKS_PER_SECOND >= 0x10000
   static unsigned int32 t;
 #elif CLOCKS_PER_SECOND >= 0x100
   static unsigned int16 t;
 #else
   static unsigned int8 t;
 #endif
   
   t++;
   clock_ticks++;
   
   if(t >= CLOCKS_PER_SECOND)
   {
      t = 0;
      calendar_time++;  //seconds since Jan 1, 1970 00:00:00
   }
}

#endif
