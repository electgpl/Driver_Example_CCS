/////////////////////////////////////////////////////////////////////////
////                            EX_CAN.C                             ////
////                                                                 ////
//// Example of CCS's CAN library, using the PIC18Fxx8.  This        ////
//// example was tested using MCP250xxx CAN Developer's Kit.         ////
////                                                                 ////
//// Connect pin B2 (CANTX) to the CANTX pin on the open NODE A of   ////
//// the developer's kit, and connect pin B3 (CANRX) to the CANRX    ////
//// pin on the open NODE A.                                         ////
////                                                                 ////
//// NODE B has an MCP250xxx which sends and responds certan canned  ////
//// messages.  For example, hitting one of the GPX buttons on       ////
//// the development kit causes the MCP250xxx to send a 2 byte       ////
//// message with an ID of 0x290.  After pressing one of those       ////
//// buttons with this firmware you should see this message          ////
//// displayed over RS232.                                           ////
////                                                                 ////
//// NODE B also responds to certain CAN messages.  If you send      ////
//// a request (RTR bit set) with an ID of 0x18 then NODE B will     ////
//// respond with an 8-byte message containing certain readings.     ////
//// This firmware sends this request every 2 seconds, which NODE B  ////
//// responds.                                                       ////
////                                                                 ////
//// If you install Microchip's CANKing software and use the         ////
//// MCP250xxx , you can see all the CAN traffic and validate all    ////
//// experiments.                                                    ////
////                                                                 ////
//// For more documentation on the CCS CAN library, see can-18xxx8.c ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
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

#include <18F248.h>
#fuses HS,NOPROTECT,NOLVP,NOWDT
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#include <can-18xxx8.c>

int16 ms;

#int_timer2
void isr_timer2(void) {
   ms++; //keep a running timer that increments every milli-second
}

void main() {
   struct rx_stat rxstat;
   int32 rx_id;
   int in_data[8];
   int rx_len;

//send a request (tx_rtr=1) for 8 bytes of data (tx_len=8) from id 24 (tx_id=24)
   int out_data[8];
   int32 tx_id=24;
   int1 tx_rtr=1;
   int1 tx_ext=0;
   int tx_len=8;
   int tx_pri=3;

   int i;

   for (i=0;i<8;i++) {
      out_data[i]=0;
      in_data[i]=0;
   }

   printf("\r\n\r\nCCS CAN EXAMPLE\r\n");

   setup_timer_2(T2_DIV_BY_4,79,16);   //setup up timer2 to interrupt every 1ms if using 20Mhz clock

   can_init();

   enable_interrupts(INT_TIMER2);   //enable timer2 interrupt
   enable_interrupts(GLOBAL);       //enable all interrupts (else timer2 wont happen)

   printf("\r\nRunning...");

   while(TRUE)
   {

      if ( can_kbhit() )   //if data is waiting in buffer...
      {
         if(can_getd(rx_id, &in_data[0], rx_len, rxstat)) { //...then get data from buffer
            printf("\r\nGOT: BUFF=%U ID=%LU LEN=%U OVF=%U ", rxstat.buffer, rx_id, rx_len, rxstat.err_ovfl);
            printf("FILT=%U RTR=%U EXT=%U INV=%U", rxstat.filthit, rxstat.rtr, rxstat.ext, rxstat.inv);
            printf("\r\n    DATA = ");
            for (i=0;i<rx_len;i++) {
               printf("%X ",in_data[i]);
            }
            printf("\r\n");
         }
         else {
            printf("\r\nFAIL on GETD\r\n");
         }

      }

      //every two seconds, send new data if transmit buffer is empty
      if ( can_tbe() && (ms > 2000))
      {
         ms=0;
         i=can_putd(tx_id, out_data, tx_len,tx_pri,tx_ext,tx_rtr); //put data on transmit buffer
         if (i != 0xFF) { //success, a transmit buffer was open
            printf("\r\nPUT %U: ID=%LU LEN=%U ", i, tx_id, tx_len);
            printf("PRI=%U EXT=%U RTR=%U\r\n   DATA = ", tx_pri, tx_ext, tx_rtr);
            for (i=0;i<tx_len;i++) {
               printf("%X ",out_data[i]);
            }
            printf("\r\n");
         }
         else { //fail, no transmit buffer was open
            printf("\r\nFAIL on PUTD\r\n");
         }
      }
   }
}
