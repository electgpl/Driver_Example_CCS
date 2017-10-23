////////////////////////////////////////////////////////////////////////////
////                       ISL1209.c                                    ////
////            Driver for Real Time Clock/Calendar                     ////
////                                                                    ////
////   rtc_init()                                 Call after power up   ////
////                                                                    ////
////   rtc_set_datetime(day,mth,year,dow,hour,min)  Set the date/time   ////
////                                                                    ////
////   rtc_get_datetime(hr,min,sec)                 Get the date/time   ////
////                                                                    ////
//// Day of the week are numbered: Monday = 0, Teusday =1, Wednesday =2,////
//// Thursday = 3, Friday = 4, Saturday = 5, Sunday = 6                 ////
////                                                                    ////
////  set_alarm_xxxx(xx) sets alarm time in the RTC                     ////
////                                                                    ////
////  set_rtcc_freqout(freq) sets frequency at the Fout pin             ////
////                                                                    ////
////  enable_event_detection(EHYS,ESMP) Enable event detection          ////
////                                                                    ////
////   get_bcd(data)                             Convert data to BCD    ////
////                                                                    ////
////   rm_bcd(data)                              Convert data to int    ////
////                                                                    ////
////   make_hex_dec                              Convert hex to dec     ////
////                                                                    ////
////                    Pin Layout - 10 LD MSOP                         ////
////   -----------------------------------------------------------      ////
////   |                                                         |      ////
////   | 1: X1                 | 10: VDD   +5V                   |      ////
////   |                       |    ___                          |      ////
////   | 2: X2                 | 9: IRQ/FOUT Interrupt/Freq Out  |      ////
////   |                       |                                 |      ////
////   | 3: VBAT Battery       | 8: SCL   Serial clock           |      ////
////   |                       |                                 |      ////
////   | 4: VSS  GND           | 7: SDA   Serial data            |      ////
////   |                       |    ______                       |      ////
////   | 5: EVIN  Event Input  | 6: EVENDT  Event Detect         |      ////
////   -----------------------------------------------------------      ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2006 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C   ////
//// compiler.  This source code may only be distributed to other       ////
//// licensed users of the CCS C compiler.  No other use, reproduction  ////
//// or distribution is permitted without written permission.           ////
//// Derivative programs created using this software in object code     ////
//// form are not restricted in any way.                                ////
////////////////////////////////////////////////////////////////////////////


#ifndef DAL_SCL
#define DAL_SCL PIN_B0
#define DAL_SDA PIN_B1
#endif

#use i2c(master, sda=DAL_SDA, scl=DAL_SCL)
////////////////////////////////////////////////////////////////////////////
//////////////Register Map of ISL1209 SR////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#define sec_addr  0x00
#define min_addr  0x01
#define hour_addr 0x02
#define day_addr  0x03
#define mth_addr  0x04
#define year_addr 0x05
#define dow_addr  0x06
#define SR_addr   0x07
#define INT_addr  0x08
#define EV_addr   0x09
#define ATR_addr  0x0A
#define DTR_addr  0x0B
#define SCA_addr  0x0C
#define MNA_addr  0x0D
#define HRA_addr  0x0E
#define DTA_addr  0x0F
#define MOA_addr  0x10
#define DWA_addr  0x11
#define USR1_addr 0x12
#define USR2_addr 0x13

///////////////////////////////
/////Function Prototypes///////
///////////////////////////////
/*
void rtcc_write_byte(BYTE address, byte data1);
Function to write a byte to the SRAM of the ISL1209
PARAM addr: The address to write to
PARAM data: The byte to write to the specified location
RETURNS: none
*/
void rtcc_write_byte(BYTE address, byte data1);

/*
int rtcc_get_data(BYTE address);
Function to read a byte from the SRAM of the ISL1209
PARAM addr: The address to read from
RETURNS: The byte at the specified address
*/
int rtcc_get_data(BYTE address);

/*
void rtcc_init();
This will initialize the RTCC
PARAMS: none
RETURNS: none
*/
void rtcc_init();

/*
  int get_bcd(BYTE data);
  This will convert data to BCD format
  PARAM data: The data to be converted
  RETURNS: Data in BCD format
*/
int get_bcd(BYTE data);

/*
  int make_hex_dec(BYTE data)
  This will convert data to hex
  PARAM data: The data to be converted
  RETURNS: Data in hex format
*/
  int make_hex_dec(BYTE data);

/*
void rtcc_get_time(BYTE& sec, BYTE& min, BYTE& hour, BYTE& day, BYTE& mth, BYTE& year, BYTE& dow);
This will read the date and time from the RTCC
PARAMS: none
RETURN: Returns the time in seconds, minutes and hours and date in month, year, day of week and day of month
*/
void rtcc_get_time(BYTE& sec, BYTE& min, BYTE& hour, BYTE& day, BYTE& mth, BYTE& year, BYTE& dow);

/*
void rtcc_set_datetime(BYTE dow, BYTE year,BYTE mth, BYTE day,BYTE hour, BYTE min);
This will set the date and time from the RTCC
PARAMS: The time in minutes and hours, and the date in month, year, day of week and day of month.
RETURN: None
*/
void rtcc_set_datetime(BYTE dow, BYTE year,BYTE mth, BYTE day,BYTE hour, BYTE min);

/*
void rtcc_set_time(BYTE hour, BYTE min, BYTE sec);
This will set the time in the RTCC
PARAMS: The time in seconds, minutes and hours
RETURNS: None
*/
void rtcc_set_time(BYTE hour, BYTE min, BYTE sec);

/*
void enable_alarms();
This will enable setting of alarms in the ISL1209 chip
PARAMS: None
RETURNS: None
*/
void enable_alarms();

/*
void disable_alarms();
This will disable setting of alarms in the ISL1209 chip
PARAMS: None
RETURNS: None
*/
void disable_alarms();

/*
void enable_alarm_int();
Function to enable alarm in Interrupt mode
PARAMS: None
RETURNS: None
*/
void enable_alarm_int();

/*
void set_alarm_sec(BYTE sec);
This will set the alarm time in seconds
PARAMS: Alarm time in seconds  0-59
RETURNS: None
*/
void set_alarm_sec(BYTE sec);

/*
void set_alarm_min(BYTE min);
This will set the alarm time in minutes
PARAMS: Alarm time in minutes  0-59
RETURNS: None
*/
void set_alarm_min(BYTE min);

/*
void set_alarm_hour(BYTE hour);
This will set the alarm time in minutes
PARAMS: Alarm time in hours 0-23
RETURNS: None
*/
void set_alarm_hour(BYTE hour);

/*
void set_alarm_day(BYTE day);
This will set the alarm time in days
PARAMS: Alarm time in days 1-31
RETURNS: None
*/
void set_alarm_day(BYTE day);

/*
void set_alarm_mth(BYTE mth);
This will set the alarm time for the month
PARAMS: Alarm time for month 1-12
RETURNS: None
*/
void set_alarm_mth(BYTE mth);

/*
void set_alarm_dow(BYTE dow);
This will set the alarm time for day of the week
PARAMS: Alarm time for day of the week 0-6
RETURNS: None
*/
void set_alarm_dow(BYTE dow);

/*
void clear_all_alarms();
This function will clear all alarms by setting the Alarms register to zero
PARAMS: None
RETURNS: None
*/
void clear_all_alarms();

/*
void set_rtcc_freqout(int value);
This will set the output frequency on the Fout pin
PARAMS: 4 bit frequency value
RETURNS: None
*/
void set_rtcc_freqout(int value);

/*
void enable_event_detection(int ESMP, int EHYS);
Function to control event detection mode
PARAMS: ESMP Event Sampling rate
PARAMS: EHYS Time base hysteresys
*/
void enable_event_detection(int ESMP, int EHYS);


///////////////////////////////////////
//////Function Implementations/////////
///////////////////////////////////////
/*
void rtcc_write_byte(BYTE address, byte data1);
Function to write a byte to the SRAM of the ISL1209
PARAM addr: The address to write to
PARAM data: The byte to write to the specified location
RETURNS: None
*/
   void rtcc_write_byte(BYTE address, byte data1){  // Function to write Byte to
   i2c_start();
   i2c_write(0xDE); // Transmit Identification Byte for write mode
   i2c_write(address);  // Address to write
   i2c_write(data1); // Data to start RTC
   i2c_stop();
   }


/*
void rtcc_init();
This will initialize the RTCC
PARAMS: none
RETURNS: none
*/
   void rtcc_init() {
   output_high(DAL_SDA);
   output_high(DAL_SCL);
   i2c_start();
   i2c_write(0xDE); // Transmit Identification Byte for write mode
   i2c_write(0x07);  // Address of the status register
   i2c_write(0x10); // Set the WRTC bit in the status register, other bits cleared
   i2c_stop();
   rtcc_write_byte(0x00,0x00); // Perform valid write t0 RTC register
//   rtcc_write_byte(0x08,0x00); // Clear All Interrupts
       }

/*
  int get_bcd(BYTE data);
  This will convert data to BCD format
  PARAM data: The data to be converted
  RETURNS: Data in BCD format
*/
   int get_bcd(BYTE data)
{
   int nibh;
   int nibl;

   nibh=data/10;
   nibl=data-(nibh*10);

   return((nibh<<4)|nibl);
}
   int rm_bcd(BYTE data)
{
   int i;

   i=data;
   data=(i>>4)*10;
   data=data+(i<<4>>4);

   return data;
}

/*
  int make_hex_dec(BYTE data)
  This will convert data to hex
  PARAM data: The data to be converted
  RETURNS: Data in hex format
*/
  int make_hex_dec(BYTE data)
{
 int nibh;
 int newdat;
 nibh=(data/10)*16;
 data=data - (data/10)*10;
 newdat = nibh+data;
 return(newdat);
}


/*
void rtcc_get_time(BYTE& sec, BYTE& min, BYTE& hour, BYTE& day, BYTE& mth, BYTE& year, BYTE& dow);
This will read the date and time from the RTCC
PARAMS: none
RETURN: Returns the time in seconds, minutes and hours and date in month, year, day of week and day of month
*/
void rtcc_get_time(BYTE& sec, BYTE& min, BYTE& hour, BYTE& day, BYTE& mth, BYTE& year, BYTE& dow){
   i2c_start();
   i2c_write(0xDE); // Transmit Identification Byte for write mode
   i2c_write(0x00); // Address for seconds register
   i2c_start();
   i2c_write(0xDF); // Transmit Identification Byte for read operation
   sec = rm_bcd(i2c_read());
   min = rm_bcd(i2c_read());
   hour = i2c_read();
   bit_clear(hour,7);
   hour = rm_bcd(hour);
   day = rm_bcd(i2c_read());   // This function reads all bytes from 0x00h to 0x06h of ISL1209 SRAM
   mth = rm_bcd(i2c_read());
   year = rm_bcd(i2c_read());
   dow = rm_bcd(i2c_read(0));
   i2c_stop();
}

/*
void rtcc_set_datetime(BYTE dow, BYTE year,BYTE mth, BYTE day,BYTE hour, BYTE min);
This will set the date and time from the RTCC
PARAMS: The time in minutes and hours, and the date in month, year, day of week and day of month.
RETURN: None
*/
void rtcc_set_datetime(BYTE dow, BYTE year,BYTE mth, BYTE day,   // Set date & time
                      BYTE hour, BYTE min){
   min = make_hex_dec(min);  // Storing decimal value to the SRAM
   hour = make_hex_dec(hour);
   day = make_hex_dec(day);
   year = make_hex_dec(year);
   mth = make_hex_dec(mth);   //dow is a 3 bit number and does not need this conversion
   hour=hour^0x80;
   output_high(DAL_SDA);
   output_high(DAL_SCL);
   rtcc_write_byte(min_addr, min);
   rtcc_write_byte(hour_addr, hour);
   rtcc_write_byte(day_addr, day);
   rtcc_write_byte(mth_addr, mth);
   rtcc_write_byte(year_addr, year);
   rtcc_write_byte(dow_addr, dow);

}

/*
void rtcc_set_time(BYTE hour, BYTE min, BYTE sec);
This will set the time in the RTCC
PARAMS: The time in seconds, minutes and hours
RETURNS: None
*/
void rtcc_set_time(BYTE hour, BYTE min, BYTE sec){ // Set date & time
   sec = make_hex_dec(sec);
   min = make_hex_dec(min);  // Storing decimal value to the SRAM
   hour = make_hex_dec(hour);
   hour=hour^0x80;
   output_high(DAL_SDA);
   output_high(DAL_SCL);
   rtcc_write_byte(0x00, sec);
   rtcc_write_byte(0x01, min);
   rtcc_write_byte(0x02, hour);
}

/*
int rtcc_get_data(BYTE address);
Function to read a byte from the SRAM of the ISL1209
PARAM addr: The address to read from
RETURNS: The byte at the specified address
*/
int rtcc_get_data(BYTE address)  // This function will read any valid address - 0x00h to 0x13h
   {
   int data;
   i2c_start();
   i2c_write(0xDE); // Transmit Identification Byte for write mode
   i2c_write(address); // Address to read
   i2c_start();
   i2c_write(0xDF); // Transmit Identification Byte for read operation
   data = rm_bcd(i2c_read(0));
   i2c_stop();
   return(data);
   }

/*
void enable_alarms();
This will ensable setting of alarms in the ISL1209 chip
PARAMS: None
RETURNS: None
*/
void enable_alarms()
{  int i;
   i = rtcc_get_data(INT_addr); // Get value stored in Interrupt Register
   bit_set(i,6);
   rtcc_write_byte(INT_addr,i);// Set the ALME bit in Interrupt register
}

/*
void disable_alarms();
This will disable setting of alarms in the ISL1209 chip
PARAMS: None
RETURNS: None
*/
void disable_alarms()
   {  int i;
      i = rtcc_get_data(INT_addr);
      bit_clear(i,6);
      rtcc_write_byte(INT_addr,i);// Clear the ALME bit in Interrupt register
   }

/*
void enable_alarm_int();
Function to enable alarm in Interrupt mode
PARAMS: None
RETURNS: None
*/
void enable_alarm_int()  //
{  int i;                                                   //            ___
   i = rtcc_get_data(INT_addr); // This will generate 250 ms low pulse on IRQ/Fout pin
   bit_set(i,7);
   rtcc_write_byte(INT_addr,i);// Set the ALME bit in Interrupt register
}

// enable_alarms() Function must be called before using any of the following set_alarms_xxx functions//

/*
void set_alarm_sec(BYTE sec);
This will set the alarm time in seconds
PARAMS: Alarm time in seconds 0-59
RETURNS: None
*/
void set_alarm_sec(BYTE sec)// // Takes value 0-59
   {
   sec = make_hex_dec(sec);
   sec^=0x80;  // Set MSB
   rtcc_write_byte(SCA_addr,sec);
   }

/*
void set_alarm_min(BYTE min);
This will set the alarm time in minutes
PARAMS: Alarm time in minutes  0-59
RETURNS: None
*/
void set_alarm_min(BYTE min)// // Takes value 0-59
   {
   min = make_hex_dec(min);
   min^=0x80;
   rtcc_write_byte(MNA_addr,min);
   }

/*
void set_alarm_hour(BYTE hour);
This will set the alarm time in minutes
PARAMS: Alarm time in hours 0-23
RETURNS: None
*/

void set_alarm_hour(BYTE hour) // Takes value 0-23
   {
   hour = make_hex_dec(hour);
   hour^=0x80;
   rtcc_write_byte(HRA_addr,hour);
   }

/*
void set_alarm_day(BYTE day);
This will set the alarm time in days
PARAMS: Alarm time in days 1-31
RETURNS: None
*/
void set_alarm_day(BYTE day) // Takes value 1-31
   {
   day = make_hex_dec(day);
   day^=0x80;
   rtcc_write_byte(DTA_addr,day);
   }

/*
void set_alarm_mth(BYTE mth);
This will set the alarm time for the month
PARAMS: Alarm time for month 1-12
RETURNS: None
*/
void set_alarm_mth(BYTE mth)  // Takes value 1-12
   {
   mth = make_hex_dec(mth);
   mth^=0x80;
   rtcc_write_byte(MOA_addr,mth);
   }

/*
void set_alarm_dow(BYTE dow);
This will set the alarm time for day of the week
PARAMS: Alarm time for day of the week 0-6
RETURNS: None
*/
void set_alarm_dow(BYTE dow)  // Takes value 0-6
   {
   dow = make_hex_dec(dow);
   dow^=0x80;
   rtcc_write_byte(DWA_addr,dow);
   }


/*
void clear_all_alarms();
This function will clear all alarms by setting the Alarms register to zero
PARAMS: None
RETURNS: None
*/
void clear_all_alarms()
   {
      rtcc_write_byte(SCA_addr,0);
      rtcc_write_byte(MNA_addr,0);
      rtcc_write_byte(HRA_addr,0);
      rtcc_write_byte(DTA_addr,0);
      rtcc_write_byte(MOA_addr,0);
      rtcc_write_byte(DWA_addr,0);

   }

////////////////////////////////////////////////////////////////////////////
////////////Function to select frequency of Fout pin////////////////////////
////////////////////////////////////////////////////////////////////////////
/*
   Selecting the frequency to 0 will disable
    Fout (Hz)    value(4 bit)
     0            0
    32768         1
    4096          2
    1024          3
    64            4
    32            5
    16            6
     8            7
     4            8
     2            9
     1            10
    1/2           11
    1/4           12
    1/8           13
    1/16          14
    1/32          15
*/
/*
void set_rtcc_freqout(int value);
This will set the output frequency on the Fout pin
PARAMS: 4 bit frequency value
RETURNS: None
*/
void set_rtcc_freqout(int value){  //This function is disabled when FOBATB bit in INT register is set
  rtcc_write_byte(INT_addr,value); // Setting a frequency will override alarm mode at the IRQ/Fout pin
}

/*
void enable_event_detection(int ESMP, int EHYS);
Function to control event detection mode
PARAMS: ESMP Event Sampling rate
PARAMS: EHYS Time base hysteresys
*/

void enable_event_detection(int ESMP, int EHYS) // sample and hysterisys are 2 bits each (0 - 3)
      {int i=0;
      i=EHYS;
      rotate_left(&i,0);
      rotate_left(&i,0); //  shift left by 2 bits
      i^=ESMP;           // Bit 0 and Bit 1 set the sampling rate
      bit_set(i,4); // Set the EVEN bit to enable the Event Sampling Mode
      rtcc_write_byte(EV_addr,i);
      }// Note: Control of other paramerters like RTC Halt, Event Output in Battery Mode and Event Current Source Enable bit
      // can be added based on the application requirements by setting bits 5,6 and 7 in EV Reister respectively.

