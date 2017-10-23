/*
Device driver for the external SRAM memory chip M68AF031 with  0x8000 words

// This should be called any other function is accessed
init_ext_sram_memory()

//this function reads the value(word) from the memory location address on the external memory chip
read_ext_sram_memory(long address)

//This function writes a word value to the memory location address on the external memory chip,
//all other words in this sector would be erased to 0xffff
write_ext_sram_memory(long address,long value)

//This function writes count number of words from the array pointed to by array to memory locations
//starting from address
//all other unwritten words in the sectors involved would be erased to 0xffff
write_block_ext_sram_memory(long address,long *array,long count)

//This function writes count number of words from the array pointed to by array to memory locations
//starting from address
//all other unwritten words in the sectors involved would be erased to 0xffff
read_block_ext_sram_memory(long address,long *array,long count)


*/
//#device *=16
#define SRAM_START_ADDRESS 0x20000 //start address of external memory

//this function should be called before writing or reading from the external memory chip
void init_ext_sram_memory()
{
      setup_external_memory(2);
}
//this function reads the value(word) from the memory location address on the external memory chip
long read_ext_sram_memory(long address)
{
     long res;
     read_external_memory(SRAM_START_ADDRESS+((int32)2*address),&res,2);
     return res;
}

//This function writes a word value to the memory location address on the external memory chip,
//all other words in this sector would be erased to 0xffff
void write_ext_sram_memory(long address,long value)
{
     write_external_memory(SRAM_START_ADDRESS+((int32)2*address),&value,2);
     while(read_ext_sram_memory(address)!=value);
}

//This function writes count number of words from the array pointed to by array to memory locations
//starting from address
//all other unwritten words in the sectors involved would be erased to 0xffff
void write_block_ext_sram_memory(long address,long *array,long count)
{
       write_external_memory(SRAM_START_ADDRESS+((int32)2*address),array,count*2);
       while(read_ext_sram_memory(address+count-1)!=*(array+(count-1)));


}

//This function writes count number of words from the array pointed to by array to memory locations
//starting from address
//all other unwritten words in the sectors involved would be erased to 0xffff
void read_block_ext_sram_memory(long address,long *array,long count)
{
    read_external_memory(SRAM_START_ADDRESS+((int32)2*address),array,count*(int32)2);

}

