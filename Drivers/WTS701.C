///////////////////////////////////////////////////////////////////////////
////          Library for a WTS701 text to speech converter            ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////////////////////////////// Basic Commands /////////////////////////////
///////////////////////////////////////////////////////////////////////////
////  tts_init()                                                       ////
////     Call before any other function is used                        ////
////                                                                   ////
////  tts_sendText(char* text)                                         ////
////     Sends a NULL terminated string to the text to speech chip.    ////
////                                                                   ////
////  tts_powerDown()                                                  ////
////     Power down the text to speech chip                            ////
////     Call tts_init() to initialize and power up again              ////
////                                                                   ////
////  tts_reset()                                                      ////
////     Resets the chip. After reset the chip enters power down mode  ////
////     Call tts_init() to initialize and power up again              ////
////                                                                   ////
////  tts_idle()                                                       ////
////     Puts the text to speech chip in idle mode                     ////
////                                                                   ////
////  tts_pause()                                                      ////
////     Pauses the current conversion                                 ////
////                                                                   ////
////  tts_resume()                                                     ////
////     Resumes a paused conversion                                   ////
////                                                                   ////
////  tts_stop()                                                       ////
////     Stop the current conversion and clears the buffer             ////
////                                                                   ////
////  tts_finishWord()                                                 ////
////     Finishes speaking the current word then stops                 ////
////                                                                   ////
////  tts_finishBuffer()                                               ////
////     Finishes speaking the words in the buffer then stops          ////
////                                                                   ////
////  tts_volumeUp()                                                   ////
////     Increases the volume                                          ////
////                                                                   ////
////  tts_volumeDown()                                                 ////
////     Decreases the volume                                          ////
////                                                                   ////
////  tts_setVolume(int8 volume)                                       ////
////     Set the volume level from 0 - 7 with 0 being maximum volume   ////
////                                                                   ////
////  int8 tts_getVolume()                                             ////
////     Get the volume level                                          ////
////                                                                   ////
////  tts_speedUp()                                                    ////
////     Increases the speech rate                                     ////
////                                                                   ////
////  tts_speedDown()                                                  ////
////     Decreases the speech rate                                     ////
////                                                                   ////
////  tts_setSpeed(int8 speed)                                         ////
////     Set the speech rate from 0 - 4 with 4 being the fastest       ////
////                                                                   ////
////  int8 tts_getSpeed()                                              ////
////     Get the speech rate                                           ////
////                                                                   ////
////  tts_setPitch(pitch)                                              ////
////     Set the speech pitch from 0 - 6 with 6 being lowest           ////
////                                                                   ////
////  tts_waitConversion()                                             ////
////     Waits for the current text to be spoken                       ////
////                                                                   ////
////  int16 tts_getStatus(int1 clearInterruptFlags)                    ////
////     Read the status register                                      ////
////     Inputs:  1 - The interrupt flags are cleared                  ////
////              0 - The interrupt flags are unchanged                ////
////                                                                   ////
////  int16 tts_getByteCount()                                         ////
////     Read the value in the byte count register.                    ////
////                                                                   ////
////  int16 tts_getVersion()                                           ////
////     Read the hardware and software version on the tts chip        ////
////     Output: The upper byte has the hardware version and           ////
////             the lower byte has the software version.              ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
//////////////////////////// Advanced Commands ////////////////////////////
///////////////////////////////////////////////////////////////////////////
////  tts_powerUp()                                                    ////
////     Powers up the text to speech chip. The same initialization    ////
////     sequence in tts_init() must occur.                            ////
////                                                                   ////
////  tts_setCOM(int8 com)                                             ////
////   * The following may be ORed together to set the COM register    ////
////     TTS_INT_COUNT      Interrupt after each spoken word           ////
////     TTS_INT_BUFFER     Interrupt when buffer crosses the threshold////
////     TTS_INT_CONVERT    Interrupt when a conversion is completed   ////
////     TTS_BUFF0          Set buffer threshold to 0%                 ////
////     TTS_BUFF10         Set buffer threshold to 10%                ////
////     TTS_BUFF50         Set buffer threshold to 50%                ////
////     TTS_BUFF75         Set buffer threshold to 75%                ////
////                                                                   ////
////  int8 tts_getCOM()                                                ////
////     Read the value of the COM register                            ////
////                                                                   ////
////  tts_setCodec(int8 codec)                                         ////
////   * The following may be ORed together to set the codec register  ////
////     TTS_DISABLE_CODEC  The codec is disabled                      ////
////     TTS_ENABLE_CODEC   The codec is enables during conversion     ////
////     TTS_PRECISION13    13 bit linear PCM output                   ////
////     TTS_PRECISION16    16 bit linear PCM output                   ////
////     TTS_PCM_UNSIGNED   Unsigned PCM output                        ////
////     TTS_PCM_2sCOMP     2's compliment PCM output                  ////
////                                                                   ////
////  int8 tts_getCodec()                                              ////
////     Read the value of the Codec register                          ////
////                                                                   ////
////  tts_setAudio(int8 audio)                                         ////
////   * The following may be ORed together to set the audio register  ////
////     TTS_BUFFER_ON      Power up the analog output buffer          ////
////     TTS_SPEAKER_ON     Power up the analog speaker driver         ////
////     TTS_SPKR_GAIN132   Analog voltage is 1.32 for 8 Ohm speakers  ////
////     TTS_SPKR_GAIN160   Analog voltage is 1.60 for 100 Ohm speakers////
////     TTS_FEED_THROUGH   Enable AUXIN to AUXOUT feed through        ////
////     TTS_AUXIN_GAIN0    No AUXIN gain                              ////
////     TTS_AUXIN_GAIN3    3 dB AUXIN gain                            ////
////     TTS_AUXIN_GAIN6    6 dB AUXIN gain                            ////
////     TTS_AUXIN_GAIN9    9 dB AUXIN gain                            ////
////                                                                   ////
////  int8 tts_getAudio()                                              ////
////     Read the value of the audio register                          ////
////                                                                   ////
////  tts_setClock(int8 clock)                                         ////
////     TTS_CLK_24576      Oscillator is 24.576 MHz (Recommended)     ////
////     TTS_CLK_16384      Oscillator is 16.384 MHz                   ////
////     TTS_CLK_32768      Oscillator is 32.768 MHz                   ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////////////////////// Abbreviation Table Commands ////////////////////////
///////////////////////////////////////////////////////////////////////////
////  tts_getAvailableAbbrMem()                                        ////
////     Get the number of bytes of available abbreviation memory      ////
////                                                                   ////
////  tts_getNumAbbrEntries()                                          ////
////     Get the number of active abbreviations                        ////
////                                                                   ////
////  tts_storeAbbreviation()                                          ////
////     Transfer an abbreviation to flash. Only call this function    ////
////     after an abbreviation has been sent.                          ////
////     Used by tts_addAbbrEntry()                                    ////
////                                                                   ////
////  tts_addAbbrEntry(char* abbr, char* text)                         ////
////     Add an abbreviation entry to the user table                   ////
////     Inputs:     1) A NULL terminated string containing            ////
////                    the abbreviation                               ////
////                 2) A NULL terminated string containing the text   ////
////                                                                   ////
////  tts_delAbbrEntry(char* abbr)                                     ////
////     Delete an abbreviation entry from the user table              ////
////       Inputs:      A NULL terminated string containing            ////
////                    the abbreviation to delete                     ////
////                                                                   ////
////  tts_startAbbrTableRead()                                         ////
////     Send the command to start reading the abbreviation table.     ////
////     Once this command has been sent, the entire 2048 byte         ////
////     abbreviation table must be read.                              ////
////     Call tts_continueAbbrTableRead(int8& data) to read            ////
////     consecutive bytes from the abbreviation table. Do not call    ////
////     other tts functions until the entire table has been read.     ////
////     See description of tts_continueAbbrTableRead(int8& data)      ////
////     See tts_readAbbrTable() for example                           ////
////                                                                   ////
////  int1 tts_continueAbbrTableRead(int8& data)                       ////
////     Read a byte of data from the abbreviation table. Only call    ////
////     this function after tts_startAbbrTableRead() has been called. ////
////     Inputs:     A reference to an 8 bit integer                   ////
////     Outputs:    0 if the entire table has been read               ////
////                 1 if more reading is required                     ////
////     Notes:      Abbreviation table format                         ////
////      1) Marker: TTS_ACTIVE_ABBR for active abbreviation    (0xFE) ////
////                 TTS_DELETED_ABBR for deleted abbreviation  (0xFC) ////
////      2) Count:  Byte count (Marker + Count + XXX + YYYY + NULLs)  ////
////      3) XXX:    Abbreviation characters. Example: "TTS"           ////
////      4) NULL:   0x00                                              ////
////      5) YYYY:   Abbreviation text. Example: "Text to speech"      ////
////      6) NULL:   0x00                                              ////
////     See tts_readAbbrTable() for example                           ////
////                                                                   ////
////  tts_readAbbrTable()                                              ////
////     Example function for reading the abbreviation table.          ////
////     This function is meant to be modified for custom use.         ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////  See the end of the file for an interrupt service routine example ////
///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////  The main program may define TTS_INT, TTS_RB, TTS_SS, TTS_SCLK,   ////
////  TTS_MISO, and TTS_MOSI to override the defaults below.           ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2004 Custom Computer Services          ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#ifndef WTS701
#define WTS701

////////////////////////////////////////////////////////////////////////////////
// PIN Connections
////////////////////////////////////////////////////////////////////////////////
#ifndef TTS_INT
#define TTS_INT          PIN_B0   // Interrupt signal. Low means interrupt occurred
#define TTS_RB           PIN_B1   // Ready/Busy signal. High is ready, low is busy
#define TTS_SS           PIN_C0   // Serial select line
#define TTS_SCLK         PIN_C3   // Serial clock line
#define TTS_MISO         PIN_C4   // Master in, slave out
#define TTS_MOSI         PIN_C5   // Master out, slave in
#endif
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Command definitions
////////////////////////////////////////////////////////////////////////////////
// Status Commands
#define TTS_RDST         0x04     // Read Status Register
#define TTS_RINT         0x06     // Read Status Register and Clear Interrupt Flags
#define TTS_RVER         0x12     // Read Device Version

// System Comands
#define TTS_PWUP         0x02     // Power Up
#define TTS_PWDN         0x40     // Power Down
#define TTS_RST          0x10     // Reset
#define TTS_IDL          0x57     // Go Idle

// Synthesis Commands
#define TTS_CONV         0x81     // Start Conversion
#define TTS_PAUS         0x49     // Pause Conversion
#define TTS_RES          0x4A     // Resume Conversion
#define TTS_ST           0x4B     // Stop Conversion
#define TTS_FINW         0x4D     // Finish Word
#define TTS_FIN          0x4C     // Finish Buffer
#define TTS_VLUP         0x53     // Volume Up
#define TTS_VLDN         0x54     // Volume Down
#define TTS_SPUP         0x55     // Speed Up Conversion
#define TTS_SPDN         0x56     // Slow Down Conversion

// Configuration Commands
#define TTS_RREG         0xC0     // Read Configuration Register
#define TTS_SCOM         0x4E     // COM Configuration Register
#define TTS_SCOD         0x4F     // CODEC Configuration Register
#define TTS_SAUD         0x50     // AUDIO Configuration Register
#define TTS_SVOL         0x51     // VOL Configuration Register
#define TTS_SSPD         0x52     // SPEED Configuration Register
#define TTS_SCLC         0x14     // CLC (Clock) Configuration Register
#define TTS_SPTC         0x77     // Set Speech Pitch

// Customization Commands
#define TTS_ABBR_NUM     0xC8     // Get number of abbreviation entries
#define TTS_ABBR_RD      0xC9     // Read abbreviation table
#define TTS_ABBR_MEM     0xC7     // Get number of free bytes
#define TTS_ABBR_ADD     0xAF     // Add abbreviation entry
#define TTS_ABBR_DEL     0x83     // Delete abbreviation entry
#define TTS_ENTER_RRSM   0x0C     // Swap memory
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
#define PHONEME_TAG      '\n'     // Signifies the start of a phoneme (0x10)
#define PHONEME_END      ' '      // Signifies the end of a phoneme   (0x20)
#define WORD_TAG_AND_END ' '      // Wrap around a group of characters to
                                  // signify a spoken word

#define TTS_ACTIVE_ABBR     0xFE  // Flag for active abbreviation
#define TTS_DELETED_ABBR    0xFC  // Flag for deleted abbreviation
#define TTS_ABBR_TABLE_SIZE 2048  // Size of the user abbreviation table
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Global Variables
////////////////////////////////////////////////////////////////////////////////
int16 tts_AbbrReadCount;
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Some Function Prototypes
////////////////////////////////////////////////////////////////////////////////
void  tts_start();
#inline
void  tts_end();
void  tts_sendByte(int8 data);
int8  tts_readByte();
int16 tts_read16();
int16 tts_getStatus(int1 clearInterruptFlags);
void  tts_sendCmd(int8 cmd, int8 cmdData);
void  tts_sendChar(char c);
#inline
void  tts_waitforReadyPin();
void  tts_abbr(int8 cmd, char* abbreviation, char* text);
void  tts_init();
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Function Implementations
////////////////////////////////////////////////////////////////////////////////
// Purpose:    Send a null terminated string to the text to speech chip.
// Inputs:     A pointer to the null terminated string
// Outputs:    None
void tts_sendText(char* text)
{
   // Setup for sending a command
   tts_start();

   // Send the conversion command
   tts_sendByte(TTS_CONV);
   tts_sendByte(0);

   // Send the start word character
   tts_sendChar(WORD_TAG_AND_END);

   // Loop until the null character is reached
   while(*text != '\0')
   {
      // Send a character and increment the pointer
      tts_sendChar(*text++);
   }

   // Send the end word character
   tts_sendChar(WORD_TAG_AND_END);

   // Cleanup from sending a command
   tts_end();

   tts_waitForReadyPin();

   // Send the finish conversion command
   tts_sendCmd(TTS_FIN, 0);
}


// Purpose:    Read the status register
// Inputs:     1 - The interrupt flags are cleared
//             0 - The interrupt flags are unchanged
int16 tts_getStatus(int1 clearInterruptFlags)
{
   int8  i;
   int8  cmd;
   int16 status;

   // Check which command should be sent
   if(clearInterruptFlags)
   {
      cmd = TTS_RINT;
   }
   else
   {
      cmd = TTS_RDST;
   }

   // Setup for sending a command
   tts_start();

   // Send the command and retrieve the status register
   for(i=0; i<32; ++i)
   {
      output_low(TTS_SCLK);
      output_bit(TTS_MOSI, shift_left(&cmd, 1, 0));
      delay_us(1);
      output_high(TTS_SCLK);
      if(i < 16)
      {
         shift_left(&status, 2, input(TTS_MISO));
      }
   }

   // Cleanup from sending a command
   tts_end();

   // Return the status register
   return status;
}


// Purpose:    Waits for a conversion to complete
//             (Until the text in the buffer has been spoken).
void tts_waitConversion()
{
   while(tts_getStatus(0) & 0x0200);
}


// Purpose:    Basic function for executing a variety of commands.
// Inputs:     1) The command to send
//             2) The command data byte
// Outputs:    16 bits of data read after the command and command data were sent
int16 tts_sendCmdGet16Bits(int8 cmd, int8 data)
{
   int16 read;

   // Setup for sending a command
   tts_start();

   // Send the command
   tts_sendByte(cmd);

   // Send the command data
   tts_sendByte(data);

   // Read 16 bits of data
   read = tts_read16();

   // Cleanup from sending a command
   tts_end();

   // Return the 16 bits of data read
   return read;
}


// Purpose:    Read the value in the byte count register.
// Outputs:    A 16 bit integer containing the byte count.
#define tts_getByteCount()      tts_sendCmdGet16Bits(TTS_RDST, 0)


// Purpose:    Read the hardware and software version on the text to speech chip
// Outputs:    A 16 bit integer. The upper byte has the hardware version and
//             the lower byte has the software version.
#define tts_getVersion()      tts_sendCmdGet16Bits(TTS_RVER, 0)


////////////////////////////////////////////////////////////////////////////////
// System functions
////////////////////////////////////////////////////////////////////////////////
// Purpose:    Power up the text to speech chip
// * NOTE:     Use tts_init() instead of tts_powerUp(),
//             or follow the same initialization sequence
//             with different values in the registers.
#define tts_powerUp()         tts_sendCmd(TTS_PWUP,  0)


// Purpose:    Power down the text to speech chip
#define tts_powerDown()       tts_sendCmd(TTS_PWDN,  0)


// Purpose:    Reset the text to speech chip.
//             The chip will enter power down state after reset
#define tts_reset()           tts_sendCmd(TTS_RST,   0)


// Purpose:    Put the chip into idle mode. The chip is active
//             and ready to accept commands.
#define tts_idle()            tts_sendCmd(TTS_IDL,  0)
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Synthesis functions
////////////////////////////////////////////////////////////////////////////////
// Purpose:    Pause the current text to speech conversion
#define tts_pause()           tts_sendCmd(TTS_PAUS, 0)


// Purpose:    Resume a paused conversion
#define tts_resume()          tts_sendCmd(TTS_RES,  0)


// Purpose:    Stops the current conversion and clears the buffer
#define tts_stop()            tts_sendCmd(TTS_ST,   0)


// Purpose:    Finish speaking the current word then stop the conversion
#define tts_finishWord()      tts_sendCmd(TTS_FINW, 0)


// Purpose:    Finish speaking the contents of the buffer then stop
#define tts_finishBuffer()    tts_sendCmd(TTS_FIN,  0)


// Purpose:    Increases the volume
#define tts_volumeUp()        tts_sendCmd(TTS_VLUP, 0)


// Purpose:    Decreases the volume
#define tts_volumeDown()      tts_sendCmd(TTS_VLDN, 0)


// Purpose:    Increases the rate of speech
#define tts_speedUp()         tts_sendCmd(TTS_SPUP, 0)


// Purpose:    Decreases the rate of speech
#define tts_speedDown()       tts_sendCmd(TTS_SPDN, 0)
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Configuration functions
////////////////////////////////////////////////////////////////////////////////
// Purpose:    Configure the communications register
// Inputs:     The following may be ORed together to set the COM register
#define TTS_INT_COUNT      0x80  // Interrupt after each spoken word
#define TTS_INT_BUFFER     0x40  // Interrupt when buffer crosses the threshold
#define TTS_INT_CONVERT    0x20  // Interrupt when a conversion is completed
#define TTS_BUFF0          0x00  // Set buffer threshold to 0%
#define TTS_BUFF10         0x01  // Set buffer threshold to 10%
#define TTS_BUFF50         0x02  // Set buffer threshold to 50%
#define TTS_BUFF75         0x03  // Set buffer threshold to 75%
#define tts_setCOM(com)       tts_sendCmd(TTS_SCOM, com)


// Purpose:    Get the COM configuration register
// Outputs:    An 8 bit integer containing the register's value
#define tts_getCOM()          tts_sendCmdGet16Bits(TTS_RREG, TTS_SCOM)


// Purpose:    Configure the codec register
// Inputs:     The following may be ORed together to set the codec register
#define TTS_DISABLE_CODEC  0x00  // The codec is disabled
#define TTS_ENABLE_CODEC   0x04  // The codec is enables during conversion
#define TTS_PRECISION13    0x00  // 13 bit linear PCM output
#define TTS_PRECISION16    0x02  // 16 bit linear PCM output
#define TTS_PCM_UNSIGNED   0x00  // Unsigned PCM output
#define TTS_PCM_2sCOMP     0x01  // 2's compliment PCM output
#define tts_setCodec(codec)   tts_sendCmd(TTS_SCOD, codec)


// Purpose:    Get the Codec configuration register
// Outputs:    An 8 bit integer containing the register's value
#define tts_getCodec()        tts_sendCmdGet16Bits(TTS_RREG, TTS_SCOD)


// Purpose:    Configure the audio register
// Inputs:     The following may be ORed together to set the audio register
#define TTS_BUFFER_ON      0x80  // Power up the analog output buffer
#define TTS_SPEAKER_ON     0x40  // Power up the analog speaker driver
#define TTS_SPKR_GAIN132   0x00  // Analog voltage is 1.32 for 8 Ohm speakers
#define TTS_SPKR_GAIN160   0x20  // Analog voltage is 1.60 for 100 Ohm speakers
#define TTS_FEED_THROUGH   0x10  // Enable AUXIN to AUXOUT feed through
#define TTS_AUXIN_GAIN0    0x00  // No AUXIN gain
#define TTS_AUXIN_GAIN3    0x01  // 3 dB AUXIN gain
#define TTS_AUXIN_GAIN6    0x02  // 6 dB AUXIN gain
#define TTS_AUXIN_GAIN9    0x03  // 9 dB AUXIN gain
#define tts_setAudio(audio)   tts_sendCmd(TTS_SAUD, audio)


// Purpose:    Get the Audio configuration register
// Outputs:    An 8 bit integer containing the register's value
#define tts_getAudio()        tts_sendCmdGet16Bits(TTS_RREG, TTS_SAUD)


// Purpose:    Set the volume level of the output
// Inputs:     0 - 7 with 0 being max volume
#define tts_setVolume(volume) tts_sendCmd(TTS_SVOL, volume)


// Purpose:    Get the volume
// Outputs:    An 8 bit integer containing the volume (0 - 7)
#define tts_getVolume()       tts_sendCmdGet16Bits(TTS_RREG, TTS_SVOL)


// Purpose:    Set the speech rate
// Inputs:     0 - 4 where 4 is the fastest rate
#define tts_setSpeed(speed)   tts_sendCmd(TTS_SSPD, speed)


// Purpose:    Get the speech rate
// Outputs:    An 8 bit integer containing the speech rate (0 - 4)
#define tts_getSpeed()        tts_sendCmdGet16Bits(TTS_RREG, TTS_SSPD)


// Purpose:    Tell the chip the frequency of the oscillator being used
// Inputs:     Use one of the following to set a clock frequency
#define TTS_CLK_24576      0x00  // Oscillator is 24.576 MHz
#define TTS_CLK_16384      0x10  // Oscillator is 16.384 MHz
#define TTS_CLK_32768      0x08  // Oscillator is 32.768 MHz
#define tts_setClock(clock)   tts_sendCmd(TTS_SCLC, clock)


// Purpose:    Set the speech pitch when TTS_IDLE
// Inputs:     0 - 6
#define tts_setPitch(pitch)   tts_sendCmd(TTS_SPTC, pitch)


////////////////////////////////////////////////////////////////////////////////
// Abbreviation table functions
////////////////////////////////////////////////////////////////////////////////
// Purpose:    Get the number of bytes of available abbreviation memory
// Inputs:     None
// Outputs:    None
#define tts_getAvailableAbbrMem()      tts_sendCmdGet16Bits(TTS_ABBR_MEM, 0)


// Purpose:    Get the number of active abbreviations
// Inputs:     None
// Outputs:    None
#define tts_getNumAbbrEntries()        tts_sendCmdGet16Bits(TTS_ABBR_NUM, 0)


// Purpose:    Transfer an abbreviation to flash. Only call this function
//             after an abbreviation has been sent.
// Inputs:     None
// Outputs:    None
#define tts_storeAbbreviation()        tts_sendCmd(TTS_ENTER_RRSM, 0)


// Purpose:    Add an abbreviation entry.
// Inputs:     1) A NULL terminated string containing the abbreviation
//             2) A NULL terminated string containing the text
// Outputs:    None
#define tts_addAbbrEntry(abbr, text)   tts_abbr(TTS_ABBR_ADD, abbr, text)


// Purpose:    Delete an abbreviation entry
// Inputs:     A NULL terminated string containing the abbreviation
// Outputs:    None
#define tts_delAbbrEntry(abbr)         tts_abbr(TTS_ABBR_DEL, abbr, 0)


// Purpose:    Send the command to start reading the abbreviation table. Once
//             this command has been sent, the entire 2048 byte abbreviation
//             table must be read. Call tts_continueAbbrTableRead(int8& data)
//             to read successive bytes from the abbreviation table. Do not call
//             other tts functions until the entire table has been read.
//             See description of tts_continueAbbrTableRead(int8& data) below.
// Inputs:     None
// Outputs:    None
void tts_startAbbrTableRead()
{
   tts_AbbrReadCount = 0;
   tts_start();
   tts_sendByte(TTS_ABBR_RD);
   tts_sendByte(0);
}


// Purpose:    Read a byte of data from the abbreviation table. Only call this
//             function after tts_startAbbrTableRead() has been called.
// Inputs:     A reference to an 8 bit integer to store the byte read
// Outputs:    0 if the entire table has been read
//             1 if more reading is required
// Notes:      Abbreviation table format
//             1) Marker: TTS_ACTIVE_ABBR for active abbreviation    (0xFE)
//                        TTS_DELETED_ABBR for deleted abbreviation  (0xFC)
//             2) Count:  Total byte count (Marker + Count + XXX + YYYY + NULLs)
//             3) XXX:    Abbreviation characters. Example: "TTS"
//             4) NULL:   0x00
//             5) YYYY:   Abbreviation text. Example: "Text to speech"
//             6) NULL:   0x00
int1 tts_continueAbbrTableRead(int8& data)
{
   if(tts_AbbrReadCount < TTS_ABBR_TABLE_SIZE + 2)
   {
      ++tts_AbbrReadCount;
      data = tts_readByte();
      return 1;
   }
   else
   {
      tts_end();
      return 0;
   }
}


// Purpose:    Example function for reading the abbreviation table.
//             This function is meant to be modified for custom use.
// Inputs:     None
// Outputs:    None
void tts_readAbbrTable()
{
   int8 tableData;
   int8 type;
   int8 state = 0;
   int8 count = 0;
   char abbr[8];
   char text[30];
   char* p;

   tts_startAbbrTableRead();
   while(tts_continueAbbrTableRead(tableData) == 1)
   {
      switch(state)
      {
         case 0:
            type = tableData;                   // Get the entry type
            ++state;                            // Increment to next state
            break;
         case 1:
            count = tableData - 2;              // Get entry size
            p = abbr;                           // Point at abbreviation array
            ++state;                            // Increment to next state
            break;
         case 2:
            *p = tableData;                     // Save a character
            if((p - abbr) < sizeof(abbr) - 1)   // Keep pointer in array
            {
               ++p;
            }

            if(tableData == '\0')               // Check for end of abbreviation
            {
               p = text;                        // Point at text array
               ++state;                         // Increment to next state
            }

            if(--count == 0)                    // Check for end of entry
            {
               state = 0;                       // Set the state back to zero
            }
            break;
         case 3:
            *p = tableData;                     // Save a character
            if((p - text) < sizeof(text) - 1)   // Keep pointer in array
            {
               ++p;
            }

            if(--count == 0)                    // Check for end of entry
            {
               state = 0;                       // Set the state back to zero
            }
            break;
      }
      if(state == 0 && type == TTS_ACTIVE_ABBR)
      {
         // Handle active abbreviations
         // printf("ABBR: %s\tText: %s\n\r", abbr, text);
      }
   }
}
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Helper functions
////////////////////////////////////////////////////////////////////////////////
// Purpose:    Setup before sending a command
// Inputs:     None
// Outputs:    None
void tts_start()
{
   output_low(TTS_SS);
   delay_us(1);
   output_low(TTS_SCLK);
   delay_us(1);
}


// Purpose:    Cleanup after a command was sent
// Inputs:     None
// Outputs:    None
#inline
void tts_end()
{
   output_high(TTS_SS);
}


// Purpose:    Send a byte of data to the text to speech chip
// Inputs:     An 8 bit integer containing the data
// Outputs:    None
void tts_sendByte(int8 data)
{
   int i;

   // Loop 8 times to send 8 bits
   for(i=0; i<8; ++i)
   {
      delay_us(1);
      output_low(TTS_SCLK);
      // Send MSB first
      output_bit(TTS_MOSI, shift_left(&data, 1, 0));

      // Pulse the clock
      delay_us(1);
      output_high(TTS_SCLK);
   }
}


// Purpose:    Read a byte of data from the text to speech chip
// Inputs:     None
// Outputs:    An 8 bit integer containing the read data
int8 tts_readByte()
{
   int8 i;
   int8 data;

   tts_waitforReadyPin();

   // Loop 8 times to read 8 bits
   for(i=0; i<8; ++i)
   {
      // Lower the clock
      output_low(TTS_SCLK);
      delay_us(1);

      // Raise the clock
      output_high(TTS_SCLK);
      delay_us(1);

      // Read a bit of data MSB first
      shift_left(&data, 1, input(TTS_MISO));
   }

   return data;
}


// Purpose:    Read 16 bits from the text to speech chip
// Inputs:     None
// Outputs:    A 16 bit integer containing the read data
int16 tts_read16()
{
   return make16(tts_readByte(), tts_readByte());
}


// Purpose:    Send a command and a byte of data to the text to speech chip
// Inputs:     1) An 8 bit command (see command list at top of file)
//             2) A byte of data to accompany the command
// Outputs:    None
void tts_sendCmd(int8 cmd, int8 cmdData)
{
   tts_start();
   tts_sendByte(cmd);
   tts_sendByte(cmdData);
   tts_end();
}


// Purpose:    Helper function. Waits until the text to speech chip
//             is ready to receive a character, then sends the character.
// Inputs:     A character to send
// Outputs:    None
void tts_sendChar(char c)
{
   tts_waitforReadyPin();
   tts_sendByte(c);
}


// Purpose:    Waits for the ready pin to go high signifying the text to
//             speech chip is ready to send data or ready to receive data.
// Inputs:     None
// Outputs:    None
#inline
void tts_waitforReadyPin()
{
   while(!input(TTS_RB));
}


// Purpose:    Common function to handle adding and deleting entries
//             to the abbreviation table
// Inputs:     1) Either TTS_ABBR_ADD to add or TTS_ABBR_DEL to delete
//             2) A pointer to a NULL terminated abbreviation
//             3) A pointer to NULL terminated text
// Outputs:    None
void tts_abbr(int8 cmd, char* abbreviation, char* text)
{
   // Do not attempt unless no other interrupts have been triggered
   if(!(tts_getStatus(FALSE) & 0xE005))
   {
      // Setup for sending a command
      tts_start();

      if(abbreviation != 0)
      {
         // Send the add abbreviation entry command and command data
         tts_sendByte(cmd);
         tts_sendByte(0);

         // Send the abbreviation
         while(*abbreviation != '\0')
         {
            tts_sendChar(*abbreviation++);
         }
         tts_sendChar(',');

         // Send the text
         if(text != 0)
         {
            while(*text != '\0')
            {
               tts_sendChar(*text++);
            }
         }
         tts_sendChar(';');

         // Cleanup from sending a command
         tts_end();

         // Wait for the add abbreviation interrupt
         while(!(tts_getStatus(FALSE) & 0x0004));

         // Write the abbreviation entry to the abbreviation table
         tts_storeAbbreviation();

         // Wait for programming to complete
         delay_ms(110);

         // Clear the abbreviation interrupt
         tts_getStatus(TRUE);

         // Adding an abbreviation entry resets the internal registers,
         // so initialization needs to reoccur.
         tts_init();
      }
   }
}


/* Example setup for interrupt service routine
#INT_EXT
void isr_ext()
{
   switch(tts_getStatus(TRUE) & 0xE005)
   {
      case 0x0001:         // Handle ignored command interrupt
         break;
      case 0x0004:         // Handle abbreviation interrupt
         break;
      case 0x2000:         // Handle conversion finished interrupt
         break;
      case 0x4000:         // Handle buffer threshold interrupt
         break;
      case 0x8000:         // Handle count interrupt
         break;
   }
}
*/


////////////////////////////////////////////////////////////////////////////////
// Initialization function
////////////////////////////////////////////////////////////////////////////////
// Purpose:    Initialze the text to speech chip
//             This must be called before any other text to speech function
// Inputs:     None
// Ouputs:     None
void tts_init()
{
   output_high(TTS_SS);                            // Init select line to high
   output_high(TTS_SCLK);                          // Init clock line to high
   output_float(TTS_INT);                          // Init interrupt pin to float

   tts_reset();                                    // Send the reset command
   tts_setClock(TTS_CLK_24576);                    // 24.576Mhz clock frequency
   tts_powerUp();                                  // Send the power up command
   delay_ms(10);                                   // Wait for power up cycle
   tts_setCOM(TTS_BUFF0);                          // No interrupts or buffer threshold
   tts_setCodec(TTS_DISABLE_CODEC);                // Codec disabled
   tts_setAudio(TTS_SPEAKER_ON | TTS_BUFFER_ON);   // Turn on speaker and output buffer
   tts_setVolume(0);                               // Maximum volume
   tts_setSpeed(2);                                // Default speech rate
   tts_setPitch(5);                                // Default pitch
}
////////////////////////////////////////////////////////////////////////////////


#endif
