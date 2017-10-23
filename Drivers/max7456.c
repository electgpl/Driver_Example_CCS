///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                           MAX7456.C                               ////
////                                                                   ////
////         Library for a Maxim MAX7456 On-Screen Display             ////
////                                                                   ////
//// osd_init() - Call before other functions.                         ////
////                                                                   ////
//// osd_enable(e) - If e is TRUE, enables the OSD display.  If e      ////
////     is FALSE, disables the display.  Display can be               ////
////     enabled/disabled and it won't lose it's display memory.       ////
////     osd_init() will enable the display with blank memory.         ////
////                                                                   ////
//// osd_gotoxy(x,y) - Set write position on OSD (upper left is 0,0).  ////
////                                                                   ////
//// osd_putc(c) - Will display c on the next position of the OSD.     ////
////     If the character is not supported, it will display an empty   ////
////     character.                                                    ////
////     The following have special meaning:                           ////
////        \f    Clear display                                        ////
////        \r    Go to start of current row                           ////
////        \n    Go to start of next column                           ////
////        \b    Go back one position                                 ////
////                                                                   ////
////  CONFIGURATION                                                    ////
////  Either define MAX7456_SPI to SPI1 or SPI2 to use the hardware    ////
////  SPI port, -OR- define MAX7456_OUT and MAX7456_CLK to the         ////
////  IO pins used (the input SPI pin is not needed by this driver).   ////
////  MAX7456_CS must be defined to configure the chip select pin.     ////
////                                                                   ////
////  EXAMPLE                                                          ////
////  osd_init();                                                      ////
////  osd_enable(TRUE);                                                ////
////  osd_putc('\f');                                                  ////
////  osd_gotoxy(0,3);                                                 ////
////  printf(osd_putc, "Hello\n  World\n");                            ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2009 Custom Computer Services          ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#if (!defined(MAX7456_SPI) && !defined(MAX7456_OUT) && !defined(MAX7456_CLK))
 #define MAX7456_SPI SPI1
#endif

#if defined(MAX7456_SPI)
 #use spi(MAX7456_SPI, mode=0, MASTER, bits=8, MSB_FIRST, stream=STREAM_MAX7456)
#else
 #use spi(mode=0, MASTER, bits=8, MSB_FIRST, do=MAX7456_OUT, clk=MAX7456_CLK, stream=STREAM_MAX7456)
#endif

#ifndef MAX7456_CS
#define MAX7456_CS   PIN_D0
#endif

#define MAX7456_XFER(x) spi_xfer(STREAM_MAX7456, x)

#define OSD_MAX_X 30
#define OSD_MAX_Y 16

#define MAX7456_CHAR_SPACE 0

#define MAX7456_READ_REG   0x80

#define MAX7456_REG_VM0    0x00
#define MAX7456_REG_VM1    0x01
#define MAX7456_REG_HOS    0x02
#define MAX7456_REG_VOS    0x03
#define MAX7456_REG_DMM    0x04
#define MAX7456_REG_DMAH   0x05
#define MAX7456_REG_DMAL   0x06
#define MAX7456_REG_DMDI   0x07
#define MAX7456_REG_CMM    0x08
#define MAX7456_REG_CMAH   0x09
#define MAX7456_REG_CMAL   0x0A
#define MAX7456_REG_CMDI   0x0B
#define MAX7456_REG_OSDM   0x0C

#define MAX7456_REG_STAT   0xA0
#define MAX7456_REG_DMDO   0xB0
#define MAX7456_REG_CMDO   0xC0

void max7456_write(unsigned int8 reg, unsigned int8 val)
{
   output_low(MAX7456_CS);
   
   MAX7456_XFER(reg);
   MAX7456_XFER(val);
   
   output_high(MAX7456_CS);
}

#define MAX7456_MY_DEFAULT_VM0   0x00

unsigned int8 g_OSDX, g_OSDY;

void osd_gotoxy(unsigned int8 x, unsigned int8 y)
{
   g_OSDX = x;
   g_OSDY = y;
   
   if (g_OSDX >= OSD_MAX_X)
      g_OSDX = 0;
   if (g_OSDY >= OSD_MAX_Y)
      g_OSDY = 0;
}

void osd_enable(int1 doEnable)
{
   int8 reg = MAX7456_MY_DEFAULT_VM0;

   if (doEnable)
      reg |= 0x8;
   
   max7456_write(MAX7456_REG_VM0, reg);
}

static int8 Max7456TransformFromASCII(char c)
{
   if ((c>='1')&&(c<='9'))
      return(c-='0');
   else if ((c>='A')&&(c<='Z'))
      return((c-'A')+0x0B);
   else if ((c>='a')&&(c<='z'))
      return((c-'a')+0x25);
   else
   {
      switch(c)
      {
         case '0':   return(0x0A);
         case '(':   return(0x3F);
         case ')':   return(0x40);
         case '.':   return(0x41);
         case '?':   return(0x42);
         case ';':   return(0x43);
         case ':':   return(0x44);
         case ',':   return(0x45);
         case '`':   return(0x46);
         case '/':   return(0x47);
         case '"':   return(0x48);
         case '=':
         case '-':   return(0x49);
         case '<':   return(0x4A);
         case '>':   return(0x4B);
         case '@':   return(0x4C);
         default:    return(MAX7456_CHAR_SPACE);
      }
   }
}

void osd_putc(char c)
{
   unsigned int8 i;
   unsigned int16 addy;
   
   switch(c)
   {
      case '\f':
         max7456_write(MAX7456_REG_DMM, 4);
         delay_us(30);
         osd_gotoxy(0, 0);
         break;

      case '\b':
         if (g_OSDX)
            g_OSDX--;
         else
         {
            if (g_OSDY)
               g_OSDY--;
            else
               g_OSDY = OSD_MAX_Y - 1;
            
            g_OSDX = OSD_MAX_X - 1;
         }
         osd_gotoxy(g_OSDX, g_OSDY);
         break;

      case '\r':
         g_OSDX = 0;
         osd_gotoxy(g_OSDX, g_OSDY);
         break;

      case '\n':
         g_OSDX = 0;
         g_OSDY++;
         if (g_OSDY >= OSD_MAX_Y)
            g_OSDY = 0;
         osd_gotoxy(g_OSDX, g_OSDY);
         break;
      
      default:
         c = Max7456TransformFromASCII(c);

         addy = (unsigned int16)g_OSDY * (unsigned int16)OSD_MAX_X;
         addy += (unsigned int16)g_OSDX;

         max7456_write(MAX7456_REG_DMM, 0);
         max7456_write(MAX7456_REG_DMAH, bit_test(addy, 8));
         max7456_write(MAX7456_REG_DMAL, addy & 0xFF);
         max7456_write(MAX7456_REG_DMDI, c);

         g_OSDX++;
         if (g_OSDX >= OSD_MAX_X)
         {
            g_OSDX = 0;
            g_OSDY++;
            if (g_OSDY >= OSD_MAX_Y)
               g_OSDY = 0;
         }
         osd_gotoxy(g_OSDX, g_OSDY);
         break;
   }
}

void osd_init(void)
{
  #if (defined(MAX7456_OUT) && defined(MAX7456_CLK))
   output_drive(MAX7456_OUT);
   output_drive(MAX7456_CLK);

   output_high(MAX7456_CS);
   output_drive(MAX7456_CS);
  #endif

   max7456_write(MAX7456_REG_VM0, 2);   //software reset
   
   delay_us(200);
   
   osd_enable(TRUE);
}
