/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////                         EX_SPI_SLAVE.C                          ////
////                                                                 ////
////  Uses the PIC's SPI peripheral in slave mode to emulate a 9356  ////
////  SPI EEPROM.  This example is the counterpart to EX_SPI.C.      ////
////  Run EX_SPI_SLAVE.C on one PIC and connect it to another PIC    ////
////  running EX_SPI.                                                ////
////                                                                 ////
////  This example will work with the PCM and PCH compilers.  The    ////
////  following conditional compilation lines are used to include a  ////
////  valid device for each compiler.  Change the device, clock and  ////
////  RS232 pins for your hardware if needed.                        ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2012 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#if defined(__PCM__)
#include <16F887.h>
#fuses NOWDT
#use delay(crystal=20MHz)
#define EEPROM_BYTES 80

#elif defined(__PCH__)
#include <18F45K22.h>
#fuses NOWDT
#use delay(crystal=16MHz)
#define EEPROM_BYTES 256

#elif defined(__PCD__)
#include <24FJ128GA006.h>
#fuses NOWDT
#use delay(crystal=20MHz)
#define EEPROM_BYTES 256
#endif

#if defined(__PCD__)
#use spi(SLAVE, SPI2, MODE=1, bits=8)
#else
#use spi(SLAVE, SPI1, MODE=1, bits=8)
#endif

#define EEPROM_COMMAND_WRITE  0x0A
#define EEPROM_COMMAND_READ   0x18
#define EEPROM_COMMAND_INIT   0x09

enum {
   EEPROM_STATE_COMMAND,
   EEPROM_STATE_ADDRESS,
   EEPROM_STATE_WRITE_DATA,
   EEPROM_STATE_READ_DATA,
} EepromState = EEPROM_STATE_COMMAND;

unsigned int8 Memory[EEPROM_BYTES];

#if defined(__PCD__)
#INT_SPI2
#else
#INT_SSP
#endif
void spi_slave_isr(void)
{
   static unsigned int8 LastCommand = EEPROM_COMMAND_WRITE;
   static unsigned int8 Address;
   unsigned int8 Data;

   Data = spi_xfer_in();

   switch(EepromState)
   {
      case EEPROM_STATE_COMMAND:
         if((Data == EEPROM_COMMAND_WRITE) || (Data == EEPROM_COMMAND_READ))
         {
            LastCommand = Data;
            EepromState++;
         }
         break;
      case EEPROM_STATE_ADDRESS:
         Address = Data;
         if(LastCommand == EEPROM_COMMAND_WRITE)
            EepromState = EEPROM_STATE_WRITE_DATA;
         else
         {
            spi_prewrite(Memory[Address]);
            EepromState = EEPROM_STATE_READ_DATA;
         }
         break;
      case EEPROM_STATE_WRITE_DATA:
         if(Address < EEPROM_BYTES)
            Memory[Address] = Data;
      case EEPROM_STATE_READ_DATA:
         EepromState = EEPROM_STATE_COMMAND;
         break;
   }
}

/*
void main(void)
PURPOSE: Peripheral Initialization
*/
void main(void)
{
  #if defined(__PCD__)
   enable_interrupts(INT_SPI2);
   enable_interrupts(INTR_GLOBAL);
  #else
   enable_interrupts(INT_SSP);
   enable_interrupts(GLOBAL);
  #endif

   while(true)
   {
   }
}
