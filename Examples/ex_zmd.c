/////////////////////////////////////////////////////////////////////////
////                              EX_ZMD.C                           ////
////                                                                 ////
//// An 'everything but the kitchen sink' example for the ZMD44102   ////
//// radio.  This example was meant to be run on the CCS Wireless    ////
//// (ZMD) Prototyping Board.                                        ////
////                                                                 ////
//// This example can by dynamically configured, over RS232, to      ////
//// perform any of the following tasks/test: 7 segment LEDs,        ////
//// LEDs, read 802.15.4 packets, write 802.15.4 packets, 802.15.4   ////
//// packet sniffer and an RS232<->802.15.4 link.                    ////
////                                                                 ////
//// If using SIOW or Hyperterminal, press C to configure the unit.  ////
//// Here you can choose the TX strength, addresses, etc.  The last  ////
//// option asks you what you want to display on the 7segment LED.   ////
//// Option 3 will display packet success rate (100-0%), option 1    ////
//// will display packet success rate in tens (which will save you   ////
//// you battery power over option 3), option N will display either  ////
//// the packet count or the last received value.                    ////
////                                                                 ////
//// The unit will save the configuration value and current          ////
//// operational mode to EEPROM, so you can power it down and move   ////
//// it and it will keep its settings (so you can put it into a      ////
//// transmit test, power down the board and move it to a new        ////
//// location, then power it back up and it will continue the        ////
//// transmit test).                                                 ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2006 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#include <18F452.h>
#fuses HS,WDT,WDT128,NOLVP
#use delay(clock=10000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7, errors)

#define BUTTON_PRESSED()  !input(PIN_A4)
#define PIN_LED1  PIN_B4
#define PIN_LED2  PIN_B5
#define PIN_LED3  PIN_A5
#define LED_ON output_low
#define LED_OFF output_high

#define EE_MAGIC     0
#define EE_LAST_MODE 1
#define EE_CHAN      4
#define EE_PAN       5  //size=2
#define EE_MY_ADDR   7  //size=2
#define EE_DEST_ADDR 9  //size=2
#define EE_SHOW_PERCENT 11
#define EE_POWER     12
#define EE_CSMA      13

int8 read_eeprom8(int8 address) {
   return(read_eeprom(address));
}

void write_eeprom8(int8 address, int8 data) {
   write_eeprom(address, data);
}

int16 read_eeprom16(int8 address) {
   union {
      int8 bytes[2];
      int16 val;
   } data;

   data.bytes[0]=read_eeprom8(address++);
   data.bytes[1]=read_eeprom8(address);

   return(data.val);
}

void write_eeprom16(int8 address, int16 data) {
   write_eeprom8(address++, make8(data,0));
   write_eeprom8(address, make8(data,1));
}

#define RS232_BUFFER_SIZE  127
char rs232_buffer[RS232_BUFFER_SIZE];
int8 rs232_nextin=0, rs232_nextout=0;

#int_rda
void isr_rda(void) {
   char c;
   c=getc();
   rs232_buffer[rs232_nextin++]=c;
   if (rs232_nextin >= RS232_BUFFER_SIZE) {rs232_nextin=0;}
}

char bgetc(void) {
   char c;
   while (rs232_nextout==rs232_nextin) {restart_wdt();}
   c=rs232_buffer[rs232_nextout++];
   if (rs232_nextout >= RS232_BUFFER_SIZE) {rs232_nextout=0;}
   return(c);
}

#include <stdlib.h>

void bget_string(char* s, int max) {
   int len;
   char c;

   --max;
   len=0;
   do {
     c=bgetc();
     if(c==8) {  // Backspace
        if(len>0) {
          len--;
          putc(c);
          putc(' ');
          putc(c);
        }
     } else if ((c>=' ')&&(c<='~'))
       if(len<max) {
         s[len++]=c;
         putc(c);
       }
   } while(c!=13);
   s[len]=0;
}

signed int bget_int()
{
  char s[3];
  signed int i;

  bget_string(s, sizeof(s)-1);
  i=atoi(s);
  return(i);
}

signed long bget_long() {
  char s[7];
  signed long l;

  bget_string(s, sizeof(s)-1);
  l=atol(s);
  return(l);
}

#define bkbhit() (rs232_nextin!=rs232_nextout)

#define EXP_OUT_ENABLE  PIN_B2
#define EXP_OUT_CLOCK   PIN_B4
#define EXP_OUT_DO      PIN_B5
#define NUMBER_OF_74595 3
#include <74595.c>

const char digit_format[10]={
//0   1     2     3     4     5     6     7     8     9
0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90
};

void lcd_clear(void) {
   int8 digits[3]={0xFF,0xFF,0xFF};
   write_expanded_outputs(&digits[0]);
   output_low(EXP_OUT_ENABLE);
}

void lcd_putd(int16 num) {
   int8 blank=0;
   int8 digits[3];

   digits[0]=num / 100;
   digits[1]=(num % 100) / 10;
   digits[2]=num % 10;

   if (digits[0])
      digits[0]=digit_format[digits[0]];
   else
   {
      blank=1;
      digits[0]=0xFF;
   }

   if (digits[1] || (blank!=1))
      digits[1]=digit_format[digits[1]];
   else
      digits[1]=0xFF;

   digits[2]=digit_format[digits[2]];

   write_expanded_outputs(&digits[0]);
   output_low(EXP_OUT_ENABLE);
}

#include <zmd44102.h>
#include <zrs232.h>

void zmd_spi_test(void) {
   int16 to=1;

   printf("\r\n\nSPI/PARALLEL TEST (Press space to exit)\r\n\n");

   while(TRUE) {
      restart_wdt();
      to--;
      delay_ms(1);

      if (!to) {
         printf("\rED0: %U     ", zmd_read_reg(ZMD_ED0));
         to=500;
      }

      if (bkbhit()) {
         if (bgetc()==' ')
            return;
      }
   }
}

void zmd_generate_cw(int8 channel, int8 modulated) {
   int8 power;

   power=read_eeprom(EE_POWER);

   if (modulated)
      ZPhy_SetTX(ZPHY_TX_MODE_MODULATED_CARRIER, power);
   else
      ZPhy_SetTX(ZPHY_TX_MODE_CARRIER, power);

   ZMac_SetTX(ZMAC_TX_MODE_DIRECT, ZMAC_TX_SLOTTED_DISABLE);
   ZPhy_SetChannel(channel);
   zmd_set_maccontrol(ZMD_mc_TxOn);

   printf("\r\n(press space to exit)\r\n\n");
   while(TRUE) {
      restart_wdt();
      if (bkbhit()) {
         if (bgetc()==' ') {
            return;
         }
      }
   }

   zmd_set_maccontrol(ZMD_mc_TxRxOff);
   ZMac_SetTX(read_eeprom(EE_CSMA), ZMAC_TX_SLOTTED_DISABLE);
   ZPhy_SetTX(ZPHY_TX_MODE_NORMAL, power);
}

void ZMac_DisplayHeader(ZMAC_HEADER *header)
{
   ZMAC_HEADER hdr;
   int8 mhr2;

   memcpy(&hdr, header, sizeof(ZMAC_HEADER));

   mhr2 = hdr.destNode.addressMode;
   mhr2 |= (hdr.sourceNode.addressMode << 4);

   printf("MHR=%X%X SQ=%X ", (int8)hdr.frameCon, mhr2, hdr.seq);

   if (hdr.frameCon.frameType == ZMAC_FCON_TYPE_DATA)
      printf("DATA ");
   else if (hdr.frameCon.frameType == ZMAC_FCON_TYPE_CMD)
      printf("CMD ");
   else if (hdr.frameCon.frameType == ZMAC_FCON_TYPE_ACK)
   {
      printf("ACK ");
      return;
   }
   else
      printf("BEACON ");

   if (hdr.destNode.addressMode)
   {
      printf("DPAN=%LX DADR=", hdr.destNode.panId.w);
      if (hdr.destNode.addressMode == ZMAC_ADDRESS_MODE_SHORT)
      {
         printf("%LX", hdr.destNode.shortAddress.w);
      }
      else
      {
         printf("%X%X%X%X%X%X%X%X",
         hdr.destNode.longAddress.b[0],
         hdr.destNode.longAddress.b[1],
         hdr.destNode.longAddress.b[2],
         hdr.destNode.longAddress.b[3],
         hdr.destNode.longAddress.b[4],
         hdr.destNode.longAddress.b[5],
         hdr.destNode.longAddress.b[6],
         hdr.destNode.longAddress.b[7]);
      }
      printf(" ");
   }
   if (hdr.sourceNode.addressMode)
   {
      if (!hdr.frameCon.intraPan)
      {
         printf("SPAN=%LX ", hdr.sourceNode.panId.w);
      }
      printf("SADR=");

      if (hdr.destNode.addressMode == ZMAC_ADDRESS_MODE_SHORT)
      {
         printf("%LX", hdr.sourceNode.shortAddress.w);
      }
      else
      {
         printf("%X%X%X%X%X%X%X%X",
         hdr.sourceNode.longAddress.b[0],
         hdr.sourceNode.longAddress.b[1],
         hdr.sourceNode.longAddress.b[2],
         hdr.sourceNode.longAddress.b[3],
         hdr.sourceNode.longAddress.b[4],
         hdr.sourceNode.longAddress.b[5],
         hdr.sourceNode.longAddress.b[6],
         hdr.sourceNode.longAddress.b[7]);
      }
      printf(" ");
   }
}

void packet_sniff(void) {
   int16 count=0;
   int8 len;
   ZMAC_HEADER hdr;

   printf("\r\n\nPacket sniffer CH=%U\r\n(press space to exit)\r\n", ZPhy_GetChannel());

   zmd_storeLQI(TRUE);
   ZMac_StartPromiscous();
   ZMac_StartListen();

   lcd_putd(0);

   while(TRUE) {
      restart_wdt();
      if(bkbhit()) {
         if (bgetc()==' ') break;
      }
      if (ZMac_IsRXReady())
      {
            count++;
            if (count>999) {count=0;}
            lcd_putd(count);

            len=ZMac_GetHeader(&hdr);

            printf("\r\n * ");
            ZMac_DisplayHeader(&hdr);

            printf("DLEN=%U ",len);
            while(len--) {
               printf("%X",ZMac_Getc());
            }

            ZMac_DiscardRX();
            printf(" LQI=%LX", zmd_getLQI());
         }
      }

   ZMac_StopListen();
   ZMac_StopPromiscous();
   zmd_storeLQI(FALSE);
   ZMac_DiscardRXAll();
}

#define GetAbsDiff(a, b)       ( (a < b) ? ((0xffff - b) + a) : (a - b) )

void read_test(void) {
   char showPercent;
   int8 percent;
   int16 bad=0, good=0;
   ZMAC_NODE_INFO remoteNode;
   int16 data,lastdata;
   int1 first=TRUE;
   int8 count;

   showPercent=read_eeprom(EE_SHOW_PERCENT);

   printf("\r\n\nReading Packets CHAN=%U PAN=%LX ME=%LX\r\n",
      ZPhy_GetChannel(),
      ZMac_GetMyPanId(),
      ZMac_GetMyShortAddress()
   );
   printf("(press space to exit)\r\n\n");

   while(TRUE) {
      restart_wdt();
      if(bkbhit()) {
         if (bgetc()==' ') break;
      }
      count=ZMac_GetPacket(&remoteNode, &data, sizeof(data), 200);
      if (count)
      {
         if (showPercent == 'n')
            lcd_putd(data);
         else
         {
            if (!first) {
               good++;
               bad+=GetAbsDiff(data, lastdata)-1;
               percent=(int32)100*good/(good+bad);
               if (showPercent=='1')
                  lcd_putd(percent / 10);
               else
                  lcd_putd(percent);

               printf("\r\n%LU %LU %LU %U", data, good, bad, percent);
            }
            lastdata=data;
            first=0;
         }
      }
   }
}

void serial_test(void)
{
   char c=' ';

   printf("\r\n\nZRS232 Test CHAN=%U PAN=%LX ME=%LX DEST=%LX\r\n",
      ZPhy_GetChannel(),
      ZMac_GetMyPanId(),
      ZMac_GetMyShortAddress(),
      read_eeprom16(EE_DEST_ADDR)
   );
   printf("(press '!' to exit)\r\n\n");

   ZRS232Init();
   ZRS232Connect(read_eeprom16(EE_DEST_ADDR));

   do
   {
      restart_wdt();
      ZRS232Task();
      while (ZRS232kbhit())
      {
         c=ZRS232getc();
         putc(c);
      }
      while (bkbhit())
      {
         c=bgetc();
         if (c=='!')
            break;
         else
            ZRS232putc(c);
      }
   } while (c!='!');

   ZRS232Stop();
}

void write_test(void)
{
   ZMAC_NODE_INFO remoteNode;
   ZMD_EC ec;
   int16 good=0,total=0;
   int16 to;
   int16 count=0;
   int8 percent;
   char showPercent;

   ZMac_StartListen();  //temp

   showPercent=read_eeprom(EE_SHOW_PERCENT);

   remoteNode.panId=ZMac_GetMyPanId();
   remoteNode.shortAddress=read_eeprom16(EE_DEST_ADDR);
   remoteNode.addressMode=ZMAC_ADDRESS_MODE_SHORT;

   printf("\r\n\nWriting Packets CHAN=%U PAN=%LX ME=%LX DEST=%LX\r\n",
      ZPhy_GetChannel(),
      ZMac_GetMyPanId(),
      ZMac_GetMyShortAddress(),
      remoteNode.shortAddress.w
   );
   printf("(press space to exit)\r\n\n");

   to=1000;

   while(TRUE) {
      restart_wdt();
      delay_ms(1);
      if(bkbhit()) {
         if (bgetc()==' ') break;
      }
      to--;
      if (!to) {
         to=1000;
         count++;
         if (count>=999) {count=0;}
         ec=ZMac_PutPacket(&remoteNode, &count, sizeof(count));
         printf("%X\r\n",ec);

         if (showPercent=='n')
            lcd_putd(count);
         else
         {
            total++;
            if (!ec)
               good++;
            percent=(int32)100*good/total;
            if (showPercent=='1')
               lcd_putd(percent / 10);
            else
               lcd_putd(percent);
         }
      }
   }
}

void show_settings(void)
{
   printf("\r\n");
   printf("\r\nRF Channel: %U", ZPhy_GetChannel());
   printf("\r\nRF Power (0=max, 3=min): %U", read_eeprom(EE_POWER));
   printf("\r\nRF CSMA (1=yes, 2=direct): %U", read_eeprom(EE_CSMA));
   printf("\r\nPAN ID: 0x%LX", read_eeprom16(EE_PAN));
   printf("\r\nMY ADDRESS: 0x%LX", read_eeprom16(EE_MY_ADDR));
   printf("\r\nDEST ADDRESS: 0x%LX", read_eeprom16(EE_DEST_ADDR));
   printf("\r\nDisplay: %C", read_eeprom(EE_SHOW_PERCENT));
}

void configure(void)
{
   int16 v;
   char c;

   printf("\r\n");

   printf("\r\nRF Channel (0-10): ");
   c=bget_int();
   write_eeprom(EE_CHAN,c);
   ZPhy_SetChannel(c);

   printf("\r\nRF Power (0=max, 3=min): ");
   do
   {
      c=bgetc();
   } while ((c>'3')&&(c<'0'));
   putc(c);
   c-='0';
   ZPhy_SetTX(ZPHY_TX_MODE_NORMAL, c);
   write_eeprom(EE_POWER, c);

   printf("\r\nRF CSMA (1=yes, 2=direct): ");
   do
   {
      c=bgetc();
   } while ((c!='1')&&(c!='2'));
   putc(c);
   c-='0';
   write_eeprom(EE_CSMA, c);
   ZMac_SetTX(c, ZMAC_TX_SLOTTED_DISABLE);

   printf("\r\nPAN ID: ");
   v=bget_long();
   write_eeprom16(EE_PAN,v);
   ZMac_SetMyPanId(v);

   printf("\r\nMY ADDRESS: ");
   v=bget_long();
   write_eeprom16(EE_MY_ADDR,v);
   ZMac_SetMyShortAddress(v);

   printf("\r\nDEST ADDRESS: ");
   v=bget_long();
   write_eeprom16(EE_DEST_ADDR,v);

   printf("\r\nShow Percent (3, 1, or N): ");
   do
   {
      c=bgetc();
      c=tolower(c);
   } while ((c!='3')&&(c!='1')&&(c!='n'));
   write_eeprom(EE_SHOW_PERCENT,c);
}

void lcd_test(void) {
   int16 to;
   int16 temp;

   to=1000;
   temp=123;
   printf("\r\n\nLCD Test\r\n(press space to exit)\r\n\n");

   while (TRUE) {
      restart_wdt();
      if (bkbhit()) {
         if (bgetc()==' ') break;
      }
      delay_ms(1);
      to--;
      if (!to) {
         to=1000;
         lcd_putd(temp);
         temp+=111;
         if (temp>999) {temp=123;}
      }
   }
   lcd_clear();
}

void led_test(void) {
   int16 to;
   char c;

   to=1000;
   c=0;
   printf("\r\n\nTesting LEDs\r\n(press space to exit)\r\n\n");

   while(TRUE) {
      delay_ms(1);
      restart_wdt();
      to--;
      if(bkbhit()) {
         if (bgetc()==' ') break;
      }
      if (!to) {
         to=1000;
         if (bit_test(c,0)) output_low(PIN_LED1); else output_high(PIN_LED1);
         if (bit_test(c,1)) output_low(PIN_LED2); else output_high(PIN_LED2);
         if (bit_test(c,2)) output_low(PIN_LED3); else output_high(PIN_LED3);
         c++;
         if (c>=8) {c=0;}
      }
   }
}


void handle_command(char command) {
   switch(command) {
      case 'u':   //unmod carrier
         printf("\r\n\nGenerating unmodulated carrier wave on channel %U\r\n", read_eeprom(EE_CHAN));
         zmd_generate_cw(read_eeprom(EE_CHAN), 0);
         break;

      case 'm':
         printf("\r\n\nGenerating modulated carrier wave on channel %U\r\n", read_eeprom(EE_CHAN));
         zmd_generate_cw(read_eeprom(EE_CHAN), 1);
         break;

      case '?':
         show_settings();
         break;

      case 's':
         serial_test();
         break;

      case 'r':
         read_test();
         break;

      case 'p':
         packet_sniff();
         break;

      case 'c':
         configure();
         break;

      case 't':
         zmd_spi_test();
         break;

      case 'w':
         write_test();
         break;

      case 'l':
         lcd_test();
         break;

      case 'b':
         led_test();
         break;
   }
}

char menu(void) {
   char mode;

   printf("\r\n\nZIGBEE TEST\r\n\n");
   printf("?) Settings        C)onfigure\r\n");
   printf("U)nmod carrier     M)od carrier\r\n");
   printf("L)CD test          B)link LEDs\r\n");
   printf("R)ead test         W)rite test\r\n");
   printf("S)erial test       P)acket sniffer\r\n");
   printf("T)est SPI/Parallel\r\n");
   printf("Command: ");

   do {
      restart_wdt();
   } while (!bkbhit());

   mode=tolower(bgetc());
   putc(mode);

   return(mode);
}

void init(void)
{
   delay_ms(72);

   ZMac_Init();
   ZPhy_SetTX(ZPHY_TX_MODE_NORMAL, read_eeprom(EE_POWER));
   ZMac_SetTX(read_eeprom(EE_CSMA), ZMAC_TX_SLOTTED_DISABLE);

   lcd_clear();

   setup_adc_ports(NO_ANALOGS);

   LED_ON(PIN_LED1);
   LED_OFF(PIN_LED2);
   LED_OFF(PIN_LED3);

   if (read_eeprom(EE_MAGIC)!=0x58) {
      write_eeprom(EE_MAGIC,0x58);
      write_eeprom(EE_LAST_MODE,0);
      write_eeprom(EE_CHAN, 6);
      write_eeprom16(EE_PAN, 1);
      write_eeprom16(EE_MY_ADDR, 1);
      write_eeprom16(EE_DEST_ADDR, 2);
      write_eeprom(EE_POWER, 0);
      write_eeprom(EE_SHOW_PERCENT, 'n');
      write_eeprom(EE_CSMA, 1);
   }

   ZPhy_SetChannel(read_eeprom(EE_CHAN));
   ZMac_SetMyShortAddress(read_eeprom16(EE_MY_ADDR));
   ZMac_SetMyPanId(read_eeprom16(EE_PAN));
   ZMac_UseMyShortAddress();

   printf("\r\n\nED0: %U", zmd_read_reg(ZMD_ED0));

   enable_interrupts(INT_RDA);
   enable_interrupts(GLOBAL);
}

void main(void) {
   char mode;

   init();

   while (TRUE) {

      if (read_eeprom(EE_LAST_MODE)==0) {
         mode=menu();
         write_eeprom(EE_LAST_MODE, mode);
      }
      else {
         mode=read_eeprom(EE_LAST_MODE);
      }

      handle_command(mode);

      write_eeprom(EE_LAST_MODE, 0);
   }
}


