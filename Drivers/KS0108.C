/////////////////////////////////////////////////////////////////////////
////                             KS0108.c                            ////
////                                                                 ////
//// This file contains drivers for using a KS0108 LCD controller    ////
//// for a 64 by 64 pixel display. The driver treats the upper left  ////
//// pixel as (0,0).                                                 ////
////                                                                 ////
//// Use #define FAST_GLCD if the target chip has at least 512 bytes ////
//// of RAM to decrease the time it takes to update the display.     ////
//// KS0108_update() must then be called to update the display after ////
//// changing the display data.                                      ////
//// See HDM64GS12.c for controlling a display with two controllers  ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////  KS0108_init(int1 mode)                                         ////
////     * Must be called before any other function                  ////
////     * TRUE turns the display on, FALSE turns display off        ////
////                                                                 ////
////  KS0108_fill()                                                  ////
////     * Fills the entire LCD with the given data                  ////
////       - color can be ON or OFF                                  ////
////                                                                 ////
////  KS0108_pixel(int8 x, int8 y, int1 color)                       ////
////     * Set the color of the pixel at (x,y)                       ////
////     * TRUE turns the pixel on, FALSE turns the pixel off        ////
////                                                                 ////
////  KS0108_update()                                                ////
////     * Write the display data stored in RAM to the LCD           ////
////     * Only available if FAST_GLCD is defined                    ////
////                                                                 ////
////  int8 KS0108_status()                                           ////
////     * Read the status register                                  ////
////       7:    0 - Ready   1 - Busy                                ////
////       6:    0                                                   ////
////       5:    0 - On      1 - Off                                 ////
////       4:    0 - Normal  1 - Reset                               ////
////       3-0:  0                                                   ////
////                                                                 ////
////  KS0108_topRow(row)                                             ////
////     * Specify the row appearing at the top of the screen (0-63) ////
////                                                                 ////
////  KS0108_on()                                                    ////
////     * Turn the display on                                       ////
////                                                                 ////
////  KS0108_off()                                                   ////
////     * Turn the display off                                      ////
////                                                                 ////
////  KS0108_page(page)                                              ////
////     * Set the page number (0-7)                                 ////
////                                                                 ////
////  KS0108_column(column)                                          ////  
////     * Set the column address (0-63)                             ////
////                                                                 ////
////  KS0108_inst()                                                  ////
////     * Specify reads and writes are instructions                 ////
////                                                                 ////
////  KS0108_data()                                                  ////
////     * Specify reads and writes are data                         ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2004 Custom Computer Services        ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#ifndef KS0108
#define KS0108

#ifndef KS0108_CS
#define KS0108_CS             PIN_B0   // Chip select
#endif
#ifndef KS0108_DI                      //      ___________
#define KS0108_DI             PIN_B2   // Data/Instruction
#endif

#ifndef KS0108_RW                      //      _____
#define KS0108_RW             PIN_B4   // Read/Write
#endif

#ifndef KS0108_E
#define KS0108_E              PIN_B5   // Enable
#endif

#ifndef KS0108_RST
#define KS0108_RST            PIN_C0   // Reset
#endif


#define KS0108_CMD_ON         0x3F
#define KS0108_CMD_OFF        0x3E
#define KS0108_CMD_PAGE       0xB8
#define KS0108_CMD_COLUMN     0x40
#define KS0108_CMD_TOP_RAM    0xC0


#ifdef FAST_GLCD
int8 displayData[512];
#endif


void KS0108_init(int1 mode);
void KS0108_fill(data);
void KS0108_pixel(int8 x, int8 y, int1 color);
int8 KS0108_status();
void KS0108_write(int8 data);
int8 KS0108_read();


// Purpose:    Specify the row appearing at the top of the screen
// Inputs:     The row number (0 - 63)
#define KS0108_topRow(row)     KS0108_write(KS0108_CMD_TOP_RAM | row)


// Purpose:    Turn the display on
#define KS0108_on()            KS0108_write(KS0108_CMD_ON)


// Purpose:    Turn the display off
#define KS0108_off()           KS0108_write(KS0108_CMD_OFF)


// Purpose:    Set the page number
// Inputs:     A page number (0 - 7)
#define KS0108_page(page)      KS0108_write(KS0108_CMD_PAGE | page)


// Purpose:    Set the column address
// Inputs:     The column address (0 - 63)
#define KS0108_column(column)  KS0108_write(KS0108_CMD_COLUMN | column)

// Purpose:    Specify reads and writes are instructions
#define KS0108_inst()          output_low(KS0108_DI);


// Purpose:    Specify reads and writes are data
#define KS0108_data()          output_high(KS0108_DI);


// Purpose:    Initialize the display controller
// Inputs:     TRUE  - Initialize with the display on
//             FALSE - Initialize with the display off
void KS0108_init(int1 mode)
{
   // Initialze some pins
   output_high(KS0108_RST);
   output_low(KS0108_E);
   output_low(KS0108_CS);

   KS0108_inst();             // Set for instruction
   KS0108_topRow(0);          // First RAM line at the top of the screen
   KS0108_page(0);            // Set the page address to 0
   KS0108_column(0);          // Set the column address to 0

   if(mode)
      KS0108_on();            // Turn the display on
   else
      KS0108_off();           // Turn the display off

   KS0108_fill(0);            // Clear the display
}


// Purpose:    Fill a display with a vertical byte pattern
// Inputs:     The byte of data to fill with
void KS0108_fill(data)
#ifdef FAST_GLCD
{
   int16 i;

   for(i=0; i<512; ++i)
   {
   	displayData[i] = data;
   }
}
#else
{
   int8 i, j;

   // Loop through the vertical pages
   for(i = 0; i < 8; ++i)
   {
      KS0108_inst();          // Set for instruction
      KS0108_page(i);         // Set page address
      KS0108_column(0);       // Set horizontal address to 0
      KS0108_data();          // Set for data

      // Loop through the horizontal sections
      for(j = 0; j < 64; ++j)
      {
         KS0108_write(data);  // Write the byte of data
      }
   }
}
#endif


// Purpose:    Turn the pixel at (x,y) on or off
// Inputs:     1) The x coordinate (0 - 63)
//             2) The y coordinate (0 - 63)
//             3) TRUE turns the pixel on, FALSE turns the pixel off
void KS0108_pixel(int8 x, int8 y, int1 color)
#ifdef FAST_GLCD
{
   int16 index = 64;
   index *=  y/8;
   index += x;

   if(color)
   {
      bit_set(displayData[index], y%8);
   }
   else
   {
      bit_clear(displayData[index], y%8);
   }
}
#else
{
   int8 data;

   KS0108_inst();             // Set for instruction
   KS0108_column(x);          // Set the horizontal address
   KS0108_page((y/8));        // Set the page address
   KS0108_data();             // Set for data
   KS0108_read();             // Need two reads to get data at new address
   data = KS0108_read();

   if(color)
      bit_set(data, y%8);     // Turn the pixel on
   else                       // or
      bit_clear(data, y%8);   // turn the pixel off

   KS0108_inst();             // Set for instruction
   KS0108_column(x);          // Set the horizontal address
   KS0108_data();             // Set for data
   KS0108_write(data);        // Write the pixel data
}
#endif


// Purpose:    Update the LCD with data from the display array
#ifdef FAST_GLCD
void KS0108_update()
{
   int8 i, j;
   int8 *p;
   
   p = displayData;

   // Loop through the vertical pages
   for(i = 0; i < 8; ++i)
   {
      KS0108_inst();          // Set for instruction
      KS0108_page(i);         // Set page address
      KS0108_column(0);       // Set horizontal address to 0
      KS0108_data();          // Set for data

      // Loop through the horizontal sections
      for(j = 0; j < 64; ++j)
      {
         KS0108_write(*p++);  // Write the byte of data
      }
   }
}
#endif


// Purpose:    Read the status register
// 7:    0 - Ready   1 - Busy
// 6:    0
// 5:    0 - On      1 - Off
// 4:    0 - Normal  1 - Reset
// 3-0:  0
int8 KS0108_status()
{
   KS0108_inst();             // Set for instruction
   return KS0108_read();      // Read and return the status
}


// Purpose:    Write a byte to the specified chip
// Inputs:     The byte to write
// Note:       Specify instruction or data before calling
void KS0108_write(int8 data)
{
   output_high(KS0108_CS);    // Raise the chip select
   output_low(KS0108_RW);     // Set for writing
   output_d(data);            // Write to the data bus
   delay_cycles(2);
   output_high(KS0108_E);     // Pulse the enable pin
   delay_cycles(10);
   output_low(KS0108_E);
   output_low(KS0108_CS);     // Lower the chip select
   delay_cycles(10);
}


// Purpose:    Reads a byte of data from the specified chip
// Ouputs:     A byte of data read from the chip
// Note:       Specify instruction or data before calling
int8 KS0108_read()
{
   int8 data;                 // Stores the data read from the LCD

   set_tris_d(0xFF);          // Set port d to input
   output_high(KS0108_CS);    // Raise the chip select
   output_high(KS0108_RW);    // Set for reading
   delay_cycles(2);
   output_high(KS0108_E);     // Raise the enable pin
   delay_cycles(10);
   data = input_d();          // Read from the data bus
   output_low(KS0108_E);      // Lower the enable pin
   output_low(KS0108_CS);     // Lower the chip select lines
   delay_cycles(10);

   return data;               // Return the read data
}

#endif
