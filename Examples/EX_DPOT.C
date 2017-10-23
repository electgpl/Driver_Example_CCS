/////////////////////////////////////////////////////////////////////////
////                          EX_DPOT.C                              ////
////                                                                 ////
////  This example demonstrates how to use a digital pot.  The       ////
////  program will prompt for the pot settings, set the pot and show ////
////  the results using an analog to digital converter.              ////
////                                                                 ////
////  If you're using the AD8400 chip, the program will not prompt   ////
////  you to select a pot, since the chip only has one.              ////
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
#fuses HS,NOWDT, NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#endif

#include <input.c>
#include <ad8400.c>


void main() {
   int   value;
   char  pot_num;

   setup_adc_ports (ALL_ANALOG);
   setup_adc (ADC_CLOCK_INTERNAL);
   init_pots ();

   do {
      set_adc_channel (0);
      delay_us (100);
      value = read_adc ();
      printf ("\n\r\n\rInput 1: %2X  ", value);

      if (NUM_POTS > 1) {
         set_adc_channel (1);
         delay_us (100);
         value = read_adc ();
         printf ("Input 2: %2X  \n\r", value);

         do {
            printf ("\n\rChange pot 1 or 2?");
            pot_num = getc ();
         } while ((pot_num!='1') && (pot_num!='2'));
         putc (pot_num);
      }
      else
         pot_num = '1';

      printf ("\n\rNew pot pot_value:");
      value = gethex ();

      set_pot ((pot_num-'1'), value);
   } while (TRUE);
}
