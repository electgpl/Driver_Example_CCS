/////////////////////////////////////////////////////////////////////////
////                        EX_STEP.C                                ////
////                                                                 ////
////  This program interfaces to a stepper motor.  The program will  ////
////  use the RS-232 interface to either control the motor with a    ////
////  analog input, a switch input or by RS-232 command.             ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////     Connect stepper motor to pins B0-B3                         ////
////     Conenct pin B7 to pushbutton switch                         ////
////     Connect output of POT to pin A0                             ////
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
#include <18F452.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#endif

#include <input.c>

#BYTE port_b = 6

#define FOUR_PHASE TRUE

#ifdef FOUR_PHASE

BYTE const POSITIONS[4] = {0b0101,
                           0b1001,
                           0b1010,
                           0b0110};
#else
BYTE const POSITIONS[8] = {0b0101,
                           0b0001,
                           0b1001,
                           0b1000,
                           0b1010,
                           0b0010,
                           0b0110,
                           0b0100};
#endif


drive_stepper(BYTE speed, char dir, BYTE steps) {
   static BYTE stepper_state = 0;
   BYTE i;

   for(i=0; i<steps; ++i) {
     delay_ms(speed);
     set_tris_b(0xf0);
     port_b = POSITIONS[ stepper_state ];
     if(dir!='R')
       stepper_state=(stepper_state+1)&(sizeof(POSITIONS)-1);
     else
       stepper_state=(stepper_state-1)&(sizeof(POSITIONS)-1);
   }
}

use_pot() {
  BYTE value;

  setup_adc(adc_clock_internal);
  set_adc_channel( 1 );
  printf("\r\n");

  while( TRUE ) {
    value=read_adc();
    printf("%2X\r",value);
    if(value<0x80)
       drive_stepper(value,'R',8);
    else if(value>0x80)
       drive_stepper(128-(value-128),'F',8);
  }

}


use_switch(BYTE speed, char dir) {

   BYTE steps;

   printf("\n\rSteps per press: ");
   steps = gethex();

   while(true) {
      while(input(PIN_B7)) ;
      drive_stepper(speed,dir,steps);
      while(!input(PIN_B7)) ;
      delay_ms(100);
   }
}


main() {

   BYTE speed, steps;
   char dir;

   setup_port_a(RA0_RA1_RA3_ANALOG);

   while (TRUE) {
       printf("\n\rSpeed (hex): ");
       speed = gethex();

       if(speed==0)
          use_pot();

       printf("\n\rDirection (F,R): ");
       dir=getc()|0x20;
       putc(dir);

       printf("\n\rSteps (hex): ");
       steps = gethex();

       if(steps==0)
          use_switch(speed,dir);

       drive_stepper(speed,dir,steps);
   }

}
