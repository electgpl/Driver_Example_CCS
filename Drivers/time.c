///////////////////////////////////////////////////////////////////////////
///                                                                     ///
///                              time.c                                 ///
///                                                                     ///
/// Contains common time routines that can be used by several           ///
/// timebases, and the runtime code needed for time.h.                  ///
///                                                                     ///
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2012 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS    ////
//// C compiler.  This source code may only be distributed to other    ////
//// licensed users of the CCS C compiler.  No other use,              ////
//// reproduction or distribution is permitted without written         ////
//// permission.  Derivative programs created using this software      ////
//// in object code form are not restricted in any way.                ////
///////////////////////////////////////////////////////////////////////////

#ifndef __TIME_C__
#define __TIME_C__

/* Returns the number of days in a given month, leap year dependent
 */
unsigned int8 DaysInMonth(unsigned int8 month, int1 IsLeapYear)
{
   switch(month)
   {
      case JANUARY:
      case MARCH:
      case MAY:
      case JULY:
      case AUGUST:
      case OCTOBER:
      case DECEMBER:
         return 31;

      case FEBRUARY:
         if(IsLeapYear)
            return 29;            
         return 28;

      case APRIL:
      case JUNE:
      case SEPTEMBER:
      case NOVEMBER:
         return 30;
      
      // error
      default:
         return 0;
   }
} 

/* Formats a given week day (as an integer) to a string
 */
void WeekdayAbbreviations(unsigned int8 day, char* wString)
{
   switch(day)
   {
      case SUNDAY:
      memcpy(wString,"Sun\0",4);
      break;
      
      case MONDAY:
      memcpy(wString,"Mon\0",4);
      break;
      
      case TUESDAY:
      memcpy(wString,"Tue\0",4);
      break;
      
      case WEDNESDAY:
      memcpy(wString,"Wed\0",4);
      break;
      
      case THURSDAY:
      memcpy(wString,"Thu\0",4);
      break;
      
      case FRIDAY:
      memcpy(wString,"Fri\0",4);
      break;
      
      case SATURDAY:
      memcpy(wString,"Sat\0",4);
      break;
      
      default:
      memcpy(wString,"Invalid\0",8);
      break;
   }
}

/* Formats a given month (as an integer) to a string
 */
void MonthAbbreviations(unsigned int8 month, char* mString)
{
   switch(month)
   {
      case JANUARY:
      memcpy(mString,"Jan\0",4);
      break;
      
      case FEBRUARY:
      memcpy(mString,"Feb\0",4);
      break;
      
      case MARCH:
      memcpy(mString,"Mar\0",4);
      break;
      
      case APRIL:
      memcpy(mString,"Apr\0",4);
      break;
      
      case MAY:
      memcpy(mString,"May\0",4);
      break;
      
      case JUNE:
      memcpy(mString,"Jun\0",4);
      break;
      
      case JULY:
      memcpy(mString,"Jul\0",4);
      break;
      
      case AUGUST:
      memcpy(mString,"Aug\0",4);
      break;
      
      case SEPTEMBER:
      memcpy(mString,"Sep\0",4);
      break;
      
      case OCTOBER:
      memcpy(mString,"Oct\0",4);
      break;
      
      case NOVEMBER:
      memcpy(mString,"Nov\0",4);
      break;
      
      case DECEMBER:
      memcpy(mString,"Dec\0",4);
      break;
      
      default:
      memcpy(mString,"Invalid\0",8);
      break;
   }
}
/* Determines if a year is a leap year
 * 'year' is the number of years since 1900
 * A year is a leap year if is evenly divisible by 4 while not divisible by 100
 * or if it is evenly divisible by 400. (Ex. 2000 is a leap year, 2100 is not)
 * Returns TRUE if the year is a leap year, FALSE if not
 */
int1 LeapYear(unsigned int16 year)
{
   if( ((year + 1900) % 400 == 0) || 
       (((year + 1900) % 4 == 0) && ((year + 1900) % 100 != 0)) )
      return TRUE;
     
   return FALSE;
}

/* Returns the difference in seconds between two times
 * Times later and earlier are expressed in seconds 
 */
signed int32 difftime(time_t later, time_t earlier)
{
   return (later - earlier);
}

/* Calculates the Unix Time from a standard time format
 * Returns the Unix Time (time in seconds since Jan 1, 1970 00:00:00)
 */
time_t mktime(struct_tm * timeT)
{
   time_t unixTime = 0;
   int1 isLeapYear = FALSE;
   unsigned int16 i = 0;
   
   if(timeT != NULL)
   {
   
      unixTime += timeT->tm_sec;
      unixTime += (unsigned int32)(timeT->tm_min) * 60;
      unixTime += (unsigned int32)(timeT->tm_hour) * 3600;
      
      isLeapYear = LeapYear(timeT->tm_year);
       /* Clamp the month to [0,11) */
      timeT->tm_mon %= 12;
      for(i = 1;i <= timeT->tm_mon;i++)
      {
         unixTime += (DaysInMonth(i - 1,isLeapYear) * 86400);
      }
      
      /* Clamp the days in the month */
      timeT->tm_mday %= DaysInMonth(timeT->tm_mon,isLeapYear);
      unixTime += (timeT->tm_mday) * 86400;
      
      if(isLeapYear)
      {
         timeT->tm_yday = (unixTime / 86400) % 366;
      }
      else
      {
         timeT->tm_yday = (unixTime / 86400) % 365;
      }
      
      i = 70;
      if(timeT->tm_year - 70 >= 0)
      {
         while(i < (timeT->tm_year))
         {
            isLeapYear = LeapYear(i);
            if(isLeapYear)
            {
               unixTime += (31622400); // seconds in 366 days
            }
            else
            {
               unixTime += (31536000); // seconds in 365 days
            }
            i++;
         }
      }
      
      timeT->tm_wday = ((unixTime / 86400) + 4) % 7;
   }

   return unixTime;
}

/* Returns the given time as a string of the form: 
 *  Day Mon X HH:MM:SS YYYY\0 
 */
char * asctime ( struct_tm * timeptr, char * szTime)
{
   char szDay[8];
   char szMon[8];
   
   WeekdayAbbreviations(timeptr->tm_wday, szDay);
   MonthAbbreviations(timeptr->tm_mon, szMon);
   
   sprintf(szTime,"%s %s %d %02d:%02d:%02d %04Lu",
      szDay,
      szMon,
      timeptr->tm_mday + 1,
      timeptr->tm_hour,
      timeptr->tm_min,
      timeptr->tm_sec,
      (timeptr->tm_year + 1900));
      
   return szTime;
}

/* Converts the given calendar time (in seconds) to local time 
 * and returns the equivalent string. 
 */
char * ctime ( time_t * timer, char *szTime )
{
   return (asctime(localtime(timer),szTime));
}

/* Global local time variable */
struct_tm g_lTime;

/* Converts the given calendar time (in seconds) to local time
 * and sets this time in the global g_lTime
 * Returns a pointer to g_lTime
 */
struct_tm * localtime(time_t * timer)
{
   time_t timeCounter;
   int1 done = FALSE;
   int1 isLeapYear = FALSE;//1970 is not a leap year

   if(timer != NULL)
   {
      timeCounter = *timer;
      g_lTime.tm_wday = ((timeCounter / 86400) + 4) % 7;//fill in the weekday
      g_lTime.tm_year = 70;//we are starting in 1970
      
      while(!done)
      {
         if(timeCounter < (31622400) && isLeapYear) // seconds in 366 days
         {
            g_lTime.tm_yday = (timeCounter / 86400);
            break;
         }
         else if(timeCounter < (31536000)) // seconds in 365 days
         {
            g_lTime.tm_yday = (timeCounter / 86400);
            break;
         }
         
         if(isLeapYear)
         {
            timeCounter -= 31622400; // seconds in 366 days
         }
         else
         {
            timeCounter -= 31536000; // seconds in 365 days
         }
         
         g_lTime.tm_year++;
         isLeapYear = LeapYear(g_lTime.tm_year);
      }
      
      g_lTime.tm_mon = 0;
      while(!done)
      {         
         if(timeCounter < DaysInMonth(g_lTime.tm_mon,isLeapYear) * 86400)
         {
            break;
         }
         else if(timeCounter >= DaysInMonth(g_lTime.tm_mon,isLeapYear) * 86400)
         {
            timeCounter -= DaysInMonth(g_lTime.tm_mon,isLeapYear) * 86400;
            g_lTime.tm_mon++;
         }
      }  

      g_lTime.tm_mday = (timeCounter / (86400));
      timeCounter -= (g_lTime.tm_mday * (86400));
      
      g_lTime.tm_hour = (timeCounter / (3600));
      timeCounter -= ((unsigned int32)g_lTime.tm_hour) * 3600;
      
      g_lTime.tm_min = (timeCounter / 60);
      timeCounter -= (((unsigned int16)g_lTime.tm_min) * 60);
     
      g_lTime.tm_sec = timeCounter;
   }
   
   return &g_lTime;
}

#endif