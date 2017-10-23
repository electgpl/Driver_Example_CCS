/////////////////////////////////////////////////////////////////////////
////                           SED1335.c                             ////
////                                                                 ////
////  Example drivers for the SED1335 LCD controller on an Ampire    ////
////  AG-320240A1 display.                                           ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////  glcd_init(mode)                                                ////
////     * Must be called before any other function.                 ////
////       - mode can be ON or OFF to turn the LCD on or off         ////
////                                                                 ////
////  glcd_pixel(x,y,color)                                          ////
////     * Sets the pixel to the given color.                        ////
////       - color can be ON or OFF                                  ////
////                                                                 ////
////  glcd_fillScreen(color)                                         ////
////     * Fills the entire LCD with the given color.                ////
////       - color can be ON or OFF                                  ////
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


/////////////////////////////////////////////////////////////////////////
#ifndef SED1335
#define SED1335

#ifndef GLCD_WIDTH
#define GLCD_WIDTH         320
#endif

#ifndef GLCD_HEIGHT
#define GLCD_HEIGHT        240
#endif

#ifndef GLCD_CHAR_WIDTH
#define GLCD_CHAR_WIDTH    8
#endif

#ifndef GLCD_CHAR_HEIGHT
#define GLCD_CHAR_HEIGHT   8
#endif

#ifndef GLCD_RST
#define GLCD_RST           PIN_C3
#endif

#ifndef GLCD_RD
#define GLCD_RD            PIN_B5
#endif

#ifndef GLCD_WR
#define GLCD_WR            PIN_B1
#endif

#ifndef GLCD_CS
#define GLCD_CS            PIN_B2
#endif

#ifndef GLCD_A0
#define GLCD_A0            PIN_B4
#endif

#ifndef ON
#define ON                 1
#endif

#ifndef OFF
#define OFF                0
#endif
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
// The following defines setup the memory used by different regions
// Currenty one text area is defined at the beginning of memory
// and a graphics area follows immediately after
/////////////////////////////////////////////////////////////////////////
#define GLCD_TEXT_ADDR              0x0000
#define GLCD_GRAPHICS_ADDR          GLCD_WIDTH * GLCD_HEIGHT / 64
#define GLCD_GRAPHICS_ADDR_END      GLCD_GRAPHICS_ADDR + (GLCD_WIDTH * GLCD_HEIGHT / 8)
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
#if GLCD_CHAR_WIDTH < 9
   #define GLCD_CR (GLCD_WIDTH/8 - 1)
#else
   #define GLCD_CR (GLCD_WIDTH/4 - 2)
#endif
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
#define TGLCD_COMMAND   output_high(GLCD_A0);
#define TGLCD_DATA      output_low(GLCD_A0);
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
int8 glcd_readByte();
void glcd_sendByte(int8 data);
void glcd_fillScreen(int1 color);
void glcd_fillScreenText(char c);
void setCursorAddress(int16 addr);
void glcd_pixel(int16 x, int16 y, int1 color);
int8 getData(int16 addr);
void glcd_sendCMD(int8 cmd);
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
void glcd_systemSetup();
void glcd_scrollSetup();
void glcd_overlaySetup();
void glcd_power(int1 mode);
void glcd_cursorDirection(int8 dir);
void glcd_cursorForm(int8 width, int8 height);
void setData(int16 addr, int8 data);
/////////////////////////////////////////////////////////////////////////


#define GLCD_CMD_SYSTEM          0x40  // General system settings
#define GLCD_CMD_SLEEP           0x53  // Enter into standy mode
#define GLCD_CMD_DISP_OFF        0x58  // Turn the display off
#define GLCD_CMD_DISP_ON         0x59  // Turn the display on
#define GLCD_CMD_SCROLL          0x44  // Setup text and graphics address regions
#define GLCD_CMD_CSR_FORM        0x5D  // Set cursor size
#define GLCD_CMD_CSRDIR_RIGHT    0x4C  // Cursor moves right after write to display memory
#define GLCD_CMD_CSRDIR_LEFT     0x4D  // Cursor moves left after write to display memory
#define GLCD_CMD_CSRDIR_UP       0x4E  // Cursor moves up after write to display memory
#define GLCD_CMD_CSRDIR_DN       0x4F  // Cursor moves down after write to display memory
#define GLCD_CMD_CGRAM_ADDR      0x5C  // Configure character generator RAM address
#define GLCD_CMD_HDOT_SCR        0x5A  // Set horizontal scroll rate
#define GLCD_CMD_OVERLAY         0x5B  // Configure how layers overlay
#define GLCD_CMD_SET_CSR_ADDR    0x46  // Set the cursor address
#define GLCD_CMD_GET_CSR_ADDR    0x47  // Read the cursor address
#define GLCD_CMD_DISPLAY_WRITE   0x42  // Write to display memory
#define GLCD_CMD_DISPLAY_READ    0x43  // Read from display memory


// Purpose:       Initialize the controller
// Inputs:        The initialization mode
//                OFF - Turns the LCD off
//                ON  - Turns the LCD on
void glcd_init(int1 mode)
{
   // Initialze some pins
   output_high(GLCD_RST);
   output_high(GLCD_CS);
   output_high(GLCD_RD);
   output_high(GLCD_WR);

   glcd_systemSetup();
   glcd_scrollSetup();
   glcd_overlaySetup();
   glcd_power(OFF);
   glcd_cursorForm(4, 6);
   glcd_fillScreen(OFF);
   glcd_fillScreenText(' ');
   glcd_power(mode);
   glcd_cursorDirection(GLCD_CMD_CSRDIR_RIGHT);
}


// Purpose:       Turn a pixel on a graphic LCD on or off
// Inputs:        x - the x coordinate of the pixel
//                y - the y coordinate of the pixel
//                color - ON or OFF
void glcd_pixel(int16 x, int16 y, int1 color)
{
   int8  data;
   int16 addr;

   // Calculate the byte address containing the pixel
   addr = GLCD_GRAPHICS_ADDR + (GLCD_WIDTH/8 * y + x/8);

   // Read the byte of data at the address
   data = getData(addr);

   // Turn the pixel on or off
   if(color == ON)
      bit_set(data, 7 - x%8);
   else
      bit_clear(data, 7 - x%8);

   // Write the new data byte to display memory
   setData(addr, data);
}


// Purpose:    Initialize the display environment
void glcd_systemSetup()
{
   glcd_sendCMD(GLCD_CMD_SYSTEM);          // Setup the system
   TGLCD_DATA                              // Set for data
   glcd_sendByte(0x30);                    // No offset
   glcd_sendByte(0x7F + GLCD_CHAR_WIDTH);  // Set character width
   glcd_sendByte(GLCD_CHAR_HEIGHT - 1);    // Set character height
   glcd_sendByte(GLCD_CR);                 // Display line address range
   glcd_sendByte(0x2F);                    // TC/R
   glcd_sendByte(GLCD_HEIGHT - 1);         // Number of lines per frame
   glcd_sendByte(GLCD_CR + 1);             // Horizontal address range LSB (APL)
   glcd_sendByte((GLCD_CR + 1) / 0xFF);    // Horizontal address range MSB (APH)
}


// Purpose:    Set the scroll start address and
//             the size of a scroll block
void glcd_scrollSetup()
{
   // Currently setup for a text and graphics layer
   glcd_sendCMD(GLCD_CMD_SCROLL);              // Setup scrolling
   TGLCD_DATA                                  // Set for data
   glcd_sendByte(GLCD_TEXT_ADDR);              // SAD1L
   glcd_sendByte(GLCD_TEXT_ADDR / 0xFF);       // SAD1H
   glcd_sendByte(GLCD_HEIGHT - 1);             // SL1
   glcd_sendByte(GLCD_GRAPHICS_ADDR);          // SAD2L
   glcd_sendByte(GLCD_GRAPHICS_ADDR / 0xFF);   // SAD2H
   glcd_sendByte(GLCD_HEIGHT - 1);             // SL2
   glcd_sendByte(0x00);                        // SAD3L
   glcd_sendByte(0x00);                        // SAD3H
   glcd_sendByte(0x00);                        // SAD4L
   glcd_sendByte(0x00);                        // SAD4H

   glcd_sendCMD(GLCD_CMD_HDOT_SCR);            // Horizontal scroll rate
   TGLCD_DATA                                  // Set for data
   glcd_sendByte(0x00);                        // Horizontal pixel shift is 0
}


// Purpose:    Setup the overlay functionality for combining
//             layers of text and graphics, or multiple
//             graphics layers
void glcd_overlaySetup()
{
   // Currently setup for a single graphics layer
   glcd_sendCMD(GLCD_CMD_OVERLAY);              // Text / graphic overlay mode
   TGLCD_DATA                                   // Set for data
   glcd_sendByte(0x09);                         // Area 1 text, others graphics
                                                // Text XOR Graphics
}


// Purpose:    Turn the display on or off
// Inputs:     ON to turn on or OFF to turn off
void glcd_power(int1 mode)
{
   if(mode == ON)
   {
      glcd_sendCMD(GLCD_CMD_DISP_ON);           // Turn the display on
   }
   else
   {
      glcd_sendCMD(GLCD_CMD_DISP_OFF);          // Turn the display off
   }

   TGLCD_DATA                                   // Set for data
   glcd_sendByte(0x14);
}


// Purpose:    Set the direction the cursor moves after
//             writing to dispaly memory
// Inputs:     Use one of the following to set the direction:
//             GLCD_CMD_CSRDIR_RIGHT
//             GLCD_CMD_CSRDIR_LEFT
//             GLCD_CMD_CSRDIR_UP
//             GLCD_CMD_CSRDIR_DOWN
void glcd_cursorDirection(int8 dir)
{
   glcd_sendCMD(dir);
}


// Purpose:    Set the size of the cursor
// Inputs:     1) The width in pixels - 1  Valid numbers: (0 - 15)
//             2) The height in pixels - 1 Valid numbers: (1 - 15)
void glcd_cursorForm(int8 width, int8 height)
{
   glcd_sendCMD(GLCD_CMD_CSR_FORM);             // Cursor form and size
   TGLCD_DATA                                   // Set for data
   glcd_sendByte(width);
   glcd_sendByte(0x80 + height);
}


// Purpose:    Fill a graphics layer passed in color
//             Works much faster than drawing a rectangle to fill the screen
// Inputs:     ON - turn all the pixels on
//             OFF - turn all the pixels off
void glcd_fillScreen(int1 color)
{
   int16 i;

   setCursorAddress(GLCD_GRAPHICS_ADDR);
   glcd_sendCMD(GLCD_CMD_DISPLAY_WRITE);
   TGLCD_DATA

   for(i = GLCD_GRAPHICS_ADDR; i < GLCD_GRAPHICS_ADDR_END; ++i)
   {
      glcd_sendByte(0xFF * color);
   }
}


// Purpose:    Fill a text layer with a the passed in character
//             Works much faster than drawing a rectangle to fill the screen
// Inputs:     ON - turn all the pixels on
//             OFF - turn all the pixels off
void glcd_fillScreenText(char c)
{
   int16 i;

   setCursorAddress(GLCD_TEXT_ADDR);
   glcd_sendCMD(GLCD_CMD_DISPLAY_WRITE);
   TGLCD_DATA

   for(i = GLCD_TEXT_ADDR; i < GLCD_GRAPHICS_ADDR; ++i)
   {
      glcd_sendByte(c);
   }
}


// Purpose:    Write a byte of data
// Inputs:     The byte of data to write
void glcd_sendByte(byte data)
{
   output_d((data));
   output_low(GLCD_CS);
   delay_cycles(1);
   output_low(GLCD_WR);
   delay_cycles(2);
   output_high(GLCD_WR);
   output_high(GLCD_CS);
}


// Purpose:    Read a byte of data
// Outputs:    The byte of data
int8 glcd_readByte()
{
   byte data;
   set_tris_d(0xFF);
   output_low(GLCD_CS);
   delay_cycles(1);
   output_low(GLCD_RD);
   delay_cycles(2);
   data = input_d();
   output_high(GLCD_RD);
   output_high(GLCD_CS);

   return data;
}


// Purpose:    Get the status
// Outputs:    The status in an 8 bit integer
int8 getStatus()
{
   int8 status;
   TGLCD_DATA
   output_low(GLCD_CS);
   output_low(GLCD_RD);
   delay_us(1);
   status = input_d();
   output_high(GLCD_RD);
   output_high(GLCD_CS);

   return status;
}


// Purpose:    Get the current address of the cursor
// Outputs:    A 16 bit integer containing the cursor address
int16 getCursorAddress()
{
   int16 addr;

   glcd_sendCMD(GLCD_CMD_GET_CSR_ADDR);
   TGLCD_COMMAND
   *(int8*)(&addr    ) = glcd_readByte();  // Read low part
   *((int8*)&addr + 1) = glcd_readByte();  // Read high part

   return addr;
}


// Purpose:    Set the cursor address
// Inputs:     A 16 bit integer containing the new cursor address
void setCursorAddress(int16 addr)
{
   glcd_sendCMD(GLCD_CMD_SET_CSR_ADDR);
   TGLCD_DATA
   glcd_sendByte(*(int8*)(&addr    ));
   glcd_sendByte(*((int8*)&addr + 1));
}


// Purpose:    Get a byte of data from the display at the address
// Inputs:     A 16 bit integer containing the address
// Outputs:    An 8 bit integer with the read data
int8 getData(int16 addr)
{
   setCursorAddress(addr);
   glcd_sendCMD(GLCD_CMD_DISPLAY_READ);
   TGLCD_COMMAND
   return glcd_readByte();
}


// Purpose:    Set a byte of display data at an address
// Inputs:     1) A 16 bit address
//             2) 8 bits worth
void setData(int16 addr, int8 data)
{
   setCursorAddress(addr);
   glcd_sendCMD(GLCD_CMD_DISPLAY_WRITE);
   TGLCD_DATA
   glcd_sendByte(data);
}


// Purpose:    Send an 8 bit command
// Inputs:     The command to send
void glcd_sendCMD(int8 cmd)
{
   TGLCD_COMMAND
   glcd_sendByte(cmd);
}

#endif
