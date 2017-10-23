//////////////////////////////////////////////////////////////////////////
////                               DS1302.C                           ////
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
#define RTC_IO   PIN_B3
#define RTC_RST  PIN_B2

#endif

void write_ds1302_byte(unsigned int8 cmd) {
   unsigned int8 i;

   for(i=0;i<=7;++i) {
      output_bit(RTC_IO, shift_right(&cmd,1,0) );
      output_high(RTC_SCLK);
      output_low(RTC_SCLK);
   }
}

void write_ds1302(unsigned int8 cmd, unsigned int8 data) {

   output_high(RTC_RST);
   write_ds1302_byte(cmd);
   write_ds1302_byte(data);
   output_low(RTC_RST);
}

unsigned int8 read_ds1302(unsigned int8 cmd) {
   unsigned int8 i,data;

   output_high(RTC_RST);
   write_ds1302_byte(cmd);
   
   input(RTC_IO);
   delay_us(1);
         
   for(i=0;i<=7;++i) {
      shift_right(&data,1,input(RTC_IO));
      output_high(RTC_SCLK);
      delay_us(2);
      output_low(RTC_SCLK);
      delay_us(2);
   }
   output_low(RTC_RST);

   return(data);
}

void rtc_init() {
   unsigned int8 x;
   output_low(RTC_RST);
   delay_us(2);
   output_low(RTC_SCLK);
   write_ds1302(0x8e,0);
   write_ds1302(0x90,0xa4);
   x=read_ds1302(0x81);
   if((x & 0x80)!=0)
     write_ds1302(0x80,0);
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
   data=data+(i & 0x0F);       

   return data;
}

void rtc_set_datetime(unsigned int8 day, unsigned int8 mth, unsigned int8 year, unsigned int8 dow, unsigned int8 hr, unsigned int8 min) {

   write_ds1302(0x86,get_bcd(day));
   write_ds1302(0x88,get_bcd(mth));
   write_ds1302(0x8c,get_bcd(year));
   write_ds1302(0x8a,get_bcd(dow));
   write_ds1302(0x84,get_bcd(hr));
   write_ds1302(0x82,get_bcd(min));
   write_ds1302(0x80,get_bcd(0));
}                                      

void rtc_get_date(unsigned int8 &day, unsigned int8 &mth, unsigned int8 &year, unsigned int8 &dow) {
   day = rm_bcd(read_ds1302(0x87));
   mth = rm_bcd(read_ds1302(0x89));
   year = rm_bcd(read_ds1302(0x8d));
   dow = rm_bcd(read_ds1302(0x8b));
}

void rtc_get_time(unsigned int8 &hr, unsigned int8 &min, unsigned int8 &sec) {
   hr = rm_bcd(read_ds1302(0x85));
   min = rm_bcd(read_ds1302(0x83));
   sec = rm_bcd(read_ds1302(0x81));
}

void rtc_write_nvr(unsigned int8 address, unsigned int8 data) {
   write_ds1302(address|0xc0,data);
}

unsigned int8 rtc_read_nvr(unsigned int8 address) {
    return(read_ds1302(address|0xc1));
}
