/////////////////////////////////////////////////////////////////////////
////                           fir.c                                 ////
////                                                                 ////
////  This file contains function definitions used for performing    ////
////  filtering.                                                     ////
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



void init_filter_coef(void)
{
   int i,k;
   float quotient;
   
   temp_kernel[0]=1; // Inititalize the 0th element- Math error will occur for sin0/0

   // Folloiwng loop will initialize the filter as a single sided sinc function
   for(i=1;i<FILTER_SIZE;i++)
   {
     quotient = (float)i * STEP_SIZE;
     temp_kernel[i]=  sin(quotient)/(quotient);//(Fn/F0);
   }

// Following code will make the filter symmetrical
   i =0;
   for(k=FILTER_SIZE/2;k<FILTER_SIZE;k++)
   {     
      temp_kernel[k]=temp_kernel[i];
      i++;
   }
   i = FILTER_SIZE-1;
   for(k=1;k<FILTER_SIZE/2;k++)
   {
      temp_kernel[k]=temp_kernel[i];
      i--;
   }

   // Scale the filter to convert to fractional format
   // The fractional format is 1.15, which is a signed 16 bit number
   // with a range of -1.0 (0x8000) to 0.999969482 (0x7FFF)
   // The purpose of this scaling is to convert the floating point
   // value of the filter kernel
   for(i=0;i<FILTER_SIZE;i++)
   {
      temp_kernel[i]= temp_kernel[i]*6553.6; 
      temp_kernel[i]= ceil(temp_kernel[i]);

   }
   // Load the floating point values into the kernel
   // The kernel is a 16 bit value, but the DSPIC treats it as
   // a fractional format data (1.15) with top bit as sign bit 
   // with a resolution of 3.051757e-5
   for(i=0;i<FILTER_SIZE-1;i++)
      kernel[i]= (int)temp_kernel[i+1]; 

}


/*
void Filter_FIR()
Parameters:
int * input  - Input Array
int * output - Output Array
iny * kernel - Filter kernel
int length - Length of the block that wil lbe filtered at one time
*/

void Filter_FIR(int * addr_in, int * addr_out, int * addr_kernel,  int length)
{
// Circular_Buf is located in BANKY. This is used as scratch register
// during the MAC instruction

   int B_Length=BLOCK_LENGTH;
   int * addr_cbuf;
   int delaycnt;
   addr_cbuf = &Circular_Buf[0];
   

#ASM
   MOV B_Length,W0  // Store Block Length in W0
   MOV length,W1    // Store filter length in W1
   MOV addr_in, W2  // Store address of input in W2
   MOV addr_out, W3  // Store address of output in W3
   MOV addr_kernel,W8 // Store kernel address in W8, this is the X AGU 
   MOV addr_cbuf,W10   // Store address of circular buffer in W9
   
   DEC W0,W0
   DEC2 W1,W1
   DEC W1,W1
   do W0, StoreAcc
   MOV [W2++],[W10]
   CLR   A,[W8]+=2,W5,[W10]+=2,W6
   MOV [W10++],W5
   MOV [W8++],W4
   
   REPEAT W1  // Repeat for FILTER_LENGTH-3
   MAC   W4*W5,A,[W8]+=2,W4,[W10]+=2,W5
   MAC   W4*W5,A,[W8]+=2,W4,[W10],W5  // Do not increment circular buffer addr
   MAC   W4*W5,A   // Perform last multiply
StoreAcc:
   SAC.R   A,[W3++] // Store result in addr_out buffer
   MOV W0, delaycnt  // Save the sample delay
#ENDASM
}



