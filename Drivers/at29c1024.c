///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////


/*
PRELIMINARY Device driver for the external memory chip AT29C1024 with 0x10000 words
            for use on the CCS PIC18F8720 Mini-Proto board

// This should be called any other function is accessed
init_ext_flash_memory()

//this function reads the value(word) from the memory location address on the external memory chip
read_ext_flash_memory(long address)

//This function writes a word value to the memory location address on the external memory chip,
//all other words in this sector would be erased to 0xffff
write_ext_flash_memory(long address,long value)

//This function writes count number of words from the array pointed to by array to memory locations
//starting from address
//all other unwritten words in the sectors involved would be erased to 0xffff
write_block_ext_flash_memory(long address,long *array,long count)

//This function writes count number of words from the array pointed to by array to memory locations
//starting from address
//all other unwritten words in the sectors involved would be erased to 0xffff
read_block_ext_flash_memory(long address,long *array,long count)


*/
//#device *=16
#define START_ADDRESS 0x40000 //start address of external memory

//this function should be called before writing or reading from the external memory chip
void init_ext_flash_memory()
{
      setup_external_memory(2);
}
//this function reads the value(word) from the memory location address on the external memory chip
long read_ext_flash_memory(long address)
{
     long res;
     read_external_memory(START_ADDRESS+((int32)2*address),&res,2);
     return res;
}

//This function writes a word value to the memory location address on the external memory chip,
//all other words in this sector would be erased to 0xffff
void write_ext_flash_memory(long address,long value)
{
     write_external_memory(START_ADDRESS+((int32)2*address),&value,2);
     while(read_ext_flash_memory(address)!=value);
}

//This function writes count number of words from the array pointed to by array to memory locations
//starting from address
//all other unwritten words in the sectors involved would be erased to 0xffff
void write_block_ext_flash_memory(long address,long *array,long count)
{
   long i;
   long srtsector;
   long endsector;
   long arrptr;
   long endaddr;
   long temp;
   endaddr=address+count-1;
   srtsector=address/128;// starting sector number
   endsector=endaddr/128;// ending sector number
   if (srtsector==endsector) //whle block in same sector
   {
       write_external_memory(START_ADDRESS+((int32)2*address),array,count*2);
       while(read_ext_flash_memory(address+count-1)!=*(array+(count-1)));

   }
   else
   {
      //write the locations in the start sector
      arrptr=(srtsector+1)*128-address;//next unused value in the passed array
      write_external_memory(START_ADDRESS+((int32)2*address),array,((arrptr)*2));
      while(read_ext_flash_memory((srtsector+1)*128-1)!=*(array+(arrptr-1)));
      //write the sectors in between
      for (i=srtsector+1;i<endsector;i++)
      {
        temp=(i*128);
        write_external_memory(START_ADDRESS+((int32)2*temp),array+arrptr,256);
        while(read_ext_flash_memory(((i+1)*128)-1)!=*(array+(arrptr+127)));
        arrptr+=128;

      }


      //write the end sector
      temp=( endsector*128);
      write_external_memory(START_ADDRESS+((int32)2*temp),(array+arrptr),((endaddr%128)+1)*2);
      while(read_ext_flash_memory(endaddr)!=*(array+(arrptr+(endaddr%128))));

   }

}

//This function writes count number of words from the array pointed to by array to memory locations
//starting from address
//all other unwritten words in the sectors involved would be erased to 0xffff
void read_block_ext_flash_memory(long address,long *array,long count)
{
    read_external_memory(START_ADDRESS+((int32)2*address),array,count*(int32)2);

}

