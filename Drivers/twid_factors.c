/////////////////////////////////////////////////////////////////////////
////                        twid_factors.c                           ////
////                                                                 ////
////   This file containing functions used to calculate the twiddle  ////
////   factors used by the FFT algorithm                             ////
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


// Function to initialize the twiddle factors
void Init_Twid_factors(void);

// Function to convert Float numbers to fractional format
int Float2Fract(float num);

// Function to initialize the twiddle factors
void Init_Twid_factors(void)
{
   float TwidComplex_R, TwidComplex_I;
   // Twid_factor defined in fft.h file : for 128 Twid factors, real + imaginary
   int i,j;                  
   int numFactors;
   int log2N=8;
   float arg;

   numFactors = (1<<log2N)/2;   
   j=0; // pointer for twid factor
   for (i = 0; i < numFactors; i++ )
   {
        arg = 2.0*PI*i/numFactors;        
        TwidComplex_R =  cos (arg);
        Twid_factor[j] = Float2Fract(TwidComplex_R);
        j = j+2; // Increment pointer to point to imaginary part
        TwidComplex_I = -sin (arg);
        Twid_factor[i] = Float2Fract(TwidComplex_I);
    }
}



// Function to convert Float numbers to fractional format
int Float2Fract(float num)
{

   int return_val;
   float temp;

   temp = num*FRACT_MULTIPLIER;  // This multiplier is for 1.15 format

   if(temp>0)
   return_val = floor(temp);
   else
   return_val = ceil(temp);

   return return_val;
}
