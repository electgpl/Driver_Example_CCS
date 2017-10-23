/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////                           max14500.c                            ////
////                                                                 ////
//// Driver for MAX14500-14503 chip, which allows an SD card to be   ////
//// either connected to USB (looks like a thumdrive to the PC) or   ////
//// to the MCU (MCU can read/write using SPI).                      ////
////                                                                 ////
//// This driver allows either simple or I2C mode, and the SD        ////
//// card must be connected to the Card 1 connector of the MAX14500. ////
////                                                                 ////
//// MODE CONFIGURATION                                              ////
//// --------------------------------------------------------------  ////
////  MAX14500_IN_I2C_MODE                                           ////
////  MAX14500_IN_SIMPLE_MODE                                        ////
////     The MAX14500 operates in two modes - simple or I2C mode.    ////
////     In simple mode the MCU or USB connection is controlled      ////
////     via few GPIO pins.  In I2C mode the MCU or USB connection   ////
////     is controlled via I2C.  One of the above must be defined    ////
////     to configure this driver.                                   ////
////                                                                 ////
//// MAX14500_IN_I2C_MODE CONFIGURATION                              ////
//// --------------------------------------------------------------  ////
////  PIN_MAX14500_SCL                                               ////
////  PIN_MAX14500_SDA                                               ////
////     Must be defind to the I2C pins wired to the 14500.  It is   ////
////     assumed that the MAX14500's address pin is low and the      ////
////     I2C address of the 14500 is 0xE0.                           ////
////                                                                 ////
////  MAX14500_CLOCK_CONFIG                                          ////
////     This needs to be defined to the clock setting that needs    ////
////     to be set for the 14500.  See max14500_clock_t for choices. ////
////                                                                 ////
////  PIN_USB_DETECT                                                 ////
////     (optional) #define to an MCU pin connected to the 5V VBUS   ////
////     of the USB connector.  This pin is then used to detect if   ////
////     USB is connected to the board.  In I2C mode this pin        ////
////     is optional.  If not defined then the library will          ////
////     will use I2C to read the VBUS status on the 14500 IC.       ////
////                                                                 ////
////  MAX14500_VID                                                   ////
////  MAX14500_PID                                                   ////
////     (optiona) If #defined, these values will be written to the  ////
////     device.  If not #defined, the default values will be        ////
////     left.                                                       ////
////                                                                 ////
//// MAX14500_IN_SIMPLE_MODE CONFIGURATION                           ////
//// --------------------------------------------------------------  ////
////  PIN_MAX14500_MODE                                              ////
////     #define to the MCU pin connected to the MAX14500's MODE     ////
////     pin.                                                        ////
////                                                                 ////
////  PIN_USB_DETECT                                                 ////
////     #define to an MCU pin connected to the 5V VBUS of the USB   ////
////     connector.  This pin is then used to detect if USB is       ////
////     connected to the board.                                     ////
////                                                                 ////
//// MAX14500_IN_I2C_MODE and MAX14500_IN_SIMPLE_MODE CONFIGURATION  ////
//// --------------------------------------------------------------  ////
////  PIN_MAX14500_RST                                               ////
////     #define to the MCU pin connected to the MAX14500's RST pin. ////
////                                                                 ////
////  PIN_MAX14500_I2CSEL                                            ////
////     (optional) If you have a software GPIO pin connected to     ////
////     the MAX14500's I2CSEL pin, then this needs to be #defined.  ////
////     The PIC will set this pin high or low depending on the      ////
////     mode choice the user has defined.                           ////
////                                                                 ////
//// API                                                             ////
//// --------------------------------------------------------------  ////
////  MAX14500_Init()                                                ////
////     Initialize the chip.  The SD card will be connected to      ////
////     the MCU.                                                    ////
////                                                                 ////
////  MAX14500_SDToMCU(int1 enable)                                  ////
////     If 'enable' is TRUE, the SD card will be connected to the   ////
////     MCU.  If 'enable' is FALSE, the SD card will be connected   ////
////     to the PC/USB.                                              ////
////                                                                 ////
////  MAX14500_Task()                                                ////
////     Calling this function is optional.  When called, see's if   ////
////     USB is connected - if USB is connected will switch SD to    ////
////     the USB.  When USB isn't connected the SD will switch to    ////
////     the MCU.                                                    ////
////                                                                 ////
////  int1 MAX14500_SDIsMCU()                                        ////
////     Returns TRUE if the SD card is connected to the MCU, FALSE  ////
////     if connected to the PC.                                     ////
////                                                                 ////
////  int1 MAX14500_USBIsConnected()                                 ////
////     Returns TRUE if USB is connected to the MAX14500.           ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 2013 Custom Computer Services              ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#ifndef __MAX14500_C__
#define __MAX14500_C__

//////////// begin public definitions         ///////////////////////////

void MAX14500_Init(void);
void MAX14500_SDToMCU(int1 enable);
void MAX14500_Task(void);
int1 MAX14500_SDIsMCU(void);
int1 MAX14500_USBIsConnected(void);

typedef enum
{
   MAX14500_CLOCK_DEFAULT = 0,
   MAX14500_CLOCK_19P2MHZ_SQUARE = 4,
   MAX14500_CLOCK_19P2MHZ_SINE = 8,
   MAX14500_CLOCK_13MHZ_SQUARE = 20,
   MAX14500_CLOCK_13MHZ_SINE = 24,
   MAX14500_CLOCK_12MHZ_SQUARE = 36,
   MAX14500_CLOCK_12MHZ_SINE = 40,
   MAX14500_CLOCK_26MHZ_SQUARE = 52,
   MAX14500_CLOCK_26MHZ_SINE = 56
} max14500_clock_t;

//////////// end public definitions         /////////////////////////////

#if !defined(MAX14500_IN_I2C_MODE) && !defined(MAX14500_IN_SIMPLE_MODE)
   #error Need to choose I2C or simple mode
#endif

int1 g_MAX14500_SDIsMCU;

int1 MAX14500_SDIsMCU(void)
{
   return(g_MAX14500_SDIsMCU);
}

void MAX14500_Task(void)
{
   if (MAX14500_USBIsConnected() == MAX14500_SDIsMCU())
   {
      MAX14500_SDToMCU(!g_MAX14500_SDIsMCU);
   }
}


#if defined(MAX14500_IN_SIMPLE_MODE)
void MAX14500_Init(void)
{
  #if defined(PIN_MAX14500_I2CSEL)
   output_low(PIN_MAX14500_I2CSEL);
   output_drive(PIN_MAX14500_I2CSEL);
  #endif

   MAX14500_SDToMCU(TRUE);
   
   output_low(PIN_MAX14500_RST);
   output_drive(PIN_MAX14500_RST);
   delay_ms(10);
   output_high(PIN_MAX14500_RST);
   delay_ms(10);
}

void MAX14500_SDToMCU(int1 enable)
{
   g_MAX14500_SDIsMCU = enable;
   
   if (!enable)
   {
      output_high(PIN_MAX14500_MODE);  //card reader mode
   }
   else
   {
      output_low(PIN_MAX14500_MODE);   //pass thru mode
   }
   output_drive(PIN_MAX14500_MODE);
}

int1 MAX14500_USBIsConnected(void)
{
   return(input(PIN_USB_DETECT));
}
#endif   //defined(MAX14500_IN_SIMPLE_MODE)

#if defined(MAX14500_IN_I2C_MODE)
#define MAX14500_I2C_ADDY  0xE0

#use i2c(MASTER, stream=STREAM_I2C_MAX14500, scl=PIN_MAX14500_SCL, sda=PIN_MAX14500_SDA)

void MAX14500_WriteReg(unsigned int8 loc, unsigned int8 val)
{
   i2c_start(STREAM_I2C_MAX14500);
   
   i2c_write(STREAM_I2C_MAX14500, MAX14500_I2C_ADDY);
   i2c_write(STREAM_I2C_MAX14500, loc);
   i2c_write(STREAM_I2C_MAX14500, val);
   
   i2c_stop(STREAM_I2C_MAX14500);
}

unsigned int8 MAX14500_ReadReg(unsigned int8 loc)
{
   unsigned int8 ret;
   
   i2c_start(STREAM_I2C_MAX14500);
   
   i2c_write(STREAM_I2C_MAX14500, MAX14500_I2C_ADDY);
   i2c_write(STREAM_I2C_MAX14500, loc);
   
   i2c_stop(STREAM_I2C_MAX14500);
   
   i2c_start(STREAM_I2C_MAX14500);
   
   i2c_write(STREAM_I2C_MAX14500, MAX14500_I2C_ADDY | 1);
   ret = i2c_read(STREAM_I2C_MAX14500, 0);
   
   i2c_stop(STREAM_I2C_MAX14500);
   
   return(ret);
}

void MAX14500_Init(void)
{
  #if defined(PIN_MAX14500_I2CSEL)
   output_high(PIN_MAX14500_I2CSEL);
   output_drive(PIN_MAX14500_I2CSEL);
  #endif
   
   output_low(PIN_MAX14500_RST);
   output_drive(PIN_MAX14500_RST);
   delay_ms(10);
   output_high(PIN_MAX14500_RST);
   delay_ms(10);
   
   MAX14500_WriteReg(0x02, MAX14500_CLOCK_CONFIG);
   
  #if defined(MAX14500_VID)
   MAX14500_WriteReg(0x06, MAX14500_VID >> 8);
   MAX14500_WriteReg(0x07, MAX14500_VID);
  #endif
  #if defined(MAX14500_PID)
   MAX14500_WriteReg(0x08, MAX14500_PID >> 8);
   MAX14500_WriteReg(0x09, MAX14500_PID);
  #endif

   g_MAX14500_SDIsMCU = TRUE;
}

void MAX14500_SDToMCU(int1 enable)
{
  #if defined(PIN_USB_DETECT)
   #define MAX14500_CONTROL_AT_IDLE 0x00  //we don't need to keep 14500 awake to detect USB
  #else
   #define MAX14500_CONTROL_AT_IDLE 0x01  //we need to keep 14500 awake to detect USB
  #endif
  
   g_MAX14500_SDIsMCU = enable;
   
   if (!enable)
   {
      //wakeup, close switches and enable card reader on port 1
      MAX14500_WriteReg(0x00, 0x1b);
   }
   else
   {
      //disable card reader and close switches
      MAX14500_WriteReg(0x00, 0x18 | MAX14500_CONTROL_AT_IDLE);
   }
}

int1 MAX14500_USBIsConnected(void)
{
  #if defined(PIN_USB_DETECT)
   return(input(PIN_USB_DETECT));
  #else
   unsigned int8 status1;
   
   status1 = MAX14500_ReadReg(0x12);
   
   return(bit_test(status1, 2));
  #endif
}
#endif   //defined(MAX14500_IN_I2C_MODE)

#endif
