///////////////////////////////////////////////////////////////////////////
////   Library for a MicroChip 25C080                                  ////
////                                                                   ////
////   init_ext_eeprom();    Call before the other functions are used  ////
////                                                                   ////
////   write_ext_eeprom(a, d);  Write the byte d to the address a      ////
////                                                                   ////
////   d = read_ext_eeprom(a);   Read the byte d from the address a    ////
////                                                                   ////
////  25C080 pin  Name     Protoboard connection                       ////
////     1        CS(NOT)     B7       54                              ////
////     2        SO          B4       51                              ////
////     3        WP(NOT)     5v       28                              ////
////     4        Vss         Gnd      27                              ////
////     5        SI          B5       52                              ////
////     6        SCK         B6       53                              ////
////     7        HOLD(NOT)   5v       28                              ////
////     8        Vcc         5v       28                              ////
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


#define EEPROM_CS    PIN_B7
#define EEPROM_SCK   PIN_B6
#define EEPROM_SI    PIN_B5
#define EEPROM_SO    PIN_B4

#define EEPROM_ADDRESS unsigned long
#define EEPROM_SIZE    1024

void init_ext_eeprom(void);
void EnableEEPROM(short State);
void WRITE_EXT_EEPROM(long int Address, BYTE DataIn);
BYTE READ_EXT_EEPROM(long int Address);
void WriteByte(BYTE Data);       // For this file's use only

void Delay()
{
   BYTE Cnt;
   for(Cnt = 0; Cnt < 10; Cnt++);
}

void WRITE_EXT_EEPROM(long int Address, BYTE DataIn)
{
   BYTE Cnt,Data;

   EnableEEPROM(TRUE);

   // Send Write Enable Sequence
   Data = 6;
   for(Cnt = 8; Cnt > 0; Cnt--)
   {
      output_bit(EEPROM_SI,bit_test(Data,(Cnt - 1)));
      output_high(EEPROM_SCK);
      output_low(EEPROM_SCK);
   }
   EnableEEPROM(FALSE);       // Setting CS high will set write enable latch

   EnableEEPROM(TRUE);

   // Send Write Instruction
   Data = 2;
   for(Cnt = 8; Cnt > 0; Cnt--)
   {
      output_bit(EEPROM_SI,bit_test(Data,(Cnt - 1)));
      output_high(EEPROM_SCK);
      output_low(EEPROM_SCK);
   }

   // Send Write Address
   for(Cnt = 16; Cnt > 0; Cnt--)
   {
      output_bit(EEPROM_SI,bit_test(Address,(Cnt - 1)));
      output_high(EEPROM_SCK);
      output_low(EEPROM_SCK);
   }

   // Write Data
   for(Cnt = 8; Cnt > 0; Cnt--)
   {
      output_bit(EEPROM_SI,bit_test(DataIn,(Cnt - 1)));
      output_high(EEPROM_SCK);
      output_low(EEPROM_SCK);
   }

   EnableEEPROM(FALSE);
}


BYTE READ_EXT_EEPROM(long int Address)
{
   BYTE Cnt,Data;

   EnableEEPROM(TRUE);

   // Send Read Instruction
   Data = 3;
   for(Cnt = 8; Cnt > 0; Cnt--)
   {
      output_bit(EEPROM_SI,bit_test(Data,(Cnt - 1)));
      output_high(EEPROM_SCK);
      output_low(EEPROM_SCK);
   }

   // Send Read Address
   for(Cnt = 16; Cnt > 0; Cnt--)
   {
      output_bit(EEPROM_SI,bit_test(Address,(Cnt - 1)));
      output_high(EEPROM_SCK);
      output_low(EEPROM_SCK);
   }

   // Read each bit from address
   Data = 0;
   for(Cnt = 8; Cnt > 0; Cnt--)     // shift in bits 7 - 0
   {
      output_high(EEPROM_SCK);
      output_low(EEPROM_SCK);
      if(input(EEPROM_SO))
         bit_set(Data,(Cnt - 1));
   }
   EnableEEPROM(FALSE);
   return(Data);
}

void init_ext_eeprom()
{
   EnableEEPROM(FALSE);
}

void EnableEEPROM(short State)
{
   if(State == TRUE)             // If enable, set CS low
   {                             // after other states are set
      output_low(EEPROM_SCK);
      output_low(EEPROM_SI);
      output_low(EEPROM_SO);
      output_low(EEPROM_CS);
   }
   else                          // If disable, set CS high
   {                             // before other states are set
      output_high(EEPROM_CS);
      output_low(EEPROM_SCK);
      output_low(EEPROM_SI);
      output_low(EEPROM_SO);
   }
}
