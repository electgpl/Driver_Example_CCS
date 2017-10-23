/////////////////////////////////////////////////////////////////////////
////                          EX_TONES.C                             ////
////                                                                 ////
////  This example plays the song "Happy Birthday."                  ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////     Connect the positive wire of the speaker to pin B0          ////
////     Connect the negative wire of the speaker to Gnd             ////
////                                                                 ////
////  This example will work with the PCM and PCH compilers.  The    ////
////  following conditional compilation lines are used to include a  ////
////  valid device for each compiler.  Change the device and clock   ////
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
#include <16F877.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)

#elif defined(__PCH__)
#include <18F458.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#endif

#include <tones.c>

#define SIZE 25

const struct note
{
   long tone;
   long length;
} happy_bday[SIZE] = {
C_note[0],350, C_note[0],100, D_note[0],500, C_note[0],500, F_note[0],500, E_note[0],900,
C_note[0],350, C_note[0],100, D_note[0],500, C_note[0],500, G_note[0],500, F_note[0],900,
C_note[0],350, C_note[0],100, C_note[1],500, A_note[0],500, F_note[0],500, E_note[0],500, D_note[0],900,
Bb_note[0],350, Bb_note[0],100, A_note[0],500, F_note[0],500, G_note[0],500, F_note[0],1200};


void main(void)  {
   int i;

   while(TRUE)
   {
      for(i=0; i<SIZE; ++i)
      {
         generate_tone(happy_bday[i].tone,happy_bday[i].length);
         delay_ms(75);
      }
   }
}
