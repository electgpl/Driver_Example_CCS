/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////              ex_mail - A simple SMTP email client               ////
////                                                                 ////
//// This program can send either a static email message at the      ////
//// press of a button, or the user can write his own dynamic email  ////
//// message through RS232.  The ISP information and dynamic email   ////
//// message are stored in the internal flash ROM, and can be        ////
//// changed by pressing the Space bar on the computer connected to  ////
//// the PIC via RS232.                                              ////
////                                                                 ////
//// Press Button 1 (pin C1) when connected to dial into the ISP and ////
//// make a PPP/TCPIP connection.  Once the PIC is connected you can ////
//// press Button 2 (pin C0) to send the static email using the      ////
//// information stored in the flash ROM.  Or you can send a dynamic ////
//// email by pressing spacebar on the computer connected to the PIC ////
//// via RS232.  Pressing Button 1 when the PIC is connected to the  ////
//// internet will close the PPP/TCPIP connection and hangup the     ////
//// modem.                                                          ////
////                                                                 ////
////  This example will work with the PCM and PCH compilers.         ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////


#include <16F877.h>
#device *=16 ADC=8
#fuses HS,NOWDT,NOPROTECT,NOBROWNOUT,NOLVP
#use delay(clock=19660000)
#use rs232(baud=19200, xmit=PIN_C6, rcv=PIN_C7, errors)

#include <ctype.h>
#include <string.h>
#include <s7600.h>
#include <smtp.h>

//EEPROM Locations
#define ROM_PHONENUM       0
#define ROM_PHONENUM_SIZE  16

#define ROM_USERNAME       16
#define ROM_USERNAME_SIZE  32

#define ROM_PASSWORD       48
#define ROM_PASSWORD_SIZE  16

#define ROM_TO             64
#define ROM_TO_SIZE        32

#define ROM_FROM           96
#define ROM_FROM_SIZE      32

#define ROM_SUBJECT        128
#define ROM_SUBJECT_SIZE   30

#define ROM_MESSAGE        158
#define ROM_MESSAGE_SIZE   60

#define BUTTON1 PIN_C1
#define BUTTON2 PIN_C0

IPAddr smtp_server;

//similar to get_string in input.c, but this allows some non-ascii characters in
void get_raw_string(char * s,int max) {
   int len;
   char c;

   max--;
   len=0;
   do {
      c=getc();
      if(c==8) {  // Backspace
         if(len>0) {
            len--;
            putc(c);
            putc(' ');
            putc(c);
         }
      } else if ((c!=0x0D) && (c!=0x0A))
      if(len<max) {
         s[len++]=c;
         putc(c);
      }
   } while(c!=13);
   s[len]=0;
}

void read_eeprom_string(char * array, int8 address, int8 max_size)
{
   int8 i=0;

   *array=0;

   while (i<max_size)
   {
      *array=read_eeprom(address+i);
      if (*array == 0) {i=max_size;}
      else {
         array++;
         *array=0;
      }
      i++;
   }
}

void write_eeprom_string(char * array, int8 address, int8 max_size)
{
   int8 i=0;

   while (i<max_size) {
      write_eeprom(address+i,*array);
      if (*array == 0) {i=max_size;}
      array++;
      i++;
   }
}

void change_settings(void) {
   char string[ROM_MESSAGE_SIZE];

   printf("\r\nPhone Number: ");
   get_raw_string(string, ROM_PHONENUM_SIZE);
   write_eeprom_string(string, ROM_PHONENUM, ROM_PHONENUM_SIZE);

   printf("\r\nUser Name: ");
   get_raw_string(string, ROM_USERNAME_SIZE);
   write_eeprom_string(string, ROM_USERNAME, ROM_USERNAME_SIZE);

   printf("\r\nPassword: ");
   get_raw_string(string, ROM_PASSWORD_SIZE);
   write_eeprom_string(string, ROM_PASSWORD, ROM_PASSWORD_SIZE);

   printf("\r\nTo: ");
   get_raw_string(string, ROM_TO_SIZE);
   write_eeprom_string(string, ROM_TO, ROM_TO_SIZE);

   printf("\r\nFrom: ");
   get_raw_string(string, ROM_FROM_SIZE);
   write_eeprom_string(string, ROM_FROM, ROM_FROM_SIZE);

   printf("\r\nSubject: ");
   get_raw_string(string, ROM_SUBJECT_SIZE);
   write_eeprom_string(string, ROM_SUBJECT, ROM_SUBJECT_SIZE);

   printf("\r\nMessage: ");
   get_raw_string(string, ROM_MESSAGE_SIZE);
   write_eeprom_string(string, ROM_MESSAGE, ROM_MESSAGE_SIZE);

   printf("\r\nSaved\r\n");
}


void static_email() {
   char to[ROM_TO_SIZE];
   char from[ROM_FROM_SIZE];
   char subject[ROM_SUBJECT_SIZE];
   char message[ROM_MESSAGE_SIZE];

   char err_str[]="SEND EMAIL";
   s7600_ec ec;

   read_eeprom_string(to, ROM_TO, ROM_TO_SIZE);
   read_eeprom_string(from, ROM_FROM, ROM_FROM_SIZE);
   read_eeprom_string(subject, ROM_SUBJECT, ROM_SUBJECT_SIZE);
   read_eeprom_string(message, ROM_MESSAGE, ROM_MESSAGE_SIZE);

   ec=email_open(smtp_server, 25, from, to, subject);

   if(ec==OK) {
      ec=email_add(message);
      if (ec==OK) {
         sprintf(message,"\r\n\r\nA/D reading: 0x%X\r\n",read_adc());
         ec=email_add(message);
         if (ec==OK) {ec=email_close();}
      }
   }

   display_error(err_str,ec);        //parse and display error reason if there was one.
}

void dynamic_email() {
   char to[ROM_TO_SIZE];
   char from[ROM_FROM_SIZE];
   char subject[ROM_SUBJECT_SIZE];
   char message[80];
   char err_str[]="MAIL";
   short int done=0;
   s7600_ec ec;

   printf("\r\n\r\nTo: ");
   get_raw_string(to, ROM_TO_SIZE);

   printf("\r\nFrom: ");
   get_raw_string(from, ROM_FROM_SIZE);

    printf("\r\nSubject: ");
   get_raw_string(subject, ROM_SUBJECT_SIZE);

   ec=email_open(smtp_server, 25, from, to, subject);

   if (ec==OK) {
      printf("\r\nBody (Insert a line with a CTRL-D (0x04) to end):\r\n");
   }

   while ( (ec==OK) && (!done) ) {
      get_raw_string(message,80);
      printf("\r\n");
      ec=email_add(message);
      if (strchr(message,0x04)) {done=1;}
   }

   printf("\r\n\r\nSending E-Mail.\r\n");
   email_close();

   display_error(err_str,ec);        //parse and display error reason if there was one.
}

s7600_ec dialup() {
   char phone[ROM_PHONENUM_SIZE];
   char username[ROM_USERNAME_SIZE];
   char password[ROM_PASSWORD_SIZE];
   char err_str[]="PPP";
   s7600_ec ec;

   read_eeprom_string(phone, ROM_PHONENUM, ROM_PHONENUM_SIZE);
   read_eeprom_string(username, ROM_USERNAME, ROM_USERNAME_SIZE);
   read_eeprom_string(password, ROM_PASSWORD, ROM_PASSWORD_SIZE);

   ec=ppp_connect(phone,username,password);
   display_error(err_str,ec);        //parse and display error reason if there was one.

   return ec;
}

void main() {
   s7600_ec ec;
   char c;

   smtp_server=Make32(216,93,66,135);

   setup_adc_ports(ALL_ANALOG);
   setup_adc(ADC_CLOCK_INTERNAL);
   set_adc_channel(0);

   tcpip_init();
   email_init();

   printf("\r\n\r\nPress BUTTON1 when not connected to dial ISP.");
   printf("\r\nPress BUTTON1 when conncted to disconnect from ISP.");
   printf("\r\nPress BUTTON2 when conncted to send a static E-Mail.");
   printf("\r\nPress SPACE when not connected to change settings.");
   printf("\r\nPress SPACE when connected to send a dynamic E-Mail.\r\n");

   debug("\r\nSTART:");

   while (TRUE) {
      delay_ms(1);

      if (!input(BUTTON1) && MyIPAddr) {
         debug("\r\nDISCONNECTING");
         ppp_disconnect();
         MyIPAddr=0;
         delay_ms(100); //debounce
      }

      else if (!input(BUTTON1) && !MyIPAddr) {
         debug("\r\nCALLING");
         ec = dialup();
         if (MyIPAddr)
         {
            debug("\r\nIP: %u.%u.%u.%u",make8(MyIPAddr,3),make8(MyIPAddr,2),make8(MyIPAddr,1),make8(MyIPAddr,0));
         }
         delay_ms(100); //debounce
      }

      if ( !input(BUTTON2) && MyIPAddr && ppp_check() ) {
         debug("\r\nSENDING EMAIL");
         static_email();
      }

      if (kbhit()) {
         c=getc();
         if ((c==' ') && MyIPAddr && ppp_check())
         {
            dynamic_email();
         }
         else if (c==' ')
         {
            change_settings();
         }
      }

   }
}
