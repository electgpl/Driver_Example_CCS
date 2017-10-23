/////////////////////////////////////////////////////////////////////////
////                           EX_LED.C                              ////
////                                                                 ////
////  This program shows how to drive a two digit LED using input    ////
////  from an RS-232 port.                                           ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////     LED seg f    Pin B0                                         ////
////     LED seg a    Pin B1                                         ////
////     LED seg e    Pin B2                                         ////
////     LED seg c    Pin B3                                         ////
////     LED seg dp   Pin B4                                         ////
////     LED seg d    Pin B5                                         ////
////     LED seg b    Pin B6                                         ////
////     LED seg g    Pin B7                                         ////
////     LED Anode 1  Pin A0                                         ////
////     LED Anode 2  Pin A1                                         ////
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
#use rs232(baud=9600, xmit=PIN_A3, rcv=PIN_A2)

#elif defined(__PCM__)
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


BYTE CONST LED_MAP[10] = {0x90,0xb7,0x19,0x15,0x36,0x54,0x50,0xb5,0,0x24};


void wait() {           // This function waits for either ~2ms or until a
  int countdown;        // event happens (in this case a rs232 character)

  countdown=200;
  while((--countdown!=0)&&!kbhit())
    delay_us(10);
}

void display_segs(char c) {
  if((c>'9')||(C<'0'))
    output_b(0xff);
  else
    output_b(LED_MAP[c-'0']);
}


void display( char one, char two) {
   display_segs(one);
   output_high(PIN_A0);
   wait();
   output_low(PIN_A0);
   display_segs(two);
   output_high(PIN_A1);
   wait();
   output_low(PIN_A1);
}


void main() {
  char pos1, pos2;

  set_tris_b(0);
  output_b(0);

  pos1='0';
  pos2='1';

  while(TRUE) {
    display(pos1,pos2);
    if(kbhit()) {
       pos1=pos2;
       pos2=getc();
    }
  }

}
