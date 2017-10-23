/////////////////////////////////////////////////////////////////////////
////                          EX_FFT_FILTER.c                        ////
////                                                                 ////
////  This program demonstrates use of the FFT filter library.       ////
////  Audio PCM data is transfered from the TLV320AIC23B codec to    ////
////  dsPIC, the data is converted into the frequency domain, and    ////
////  this data is passed to the user for manipulation.  After the   ////
////  user has finished the frequency data manipulation, the altered ////
////  frequency data is converted back to the time-domain and output ////
////  to the codec.  The following functions are implemented :       ////
////                                                                 ////
//// void codec_init(void) : Initializes the codec for automatic     ////
////                          data transfers.                        ////
////                                                                 ////
//// void FFT_filter_hook(Complex * freq_data) : Manipulates the     ////
////                          instantaneous frequency data of the    ////
////                          signal.                                ////
////                                                                 ////
//// This example is only supported for dsPIC controllers with a DSP ////
//// core processor with the PCD C Compiler.  This program will work ////
//// on a DSP Analog Development Kit without modification.           ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996-2010 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#include <33fj128gp706.h>
#fuses HS, PR_PLL, NOWDT   
#use delay(crystal=12M, clock=80M) //40 MIPS

/* Define the FFT length and include the FFT_filter library */
#define FFT_LENGTH 256 
#include "FFT_filter.c"

/* Include the appropriate codec driver file */
#include "TLV320AIC23B.c"

void main()
{
   /* Initialize the FFT Framework */  
   FFT_filter_init();
   
   while(TRUE)
   {
      /* Run the FFT filtering task in an infinite loop */
      fft_filter_task();
   }
}

/* Implementation of void codec_init(void) 
 *
 * Initialize the TLV320AIC23B codec for line-input, headphone-output, 8 kHz
 * sampling frequency.  Set up the DCI for mono-channel reception/transmission
 * of audio PCM data in MULTICHANNEL (DSP) format.
 **/
void codec_init()
{
   /* Deselect the codec. */
   output_high(CODEC_CS);
   
   /* Setup the DCI interface. */
   setup_dci((MULTICHANNEL_MODE | UNDERFLOW_LAST
               | DCI_SLAVE | DCI_CLOCK_INPUT | SAMPLE_RISING_EDGE),
               DCI_1WORD_FRAME | DCI_16BIT_WORD, RECEIVE_SLOT0, TRANSMIT_SLOT0, 0);
   
   dci_start();
   
   /* Setup the codec. */
   codec_initialize();
   codec_setup_analog_path(DAC_SELECT);
   codec_setup_line_input(LINE_IN_NO_GAIN_VOLUME, LINE_IN_NO_GAIN_VOLUME);
   codec_setup_hp_output(HP_NO_GAIN-0x27, HP_NO_GAIN-0x27);
   
   codec_write_register(INTERFACE_ACTIVATION, DEACTIVATE);
   codec_setup_sample_rate(USB_8KHZ); 
   codec_enable_interface();
}

/* Implementation of void FFT_filter_hook(Complex * freq_data) 
 * 
 * Simple Low-Pass filter, all frequencies above (8000/256 * 25) = 781 Hz
 * are set to zero.
 **/
void FFT_filter_hook(Complex * freq_data)
{
   unsigned int16 i;
   
   for ( i = 25; i < (FFT_LENGTH - 25); i++ )
   {
      freq_data[i].re = freq_data[i].im = 0;
   }
}
