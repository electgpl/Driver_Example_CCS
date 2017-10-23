//////////////////////////////////////////////////////////////////////////
////                               DS1305.C                           ////
////                     Driver for Real Time Clock                   ////
////                                                                  ////
////  rtc_init()                                   Call after power up////
////                                                                  ////
////  rtc_set_datetime(day,mth,year,dow,hour,min)  Set the date/time  ////
////                                                                  ////
////  rtc_get_date(day,mth,year,dow)               Get the date       ////
////                                                                  ////
////  rtc_get_time(hr,min,sec)                     Get the time       ////
////                                                                  ////
////  rtc_write_nvr(address,data)                  Write to NVR       ////
////                                                                  ////
////  data = rtc_read_nvr(address)                 Read from NVR      ////
////                                                                  ////
////  get_bcd(data)                              Convert data to BCD  ////
////                                                                  ////
////  rm_bcd(data)                               Convert data to int  ////
////                                                                  ////
////                                                                  ////
//// If time.h is included before DS1305.c, this driver will provides ////
//// the functions needed to be a time.h compatable driver.           ////
//// See time.h for documentation of the time.h library.              ////
////                                                                  ////
////                                                                  ////
//// PIC Configuration Requirements                                   ////
//// ---------------------------------------------------------------  ////
////                                                                  ////
////     RTC_RST - chip select pin.                                   ////
////     RTC_SCLK - serial clock pin.                                 ////
////     RTC_IO, RTC_MOSI, RTC_MISO -                                 ////
////        Serial data pins.  You can either use RTC_IO (one wire    ////
////        mode for both directions) or RTC_MOSI and RTC_MISO        ////
////        (two wires for both directions).                          ////
////                                                                  ////
////     Alternately, instead of defining RTC_IO, RTC_MOSI and        ////
////     RTC_MISO you can provide a DS1305_SPI_XFER() macro that      ////
////     defines how to do the serial transfer.                       ////
////                                                                  ////
//////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services          ////
//// This source code may only be used by licensed users of the CCS C ////
//// compiler.  This source code may only be distributed to other     ////
//// licensed users of the CCS C compiler.  No other use, reproduction////
//// or distribution is permitted without written permission.         ////
//// Derivative programs created using this software in object code   ////
//// form are not restricted in any way.                              ////
//////////////////////////////////////////////////////////////////////////

#ifndef __DS1305_C__
#define __DS1305_C__

#ifndef RTC_RST
   // only used if DS1305_SPI_XFER isn't defined
   #define RTC_RST  PIN_B2
#endif

#ifndef DS1305_SPI_XFER
  #ifndef RTC_SCLK
   #define RTC_SCLK PIN_B1
  #endif
  
  #if !defined(RTC_IO) && !defined(RTC_MOSI) && !defined(RTC_MISO)
   #define RTC_IO PIN_B3
  #endif

  #if defined(RTC_IO)
   // mosi and miso are shared lines
   #use spi(master, mode=0, MSB_FIRST, do=RTC_IO, di=RTC_IO, clk=RTC_SCLK, bits=8, baud=600000, stream=STREAM_DS1305_SPI)
  #else
   // mosi and miso are seperate lines
   #use spi(master, mode=0, MSB_FIRST, do=RTC_MOSI, di=RTC_MISO, clk=RTC_SCLK, bits=8, baud=600000, stream=STREAM_DS1305_SPI)
  #endif

   #define DS1305_SPI_XFER(x) spi_xfer(STREAM_DS1305_SPI, x)
#endif

///////// end configuration

#define write_ds1305_byte(x)  DS1305_SPI_XFER(x)

void write_ds1305(unsigned int8 cmd, unsigned int8 data) {
   output_high(RTC_RST);
   delay_us(2);
   write_ds1305_byte(cmd);
   write_ds1305_byte(data);
   output_low(RTC_RST);
}

unsigned int8 read_ds1305(unsigned int8 cmd) 
{
   unsigned int8 data;

   output_high(RTC_RST);
   write_ds1305_byte(cmd);
   data = DS1305_SPI_XFER(0x00);

   output_low(RTC_RST);
   return(data);
}

void rtc_init() 
{
   output_low(RTC_RST);
   output_drive(RTC_RST);

   output_drive(RTC_SCLK);
   output_drive(RTC_MOSI);
  
   write_ds1305(0x8f,0);      //osc enabled; no write protect; no interrupts
}

unsigned int8 get_bcd(unsigned int8 data)
{
   unsigned int8 nibh;
   unsigned int8 nibl;

   nibh=data/10;
   nibl=data-(nibh*10);

   return((nibh<<4)|nibl);
}

unsigned int8 rm_bcd(unsigned int8 data)
{
   unsigned int8 i;

   i=data;
   data=(i>>4)*10;
   data=data+(i&0x0F);

   return data;
}

void rtc_set_datetime(unsigned int8 day, unsigned int8 mth, unsigned int8 year, unsigned int8 dow, unsigned int8 hr, unsigned int8 min) {
   write_ds1305(0x84,get_bcd(day));
   write_ds1305(0x85,get_bcd(mth));
   write_ds1305(0x86,get_bcd(year));
   write_ds1305(0x83,get_bcd(dow));
   write_ds1305(0x82,get_bcd(hr));
   write_ds1305(0x81,get_bcd(min));
   write_ds1305(0x80,0);
}

void rtc_get_date(unsigned int8 & day, unsigned int8 & mth, unsigned int8 & year, unsigned int8 & dow) {
   day = rm_bcd(read_ds1305(0x04));
   mth = rm_bcd(read_ds1305(0x05));
   year = rm_bcd(read_ds1305(0x06));
   dow = rm_bcd(read_ds1305(0x03));
}


void rtc_get_time(unsigned int8 & hr, unsigned int8 & min, unsigned int8 & sec) {
   hr = rm_bcd(read_ds1305(0x02));
   min = rm_bcd(read_ds1305(0x01));
   sec = rm_bcd(read_ds1305(0x00));
}

void rtc_write_nvr(unsigned int8 address, unsigned int8 data) {   //valid address: 0x00 to 0x5F
   write_ds1305(address|0xa0,data);
}

unsigned int8 rtc_read_nvr(unsigned int8 address) {
    return(read_ds1305(address|0x20));
}

// add functions to be time.h compatible
#ifdef __TIME_H__

#include <time.c>

/* Globals & Resources */
static time_t calendar_time; //seconds since Jan 1, 1970 00:00:00

/* Returns processor time used by program (clock ticks) 
 * The Ds1305 does not use ticks, so return 0
 */
clock_t clock()
{
   return 0;
}

/* Returns the current calendar time.
 * First gets the time from the Ds1305, converts it to seconds,
 * and sets this as the calendar time.
 * If a timer is specified, it will be set to the current calendar time.
 * It is assumed that SetTime has already been called and the Ds1305
 * is initialized.
 */
time_t time(time_t * timer)
{
   struct_tm nTime;
   unsigned int8 hour,min,sec,mday,mon,year,wday;
   
   rtc_get_time(hour, min, sec);
   rtc_get_date(mday, mon, year, wday);

   nTime.tm_hour = hour;
   nTime.tm_min = min;
   nTime.tm_sec = sec;
   // adjust values to be time.h compatible
   nTime.tm_mday = mday - 1;   // 0-30
   nTime.tm_mon = mon - 1;    // 0-11
   nTime.tm_year = year + 100; // starting at 1900
   
   // mktime() calculates the yday and wday
   
   calendar_time = mktime(&nTime);
   
   if(timer != NULL)
   {
      *timer = calendar_time;
   }
   
   return calendar_time;
}

/* Sets the current calendar time (in seconds)from the given time, 
 * and initializes and sets the time/date on the Ds1305.
 */
void SetTime(struct_tm * nTime)
{   
   // date must be adjusted to be Ds1305 compatible
   rtc_set_datetime(nTime->tm_mday+1, nTime->tm_mon+1, nTime->tm_year-100, nTime->tm_wday+1, nTime->tm_hour, nTime->tm_min);
   calendar_time = mktime(nTime);
}

/* Sets the current time on the DS1305 given a time in seconds and
 * sets the calendar time to this time.
 */
void SetTimeSec(time_t sTime)
{
   struct_tm * nTime;
   nTime = localtime(&sTime);
   // date must be adjusted to be Ds1305 compatible
   rtc_set_datetime(nTime->tm_mday+1, nTime->tm_mon+1, nTime->tm_year-100, nTime->tm_wday+1, nTime->tm_hour, nTime->tm_min);
   calendar_time = sTime;
}

/* Returns the current time from the DS1305 as a time struct
 */
void GetTime(struct_tm *pRetTm)
{
   struct_tm cTime;
   unsigned int8 hour,min,sec,mday,mon,year,wday;
   
   rtc_get_time(hour, min, sec);
   rtc_get_date(mday, mon, year, wday);

   cTime.tm_hour = hour;
   cTime.tm_min = min;
   cTime.tm_sec = sec;
   // adjust values to be time.h compatible
   cTime.tm_mday = mday - 1;   // 0-30
   cTime.tm_mon = mon - 1;    // 0-11
   cTime.tm_year = year + 100; // starting at 1900
   cTime.tm_wday = wday - 1; // 0-6
   
   memcpy(pRetTm, &cTime, sizeof(struct_tm));
}


/* Initializes The DS1305
 */
void TimeInit(void)
{
   rtc_init();
}

#endif   //__TIME_H__
#endif   //__DS1305_C__
