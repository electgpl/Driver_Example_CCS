///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                          rtcperipheral.c                          ////
////                                                                   ////
//// Using the built-in RTC provided on some devices, provides a time  ////
//// base and the functions needed for a time.h compatable driver.     ////
//// See time.h for documentation of the time.h library.               ////
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

#ifndef __RTCPERIPHERAL_C__
#define __RTCPERIPHERAL_C__

#include <time.c>

#define CLOCKS_PER_SECOND  -1

/* Globals & Resources */
static time_t calendar_time; //seconds since Jan 1, 1970 00:00:00

/* Returns processor time used by program (clock ticks) 
 * The built-in RTC does not use ticks, so return 0
 */
clock_t clock()
{
   return 0;
}

/* Returns the current calendar time.
 * First gets the time from the built-in RTC, converts it to seconds,
 * and sets this as the calendar time.
 * If a timer is specified, it will be set to the current calendar time.
 * It is assumed that SetTime has already been called and the
 * built-in RTCC is initialized.
 */
time_t time(time_t * timer)
{
   struct_tm nTime;
   rtc_time_t datetime;
   rtc_read(&datetime);
   
   // adjust date to be time.h compatible
   nTime.tm_year = datetime.tm_year + 100; // starting at 1900
   nTime.tm_mon = datetime.tm_mon - 1;     // 0-11
   nTime.tm_mday = datetime.tm_mday - 1;   // 0-30
   
   nTime.tm_hour = datetime.tm_hour;
   nTime.tm_sec = datetime.tm_sec;
   nTime.tm_min = datetime.tm_min;
   
   // mktime() calculates the yday and wday
   
   calendar_time = mktime(&nTime);

   if(timer != NULL)
   {
      *timer = calendar_time;
   }
   
   return calendar_time;
}

/* Sets the current calendar time (in seconds) from the given time,
 * and sets the time on and enables the built-in RTC
 */
void SetTime(struct_tm * nTime)
{   
   rtc_time_t datetime;
   calendar_time = mktime(nTime);  // will also set the yday and wday
   
   // adjust date to be built-in RTC compatible
   datetime.tm_year = nTime->tm_year - 100; // 20(00-99)
   datetime.tm_mon = nTime->tm_mon + 1;     // 1-12
   datetime.tm_mday = nTime->tm_mday + 1;   // 1-31
   datetime.tm_yday = 0; 
   
   datetime.tm_hour = nTime->tm_hour;
   datetime.tm_wday = nTime->tm_wday +1; //1-7
   datetime.tm_sec = nTime->tm_sec;
   datetime.tm_min = nTime->tm_min;
   
   rtc_write(&datetime);
}

/* Sets the current time on the built-in RTC given a time in seconds and
 * sets the calendar time to this time.
 */
void SetTimeSec(time_t sTime)
{
   struct_tm * nTime;
   nTime = localtime(&sTime);
   calendar_time = sTime;
   
   rtc_time_t datetime;
   
   // adjust date to be built-in RTC compatible
   datetime.tm_year = nTime->tm_year - 100; // 20(00-99)
   datetime.tm_mon = nTime->tm_mon + 1;     // 1-12
   datetime.tm_mday = nTime->tm_mday + 1;   // 1-31
   datetime.tm_wday = nTime->tm_wday +1;    // 1-7
   
   datetime.tm_hour = nTime->tm_hour;
   datetime.tm_sec = nTime->tm_sec;
   datetime.tm_min = nTime->tm_min;
   
   rtc_write(&datetime);
}

/* Returns the current time from the built-in RTC as a time struct
 */
void GetTime(struct_tm *pRetTm)
{
   struct_tm cTime;
   rtc_time_t datetime;
   rtc_read(&datetime);
   
   // adjust date to be time.h compatible
   cTime.tm_year = datetime.tm_year + 100; // starting at 1900
   cTime.tm_mon = datetime.tm_mon - 1;     // 0-11
   cTime.tm_mday = datetime.tm_mday - 1;   // 0-30
   cTime.tm_wday = datetime.tm_wday - 1;   // 0-6
   
   cTime.tm_hour = datetime.tm_hour;
   cTime.tm_sec = datetime.tm_sec;
   cTime.tm_min = datetime.tm_min;
   
   memcpy(pRetTm, &cTime, sizeof(struct_tm));
}


/* Initializes The Built-In RTC
 */
void TimeInit(void)
{
   setup_rtc(RTC_ENABLE | RTC_OUTPUT_SECONDS, 0);
}

#endif //__RTCPERIPHERAL_C__