/////////////////////////////////////////////////////////////////////////
////                      ex_mmcsd.c                                 ////
////                                                                 ////
//// Similar to ex_extee.c, an example that demonstrates             ////
//// writing and reading to an MMC/SD card.                          ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 2007 Custom Computer Services              ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

//These settings are for the CCS PICEEC development kit which contains
//an MMC/SD connector.
#include <18F67J60.h>
#fuses NOWDT, HS, NOPROTECT
#use delay(clock=25M)

#use rs232(baud=9600, UART1, errors)

#include <stdlib.h> // for atoi32

//meda library, a compatable media library is required for FAT.
#use fast_io(c)
#define MMCSD_PIN_SCL     PIN_C3 //o
#define MMCSD_PIN_SDI     PIN_C4 //i
#define MMCSD_PIN_SDO     PIN_C5 //o
#define MMCSD_PIN_SELECT  PIN_C2 //o
#include <mmcsd.c>

#include <input.c>

void main(void)
{
   BYTE value, cmd;
   int32 address;

   printf("\r\n\nex_mmcsd.c\r\n\n");
   
   if (mmcsd_init())
   {
      printf("Could not init the MMC/SD!!!!");
      while(TRUE);
   }
   
   do {
      do {
         printf("\r\nRead or Write: ");
         cmd=getc();
         cmd=toupper(cmd);
         putc(cmd);
      } while ( (cmd!='R') && (cmd!='W') );

      printf("\n\rLocation: ");

      address = gethex();
      address = (address<<8)+gethex();

      if(cmd=='R')
      {  
         mmcsd_read_byte(address, &value);
         printf("\r\nValue: %X\r\n", value);
      }

      if(cmd=='W') {
         printf("\r\nNew value: ");
         value = gethex();
         printf("\n\r");
         mmcsd_write_byte(address, value);
         mmcsd_flush_buffer();
      }
   } while (TRUE);   
}

