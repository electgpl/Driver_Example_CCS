//////////// Driver for ISD4003 Voice Record/Playback Chip //////////////
////                                                                 ////
////  init_isd()                 Initializes the chip for operation. ////
////                                                                 ////
////  record_message(address)    Records a message at the given      ////
////                             address.  Stop_message must be      ////
////                             called to stop recording            ////
////                                                                 ////
////  play_message(address)      Plays a message at the given        ////
////                             address.  Stop_message must be      ////
////                             called to stop playing.             ////
////                                                                 ////
////  stop_message()             Stops the current operation and     ////
////                             returns the isd pointer value of    ////
////                             where the action stopped.           ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////


#ifndef ISD4003_SELECT
#define ISD4003_RAC     PIN_C1
#define ISD4003_SELECT  PIN_C2
#define ISD4003_CLK     PIN_C3
#define ISD4003_MISO    PIN_C4
#define ISD4003_MOSI    PIN_C5
#endif


#define  POWER_UP    0X2000
#define  SET_PLAY    0XE000
#define  PLAY        0XF000
#define  SET_RECORD  0XA000
#define  RECORD      0XB000
#define  SET_MC      0XE800
#define  MESSAGE_CUE 0XF800
#define  STOP        0X3000
#define  POWER_DOWN  0X1000
#define  READ_INT    0X3000

enum isd_mode  {playing, recording, stopped};
isd_mode current_isd_mode; // global variable for state of isd chip


//this function writes a 16 bit number to the isd chip
void write_isd(long value)  {
   int i;

   output_low(ISD4003_SELECT);      // enable the device
   for(i=0;i<16;i++)
   {                                // shift LSB first so it latches on rising edge
      output_bit(ISD4003_MOSI, shift_right(&value,2,0));
      delay_us(50);
      output_high(ISD4003_CLK);
      delay_us(100);
      output_low(ISD4003_CLK);
      delay_us(50);
   }
   output_high(ISD4003_SELECT);     // disable the device
   delay_ms(1);
}

//this function reads and writes a 16 bit number to the isd chip
long read_isd(long value)  {
   long retval;
   int i;

   output_low(ISD4003_SELECT);      // enable the device
   for(i=0;i<16;i++)
   {        // shift out LSB first and shift in LSB first
      shift_right(&retval,2,input(ISD4003_MISO));
      output_bit(ISD4003_MOSI, shift_right(&value,2,0));
      delay_us(25);
      output_high(ISD4003_CLK);
      delay_us(50);
      output_low(ISD4003_CLK);
      delay_us(25);
   }
   output_high(ISD4003_SELECT);     // disable the device
   delay_ms(1);
   return(retval);
}

//this function initializes the device
void init_isd() {
   output_high(ISD4003_SELECT);
   output_low(ISD4003_MOSI);
   output_low(ISD4003_MISO);
   output_high(ISD4003_CLK);

   current_isd_mode = stopped;
}

//stops the current function (usually play or record)
//also turns off the chip
long stop_message()  {
   long address=0;

   address = read_isd(STOP);
   address >>=2;
   address &=0x07ff;
   write_isd(POWER_DOWN);
   current_isd_mode= stopped;
   return(address);
}

//this function is used to record a message on the chip
void record_message(long address)  {
   long data_out;

   current_isd_mode= recording;
   read_isd(POWER_UP);           // turn on chip
   delay_ms(50);

   read_isd(POWER_UP);
   delay_ms(100);

   data_out=SET_RECORD|address;  // set to record
   read_isd(data_out);

   read_isd(RECORD);             // record the message
 }

//this function is used to play a message on the chip
void play_message(long address)  {
   long data_out;

   current_isd_mode=playing;
   read_isd(POWER_UP);           // turn on chip
   delay_ms(50);

   data_out=SET_PLAY|address;    // set to play
   read_isd(data_out);

   read_isd(PLAY);               // play the message
}
