/////////////////////////////////////////////////////////////////////////////
////                                                                     ////
////                             ex_logger.c                             ////
////                                                                     ////
//// This example pulls temperature data from several DS1621 temperature ////
//// sensors and keeps a log of the data in the flash program memory.    ////
//// The data is polled every second.                                    ////
////                                                                     ////
//// Change the define NUM_SENSORS to specify the number of DS1621       ////
//// sensors connected to the PIC.  Each sensor must have it's I2C       ////
//// address different, starting with address 0 and ending in address    ////
//// NUM_SENSORS-1.                                                      ////
////                                                                     ////
//// If you change the clock speed from 20Mhz then you must either       ////
//// change the define INTERRUPTS_PER_MINUTE or change the Timer2 timing ////
//// setup.                                                              ////
////                                                                     ////
//// BUFFER_SIZE can be changed to specify more or less space for saving ////
//// temperature data.  Make sure BUFFER_SIZE can be evenly divided by   ////
//// NUM_SENSORS.                                                        ////
////                                                                     ////
////  Jumpers:                                                           ////
////     PCB        pin A2 to RS232 RX, pin A3 to RS232 TX               ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX               ////
////                                                                     ////
////  This example will work with the PCM and PCH compilers.             ////
/////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,1997 Custom Computer Services             ////
//// This source code may only be used by licensed users of the CCS C    ////
//// compiler.  This source code may only be distributed to other        ////
//// licensed users of the CCS C compiler.  No other use, reproduction   ////
//// or distribution is permitted without written permission.            ////
//// Derivative programs created using this software in object code      ////
//// form are not restricted in any way.                                 ////
/////////////////////////////////////////////////////////////////////////////

#if defined(__PCM__)
#include <16F877.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCH__)
#include <18F458.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#endif

//user config
#define NUM_SENSORS           2
#define BUFFER_SIZE           4096
#define INTERRUPTS_PER_MINUTE 60000

#define DAL_SCL PIN_C3
#define DAL_SDA PIN_C4

#include <ds1621m.c>

//rom locations defines.  do not change
#define PROGRAM_MEMORY_SIZE   getenv("PROGRAM_MEMORY")
#define BUFFER_END            PROGRAM_MEMORY_SIZE-1
#define BUFFER_START          (PROGRAM_MEMORY_SIZE-BUFFER_SIZE)
#if defined(__PCM__)
   #if BUFFER_SIZE > 6144
      #org BUFFER_START, BUFFER_END-0x1800 {}
      #org BUFFER_END-0x17FF, BUFFER_END-0x1000 {}
      #org BUFFER_END-0xFFF, BUFFER_END-0x800 {}
      #org BUFFER_END-0x7FF, BUFFER_END {}
   #elif BUFFER_SIZE > 4096
      #org BUFFER_START, BUFFER_END-0x1000 {}
      #org BUFFER_END-0xFFF, BUFFER_END-0x800 {}
      #org BUFFER_END-0x7FF, BUFFER_END {}
   #elif BUFFER_SIZE > 2048
      #org BUFFER_START, BUFFER_END-0x800 {}
      #org BUFFER_END-0x7FF, BUFFER_END {}
   #else
      #org BUFFER_START, BUFFER_END {}
   #endif
#else
   #org BUFFER_START, BUFFER_END {}
#endif

int16 timer2_count=INTERRUPTS_PER_MINUTE;
int16 pm_address=BUFFER_START;
int16 log_count=0;
int1 display_new_data=1;
#int_timer2
void timer2_isr(void) {
   int sensor;
   signed int16 temp;

   timer2_count++;
   if (timer2_count>INTERRUPTS_PER_MINUTE) {
      timer2_count=0;
      if (display_new_data) {printf("\rData: ");}
      for (sensor=0;sensor<NUM_SENSORS;sensor++) {
         temp=read_full_temp(sensor);
         if (display_new_data) {
            printf("%ld   ",temp);
         }
         temp+=67;   //sensor returns -67 to 257.  fix to 0 to 324
         temp=abs(temp);   //just in case
         write_program_eeprom(pm_address++,temp);
         if (pm_address > BUFFER_END) {pm_address=BUFFER_START;}
         log_count++;
         if (log_count > BUFFER_SIZE) {log_count=BUFFER_SIZE;}
      }
   }
}

void do_user(void) {
   int16 i,pm,lc;
   signed long temp;
   int sensor;

   display_new_data=0;
   lc=log_count;
   pm=pm_address;

   printf("\r\n\r\n\r\nSaved Data (%lu samples for %u channels):\r\n",lc,NUM_SENSORS);
   for (sensor=0;sensor<NUM_SENSORS;sensor++) {
      printf("%4d  ",sensor);
   }
   printf("\r\n");
   for (i=0;i<80;i++) {putc('-');}
   printf("\r\n");

   i=0;
   if (lc != BUFFER_SIZE) {pm=BUFFER_START;}
   while (i<lc) {
      for (sensor=0;sensor<NUM_SENSORS;sensor++) {
         temp=read_program_eeprom(pm++);
         temp-=67;
         if (pm > BUFFER_END) {pm=BUFFER_START;}
         printf("%4ld  ",temp);
      }
      printf("\r\n");
      i+=NUM_SENSORS;
   }
   printf("\r\n\r\n");

   display_new_data=1;
}

void main(void) {
   char c;
   int sensor;
   setup_timer_2(T2_DIV_BY_16,20,16);  //interrupt every 1ms

   for (sensor=0;sensor<NUM_SENSORS;sensor++) {
      init_temp(sensor);
   }

   printf("\r\n\r\nData Logger\r\n\r\n");

   delay_ms(5);

   enable_interrupts(int_timer2);
   enable_interrupts(global);


   while(TRUE) {
      if (kbhit()) {
         c=getc();
         if (c==' ') {do_user();}
         else if (c=='z') {timer2_count=INTERRUPTS_PER_MINUTE;}
      }
   }
}
