///////////////////////////////////////////////////////////////////////////
////                         dci.h		                               ////
////                                                                   ////
//// This file contains register definitions and control functions for ////
//// the dci peripheral found on some dsPIC devices. This library is   ////
//// mainly intended for use with the TLV320AIC23B driver.			   ////
////                                                                   ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#ifndef DCI_H
#define DCI_H 1

#word DCICON1  = 0x280
#bit COFSM0    = DCICON1.0
#bit COFSM1    = DCICON1.1
#bit DJST      = DCICON1.5
#bit CSDOM     = DCICON1.6
#bit UNFM      = DCICON1.7
#bit COFSD     = DCICON1.8
#bit CSCKE     = DCICON1.9
#bit CSCKD     = DCICON1.10
#bit DLOOP     = DCICON1.11
#bit DCISIDL   = DCICON1.13
#bit DCIEN     = DCICON1.15

#define MULTICHANNEL_MODE  0x0000
#define I2S_MODE           0x0001
#define AC97_16BIT_MODE    0x0002
#define AC97_20BIT_MODE    0x0003

#define JUSTIFY_DATA       0x0020 //Start data transmission on the same clock cycle as COFS pulses (multichannel)
#define MULTI_DEVICE_BUS   0x0040 //Tri-states the CSDO pin during unused time slots, normally drives '0'

#define UNDERFLOW_LAST     0x0080 //Transmits the last written value to the dci when data underflow occurs. Default is undeflow silence.
#define UNDERFLOW_SILENCE  0x0000 //Transmit '0's on data undeflow. This will cause a clicking noise if you are underflowing data while outputing to a codec.

#define DCI_SLAVE          0x0100 //Default is master
#define DCI_MASTER         0x0000 

#define SAMPLE_FALLING_EDGE 0x0000 //Default is Sample on the rising edge of the clock
#define SAMPLE_RISING_EDGE 0x0200

#define DCI_CLOCK_INPUT    0x0400 //SCLK is an input (supplied by codec or external source)
#define DCI_CLOCK_OUTPUT   0x0000 //Default is clock output

#define ENABLE_LOOPBACK    0x0800 //Connect the CSDI/CSDO internally; loops all of the sound back onto the bus

#word DCICON2  = 0x282
#bit BLEN0     = DCICON2.10
#bit BLEN1     = DCICON2.11
#bit COFSG3    = DCICON2.8
#bit COFSG2    = DCICON2.7
#bit COFSG1    = DCICON2.6
#bit COFSG0    = DCICON2.5
#bit WS3       = DCICON2.3
#bit WS2       = DCICON2.2
#bit WS1       = DCICON2.1
#bit WS0       = DCICON2.0

#word DCICON3  = 0x284

#word DCISTAT  = 0x286
#bit TMPTY     = DCISTAT.0
#bit TUNF      = DCISTAT.1
#bit RFUL      = DCISTAT.2
#bit ROV       = DCISTAT.3
#bit SLOT0     = DCISTAT.8
#bit SLOT1     = DCISTAT.9
#bit SLOT2     = DCISTAT.10
#bit SLOT3     = DCISTAT.11

#word TSCON    = 0x0288 
#word RSCON    = 0x028C
#word RXBUF0   = 0x290
#word RXBUF1   = 0x292
#word RXBUF2   = 0x294
#word RXBUF3   = 0x296
#word TXBUF0   = 0x298
#word TXBUF1   = 0x29A
#word TXBUF2   = 0x29C
#word TXBUF3   = 0x29E

#word BCG      = 0x284

#define CODEC_MULTICHANNEL  0x0000  //enables multichannel (generic) codec support
#define CODEC_I2S           0x0001  //enables the I2S protocol
#define CODEC_AC16          0x0002  //enables the AC-16 protocol, setupCodecTransmission has no effect
#define CODEC_AC20          0x0003  //enables the AC-20 protocol, setupCodecTransmission has no effect

#define BUS_MASTER          0x0000  //designates this device as the bus master
#define BUS_SLAVE           0x0300  //designates this device as a slave on a bus controlled by another codec

#define TRISTATE_BUS        0x0040        //when in master mode, the module will be in high impedance 
                                    //during disabled frames (default drives 0's onto bus)
#define SYNC_COFS_DATA_PULSE 0x0020 //signifies that data starts transmitting on the same SCK pulse as the edge of the COFS pulse
                                    //(default data starts on the clock after the rising edge of COFS             
/* Determines when a sample is taken */
#define SAMPLE_RISING        0x0200 //sample data on the rising edge of the clock
#define SAMPLE_FALLING       0x0000 //sample data on the falling edge of the clock

#define DCI_4BIT_WORD      0x0003
#define DCI_5BIT_WORD      0x0004
#define DCI_6BIT_WORD      0x0005
#define DCI_7BIT_WORD      0x0006
#define DCI_8BIT_WORD      0x0007
#define DCI_9BIT_WORD      0x0008
#define DCI_10BIT_WORD     0x0009
#define DCI_11BIT_WORD     0x000A
#define DCI_12BIT_WORD     0x000B
#define DCI_13BIT_WORD     0x000C
#define DCI_14BIT_WORD     0x000D
#define DCI_15BIT_WORD     0x000E
#define DCI_16BIT_WORD     0x000F

#define DCI_1WORD_FRAME    0x0000
#define DCI_2WORD_FRAME    0x0010 << 1
#define DCI_3WORD_FRAME    0x0020 << 1
#define DCI_4WORD_FRAME    0x0030 << 1
#define DCI_5WORD_FRAME    0x0040 << 1
#define DCI_6WORD_FRAME    0x0050 << 1
#define DCI_7WORD_FRAME    0x0060 << 1
#define DCI_8WORD_FRAME    0x0070 << 1
#define DCI_9WORD_FRAME    0x0080 << 1
#define DCI_10WORD_FRAME   0x0090 << 1
#define DCI_11WORD_FRAME   0x00A0 << 1
#define DCI_12WORD_FRAME   0x00B0 << 1
#define DCI_13WORD_FRAME   0x00C0 << 1
#define DCI_14WORD_FRAME   0x00D0 << 1
#define DCI_15WORD_FRAME   0x00E0 << 1
#define DCI_16WORD_FRAME   0x00F0 << 1

#define RECEIVE_NONE    0x0000
#define RECEIVE_ALL     0xFFFF
#define RECEIVE_SLOT0   0x0001
#define RECEIVE_SLOT1   0x0002
#define RECEIVE_SLOT2   0x0004
#define RECEIVE_SLOT3   0x0008
#define RECEIVE_SLOT4   0x0010
#define RECEIVE_SLOT5   0x0020
#define RECEIVE_SLOT6   0x0040
#define RECEIVE_SLOT7   0x0080
#define RECEIVE_SLOT8   0x0100
#define RECEIVE_SLOT9   0x0200
#define RECEIVE_SLOT10  0x0400
#define RECEIVE_SLOT11  0x0800
#define RECEIVE_SLOT12  0x1000
#define RECEIVE_SLOT13  0x2000
#define RECEIVE_SLOT14  0x4000
#define RECEIVE_SLOT15  0x8000

#define TRANSMIT_NONE   0x0000
#define TRANSMIT_ALL    0xFFFF
#define TRANSMIT_SLOT0  0x0001
#define TRANSMIT_SLOT1  0x0002
#define TRANSMIT_SLOT2  0x0004
#define TRANSMIT_SLOT3  0x0008
#define TRANSMIT_SLOT4  0x0010
#define TRANSMIT_SLOT5  0x0020
#define TRANSMIT_SLOT6  0x0040
#define TRANSMIT_SLOT7  0x0080
#define TRANSMIT_SLOT8  0x0100
#define TRANSMIT_SLOT9  0x0200 
#define TRANSMIT_SLOT10 0x0400
#define TRANSMIT_SLOT11 0x0800
#define TRANSMIT_SLOT12 0x1000
#define TRANSMIT_SLOT13 0x2000 
#define TRANSMIT_SLOT14 0x4000
#define TRANSMIT_SLOT15 0x8000

void set_sample_rate(int32 sr);


void dci_initialize(int16 configuration, int16 data_size, int16 rx_config, int16 tx_config, int32 sample_rate)
{
   //signed int16 flush_dump = 0;
   /* Setup DCI */
   
   /* I2S Mode *
   COFSM1 = 0;
   COFSM0 = 1;
   /* As per I2S, data is begun 1 CSCK cycle after justification pulse *
   DJST = 0;
   
   /* Master mode *
   COFSD = 0;
   CSCKD = 0;
   CSDOM = 0;//tristate bit, clear this bit to clear up the scope waveform
      
   /* Data on rising edge *
   CSCKE = 1;
   /* No active idle *
   DCISIDL = 0;
   /* No Loopback *
   DLOOP = 0;
   /* Transmit last value on underflow (reduces clicking) *
   UNFM = 1;
   
   /* Master I2S mode, tristate-bus when inactive, underflow last value, sample rising edge, standard I2S justification */
   DCICON1 = configuration;//(I2S_MODE | MULTI_DEVICE_BUS | UNDERFLOW_LAST | DCI_MASTER | DCI_CLOCK_OUTPUT | SAMPLE_RISING_EDGE);
   
   /* Set BLEN to create an interrupt every I2S frame.
    * 2 Data Frames per Word
    * 16 bits per frame */
   DCICON2 = data_size;
   
   /* This is hard coded to allow DMA & as a work around for a silicon errata */
   BLEN1 = 0;
   BLEN0 = 0;
   /*
   COFSG3 = 0;
   COFSG2 = 0;
   COFSG1 = 0;
   COFSG0 = 0;
   WS3 = 1;
   WS2 = 1;
   WS1 = 1;
   WS0 = 1;
 /*
   /* Receive & Transmit on the first two frames. */
   TSCON = tx_config;//0x0003;
   RSCON = rx_config;//0x0003;
   
   set_sample_rate(sample_rate);
}

void set_sample_rate(int32 sr)
{
   /* Calculate the BCG from the clock speed & sample rate */
   //BCG = (getenv("CLOCK") / (4 * sr)) - 1;
   int32 fcy;
   
   fcy = getenv("CLOCK") / 2;
   sr *= (8 * ((DCICON2 & 0x000F) + 1));
   BCG = ((fcy) / (2 * sr)) - 1;
}

void dci_start()
{
   /* Enable the DCI Unit */
   DCIEN = 1;
   while(!DCIEN);
}

void setup_dci(int16 mode)
{
   DCICON1 = mode;
   
   DCISIDL = 0;
   DCIEN = 1;
   while(!DCIEN);
}

void setup_dci_tx_frames(int16 frmEn)
{
   TSCON = frmEn;
}

void setup_dci_rx_frames(int16 frmEn)
{
   RSCON = frmEn;
}

#endif
