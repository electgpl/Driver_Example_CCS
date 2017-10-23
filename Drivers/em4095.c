/////////////////////////////////////////////////////////////////////////
////                             em4095.c                            ////
//// This file contains drivers for a EM4095 RFID basestation.       ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////                          Pin Layout                             ////
////   ------------------------------------------------------------  ////
////   |                                                          |  ////
////   | 1: VSS       GND         | 16: DC2                       |  ////
////   |                          |                               |  ////
////   | 2: RDY/CLK   RF_RDY_CLK  | 15: FCAP                      |  ////
////   |                          |                               |  ////
////   | 3: ANT1                  | 14: SHD         RF_SHD        |  ////
////   |                          |                               |  ////
////   | 4: DVDD                  | 13: DEMOD_OUT   RF_DEMOD_OUT  |  ////
////   |                          |                               |  ////
////   | 5: DVDS                  | 12: MOD         RF_MOD        |  ////
////   |                          |                               |  ////
////   | 6: ANT2                  | 11: AGND                      |  ////
////   |                          |                               |  ////
////   | 7: VDD       +5V         | 10: CDEC_IN                   |  ////
////   |                          |                               |  ////
////   | 8: DMOD_IN               |  9: CDEC_OUT                  |  ////
////   ------------------------------------------------------------  ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2004 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#ifndef EM4095
#define EM4095

#ifndef RF_SHD
#define RF_RDY_CLK   PIN_C0         // External interrupt used to read clock
#define RF_SHD       PIN_B1         // High disables the antenna signal
#define RF_MOD       PIN_B2         // High does 100% modulation
#define RF_DEMOD_OUT PIN_C2         // Data read in interrupt service routine
#endif


// Provide a buffer for storing recieved data and data to be sent
#define RFBUFFER_SIZE 20
int8 RFbuffer[RFBUFFER_SIZE];
int8 RFbuffer_index = 0;
int8 RFbuffer_bitIndex = 0;
#define END_OF_RFBUFFER (RFbuffer_index == sizeof(RFbuffer))


/////////////////////////////////////////////////////////////////////////
//// Read modes available for reading data from a transponder
/////////////////////////////////////////////////////////////////////////
int8 RF_readMode;
#define RF_MANCHESTER_DATA    0     // Reads Manchester encoded data
#define RF_MEASURE_WIDTHS     1     // Measure a series of widths
#define RF_FIND_WIDTH         2     // Find a specific width
#define RF_FIND_PATTERN       3     // Find a pattern of widths
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
//// Global Variables
/////////////////////////////////////////////////////////////////////////
int1  bitValue              = 1;
int1  storeData             = TRUE;
int1  RE_FE_TOGGLE          = 1;
int1  RF_widthFound         = FALSE;
int1  RF_patternFound       = FALSE;
int8  RF_widthToFind        = 0;
int8* RF_findWidths         = 0;
int8  RF_uncertainty        = 0;
int8  timer0_overflows      = 0;
int8  dataTransferred       = 0;
int16 old_clock             = 0;
/////////////////////////////////////////////////////////////////////////


// Purpose:       Initializes the 4095 into sleep mode
//                Sets up the timers and interrupts
void rf_init()
{
   output_low(RF_SHD);
   output_low(RF_MOD);

   setup_timer_1(T1_EXTERNAL_SYNC | T1_DIV_BY_1);
   setup_ccp1(CCP_CAPTURE_RE);
   setup_ccp2(CCP_COMPARE_INT);

   setup_timer_0(RTCC_INTERNAL | RTCC_DIV_256 | RTCC_8_BIT);
   enable_interrupts(INT_RTCC);
   enable_interrupts(GLOBAL);
}

// Purpose:       Powers down the RF antenna
#define rf_powerDown()  output_high(RF_SHD);

// Purpose:       Powers up the RF antenna
#define rf_powerUp()    output_low(RF_SHD);


// Purpose:       Select which edge to begin reading data
void RF_readEdge(int1 edge)
{
   if(edge)
   {
      setup_ccp1(CCP_CAPTURE_RE);
      RE_FE_TOGGLE = 1;
   }
   else
   {
      setup_ccp1(CCP_CAPTURE_FE);
      RE_FE_TOGGLE = 0;
   }
}


// Purpose:       Interrupt service routine to handle compare 1 interrupts.
//                Reads incoming data from a transponder and stores it in
//                the global buffer.
#INT_CCP1
void isr_ccp1()
{
   int8  width;

   // Toggle between capturing rising and falling edges to meausure width
   if(RE_FE_TOGGLE)
   {
      setup_ccp1(CCP_CAPTURE_FE);
      RE_FE_TOGGLE = 0;
   }
   else
   {
      setup_ccp1(CCP_CAPTURE_RE);
      RE_FE_TOGGLE = 1;
   }

   // Calculate the width
   width = CCP_1 - old_clock;
   old_clock = CCP_1;

   switch(RF_readMode)
   {
      // Use to receive manchester formatted data from a transponder
      case RF_MANCHESTER_DATA:
      {
         if(width > 54)           // Check for a phase change
         {
            bitValue = ~bitValue; // Invert the save bit value
            storeData = TRUE;     // Force a bit store
         }

         if(storeData)
         {
            shift_right(RFbuffer+RFbuffer_index, 1, bitValue);
            ++dataTransferred;

            if(++RFbuffer_bitIndex == 8)
            {
               RFbuffer_bitIndex = 0;
               ++RFbuffer_index;
            }
         }

         storeData = ~storeData;
         break;
      }

      // Use to read high and low widths
      case RF_MEASURE_WIDTHS:
      {
         RFbuffer[RFbuffer_index++] = width;
         ++dataTransferred;

         break;
      }

      // Use to search for a certain pulse width
      case RF_FIND_WIDTH:
      {
         if(width > (RF_widthToFind - RF_uncertainty)
         && width < (RF_widthToFind + RF_uncertainty))
         {
            RF_widthFound = TRUE;
         }
         break;
      }

      case RF_FIND_PATTERN:
      {
         if(width > RF_findWidths[RFbuffer_index] - RF_uncertainty
         && width < RF_findWidths[RFbuffer_index] + RF_uncertainty)
         {
            if(++RFbuffer_index == dataTransferred)
            {
               RF_patternFound = TRUE;
            }
         }
         else
         {
            if(RFbuffer_index > 0)
            {
               int8 pos, i, j;
               pos = RFbuffer_index-1;   // Save the initial position

               // Try to match partial pattern
               while(--RFbuffer_index != 0)
               {
                  if(width > RF_findWidths[RFbuffer_index] - RF_uncertainty
                  && width < RF_findWidths[RFbuffer_index] + RF_uncertainty)
                  {
                     for(i=pos, j=RFbuffer_index-1; j!=255; --i, --j)
                     {
                        if(RF_findWidths[j] != RF_findWidths[i])
                        {
                           break;
                        }
                     }
                     if(j == 255)
                     {
                        break;
                     }
                  }
               }
            }
         }
         break;
      }
   }
}


// Purpose:       This interrupt service routine is used
//                to send data to a transponder
// Inputs:        None
// Outputs:       None
#INT_CCP2
void isr_ccp2()
{
   static int1 mode = 1;

   if(mode == 1 && !END_OF_RFBUFFER)
   {
      // Output high to modulate the antenna, so send a 0 with modulation pin high
      output_bit(RF_MOD, !bit_test(RFbuffer[RFbuffer_index], RFbuffer_bitIndex));

      if(++RFbuffer_bitIndex == 8)   // Increment the buffer indexes as necessary
      {
         RFbuffer_bitIndex = 0;
         ++RFbuffer_index;
      }

      CCP_2 += 30;            // Wait for half the bit period minus two RF periods
      mode = 0;               // Toggle the mode
   }
   else
   {
      output_low(RF_MOD);     // No modulation
      CCP_2 += 34;            // Wait for half the bit period plus 2 RF periods before sending another bit
      ++dataTransferred;      // Increment the bits transferred counter
      mode = 1;               // Toggle the mode
   }
}


// Purpose:       Interrupt for timer 0. Keeps track of the number of
//                overflows for timeouts.
// Inputs:        None
// Outputs:       None
#INT_RTCC
void isr_rtcc()
{
   ++timer0_overflows;
}


// Purpose:       Fill the buffer with data read from the basestation
// Inputs:        1) The number of bits to read
//                2) TRUE start on rising edge
//                   FALSE start on falling edge
// Outputs:       The number of bits read. Could be used to check for timeout
int8 RF_get(int8 numBits, int1 edge)
{
   RF_readEdge(edge);
   RF_readMode = RF_MANCHESTER_DATA;

   storeData         = TRUE;
   bitValue          = 0;
   RFbuffer_index    = 0;
   RFbuffer_bitIndex = 0;
   dataTransferred   = 0;
   timer0_overflows  = 0;
   old_clock         = 0;
   set_timer1(0);

   clear_interrupt(INT_CCP1);
   enable_interrupts(INT_CCP1);
   while(dataTransferred < numBits && timer0_overflows < 15);
   disable_interrupts(INT_CCP1);

   RFbuffer_index = 0;
   RFbuffer_bitIndex = 0;

   return dataTransferred;
}


// Purpose:       Send data from the buffer to the transponder
// Inputs:        1) Send numBits of data to the transponder
//                2) The index in the buffer to start at
//                3) The bit position at the index to start at
// Outputs:       None
void RF_send(int8 numBits, int8 index, int8 bitPosition)
{
   RFbuffer_index    = index;
   RFbuffer_bitIndex = bitPosition;
   dataTransferred   = 0;
   CCP_2             = 3;  //
   set_timer1(0);          // Cause an interrupt imediately

   enable_interrupts(INT_CCP2);
   while(dataTransferred < numBits);
   disable_interrupts(INT_CCP2);
}


// Purpose:       Search for a certain pulse width
// Inputs:        1) The width length in clocks
//                2) Uncertainty to search over a range
//                3) TRUE start on rising edge
//                   FALSE start on falling edge
//                ex) numClocks = 128; uncertainty = 6; range = 122 to 134
// Outputs:       TRUE if width was found, FALSE if not found
int1 RF_findWidth(int8 numClocks, int8 uncertainty, int1 edge)
{
   RF_readEdge(edge);

   RF_readMode          = RF_FIND_WIDTH;
   RF_widthToFind       = numClocks;
   RF_widthFound        = FALSE;
   RF_uncertainty       = uncertainty;
   timer0_overflows     = 0;
   old_clock            = 0;
   set_timer1(0);

   clear_interrupt(INT_CCP1);
   enable_interrupts(INT_CCP1);
   while(RF_widthFound == FALSE && timer0_overflows < 50);
   disable_interrupts(INT_CCP1);

   return RF_widthFound;
}


// Purpose:    Measure a number of pulse widths, both high and low
// Inputs:     1) The number of widths to measure
//             2) TRUE start on rising edge
//                FALSE start on falling edge
// Outputs:    The number of widths that were measured. If there is
//             no transponder in range, the timeout could occur.
int8 RF_measureWidths(int8 numWidths, int1 edge)
{
   RF_readEdge(edge);

   RF_readMode       = RF_MEASURE_WIDTHS;
   dataTransferred   = 0;
   RFbuffer_index    = 0;
   timer0_overflows  = 0;
   old_clock         = 0;
   set_timer1(0);

   clear_interrupt(INT_CCP1);
   enable_interrupts(INT_CCP1);
   while(dataTransferred < numWidths && timer0_overflows < 50);
   disable_interrupts(INT_CCP1);

   return dataTransferred;
}

// Purpose:    Measure a number of pulse widths, both high and low
// Inputs:     1) A pointer to an array of widths. It is safe to use RFbuffer.
//             2) The number of widths in the pattern
//             3) Uncertainty to search over a range
//             4) TRUE start on rising edge
//                FALSE start on falling edge
// Outputs:    The number of widths that were measured. If there is
//             no transponder in range, the timeout could occur.
int8 RF_findPattern(int8* widths, int8 numWidths, int8 uncertainty, int1 edge)
{
   RF_readEdge(edge);

   RF_readMode       = RF_FIND_PATTERN;
   RF_patternFound   = FALSE;
   RFbuffer_index    = 0;
   RF_findWidths     = widths;
   dataTransferred   = numWidths;
   RF_uncertainty    = uncertainty;
   timer0_overflows  = 0;
   old_clock         = 0;
   set_timer1(0);

   clear_interrupt(INT_CCP1);
   enable_interrupts(INT_CCP1);
   while(RF_patternFound == FALSE && timer0_overflows < 40);
   disable_interrupts(INT_CCP1);

   return RF_patternFound;
}


// Purpose:       Set every byte in the buffer to data
// Inputs:        None
// Outputs:       None
void RFbuffer_fill(int8 data)
{
   int i;

   for(i=0; i<sizeof(RFbuffer); ++i)
   {
      RFbuffer[i] = data;
   }
}


// Purpose:       Inverts every byte in the buffer
// Inputs:        None
// Outputs:       None
void RFbuffer_invert()
{
   int i;

   for(i=0; i<sizeof(RFbuffer); ++i)
   {
      RFbuffer[i] = ~RFbuffer[i];
   }
}


// Purpose:       Get a bit of data from the buffer and increment to the next bit
// Inputs:        None
// Ouputs:        A bit of data
int1 RFbuffer_getBit()
{
   int1 bit;

   if(!END_OF_RFBUFFER)
   {
      bit = bit_test(RFbuffer[RFbuffer_index], RFbuffer_bitIndex);

      if(++RFbuffer_bitIndex == 8)
      {
         ++RFbuffer_index;
         RFbuffer_bitIndex = 0;
      }
   }

   return bit;
}


// Purpose:       Get a byte of data from the buffer
// Inputs:        None
// Outputs:       The byte of data
int8 RFbuffer_getByte()
{
   if(!END_OF_RFBUFFER)
   {
      int8 i;
      int8 data;

      for(i=0; i<8; ++i)
      {
         shift_right(&data, 1, RFbuffer_getBit());
      }

      return data;
   }
}


// Purpose:       Set the value of the next bit in the buffer
// Inputs:        None
// Outputs:       None
void RFbuffer_setBit(int1 bit)
{
   if(!END_OF_RFBUFFER)
   {
      if(bit)
      {
         bit_set(RFbuffer[RFbuffer_index], RFbuffer_bitIndex);
      }
      else
      {
         bit_clear(RFbuffer[RFbuffer_index], RFbuffer_bitIndex);
      }

      if(++RFbuffer_bitIndex >= 8)
      {
         ++RFbuffer_index;
         RFbuffer_bitIndex = 0;
      }
   }
}

// Purpose:       Set the value of the next byte in the buffer
// Inputs:        None
// Outputs:       None
void RFbuffer_setByte(int8 data)
{
   if(!END_OF_RFBUFFER)
   {
      int8 i;
      for(i=0; i<8; ++i)
      {
         RFbuffer_setBit(bit_test(data, 7));
         rotate_left(&data, 1);
      }
   }
}

#endif
