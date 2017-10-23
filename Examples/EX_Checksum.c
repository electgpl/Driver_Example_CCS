/////////////////////////////////////////////////////////////////////////
////                         EX_CHECKSUM.C                           ////
////                                                                 ////
////  This program is used to generate a checksum for the code and   ////
////  store it in the ID location. This checksum is verified during  ////
////  run-time by reading the entire program memory and performing   ////
////  a match between this additions checksum, and the checkum       ////
////  stored at the ID location for PCH parts and a modified checksum////
////  stored at last location in memory for PCM parts                ////
////                                                                 ////
////  This example will work with the PCM and PCH compilers.         ////
////  The following conditional compilation lines are used to        ////
////  include a valid device for each compiler.  Change the device,  ////
////  clock and RS232 pins for your hardware if needed.              ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2006 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////



#if defined(__PCM__)
#include <16F877A.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7, BRGH1OK)
#id checksum_program
#rom getenv("Program_memory")-1 = CHECKSUM

#elif defined(__PCH__)
#include <18F4520.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7, BRGH1OK)
#id checksum_program
#endif


/*
int perform_checksum()
This will return a 1 if checksum is verified and a 0 if checksum fails.
PARAM : none
RETURNS : Boolean
*/
int perform_checksum()
      {
         int16  checksum, check_id, check_temp, check_last_loc;
         int32 i;

          checksum = 0; // Initialize the addition checksum to zero at start

          check_id = 0; // Initialize variable to read ID location - only for PIC18

          check_last_loc = 0; // Last location data for PCM parts

          check_temp = 0;  // Temporary variable for computation


          #if defined(__PCH__)
         // The following code will return an additions checksum for entire program memory

          for(i=0;i<getenv("Program_memory");i+=2)
         {
            check_temp=read_program_eeprom(i);  //read 16 bits from program memory
            checksum += (int8)check_temp;       //add each byte to checksum
            checksum += (int8)(check_temp>>8);  //
         }
         printf("\n\r Addition checksum : %LX", checksum);
         // The following routine will read the checksum stored using #ID checksum_program command
         for(i=0;i<=3;i++)
         {
         check_id=check_id*16+(read_program_eeprom(0x200000+i*2) & 15); // mask so only lower nibble is used.
         }
         printf("\n\r Checksum from ID : %LX", check_id);

         if(check_id==checksum)
         return 1;
         else
         return 0;
          #endif

      #if defined(__PCM__)
      // The following code will return an additions checksum for entire program memory
        for(i=0;i<getenv("Program_memory")-1;i++)
         {
           checksum+=read_program_eeprom(i);  //read 16 bits from program memory)
         }
          printf("\n\r Addition checksum : %LX", checksum);

         //Read last location in memory
         check_last_loc = read_program_eeprom(getenv("Program_memory")-1);

         printf("\n\r Last Location : %LX\r\n",check_last_loc);

         check_temp = 0x1248-checksum;

         check_temp&=0x3FFF;  // Mask the 2 MSB's

         printf("\n\r Modified Checksum : %LX\r\n",check_temp);
         if(check_temp == check_last_loc)
         return 1;
         else
         return 0;
       #endif


}


void main() {

   if(perform_checksum())
      {
         printf("\n\r Checksum Verified");
      }
   else
      {
         printf("\n\r Checksum FAILED");
      }

   while(1);

}

