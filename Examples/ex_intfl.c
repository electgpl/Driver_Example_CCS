///////////////////////////////////////////////////////////////////////////
////                             EX_INTFL.C                            ////
////                                                                   ////
//// This example demonstrates the use of the write_program_eeprom()   ////
//// and the read_program_eeprom() functions on the PIC16 and PIC18.   ////
//// The PIC16 series uses 16 bit addresses and eeprom is word         ////
//// addressable. The PIC18 uses 32 bit addresses and eeprom is byte   ////
//// addressable.                                                      ////
////                                                                   ////
//// This example will work with the PCM and PCH compilers.  The       ////
//// following conditional compilation lines are used to include a     ////
//// valid device for each compiler.  Change the device, clock and     ////
//// RS232 pins for your hardware if needed.                           ////
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

#if defined(__PCM__)
#include <16F877.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP,NOWRT
#use delay(clock=20000000)
#use rs232(xmit=PIN_C6, rcv=PIN_C7, baud=9600)  // Jumpers: 8 to 11, 7 to 12

#elif defined(__PCH__)
#include <18F452.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP,NOWRTD
#use delay(clock=20000000)
#use rs232(xmit=PIN_C6, rcv=PIN_C7, baud=9600)  // Jumpers: 8 to 11, 7 to 12
#endif

#include <stdlib.h>
#include <input.c>

#define start_addr getenv("PROGRAM_MEMORY") - 0x200
#org start_addr,start_addr+64 {}

void main()
{
   int   location, i, j;
   byte upper_byte, lower_byte;
   int16 data;

   #if defined(__PCH__)
      int32 addr;                               // 32 bit int to store an address
      erase_program_eeprom(start_addr);         // Makes 64 bytes 0xFFFF
   #elif defined(__PCM__)
      int16 addr;                               // 16 bit int to store an address
   #endif

   while(TRUE) {                                // Loop forever
      for(i=0; i<64; i+=8) {                    // Loop for 8 rows
         printf("\r\n");                        // Output a newline
         for(j=0; j<8; ++j) {                   // Loop for 8 columns
            addr = start_addr + (i+j) * (3-getenv("BYTES_PER_ADDRESS"));
            data = read_program_eeprom(addr);   // Read data from eeprom at addr
            printf("%u:%4LX\t", i+j, data);     // Output to console
         }
      }

      do{
         printf("\r\nWhich location do you want to change? (0-63): ");
         location = get_int();                  // Get the location
      } while(location < 0 || location > 63);   // Loop until valid

      printf("\r\nEnter a 16 bit hex value: "); // Ask for 16 bit hex value
      upper_byte = gethex();                    // Get upper byte
      lower_byte = gethex();                    // Get lower byte

      // Write the input data to program eeprom.
      // *Note* PIC16 can only save the lower 14 bits.
      //        PIC18 can only write 0s, which is why the erase is done first.
      addr = start_addr + location * (3-getenv("BYTES_PER_ADDRESS"));
      write_program_eeprom(addr, make16(upper_byte, lower_byte));
      printf("\r\n");
   }
}
