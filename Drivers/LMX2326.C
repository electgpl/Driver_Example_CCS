///////////////////// Driver for LMX2326 Chip ///////////////////////////
////                                                                 ////
////                                                                 ////
////  pll_select_chip()                   selects the pll chip       ////
////                                                                 ////
////  pll_deselect_chip()                 deselects the pll chip     ////
////                                                                 ////
////  pll_init()                          initializes the pll chip   ////
////                                                                 ////
////  pll_set_channel()                   sets the channel           ////
////                                                                 ////
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


// CONSTANTS
#ifndef PLL_CS
#define PLL_CS          PIN_C3                  // Chip select pin
#endif
#ifndef PLL_DATA
#define PLL_DATA        PIN_C1                  // Serial data pin
#endif
#ifndef PLL_CLOCK
#define PLL_CLOCK       PIN_C2                  // Clock pin
#endif
#ifndef PLL_LOAD
#define PLL_LOAD        PIN_C0                  // load enable pin
#endif


#define PLL_INIT_F      0x03
#define PLL_R_COUNTER   0x00
#define PLL_N_COUNTER   0x01
#define PLL_FUNCTION    0x02


// OPTIONS FOR SETUP PARAMATER
#define LOCK_DETECT_ENABLE             0x080000
#define LOCK_DETECT_DISABLE            0x000000
#define POSITIVE_PHASE_DETECTOR        0x000020
#define NEGATIVE_PHASE_DETECTOR        0x000000

#define POWERDOWN_CS_ONLY              0x000000
#define POWERDOWN_ASYNC                0x000002
#define POWERDOWN_SYNC                 0x020002

#define FoLD_TRI_STATE                 0x000000
#define FoLD_R_OUTPUT                  0x000010
#define FoLD_N_OUTPUT                  0x000004
#define FoLD_DATA_OUTPUT               0x000014
#define FoLD_LOCK_DETECT               0x000008
#define FoLD_OPEN_DRAIN_LOCK_DETECT    0x000018
#define FoLD_ACTIVE_HIGH               0x00000C
#define FoLD_ACTIVE_LOW                0x00001C

#define FASTLOCK_MODE1                 0x100080
#define FASTLOCK_MODE2                 0x100280
#define FASTLOCK_MODE3                 0x100180
#define FASTLOCK_MODE4                 0x100380

#define TIMEOUT_COUNT_3                0x000200
#define TIMEOUT_COUNT_7                0x000500
#define TIMEOUT_COUNT_11               0x000A00
#define TIMEOUT_COUNT_15               0x000E00
#define TIMEOUT_COUNT_19               0x001200
#define TIMEOUT_COUNT_23               0x001500
#define TIMEOUT_COUNT_27               0x001A00
#define TIMEOUT_COUNT_31               0x001E00
#define TIMEOUT_COUNT_35               0x002200
#define TIMEOUT_COUNT_39               0x002500
#define TIMEOUT_COUNT_43               0x002A00
#define TIMEOUT_COUNT_47               0x002E00
#define TIMEOUT_COUNT_51               0x003200
#define TIMEOUT_COUNT_55               0x003500
#define TIMEOUT_COUNT_59               0x003A00
#define TIMEOUT_COUNT_63               0x003E00

// GLOBAL VARIABLE
int32 N_counter;
short go_bit;

//FUNCTIONS
void pll_select_chip();
void pll_deselect_chip();
void pll_init(int32 setup_param, int32 ref_input_freq, int16 channel_spacing, int32 base_freq);
void pll_set_channel(int16 chan);
void load_pll(int control, int c[3]);


//~~~~~~~~~~~~~~~~
// pll_select_chip()
//    Selects the pll chip
//
// Paramaters:
//    <none>
//
// Returns:
//    <none>
//
// Special Info:
//    Must be called before any other functions
//~~~~~~~~~~~~~~~~
void pll_select_chip()
{
   output_high(PLL_CS);
}

//~~~~~~~~~~~~~~~~
// pll_deselect_chip()
//    Deselects the pll chip
//
// Paramaters:
//    <none>
//
// Returns:
//    <none>
//
// Special Info:
//    <none>
//~~~~~~~~~~~~~~~~
void pll_deselect_chip()
{
   output_low(PLL_CS);                          // deselect the chip
}


//~~~~~~~~~~~~~~~~
// pll_init()
//    Initializes the pll chip
//
// Paramaters:
//    setup_param       :  int32 >  setup paramaters for pll chip (see CONSTANTS above)
//    ref_input_freq    :  int32 >  reference input frequency (in KHz)
//    channel_spacing   :  int16 >  frequency between channels (in KHz)
//    base_freq         :  int32 >  frequency of channel 1 (in KHz)
//
// Returns:
//    <none>
//
// Special Info:
//    <none>
//~~~~~~~~~~~~~~~~
void pll_init(int32 setup_param, int32 ref_input_freq, int16 channel_spacing, int32 base_freq)
{
   int buf[3];
   int32 temp;
   int16 temp2;

   buf[0] = (int) (setup_param);                // setup Function latch
   buf[1] = (int) (setup_param >> 8);
   buf[2] = (int) (setup_param >> 16);

   load_pll(PLL_INIT_F,buf);                    // load function latch in chip

   temp = ref_input_freq / channel_spacing;     // calculate R counter
   temp &= 0x00003FFF;
   if(bit_test(setup_param, 19))
      bit_set(temp, 18);

   buf[0] = (int) (temp);                       // setup R counter
   buf[1] = (int) (temp >> 8);
   buf[2] = (int) (temp >> 16);

   load_pll(PLL_R_COUNTER,buf);                 // load R counter in chip

   N_counter = base_freq / channel_spacing;     // N_counter

   if(bit_test(setup_param, 20))                // set bit 18 if needed (GO BIT) NEED TO FIND WAY TO DO
      go_bit = TRUE;
   else
      go_bit = FALSE;

   pll_set_channel(1);                          // init to channel 1 (base channel)

}

//~~~~~~~~~~~~~~~~
// pll_set_channel()
//    Sets the pll chip to a different channel
//
// Paramaters:
//    chan  :  int16 >  channel number to set the pll to
//
// Returns:
//    <none>
//
// Special Info:
//    Output Frequency = Channel 1 Frequency + (Channel Spacing * Channel Number)
//~~~~~~~~~~~~~~~~
void pll_set_channel(int16 chan)
{
   int   buf[3];
   int32 temp_N_counter;
   int32 temp;
   int   A_counter;

   temp_N_counter = N_counter + (chan-1);       // save new N latch

   temp = temp_N_counter / 32;                  // temp is B counter

   A_counter = temp_N_counter - (temp * 32);

   temp = (temp << 5) | A_counter;              // A and B in temp

   if(go_bit)                                   // set bit 18 if needed (GO BIT)
      bit_set(temp, 18);

   buf[0] = (int) (temp);                       // setup N counter
   buf[1] = (int) (temp >> 8);
   buf[2] = (int) (temp >> 16);

   load_pll(PLL_N_COUNTER,buf);                 // load N counter in chip
}



//~~~~~~~~~~~~~~~~
// load_pll()
//    Function to load data into pll chip
//
// Paramaters:
//    control  :  byte              > what latch to load
//    c        :  array of 3 bytes  > data to load into latch
//
// Returns:
//    <none>
//
// Special Info:
//    <none>
//~~~~~~~~~~~~~~~~
void load_pll(int control, int c[3])
{
   BYTE i;

   for(i=1;i<=(24-19);i++)                      // don't use top 5 bits
      shift_left(c,3,0);

   output_low(PLL_CLOCK);
   delay_us(2);

   for(i=1;i<=19;i++)                           // 19 bits in counter or latch
   {
      output_bit(PLL_DATA,shift_left(c,3,0));   // load data (for N,R, or F)
      delay_us(2);
      output_high(PLL_CLOCK);
      delay_us(2);
      output_low(PLL_CLOCK);
      delay_us(2);
   }

   if(bit_test(control,1))                      // output high control bit
      output_high(PLL_DATA);
   else
      output_low(PLL_DATA);

   delay_us(2);                                 // clock in control bit
   output_high(PLL_CLOCK);
   delay_us(2);
   output_low(PLL_CLOCK);
   delay_us(2);

   if(bit_test(control,0))                      // output low control bit
      output_high(PLL_DATA);
   else
      output_low(PLL_DATA);

   delay_us(2);                                 // clock in control bit
   output_high(PLL_CLOCK);
   delay_us(2);
   output_low(PLL_CLOCK);
   delay_us(2);

   output_high(PLL_LOAD);                       // load data into latch
   delay_us(2);
   output_low(PLL_LOAD);
}
