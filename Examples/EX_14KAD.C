/////////////////////////////////////////////////////////////////////////
////                          EX_14KAD.C                             ////
////                                                                 ////
////  This program displays the filtered and calibrated A/D samples  ////
////  from any analog input pin (chosen by user) over the RS-232     ////
////  interface.  The process is repeated forever.                   ////
////                                                                 ////
////                                                                 ////
////  Configure the CCS prototype board as follows:                  ////
////                                                                 ////
////     Connect 0.033 uF capacitor between pin B0 and GND.          ////
////     Use a 10K POT to vary the voltage.                          ////
////                                                                 ////
////     For A/D sampling, insert a jumper from the output of the    ////
////     POT to:                                                     ////
////               pin A0       for        RA0/AN0                   ////
////               pin A1                  RA1/AN1                   ////
////               pin A2                  RA2/AN2                   ////
////               pin A3                  RA3/AN3                   ////
////               pin C0                  RC0/REFA                  ////
////               pin B6                  RD3/REFB                  ////
////               pin D4                  RD4/AN4                   ////
////               pin D5                  RD5/AN5                   ////
////               pin D6                  RD6/AN6                   ////
////               pin D7                  RD7/AN7                   ////
////                                                                 ////
////  Jumpers: pin C7 to RS232 RX, pin C6 to RS232 TX                ////
////                                                                 ////
////  This example will work with the PCM compiler.  The following   ////
////  conditional compilation lines are used to include a valid      ////
////  device for the compiler.  Change the device, clock and RS232   ////
////  pins for your hardware if needed.                              ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#if defined(__PCM__)
#include <14000.h>
#fuses HS,NOWDT,NOPROTECT
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
//#include <cal9717n.h>  // Put your calibration constants here
#include <14kcal.c>
#endif


void main() {

   int source;
   float res;

   setup_adc_ports(ALL_ANALOG);
   setup_adc(CURRENT_34);

   delay_ms(2000);

   do {
      printf("\r\nChoose analog input channel:\r\n");
      printf("\r\n0) RA0/AN0  1) RA1/AN1   2) RA2/AN2  3) RA3/AN3");
      printf("\r\n7) IntTemp  8) RC0/REFA  9) RD3/REFB");
      printf("\r\nA) RD4/AN4  B) RD5/AN5   C) RD6/AN6  D) RD7/AN7");

      source = toupper(getc());
      if (source >= 'A')
         source = source - 'A' + 10;
      else
         source = source - '0';

      res = READ_CALIBRATED_AD(source);
      printf("\r\n\nCalibrated A/D = %E\n", res);

   } while(TRUE);
}
