/////////////////////////////////////////////////////////////////////////
////                             EX_FFT.C                            ////
////                                                                 ////
////  Perform an FFT on a set of samples, and displays the results.  ////
////                                                                 ////
////  This example will work only with the PCD Compiler.             ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2009 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

// optional
// generally this isn't needed when dealing with only one sinewave.
//#define __DO_WINDOW__

#if !defined(__PCD__)
   #error This example works only for PCD. Please select the PCD Compiler
#endif

#if 0
   //ccs dsp audio development kit rev 1
   #include <33fj128gp706.h>
   #use delay(crystal=12M, clock=80M)

   #use rs232(UART2, baud=9600, errors)
#endif

#if 1
   //ccs dsp audio development kit rev 2
   #include <33EP256MU806.h>
   #use delay(crystal=12MHz, clock=140MHz)

   #pin_select U1TX = PIN_E7
   #pin_select U1RX = PIN_D11
   #use rs232(UART1, baud=9600, errors)
#endif

// bin/slot size result of FFT is SampeRate/FFT_LENGTH.
// must be power of 2.
#define FFT_LENGTH 32

// sine windowing algorithms
#if defined(__DO_WINDOW__)
   #include <dsp_data_util.c>
   #include <sine_window.h>
#endif

// primary FFT algorithms
#include <fft.h>

// populate samples[] for example.
// basically creates a sinewave (cos) of freqency 'f' with amplitude of 'amp'.
// 'dc' is the dc offset of the frequency.
// when you perform fft of this sample set, bin 'f' should be 'amp'/2 and
// bin 'f'+FFT_LEN/2 should also be 'amp'/2.
// DC bin (bin 0) should be 'dc'.
void populate_samples_for_example(unsigned int f, int amp, int dc, Complex *p)
{
   int i;
   float rad;

   printf("\r\nCreating f=%u, amplitude=%d and dc=%d", f, amp, dc);
   
   printf("\r\nSamples in:\r\n");

   for(i=0; i<FFT_LENGTH; i++)
   {
      rad = 2.0*PI*(float)i*(float)f/(float)FFT_LENGTH;
      p->re = (fft_int_t)(cos(rad) * (fft_int_t)amp);
      p->re += dc;
      printf("%ld ", p->re);
      p->im = 0;
      p++;
   }

   printf("\r\n");
}

void main(void)
{
   int i;
   #bankx   //if you want to use the Windowing library, then samples[] must be in BankX
   Complex samples[FFT_LENGTH];
   Complex * fft_result, *p;

   printf("\r\n\nCCS ex_fft.c example\r\n");

   populate_samples_for_example(2, 100, 100, samples);

   fft_init(FFT_LENGTH);

  #if defined(__DO_WINDOW__)
   windowwc(sine_window, samples, samples, FFT_LENGTH);
  #endif

   fft_result = fft(samples, FFT_LENGTH);

   printf("FFT Result:\r\n");

   p = fft_result;
   for(i=0; i<FFT_LENGTH; i++)
   {
      printf("%u: %u (%d, %d)\r\n", i, cplx_magnitude(p), p->re, p->im);
      p++;
   }

   while(1);
}

