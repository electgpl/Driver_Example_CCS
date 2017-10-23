///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                            rtcticks.c                             ////
////                                                                   ////
//// Using the #USE TIMER() directive, provides a time base and        ////
//// the functions needed for a time.h compatable driver.  See time.h  ////
//// for documentation of the time.h library.                          ////
////                                                                   ////
//// Options for #USE TIMER:                                           ////
//// TIMER=x : which PIC timer to use                                  ////
//// TICK=xx : the desired time for 1 tick - units of s, ms, us, ns    ////
//// BITS=x  : size of value returned by get_ticks() and used by       ////
////           set_ticks(). 8 for 8 bits, 16 for 16 bits, 32 for 32bits////
//// ISR   : timer's interrupt used to update tick timer. GLOBAL       ////
////         INTERRUPT MUST BE ENABLED BY MAIN PROGRAM BEFORE USE.     ////
//// NOISR : get_ticks() updates tick timer. get_ticks() MUST BE       ////
////         CALLED MORE OFTEN THAN THE TIMER'S OVERFLOW RATE.         ////
////         Interrupt need not be enabled for this mode.              ////
////                                                                   ////
////                                                                   ////
//// Alternately you can provide your own Tick system if you override  ////
//// the GET_RTC_TICKS and CLOCKS_PER_SECOND macro before including    ////
//// this file.                                                        ////
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

#ifndef __RTCTICKS_C__
#define __RTCTICKS_C__

#include <time.c>

#ifndef GET_RTC_TICKS
   #use timer(TICK=10ms, BITS=32, ISR)
   //if you change the above #use timer() to use ISR, then you need
   //to enable_interrupts(GLOBAL) in order for timer ISR to be triggered.
   
   #define GET_RTC_TICKS()       get_ticks()
   
   #define CLOCKS_PER_SECOND  TICKS_PER_SECOND
   
  #if defined(__PCD__)
   //PCD has global interrupts enabled by default
   #define INIT_RTC_TICKS()
  #else
   //enable interrupts because our #use timer() above is using ISR option.
   #define INIT_RTC_TICKS()   enable_interrupts(GLOBAL)
  #endif
#endif

#ifndef INIT_RTC_TICKS
#define INIT_RTC_TICKS()
#endif

/* Globals & Resources */
static clock_t clock_ticks;
static time_t calendar_time; //seconds since Jan 1, 1970 00:00:00

/* Returns processor time used by program (clock ticks)
 * Ticks occur at a rate specified by CLOCKS_PER_SECOND
 */
clock_t clock(void)
{
   while (((clock_t)GET_RTC_TICKS() - clock_ticks) >= (clock_t)CLOCKS_PER_SECOND)
   {
      calendar_time += 1;
      clock_ticks += (clock_t)CLOCKS_PER_SECOND;
   }
   
   return((clock_t)GET_RTC_TICKS());
}

/* Returns the current calendar time.
 */
time_t time(time_t * timer)
{
   clock();
   
   if (!timer)
      *timer = calendar_time;
      
   return(calendar_time);
}

/* Sets the current calendar time (in seconds) from the given time
 * and resets the timer ticks.
 */
void SetTime(struct_tm * nTime)
{   
   calendar_time = mktime(nTime);
}

/* Sets the current calendar time from the given seconds time
 * and resets the timer ticks.
 */
void SetTimeSec(time_t sTime)
{   
   calendar_time = sTime;
}

/* Returns the current time as a struct_tm
 */
void GetTime(struct_tm *pRetTm)
{
   struct_tm *p;
   
   clock();
   
   p = localtime(&calendar_time);
   
   memcpy(pRetTm, p, sizeof(struct_tm));
}


/* Initializes the timer
 * The work is done by #use timer(), so just reset the ticks.
 */
void TimeInit(void)
{
   INIT_RTC_TICKS();
   clock_ticks = GET_RTC_TICKS(); 
   calendar_time = 0;
}

#endif
