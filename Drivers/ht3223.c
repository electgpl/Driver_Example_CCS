//////////////////////////////////////////////////////////////////////////////
////                                                                      ////
////  HT3223.C - Driver for Humirel HT3223/HTF3223 Humidity Module        ////
////                                                                      ////
////  HT3223 has 3 pins: Power, Ground and Output.  The Output pin is     ////
////  a square wave with the frequency denoting the RH% (relative         ////
////  humidty, in percent).                                               ////
////                                                                      ////
////  The math equation is: Fout = 9740 - 18*RH                           ////
////                    or: RH = (9740 - Fout)/18                         ////
////                                                                      ////
//// Connect the HT3223 output pin to the CCP1 pin of the PIC.  This      ////
//// library will configure the CCP1 to act as a capture, and calculate   ////
//// the frequency of Fout based upon the last two captures.              ////
//// NOTE: Some PICs let you change the CCP1 timer to TIMER3.  Do not     ////
////       change this setting, leave the CCP1 timer to TIMER1.           ////
////                                                                      ////
//////////////////////////////////////////////////////////////////////////////
////                                                                      ////
//// HT3223.C Functions:                                                  ////
////                                                                      ////
////  humidity_init() - Must be called before other humidity functions    ////
////                    are called.  Configures the CCP1 and TIMER1, and  ////
////                    enables the CCP1 interrupt.                       ////
////                    NOTE: You must enable global_interrupts() in your ////
////                    application.                                      ////
////                                                                      ////
////  humidity_read() - Returns the RH%, an int8 ranging from 0 to 100.   ////
////                                                                      ////
////  humidity_ready() - The driver keeps the last 15 frequency counts in ////
////             memory.  This function returns TRUE if we have read 15   ////
////             counts.  If it returns FALSE wait a little bit longer.   ////
////                                                                      ////
//////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2005 Custom Computer Services              ////
//// This source code may only be used by licensed users of the CCS C     ////
//// compiler.  This source code may only be distributed to other         ////
//// licensed users of the CCS C compiler.  No other use, reproduction    ////
//// or distribution is permitted without written permission.             ////
//// Derivative programs created using this software in object code       ////
//// form are not restricted in any way.                                  ////
//////////////////////////////////////////////////////////////////////////////

#define HUMIDITY_NUM_SAMPLES   15

int16 humidity_samples[HUMIDITY_NUM_SAMPLES];
int8 humidity_sample_index=0;

int1 __humidity_ready=FALSE;
#define humidity_ready()   __humidity_ready

void humidity_init(void) {
   setup_ccp1(CCP_CAPTURE_FE);    // Configure CCP2 to capture fall
   setup_timer_1(T1_INTERNAL);
   clear_interrupt(INT_CCP1);
   enable_interrupts(INT_CCP1);
}

int8 humidity_read(void) {
   int8 i;
   int8 num=0;
   int32 freq;
   int32 freq_sum=0;
   int16 ticks;

   for (i=0;i<HUMIDITY_NUM_SAMPLES;i++) {
      freq=getenv("CLOCK")/4;
      freq/=humidity_samples[i];
      if ((freq < 9741) && (freq > 7800)) {
         num++;
         freq_sum+=freq;
      }
   }
   
   if (num)
      freq=freq_sum / num;
   else
      freq=9740;
      
   i = ((9740 - freq) / 18);

   return(i);
}

#define TickGetDiff(a, b)       ((int16)(a < b) ? (((int16)0xffff - b) + a + 1) : (a - b))

#int_ccp1
void humidity_isr(void) {
   static int8 first=1;
   static int16 last;

   if (!first) {
      humidity_samples[humidity_sample_index++]=TickGetDiff(CCP_1,last);
      if (humidity_sample_index >= HUMIDITY_NUM_SAMPLES) {
         humidity_sample_index=0;
         __humidity_ready=TRUE;
      }
   }
   else
      first=0;

   last=CCP_1;
}
