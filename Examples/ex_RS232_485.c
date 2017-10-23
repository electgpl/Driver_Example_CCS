/////////////////////////////////////////////////////////////////////////
////                             EX_RS232_485.C                      ////
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
////        (C) Copyright 1996,2003 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#include <16F627A.h>
#device *=16
#fuses HS, NOWDT, NOLVP, NOBROWNOUT, NOPROTECT, PUT
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_B2, rcv=PIN_B1, stream=PC)

// Defines the destination's RS485 ID
#define  RS485_DEST_ID        0x11

// Defines the device's RS485 ID
#define  RS485_ID             0x7F
#define  RS485_USE_EXT_INT    TRUE
#include <rs485.c>

int8 buffer[40];
int8 next_in  = 0;
int8 next_out = 0;

#INT_RDA
void serial_isr()
{
   int8 t;

   buffer[next_in] = fgetc(PC);
   t = next_in;
   next_in = (next_in+1) % sizeof(buffer);
   if(next_in == next_out)
      next_in=t;        // Buffer full !!
}

#define bkbhit (next_in != next_out)

int8 bgetc()
{
   int8 c;

   while(!bkbhit);
   c = buffer[next_out];
   next_out = (next_out+1) % sizeof(buffer);
   return c;
}

void main() {
   int8 i, msg[32];

   enable_interrupts(INT_RDA);
   enable_interrupts(GLOBAL);
   rs485_init();

   for(;;) {
      if(rs485_get_message(msg, FALSE)) {
         for(i=0; i<msg[1]; ++i)
            fputc(msg[i+2], PC);
      }

      for(i=0; bkbhit && i<sizeof(msg); ++i)
         msg[i] = bgetc();

      if(i > 0) {
         rs485_wait_for_bus(FALSE);
         while(!rs485_send_message(RS485_DEST_ID, i, msg)) {
            delay_ms(RS485_ID);
         }
      }
   }
}
