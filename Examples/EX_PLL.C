/////////////////////////////////////////////////////////////////////////
////                             EX_PLL.C                            ////
////                                                                 ////
////  This is an example program showing how to use the LMX2326      ////
////  chip.  The program configures the chip and then prompts        ////
////  the user for a channel number.  When the channel number is     ////
////  entered, the pll tunes to the desired frequency.               ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////                                                                 ////
////     LMX2326     Protoboard                                      ////
////         9           Digital GND                                 ////
////         10          pin C3                                      ////
////         11          pin C2                                      ////
////         12          pin C1                                      ////
////         13          pin C0                                      ////
////         15          +5V                                         ////
////                                                                 ////
////     LMX2326     Other Connections                               ////
////         1               Fastlock Output                         ////
////         2               Charge Pump Output                      ////
////         3               Charge Pump GND                         ////
////         4               Analog GND                              ////
////         5               RF Prescaler Complementary Input        ////
////         6               RF Prescaler Input                      ////
////         7               Analog Power Supply Voltage Input       ////
////         8               Oscillator Input                        ////
////         14              FoLD                                    ////
////         16              Power Supply for Charge Pump            ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////                                                                 ////
////  This example will work with the PCM and PCH compilers.  The    ////
////  following conditional compilation lines are used to include a  ////
////  valid device for each compiler.  Change the device, clock and  ////
////  RS232 pins for your hardware if needed.                        ////
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
#include <16F877.H>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCH__)
#include <18F452.H>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#endif

#include <stdlib.h>
#include <input.c>
#include <LMX2326.c>

// CONSTANTS
#define  REFERENCE_FREQUENCY  19440    // in kHz
#define  CHANNEL_1_FREQUENCY  1012000  // in kHz
#define  CHANNEL_SPACING      240      // in kHz


void main()
{
   int16 chan_num;

   pll_select_chip();

   pll_init(POWERDOWN_CS_ONLY|POSITIVE_PHASE_DETECTOR|FoLD_TRI_STATE|LOCK_DETECT_DISABLE,
      REFERENCE_FREQUENCY, CHANNEL_SPACING, CHANNEL_1_FREQUENCY);

   printf("Channel 1 frequency is 1012MHz and the channel spacing is 240kHz");

   while(TRUE)
   {
      printf("\n\nHit ENTER to quit or channel number to tune to:  ");
      chan_num = get_long();
      if(0 == chan_num)
         break;
      else
         pll_set_channel(chan_num);
   }

   pll_deselect_chip();
}
