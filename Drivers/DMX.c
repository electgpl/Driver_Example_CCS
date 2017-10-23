////////////////////////////////////////////////////////////////////////////////
////                                 DMX.c                                  ////
//// DMX Library for DMX512 protocol.                                       ////
////                                                                        ////
//// This library provides the following functions:                         ////
////  (for more information on these functions see the comment              ////
////   header above each function)                                          ////
////                                                                        ////
//// DMXInit() - Initializes the Enable line and clears the DMXData array.  ////
////                                                                        ////
//// DMXSetChannel() - Sets the DMX Channel to the specified value.  This   ////
////                   only writes the value to the DMXData array, use      ////
////                   DMXCommit() to send value to devices.                ////
////                                                                        ////
//// DMXGetChannel() - Reads the value that the specifed DMX channel is set ////
////                   to.  This reads the value that is stored in the      ////
////                   DMXData array, may not match what the actual channel ////
////                   is set to if DMXCommit() was done after making a     ////
////                   change with DMXSetChannel().                         ////
////                                                                        ////
//// DMXCommit() - Sends channel values set in DMXData array to DMX         ////
////               devices.                                                 ////
////                                                                        ////
//// Use the following defines to set the PIC pins that connect to the      ////
////  RS485 Transceiver DE and DI pins:                                     ////
////   #define DMX_XMIT PIN_xx     //RS485 Transceiver DI pin               ////
////   #define DMX_ENABLE PIN_xx   //RS485 Transceiver DE pin               ////
////                                                                        ////
//// Use the following defines to set the number of DMX unit you are        ////
////  controlling and the number of channels each unit has.                 ////
////   #define DMX_UNITS x                                                  ////
////   #define DMX_CHANNELS x                                               ////
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

#include <DMX.h>

////////////////////////////////////////////////////////////////////////////////
//DMXInit()
// Initializes the RS485 chip and sets all DXM channel values to zero.
//  Parameters: None
//  Returns:    Nothing
////////////////////////////////////////////////////////////////////////////////
void DMXInit(void)
{
   output_low(DMX_ENABLE);

   memset(DMXData,0,sizeof(DMXData));
}

////////////////////////////////////////////////////////////////////////////////
//DMXSetChannel()
// Writes the specified value to the specified DMX channel
//  Parameters: channel - channel to write the value to, channel is 1 indexed.
//              value - value to set specified channel to.
//  Returns:    Nothing
////////////////////////////////////////////////////////////////////////////////
void DMXSetChannel(uint8_t channel, uint8_t value)
{
   uint8_t unit;
   
   channel -= 1;
   unit = channel / DMX_CHANNELS;

   DMXData[unit][channel] = value;
}

////////////////////////////////////////////////////////////////////////////////
//DMXGetChannel()
// Reads the value of the specified DMX channel.
//  Parameters: channel - channel to read the value from, channel is 1 indexed.
//  Returns:    The value read from the specified channel.
////////////////////////////////////////////////////////////////////////////////
uint8_t DMXGetChannel(uint8_t channel)
{
   uint8_t unit;
  
   channel -= 1;
   unit = channel / DMX_CHANNELS;

   return(DMXData[unit][channel]);
}

////////////////////////////////////////////////////////////////////////////////
//DMXCommit()
// Sends the DMX channel data to the the DMX devices.
//  Parameters: None
//  Returns:    Nothing
////////////////////////////////////////////////////////////////////////////////
void DMXCommit(void)
{
   uint16_t i;
   uint8_t *ptr = DMXData;
   
   output_high(DMX_ENABLE);
   
   output_low(DMX_XMIT);   //Break
   delay_us(100);
   
   output_high(DMX_XMIT);  //MAB
   delay_us(8);
   
   fputc(0x00,DMX);        //SC
   
   for(i=0;i<(DMX_UNITS*DMX_CHANNELS);i++)
   {
      fputc(*ptr++,DMX);
   }
   
   output_low(DMX_ENABLE);
}
   
