/////////////////////////////////////////////////////////////////////////
////                          EX_PBUSM.C                             ////
////                                                                 ////
////  This program shows a simple PIC to PIC message system that     ////
////  uses a one wire interface between the PICs.  Any (reasonable)  ////
////  number of PICs may be connected to the one wire.  The program  ////
////  also shows how to handle two serial ports within one program.  ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////     Install a 1K resistor from pin B0 to +5V                    ////
////     Connect pin B0 to another unit's pin B0                     ////
////     Change OUR_ID below for each PIC                            ////
////     See additional connections below.                           ////
////                                                                 ////
////  Optional: To monitor communications at a PC:                   ////
////     Connect pin B0 to RS232 TX and plug monitor PC into port B  ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCB        pin A2 to RS232 RX, pin A3 to RS232 TX           ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////                                                                 ////
////  This example will work with the PCB, PCM and PCH compilers.    ////
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


#if defined(__PCB__)
#include <16C56.h>
#fuses HS,NOWDT,NOPROTECT
#use delay(clock=20000000)

#elif defined(__PCM__)
#include <16F877.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)

#elif defined(__PCH__)
#include <18F452.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#endif

#use rs232(baud=9600, float_high, bits=9, xmit=PIN_B0, rcv=PIN_B0)

#define OUR_ID 1

#define MAX_MESSAGES 4
#define MAX_LENGTH   7

byte pbus_buffer[MAX_MESSAGES][MAX_LENGTH+1];
byte next_in=0;
byte next_out=0;
enum pbus_states {PBUS_IDLE=0x80,PBUS_NEED_LEN=0x81,PBUS_NEED_TO=0x82,
                  PBUS_NEED_FROM=0x83,PBUS_NEED_CS=0x84};
byte pbus_state=PBUS_IDLE;
byte checksum;
byte last_byte;

#bit ninth_bit = RS232_ERRORS.7
#bit collision = RS232_ERRORS.6
#bit intf = 11.1

#int_ext
void pbus_isr() {
    byte data;

    if(kbhit()) {

      data=getc();

      if(ninth_bit) {
         switch(pbus_state) {
           case PBUS_IDLE : if(data==0xf1)
                                pbus_state=PBUS_NEED_TO;
                            break;
           case PBUS_NEED_TO :
                            if(data==OUR_ID)
                               pbus_state=PBUS_NEED_FROM;
                            else
                               pbus_state=PBUS_IDLE;
                            checksum=data;
                            break;
           case PBUS_NEED_FROM :
                            pbus_buffer[next_in][0] = data;
                            pbus_state=PBUS_NEED_LEN;
                            checksum^=data;
                            break;
           case PBUS_NEED_LEN :
                            last_byte = data+1;
                            pbus_buffer[next_in][1] = data;
                            pbus_state=2;
                            checksum^=data;
                            break;
         }
      } else
        if(pbus_state==PBUS_NEED_CS) {
           if(checksum==data)
               next_in = (next_in+1) % MAX_MESSAGES;
           pbus_state=PBUS_IDLE;
        } else if(pbus_state<0x80) {
           pbus_buffer[next_in][pbus_state] = data;
           checksum^=data;
           if(++pbus_state>last_byte)
              pbus_state=PBUS_NEED_CS;
      }
    }
}

void pbus_send( byte * message, byte to, byte len) {
   byte checksum,i;

   retry:
      checksum=len^OUR_ID^to;
      disable_interrupts(GLOBAL);
      collision=false;
      ninth_bit=1;

      putc(0xf1);           if(collision) goto error;
      putc(to);             if(collision) goto error;
      putc(OUR_ID);         if(collision) goto error;
      putc(len);            if(collision) goto error;
      ninth_bit=0;
      for(i=1;i<=len;++i) {
        checksum^=*message;
        putc(*(message++)); if(collision) goto error;
      }
      putc(checksum);       if(collision) goto error;
      intf=false;
      enable_interrupts(GLOBAL);
      return;

   error:
      delay_ms(16);
      enable_interrupts(GLOBAL);
      goto retry;
}


#if defined( __PCB__)
#use rs232(baud=9600, xmit=PIN_A3, rcv=PIN_A2)

#elif defined(__PCM__)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCH__)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#endif

#include <input.c>

void main() {
   byte to,len,i,msg[MAX_LENGTH];

   printf("\r\nOur unit ID is %d\r\nPress S to send.\r\n",OUR_ID);

   ext_int_edge( h_to_l );
   enable_interrupts(global);
   enable_interrupts(int_ext);

   do {
      if(next_in!=next_out) {
         printf("\r\nMessage from #%d: ",pbus_buffer[next_out][0]);
         for(i=2;i<=pbus_buffer[next_out][1]+1;++i)
           printf(" %2X",pbus_buffer[next_out][i]);
         next_out=(next_out+1) % MAX_MESSAGES;
      }

      if(kbhit())
        if(toupper(getc())=='S') {
           printf("\r\nSend to: ");
           to=gethex();
           printf("\r\nLength: ");
           len=gethex();
           for(i=0;i<len;++i) {
             printf("\r\nByte %d: ",i+1);
             msg[i]=gethex();
           }
           pbus_send(msg,to,len);
           printf("\r\nSent.");
      }

   } while (TRUE);
}
