///////////////////////////////////////////////////////////////////////////
////                         sst39sf010a.c                             ////
////                                                                   ////
//// Library for two SST39SF010A Flash chips configured for word       ////
//// operation using PIC's External Memory Interface.                  ////
////                                                                   ////
//// init_ext_flash_memory(); Call before the other functions are used ////
////                                                                   ////
//// d = read_ext_flash_memory(a); Read word d from address a.         ////
////                                                                   ////
//// write_ext_flash_memory(a,d); Write word d to address a. If a is   ////
////    the first address of a sector, a sector erase will be          ////
////    performed and all other words in sector will be erased to      ////
////    0xffff.                                                        ////
////                                                                   ////
//// write_block_ext_flash_memory(a,d,c); Write c number of words from ////
////    array d to address starting at a. If the first address of a    ////
////    sector is written to, a sector erase will be performed and all ////
////    unwritten words in that sector will be erased to 0xffff.       ////
////                                                                   ////
//// read_block_ext_flash_memory(a,d,c); Read c number of words        ////
////    starting from address a to array d.                            ////
////                                                                   ////
//// sector_erase_ext_flash_memory(a); Erases sector a to 0xffff. 0    ////
////    erases addresses 0 to FLASH_SECTOR_SIZE-1, 1 erases addresses  ////
////    FLASH_SECTOR_SIZE to 2*FLASH_SECTOR_SIZE-1, 2 erases           ////
////    2*FLASH_SECTOR_SIZE to 3*FLASH_SECTOR_SIZE-1, etc.             ////
////                                                                   ////
//// chip_erase_ext_flash_memory(); Erases all words on external flash ////
////    to 0xffff.                                                     ////
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
#ifndef START_ADDRESS
 #define START_ADDRESS     0x80000 //start address of external memory
#endif

#ifndef FLASH_MEMORY_SIZE
 #define FLASH_MEMORY_SIZE 0x20000   //128K
#endif
#ifndef FLASH_SECTOR_SIZE
 #define FLASH_SECTOR_SIZE 0x1000    //4K
#endif
#define FLASH_SECTORS      FLASH_MEMORY_SIZE / FLASH_SECTOR_SIZE

void enable_write_ext_flash_memory(void);
void sector_erase_ext_flash_memory(unsigned int8 sector);

//This function should be called before writing or reading from the external memory chip
void init_ext_flash_memory()
{
   setup_external_memory(EXTMEM_WORD_WRITE);
}

//This function reads the value(word) from the memory location address on the external memory chip.
unsigned int16 read_ext_flash_memory(unsigned int32 address)
{
   unsigned int16 res;
   read_external_memory(START_ADDRESS+((unsigned int32)2*address),&res,2);
   return res;
}

//This function writes a word value to the memory location address on the external memory chip.
//If writing to first address of sector a sector erase will be performed and all other words in 
//the sector will be erased to 0xffff.
void write_ext_flash_memory(unsigned int32 address,unsigned int16 value)
{
   if((address & (FLASH_SECTOR_SIZE-1)) == 0)
      sector_erase_ext_flash_memory(address/FLASH_SECTOR_SIZE);
   enable_write_ext_flash_memory();
   write_external_memory(START_ADDRESS+((unsigned int32)2*address),&value,2);
   while(read_ext_flash_memory(address)!=value);
}

//This function writes count number of words from the array pointed to by array to memory locations
//starting from address.  If the first address of a sector is written to, a sector erase will be 
//performed and all unwritten words in that sector will be erased to 0xffff.
void write_block_ext_flash_memory(unsigned int32 address,unsigned int16 *array,unsigned int32 count)
{
   unsigned int32 i;
   
   for(i=0;i<count;i++)
   {
      if((address & (FLASH_SECTOR_SIZE-1)) == 0)
         sector_erase_ext_flash_memory(address/FLASH_SECTOR_SIZE);
      enable_write_ext_flash_memory();
      write_external_memory(START_ADDRESS+((unsigned int32)2*address),array,2);
      while(read_ext_flash_memory(address) != *array);
      array++;
      address++;
   }
}

//This function reads count number of words from the memory locations starting from address to the
//array pointed to by array.
void read_block_ext_flash_memory(unsigned int32 address,unsigned int16 *array,unsigned int32 count)
{
   read_external_memory(START_ADDRESS+((int32)2*address),array,count*(int32)2);
}

#define CMD_ADDR_1   0x5555
#define CMD_ADDR_2   0x2AAA
#define CMD_DATA_1   0xAAAA
#define CMD_DATA_2   0x5555
#define CMD_WRITE    0xA0A0
#define CMD_ERASE_1  0x8080
#define CMD_ERASE_2  0x3030
#define CMD_ERASE_3  0x1010

//This function erases all words in the specified sector to 0xFFFF.  0 erases address
//0x00000 to FLASH_SECTOR_SIZE-1, 1 erases address FLASH_SECTOR_SIZE to 
//2*FLASH_SECTOR_SIZE-1, 2 erases address 2*FLASH_SECTOR_SIZE to 3*FLASH_SECTOR_SIZE-1, etc.
void sector_erase_ext_flash_memory(unsigned int8 sector)
{
   unsigned int16 data;
   
   data=CMD_DATA_1;
   write_external_memory(START_ADDRESS+(CMD_ADDR_1*2),&data,2);
   data=CMD_DATA_2;
   write_external_memory(START_ADDRESS+(CMD_ADDR_2*2),&data,2);
   data=CMD_ERASE_1;
   write_external_memory(START_ADDRESS+(CMD_ADDR_1*2),&data,2);
   data=CMD_DATA_1;
   write_external_memory(START_ADDRESS+(CMD_ADDR_1*2),&data,2);
   data=CMD_DATA_2;
   write_external_memory(START_ADDRESS+(CMD_ADDR_2*2),&data,2);
   data=CMD_ERASE_2;
   write_external_memory(START_ADDRESS+(((unsigned int32)sector*0x1000)*2),&data,2);

   delay_ms(25);
}

//This function erases all words on flash to 0xFFFF.
void chip_erase_ext_flash_memory(void)
{
   unsigned int16 data;
   
   data = CMD_DATA_1;
   write_external_memory(START_ADDRESS+(CMD_ADDR_1*2),&data,2);
   data = CMD_DATA_2;
   write_external_memory(START_ADDRESS+(CMD_ADDR_2*2),&data,2);
   data = CMD_ERASE_1;
   write_external_memory(START_ADDRESS+(CMD_ADDR_1*2),&data,2);
   data = CMD_DATA_1;
   write_external_memory(START_ADDRESS+(CMD_ADDR_1*2),&data,2);
   data = CMD_DATA_2;
   write_external_memory(START_ADDRESS+(CMD_ADDR_2*2),&data,2);
   data = CMD_ERASE_3;
   write_external_memory(START_ADDRESS+(CMD_ADDR_1*2),&data,2);
   
   delay_ms(100);
}
   
//This function is used by write_ext_flash_memory() and write_block_ext_flash_memory()
//to enable writes to external flash.  This function is called internally by the
//functions, and should not be called by user.
void enable_write_ext_flash_memory(void)
{
   unsigned int16 data;
   data=CMD_DATA_1;
   write_external_memory(START_ADDRESS+(CMD_ADDR_1*2),&data,2);
   data=CMD_DATA_2;
   write_external_memory(START_ADDRESS+(CMD_ADDR_2*2),&data,2);
   data=CMD_WRITE;
   write_external_memory(START_ADDRESS+(CMD_ADDR_1*2),&data,2);
}

