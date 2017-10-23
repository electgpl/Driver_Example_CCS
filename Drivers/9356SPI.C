///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////   This driver is deprecated and provided for backwards            ////
////   compatibility.  It is strongly suggested you use the 9356.c     ////
////   driver instead.  The 9356.c driver uses the CCS C Compiler      ////
////   #use spi() library, which is extremely flexible.  #use spi()    ////
////   allows the user to specify many options, including SPI mode,    ////
////   speed, # of bits, and which pins to use.  #use spi() will       ////
////   automatically use hardware SPI peripheral, if configured        ////
////   to do so, otherwise will fallback and manually big-bang the     ////
////   GPIO for SPI.                                                   ////
////                                                                   ////
////   CCS also provides 9356bitbang.c driver, in which the GPIO       ////
////   pins are manually bit banged.                                   ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
#warning This driver is deprecated.  See comments above.

///////////////////////////////////////////////////////////////////////////
////   Library for a MicroChip 93C56 configured for a x8 org           ////
////   Uses hardware SSP unit.                                         ////
////                                                                   ////
////   init_ext_eeprom();    Call before the other functions are used  ////
////                                                                   ////
////   write_ext_eeprom(a, d);  Write the byte d to the address a      ////
////                                                                   ////
////   d = read_ext_eeprom(a);   Read the byte d from the address a    ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services           ////
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


#define EEPROM_ADDRESS BYTE
#define EEPROM_SIZE    256


void init_ext_eeprom() {
   int1 i;

   output_low(EEPROM_DI);
   output_low(EEPROM_CLK);
   output_low(EEPROM_SELECT);
   i=input(EEPROM_DO);

   setup_spi(SPI_MASTER | SPI_H_TO_L | SPI_CLK_DIV_16);

   output_high(EEPROM_SELECT);
   spi_write(0x9);
   spi_write(0x80);
   output_low(EEPROM_SELECT);
}


void write_ext_eeprom(EEPROM_ADDRESS address, BYTE data) {

   output_high(EEPROM_SELECT);
   spi_write(0xa);
   spi_write(address);
   spi_write(data);
   output_low(EEPROM_SELECT);
   delay_ms(11);
}


BYTE read_ext_eeprom(EEPROM_ADDRESS address) {
   BYTE data;

   output_high(EEPROM_SELECT);
   spi_write(0x18);
   spi_write(address);
   data=spi_read(0);
   output_low(EEPROM_SELECT);

   return(data);
}
