///////////////////////////////////////////////////////////////////////////
////              Library for a DS1933 SRAM                            ////
////                                                                   ////
////   data = read_scratchpad(addr, ES)                                ////
////     Used to verify scratchpad data and target address             ////
////                                                                   ////
////   write_scratchpad(addr, data)                                    ////
////     Used to write data to scratchpad                              ////
////                                                                   ////
////   copy_scratchpad(addr, ES)                                       ////
////     Used to copy data from scratchpad to memory                   ////
////                                                                   ////
////   read_entire_memory(mem)                                         ////
////     used to read the entire 512 bytes of memory                   ////
////                                                                   ////
////   data = read_memory(addr)                                        ////
////     Used to read a place in memory                                ////
////                                                                   ////
////   block_fill(from, to, filler)                                    ////
////     Used to block fill a specified amount of memory               ////
////                                                                   ////
////   match_block_fill(from, to, filler)                              ////
////     Used to block fill a specified amount of memory               ////
////     from a specific ROM                                           ////
////                                                                   ////
////   Following functions are used to access the 64 bit lasered ROM   ////
////                                                                   ////
////   read_rom_specs(family, ID, CRC)                                 ////
////     Seperates different data from the rom                         ////
////                                                                   ////
////   read_rom(data)                                                  ////
////     Reads in the entire rom                                       ////
////                                                                   ////
////   search_rom()                                                    ////
////     Identify the connected device                                 ////
////                                                                   ////
////   match_rom(romdata)                                              ////
////     Device with correct 64-bit sequence will respond              ////
////                                                                   ////
////   skip_rom()                                                      ////
////     To save time on single drop bus system                        ////
////                                                                   ////
////   result = check_crc8(data)                                       ////
////     checks CRC of 64 bit sequence                                 ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2010 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#ifndef  DS1993_C
#define  DS1993_C

#ifndef TOUCH_PIN
   #define TOUCH_PIN  PIN_B0
#endif

#include "touch.c"


/////////////////////////////
////                     ////
//// Function Prototypes ////
////                     ////
/////////////////////////////

/*
BYTE read_scratchpad(int16 addr, BYTE* ES)
This will read scratchpad data from a specified address
PARAM addr: The address to read from
NOTE: In order to copy data from the scratchpad, the ES byte must be known
RETURNS: The data in the address of scratchpad
*/
BYTE read_scratchpad(int16 *addr, BYTE* ES);

/*
void write_scratchpad(int16 addr, BYTE data)
This will write a byte of data to the scratchpad at a specified location
PARAM addr: The address to write to
PARAM data: The byte to write to the specified location
RETURNS: none
*/
void write_scratchpad(int16 addr, BYTE data);

/*
void copy_scratchpad(int16 addr, BYTE ES)
This will copy the contents of the scratchpad to the specified location
PARAM addr: The address to copy to
PARAM ES: The E/S byte to use
NOTE: All parameters passed in must match the DS1993's 3 address registers
*/
void copy_scratchpad(int16 addr, BYTE ES);

/*
void read_entire_memory(BYTE mem[])
This will read all 512 bytes of a DS1993 into a 512 byte array on the PIC
PARAM: A pointer a variable to put the data into
NOTE: mem must be pointing to the beginning of a 512 wide array
RETURNS: none
*/
void read_entire_memory(BYTE mem[]);

/*
BYTE read_memory(int16 addr)
This will read one byte from memory at a specified address
PARAM: The address to read from
RETURNS: The data at the address
*/
BYTE read_memory(int16 addr);

/*
void block_fill(int16 from, int16 to, BYTE filler)
This will block fill a chunk of memory
PARAM: Where to start the block fill
PARAM: Where to stop the block fill, including the address
PARAM: The byte to fill the block with
RETURNS: none
*/
void block_fill(int16 from, int16 to, BYTE filler);

/*
void match_block_fill(BYTE rom[], int16 from, int16 to, BYTE filler)
This will block fill a chunk of memory at a specific DS1993
PARAM: A pointer to the beginning of the rom data to test
PARAM: Where to start the block fill
PARAM: Where to stop the block fill, including the address
PARAM: The byte to fill the block with
NOTE: rom must be pointing to the beginning of a 8-wide array
RETURNS: none
*/
void match_block_fill(BYTE address[], int16 from, int16 to, BYTE filler);

/*
void read_rom_specs(BYTE* family, BYTE ID[], BYTE* CRC)
This will take the 64-bit lasered ROM and put it into seperate variables.
  The DS1993 will then be ready to excecute a memory function.
PARAMS: Pointers to the variables to put the various data into
NOTE: ID must be pointing to the beginning a 6 wide array
RETURNS: none
*/
void read_rom_specs(BYTE* family, BYTE ID[], BYTE* CRC);

/*
void read_rom(BYTE data[])
This will take the 64-bit lasered ROM and put it into a variable. The DS1993
  will then be ready to excecute a memory function.
PARAM: A pointer to the variable to put the rom data into
NOTE: data must be pointing to the beginning of a 8-wide array.
NOTE: the LSB in the ROM (family) will start at index 0,
  the MSB in the rom (CRC) will be at index 7.
RETURNS: none
*/
void read_rom(BYTE data[]);

/*
void match_rom(BYTE romdata[])
This will send the match ROM command to the DS1993.
  If the rom data sent into the function equals the rom data on the
  DS1993, then the DS1993 will be ready for a memory function, otherwise
  the DS1993 will wait for a reset pulse to begin a rom function.
PARAM: A pointer to the beginning of the rom data to test
NOTE: romdata must be pointing to the beginning of a 8-wide array
RETURNS: none
*/
void match_rom(BYTE romdata[]);

/*
void search_rom()
This will print to the console all of the contents of all of the
  ROMs that are connected to the one-wire bus
PARAM: none
RETURNS: none
*/
void search_rom();

/*
void skip_rom()
This will save time on single drop bus system by bypassing any rom function
  and heading straight to a memory function
PARAMS: none
RETURNS: none
*/
void skip_rom();

//////////////////////////////////
////                          ////
//// Function Implementations ////
////                          ////
//////////////////////////////////

/*
BYTE read_scratchpad(int16 addr, BYTE* ES)
This will read scratchpad data from a specified address
PARAM addr: The address to read from
NOTE: In order to copy data from the scratchpad, the ES byte must be known
RETURNS: The data in the address of scratchpad
*/
BYTE read_scratchpad(int16 addr, BYTE* ES)
{
   touch_write_byte(0xAA);    // read scratchpad command
   touch_read_byte();         // read TA1
   touch_read_byte();         // read TA2
   *ES = touch_read_byte();   // read E/S
   return touch_read_byte();  // read the result
}

/*
void write_scratchpad(int16 addr, BYTE data)
This will write a byte of data to the scratchpad at a specified location
PARAM addr: The address to write to
PARAM data: The byte to write to the specified location
RETURNS: none
*/
void write_scratchpad(int16 addr, BYTE data)
{
   touch_write_byte(0x0F);       // write scratchpad command
   touch_write_byte(addr);       // write TA1
   touch_write_byte(addr >> 8);  // write TA2
   touch_write_byte(data);       // write data
   
}

/*
void copy_scratchpad(int16 addr, BYTE ES)
This will copy the contents of the scratchpad to the specified location
PARAM addr: The address to copy to
PARAM ES: The E/S byte to use
NOTE: All parameters passed in must match the DS1993's 3 address registers
*/
void copy_scratchpad(int16 addr, BYTE ES)
{
   touch_write_byte(0x55);       // copy scratchpad command
   touch_write_byte(addr);       // write TA1
   touch_write_byte(addr >> 8);  // write TA2
   touch_write_byte(ES);         // write E/S
}

/*
void read_entire_memory(BYTE mem[])
This will read all 512 bytes of a DS1993 into a 512 byte array on the PIC
PARAM: A pointer a variable to put the data into
NOTE: mem must be pointing to the beginning of a 512 wide array
RETURNS: none
*/
void read_entire_memory(BYTE mem[])
{
   int16 i;

   touch_write_byte(0xF0);       // read memory command
   touch_write_byte(0x00);       // start at 0x00...
   touch_write_byte(0x00);       // ... on page 0
   for(i = 0; i < 512; i++)
      mem[i] = touch_read_byte();
}

/*
BYTE read_memory(int16 addr)
This will read one byte from memory at a specified address
PARAM: The address to read from
RETURNS: The data at the address
*/
BYTE read_memory(int16 addr)
{
   touch_write_byte(0xF0);       // read memory command
   touch_write_byte(addr);       // write TA1
   touch_write_byte(addr >> 8);  // write TA2
   return touch_read_byte();     // read in result
}

/*
void block_fill(int16 from, int16 to, BYTE filler)
This will block fill a chunk of memory
PARAM: Where to start the block fill
PARAM: Where to stop the block fill, including the address
PARAM: The byte to fill the block with
RETURNS: none
*/
void block_fill(int16 from, int16 to, BYTE filler)
{
   int16 i;
   BYTE ES, scratchpad_data;

   if(to < from)
   {
      printf("\r\nError: End of block before beginning of block.\r\nStopping block fill...");
      return;
   }

   for(i = from; i <= to; i++)
   {
      // write some data to the scratchpad
      reset_pulse();
      skip_rom();
      write_scratchpad(i, filler);

      // read back that data
      reset_pulse();
      skip_rom();
      scratchpad_data = read_scratchpad(i, &ES);

      // copy that data
      reset_pulse();
      skip_rom();
      copy_scratchpad(i, ES);
   }
}

/*
void match_block_fill(BYTE rom[], int16 from, int16 to, BYTE filler)
This will block fill a chunk of memory at a specific DS1993
PARAM: A pointer to the beginning of the rom data to test
PARAM: Where to start the block fill
PARAM: Where to stop the block fill, including the address
PARAM: The byte to fill the block with
NOTE: rom must be pointing to the beginning of a 8-wide array
RETURNS: none
*/
void match_block_fill(BYTE address[], int16 from, int16 to, BYTE filler)
{
   int16 i;
   BYTE ES, scratchpad_data;

   if(to < from)
   {
      printf("\r\nError: End of block before beginning of block.\r\nStopping block fill...");
      return;
   }

   for(i = from; i <= to; i++)
   {
      // write some data to the scratchpad
      reset_pulse();
      match_rom(address);
      write_scratchpad(i, filler);

      // read back that data
      reset_pulse();
      match_rom(address);
      scratchpad_data = read_scratchpad(i, &ES);

      // copy that data
      reset_pulse();
      match_rom(address);
      copy_scratchpad(i, ES);
   }
}

/*
void read_rom_specs(BYTE* family, BYTE ID[], BYTE* CRC)
This will take the 64-bit lasered ROM and put it into seperate variables.
  The DS1993 will then be ready to excecute a memory function.
PARAMS: Pointers to the variables to put the various data into
NOTE: ID must be pointing to the beginning a 6 wide array
RETURNS: none
*/
void read_rom_specs(BYTE* family, BYTE ID[], BYTE* CRC)
{
   BYTE i;

   // give the command to read back the requested data
   touch_write_byte(0x33);

   // family is the first item in the array
   *family = touch_read_byte();

   // the ID is the next 6 bytes
   for(i = 0; i < 6; i++)
      ID[i] = touch_read_byte();

   // CRc is the last byte
   *CRC = touch_read_byte();
}

/*
void read_rom(BYTE data[])
This will take the 64-bit lasered ROM and put it into a variable. The DS1993
  will then be ready to excecute a memory function.
PARAM: A pointer to the variable to put the rom data into
NOTE: data must be pointing to the beginning of a 8-wide array.
NOTE: the LSB in the ROM (family) will start at index 0,
  the MSB in the rom (CRC) will be at index 7.
RETURNS: none
*/
void read_rom(BYTE data[])
{
   BYTE i;

   // give the command to read back the requested data
   touch_write_byte(0x33);

   // fill up data
   for(i = 0; i < 8; i++)
      data[i] = touch_read_byte();
}

/*
void match_rom(BYTE romdata[])
This will send the match ROM command to the DS1993.
  If the rom data sent into the function equals the rom data on the
  DS1993, then the DS1993 will be ready for a memory function, otherwise
  the DS1993 will wait for a reset pulse to begin a rom function.
PARAM: A pointer to the beginning of the rom data to test
NOTE: romdata must be pointing to the beginning of a 8-wide array
RETURNS: none
*/
void match_rom(BYTE romdata[])
{
   BYTE romdatacopy[8], i;

   // make a copy of romdata, we don't necessarily want to modify the original data
   for(i = 0; i < 8; i++)
      romdatacopy[i] = romdata[i];

   // give the command to match ROM
   touch_write_byte(0x55);

   // start trying to match
   for(i = 0; i < 64; i++)
      touch_write_bit(shift_right(romdatacopy, 8, 0));
}

/*
void search_rom()
This will print to the console all of the contents of all of the
ROMs that are connected to the one-wire bus
PARAM: none
RETURNS: none
*/
void search_rom()
{
   /*
   Please read p51-p54 in the "standard" data sheet for the DS1993 for a basic algorithm explanation.
   At the time of this writing, this data sheet could be found at
   www.maxim-ic.com/products/ibutton/ibuttons/standard.pdf
   */
   BYTE
      i,                               // used for loops
      last_desc_pos = 0,               // the position of the last discrepancy in the current iteration
      desc_marker = 0,                 // the position of the discrepancy of last iteration
      contents[8] = {0,0,0,0,0,0,0,0}, // the contents of the ROM in the current iteration
      firstromcont[8];                 // the contents of the first ROM read in

   int1
      bitA,                            // the bit read in from the current index of all of the rom(s)
      bitB,                            // the complement of the bit read in from the current index of all of the rom(s)
      last_desc = 0;                   // the bit to shift into contents[]

   BOOLEAN
      firstrom = TRUE,
      done = FALSE;

   // printf nice header
   printf("\r\n---ROMs found---\r\n");

   do
   {
      // set the last discrepancy position to the discrepancy marker from the last iteration
      last_desc_pos = desc_marker;

      // search rom command
      reset_pulse();
      touch_write_byte(0xF0);

      // read in one entire rom
      for(i = 0; i < 64; i++)
      {
         bitA = touch_read_bit(); // read one bit from rom(s)
         bitB = touch_read_bit(); // read complement of previous bit from rom(s)

         if((!bitA) && (!bitB))
         {
            // at this point we know that we don't know a bit
            // set the discrepancy marker to the current index
            desc_marker = i;

            // check to see if the current index is where the last discrepancy was
            if(i == last_desc_pos)
               // if so, flip the bit that gets shifted in
               last_desc = !last_desc;

            // shift in a bit
            shift_right(contents, 8, last_desc);
            touch_write_bit(last_desc);
         }

         else
         {
            // at this point we know that we know a bit
            // shift in bitA because that is the known bit
            shift_right(contents, 8, bitA);
            touch_write_bit(bitA);
         }
      }

      // because of this weird algorithm, we need to keep track of the first rom;
      //   eventually the algorithm will loop back to the first rom and we'll know we're done
      if(firstrom)
      {
         for(i = 0; i < 8; i++)
            firstromcont[i] = contents[i];
         firstrom = FALSE;
      }

      else
      {
         // print off what we found
         for(i = 0; i < 8; i++)
            printf("%X", contents[7 - i]);
         printf("\r\n");

         done = TRUE;
         for(i = 0; i < 8; i++)
            if(firstromcont[i] != contents[i])
               done = FALSE;
      }
   } while(!done);
}

/*
void skip_rom()
This will save time on single drop bus system by bypassing any rom function
  and heading straight to a memory function
PARAMS: none
RETURNS: none
*/
void skip_rom()
{
   // give the command to skip ROM
   touch_write_byte(0xCC);
}

//constant data table used for checking CRC of 64 bit unique ROM codes
const char crc8_table[256]={
     0,  94, 188, 226,  97,  63, 221, 131, 194, 156, 126,  32, 163, 253,  31,  65,
   157, 195,  33, 127, 252, 162,  64,  30,  95,   1, 227, 189,  62,  96, 130, 220,
    35, 125, 159, 193,  66,  28, 254, 160, 225, 191,  93,   3, 128, 222,  60,  98,
   190, 224,   2,  92, 223, 129,  99,  61, 124,  34, 192, 158,  29,  67, 161, 255,
    70,  24, 250, 164,  39, 121, 155, 197, 132, 218,  56, 102, 229, 187,  89,   7,
   219, 133, 103,  57, 186, 228,   6,  88,  25,  71, 165, 251, 120,  38, 196, 154,
   101,  59, 217, 135,   4,  90, 184, 230, 167, 249,  27,  69, 198, 152, 122,  36,
   248, 166,  68,  26, 153, 199,  37, 123,  58, 100, 134, 216,  91,   5, 231, 185,
   140, 210,  48, 110, 237, 179,  81,  15,  78,  16, 242, 172,  47, 113, 147, 205,
    17,  79, 173, 243, 112,  46, 204, 146, 211, 141, 111,  49, 178, 236,  14,  80,
   175, 241,  19,  77, 206, 144, 114,  44, 109,  51, 209, 143,  12,  82, 176, 238,
    50, 108, 142, 208,  83,  13, 239, 177, 240, 174,  76,  18, 145, 207,  45, 115,
   202, 148, 118,  40, 171, 245,  23,  73,   8,  86, 180, 234, 105,  55, 213, 139,
    87,   9, 235, 181,  54, 104, 138, 212, 149, 203,  41, 119, 244, 170,  72,  22,
   233, 183,  85,  11, 136, 214,  52, 106,  43, 117, 151, 201,  74,  20, 246, 168,
   116,  42, 200, 150,  21,  75, 169, 247, 182, 232,  10,  84, 215, 137, 107,  53};

/*
int1 check_crc8(int8 *ptr)
This checks to see if the unique 64 bit ROM code was received correctly by 
checking it's CRC.
PARAMS: pointer to 64 bit ROM code.
RETURNS: TRUE if CRC matches, FALSE if CRC doesn't match
*/
int1 check_crc8(int8 *ptr)
{
   int8 i;
   int8 result=0;
   
   for(i=0;i<8;i++)
   {
      result=crc8_table[result^ptr[i]];
   }
   
   if(result)
      return(FALSE);
   else
      return(TRUE);
}

#endif
