///////////////////////////////////////////////////////////////////////////
////   Library for the N9085UD VCR / TV remote                         ////
////                                                                   ////
////   * Note: This driver uses timer one to decode a signal. Edit the ////
////           line "#define TIMER_1_DIV" to configure timer one       ////
////           division.                                               ////
////                                                                   ////
////   init_IRremote()                                                 ////
////     Call before the readIRremote() function is used               ////
////                                                                   ////
////   int1 readIRremote(int8& button, int1 wait)                      ////
////     Read a button press from the IR remote                        ////
////     * Will wait forever if TRUE is passed in,                     ////
////       or will wait for one command period if FALSE                ////
////     * Returns a button code in button. See the code listing below ////
////     * Returns the repeat bit from the signal. This alternates     ////
////       between 0 and 1 for each button press.                      ////
////                                                                   ////
////   The main program may define IR_REMOTE_PIN to override           ////
////   the default below.                                              ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2003 Custom Computer Services          ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////


#ifndef _N9085UD
#define _N9085UD

#ifndef IR_REMOTE_PIN
#define IR_REMOTE_PIN   PIN_C1
#endif

///////////////////////////////////////////////////////////////////////////
// The following allows the signal scan to work for different
// timer 1 division settings.
///////////////////////////////////////////////////////////////////////////
#ifndef TIMER_1_DIV
#define TIMER_1_DIV     2           // Number of divisions for timer 1
#endif
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
// The following show the hexadecimal values returned by readIRremote()
// and which button the value cooresponds with.
///////////////////////////////////////////////////////////////////////////
/////////////////////////
////   Error Code    ////
#define VCR_TV_ERR   0xFF
/////////////////////////

/////////////////////////
////   VCR Buttons   ////
#define VCR_ZERO     0x00
#define VCR_ONE      0x01
#define VCR_TWO      0x02
#define VCR_THREE    0x03
#define VCR_FOUR     0x04
#define VCR_FIVE     0x05
#define VCR_SIX      0x06
#define VCR_SEVEN    0x07
#define VCR_EIGHT    0x08
#define VCR_NINE     0x09

#define VCR_POWER    0x0C
#define VCR_EJECT    0x2D
#define VCR_TV       0x3E
#define VCR_CHANUP   0x20
#define VCR_CHANDN   0x21
#define VCR_VCRPLUS  0x85
#define VCR_PLUS100  0x0A
#define VCR_SLOW     0x28
#define VCR_SPEED    0x3A
#define VCR_MENU     0x1D
#define VCR_STATUS   0x0F
#define VCR_CLEAR    0x31
#define VCR_MEMORY   0x3B
#define VCR_REWIND   0x32
#define VCR_PLAY     0x35
#define VCR_FASTFWD  0x34
#define VCR_REC_OTR  0x37
#define VCR_STOP     0x36
#define VCR_PAUSE    0x29
/////////////////////////

/////////////////////////
////   TV Buttons    ////
#define TV_POWER     0x4C
#define TV_CHANUP    0x60
#define TV_CHANDN    0x61
#define TV_VOLUP     0x50
#define TV_VOLDN     0x51
#define TV_MUTE      0x4D
/////////////////////////
///////////////////////////////////////////////////////////////////////////


// The timer 1 overflow count is used for timeouts
int8 timer1_overflows;

// Function prototypes
int1 IR_scanSignal(int16* width_buffer, int1 wait);
int8 IR_decodeData(int8& button, int16* width_buffer);


#ifndef TIMER_RATE
#define TIMER_RATE      getenv("clock") / 4 / TIMER_1_DIV
#endif

#define OVERFLOW_LIMIT  (int)((float)TIMER_RATE / 65535.0 * 0.14 + 1.0)


// Purpose:    Initialize timer 1 and enable timer 1 interrupt for timeouts
void init_IRremote()
{
   #if TIMER_1_DIV == 1
      setup_timer_1(T1_DIV_BY_1 | T1_INTERNAL);
   #elif TIMER_1_DIV == 2
      setup_timer_1(T1_DIV_BY_2 | T1_INTERNAL);
   #elif TIMER_1_DIV == 4
      setup_timer_1(T1_DIV_BY_4 | T1_INTERNAL);
   #elif TIMER_1_DIV == 8
      setup_timer_1(T1_DIV_BY_8 | T1_INTERNAL);
   #endif

   enable_interrupts(INT_TIMER1);
   enable_interrupts(GLOBAL);
}


// Purpose:    Read a button press from an IR remote
// Inputs:     TRUE wait until a signal is received
//             FALSE wait for one command period, then return
// Outputs:    A button code. See the define listing above.
int1 readIRremote(int8& button, int1 wait)
{
   // A buffer to hold pulse widths
   int16 width_buffer[24];

   if(IR_scanSignal(width_buffer, wait))
   {
      button = VCR_TV_ERR;
      return 0;
   }
   else
   {
      return IR_decodeData(button, width_buffer);
   }
}


// Purpose:    Interrupt service routine for timer 1 overflow
//             Increments an overflow counter
#INT_TIMER1
void isr_timer1()
{
   ++timer1_overflows;
}


// Purpose:       Waits for an IR signal, then measures the pulse widths
// Inputs:        1) A pointer to a buffer to fill with pulse widths
//                2) TRUE to wait for a button. FALSE to allow for timeout
// Outputs:       0 if a scan was successful, 1 if the scan failed
int1 IR_scanSignal(int16* width_buffer, int1 wait)
{
   int16  rise;
   int16  fall;
   int16* index;
   int8   i;

   index = width_buffer;

   // Reset the timeout to 0
   timer1_overflows = 0;

   // Find long high pulse
   do
   {
      while(!input(IR_REMOTE_PIN) && (timer1_overflows < OVERFLOW_LIMIT*2));
      rise = get_timer1();
      while(input(IR_REMOTE_PIN) && (timer1_overflows < OVERFLOW_LIMIT*2));
      fall = get_timer1();

      if(!wait && (timer1_overflows > OVERFLOW_LIMIT))
      {
         return 1;
      }
   } while((fall - rise) < (TIMER_RATE / 250));

   // Reset the timeout to 0
   timer1_overflows = 0;

   // Measure 24 pulse widths
   for(i = 0; i < 12; ++i)
   {
      // Wait for signal to rise. A timeout prevents an infinite loop.
      while(!input(IR_REMOTE_PIN) && timer1_overflows < OVERFLOW_LIMIT);
      rise = get_timer1();
      *index = rise - fall;
      ++index;

      // Wait for signal to fall. A timeout prevents an infinite loop.
      while(input(IR_REMOTE_PIN) && timer1_overflows < OVERFLOW_LIMIT);
      fall = get_timer1();
      *index = fall - rise;
      ++index;
   }

   // If a timeout occurred, then return with error
   if(timer1_overflows > OVERFLOW_LIMIT)
   {
      return 1;
   }

   // Set the index pointer back to the beginning of the buffer
   index = width_buffer;

   // Sometimes an IR sensor pulses sporadically
   // The following loop ensures good data
   for(i = 0; i < 17; ++i, ++index)
   {
      if(*index > (TIMER_RATE / 521) || *index < (TIMER_RATE / 1330))
      {
         return 1;
      }
   }

   // Return no error
   return 0;
}


// Purpose:       Decode Manchester encoded command and button number
// Inputs:        A pointer to a buffer containing pulse widths
// Outputs:       A button code. See the define listing above
int1 IR_decodeData(int8& button, int16* width_buffer)
{
   int1 bit_value = 1;
   int8 data = 0;
   int8 cmd = 0;
   int1 save_bit = 0;
   int8 bitCount = 0;

   // Decode 13 bits from the Manchester encoded data
   while(bitCount < 13)
   {
      // Invert the bit value and save if the phase changed
      if(*width_buffer > (TIMER_RATE / 781))
      {
         save_bit = TRUE;
         bit_value = ~bit_value;
      }

      if(save_bit)
      {
         // The first 7 bits tell TV, VCR, or VCR PLUS+
         // and the next 6 tell which button was pressed
         if(bitCount < 7)
         {
            shift_left(&cmd, 1, bit_value);
         }
         else
         {
            shift_left(&data, 1, bit_value);
         }

         ++bitCount;
      }

      save_bit = ~save_bit;
      ++width_buffer;
   }

   bit_value = bit_test(cmd, 5);

   // Find the command type
   switch(cmd & 0xDF)
   {
      case 0x45:
         cmd = 0x00;    // VCR command
         break;
      case 0x40:
         cmd = 0x40;    // TV command
         break;
      case 0x05:
         cmd = 0x80;    // VCR PLUS+
         break;
   }

   button = data | cmd;
   return bit_value;
}

#endif
