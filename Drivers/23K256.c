////////////////////////////////////////////////////////////////////////////////
////                              23K256.c                                  ////
////      Driver for Microchip's 256K SPI Bus Low-Power Serial SRAM         ////
////                                                                        ////
////     init_ext_sram();        Call before the other functions are used   ////
////                                                                        ////
////     write_ext_sram(a,d);    Write the byte d to the address a          ////
////                                                                        ////
////     d=read_ext_eeprom(a);   Read the byte d from the address a         ////
////                                                                        ////
////   The main program may define SRAM_CS                                  ////
////                                                                        ////
////                            Pin Layout                                  ////
////   --------------------------------------------------------------       ////
////   |    __                                                       |      ////
////   | 1: CS   Chip Select Input | 8: VCC   +3.3V                  |      ////
////   |                           |    ____                         |      ////
////   | 2: SO   Serial Data Output| 7: HOLD  Hold Input             |      ////
////   |                           |                                 |      ////
////   | 3: NC   Not Connected     | 6: SCK   Serial Clock Input     |      ////
////   |                           |                                 |      ////
////   | 4: VSS  GND               | 5: SI    Serial Data Input      |      ////
////   --------------------------------------------------------------       ////
////     __                                                                 ////
////     CS connected to PIN_D0 (pin 19 of PIC18F4620)                      ////
////     S0 connected to PIN_C4 (pin 23 of PIC18F4620)                      ////
////     SI connected to PIN_C5 (pin 24 of PIC18F4620)                      ////
////     SCK connected to PIN_C3 (pin 18 of PIC18F4620)                     ////
////     HOLD connected to +3.3V                                            ////
////                                                                        ////
////////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2009 Custom Computer Services               ////
//// This source code may only be used by licensed users of the CCS C       ////
//// compiler.  This source code may only be distributed to other licensed  ////
//// users of the CCS C compiler.  No other use, reproduction or            ////
//// distribution is permitted without written permission. Derivative       ////
//// programs created using this software in object code form are not       ////
//// restricted in any way.                                                 ////
////////////////////////////////////////////////////////////////////////////////

#ifndef SRAM_CS
   #define SRAM_CS PIN_D0        //sets SRAM chip select to be connected to PIC pin_D0,
#endif                           //different pin can be used

#use spi(MASTER, SPI1, MODE=0, bits=8, STREAM=STREAM_SRAM) //sets up spi to communicate with sram
                                                  //Sets PIC to be Master,
                                                  //to use SPI pins for SDI, SD0, and SCK, use PIC
                                                  //SPI hardware, and do 8 bit transfers

#define SPI_XFER_SRAM(c)   spi_xfer(STREAM_SRAM, c)
#define SPI_READ_SRAM()    SPI_XFER_SRAM(0xFF)
                                      
//Check to see if SRAM is in Byte mode and Sets to Byte mode if not
void init_ext_sram(void)
{
   int8 status;
   
   output_high(SRAM_CS);
   delay_ms(100);
   
   output_low(SRAM_CS);
   SPI_XFER_SRAM(0x05);                 //sends read status command
   status=SPI_READ_SRAM();              //reads status of SRAM
   output_high(SRAM_CS);
   if(status!=0)                    //checks to see if SRAM is in Byte mode
   {
      output_low(SRAM_CS);
      SPI_XFER_SRAM(0x01);              //sends write status command
      SPI_XFER_SRAM(0x00);              //sends value to set SRAM in Byte mode
      output_low(SRAM_CS);
   }
}

// Writes a byte of data to specified address
void write_ext_sram(int16 address,int8 data)
{
   output_low(SRAM_CS);             
   SPI_XFER_SRAM(0x02);                 //sends write command
   SPI_XFER_SRAM(make8(address,1));     //sends MS Byte of address
   SPI_XFER_SRAM(make8(address,0));     //sends LS Byte of address
   SPI_XFER_SRAM(data);                 //writes the byte of data
   output_high(SRAM_CS);
}

// Reads a byte of data from specified address
int8 read_ext_sram(int16 address)
{
   int8 data;
   
   output_low(SRAM_CS);
   SPI_XFER_SRAM(0x03);                 //sends read command
   SPI_XFER_SRAM(make8(address,1));     //sends MS Byte of address
   SPI_XFER_SRAM(make8(address,0));     //sends LS Byte of address
   data=SPI_READ_SRAM();                //reads the byte of data
   output_high(SRAM_CS);
   
   return(data);
}
