/////////////////////////////////////////////////////////////////////////////
///                                                                       ///
///                               time.h                                  ///
///                                                                       ///
/// Time algorithms.  This follows the standard C API, with the following ///
/// exceptions:                                                           ///
///   * ctime() has another input parameter which is the pointer to where ///
///     the input string is.  Normally ctime() and asctime() write the    ///
///     output to a globally allocated string and return a pointer to     ///
///     this string.  This library doesn't want to make this assumption   ///
///     so the library doesn't allocate the space needed for those        ///
///     functions.                                                        ///
///   * asctime() has nother input parameter which is the pointer to      ///
///     where the input string is.  See the above paragraph.              ///
///   * strftime() is not supported.                                      ///
///   * SetTime() is added that initializes/set the current time.         ///
///   * GetTime() is added so you can read directly into a struct_tm,     ///
///      instead of having to use localtime(time()) which is usually      ///
///       innefecient.                                                    ///
///   * TimeInit() added for any real time clock devices that may need    ///
///      to be initialized first.                                         ///
///   * Not all timebases have a tick system, so it's not recommended     ///
///      to use clock().  If you need clock(), look at CCS's              ///
///      #use timer() library.                                            ///
///                                                                       ///
/// This file only provides the prototypes and definitions needed to      ///
/// proved a time alogrithm that follows the C standard library.  You     ///
/// also need to include/link the actual library that performs the time   ///
/// base.  As of this writing CCS provides the following compatible       ///
/// timebase libraries:                                                   ///
///      rtcperipheral.c - for PICs with internal real time clock.        ///
///      ds1305.c - external DS1305 real time clock.                      ///
///      rtcticks.c - Use a PIC's timer with CCS #use timer() library.    ///
///                                                                       ///
/// API:                                                                  ///
///                                                                       ///
/// Variable definitions:                                                 ///
///   c - clock timer (clock_t), number of ticks since powerup.  See      ///
///       CLOCKS_PER_SECOND to determine clock rate.                      ///
///                                                                       ///
///   t - second timer (time_t), number of seconds since Jan 1st, 1970.   ///
///                                                                       ///
///   ts - time struct (struct_tm), a structure that holds time in        ///
///        descriptive format (seconds, minutes, hours, day, month, etc). ///
///                                                                       ///
/// CLOCKS_PER_SECOND - This is a constant which needs to be defined that ///
///   configures the timebase used by the clock timer and clock().        ///
///   If you are not using clock() then you don't need to define this.    ///
///   If you are using a method such a PIC's timer for the timebase then  ///
///   you will need to set this.                                          ///
///                                                                       ///
/// c = clock() - Return current clock timer.                             ///
///                                                                       ///
/// t = time(*t) - Return current second timer.  Returns twice (as a      ///
///         a return, and saves to input pointer).                        ///
///                                                                       ///
/// SetTime(*tm) - Initializes the current time with a struct_tm          ///
///                                                                       ///
/// SetTimeSec(t) - Initializes the current time with a seconds time      ///
///                                                                       ///
/// t = mktime(*tm) - Converts a time struct to a second timer.           ///
///                                                                       ///
/// t = difftime(t,t) - Returns difference between two second timers.     ///
///                                                                       ///
/// *char = ctime(*t, *char) - Converts second timer to a readable string ///
///                            Www Mmm dd hh:mm:ss yyyy                   ///
///                                                                       ///
/// *char = asctime(*ts, *char) - Converts time struct to a readable      ///
///                            string.                                    ///
///                            Www Mmm dd hh:mm:ss yyyy                   ///
///                                                                       ///
/// *ts = localtime(*t) - Converts second timer to a time struct.         ///
///                  *ts points to a global time struct and will be       ///
///                  corrupted in future calls to localtime().            ///
///                                                                       ///
/// GetTime(*tm) - Returns the current time as a time struct              ///
///                                                                       ///
/// TimeInit() - Initializes the timing device                            ///
///                                                                       ///
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2011 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS    ////
//// C compiler.  This source code may only be distributed to other    ////
//// licensed users of the CCS C compiler.  No other use,              ////
//// reproduction or distribution is permitted without written         ////
//// permission.  Derivative programs created using this software      ////
//// in object code form are not restricted in any way.                ////
///////////////////////////////////////////////////////////////////////////

#ifndef __TIME_H__
#define __TIME_H__

#include <stdlibm.h>

/* API Types*/
typedef signed int32 time_t;
typedef unsigned int32 clock_t;

typedef enum
{
   SUNDAY = 0,
   MONDAY,
   TUESDAY,
   WEDNESDAY,
   THURSDAY,
   FRIDAY,
   SATURDAY   
}  Weekday;

typedef enum
{
   JANUARY = 0,
   FEBRUARY,
   MARCH,
   APRIL,
   MAY,
   JUNE,
   JULY,
   AUGUST,
   SEPTEMBER,
   OCTOBER,
   NOVEMBER,
   DECEMBER
}  Month;

typedef struct
{
   unsigned int8 tm_sec;   // seconds after the minute (0-59)
   unsigned int8 tm_min;   // minutes after the hour (0-59)
   unsigned int8 tm_hour;  // hours since midnight (0-23)
   unsigned int8 tm_mday;  // day of the month (0-30)
   Month tm_mon;           // month of the year (0-11)
   unsigned int16 tm_year; // years since 1900
   Weekday tm_wday;        // day of the week (0-6) (Sunday=0)
   unsigned int16 tm_yday; // day of the year (0-365)
} struct_tm;


/* Functions */
clock_t clock(void);
time_t time(time_t * timer);
signed int32 difftime(time_t later, time_t earlier);
time_t mktime(struct_tm * timeT);

char * asctime(struct_tm * timeptr, char *szTime);
char * ctime(time_t * timer, char *szTime);
struct_tm * localtime ( time_t * timer );
void SetTime(struct_tm * nTime);
void SetTimeSec(time_t sTime);
void GetTime(struct_tm *pRetTm);
void TimeInit(void);

#endif
