/////////////////////////////////////////////////////////////////////////
////                        ex_dnslookup.c                           ////
////                                                                 ////
//// Uses s7600.h (Seiko s7600 TCP/IP stack) and dns.c (CCS provided ////
//// DNS resolver driver) to perform DNS lookups - ie get the IP     ////
//// address of a specified hostname.                                ////
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
#include <s7600.h>
#include <dns.c>

//EEPROM Locations
#define ROM_PHONENUM       0
#define ROM_PHONENUM_SIZE  16

#define ROM_USERNAME       16
#define ROM_USERNAME_SIZE  32

#define ROM_PASSWORD       48
#define ROM_PASSWORD_SIZE  16

#define BUTTON1 PIN_C1
#define BUTTON2 PIN_C0

void dns_lookup(void) {
   IPAddr dns1, dns2;
   dns_return_type ret;
   char * hostname[40];
   char * err_str[]="DNS ERROR";

   dns1=Make32(169,207,1,3);
   dns2=Make32(209,153,128,4);

   printf("\r\n\r\nHostname to DNS resolve: ");

   get_string(hostname,40);

   printf("\r\nAttempting to resolve %s (using %U.%U.%U.%U) ....", hostname, make8(dns1,3),make8(dns1,2),make8(dns1,1),make8(dns1,0));

   ret=dns_resolve_name(tcp_find_free_socket(), dns1, 53, hostname); //dns lookups;

   if (ret.ec != OK) {
      display_error(err_str, ret.ec);
      printf("\r\nAttempting to resolve %s (using %U.%U.%U.%U) ....", hostname, make8(dns2,3),make8(dns2,2),make8(dns2,1),make8(dns2,0));
      ret=dns_resolve_name(tcp_find_free_socket(), dns2, 53, hostname); //dns lookups;
   }

   if (ret.ec==OK) {
      printf("\r\n%s IP Address: %U.%U.%U.%U", hostname,make8(ret.ip,3),make8(ret.ip,2),make8(ret.ip,1),make8(ret.ip,0));
   }
   else {
      display_error(err_str, ret.ec);
   }
}

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

   printf("\r\nSaved\r\n");
}

s7600_ec dialup() {
   char err_str[]="PPP CONNECT";
   char phone[ROM_PHONENUM_SIZE];
   char username[ROM_USERNAME_SIZE];
   char password[ROM_PASSWORD_SIZE];
   s7600_ec ec;

   read_eeprom_string(phone, ROM_PHONENUM, ROM_PHONENUM_SIZE);
   read_eeprom_string(username, ROM_USERNAME, ROM_USERNAME_SIZE);
   read_eeprom_string(password, ROM_PASSWORD, ROM_PASSWORD_SIZE);

   ec=ppp_connect(phone,username,password);
   display_error(err_str,ec);        //parse and display error reason if there was one.
   return(ec);
}

void main() {
   setup_adc_ports(ALL_ANALOG);
   setup_adc(ADC_CLOCK_INTERNAL);

   debug("\r\nSTART");

   tcpip_init();

   printf("\r\nPress BUTTON1 when not connected to dial ISP.");
   printf("\r\nPress BUTTON2 when not connected to change between MODEM modes.");
   printf("\r\nPress SPACE when not connected to change ISP settings.");
   printf("\r\nPress SPACE when conncted to ask for DNS-Lookup.\r\n");

   while (TRUE) {
      delay_ms(1);

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

      if (MyIPAddr && kbhit()) {
         if (getc() == ' ') {
            dns_lookup();
         }
      }

      else if (!input(BUTTON1) && !MyIPAddr) {
         debug("\r\nCALLING");
         dialup();
         if (MyIPAddr) {
            debug("\r\nIP: %u.%u.%u.%u",make8(MyIPAddr,3),make8(MyIPAddr,2),make8(MyIPAddr,1),make8(MyIPAddr,0));
         }
         delay_ms(100); //debounce
      }

      if (MyIPAddr) {
         ppp_keepalive();
      }
   }
}


