/////////////////////////////////////////////////////////////////////////
////                            EX_RTCC.C                            ////
////                                                                 ////
////  This program reads and writes to the Internal Real-Time Clock  ////
////  and Calendar (RTCC).  It communicates to the user over the     ////
////  RS-232 interface.                                              ////
////                                                                 ////
////  This example works with the PCD and PCH compilers on PICs that ////
////  have a built in RTCC. The following conditional compilation    //// 
////  lines are used to include a valid device for each compiler.    //// 
////  Change the device, clock and RS232 pins for your hardware if   //// 
////  needed.                                                        ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2009 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#if defined(__PCH__)
#include <18F26J11.h>
#fuses HS,NOWDT,RTCOSC_T1
#use delay(clock=20M)
#pin_select U2TX=PIN_B5
#pin_select U2RX=PIN_B4
#use rs232(baud=9600,UART2)

#elif defined(__PCD__)
#include <24FJ128GA010.h>
#fuses PR,HS,NOWDT,ICSP1,NOJTAG
#use delay(clock=8M)
#use rs232(baud=9600,UART2)
#endif

int8 get_number() 
{
  char first,second;

  do {
    first=getc();
  } while ((first<'0') || (first>'9'));
  putc(first);
  first-='0';

  do {
    second=getc();
  } while (((second<'0') || (second>'9')) && (second!='\r'));
  putc(second);

  if(second=='\r')
    return(first);
  else
    return((first*10)+(second-'0'));
}

void set_clock(rtc_time_t &date_time)
{
   printf("\r\nPress ENTER after 1 digit answers.");
   printf("\r\nYear 20: ");
   date_time.tm_year=get_number();
   printf("\r\nMonth: ");
   date_time.tm_mon=get_number();
   printf("\r\nDay: ");
   date_time.tm_mday=get_number();
   printf("\r\nWeekday 1-7: ");
   date_time.tm_wday=get_number();
   printf("\r\nHour: ");
   date_time.tm_hour=get_number();
   printf("\r\nMin: ");
   date_time.tm_min=get_number();
   date_time.tm_sec=0;

   printf("\r\n\n");
}


void main()
{
   rtc_time_t write_clock, read_clock;
   
   setup_rtc(RTC_ENABLE,0);         //enables internal RTCC
   
   set_clock(write_clock);

   rtc_write(&write_clock);         //writes new clock setting to RTCC
   
   while(1)
   {
      rtc_read(&read_clock);        //reads clock value from RTCC
      printf("\r%02u/%02u/20%02u %02u:%02u:%02u",read_clock.tm_mon,read_clock.tm_mday,read_clock.tm_year,read_clock.tm_hour,read_clock.tm_min,read_clock.tm_sec);
      delay_ms(250);
   }
}
