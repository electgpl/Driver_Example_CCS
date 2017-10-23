///////////////////////////////////////////////////////////////////////////
////                         loader_pcd.c                              ////
////                                                                   ////
//// This driver will take an Intel 8-bit Hex file over RS232 channels ////
//// and modify the flash program memory with the new code.            ////
////                                                                   ////
//// After each good line, the loader sends an ACKLOD character.  The  ////
//// driver uses XON/XOFF flow control.  Also, any buffer on the PC    ////
//// UART must be turned off, or to its lowest setting, otherwise it   ////
//// will miss data.                                                   ////
////                                                                   ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2013 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#ifndef LOADER_PAGES
 #if getenv("FLASH_ERASE_SIZE") == 128
  #define LOADER_PAGES 13
 #else
  #define LOADER_PAGES 1
 #endif
#endif

#ifndef LOADER_SIZE
 #if ((getenv("PROGRAM_MEMORY") % (getenv("FLASH_ERASE_SIZE")/2)) !=0 )
  #define LOADER_SIZE (((getenv("PROGRAM_MEMORY") % (getenv("FLASH_ERASE_SIZE")/2)) - 1) + ((LOADER_PAGES - 1) * (getenv("FLASH_ERASE_SIZE")/2)))
 #else
  #define LOADER_SIZE ((LOADER_PAGES * (getenv("FLASH_ERASE_SIZE")/2)) - 1)
 #endif
 #define LOADER_END  (getenv("PROGRAM_MEMORY") - 1)
 #define LOADER_ADDR (LOADER_END - LOADER_SIZE)
#endif

#define BUFFER_LEN_LOD 64

#define ACKLOD 0x06
#define XON    0x11
#define XOFF   0x13

#ifndef BOOTLOADER_AT_START
 #org LOADER_ADDR+10, LOADER_END auto=0 default
#endif

unsigned int8 atoi_b16(char *s);

void real_load_program (void)
{
   unsigned int1  do_ACKLOD, done=FALSE;
   unsigned int8  checksum, line_type;
   unsigned int16 l_addr,h_addr=0;
   unsigned int32 addr;
   #if getenv("FLASH_ERASE_SIZE") != getenv("FLASH_WRITE_SIZE")
      unsigned int32 next_addr;
   #endif
   unsigned int8  dataidx, i;
   unsigned int16 count=0;
   unsigned int8  data[32];
   unsigned int16  buffidx;
   char buffer[BUFFER_LEN_LOD];

   while (!done)  // Loop until the entire program is downloaded
   {
      buffidx = 0;  // Read into the buffer until 0x0D ('\r') is received or the buffer is full
      do
      {
         buffer[buffidx] = getc();
      }
      while ( (buffer[buffidx++] != '\r') && (buffidx <= BUFFER_LEN_LOD) );

      putc(XOFF);  // Suspend sender

      do_ACKLOD = TRUE;

      // Only process data blocks that start with ':'
      if (buffer[0] == ':')
      {
         count = atoi_b16 (&buffer[1]);  // Get the number of bytes from the buffer

         // Get the lower 16 bits of address
         l_addr = make16(atoi_b16(&buffer[3]),atoi_b16(&buffer[5]));

         line_type = atoi_b16 (&buffer[7]);

         addr = ((make32(h_addr,l_addr))/2);

         checksum = 0;  // Sum the bytes to find the check sum value
         for (i=1; i<(buffidx-3); i+=2)
            checksum += atoi_b16 (&buffer[i]);
         checksum = 0xFF - checksum + 1;

         if (checksum != atoi_b16 (&buffer[buffidx-3]))
            do_ACKLOD = FALSE;
         else
         {
            // If the line type is 1, then data is done being sent
            if (line_type == 1)
            {
               done = TRUE;
            }
            else if (line_type == 4)
               h_addr = make16(atoi_b16(&buffer[9]), atoi_b16(&buffer[11]));
            else if (line_type == 0)
            {

               if (((addr < LOADER_ADDR) || (addr > LOADER_END)) && addr < getenv("PROGRAM_MEMORY"))
               {        // Loops through all of the data and stores it in data
                        // The last 2 bytes are the check sum, hence buffidx-3

                  for (i = 9,dataidx=0; i < buffidx-3; i += 2)
                     data[dataidx++]=atoi_b16(&buffer[i]);

                  #if getenv("FLASH_ERASE_SIZE") > getenv("FLASH_WRITE_SIZE")
                     if ((addr!=next_addr) && (addr > (next_addr + ((getenv("FLASH_ERASE_SIZE")/2) - (next_addr % (getenv("FLASH_ERASE_SIZE")/2))))) && (addr&(getenv("FLASH_ERASE_SIZE")/2-1)!=0))
                     {
                        #ifdef BOOTLOADER_AT_START
                           #if ((getenv("PROGRAM_MEMORY") % (getenv("FLASH_ERASE_SIZE")/2)) != 0)
                              if (addr > (getenv("PROGRAM_MEMORY") - (getenv("PROGRAM_MEMORY") % (getenv("FLASH_ERASE_SIZE")/2))))
                              {
                                 read_program_memory(getenv("PROGRAM_MEMORY"), buffer, ((getenv("FLASH_ERASE_SIZE")/2) - (getenv("PROGRAM_MEMORY") % (getenv("FLASH_ERASE_SIZE")/2)))*2);
                                 erase_program_memory(addr);
                                 write_program_memory(getenv("PROGRAM_MEMORY"), buffer, ((getenv("FLASH_ERASE_SIZE")/2) - (getenv("PROGRAM_MEMORY") % (getenv("FLASH_ERASE_SIZE")/2)))*2);
                              }
                              else
                           #endif
                        #endif
                                 erase_program_memory(addr);
                     }
                     next_addr = addr + count/2;
                  #endif

                  #ifdef BOOTLOADER_AT_START
                     #if ((getenv("PROGRAM_MEMORY") % (getenv("FLASH_ERASE_SIZE")/2)) != 0)
                        if (addr == (getenv("PROGRAM_MEMORY") - (getenv("PROGRAM_MEMORY") % (getenv("FLASH_ERASE_SIZE")/2))))
                        {
                           read_program_memory(getenv("PROGRAM_MEMORY"), buffer, ((getenv("FLASH_ERASE_SIZE")/2) - (getenv("PROGRAM_MEMORY") % (getenv("FLASH_ERASE_SIZE")/2)))*2);
                           write_program_memory(addr, data, count);
                           write_program_memory(getenv("PROGRAM_MEMORY"), buffer, ((getenv("FLASH_ERASE_SIZE")/2) - (getenv("PROGRAM_MEMORY") % (getenv("FLASH_ERASE_SIZE")/2)))*2);
                        }
                        else
                     #endif
                  #endif
                           write_program_memory(addr, data, count);
               }
            }
         }

         if (do_ACKLOD)
            putc(ACKLOD);

         putc(XON);
      }
      else
      {
         putc(ACKLOD);
         putc(XON);
      }
   }
   putc(ACKLOD);
   putc(XON);

   printf("\r\nResetting.....");

   reset_cpu();
}

unsigned int8 atoi_b16(char *s) {  // Convert two hex characters to a int8
   unsigned int8 result = 0;
   unsigned int8 i;

   for (i=0; i<2; i++,s++)  {
      if (*s >= 'A')
         result = 16*result + (*s) - 'A' + 10;
      else
         result = 16*result + (*s) - '0';
   }

   return(result);
}

#ifndef BOOTLOADER_AT_START
#org default
#org LOADER_ADDR, LOADER_ADDR+9
#endif

void load_program(void)
{
   real_load_program();
}

