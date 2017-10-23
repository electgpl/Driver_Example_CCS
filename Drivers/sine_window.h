#ifndef SINE_WINDOW_H 
#define SINE_WINDOW_H 1

/* sine_window.h
 *
 * Defines up to a 512 point sine_window calculated from the expression:
 * 
 * sine_window[i] = sin(pi*(i+0.5)/256);
 * 
 */

#ifndef FFT_LENGTH
#error FFT LENGTH must be defined to use the sine window!
#endif

#if ((FFT_LENGTH != 2) && (FFT_LENGTH != 4) && (FFT_LENGTH != 8) && (FFT_LENGTH != 16) && (FFT_LENGTH != 32) && (FFT_LENGTH != 64) && (FFT_LENGTH != 128) && (FFT_LENGTH != 256) && (FFT_LENGTH != 512))
#error FFT LENGTH must be a power of two to use the sine window!
#endif

#if (FFT_LENGTH == 2)
#include "sw2.c"
#elif (FFT_LENGTH == 4)
#include "sw4.c"
#elif (FFT_LENGTH == 8)
#include "sw8.c"
#elif (FFT_LENGTH == 16)
#include "sw16.c"
#elif (FFT_LENGTH == 32)
#include "sw32.c"
#elif (FFT_LENGTH == 64)
#include "sw64.c"
#elif (FFT_LENGTH == 128)
#include "sw128.c"
#elif (FFT_LENGTH == 256)
#include "sw256.c"
#elif (FFT_LENGTH == 512)
#include "sw512.c"
#endif

#ENDIF
