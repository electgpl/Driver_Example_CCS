#ifndef FFT_filter
#define FFT_filter 1
////////////////////////////////////////////////////////////////////////////
////                             FFT_filter.c                           ////
////                                                                    ////
//// This file is a library which allows users to view, edit, and save  ////
//// frequency-domain data generated from Short-time Fourier Transforms ////
//// (STFT).  In a continuous format, this library uses a user-defined  ////
//// codec to sample data at a specified rate (without overhead from    ////
//// the CPU), take the STFT of the sample, allow the user the alter    ////
//// the data, then perform the inverse STFT and output the sound data  ////
//// back to the codec.                                                 ////
////                                                                    ////
//// In order to determine the size of the STFT to be performed, the    ////
//// user must define the quantity "FFT_LENGTH" to be a power-of-two    ////
//// that is 512 or less.                                               ////
////                                                                    ////
//// Before this library can be used, the user must implement the       ////
//// following two functions :                                          ////
////                                                                    ////
//// void codec_init(void) : Initializes the codec for automatic data   ////
////                          transfers.                                ////
////                                                                    ////
//// void FFT_filter_hook(Complex * freq_data) : Manipulate the         ////
////                          instantaneous frequency data of the       ////
////                          signal.                                   ////
////                                                                    ////
//// The codec_init() function should initialize both the codec         ////
//// hardware and the DCI peripheral to automatically transfer data.    ////
////                                                                    ////
//// There are two API functions for this library :                     ////
////                                                                    ////
//// void FFT_filter_init(void) : Should be caled during initialization ////
////                                                                    ////
//// void FFT_filter_task(void) : Should be called every main loop.     ////
////                                                                    ////
//// All of the frequency data manipulation should be handled in the    ////
//// user-implemented FFT_filter_hook() function.  The library assures  ////
//// smooth transitions between STFT frames by utilizing a windowed     ////
//// overlap-add-by-two method for data reconstruction.                 ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996-2010 Custom Computer Services            ////
//// This source code may only be used by licensed users of the CCS     ////
//// C compiler.  This source code may only be distributed to other     ////
//// licensed users of the CCS C compiler.  No other use,               ////
//// reproduction or distribution is permitted without written          ////
//// permission.  Derivative programs created using this software       ////
//// in object code form are not restricted in any way.                 ////
////////////////////////////////////////////////////////////////////////////
#nolist

#ifndef FFT_LENGTH
#error FFT_LENGTH must be defined in order to use filter library!
#endif

#if (FFT_LENGTH > 512)
#error FFT LENGTH must be 512 or less in order to use filter library!
#endif

#if ((FFT_LENGTH != 2) && (FFT_LENGTH != 4) && (FFT_LENGTH != 8) && (FFT_LENGTH != 16) && (FFT_LENGTH != 32) && (FFT_LENGTH != 64) && (FFT_LENGTH != 128) && (FFT_LENGTH != 256) && (FFT_LENGTH != 512))
#error FFT LENGTH must be a power of two!
#endif

#ifdef DMA_LENGTH
#warning DMA LENGTH being redefined to (FFT LENGTH / 2)
#undef DMA_LENGTH
#endif

#define DMA_LENGTH (FFT_LENGTH / 2)

/* DMA Buffers & Pointers */
#bank_dma
signed int16 TxBufferA[DMA_LENGTH];
#bank_dma
signed int16 TxBufferB[DMA_LENGTH];
#bank_dma
signed int16 RxBufferA[DMA_LENGTH];
#bank_dma
signed int16 RxBufferB[DMA_LENGTH];

signed int16* TxBuffer;
signed int16* RxBuffer;

/* DMA State Machine */
int1 dma_switch = 0;
int1 dma_data_full = 0;

#include "fft.h"
#include "sine_window.h"
#include "dsp_data_util.c"

/* The following two functions here are prototyped but not implemented in
 * this file.  You must implement these two functions yourself.  
 *
 * The first prototype (FFT_filter_hook) allows you to alter the FFT data of the
 * latest data window prior to the data being converted back into the 
 * time-domain and output to the codec in block format. If you do not wish to
 * alter the data, simply implement an empty function.  This function could also
 * be used to store or display the FFT data graphically to an LCD.
 *
 * The second prototype (codec_init) should initialize both the codec you are
 * using as well as the DCI peripheral to automatically transfer mono-channel
 * data. Since requirements vary from codec to codec, this implementation is 
 * left to the user to complete.  To set up the TLV320AIC23B on the DSP Analog
 * Kit for mono-channel mode, use the following line:
 *
 * setup_dci((MULTICHANNEL_MODE | UNDERFLOW_LAST | DCI_SLAVE | DCI_CLOCK_INPUT | SAMPLE_RISING_EDGE), DCI_1WORD_FRAME | DCI_16BIT_WORD, RECEIVE_SLOT0, TRANSMIT_SLOT0, 0);
 *
 **/
void FFT_filter_hook(Complex * freq_data);
void codec_init(void);

#int_dma1
void dma_isr()
{     
   /* Change the state machine. */
   dma_switch = !dma_switch;
   dma_data_full = 1;
   
   /* Setup the buffer pointers */
   if(dma_switch)
   {
      TxBuffer = TxBufferA;
      RxBuffer = RxBufferA;
   }
   else
   {
      TxBuffer = TxBufferB;
      RxBuffer = RxBufferB;
   }  
}

void FFT_filter_init()
{
   /* Setup DMA */
   setup_dma(1,DMA_OUT_DCI, DMA_WORD);
   setup_dma(2,DMA_IN_DCI, DMA_WORD);
   enable_interrupts(INT_DMA1);  
   enable_interrupts(INTR_GLOBAL); 
   dma_start(1, (DMA_CONTINOUS | DMA_PING_PONG), TxBufferA, TxBufferB, DMA_LENGTH - 1);
   dma_start(2, (DMA_CONTINOUS | DMA_PING_PONG), RxBufferA, RxBufferB, DMA_LENGTH - 1);
   
   /* Start the codec. */
   codec_init();   
      
   /* Initialize the FFT. */
   fft_init(FFT_LENGTH);
}

void fft_filter_task(void)
{
   #bankx
   static Complex samples[FFT_LENGTH];
   static signed int16 last_in[DMA_LENGTH];
   static signed int16 next_out[DMA_LENGTH];
   Complex * fft_result;

   if(dma_data_full)
   {  
      /* Setup the DSP core for signed, fractional operation. */
      US = 0;     //signed
      IF_EN = 0;  //fractional
      RND = 1;    //convergent rounding
      
      /* Concatenate current sample with previous sample */
      move_bufferwc(last_in, &samples, DMA_LENGTH);
      move_bufferwc(RxBuffer, &samples[DMA_LENGTH].re, DMA_LENGTH);

      /* Window the real part of the concatenated samples */
      windowwc(sine_window, samples, samples, FFT_LENGTH);

      /* FFT the windowed sample */
      fft_result = fft(samples, FFT_LENGTH);
      
      /* Filter the FFT result */
      move_buffercc(fft_result, samples, FFT_LENGTH);
      FFT_filter_hook(samples);
      
      /* IFFT the filtered spectrum */
      fft_result = ifft(samples, FFT_LENGTH);

      /* Window the real part of the filtered samples */
      move_buffercc(fft_result, samples, FFT_LENGTH);
      windowwc(sine_window, samples, samples, FFT_LENGTH);
      
      /* Overlap-add-by-2 the filtered samples */
      add_bufferscw(&samples, next_out, TxBuffer, DMA_LENGTH );
      
      /* Store extended part of filtered samples for next loop */
      move_buffercw(&samples[DMA_LENGTH].re, next_out, DMA_LENGTH);
      move_bufferww(RxBuffer, last_in, DMA_LENGTH);
       
      /* Signal DMA State Machine that processing is finished */
      dma_data_full = 0;//done processing
   }
}

#list
#endif
