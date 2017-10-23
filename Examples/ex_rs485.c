/////////////////////////////////////////////////////////////////////////
////                             EX_RS485_CHAT.C                     ////
////                                                                 ////
////     Converts RS485 data and puts it onto RS232 data line.       ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCM,PCH    pin B2 to RS232 RX, pin B1 to RS232 TX           ////
////                                                                 ////
////                                                                 ////
////  The following conditional compilation lines are used to        ////
////  include a valid device for each compiler.  Change the device,  ////
////  clock and RS232 pins for your hardware if needed.              ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2005 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#include <16F628A.h>
#device *=16
#fuses HS, NOWDT, NOLVP, NOBROWNOUT, NOPROTECT, PUT
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_B2, rcv=PIN_B1, stream=PC)

#define  RS485_RX_BUFFER_SIZE 64
#define  RS485_USE_EXT_INT    TRUE

int8 OUR_RS485_ID = 0;
#define RS485_ID OUR_RS485_ID

#include <rs485.c>
#include <stdlib.h>

int8 in_char = 0;
int8 next_in = 0;
int8 next_out = 0;
int8 msg[64];

#INT_RDA
void serial_isr()
{
   in_char = fgetc(PC);
}

#INT_TIMER1
void timer1_isr()
{
   int8 i;

   if(rs485_get_message(msg, FALSE))
   {
      printf("\n\r%d: ", msg[0]);

      for(i=0; i < msg[1]; ++i)
         putc(msg[i+2]);

      printf("\n\r");
   }
}

void RS485send(char* s, int8 id)
{
   int8 size;

   for(size=0; s[size]!='\0'; ++size);

   rs485_wait_for_bus(FALSE);

   while(!rs485_send_message(id, size, s))
      delay_ms(OUR_RS485_ID);
}

char PCgetc()
{
   in_char = 0;

   while(!in_char);

   return in_char;
}

int8 PCgetInt()
{
   int8 i, s[3];

   for(i=0; (s[i]=PCgetc()) != '\r' && i<3; ++i);

   return atoi(s);
}

char* PCgetMsg()
{
   int8 i;

   for(i=0; (msg[i] = PCgetc()) != '\r' && i<64; ++i);

   msg[i] = '\0';

   return &(msg[0]);
}

void main()
{
   int8 i, send_addr = 0;
   char command;

   setup_timer_1(T1_INTERNAL | T1_DIV_BY_1);

   enable_interrupts(INT_RDA);
   enable_interrupts(GLOBAL);

   rs485_init();

   while(OUR_RS485_ID == 0 || OUR_RS485_ID >= 255)
   {
      printf("Please choose a network ID (1-255): ");
      OUR_RS485_ID = PCgetInt();
   }

   printf("\n\rYour ID is: %d\n\r\n\rIf you require assistance, press h.\n\r", OUR_RS485_ID);

   while(command != 'Q')
   {
         enable_interrupts(INT_TIMER1);

         command = toupper(PCgetc());

         disable_interrupts(INT_TIMER1);

         switch(command)
         {
            case 'S':
               if(send_addr == 0 || send_addr > 255)
               {
                  printf("\n\r\n\rEnter send address (1-255): ");
                  send_addr = PCgetInt();
               }

               printf("\n\r%d:>", send_addr);
               RS485send(PCgetMsg(), send_addr);
               printf("\n\r");
               break;

            case 'C':
               send_addr = 0;

               while(send_addr == 0 || send_addr > 255)
               {
                  printf("\n\r\n\rEnter send address (1-255): ");
                  send_addr = PCgetInt();
                  printf("\n\r");
               }
               break;

            case 'I':
               OUR_RS485_ID = 0;

               while(OUR_RS485_ID == 0 || OUR_RS485_ID >= 255)
               {
                  printf("\n\rPlease choose a network ID (1-255): ");
                  OUR_RS485_ID = PCgetInt();
               }
               printf("\n\rYour ID is: %d\n\r\n\r", OUR_RS485_ID);
               break;

            case 'H':
               printf("\n\rCommands: (S)end message, (C)hange Send Address, ");
               printf("Change (I)D, (H)elp, (Q)uit.\n\r");
               break;

            default:
               if(command != 'Q')
                  printf("\n\rInvalid command!\n\r");
         }
   }
}
