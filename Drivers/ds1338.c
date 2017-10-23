//////////////////////////////////////////////////////////////////////////
////                               DS1338.C                           ////
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
//////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2013 Custom Computer Services          ////
//// This source code may only be used by licensed users of the CCS C ////
//// compiler.  This source code may only be distributed to other     ////
//// licensed users of the CCS C compiler.  No other use, reproduction////
//// or distribution is permitted without written permission.         ////
//// Derivative programs created using this software in object code   ////
//// form are not restricted in any way.                              ////
//////////////////////////////////////////////////////////////////////////

#ifndef RTC_SCLK
 #define RTC_SCLK PIN_B1
 #define RTC_SDA  PIN_B2
#endif

#ifdef USE_HARDWARE_I2C
 #use i2c(MASTER, FAST, SCL=RTC_SCLK, SDA=RTC_SDA, FORCE_HW, stream=DS1338, SLOW)
#else
 #use i2c(MASTER, FAST, SCL=RTC_SCLK, SDA=RTC_SDA, FORCE_SW, stream=DS1338)
#endif

#define DS1338_SEC      0
#define DS1338_MIN      1   
#define DS1338_HR       2
#define DS1338_DAY      3
#define DS1338_DATE     4
#define DS1338_MONTH    5
#define DS1338_YR       6
#define DS1338_CONTROL  7

unsigned int8 read_ds1338(unsigned int8 address)
{
   unsigned int8 result;

   i2c_start(DS1338);
   i2c_write(DS1338, 0xD0);
   i2c_write(DS1338, address);
   i2c_start(DS1338);
   i2c_write(DS1338, 0xD1);
   result = i2c_read(DS1338, 0);
   i2c_stop(DS1338);
   
   return(result);
}

void write_ds1338(unsigned int8 address, unsigned int8 data)
{
   i2c_start(DS1338);
   i2c_write(DS1338, 0xD0);
   i2c_write(DS1338, address);
   i2c_write(DS1338, data);
   i2c_stop(DS1338);
}

void rtc_init()
{
   unsigned int8 data;
   
   data = read_ds1338(DS1338_SEC);
   
   data &= 0x7F;
   
   i2c_start(DS1338);
   i2c_write(DS1338, 0xD0);
   i2c_write(DS1338, DS1338_SEC);
   i2c_write(DS1338, data);
   i2c_start(DS1338);
   i2c_write(DS1338, 0xD0);
   i2c_write(DS1338, DS1338_CONTROL);
   i2c_write(DS1338, 0x80);
   i2c_stop(DS1338);
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

void rtc_set_datetime(unsigned int8 day, unsigned int8 mth, unsigned int8 year, unsigned int8 dow, unsigned int8 hr, unsigned int8 min)
{
   i2c_start(DS1338);
   i2c_write(DS1338, 0xD0);
   i2c_write(DS1338, DS1338_SEC);
   i2c_write(DS1338, 0x00);
   i2c_write(DS1338, get_bcd(min));
   i2c_write(DS1338, get_bcd(hr));
   i2c_write(DS1338, get_bcd(dow));
   i2c_write(DS1338, get_bcd(day));
   i2c_write(DS1338, get_bcd(mth));
   i2c_write(DS1338, get_bcd(year));
   i2c_stop();
}

void rtc_get_date(unsigned int8 &date, unsigned int8 &mth, unsigned int8 &year, unsigned int8 &dow)
{
   date = rm_bcd(read_ds1338(DS1338_DATE));
   mth = rm_bcd(read_ds1338(DS1338_MONTH));
   year = rm_bcd(read_ds1338(DS1338_YR));
   dow = rm_bcd(read_ds1338(DS1338_DAY));
}

void rtc_get_time(unsigned int8 &hr, unsigned int8 &min, unsigned int8 &sec)
{
   hr = rm_bcd(read_ds1338(DS1338_HR));
   min = rm_bcd(read_ds1338(DS1338_MIN));
   sec = rm_bcd(read_ds1338(DS1338_SEC));
}

void rtc_write_nvr(unsigned int8 address, unsigned int8 data)
{
   write_ds1338(address, data);
}

unsigned int8 rtc_read_nvr(unsigned int8 address)
{
   return(read_ds1338(address));
}
