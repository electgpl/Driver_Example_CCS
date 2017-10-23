#ifndef DSP_DATA_UTIL
#define DSP_DATA_UTIL 1
////////////////////////////////////////////////////////////////////////////
////                              DATA_UTIL.C                           ////
////                                                                    ////
//// This file contains helper functions for the efficient execution    ////
//// of common data structure manipulations in DSP.  The following      ////
//// functions are supported:                                           ////
////                                                                    ////
//// void move_buffer(source, destination, size)                        ////
////     -copies the contents of source buffer to destination buffer    ////
////                                                                    ////
//// void vector_multiply(y_buffer, x_buffer, destination, size)        ////
////     -multiplies two buffers itemwise                               ////
////                                                                    ////
//// void x_scalar_multiply(buffer, scalar, destination, size)          ////
////     -multiplies each item of a buffer in X_RAM by a scalar         ////
////                                                                    ////
//// void y_scalar_multiply(buffer, scalar, destination, size)          ////
////     -multiplies each item of a buffer in Y_RAM by a scalar         ////
////                                                                    ////
//// void clear_buffer(buffer, size)                                    ////
////     -clears contents of a buffer                                   ////
////                                                                    ////
//// void add_buffers(buffer1, buffer2, destination, size)              ////
////     -adds contents of two buffers                                  ////
////                                                                    ////
//// void subtract_buffers(buffer1, buffer2, destination, size)         ////
////     -subtracts buffer2 from buffer1                                ////
////                                                                    ////
//// void window(y_buffer, x_buffer, destination, size)                 ////
////     -Windows the real part of Complex x_buffer by sint16 y_buffer  ////
////                                                                    ////
//// void left_shift_buffer(buffer, shifts, buffer_size)                ////
////     -shifts buffer left the specified number of times              ////
////                                                                    ////
//// void right_shift_buffer(buffer, shifts, buffer_size)               ////
////     -shifts buffer right the specified number of times             ////
////                                                                    ////
//// There are overloaded versions of each of these functions that      ////
//// support most combinations of signed int16 and complex buffers.     ////
//// For all of the funtions presented, it is assumed that all buffers  ////
//// are stored in signed, fixed-point, fractional format, and that the ////
//// DSP core on the PIC has already been initialized for fixed-point,  ////
//// fractional math.                                                   ////
////////////////////////////////////////////////////////////////////////////

/* Represent a complex number with both a real and imaginary part. */
#ifndef _complexnum
#define _complexnum
typedef struct _complex
{
   signed int16 re;
   signed int16 im;
} Complex;
#endif

// void move_buffer(source, destination, size)
//
// This function copies data from a signed int16 source buffer to a destination
// buffer.  The inputs to the function are pointers to the source and
// destination, along with a parameter for many signed int16's to move.  This
// function could also be implemented with a loop in C, but the REPEAT
// DSP instruction makes this an efficient implementation
//
void move_bufferww(signed int16* source, signed int16* destination, unsigned int16 size)
{
   #asm
   mov   source, w3        // w3 -> source[0]
   mov   destination, w4   // w4 -> dest[0]
   mov   size, w5          // w5 = size
   dec   w5, w5            // w5 = size - 1
   do    w5, mbww          // do next instruction "size" times
   mov   [w3++], w5        // dest[i] = source[i]
   mbww: mov   w5, [w4++]
   #endasm
}

// void move_buffer(source, destination, size)
//
// This function moves data from a complex source buffer to a destination
// buffer.  The inputs to the function are pointers to the source and
// destination, along with a parameter for many complex values to move.  This
// function could also be implemented with a loop in C, but the REPEAT
// DSP instruction makes this an efficient implementation
//
void move_buffercc(Complex* source, Complex* destination, unsigned int16 size)
{
   /*
   #asm
   mov   source, w3        // w3 -> source[0].re
   mov   destination, w4   // w4 -> dest[0].re
   mov   size, w5          // w5 = size
   sl    w5, w5            // w5 = 2*size (Each complex number is 2 words)
   dec   w5, w5            // w5 = 2*size - 1
   repeat   w5             // do next instruction "2*size" times
   mov   [w3++], [w4++]    // dest[i] = source[i]
   #endasm
   */
   memcpy(destination, source, size*sizeof(Complex));
}

// void move_buffer(source, destination, size)
//
// This function moves data from a signed int16 source buffer to a destination
// buffer.  The inputs to the function are pointers to the source and
// destination, along with a parameter for many int16 values to move.  Since
// the destination and source are not the same size, it is assumed that the
// imaginary part of the complex number should be zero.  This functions could
// also be implemented with a loop in C, but the REPEAT DSP instruction makes 
// this an efficient implementation.
//
void move_bufferwc(signed int16* source, Complex* destination, unsigned int16 size)
{
   #asm
   mov   source, w3        // w3 -> source[0]
   mov   destination, w4   // w4 -> dest[0].re
   mov   size, w5          // w5 = size
   dec   w5, w5            // w5 = size-1
   do    w5, mvbufwc       // do the following loop "size" times
   mov   [w3++], [w4++]    // dest[i].re = source[i]
   mvbufwc: clr   [w4++]   // dest[i].im = 0 
   #endasm
}

// void move_buffer(source, destination, size)
//
// This function moves data from a complex source buffer to a destination
// buffer.  The inputs to the function are pointers to the source and
// destination, along with a parameter for many int16 values to move.  Since
// the destination and source are not the same size, it is assumed that the
// imaginary part of the complex number should be ignored. This functions could
// also be implemented with a loop in C, but the REPEAT DSP instruction makes 
// this an efficient implementation.
//
void move_buffercw(Complex* source, signed int16* destination, unsigned int16 size)
{
   #asm
   mov   source, w3        // w3 -> source[0].re
   mov   destination, w4   // w4 -> dest[0]
   mov   size, w5          // w5 = size
   dec   w5, w5            // w5 = size-1
   do    w5, mvbufcw       // do the following loop "size" times
   mov   [w3++], [w4++]    // dest[i] = source[i].re
   mvbufcw: inc2  w3, w3   // w3++ (i++)
   #endasm
}

// void vector_multiply(y_buffer, x_buffer, destination, size)
//
// This function multiplies the contents of two complex buffers item-wise and
// stores the result in the complex buffer dest.  The first buffer parameter 
// must located in Y_RAM, and the second parameter must be located in X_RAM. 
// The destination buffer can be anywhere in RAM and can be the X or Y input
// buffer.  The size parameter is the number of complex multiplications to be
// done.  The complex multiplication is carried out according to the following:
//
// (a + b*i) * (c + d*i) = (a*c - b*d) + (a*d + b*c)*i
//


void vector_multiplycc(Complex* y_buffer, Complex* x_buffer, Complex* destination, unsigned int16 size)
{
   #asm asis
   mov   x_buffer, w8
   mov   y_buffer, w10
   mov   destination, w13
   mov   size, w2
   dec   w2, w2
   clr   B, [w8]+=2, w4, [w10]+=2, w5     //w4 = x_data.re, w5 = y_data.re
   clr   A, [w8]+=2, w6, [w10]+=2, w7     //w6 = x_data.im, w7 = y_data.im
   do    w2, vmc
   //A contains the real result, B contains imaginary result
   mpy   w4*w5, A
   msc   w6*w7, A
   mpy   w4*w7, B
   mac   w5*w6, B, [w8]+=2, w4, [w10]+=2, w5, [w13]+=2
   vmc:   clr   A, [w8]+=2, w6, [w10]+=2, w7, [w13]+=2
   #endasm
}

// void vector_multiply(y_buffer, x_buffer, destination, size)
//
// This function multiplies the contents of two int16 buffers item-wise and
// stores the result in the int16 destination buffer.  The first buffer  
// must located in Y_RAM, and the second buffer must be located in X_RAM. 
// The destination buffer can be anywhere in RAM and can be the X or Y input
// buffer.  The size parameter is the number of int16 multiplications to be
// done.  
//
void vector_multiplyww(signed int16* y_buffer, signed int16* x_buffer, signed int16* destination, unsigned int16 size)
{
   #asm
   mov   x_buffer, w8
   mov   y_buffer, w10
   mov   destination, w13
   mov   size, w2
   dec   w2, w2
   clr   A, [w8]+=2, w4, [w10]+=2, w5
   do    w2, vmw
   mpy   w4*w5, A, [w8]+=2, w4, [w10]+=2, w5
   vmw: sac.r  A, [w13++]
   #endasm
}

// void vector_conjugate_multiply(y_buffer, x_buffer, destination, size)
//
// Same as the complex vector_multiply() function except that the Y_RAM input  
// is conjugated prior to multiplication, as shown below:
//
// conj(a + b*i) = (a - b*i)
//
void vector_conjugate_multiplycc(Complex* y_buffer_conjugate, Complex* x_buffer, Complex* destination, unsigned int16 size)
{
   #asm
   mov   x_buffer, w8
   mov   y_buffer_conjugate, w10
   mov   destination, W13
   mov   size, w2
   dec   w2, w2
   clr   B, [w8]+=2, w4, [w10]+=2, w5
   clr   A, [w8]+=2, w6, [w10]+=2, w7
   do    w2, vcm
   //A contains the real result,B contains imaginary result
   mpy   w4*w6, A
   mac   w6*w7, A
   mpy   w5*w6, B
   msc   w4*w7, B, [w8]+=2, w4, [w10]+=2, w5, [w13]+=2
   vcm:  clr    A, [w8]+=2, w6, [w10]+=2, w7, [w13]+=2
   #endasm
}


// void clear_buffer(buffer, size)
//
// Clears the contents of an int16 buffer.  The first parameter is a pointer
// to the start of the buffer, and the second parameter is the size of the
// buffer (in int16 units)
//
void clear_bufferw(signed int16* buffer, unsigned int16 size)
{
   #asm
   mov   buffer, w3        // w3 -> buffer[0]
   mov   size, w5          // w5 = size
   dec   w5, w5            // w5 = size-1
   repeat   w5             // do next instruction "size" times
   clr   [w3++]            // buffer[i] = 0
   #endasm
}

// void clear_buffer(buffer, size)
//
// Clears the contents of an complex buffer.  The first parameter is a pointer
// to the start of the buffer, and the second parameter is the size of the
// buffer (in Complex units)
//
void clear_bufferc(Complex * buffer, unsigned int16 size)
{
   #asm
   mov   buffer, w3        // w3 -> buffer[0].re
   mov   size, w5          // w5 = size
   sl    w5, w5            // w5 = 2*size
   dec   w5, w5            // w5 = 2*size - 1
   repeat w5               // do next instruction "size" times
   clr   [w3++]            // buffer[i] = 0;
   #endasm
}

// void x_scalar_multiply(buffer, scalar, destination, size)
//
// Multiplies each item in a complex buffer by sint16 scalar and stores the
// result in the destination buffer.  The input buffer must be located in X_RAM,
// but the scalar and destination buffer can be anywhere in RAM.  Write-back
// to the input buffer is supported.  The size parameter gives the length of
// the input and output buffers.
//
void x_scalar_multiplyc(Complex * buffer, signed int16 scalar, Complex * destination, unsigned int16 size)
{
   #asm
   mov   buffer, w8              // w8 -> buffer[0].re
   mov   scalar, w5              // w5 = scalar
   mov   destination, w13        // w13 -> destination[0].re
   mov   size, w2                // w2 = size
   sl    w2, w2                  // w2 = 2*size
   dec   w2, w2                  // w2 = 2*size-1
   clr   A, [w8]+=2, w4          // w4 = buffer[0].re, w8++
   do    w2, smc                 // loop "2*size" times
   mpy   w4*w5, A, [w8]+=2, w4   // A = buffer[i]*scalar, w4 = buffer[i+1], w8++
   smc:  sac.r A, [w13++]        // destination[i] = A, w13++
   #endasm
}

// void x_scalar_multiply(buffer, scalar, destination, size)
//
// Multiplies each item in a sint16 buffer by sint16 scalar and stores the
// result in the destination buffer.  The input buffer must be located in X_RAM,
// but the scalar and destination buffer can be anywhere in RAM.  Write-back
// to the input buffer is supported.  The size parameter gives the length of
// the input and output buffers.
//
void x_scalar_multiplyw(signed int16* buffer, signed int16 scalar, signed int16* destination, unsigned int16 size)
{
   #asm
   mov   buffer, w8              // w8 -> buffer[0]
   mov   scalar, w5              // w5 = scalar
   mov   destination, w13        // w13 -> destination[0]
   mov   size, w2                // w2 = size
   dec   w2, w2                  // w2 = size-1
   clr   A, [w8]+=2, w4          // w4 = buffer[0], w8++
   do    w2, smc                 // loop "size" times
   mpy   w4*w5, A, [w8]+=2, w4   // A = buffer[i]*scalar, w4 = buffer[i+1], w8++
   smc:  sac.r A, [w13++]        // destination[i] = A, w13++
   #endasm
}

// void y_scalar_multiply(buffer, scalar, destination, size)
//
// Multiplies each item in a complex buffer by sint16 scalar and stores the
// result in the destination buffer.  The input buffer must be located in Y_RAM,
// but the scalar and destination buffer can be anywhere in RAM.  Write-back
// to the input buffer is supported.  The size parameter gives the length of
// the input and output buffers.
//
void y_scalar_multiplyc(Complex * buffer, signed int16 scalar, Complex * destination, unsigned int16 size)
{
   #asm
   mov   buffer, w10             // w10 -> buffer[0].re
   mov   scalar, w5              // w5 = scalar
   mov   destination, w13        // w13 -> destination[0].re
   mov   size, w2                // w2 = size
   sl    w2, w2                  // w2 = 2*size
   dec   w2, w2                  // w2 = 2*size-1
   clr   A, [w10]+=2, w4         // w4 = buffer[0].re, w10++
   do    w2, smc                 // loop "2*size" times
   mpy   w4*w5, A, [w10]+=2, w4  // A = buffer[i]*scalar, w4 = buffer[i+1], w10++
   smc:  sac.r A, [w13++]        // destination[i] = A, w13++
   #endasm
}

// void y_scalar_multiply(buffer, scalar, destination, size)
//
// Multiplies each item in a sint16 buffer by sint16 scalar and stores the
// result in the destination buffer.  The input buffer must be located in Y_RAM,
// but the scalar and destination buffer can be anywhere in RAM.  Write-back
// to the input buffer is supported.  The size parameter gives the length of
// the input and output buffers.
//
void y_scalar_multiplyw(signed int16* buffer, signed int16 scalar, signed int16* destination, unsigned int16 size)
{
   #asm
   mov   buffer, w10             // w10 -> buffer[0]
   mov   scalar, w5              // w5 = scalar
   mov   destination, w13        // w13 -> destination[0]
   mov   size, w2                // w2 = size
   dec   w2, w2                  // w2 = size-1
   clr   A, [w10]+=2, w4         // w4 = buffer[0], w10++
   do    w2, smc                 // loop "size" times
   mpy   w4*w5, A, [w10]+=2, w4  // A = buffer[i]*scalar, w4 = buffer[i+1], w10++
   smc:  sac.r A, [w13++]        // destination[i] = A, w13++
   #endasm
}

// void add_buffers(buffer1, buffer2, destination, size)
//
// Adds the contents of two complex buffers item-wise and stores the result in
// the destination buffer.  The first three input parameters are pointers to
// complex data which can be anywhere in RAM.  The destination buffer may be
// either of the input buffers as well.  The size parameter is the size of the
// complex buffers to be added.
//
void add_bufferscc(Complex* buffer1, Complex* buffer2, Complex* destination, unsigned int16 size)
{
   #asm
   mov   buffer1, w3       // w3 -> buffer1[0].re
   mov   buffer2, w4       // w4 -> buffer2[0].re
   mov   destination, w5   // w5 -> dest[0].re
   mov   size, w2          // w2 = size
   sl    w2, w2            // w2 = 2*size
   dec   w2, w2            // w2 = 2*size - 1
   do    w2, addbufcc      // do the following loop "2*size" times
   mov   [w3++],w0         // w0 = buffer1[i]
   addbufcc: add  w0, [w4++], [w5++]   // dest[i] = buffer1[i] + buffer2[i]
   #endasm
}

// void add_buffers(buffer1, buffer2, destination, size)
//
// Adds the contents of two int16 buffers item-wise and stores the result in
// the destination buffer.  The first three input parameters are pointers to
// int16 data which can be anywhere in RAM.  The destination buffer may be
// either of the input buffers as well.  The size parameter is the size of the
// int16 buffers to be added.
//
void add_buffersww(signed int16* buffer1, signed int16* buffer2, signed int16* destination, unsigned int16 size)
{
   #asm
   mov   buffer1, w3       // w3 -> buffer1[0]
   mov   buffer2, w4       // w4 -> buffer2[0]
   mov   destination, w5   // w5 -> dest[0]
   mov   size, w2          // w2 = size
   dec   w2, w2            // w2 = size - 1
   do    w2, addbufww      // do the following loop "size" times
   mov   [w3++],w0         // w0 = buffer1[i]
   addbufww: add  w0, [w4++], [w5++]   // dest[i] = buffer1[i] + buffer2[i]
   #endasm
}

// void add_buffers(buffer1, buffer2, destination, size)
//
// Adds the contents of two int16 buffers item-wise and stores the result in
// the destination buffer.  The first three input parameters are pointers to
// int16 data which can be anywhere in RAM.  The destination buffer may be
// either of the input buffers as well.  The size parameter is the size of the
// int16 buffers to be added.
//
void add_bufferscw(Complex* buffer1, signed int16* buffer2, signed int16* destination, unsigned int16 size)
{
   #asm
   mov   buffer1, w3       // w3 -> buffer1[0].re
   mov   buffer2, w4       // w4 -> buffer2[0]
   mov   destination, w5   // w5 -> dest[0]
   mov   size, w2          // w2 = size
   dec   w2, w2            // w2 = size - 1
   do    w2, addbufcw      // do the following loop "size" times
   mov   [w3++],w0         // w0 = buffer1[i]
   add  w0, [w4++], [w5++]   // dest[i] = buffer1[i] + buffer2[i]
   addbufcw: inc2 w3, w3   // skip imaginary part of buffer1
   #endasm
}

// void subtract_buffers(positive_input, negative_input, destination, size)
//
// Subtracts the negative input buffer from the positive input buffer and stores
// the result in the destination buffer.  All buffers are in complex format
// and can located anywhere in RAM.  The size parameter is the size of the
// complex buffers to be subtracted.
//
void subtract_bufferscc(Complex* positive_input, Complex* negative_input, signed int16* destination, unsigned int16 size)
{
   #asm
   mov   positive_input, w3      // w3 -> positive_input[0].re
   mov   negative_input, w4      // w4 -> negative_input[0].re
   mov   destination, w5         // w5 -> dest[0].re
   mov   size, w2                // w2 = size
   sl    w2, w2                  // w2 = 2*size
   dec   w2, w2                  // w2 = 2*size - 1
   do    w2, subbufcc            // do the following loop "2*size" times
   mov   [w3++], w0              // w0 = positive_input[i]
   subbufcc: sub  w0, [w4++], [w5++]   // dest[i] = positive_input[i] - negative_input[i]
   #endasm
}

// void subtract_buffers(positive_input, negative_input, destination, size)
//
// Subtracts the negative input buffer from the positive input buffer and stores
// the result in the destination buffer.  All buffers are in signed int16 format
// and can located anywhere in RAM.  The size parameter is the size of the
// int16 buffers to be subtracted.
//
void subtract_buffersww(signed int16* positive_input, signed int16* negative_input, signed int16* destination, unsigned int16 size)
{
   #asm
   mov   positive_input, w3      // w3 -> positive_input[0]
   mov   negative_input, w4      // w4 -> negative_input[0]
   mov   destination, w5         // w5 -> dest[0]
   mov   size, w2                // w2 = size
   dec   w2, w2                  // w2 = size - 1
   do    w2, subbufww            // do the following loop "size" times
   mov   [w3++], w0              // w0 = positive_input[i]
   subbufww: sub  w0, [w4++], [w5++]   // dest[i] = positive_input[i] - negative_input[i]
   #endasm
}

// void window(sint16* y_buffer, Complex* x_buffer, Complex* destination, size)
//
// This function multiplies the contents of a sint16 buffer and the real part of
// a Complex buffer item-wise and stores the result in a Complex destination 
// buffer.  The first buffer must located in Y_RAM, and the second buffer must 
// be located in X_RAM. The destination buffer can be anywhere in RAM and can be
// the X input buffer. The size parameter is the number of int16 multiplications
// to be done.  The imaginary values of the destination buffer are set to zero.
//
void windowwc(signed int16* y_buffer, Complex* x_buffer, Complex* destination, unsigned int16 size)
{  
   #asm
   mov   x_buffer, w8                  // w8 -> x_buffer[0].re
   mov   y_buffer, w10                 // w10 -> y_buffer[0]
   mov   destination, w13              // w13 -> destination[0].re
   mov   size, w2                      // w2 = size
   dec   w2, w2                        // w2 = size - 1
   clr   A, [w8]+=4, w4, [w10]+=2, w5  // A = 0, w4 = x_buffer[0].re, w5 = y_buffer[0], w8 -> x_buffer[1].re, w10 -> y_buffer[1]
   do    w2, vmwc                      // DO size times
   mpy   w4*w5, A, [w8]+=4, w4, [w10]+=2, w5
   sac.r  A, [w13++]  
   vmwc:  clr   [w13++]
   #endasm
}

// void left_shift_bufferc(Complex * buffer, uint16 shifts, uint16 buffer_length)
//
// This function shifts every complex value in the input buffer right by the 
// number of shifts specified.  This is not copy-safe : the results are stored 
// back to the input buffer.
//
void left_shift_bufferc(Complex * buffer, unsigned int16 shifts, unsigned int16 buffer_length)
{
   #asm
   mov   buffer, w8
   mov   shifts, w3
   mov   buffer_length, w2
   sl    w2, w2
   dec   w2, w2
   do    w2, qsc
   mov   [w8], w4
   sl    w4, w3, w5
   qsc:  mov   w5, [w8++]
   #endasm
}

// void right_shift_bufferc(Complex * buffer, uint16 shifts, uint16 buffer_length)
//
// This function shifts every complex value in the input buffer left by the 
// number of shifts specified.  This is not copy-safe : the results are stored 
// back to the input buffer.
//
void right_shift_bufferc(Complex * buffer, unsigned int16 shifts, unsigned int16 buffer_length)
{
   #asm
   mov   buffer, w8
   mov   shifts, w3
   mov   buffer_length, w2
   sl    w2, w2
   dec   w2, w2
   do    w2, qsc
   mov   [w8], w4
   asr   w4, w3, w5
   qsc:  mov   w5, [w8++]
   #endasm
}

// void left_shift_bufferc(sint16 * buffer, uint16 shifts, uint16 buffer_length)
//
// This function shifts every sint16 value in the input buffer right by the 
// number of shifts specified.  This is not copy-safe : the results are stored 
// back to the input buffer.
//
void left_shift_bufferw(signed int16 * buffer, unsigned int16 shifts, unsigned int16 buffer_length)
{
   #asm
   mov   buffer, w8
   mov   shifts, w3
   mov   buffer_length, w2
   dec   w2, w2
   do    w2, qsc
   mov   [w8], w4
   sl    w4, w3, w5
   qsc:  mov   w5, [w8++]
   #endasm
}

// void right_shift_bufferw(sint16 * buffer, uint16 shifts, uint16 buffer_length)
//
// This function shifts every sint16 value in the input buffer left by the 
// number of shifts specified.  This is not copy-safe : the results are stored 
// back to the input buffer.
//
void right_shift_bufferw(signed int16 * buffer, unsigned int16 shifts, unsigned int16 buffer_length)
{
   #asm
   mov   buffer, w8
   mov   shifts, w3
   mov   buffer_length, w2
   dec   w2, w2
   do    w2, qsc
   mov   [w8], w4
   asr   w4, w3, w5
   qsc:  mov   w5, [w8++]
   #endasm
}

#endif
