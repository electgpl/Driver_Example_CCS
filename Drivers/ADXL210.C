////////////////////////////////////////////////////////////////////////////
////                                                                    ////
////                              adxl210.c                             ////
////         Library for an Analog Devices ADXL210E accelerometer       ////
////                                                                    ////
//// The ADXL210E generates a square wave where the duty cycle          ////
//// reflects the acceleration measured.  A 50% duty cycle represents   ////
//// 0G, or no acceleration.  The library uses the CCP peripheral to    ////
//// to measure the duty cycle using the CCP interrupt.  The equation   ////
//// to find the acceleration is:                                       ////
////   A(g) = (T1/T2 - 0.5) / 0.04                                      ////
//// T2 is the total time of the square wave, T1 is the time of the     ////
//// high cycle of the square wave.  For means of calibration, 0.5      ////
//// has been replaced with zerog and 0.04 has been replaced with ppg.  ////
////                                                                    ////
//// To maximize precision, adjust the constant ADXL210_SCALING_FACTOR  ////
//// as high as possible without overflowing the int32 registers        ////
//// used to calculate the acceleration.                                ////
////                                                                    ////
//// FUNCTIONS:                                                         //// 
////                                                                    ////
//// init_accelerometer(signed int32 zerog, long ppg)                   ////
////   Initializes the accelerometer and turns on CCP interrupts.       ////
////   WARNING: ENABLES GLOBAL INTERRUPTS!                              ////
////   If you do not wish to use calibration variables,                 ////
////   use 0.5 * ADXL210_SCALING_FACTOR for zerog and                   ////
////   use 0.04 * ADXL210_SCALING_FACTOR for ppg.                       ////
////                                                                    ////
//// signed long accelerometer_read()                                   ////
////   Returns the current acceleration, in Gs.                         ////
////                                                                    ////
//// signed int32 accelerometer_find_zerog()                            ////
////   Set the ADXL210E on a flat surface (0G), and call this           ////
////   function.  It will return a new zerog variable to calibarte      ////
////   for zero g.  After this function has been called you can         ////
////   use accelerometer_init() to assign this new zerog calibration.   ////
////   WARNING: THIS FUNCTION WAITS FOR A NEW READING, Make sure        ////
////            init_accelerometer() was called before this.            ////
////                                                                    ////
//// long accelerometer_find_ppg(signed long val_1g, signed longval_ng) ////
////   Calculates and returns a new ppg value to use based upon         ////
////   previous readings which should have been 1G and -1G.  To use     ////
////   this function, set ppg to 1 and tilt the ADXL down to where it   ////
////   should read 1G.  Use accelerometer_read() and store to val_1g.   ////
////   While ppg is set to 1, tilt the ADXL up to where it should read  ////
////   -1G.  Use accelerometer_read() and store to val_ng.  Pass val_1g ////
////   and val_ng to this function to calculate the proper ppg that     ////
////   will result in a 1G and -1G calculation.  The ADXL should        ////
////   already have been calibrated for 0G before this operation.       ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services            ////
//// This source code may only be used by licensed users of the CCS C   ////
//// compiler.  This source code may only be distributed to other       ////
//// licensed users of the CCS C compiler.  No other use, reproduction  ////
//// or distribution is permitted without written permission.           ////
//// Derivative programs created using this software in object code     ////
//// form are not restricted in any way.                                ////
////////////////////////////////////////////////////////////////////////////

//raise this value as high as you can without overflowing the math
#define ADXL210_SCALING_FACTOR 100000


//prototypes
void accelerometer_isr(void);
void accelerometer_init(signed int32 zerog, long ppg);
signed long accelerometer_read(void);
signed int32 accelerometer_find_zerog(void);
long accelerometer_find_ppg(signed long val_1g, signed long val_ng);

//global vars used to calculate G
long adxl_t1, adxl_t2;
short adxl_waiting_for_fall;

//global vars used calibrate
signed int32 adxl_zerog;
long adxl_ppg;

#int_ccp1
void accelerometer_isr(void) {
   static long adxl_last_rise;   
   	
   if(adxl_waiting_for_fall) {
      adxl_t1=CCP_1-adxl_last_rise;
      setup_ccp1(CCP_CAPTURE_RE);
      adxl_waiting_for_fall=FALSE;
   } else {
      adxl_t2=CCP_1-adxl_last_rise;
      adxl_last_rise=CCP_1;
      setup_ccp1(CCP_CAPTURE_FE);
      adxl_waiting_for_fall=TRUE;
   }
}


void accelerometer_init(signed int32 zerog, long ppg) {
   adxl_zerog=zerog;
   adxl_ppg=ppg;

   setup_ccp1(CCP_CAPTURE_RE);
   adxl_waiting_for_fall=FALSE;
   setup_timer_1(T1_INTERNAL);
   enable_interrupts(INT_CCP1);
   enable_interrupts(GLOBAL);
}

// A(g) = (T1/T2 - 0.5) / 0.04
// 0g = 50% duty cycle
signed long accelerometer_read(void) {  // Returns hundreths of a g
    signed int32 value;

    value = adxl_t1 * ADXL210_SCALING_FACTOR;
    value = value / adxl_t2;
    value = value - adxl_zerog;
    value = value / adxl_ppg;

    return(value);
}

signed int32 accelerometer_find_zerog(void) {
    signed int32 value;

    //wait for new reading
    adxl_waiting_for_fall=FALSE;
    while (!adxl_waiting_for_fall) {}
    while (adxl_waiting_for_fall) {}
    while (!adxl_waiting_for_fall) {}
    
    //calculate the new zerog
    value = adxl_t1 * ADXL210_SCALING_FACTOR;
    value = value / adxl_t2;
    
    return(value);    
}

long accelerometer_find_ppg(signed long val_1g, signed long val_ng) {
    long value;
    int32 val_2g;

    val_2g=abs(val_1g) + abs(val_ng);
    value=val_2g/200;    
    
    return(value);
}