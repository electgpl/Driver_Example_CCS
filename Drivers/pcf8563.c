/////////////////////////////////////////////////////////////////////////
////                          pcf8563.c                              ////
////                                                                 ////
////               Driver for PCF8563 Real Time Clock                ////
////                                                                 ////
////  rtc_init()                                Call after power up  ////
////                                                                 ////
////  rtc_set_datetime(TIME_STRUCT *ts)           Set the date/time  ////
////                                                                 ////
////  rtc_get_datetime(TIME_STRUCT *ts)           Get the date/time  ////
////                                                                 ////
////  TIME_STRUCT                          Struct holding date/time  ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2013 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#ifndef __PCF8563_C__
#define __PCF8563_C__

#ifndef PIN_RTC_SDA
 #define PIN_RTC_SDA PIN_C4
 #define PIN_RTC_SCL PIN_C3
#endif

#use i2c(MASTER, sda=PIN_RTC_SDA, scl=PIN_RTC_SCL, stream=STREAM_PCF8563)

typedef struct
{
   unsigned int8 year;  //years since 2000
   unsigned int8 month; //1..12
   unsigned int8 day;   //1..31
   unsigned int8 dow;   //0..6
   unsigned int8 hour;  //0..23
   unsigned int8 min;   //0..59
   unsigned int8 sec;   //0..59
   //int1 valid;
} TIME_STRUCT;

//////////////////////////////////////////

#define PCF8563_I2C_ADDR   0xA2

// registers //
#define PCF8563_CONTROL1   0x00     //control/status 1
#define PCF8563_CONTROL2   0x01     //control/status 2
#define PCF8563_CLKOUT     0x0D     //CLKOUT control
#define PCF8563_TCONTROL   0x0E     //timer control
#define PCF8563_TIMER      0x0F     //timer countdown value

#define PCF8563_SECONDS    0x02     //0..59 BCD (bit7 is VL)
#define PCF8563_MINUTES    0x03     //0..59 BCD
#define PCF8563_HOURS      0x04     //0..23 bcd
#define PCF8563_DAYS       0x05     //1..31 bcd
#define PCF8563_WEEKDAY    0x06     //0..6
#define PCF8563_MONTHS     0x07     //0..12 (bit7 is Century, leave clear for 20xx)
#define PCF8563_YEARS      0x08     //0..99 bcd

#define PCF8563_MINUTE_ALARM  0x09  //0..59 BCD
#define PCF8563_HOUR_ALARM    0x0A  //0..23 BCD
#define PCF8563_DAY_ALARM     0x0B  //0..31 BCD
#define PCF8563_WEEKDAY_ALARM 0x0C  //0..6

static unsigned int8 _get_bcd(unsigned int8 data)
{
   unsigned int8 nibh;
   unsigned int8 nibl;

   nibh=data/10;
   nibl=data-(nibh*10);

   return((nibh<<4)|nibl);
}

static unsigned int8 _rm_bcd(unsigned int8 data)
{
   unsigned int8 i;

   i=data;
   data=(i>>4)*10;
   data=data+(i&0x0F);

   return data;
}

void rtc_init(void)
{
   i2c_start(STREAM_PCF8563);
   i2c_write(STREAM_PCF8563, PCF8563_I2C_ADDR);
   i2c_write(STREAM_PCF8563, PCF8563_CONTROL1);
   i2c_write(STREAM_PCF8563, 0x00);
   i2c_write(STREAM_PCF8563, 0x00);
   i2c_stop(STREAM_PCF8563);
}

void rtc_set_datetime(TIME_STRUCT *pTs)
{
   TIME_STRUCT ts;

   memcpy(&ts, pTs, sizeof(ts));

   i2c_start(STREAM_PCF8563);
   i2c_write(STREAM_PCF8563, PCF8563_I2C_ADDR);
   i2c_write(STREAM_PCF8563, PCF8563_SECONDS);
   i2c_write(STREAM_PCF8563, _get_bcd(ts.sec));
   i2c_write(STREAM_PCF8563, _get_bcd(ts.min));
   i2c_write(STREAM_PCF8563, _get_bcd(ts.hour));
   i2c_write(STREAM_PCF8563, _get_bcd(ts.day));
   i2c_write(STREAM_PCF8563, _get_bcd(ts.dow));
   i2c_write(STREAM_PCF8563, _get_bcd(ts.month));
   i2c_write(STREAM_PCF8563, _get_bcd(ts.year));
   i2c_stop(STREAM_PCF8563);
}

void rtc_get_datetime(TIME_STRUCT *pTs)
{
   TIME_STRUCT ts;
   unsigned int8 val;

   i2c_start(STREAM_PCF8563);
   i2c_write(STREAM_PCF8563, PCF8563_I2C_ADDR);
   i2c_write(STREAM_PCF8563, PCF8563_SECONDS);
   i2c_start(STREAM_PCF8563, 2);
   i2c_write(STREAM_PCF8563, PCF8563_I2C_ADDR | 1);
   val = i2c_read(STREAM_PCF8563, 1);
   //ts.valid = !bit_test(val, 7);
   ts.sec = _rm_bcd(val & 0x7F);
   ts.min = _rm_bcd(i2c_read(STREAM_PCF8563, 1) & 0x7F);
   ts.hour = _rm_bcd(i2c_read(STREAM_PCF8563, 1) & 0x3F);
   ts.day = _rm_bcd(i2c_read(STREAM_PCF8563, 1) & 0x3F);
   ts.dow = _rm_bcd(i2c_read(STREAM_PCF8563, 1) & 0x07);
   ts.month = _rm_bcd(i2c_read(STREAM_PCF8563, 1) & 0x1F);
   ts.year = _rm_bcd(i2c_read(STREAM_PCF8563, 0));
   i2c_stop(STREAM_PCF8563);
   
   memcpy(pTs, &ts, sizeof(ts));
}



#endif
