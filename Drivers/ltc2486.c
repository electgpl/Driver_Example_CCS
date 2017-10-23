///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                            LTC2486.C                              ////
////                                                                   ////
////  Library for a Linear LTC2486 16bit ADC                           ////
////                                                                   ////
////  This library assumes the CS pin is held low.  Changing the state ////
////  of the CS pin during a conversion seemed to negatively affect    ////
////  the results of the conversion.                                   ////
////                                                                   ////
////  API                                                              ////
////  ---------------------------------------------------------------- ////
////                                                                   ////
////  int1 suc = ext_adc_read_and_start(signed int32 *pRet, int16 cmd) ////
////                                                                   ////
////     Reads the previous conversion and saves result to pRet,       ////
////     then starts a new conversion using the parameters of          ////
////     cmd.  For a list of valid parameters for cmd, look in         ////
////     this file for the section labeled LTC2486_CONFIG.             ////
////     Many of the parameters for cmd can be ORd together.           ////
////     If you are only performing simple single channel              ////
////     conversion (not differential) then the function               ////
////     ext_adc_chan_to_config() can be used to generated             ////
////     the cmd needed for that channel.                              ////
////                                                                   ////
////     It is important to note that this routine reads the previous  ////
////     conversion result, and then starts a new conversion using     ////
////     the cmd parameters.                                           ////
////                                                                   ////
////     Returns TRUE if:                                              ////
////        the previous conversion is complete.                       ////
////        pRet is updated.                                           ////
////        a new conversion using cmd is started.                     ////
////     Returns FALSE if:                                             ////
////        previous conversion is not finished.                       ////
////        pRet is not updated.                                       ////
////        cmd is ignored (allow previous conversion to fininsh).     ////
////                                                                   ////
////     pRet will be assigned LTC2486_UNDERVOLTAGE if an undervoltage ////
////     error occured.  pRet will be assigned to LTC2486_OVERVOLTAGE  ////
////     if an overvoltage error occurred.                             ////
////                                                                   ////
////                                                                   ////
////  int1 ready = ext_adc_ready()                                     ////
////                                                                   ////
////     Returns TRUE if the previous conversion has finished          ////
////     and is ready to be read with ext_adc_read_and_start().        ////
////     This is done by looking at the SPI MISO line (low means       ////
////     ready).  Under some configuration conditions you may          ////
////     need to provide this macro/callback in your application       ////
////     (see CONFIG section below).                                   ////
////                                                                   ////
////                                                                   ////
////  int16 cmd = ext_adc_chan_to_config(int8 chan)                    ////
////                                                                   ////
////     chan is a single ended (non differential) channel (0-3        ////
////     for voltage channel, 4 for temperature), returns int16 cmd    ////
////     register that can be used for ext_adc_read_and_start().       ////
////                                                                   ////
////                                                                   ////
////  int32 result = ext_adc_read(int8 chan)                           ////
////                                                                   ////
////     Performs a simple read of channel (0-3, single ended)         ////
////     and returns result.  Will deadlock the processor until        ////
////     conversion is started and then read.  The time this           ////
////     takes is 2 conversions (see SPEED section below).             ////
////                                                                   ////
////                                                                   ////
////  INIT                                                             ////
////  --------------------------------------------------------------   ////
////                                                                   ////
////  There is no real init routine.  It is assumed that SPI and       ////
////  SPI pins are already initialized before calling this (use        ////
////  #use spi()).  It is also assumed CS is always low during         ////
////  entire conversion (we've had accuracy problems if CS is          ////
////  modified during conversion).  The first call to                  ////
////  ext_adc_read_and_start() will start the first conversion.        ////
////                                                                   ////
////  ext_adc_init() is provided for backwards compability with        ////
////  other libraries, but this is an empty macro.                     ////
////                                                                   ////
////                                                                   ////
////  SPEED                                                            ////
////  --------------------------------------------------------------   ////
////                                                                   ////
////  When using the internal oscillator of the LTC2486, the           ////
////  conversion speed is limited (6 samples/sec) and the SPI          ////
////  communication speed is limited (38000bps).                       ////
////                                                                   ////
////  Clocking at 2Mhz seemd to produce no negative conversion         ////
////  effects.  Conversion speed is ~48 samples/sec.  SPI comm         ////
////  speed is 4Mbps.                                                  ////
////                                                                   ////
////  PIC PWM can be used to clock to LTC2486                          ////
////                                                                   ////
////  Above conversion speeds are for using 1x speed option.  2x       ////
////  speed option could also be used, and sample rate would           ////
////  be doubled.                                                      ////
////                                                                   ////
////                                                                   ////
////  CONFIG                                                           ////
////  --------------------------------------------------------------   ////
////                                                                   ////
////  If nothing is defined before including this file, then           ////
////  EXT_ADC_MISO, EXT_ADC_MOSI and EXT_ADC_CLK and #use spi()        ////
////  are defined to use SPI1 on most PICs.  EXT_ADC_XFER()            ////
////  is defined for a SPI transfer, and ext_adc_ready() is defined    ////
////  to look for conversion ready signal.                             ////
////                                                                   ////
////  If you define your own EXT_ADC_XFER() macro before including     ////
////  this file, then those config options are ignored.                ////
////  EXT_ADC_XFER() should be a SPI read/write transfer function.     ////
////  ext_adc_ready() will also need to be defined to look at the      ////
////  conversion ready signal.                                         ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2012 Custom Computer Services          ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#ifndef __LTC2486_C__
#define __LTC2486_C__

#include <stdint.h>

#ifndef EXT_ADC_XFER
   #define EXT_ADC_MISO   PIN_C4
   #define EXT_ADC_MOSI   PIN_C5
   #define EXT_ADC_CLK    PIN_C3
   
   // you can get a faster baud rate if you clock the LTC2486 with an external
   // oscillator.
   #use spi(MASTER, bits=8, MSB_FIRST, mode=0, BAUD=38000, di=EXT_ADC_MISO, do=EXT_ADC_MOSI, clk=EXT_ADC_CLK, stream=SPI_STREAM_LTC2486)
   
   #define EXT_ADC_XFER(x)   spi_xfer(SPI_STREAM_LTC2486,x)
   
   //assumes already selected
   #define ext_adc_ready()   !input(EXT_ADC_MISO)
#else
   // user has already defined his own xfer routines.  useful if user is sharing
   // spi port or is multiple ltc2486 in his application.
      
   // user needs to point ext_adc_ready() to his own routine
#endif

////////////////// begin LTC2486_CONFIG choices //////////////////////////////

// this can be sent alone to ext_adc_read_and_start().
#define LTC2486_KEEP_PREVIOUS 0x8000

// the following groups can be or'd together when sent to 
// ext_adc_read_and_start().
// first group (channel choice) is required by ext_adc_read_and_start(),
// other groups are not required.

// first group, channel choice:
#define LTC2486_CHAN_0_1      0xA080   //0=in+, 1=in-
#define LTC2486_CHAN_2_3      0xA180   //2=in+, 3=in-
#define LTC2486_CHAN_1_0      0xA880   //0=in-, 1=in+
#define LTC2486_CHAN_3_2      0xA980   //2=in-, 3=in+
#define LTC2486_CHAN_0        0xB080   //0=in+, com=in-
#define LTC2486_CHAN_2        0xB180   //2=in+, com=in-
#define LTC2486_CHAN_1        0xB880   //1=in+, com=in-
#define LTC2486_CHAN_3        0xB980   //3=in+, com=in-
#define LTC2486_CHAN_TEMP     0xA0C0   //when using this option, speed and gain is ignored

// second group, reject mode
#define LTC2486_REJECT_5060   0x0000
#define LTC2486_REJECT_50     0x0010
#define LTC2486_REJECT_60     0x0020

// third group, speed
#define LTC2486_SPEED_1x      0x0000
#define LTC2486_SPEED_2x      0x0008

// fourth group, gain
#define LTC2486_GAIN_1x       0x0000
#define LTC2486_GAIN_4x       0x0001
#define LTC2486_GAIN_8x       0x0002
#define LTC2486_GAIN_16x      0x0003
#define LTC2486_GAIN_32x      0x0004
#define LTC2486_GAIN_64x      0x0005
#define LTC2486_GAIN_128x     0x0006
#define LTC2486_GAIN_264x     0x0007

////////////////// end LTC2486_CONFIG choices //////////////////////////////


////////////////// begin library code ///////////////////////////////////////

// provided for compatability with other libraries
#define ext_adc_init()

#define LTC2486_UNDERVOLTAGE  0xFFFFFFFF
#define LTC2486_OVERVOLTAGE   0xFFFFFFFE

static uint32_t ext_adc_write24(uint16_t val)
{
   union
   {
      uint8_t b[4];
      uint32_t dw;
   } res;
   
   res.b[3] = 0;
   res.b[2] = EXT_ADC_XFER(val >> 8);
   res.b[1] = EXT_ADC_XFER(val >> 0);
   res.b[0] = EXT_ADC_XFER(0);
   
   return(res.dw);
}

// see documentation above
uint16_t ext_adc_chan_to_config(uint8_t chan)
{
   uint16_t ret;

   switch(chan)
   {
      default:
      case 0:  ret = LTC2486_CHAN_0;      break;
      case 1:  ret = LTC2486_CHAN_1;      break;
      case 2:  ret = LTC2486_CHAN_2;      break;
      case 3:  ret = LTC2486_CHAN_3;      break;
      case 4:  ret = LTC2486_CHAN_TEMP;   break;
   }

   return(ret);
}

// see documentation above
int1 ext_adc_read_and_start(int32_t *pRet, uint16_t cmd)
{
   int32_t res;   //signed
   int1 sig,msb;
   int1 ret = FALSE;

   if (ext_adc_ready())
   {
      ret = TRUE;
      
      res = ext_adc_write24(cmd);

      if (pRet)
      {
         sig = bit_test(res, 21);
         msb = bit_test(res, 20);
         
         res >>= 4;
         res &= (uint32_t)0x0000FFFF;
      
         //printf("\r\nadc s=%u m=%u %ld ", sig, msb, res);
         
         if (sig && msb)
            res = LTC2486_OVERVOLTAGE;
         else if (!sig && !msb)
            res = LTC2486_UNDERVOLTAGE;
         if ((res!=0) && !sig)
         {
            //non standard sign extension
            res |= (uint32_t)0xFFFF0000;
         }
      
         *pRet = res;
      }
   }
   
   return(ret);
}

int32_t ext_adc_read(uint8_t chan)
{
   int32_t ret;
   
   while(!ext_adc_read_and_start(0, ext_adc_chan_to_config(chan)));
      
   while(!ext_adc_read_and_start(&ret, ext_adc_chan_to_config(chan)));
   
   return(ret);
}


#endif   //once __LTC2486_C__
