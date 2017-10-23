///////////////////////////////////////////////////////////////////////////
////                           cy62256.c                               ////
////                                                                   ////
//// Library for two CY62256 SRAM chips configured for word operation  ////
//// using the PIC's External Memory Interface.                        ////
////                                                                   ////
//// init_ext_sram_memory(); Call before the other functions are used  ////
////                                                                   ////
//// d = read_ext_sram_memory(a); Read word d from address a           ////
////                                                                   ////
//// write_ext_sram_memory(a,d); Write word d to address a             ////
////                                                                   ////
//// write_block_ext_sram_memory(a,d,c); Write c number of words from  ////
////    array d to address starting at a.                              ////
////                                                                   ////
//// read_block_ext_sram_memory(a,d,c); Read c number of words         ////
////    starting from address a to array d.                            ////
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

//#device *=16
#define SRAM_START_ADDRESS 0x40000 //start address of external memory

//This function should be called before writing or reading from the external memory chip
void init_ext_sram_memory()
{
      setup_external_memory(2);
}
//This function reads the value(word) from the memory location address on the external memory chip
long read_ext_sram_memory(long address)
{
     long res;
     read_external_memory(SRAM_START_ADDRESS+((int32)2*address),&res,2);
     return res;
}

//This function writes a word value to the memory location address on the external memory chip
void write_ext_sram_memory(long address,long value)
{
     write_external_memory(SRAM_START_ADDRESS+((int32)2*address),&value,2);
     while(read_ext_sram_memory(address)!=value);
}

//This function writes count number of words from the array pointed to by array to memory locations
//starting from address
void write_block_ext_sram_memory(long address,long *array,long count)
{
       write_external_memory(SRAM_START_ADDRESS+((int32)2*address),array,count*2);
       while(read_ext_sram_memory(address+count-1)!=*(array+(count-1)));


}

//This function reads count number of words starting from address to the array pointed to by array
void read_block_ext_sram_memory(long address,long *array,long count)
{
    read_external_memory(SRAM_START_ADDRESS+((int32)2*address),array,count*(int32)2);

}

