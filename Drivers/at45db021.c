////////////////////////////////////////////////////////////////////////////
////   Library for an ATMEL AT45DB021 DataFlash                         ////
////                                                                    ////
//// init_ext_flash()                                                   ////
////     Initializes the pins that control the flash device. This must  ////
////     be called before any other flash function is used.             ////
////                                                                    ////
//// ****************************************************************** ////
//// void ext_flash_startContinuousRead(p, i)                           ////
////     Initiate a continuous read starting with page p at index i     ////
////                                                                    ////
//// BYTE ext_flash_getByte()                                           ////
////     Gets a byte of data from the flash device                      ////
////     Use after calling ext_flash_startContinuousRead()              ////
////                                                                    ////
//// void ext_flash_getBytes(a, n)                                      ////
////     Read n bytes and store in array a                              ////
////     Use after calling ext_flash_startContinuousRead()              ////
////                                                                    ////
//// void ext_flash_stopContinuousRead()                                ////
////     Use to stop continuously reading data from the flash device    ////
//// ****************************************************************** ////
////                                                                    ////
//// void ext_flash_readPage(p, i, a, n)                                ////
////     Read n bytes from page p at index i and store in array a       ////
////                                                                    ////
//// void ext_flash_readBuffer(b, i, a, n)                              ////
////     Read n bytes from buffer b at index i and store in array a     ////
////                                                                    ////
//// BYTE ext_flash_readStatus()                     ____               ////
////     Return the status of the flash device:  Rdy/Busy Comp 0101XX   ////
////                                                                    ////
//// void ext_flash_writeToBuffer(b, i, a, n)                           ////
////     Write n bytes from array a to buffer b at index i              ////
////                                                                    ////
//// void ext_flash_BufferToPage(b, p, mode)                            ////
////     Copy buffer b to page p                                        ////
////     The 2 modes:                                                   ////
////      - Use ERASE to use built in erase first functionality         ////
////      - Use NO_ERASE to write to a previously erased page           ////
////                                                                    ////
//// void ext_flash_erasePage(p)                                        ////
////     Erase all bytes in page p to 0xFF                              ////
////                                                                    ////
//// void ext_flash_eraseBlock(b)                                       ////
////     Erase all bytes in block b to 0xFF. A block is 8 pages.        ////
////                                                                    ////
//// void ext_flash_writePageThroughBuffer(p, b, i, a, n)               ////
////     Write n bytes from array a to page p at index i through        ////
////     buffer b                                                       ////
////                                                                    ////
//// void ext_flash_PageToBuffer(p, b)                                  ////
////     Copy the data from page p to buffer b                          ////
////                                                                    ////
//// int1 ext_flash_comparePageToBuffer(p, b)                           ////
////     Compare the data in page p to buffer b                         ////
////     Returns 1 if equivalent or 0 if not equivalent                 ////
////                                                                    ////
//// void ext_flash_rewritePage(p, b)                                   ////
////     Rewrite the data in page p using buffer b                      ////
////                                                                    ////
//// void ext_flash_waitUntilReady()                                    ////
////     Waits until the flash device is ready to accept commands       ////
////                                                                    ////
////   The main program may define FLASH_SELECT, FLASH_CLOCK,           ////
////   FLASH_DI, and FLASH_DO to override the defaults below.           ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////
////                                                                    ////
////                       Pin Layout                                   ////
////   ---------------------------------------------------              ////
////   |                                                 |              ////
////   | 1: SI     FLASH_DI      | 8: SO   FLASH_DO      |              ////
////   |                         |                       |              ////
////   | 2: SCK    FLASH_CLOCK   | 7: GND  GND           |              ////
////   |    _____                |                       |              ////
////   | 3: RESET  +2.7V - +3.6V | 6: VCC  +2.7V - +3.6V |              ////
////   |    __                   |    __                 |              ////
////   | 4: CS     FLASH_SELECT  | 5: WP   +2.7V - +3.6V |              ////
////   ---------------------------------------------------              ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C   ////
//// compiler.  This source code may only be distributed to other       ////
//// licensed users of the CCS C compiler.  No other use, reproduction  ////
//// or distribution is permitted without written permission.           ////
//// Derivative programs created using this software in object code     ////
//// form are not restricted in any way.                                ////
////////////////////////////////////////////////////////////////////////////

#ifndef FLASH_SELECT
#define FLASH_SELECT PIN_B0
#define FLASH_CLOCK  PIN_B1
#define FLASH_DI     PIN_B2
#define FLASH_DO     PIN_A4
#endif

#define FLASH_SIZE 270336  // The size of the flash device in bytes

// Used in ext_flash_BufferToPage()
#define ERASE     1  // The flash device will initiate an erase before writing
#define NO_ERASE  0  // The flash device will not initiate an erase before writing




void ext_flash_sendData(int16 data, int8 size);
void ext_flash_sendBytes(BYTE* data, int16 size);
void ext_flash_getBytes(BYTE* data, int16 size);
void ext_flash_waitUntilReady();


// Purpose:       Initialize the pins that control the flash device.
//                This must be called before any other flash function is used.
// Inputs:        None
// Outputs:       None
// Dependencies:  None
void init_ext_flash()
{
   output_low(FLASH_CLOCK);
   output_high(FLASH_SELECT);
}


// Purpose:       This function will start reading a continuous stream of
//                data from the entire flash device.
// Inputs:        1) A page address
//                2) An index into the page
// Outputs:       None
// Dependencies:  ext_flash_sendData(), ext_flash_waitUntilReady()
void ext_flash_startContinuousRead(int16 pageAddress, int16 pageIndex)
{
   ext_flash_waitUntilReady();
   output_low(FLASH_SELECT);                 // Enable select line
   ext_flash_sendData(0xE8, 8);              // Send opcode
   ext_flash_sendData(pageAddress, 15);      // Send page address
   ext_flash_sendData(pageIndex, 9);         // Send index and 32 bits
   ext_flash_sendData(0, 32);                // Send 32 don't care bits
}

// Purpose:       Get a byte of data from the flash device. This function is
//                meant to be used after ext_flash_startContinuousRead() has
//                been called to initiate a continuous read.
// Inputs:        None
// Outputs:       1) A byte of data
// Dependencies:  None
BYTE ext_flash_getByte()
{
   BYTE flashData;
   int i;
   for(i=0; i<8; ++i)                        // Get 8 bits of data
   {
      output_high(FLASH_CLOCK);
      shift_left(&flashData, 1, input(FLASH_DO));
      output_low(FLASH_CLOCK);
   }
   return flashData;
}


// Purpose:       Get a byte of data from the flash device. This function is
//                meant to be used after ext_flash_startContinuousRead() has
//                been called to initiate a continuous read. This function is
//                also used by ext_flash_readPage() and ext_flash_readBuffer().
// Inputs:        1) A pointer to an array to fill
//                2) The number of bytes of data to read
// Outputs:       None
// Dependencies:  None
void ext_flash_getBytes(BYTE* data, int16 size)
{
   int16 i;
   signed int8  j;
   for(i=0; i<size; ++i)
   {
      for(j=0; j<8; ++j)
      {
         output_high(FLASH_CLOCK);
         shift_left(data+i, 1, input(FLASH_DO));
         output_low(FLASH_CLOCK);
      }
   }
}


// Purpose:       Stop continuously reading data from the flash device.
// Inputs:        None
// Outputs:       None
// Dependencies:  None
void ext_flash_stopContinuousRead()
{
   output_high(FLASH_SELECT);                // Disable select line
}


// Purpose:       Read data from a memory page.
// Inputs:        1) A page address
//                2) An index into the page to start reading at
//                3) A pointer to a data array to fill
//                4) The number of bytes of data to read
// Outputs:       None
// Dependencies:  ext_flash_sendData(), ext_flash_waitUntilReady(), ext_flash_getBytes()
void ext_flash_readPage(int16 pageAddress, int16 pageIndex, BYTE* data, int16 size)
{
   ext_flash_waitUntilReady();               // Wait until ready
   output_low(FLASH_SELECT);                 // Enable select line
   ext_flash_sendData(0xD2, 8);              // Send opcode and 5 bits
   ext_flash_sendData(pageAddress, 15);      // Send page address
   ext_flash_sendData(pageIndex, 9);         // Send index
   ext_flash_sendData(0, 32);                // Send 32 don't care bits
   ext_flash_getBytes(data, size);           // Get data from the flash device
   output_high(FLASH_SELECT);                // Disable select line
}


// Purpose:       Read data from a buffer
// Inputs:        1) A buffer number (0 or 1)
//                2) An index into the buffer to start reading at
//                3) A pointer to a data array to be filled
//                4) The number of bytes of data to read
// Outputs:       None
// Dependencies:  ext_flash_sendData(), ext_flash_waitUntilReady(), ext_flash_getBytes()
void ext_flash_readBuffer(int1 bufferNumber, int16 bufferAddress, BYTE* data, int16 size)
{
   BYTE opcode;

   output_low(FLASH_SELECT);                 // Enable select line

   if(bufferNumber)
      opcode = 0xD6;                         // Opcode for second buffer
   else
      opcode = 0xD4;                         // Opcode for first buffer

   ext_flash_sendData(opcode, 8);            // Send opcode
   ext_flash_sendData(0, 15);                // Send 15 don't care bits
   ext_flash_sendData(bufferAddress, 9);     // Send buffer address
   ext_flash_sendData(0, 8);                 // Send 8 don't care bits
   ext_flash_getBytes(data, size);           // Get data from the flash device
   output_high(FLASH_SELECT);                // Disable select line
}


// Purpose:       Return the status of the flash device
// Inputs:        None            ____
// Outputs:       The status: Rdy/Busy Comp 0101XX
// Dependencies:  ext_flash_sendData(), ext_flash_getByte()
BYTE ext_flash_readStatus()
{
   BYTE status;
   output_low(FLASH_SELECT);                 // Enable select line
   ext_flash_sendData(0xD7, 8);              // Send status command
   status = ext_flash_getByte();             // Get the status
   output_high(FLASH_SELECT);                // Disable select line

   return status;                            // Return the status
}


// Purpose:       Write data to a buffer
// Inputs:        1) A buffer number (0 or 1)
//                2) An index into the buffer
//                3) A pointer to the data to write
//                4) The number of bytes of data to write
// Outputs:       None
// Dependencies:  ext_flash_sendData(), ext_flash_waitUntilReady(), ext_flash_sendBytes()
void ext_flash_writeToBuffer(int1 bufferNumber, int16 bufferAddress, BYTE* data, int16 size)
{
   BYTE opcode;

   output_low(FLASH_SELECT);                 // Enable select line

   if(bufferNumber)
      opcode = 0x87;                         // Opcode for second buffer
   else
      opcode = 0x84;                         // Opcode for first buffer

   ext_flash_sendData(opcode, 8);            // Send opcode
   ext_flash_sendData(0, 15);                // Send 15 don't care bits
   ext_flash_sendData(bufferAddress, 9);     // Send buffer address
   ext_flash_sendBytes(data, size);          // Write data to the buffer
   output_high(FLASH_SELECT);                // Disable select line
}


// Purpose:       Write the data in a buffer to a page
// Inputs:        1) A page address
//                2) A buffer number (0 or 1)
//                3) The writing mode to use
//                   - Use ERASE to first erase a page then write
//                   - Use NO_ERASE to write to a previously erased page
// Outputs:       None
// Dependencies:  ext_flash_sendData(), ext_flash_waitUntilReady()
void ext_flash_BufferToPage(int1 bufferNumber, int16 pageAddress, int1 mode)
{
   BYTE opcode;
   ext_flash_waitUntilReady();               // Wait until ready
   output_low(FLASH_SELECT);                 // Enable select line

   if(mode)
   {
      if(bufferNumber)
         opcode = 0x86;                      // Opcode for second buffer
      else
         opcode = 0x83;                      // Opcode for first buffer
   }
   else
   {
      if(bufferNumber)
         opcode = 0x89;                      // Opcode for second buffer
      else
         opcode = 0x88;                      // Opcode for first buffer
   }
   ext_flash_sendData(opcode, 8);            // Send opcode
   ext_flash_sendData(pageAddress, 15);      // Send page address
   ext_flash_sendData(0, 9);                 // Send 9 don't care bits
   output_high(FLASH_SELECT);                // Disable select line
}


// Purpose:       Erase a page
// Inputs:        A page address
// Outputs:       None
// Dependencies:  ext_flash_sendData(), ext_flash_waitUntilReady()
void ext_flash_erasePage(int16 pageAddress)
{
   ext_flash_waitUntilReady();
   output_low(FLASH_SELECT);                 // Enable select line
   ext_flash_sendData(0x81, 8);              // Send opcode
   ext_flash_sendData(pageAddress, 15);      // Send page address
   ext_flash_sendData(0, 9);                 // Send 9 don't care bits
   output_high(FLASH_SELECT);                // Disable select line
}


// Purpose:       Erase a block of 8 pages
// Inputs:        A block address
// Outputs:       None
// Dependencies:  ext_flash_sendData(), ext_flash_waitUntilReady()
void ext_flash_eraseBlock(int8 blockAddress)
{
   ext_flash_waitUntilReady();
   output_low(FLASH_SELECT);                 // Enable select line
   ext_flash_sendData(0x50, 8);              // Send opcode
   ext_flash_sendData(blockAddress, 12);     // Send block address
   ext_flash_sendData(0, 12);                // Send 12 don't care bits
   output_high(FLASH_SELECT);                // Disable select line
}


// Purpose:       Write data to a page through a buffer
// Inputs:        1) The address of the page to write to
//                2) The number of the buffer to use (0 or 1)
//                3) The index into the buffer to start writing at
//                4) A pointer to the data to write
//                5) The number of bytes of data to write
// Outputs:       None
// Dependencies:  ext_flash_sendData(), ext_flash_waitUntilReady(), ext_flash_sendBytes()
void ext_flash_writePageThroughBuffer(int16 pageAddress,
                                      int1 bufferNumber, int16 bufferAddress,
                                      BYTE* data, int16 size)
{
   BYTE opcode;

   ext_flash_waitUntilReady();
   output_low(FLASH_SELECT);                 // Enable select line

   if(bufferNumber)
      opcode = 0x85;                         // Opcode for second buffer
   else
      opcode = 0x82;                         // Opcode for first buffer

   ext_flash_sendData(opcode, 8);            // Send opcode
   ext_flash_sendData(pageAddress, 15);      // Send page address
   ext_flash_sendData(bufferAddress, 9);     // Send buffer address
   ext_flash_sendBytes(data, size);          // Write data to the buffer
   output_high(FLASH_SELECT);                // Disable select line
}


// Purpose:       Get the data from a page and put it in a buffer
// Inputs:        1) A page address
//                2) A buffer number (0 or 1)
// Outputs:       None
// Dependencies:  ext_flash_sendData(), ext_flash_waitUntilReady()
void ext_flash_PageToBuffer(int16 pageAddress, int1 bufferNumber)
{
   BYTE opcode;
   ext_flash_waitUntilReady();
   output_low(FLASH_SELECT);                 // Enable select line

   if(bufferNumber)
      opcode = 0x55;                         // Opcode for second buffer
   else
      opcode = 0x53;                         // Opcode for first buffer

   ext_flash_sendData(opcode, 8);            // Send opcode
   ext_flash_sendData(pageAddress, 15);      // Send page address
   ext_flash_sendData(0, 9);                 // Send 9 don't care bits
   output_high(FLASH_SELECT);                // Disable select line
}


// Purpose:       Compare the data in a page to the data in a buffer
// Inputs:        1) A page address
//                2) A buffer number (0 or 1)
// Outputs:       1 if the data is the same, 0 if the data is not the same
// Dependencies:  ext_flash_sendData(), ext_flash_waitUntilReady()
int1 ext_flash_comparePageToBuffer(int16 pageAddress, int1 bufferNumber)
{
   int1 CompareFlag;
   BYTE opcode;
   ext_flash_waitUntilReady();
   output_low(FLASH_SELECT);                 // Enable select line

   if(bufferNumber)
      opcode = 0x61;                         // Opcode for second buffer
   else
      opcode = 0x60;                         // Opcode for first buffer

   ext_flash_sendData(opcode, 8);            // Send opcode
   ext_flash_sendData(pageAddress, 15);      // Send page address
   ext_flash_sendData(0, 9);                 // Send 9 don't care bits
   output_high(FLASH_SELECT);                // Disable select line

   output_low(FLASH_SELECT);                 // Enable select line
   ext_flash_sendData(0xD7, 8);              // Send status command
   while(!input(FLASH_DO));                  // Wait until ready
   output_high(FLASH_CLOCK);                 // Pulse the clock
   output_low(FLASH_CLOCK);
   CompareFlag = !input(FLASH_DO);           // Get the compare flag
   output_high(FLASH_SELECT);                // Disable select line

   return CompareFlag;
}


// Purpose:       Rewrite the data in a page.
//                The flash device does this by transfering the data to a
//                buffer, then writing the data back to the page.
// Inputs:        1) A page address
//                2) A buffer number (0 or 1)
// Outputs:       None
// Dependencies:  ext_flash_sendData(), ext_flash_waitUntilReady()
void ext_flash_rewritePage(int16 pageAddress, int bufferNumber)
{
   BYTE opcode;
   ext_flash_waitUntilReady();
   output_low(FLASH_SELECT);                 // Enable select line

   if(bufferNumber == 1)
      opcode = 0x58;
   else
      opcode = 0x59;

   ext_flash_sendData(opcode, 8);            // Send opcode
   ext_flash_sendData(pageAddress, 15);      // Send page address
   ext_flash_sendData(0, 9);                 // Send 9 don't care bits
   output_high(FLASH_SELECT);                // Disable select line
}


// Purpose:       Send some data to the flash device
// Inputs:        1) Up to 16 bits of data
//                2) The number of bits to send
// Outputs:       None
// Dependencies:  None
void ext_flash_sendData(int16 data, int8 size)
{
   int8 i;
   data <<= (16-size);
   for(i=0; i<size; ++i)
   {
      output_bit(FLASH_DI, shift_left(&data,2,0));    // Send a data bit
      output_high(FLASH_CLOCK);                       // Pulse the clock
      output_low(FLASH_CLOCK);
   }
}


// Purpose:       Send some bytes of data to the flash device
// Inputs:        1) A pointer to an array of data to send
//                2) The number of bytes to send
// Outputs:       None
// Dependencies:  None
void ext_flash_sendBytes(BYTE* data, int16 size)
{
   int16 i;
   signed int8  j;
   for(i=0; i<size; ++i)
   {
      for(j=7; j>=0; --j)
      {
         output_bit(FLASH_DI, bit_test(data[i], j));  // Send a data bit
         output_high(FLASH_CLOCK);                    // Pulse the clock
         output_low(FLASH_CLOCK);
      }
   }
}

// Purpose:       Wait until the flash device is ready to accept commands
// Inputs:        None
// Outputs:       None
// Dependencies:  ext_flash_sendData()
void ext_flash_waitUntilReady()
{
   output_low(FLASH_SELECT);                 // Enable select line
   ext_flash_sendData(0xD7, 8);              // Send status command
   while(!input(FLASH_DO));                  // Wait until ready
   output_high(FLASH_SELECT);                // Disable select line
}
