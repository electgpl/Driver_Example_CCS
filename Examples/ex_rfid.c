/////////////////////////////////////////////////////////////////////////
////                          EX_RFID.C                              ////
////                                                                 ////
////  This example shows how to read an RFID number from an RFID     ////
////  read-only transponder.  It will work on a CCS RFID Development ////
////  kit.  Checks received RFID, and compares the ID to an ID       ////
////  stored in the EEPROM - if the IDs match then it lights the     ////
////  green LED.  If the IDs don't match then it lights the red      ////
////  LED.                                                           ////
////                                                                 ////
////  This example will work with the PCM and PCH compilers.  The    ////
////  following conditional compilation lines are used to include a  ////
////  valid device for each compiler.  Change the device, clock and  ////
////  RS232 pins for your hardware if needed.                        ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2006 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#include  <16F876A.h>

//seting the u-chip
#if defined(__PCM__)
#device *=16
#fuses HS,NOWDT,NOPROTECT,NOLVP,NOBROWNOUT,PUT
#use delay(clock=20000000)

#elif defined(__PCH__)
#include <18F452.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#endif

// RS485 adresses
#define  RS485_ID 0x11
#define  RS485_USE_EXT_INT FALSE
#define  ADAPTER_RS485_ID  0x7f

// definition of LEDs
#define GREEN_LED PIN_C3
#define YELLOW_LED PIN_C4
#define RED_LED PIN_C5

#include <em4095.c>
#include <em4102.c>

#include <rs485.c>

//string used for sending using RS485
int8 msg[32];

//definition of two-color LED states
typedef enum {OFF, GREEN, RED} LEDcolor;

// function for setting two-color LED
void twoColorLED(LEDcolor color) {
   switch(color) {
      case OFF:
         output_low(PIN_A3);
         output_low(PIN_A5);
         break;
      case GREEN:
         output_high(PIN_A3);
         output_low(PIN_A5);
         break;
      case RED:
         output_low(PIN_A3);
         output_high(PIN_A5);
         break;
   }
}

// sends a string using RS485
void RS485send(char* s) {
   int8 size;
   for(size=0; s[size]!='\0'; ++size);
   rs485_wait_for_bus(FALSE);
   while (!rs485_send_message(ADAPTER_RS485_ID, size, s)) {
      delay_ms(RS485_ID);
   }
}

// read a char using RS485
char RS485getc() {
   rs485_get_message(msg, TRUE);
   return msg[2];
}

// RFID database, terminated with 5xzero

#define EEPROMCEIL 250            // MUST be under 255-5=250
#define EEPROMBASE 0x2100         // base address of EEPROM
#rom  EEPROMBASE = { 32, 72,185, 90,  2,
                      1,  2,  3,  4,  5,
                      0,  0,  0,  0,  0
                   }              // EEPROM data

int8 code[5];

//check if the data (ID) match any ID stored and EEPROM and returns 1 if it is true
int check_id(int8* data) {       
  int8 tmp[5];
  int i,j;

  j=0;
  while(j<EEPROMCEIL) {          //till the ceil of EEPROM
     for(i=0;i<=4;i++) {
       tmp[i]=read_eeprom(i+j);
     }
     if((tmp[0] | tmp[1] | tmp[2] | tmp[3] | tmp[4])!=0)   // isn't all bits == 0?
     {
        if ((tmp[0]==data[0])&&(tmp[1]==data[1])&&(tmp[2]==data[2])&&(tmp[3]==data[3])&&(tmp[4]==data[4]))  //check for matching
           return 1;
        j+=5;
     } else
         return 0;
   }
   return 0;

}

// -------------- main 
void main(void) {

   int8  wrong_attemps;
   int32 tagNum;
   int8  customerCode;

   rf_init();     //initialization
   rf_powerUp();

   // green LED on
   output_low(GREEN_LED);
   twoColorLED(OFF);

   wrong_attemps = 0;
   
   // sent a message using RS485
   sprintf(msg,"\n\r\n\rReady...\n\r");
   RS485send(msg);

   while(1) {

      if(read_4102(code))  //read the code
      {
         tagNum = make32(code[1],code[2],code[3],code[4]);
         customerCode = code[0];
         sprintf(msg,"\r\nScanned ID: %u-%lu",customerCode,tagNum);
         RS485send(msg);         
         sprintf(msg,"( [%u][%u]",code[0],code[1]);
         RS485send(msg);
         sprintf(msg,"[%u][%u][%u] )",code[2],code[3],code[4]);
         RS485send(msg);
         
         
         if(check_id(code)==1)   // and check the code
         {
           wrong_attemps=0;
           sprintf(msg,"\r\nOK - Approved code");
           RS485send(msg);
           twoColorLED(GREEN);
           delay_ms(800);
           twoColorLED(OFF);

         } else {
           wrong_attemps++;
           sprintf(msg,"\r\nX  - Disapproved code");
           RS485send(msg);
           twoColorLED(RED);
           delay_ms(800);
           twoColorLED(OFF);
         }

         if (wrong_attemps>4) {  // if there is too many attemps, disable for a while
           sprintf(msg,"\r\nToo many attemps!");
           RS485send(msg);
           output_low(RED_LED);
           delay_ms(10000);
           output_high(RED_LED);
           wrong_attemps=0;
         }
      }
   }
}
