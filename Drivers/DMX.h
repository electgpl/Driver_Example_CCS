////////////////////////////////////////////////////////////////////////////////
////                                 DMX.c                                  ////
////                                                                        ////
//// Includes, defines, variables and prototypes for the DMX library.       ////
////                                                                        ////
////////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2012 Custom Computer Services                ////
//// This source code may only be used by licensed users of the CCS         ////
//// C compiler.  This source code may only be distributed to other         ////
//// licensed users of the CCS C compiler.  No other use,                   ////
//// reproduction or distribution is permitted without written              ////
//// permission.  Derivative programs created using this software           ////
//// in object code form are not restricted in any way.                     ////
////////////////////////////////////////////////////////////////////////////////

#ifndef _DMX_H
#define _DMX_H

// Includes
#include <stdint.h>

// Defines
#ifndef DMX_XMIT
#define DMX_XMIT  PIN_B3
#endif

#ifndef DMX_ENABLE
#define DMX_ENABLE PIN_B4

#ifndef DMX_UNITS
#define DMX_UNITS    1
#endif

#ifndef DMX_CHANNELS
#define DMX_CHANNELS    7
#endif

#if (DMX_CHANNELS*DMX_UNITS) > 512
#error Max number of DMX channels is 512
#endif

// Variables
uint8_t DMXData[DMX_UNITS][DMX_CHANNELS];

//sets up software UART for DMX protocol
#use rs232(xmit=DMX_XMIT,BAUD=250000,BITS=8,PARITY=N,STOP=2,DISABLE_INTS,stream=DMX)

// Prototypes
void DMXInit(void);
void DMXSetChannel(uint8_t channel, uint8_t value);
uint8_t DMXGetChannel(uint8_t channel);
void DMXCommit(void);

#endif
