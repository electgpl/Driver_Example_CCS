///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////  Library for a MicroChip 93C56 EEPROM configured for a x8 org     ////
////                                                                   ////
////  #use spi() is used to configure the SPI pins on your PIC.  If    ////
////  you use hardware SPI pins, the library will automatically use    ////
////  the SPI peripheral.  If you use pins that are not SPI pins,      ////
////  the library will fall back to bit banging thos GPIO pins.        ////
////                                                                   ////
////                                                                   ////
////  Required configuration:                                          ////
////  ---------------------------------------------------------        ////
////     EEPROM_SELECT - Chip select, active low                       ////
////     EEPROM_CLK - SPI clock                                        ////
////     EEPROM_DI - SPI MOSI                                          ////
////     EEPROM_DO - SPI MISO                                          ////
////                                                                   ////
////                                                                   ////
////  API:                                                             ////
////  ---------------------------------------------------------        ////
////   init_ext_eeprom();    Call before the other functions are used  ////
////                                                                   ////
////   write_ext_eeprom(a, d);  Write the byte d to the address a      ////
////                                                                   ////
////   d = read_ext_eeprom(a);   Read the byte d from the address a    ////
////                                                                   ////
////   The main program may define eeprom_select, eeprom_di, eeprom_do ////
////   and eeprom_clk to override the defaults below.                  ////
////                                                                   ////
////                                                                   ////
////  Example files:                                                   ////
////  ---------------------------------------------------------        ////
////     ex_spi.c - Demo of using this library to read/write.          ////
////                                                                   ////
////                                                                   ////
////                                                                   ////
////  Alternate compatible drivers:                                    ////
////  ---------------------------------------------------------        ////
////     9356spi.c - Use CCS's legacy SPI libraries.                   ////
////                                                                   ////
////     9356bitbang.c - GPIO pins are manually bit banged.            ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2012 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#ifndef EEPROM_SELECT
   #if defined(__PCD__)
      #define EEPROM_SELECT PIN_D3
      #define EEPROM_CLK    PIN_D2
      #define EEPROM_DI     PIN_D1  //mosi
      #define EEPROM_DO     PIN_D0  //miso
   #else
      #define EEPROM_SELECT PIN_B4
      #define EEPROM_CLK    PIN_B2
      #define EEPROM_DI     PIN_B1  //mosi
      #define EEPROM_DO     PIN_B0  //miso
   #endif
#endif

#use spi(master, di=EEPROM_DI, do=EEPROM_DI, clk=EEPROM_CLK, mode=0, baud=1000000, stream=STREAM_SPI_9356)

#define SPI_XFER_9356(x)   spi_xfer(STREAM_SPI_9356, x)

#define EEPROM_ADDRESS unsigned int8
#define EEPROM_SIZE    256

void init_ext_eeprom(void) 
{
   output_high(EEPROM_SELECT);
   output_low(EEPROM_DI);
   output_low(EEPROM_CLK);
   output_float(EEPROM_DO);

   output_high(EEPROM_SELECT);
   SPI_XFER_9356(0x9);
   SPI_XFER_9356(0x80);
   output_low(EEPROM_SELECT);
}


void write_ext_eeprom(EEPROM_ADDRESS address, BYTE data) 
{
   output_high(EEPROM_SELECT);
   
   SPI_XFER_9356(0xa);
   SPI_XFER_9356(address);
   SPI_XFER_9356(data);
   
   output_low(EEPROM_SELECT);
   
   delay_ms(11);
}


BYTE read_ext_eeprom(EEPROM_ADDRESS address) 
{
   BYTE data;

   output_high(EEPROM_SELECT);
   
   SPI_XFER_9356(0x18);
   SPI_XFER_9356(address);
   data=SPI_XFER_9356(0);
   
   output_low(EEPROM_SELECT);

   return(data);
}
