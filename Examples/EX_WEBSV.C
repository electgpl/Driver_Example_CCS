/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////              ex_websv - A simple HTTP web server                ////
////                                                                 ////
//// The program creates a very rudimentary HTTP web server on the   ////
//// PIC.  It only displays one page, and it doesn't read the GET    ////
//// variable from the HTTP request to know what page the web client ////
//// is requesting.  It does, however, show how to create dynamic    ////
//// content in that it reads the A/D reading and displays that to   ////
//// the user.  It also demonstrates a possible method of reading    ////
//// tags in the HTTP header by reading and displaying the users     ////
//// web browser in the web page.                                    ////
////                                                                 ////
//// Another interesting feature of this firmware is the ability to  ////
//// telnet into Port 6666 and change the dynamic message that is    ////
//// stored into the flash ROM.  When telneting into port 6666 you   ////
//// must use the RAW protocol since s7600 isn't compatable with the ////
//// telnet protocol.                                                ////
////                                                                 ////
//// The ISP information and dynamic email message are stored in the ////
//// internal flash ROM, and can be changed by pressing the Space    ////
//// bar on the computer connected to the PIC via RS232.             ////
////                                                                 ////
//// Press Button 1 when connected to dial into the ISP and make a   ////
//// PPP/TCPIP connection.  Pressing Button 1 when the PIC is        ////
//// connected to the internet will close the PPP/TCPIP connection   ////
//// and hangup the modem.                                           ////
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
#fuses HS,NOWDT,NOPROTECT,NOBROWNOUT,NOLVP,NOPUT
#use delay(clock=19660000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7, errors)

#include <ctype.h>
#include <string.h>
#include <input.c>
#include "s7600.h"

//EEPROM Locations
#define ROM_PHONENUM       0
#define ROM_PHONENUM_SIZE  16

#define ROM_USERNAME       16
#define ROM_USERNAME_SIZE  32

#define ROM_PASSWORD       48
#define ROM_PASSWORD_SIZE  16

#define ROM_MESSAGE        64
#define ROM_MESSAGE_SIZE   80

#define BUTTON1 PIN_C1
#define BUTTON2 PIN_C0

void external_modem(void) {
   printf("\r\n\r\nModem in AT command mode:\r\n");
   delay_ms(250); //debounce
   while (input(BUTTON2)) {
      if (kbhit()) {
         s7600_serial_write(getc());
      }
      if (s7600_serial_kbhit()) {
         putc(s7600_serial_read());
      }
   }
   printf("\r\nModem out of AT command mode.");
   delay_ms(250);
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
   char string[ROM_USERNAME_SIZE];

   printf("\r\nPhone Number: ");
   get_string(string, ROM_PHONENUM_SIZE);
   write_eeprom_string(string, ROM_PHONENUM, ROM_PHONENUM_SIZE);

   printf("\r\nUser Name: ");
   get_string(string, ROM_USERNAME_SIZE);
   write_eeprom_string(string, ROM_USERNAME, ROM_USERNAME_SIZE);

   printf("\r\nPassword: ");
   get_string(string, ROM_PASSWORD_SIZE);
   write_eeprom_string(string, ROM_PASSWORD, ROM_PASSWORD_SIZE);

   printf("\r\nMessage: ");
   get_string(string, ROM_MESSAGE_SIZE);
   write_eeprom_string(string, ROM_MESSAGE, ROM_MESSAGE_SIZE);

   printf("\r\nSaved\r\n");
}

s7600_ec dialup() {
   char phone[ROM_PHONENUM_SIZE];
   char username[ROM_USERNAME_SIZE];
   char password[ROM_PASSWORD_SIZE];
   char err_str[]="PPP CONNECT";
   s7600_ec ec;

   read_eeprom_string(phone, ROM_PHONENUM, ROM_PHONENUM_SIZE);
   read_eeprom_string(username, ROM_USERNAME, ROM_USERNAME_SIZE);
   read_eeprom_string(password, ROM_PASSWORD, ROM_PASSWORD_SIZE);

   ec=ppp_connect(phone,username,password);
   display_error(err_str,ec);        //parse and display error reason if there was one.
   
   return(ec);
}

void parse_http_header(char * input, char * key, char * output,int8 max_size)
{
   int8 ptr,end,i;

   if (strstr(input,key)) {
      ptr=strchr(input,':') + 2;
      end=strlen(input)+input;
      for (i=ptr;i<end;i++) {
         if ((*ptr == 0x0A) || (*ptr == 0x0D)) {break;}
         *output=*ptr;
         ptr++;
         output++;
      }
      *output=0;
   }
}

void webserver(int8 socket) {
   #DEFINE MAX_DYNAMIC_FRAGMENT 80
   const char string1[]="<HTML><HEAD><TITLE>PICnet</TITLE></HEAD>";
   const char string2[]="<BODY><H1>Welcome to PICnet</H1>";
   const char string3[]="<P><B>A/D Reading:</B> ";
   const char string4[]="<P><B>Current Message:</B> ";
   const char string5[]="<P><B>Your Browswer Information:</B> ";
   const char string6[]="<P><A HREF=\"http://www.ccsinfo.com\">CCS</A></BODY></HTML>";

   char string[MAX_DYNAMIC_FRAGMENT];
   char browser[MAX_DYNAMIC_FRAGMENT];
   char key[]="User-Agent";
   int i=0;
   char c;

   if (tcp_connected(socket)) {
      debug("\r\nSOCKET %u Connected to: %u.%u.%u.%u",socket,s7600_DMA_Read(SOCKET_PEER_IP_ADDR+3),s7600_DMA_Read(SOCKET_PEER_IP_ADDR+2),s7600_DMA_Read(SOCKET_PEER_IP_ADDR+1),s7600_DMA_Read(SOCKET_PEER_IP_ADDR+0));
      debug("\r\nIncoming HTTP request:\r\n");
      while (!tcp_kbhit(socket)) {} //wait for HTML GET header
      while (tcp_kbhit(socket)) {
         c=tcp_getc(socket);
         debug("%c",c);
         if (c==0x0D) {
            parse_http_header(string,key,browser,MAX_DYNAMIC_FRAGMENT);
            string[0]=0;
            i=0;
         }
         else {
            string[i++]=c;
            string[i]=0;
         }
      }
      set_adc_channel(0);

      sprintf(string,"%s",string1);
      tcp_putd(socket,string,strlen(string));

      sprintf(string,"%s",string2);
      tcp_putd(socket,string,strlen(string));

      sprintf(string,"%s",string3);
      tcp_putd(socket,string,strlen(string)); //ad part 1

      sprintf(string,"0x%X",read_adc());
      tcp_putd(socket,string,strlen(string)); //ad part 2

      sprintf(string,"%s",string4);
      tcp_putd(socket,string,strlen(string)); //message part 1

      read_eeprom_string(string, ROM_MESSAGE, ROM_MESSAGE_SIZE);
      tcp_putd(socket,string,strlen(string)); //message part 2

      sprintf(string,"%s",string5);
      tcp_putd(socket,string,strlen(string));

      tcp_putd(socket,browser,strlen(browser));

      sprintf(string,"%s",string6);
      tcp_putd(socket,string,strlen(string));

      delay_ms(1000);  //give client time to read data before closing, else they may not like it

      tcp_close(socket);
      tcp_listen(socket,80);

      debug("\r\nSOCKET %u Disconnected",socket);
   }
}

void rawserver(int8 socket) {
   char welcome[]="PICnet message: ";
   char incoming[ROM_MESSAGE_SIZE];
   int8 i=0;
   char c;

   if (tcp_connected(socket)) {
      debug("\r\nSOCKET %u Connected to: %u.%u.%u.%u",socket,s7600_DMA_Read(SOCKET_PEER_IP_ADDR+3),s7600_DMA_Read(SOCKET_PEER_IP_ADDR+2),s7600_DMA_Read(SOCKET_PEER_IP_ADDR+1),s7600_DMA_Read(SOCKET_PEER_IP_ADDR+0));
      debug("\r\nIncoming data:\r\n");

      tcp_putd(socket,welcome,strlen(welcome));

      while (tcp_connected(socket)) {
         if (tcp_kbhit(socket)) {
            c=tcp_getc(socket);
            debug("%c",c);
            tcp_putc(socket,c); //echo
            if (c==0x0D) {break;}
            else {incoming[i++]=c;}
         }
      }

      incoming[i]=0;
      write_eeprom_string(incoming, ROM_MESSAGE, ROM_MESSAGE_SIZE);

      debug("\r\nSOCKET %u Disconnected",socket);
      tcp_close(socket);
      tcp_flush_socket(socket);
      tcp_listen(socket,6666); //reset
   }
}

void main() {
   s7600_ec ec;

   setup_adc_ports(ALL_ANALOG);
   setup_adc(ADC_CLOCK_INTERNAL);

   debug("\r\nSTART");

   tcpip_init();

   printf("\r\nPress BUTTON1 when not connected to dial ISP.");
   printf("\r\nPress BUTTON1 when conncted to disconnect from ISP.");
   printf("\r\nPress SPACE when not connected to change settings.\r\n");

   while (TRUE) {
      delay_ms(1);

      if (ppp_check() && MyIPAddr) {
         rawserver(0);
         webserver(1);
      }

      if (!input(BUTTON2) && !MyIPAddr) {
         external_modem();
      }

      if (!input(BUTTON1) && MyIPAddr) {
         debug("\r\nDISCONNECTING");
         ppp_disconnect();
         delay_ms(100); //debounce
      }

      if (!MyIPAddr && kbhit()) {
         if (getc() == ' ') {
            change_settings();
         }
      }

      else if (!input(BUTTON1) && !MyIPAddr) {
         debug("\r\nCALLING");
         dialup();
         if (MyIPAddr)
         {
            debug("\r\nIP: %u.%u.%u.%u",make8(MyIPAddr,3),make8(MyIPAddr,2),make8(MyIPAddr,1),make8(MyIPAddr,0));
            ec=tcp_listen(0,6666);
            ec=tcp_listen(1,80);
         }
         delay_ms(100); //debounce
      }

      if (MyIPAddr) {
         ppp_keepalive();
      }
   }
}


