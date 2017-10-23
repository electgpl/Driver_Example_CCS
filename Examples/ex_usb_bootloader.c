///////////////////////////////////////////////////////////////////////////
////                    EX_USB_BOOTLOADER.C                            ////
////                                                                   ////
////  This program is an example stand alone USB bootloader.  It uses  ////
////  the communication device class (CDC) that creates a virtual COM  ////
////  port on your PC.  The bootloader contains it's own set of USB    ////
////  and bootloader code in low memory, the high memory will contain  ////
////  the application (and if you are developing a USB application,    ////
////  this will hold it's own USB code).                               ////
////                                                                   ////
////  This program must be loaded into a target chip using a device    ////
////  programmer.  Afterwards this program may be used to load new     ////
////  versions of the application program.  There is a specific        ////
////  protocol used by the bootloader, SIOW.EXE uses this protocol     ////
////  if you use the 'Download Software' option.                       ////
////                                                                   ////
////  Compiling this program gives you a HEX file of just the loader.  ////
////  To load the application you have two choices: a.) use the loader ////
////  to load the application, or b.) merge the application HEX and    ////
////  loader HEX into one production HEX.  The #import() CCS command   ////
////  will help you perfrom the latter.  Be aware that if you do the   ////
////  former the PIC will get it's configuration bit settings (fuses)  ////
////  from the loader and not the application (this bootloader does    ////
////  not change the configuration bits)                               ////
////                                                                   ////
////  To create an application that is compatible with this loader     ////
////  simply #include usb_bootloader.h into your application.  This    ////
////  loader does support interrupts in your application.              ////
////                                                                   ////
////  This bootloader is designed to detect pin A4 low on reset (this  ////
////  is the push-button on the CCS USB Development kit).  If A4 is    ////
////  low it starts the USB CDC stack and checks for bootload          ////
////  information over USB.  Otherwise the application program is      ////
////  started.                                                         ////
////                                                                   ////
////  This is a port of ex_bootloader.c, which is a serial bootloader. ////
////  Since CDC makes it easy to port legacy applications, the         ////
////  previous example makes the bulk of this code.                    ////
////                                                                   ////
//// This file is part of CCS's PIC USB driver code.  See USB.H        ////
//// for more documentation and a list of examples.                    ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////                                                                   ////
//// VERSION HISTORY                                                   ////
////                                                                   ////
//// July 9th, 2012:                                                   ////
////  Added support for PCD.                                           ////
////  No longer uses interrupts (polling).  This simplifies the        ////
////     interrupt support in your application, because they don't     ////
////     need to be shared.                                            ////
////                                                                   ////
//// Oct 15th, 2010:                                                   ////
////  Potential bug in CDC that causes lost PIC->PC chars resolved?    ////
////  Added initial 18F47J53 family support.                           ////
////  Added USB_ISR_POLLING support.  Define this and interrupts       ////
////     will not be used.  usb_task() must be called periodically     ////
////     in your main loop.  If it is not called faster than once      ////
////     per millisecond, USB may not work (PIC18 and PIC24 only).     ////
////                                                                   ////
//// April 28th, 2010:                                                 ////
////  Added ex_usb_common.h.                                           ////
////  Initial support for CCS PIC24USB board.                          ////
////                                                                   ////
//// March 5th, 2009:                                                  ////
////   Cleanup for Wizard.                                             ////
////   PIC24 Initial release.                                          ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2009 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS    ////
//// C compiler.  This source code may only be distributed to other    ////
//// licensed users of the CCS C compiler.  No other use,              ////
//// reproduction or distribution is permitted without written         ////
//// permission.  Derivative programs created using this software      ////
//// in object code form are not restricted in any way.                ////
///////////////////////////////////////////////////////////////////////////

#include <ex_usb_common.h>

/*
 Configure, then load the bootloader definitions
*/

#define _bootloader
#include "usb_bootloader.h"

// Includes all USB code and interrupts, as well as the CDC API
#include <usb_cdc.h>

#define ROM_BLOCK_INVALID -1

int32 rom_block_start = ROM_BLOCK_INVALID;

#if defined(__PCD__)
 #define EEPROM_ERASE_SIZE  getenv("FLASH_ERASE_SIZE")/2
#else
 #define EEPROM_ERASE_SIZE  getenv("FLASH_ERASE_SIZE")
#endif
int8 rom_block[EEPROM_ERASE_SIZE];

// see rom_w() documentation.  performs the flushing
void rom_w_flush(void)
{
   if (rom_block_start != ROM_BLOCK_INVALID)
   {
     #if !defined(__PCD__)
      erase_program_eeprom(rom_block_start);     //erase entire block
     #else
      erase_program_memory(rom_block_start);
     #endif
      write_program_memory(rom_block_start, rom_block, sizeof(rom_block));    //write modified block
      rom_block_start = ROM_BLOCK_INVALID;
   }
}

// see rom_w() documentation.  performs the writing
void rom_w_block(int32 location, char *src, int16 size)
{
   int32 block_start;
   int16 i,num;
   
   block_start = location & (~((int32)EEPROM_ERASE_SIZE-1));
   
   i = location - block_start;

   while (size) 
   {
      if (block_start != rom_block_start)
      {
         rom_w_flush();
         
         rom_block_start = block_start;
         
         read_program_memory(block_start, rom_block, sizeof(rom_block));  //read entire block to ram buffer
      }
      
      if (size>(EEPROM_ERASE_SIZE-i)) {num=EEPROM_ERASE_SIZE-i;} else {num=size;}

      memcpy(&rom_block[i], src, num);    //modify ram buffer

      src += num;
      block_start += EEPROM_ERASE_SIZE;
      i = 0;
      size -= num;
   }
}

// Write to Flash ROM.
//
// location - flash program memory address
// src - pointer to data to write
// size - number of bytes to write to flash
//
// Here is the sequence of events:
//   1.) Goes to the beginning of the first erase block for this address
//   2.) Reads n records to ram, where n is the PIC's flash erase size
//   3.) Erases block in flash
//   4.) Modifies block in RAM
//   5.) Writes changed block back to FLASH.  Writes in chunks defined by PIC's flash write size
//   6.) Goes back to step1 if there is still more data to be written
void rom_w(int32 location, char *src, int16 size)
{
   rom_w_block(location, src, size);
   rom_w_flush();
}

#define BUFFER_LEN_LOD 64

#define ACKLOD 0x06
#define XON    0x11
#define XOFF   0x13

// Convert two hex characters to a int8
unsigned int8 atoi_b16(char *s) 
{  
   char c;
   unsigned int8 result = 0;
   int i;

   for (i=0; i<2; i++,s++)  
   {
      c = *s;
      if (c >= 'A')
         result = 16*result + c - 'A' + 10;
      else
         result = 16*result + c - '0';         
   }

   return(result);
}

void load_program(void)
{
   int1  do_ACKLOD, done=FALSE;
   int8  checksum, line_type;
   int16 l_addr,h_addr=0;
   int8 to;
   int32 addr;
   int8  dataidx, i, count;
   int8  data[32];
   int  buffidx;
   char buffer[BUFFER_LEN_LOD];
   
  #if defined(__PCD__)
   unsigned int32 address_erase;
   for(address_erase=APPLICATION_START;address_erase<APPLICATION_END-EEPROM_ERASE_SIZE+1;address_erase+=EEPROM_ERASE_SIZE)
         erase_program_memory(address_erase);
  #endif
   
   while (!done)  // Loop until the entire program is downloaded
   {
      usb_task();
      
      if (!usb_cdc_kbhit())
         continue;
      
      buffidx = 0;  // Read into the buffer until 0x0D ('\r') is received or the buffer is full
      to = 250;   //250 milliseconds
      do
      {
         if (!usb_cdc_kbhit())
         {
            delay_ms(1);
            usb_task();
            to--;
            if (!to)
               break;
         }
         else
            to = 250;
         i = usb_cdc_getc();
         buffer[buffidx++] = i;
      } while ( (i != 0x0D) && (i != 0x0A) && (buffidx <= BUFFER_LEN_LOD) );
      
      if (!to)
         continue;
      
      usb_cdc_putc(XOFF);  // Suspend sender
      
      do_ACKLOD = TRUE;
      
      // Only process data blocks that start with ':'
      if (buffer[0] == ':') 
      {
         checksum = 0;  // Sum the bytes to find the check sum value
         
         for (i=1; i<(buffidx-3); i+=2)
            checksum += atoi_b16 (&buffer[i]);
         checksum = 0xFF - checksum + 1;
         
         if (checksum != atoi_b16 (&buffer[buffidx-3]))
            do_ACKLOD = FALSE;
         else
         {
            count = atoi_b16(&buffer[1]);  // Get the number of bytes from the buffer
         
            // Get the lower 16 bits of address
            l_addr = make16(atoi_b16(&buffer[3]),atoi_b16(&buffer[5]));
            
            line_type = atoi_b16 (&buffer[7]);
            
           #if defined(__PCD__)
            addr = (make32(h_addr,l_addr)/2);
           #else
            addr = make32(h_addr,l_addr);
           #endif
            
            // If the line type is 1, then data is done being sent
            if (line_type == 1)
               done = TRUE;
            else if (line_type == 4)
               h_addr = make16(atoi_b16(&buffer[9]), atoi_b16(&buffer[11]));
            else if ((line_type == 0) && (addr >= (int32)APPLICATION_START) && (addr < ((int32)APPLICATION_END)))
            {
               // Loops through all of the data and stores it in data
               // The last 2 bytes are the check sum, hence buffidx-3
               for (i = 9,dataidx=0; i < buffidx-3; i += 2)
                  data[dataidx++]=atoi_b16(&buffer[i]);
              #if !defined(__PCD__)
               rom_w_block(addr, data, count);
              #else
               write_program_memory(addr, data, count);
               addr += count;
               read_program_memory(addr, data, count);
              #endif
            }
         }
      }
      if(do_ACKLOD)
         usb_cdc_putc(ACKLOD);
      
      usb_cdc_putc(XON);
   }
  #if !defined(__PCD__)
   rom_w_flush();
  #endif
   
   while(!usb_cdc_put_buffer_free())
      uart_task();
   
   usb_cdc_putc(ACKLOD);
   usb_cdc_putc(XON);
   delay_ms(2000);   //give time for packet to flush
   reset_cpu();
}

void main(void)
{  
   HW_INIT();

   //we use a button as an event to determine if we should start the USB CDC
   //bootloader.  if button is not pressed then goto the application, else if 
   //button is pressed then do the bootloader.
   if(BUTTON_PRESSED())
   {
      usb_init_cs();
      
      while(!usb_enumerated())
      {
         // necessary for polling function to work
        #ifdef USB_ISR_POLLING
         usb_task();
        #endif
      }
      LED_ON(LED3);
      load_program();
   }
   
  #asm
   goto APPLICATION_START
  #endasm
}

#int_default
void isr(void)
{
   jump_to_isr(APPLICATION_ISR);
}

