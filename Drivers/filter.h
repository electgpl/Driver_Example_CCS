/////////////////////////////////////////////////////////////////////////
////                             filter.h                            ////
////                                                                 ////
////  This file contains defines and declarations used by the filter ////
////  example.                                                       ////
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
Define the SFR registers used for FIR implementation using #word directive
*/
#word CORCON = getenv("SFR:CORCON")
#word MODCON = getenv("SFR:MODCON")
#word XMODSRT = getenv("SFR:XMODSRT")
#word XMODEND = getenv("SFR:XMODEND")
#word YMODSRT = getenv("SFR:YMODSRT")
#word YMODEND = getenv("SFR:YMODEND")

#define TABLE_SIZE 15
#define FILTER_SIZE 128
#define HALF_SIZE FILTER_SIZE/2
#define SAMPLE_INTERVAL TABLE_SIZE/FILTER_N
#define SAMPLE_SIZE 128
#define BLOCK_LENGTH 256
#define PI 3.141592654
#define STEP_SIZE ((2*PI)/TABLE_SIZE)
float temp_kernel[FILTER_SIZE];
signed int16 kernel[FILTER_SIZE];
signed int16 Output[BLOCK_LENGTH];

// Define bit structures for CORCON Register
struct
{
   int IF_Bit:1;
   int RND:1;
   int PSV:1;
   int IPL3:1;
   int ACCSAT:1;
   int SATDW:1;
   int SATB:1;   
   int SATA:1;
   int DL:3;
   int EDT:1;   
   int US:1;
   int Reserved:3;
}CORCONbits;
#locate CORCONbits = getenv("SFR:CORCON")

// Define bit structures for MODCON Register
struct
{
   int XWM:4;
   int YWM:4;
   int BWM:4;
   int Reserved:2;
   int YMODEN:1;
   int XMODEN:1;
}MODCONbits;
#locate MODCONbits = getenv("SFR:MODCON")

void init_filter_coef(void);
void init_sq_wave(void);
#BANKX
signed int16 Sq_wave[256];

#BANKY
signed int16 Circular_Buf[256];
