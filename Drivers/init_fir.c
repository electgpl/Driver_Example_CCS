/////////////////////////////////////////////////////////////////////////
////                         init_fir.c                              ////
////                                                                 ////
////  This file contains function definitions used for initialising  ////
////  the filter related parameters.                                 ////
////                                                                 ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2009 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

/*
The init_sq_wave function will store a sq wave in RAM in fractional format.
0x7FFF = 0.999969482
0x8001 = -0.999969842
*/
void init_sq_wave(void)
{
   int16 i,j;
   BOOLEAN toggle;

   toggle=1;
   j=0;
   for(i=0;i<BLOCK_LENGTH;i++)
   {
      j++;
      if(toggle)
      Sq_wave[i]=0x7FFF;
      else 
      Sq_wave[i]=0x8001;
      if(j==10)
      {
         toggle^=1;
         j=0;
      }
   }
   
}


/*
void setup_registers
int * kernel - Address of Filter kernel
int * Circular_Buf - Address of Circular Buffer
int length - Length of the block that wil lbe filtered at one time
*/

// Following function will setup the CORCON and MODCON registers
// It will also setup the Start and End addresses for circular
// buffer addressing
void setup_registers(signed int * kernel,  signed int length)
{   
   CORCONbits.IF_Bit = 0; // Use the fractional mode for DSP multiply
   CORCONbits.RND = 0;    // Unbiased rounding enabled
   CORCONbits.PSV = 0; // program space NOT visible in data space
   CORCONbits.IPL3 = 0; // CPU Interrupt priority is 7 or less
   CORCONbits.ACCSAT = 1; // Super saturation mode 9.31
   CORCONbits.SATDW = 1; // Data space write saturation enabled
   CORCONbits.SATA = 1; // Accumulator A saturation enabled
   CORCONbits.US = 0; // DSP multiply is signed

   MODCONbits.XMODEN=1;  // X AGU modulus addressing enabled
   MODCONbits.YMODEN=1;  // Y AGU modulus addressing enabled
   MODCONbits.BWM=15; // Disable bit-reveresed addressing
   MODCONbits.XWM=8; // Use register W8
   MODCONbits.YWM=9; // Use register W9

// Setup the XMOD and YMOD start and end register for modulo addressing
   YMODEND = &Circular_Buf[0] + length -1;
   YMODSRT = &Circular_Buf[0]; // Set the End address
   
   XMODEND = &kernel[0] + length - 1;
   XMODSRT = &kernel[0];

}

