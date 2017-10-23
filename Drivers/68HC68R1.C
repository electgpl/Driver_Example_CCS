///////////////////////////////////////////////////////////////////////////
////   Library for the CDP68HC68R1 128 x 8 bit SRAM                    ////
////                                                                   ////
////   short InitSRAM(); Sets RAM locations to 0 & verify then sets    ////
////                    locations to FF and verify. Returns TRUE if ok ////
////                                                                   ////
////   WRITE_EXT_SRAM(Address, Data);  Write Data to the Address       ////
////                                                                   ////
////   WRITE_EXT_SRAM_STRING(Address,ptrData); This will write a       ////
////                                 string of data starting at        ////
////                                 address faster than single writes ////
////                                                                   ////
////   byte READ_EXT_SRAM(Address); Read a byte from the Address       ////
////                                                                   ////
////   READ_EXT_SRAM_STRING(Address,ptrData); This will read a string  ////
////                                 of data into array 'ptrData' at   ////
////                                 address faster than single reads  ////
////                                                                   ////
////   The main program may define SRAM_select, SRAM_SS, SRAM_CE       ////
////   SRAM_SCK, SRAM_MOSI, & SRAM_MISO to override the defaults below ////
////                                                                   ////
////   With SCK starting low data is read into the SRAM on the falling ////
////   edge and written from SRAM on the rising edge                   ////
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

/* The Read/writefunctions are ONLY for reading/writing strings, not data.
   Reading in or writing out other than a keyboard character will terminate
   the operation. When sing these functions, the address is automatically
   incremented by the SRAM thus saving time. Keep in mind if an access is
   done beyond the last address, the SRAM will roll the address over to 0.
*/

#ifndef SRAM_SELECT

#define SRAM_SS     PIN_B7 // Board Terminal pin 54
#define SRAM_CE     PIN_B6 // Board Terminal pin 53
#define SRAM_SCK    PIN_B5 // Board Terminal pin 52
#define SRAM_MOSI   PIN_B4 // Board Terminal pin 51
#define SRAM_MISO   PIN_B3 // Board Terminal pin 50

#endif

void WRITE_EXT_SRAM(BYTE Address, BYTE DataIn);
void WRITE_EXT_SRAM_STRING(BYTE Address, char* ptrData);
BYTE READ_EXT_SRAM(BYTE Address);
void READ_EXT_SRAM_STRING(BYTE Address, char* ptrString);
void EnableSRAM(short State);

BOOLEAN InitSRAM()      // write 00 into all locations & verify then
{                    // write FF into all locations & verify
   BYTE Cnt;

   for(Cnt = 0; Cnt < 0x7F; Cnt++) // write & check 0s
   {
      WRITE_EXT_SRAM(Cnt,0);
      if(READ_EXT_SRAM(Cnt) != 0)
         return FALSE;
   }
   for(Cnt = 0; Cnt < 0x7F; Cnt++) // write & check FFs
   {
      WRITE_EXT_SRAM(Cnt,0xFF);
      if(READ_EXT_SRAM(Cnt) != 0xFF)
         return FALSE;
   }
   return TRUE;
}


void WRITE_EXT_SRAM(BYTE Address, BYTE DataIn)
{
   BYTE Cnt,Data;

   EnableSRAM(TRUE);

   // Send Write Address       // write: bit 7 = 1
   Data = 0x80;               // other bits are address
   Data |= Address;
   EnableSRAM(TRUE);
   for(Cnt = 8; Cnt > 0; Cnt--)
   {
      output_bit(SRAM_MOSI,bit_test(Data,(Cnt - 1)));
      output_high(SRAM_SCK);
      output_low(SRAM_SCK);
   }

   // Write DataIn to SRAM
   for(Cnt = 8; Cnt > 0; Cnt--)     // shift in bits 7 - 0
   {
      output_bit(SRAM_MOSI,bit_test(DataIn,(Cnt - 1)));
      output_high(SRAM_SCK);
      output_low(SRAM_SCK);
   }
   EnableSRAM(FALSE);
}


BYTE READ_EXT_SRAM(BYTE Address)
{
   BYTE Cnt,Data;

   EnableSRAM(TRUE);

   // Send Read Address       // write: bit 7 = 0
   Data = 0;                  // other bits are address
   Data |= Address;
   EnableSRAM(TRUE);
   for(Cnt = 8; Cnt > 0; Cnt--)
   {
      output_bit(SRAM_MOSI,bit_test(Data,(Cnt - 1)));
      output_high(SRAM_SCK);
      output_low(SRAM_SCK);
   }

   // Read each bit from address
   Data = 0;
   for(Cnt = 8; Cnt > 0; Cnt--)     // shift in bits 7 - 0
   {
      output_high(SRAM_SCK);
      output_low(SRAM_SCK);
      if(input(SRAM_MISO))
         bit_set(Data,(Cnt - 1));
   }
   EnableSRAM(FALSE);
   return(Data);
}

void WRITE_EXT_SRAM_STRING(BYTE Address, char* ptrData)
{
   BYTE Cnt,Data;

   EnableSRAM(TRUE);

   // Send Write Address       // write: bit 7 = 1
   Data = 0x80;               // other bits are address
   Data |= Address;
   EnableSRAM(TRUE);
   for(Cnt = 8; Cnt > 0; Cnt--)
   {
      output_bit(SRAM_MOSI,bit_test(Data,(Cnt - 1)));
      output_high(SRAM_SCK);
      output_low(SRAM_SCK);
   }

   // Write ptrData to SRAM
   Data = *ptrData;
   while(Data != 0)
   {
      for(Cnt = 8; Cnt > 0; Cnt--)     // shift in bits 7 - 0
      {
         output_bit(SRAM_MOSI,bit_test(Data,(Cnt - 1)));
         output_high(SRAM_SCK);
         output_low(SRAM_SCK);
      }
      ++ptrData;
      Data = *ptrData;
   }

   EnableSRAM(FALSE);
}

void READ_EXT_SRAM_STRING(BYTE Address, char* ptrString)
{
	BYTE Cnt, Data;
   char* ptrStartString;
   ptrStartString = ptrString;

	if(!*ptrString)
		return;

   EnableSRAM(TRUE);

   // Send Read Address       // write: bit 7 = 0
   Data = 0;                  // other bits are address
   Data |= Address;
   EnableSRAM(TRUE);
   for(Cnt = 8; Cnt > 0; Cnt--)
   {
      output_bit(SRAM_MOSI,bit_test(Data,(Cnt - 1)));
      output_high(SRAM_SCK);
      output_low(SRAM_SCK);
   }

   // Read ptrString to SRAM
   do
   {
      Data = 0;
      for(Cnt = 8; Cnt > 0; Cnt--)     // shift in bits 7 - 0
      {
         output_high(SRAM_SCK);
         output_low(SRAM_SCK);
         if(input(SRAM_MISO))
            bit_set(Data,(Cnt - 1));
      }
      if(Data < 32 || Data > 126)      // if not a keyboard char
         *ptrString = 0;
      *ptrString = Data;
      ptrString++;
   }while(*ptrString != 0);
   *ptrString = 0;                       // make sure string terminates w/ 0
   ptrString = &ptrStartString;
   EnableSRAM(FALSE);
}

void EnableSRAM(short State)
{
   output_bit(SRAM_SCK,0);
   output_bit(SRAM_CE,State);
   output_bit(SRAM_SS,!State);
   output_bit(SRAM_MOSI,0);
}
