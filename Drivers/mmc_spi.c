////////////////////// Driver for Multimedia Card ///////////////////////
////                                                                 ////
////  mmc_init() - Reset and Initialize the MMC.  Returns zero if OK ////
////                                                                 ////
////  mmc_modify_byte(address, val) - Modify the byte at address to  ////
////               change it's value to val.  Will read/write the    ////
////               entire 512 byte block but only change this        ////
////               specific byte.  Returns zero if OK.               ////
////                                                                 ////
////  mmc_modify_block(address, size, *ptr) - Modifies the bytes     ////
////               to change their value to whats stored at *ptr.    ////
////               Will read/write the entire 512 byte block(s) but  ////
////               only change the values defined by ptr and size.   ////
////               Returns zero if OK.                               ////
////                                                                 ////
////  mmc_write_block(address, size, *ptr) - Writes a 512 byte       ////
////               block to the MMC.  If size is less than 512 then  ////
////               unspecified data will be written as 0.  Returns   ////
////               zero if OK.                                       ////
////                                                                 ////
////  mmc_read_byte(address,*ptr) - Reads the byte specified at      ////
////               address.  Result is saved to ptr.  Returns zero   ////
////               if OK.                                            ////
////                                                                 ////
////  mmc_read_block(address, size, *ptr) - Reads the bytes          ////
////               specified at address.  Result is saved to ptr.    ////
////               Returns zero if OK.                               ////
////               NOTE: You might get an address error if you try   ////
////               to read over a page size.  For example, trying    ////
////               to read a block size of 512 starting at address   ////
////               0x100 may cause an error because you are reading  ////
////               two blocks.                                       ////
////                                                                 ////
////  mmc_erase(address, blocks) - Erases the block specified at     ////
////               address.  Will erase the entire 512 byte block.   ////
////               If you wish to erase more blocks after specified  ////
////               block use the blocks parameter to specifiy how    ////
////               many extra blocks to erase.  Returns zero if OK.  ////
////                                                                 ////
////              ~~~~~~~ MULTI-READ FUNCTIONS ~~~~~~~~              ////
////                                                                 ////
////  mmc_read_enable(address, size) - Start multi-reads at          ////
////               specified address.  Size is the size of each      ////
////               individual read.  Returns zero if OK.             ////
////                                                                 ////
////  mmc_read_mult_block(*ptr) - Reads data from the MMC, and saves ////
////               to ptr.  The number of bytes read is defined      ////
////               by mmc_read_enable().  You must call              ////
////               mmc_read_enable() before you can call this.       ////
////               Returns zero if OK.                               ////
////                                                                 ////
////  mmc_read_disable(void) - Stop a multi-read.                    ////
////               Returns zero if OK.                               ////
////               MAY BE BROKEN.                                    ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2001 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

/////*** USER CONFIG ***/////

//SanDisk’s MultiMediaCards clock data in on the rising edge and out on the falling edge.
#ifndef MMC_CLK
#define MMC_CLK   PIN_B1
#endif
#ifndef MMC_DI
#define MMC_DI    PIN_B0
#endif
#ifndef MMC_DO
#define MMC_DO    PIN_B3
#endif
#ifndef MMC_CS
#define MMC_CS    PIN_B2
#endif

enum MMC_EC {
   MMC_EC_OK = 0,
   MMC_EC_NOT_IDLE = 1, //after reset command, device wasn't in idle state
   MMC_EC_NOT_INIT = 2, //after init command, device had an error
   MMC_EC_NO_CRC = 3, //CRC wouldn't turn off
   MMC_EC_SET_READ_BLOCKSIZE_FAIL = 4, //gave us an error during set blocksize
   MMC_EC_SET_WRITE_BLOCKSIZE_FAIL = 5, //gave us an error during set blocksize
   MMC_EC_SET_MULTI_BLOCKSIZE_FAIL = 6, //gave us an error during set blocksize
   MMC_EC_SET_WRITE_ADDRESS_FAIL = 7, //gave us an error when we told it we want to write block
   MMC_EC_BLOCK_NOT_WRITTEN = 8, //after sending block, it gave us this response
   MMC_EC_SET_READ_ADDRESS_FAIL = 9, //gave us an error when we told it we wanted to read block
   MMC_EC_ERASE_SECTOR_START_FAIL = 10, //mmc gave us an error when we set the start sector for erase
   MMC_EC_ERASE_SECTOR_END_FAIL = 11, //mmc gave us an error when we set the end sector for erase
   MMC_EC_ERASE_SECTORS_FAIL = 12, //mmc gave us an error when we told it to erase tagged sectors
   MMC_EC_BAD_STATE = 13,  //we cant start/continue a multi transfer because we didn't start/finish the last one
   MMC_EC_START_MULTI_READ_FAIL = 14, //mmc gave us an error when we started a multi-read
   MMC_EC_READ_NEXT_BLOCK_FAIL = 15, //mmc gave us an error when started to read the next block (in multi-read)
   MMC_EC_STOP_TRANSMISSION_FAIL = 16, //mmc gave us an error when tried to stup a multi read/write transmission
   MMC_EC_WRITE_ALWAYS_BUSY = 17 //after a block write, we waited longer than timeout period for busy signal to end
};


//#define MMC_DEBUG PRINTF      //uncomment this line to print out debug messages
#define MMC_DEBUG(a,b,c,d,e,f,g,h,i,j,k)   //uncomment this line to not print out debug messages

/////*** END USER CONFIG ***/////

//if you want to slow down the speed of the MMC clock, define delay_clk to a delay_xx() routine
//#define mmc_delay_clk() delay_us(1)
#define mmc_delay_clk()

#define MMC_BLOCK_SIZE  512

int16 mmc_last_result=0;

// FUNCTIONS
//public functions
MMC_EC   mmc_init();

MMC_EC   mmc_modify_byte(int32 address, int8 val);
MMC_EC   mmc_modify_block(int32 address, int16 block_size, int *ptr);
MMC_EC   mmc_write_block(int32 address, int16 block_size, int *ptr);

#define  mmc_read_byte(a,p) mmc_read_block(a,1,p)
MMC_EC   mmc_read_block(int32 address,int16 block_size, int *ptr);

MMC_EC   mmc_erase(int32 address, int32 sectors);


MMC_EC   mmc_read_enable(int32 address, int16 block_size);
MMC_EC   mmc_read_mult_block(int *ptr);
MMC_EC   mmc_read_disable(void);

/* NOT TESTED ROUTINES
short mmc_write_enable(int32 address, int16 block_size);
short mmc_write_mult_block(int *ptr);
void  mmc_write_disable();

void  mmc_read_csd(int *ptr);
void  mmc_write_csd(int *ptr);
*/

//private functions
int16 mmc_send_cmd(int cmd, int32 arg, int mode);
void  mmc_send_data(int d_byte);
int   mmc_read_data();
void  mmc_select(void);
void  mmc_deselect(void);
int1  mmc_wait_for_not_busy(int16 to); //to in 8 clocks
int1  mmc_receive_data(int8 *ptr, int16 size);

// GLOBALS

enum {MMC_STARTUP_STATE=0, MMC_STANDBY_STATE=1, MMC_WRITE_STATE=2,
      MMC_READ_STATE=3, MMC_ERASE_STATE=4} mmc_state=MMC_STARTUP_STATE;

//holds the current block size of our multi-read or multi-write routines
int16   mmc_blk_size;

#define MMC_CMD_GO_IDLE_STATE          0
#define MMC_CMD_SEND_OP_COND           1
#define MMC_CMD_STOP_TRANSMISSION      12
#define MMC_CMD_SET_BLOCKLEN           16
#define MMC_CMD_READ_SINGLE_BLOCK      17
#define MMC_CMD_READ_MULTITPLE_BLOCK   18
#define MMC_CMD_WRITE_BLOCK            24
#define MMC_CMD_TAG_SECTOR_START       32
#define MMC_CMD_TAG_SECTOR_END         33
#define MMC_CMD_ERASE                  38
#define MMC_CMD_CRC_ON_OFF             59

//***********************************************************************//
//************************* Public Functions ****************************//
//***********************************************************************//


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// mmc_init()     Initializes the unit.
//
// Paramaters:    <none>
//
// Returns:       0 if initiated correctly
//                Non-zero if initiation failed
//
// Special Info:  This should be called immediately to initialize
//                the multi media card.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MMC_EC mmc_init(void)   {
   int16 result;
   int8 to=50;

   mmc_state = MMC_STARTUP_STATE;

   output_high(MMC_CS);
   output_high(MMC_DO);             // set pin levels

   output_high(MMC_CLK);
//   output_low(MMC_CLK);

   output_float(MMC_DI);

   delay_ms(15);

   do {
      mmc_select();
      result=mmc_send_cmd(MMC_CMD_GO_IDLE_STATE,0,1);
      mmc_deselect();
      if (result==1)
         break;
      else {
         delay_ms(10);
         to--;
      }
   } while (to);

   if (result!=1) {
      MMC_DEBUG(" ERR: after reset, device isn't idle");
      return(MMC_EC_NOT_IDLE);
   }


   to=50;

   do {
      mmc_select();
      result = mmc_send_cmd(MMC_CMD_SEND_OP_COND,0,1);                // puts card into ready state
      mmc_deselect();
      if (result && (result!=1)) {
         MMC_DEBUG(" ERR: after init we got a funky error %LX", result);
         return(MMC_EC_NOT_INIT);
      }
      else if (result==1) {
         to--;
         delay_ms(1);
      }
      else
         break;
   } while (to);     // loop until not busy

   MMC_DEBUG(" init cmd returns: %X\n\r",result);


   mmc_select();
   result=mmc_send_cmd(MMC_CMD_CRC_ON_OFF,0,1);
   mmc_deselect();
   if (result) {
      MMC_DEBUG(" ERR: CRC didn't turn off");
      return(MMC_EC_NO_CRC);
   }

   mmc_state = MMC_STANDBY_STATE;        // set current state
   return(MMC_EC_OK);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// mmc_modify_block()
//                Writes a block to the MMC.  This routine will read the
//                entire 512 byte block first, and then make modifications
//                to that in RAM and save it back to MMC.  Therefore, this
//                is the function to use if you want to write data that is
//                less than the size of the block (512 bytes)
//
// Paramaters:    address:  address to write to
//                ptr:      data byte to write
//                size:     number of bytes to write
//
// Returns:       TRUE  if write worked correctly
//                FALSE if failed
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MMC_EC mmc_modify_block(int32 address, int16 size, int *ptr)   {
   int8 mmc_buffer[MMC_BLOCK_SIZE];
   int val;
   int16 i;
   int32 block_start;
   int16 num;
   MMC_EC res;

   val = 0;

   block_start=address & 0xFFFFFE00;
   i=address-block_start;

   while(size) {
      res=mmc_read_block(block_start, MMC_BLOCK_SIZE, mmc_buffer);
      if (res) {
         MMC_DEBUG(" cannot read 512 byte block.  nothing written! \n\r");
         return(res);
      }
      MMC_DEBUG(" read 512\r\n");

      if ((size+i)>MMC_BLOCK_SIZE) {num=MMC_BLOCK_SIZE-i;} else {num=size;}

      memcpy(&mmc_buffer[i],ptr,num);

      res=mmc_write_block(block_start, MMC_BLOCK_SIZE, mmc_buffer);
      if (res) {
         MMC_DEBUG(" cannot write MMC_BLOCK_SIZE byte block\r\n");
         return(res);
      }
      MMC_DEBUG(" 512 written\r\n");

      ptr+=num;
      block_start+=MMC_BLOCK_SIZE;
      size-=num;
      i=0;
   }
   return(MMC_EC_OK);
}

MMC_EC mmc_modify_byte(int32 address, int8 val) {
   return(mmc_modify_block(address,1,&val));
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// mmc_write_block()
//                Writes a block to the MMC.  The address must start at the
//                beginning of a block (every 512 bytes).
//
// Paramaters:    address:  address to write to
//                ptr:      data byte to write
//                size:     number of bytes to write from RAM
//
// Returns:       TRUE  if write worked correctly
//                FALSE if failed
//
// Notes:         If address does not start at a block boundry, the MMC will
//                give you an error.
//                If you have a size less than 512 the rest of the data will
//                be saved as 0.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MMC_EC mmc_write_block(int32 address, int16 size, int *ptr)   {
   int16 j;
   int8 val;
   int8 res;

   mmc_select();
   res=mmc_send_cmd(MMC_CMD_SET_BLOCKLEN, MMC_BLOCK_SIZE, 1);
   mmc_deselect();
   if (res)
   {
      MMC_DEBUG(" set block size failed\n\r");
      return(MMC_EC_SET_WRITE_BLOCKSIZE_FAIL);
   }

   mmc_select();
   res=mmc_send_cmd(MMC_CMD_WRITE_BLOCK,address,1);   // command to write byte at address
   if (res)
   {
      MMC_DEBUG(" write byte failed.  Nothing written!\n\r");
      mmc_deselect();
      return(MMC_EC_SET_WRITE_ADDRESS_FAIL);
   }

   mmc_send_data(0xFF);                    // send nothing for 8 bits
   mmc_send_data(0xFE);                    // send start bit

   for(j=0; j<MMC_BLOCK_SIZE; j++)
   {
      if (size) {
         val=*ptr;
         ptr++;
         size--;
      }
      else {
         val=0;
      }
      mmc_send_data(val);                    // writes the byte
   }
   mmc_send_data(0xFF);  //crc
   mmc_send_data(0xFF);  //crc

   val = mmc_read_data() & 0x1F;

   MMC_DEBUG(" write data returned:  %X\n\r",val);

   if (!mmc_wait_for_not_busy(1000)) {
      MMC_DEBUG(" ERR: mmc spi is busy\r\n");
      return(MMC_EC_WRITE_ALWAYS_BUSY);
   }

   mmc_deselect();

   if(val!=5)         // if not stored correctly
   {
      MMC_DEBUG(" data not stored\n\r");
      return(MMC_EC_BLOCK_NOT_WRITTEN);
   }

   MMC_DEBUG(" data was stored\n\r");


   return(MMC_EC_OK);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// mmc_read_block()
//                Reads a single block to the mmc
//
// Paramaters:    address:    address to read from
//                block_size: number of bytes in block
//                ptr:        value to store bytes
//
// Returns:       TRUE   if valid data
//                FALSE  if not valid data
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MMC_EC mmc_read_block(int32 address, int16 block_size, int *ptr)   {
   int16 i;
   int16 result;

   mmc_select();
   result=mmc_send_cmd(MMC_CMD_SET_BLOCKLEN,block_size,1);
   mmc_deselect();
   if (result) {   // sets the block size
      MMC_DEBUG(" ERR: couldn't set read block size\r\n");
      return(MMC_EC_SET_READ_BLOCKSIZE_FAIL);
   }

   mmc_select();
   result=mmc_send_cmd(MMC_CMD_READ_SINGLE_BLOCK,address,1);
   if(result)      // command to read byte at address
   {
      mmc_deselect();
      MMC_DEBUG(" ERR: couldnt set read address\r\n");
      return(MMC_EC_SET_READ_ADDRESS_FAIL);
   }

      mmc_receive_data(ptr,block_size);

      mmc_deselect();
      return(MMC_EC_OK);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// mmc_erase()
//                Erases sectors on the mmc
//
// Paramaters:    address:  sector number
//                sectors:  number of sectors to erase
//
// Returns:       <none>
//
// Calls:         send_cmd
//
// Special Info:  All data will be lost in the sectors!
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MMC_EC mmc_erase(int32 address, int32 sectors) {
   int32 sector_start;
   int32 sector_end;
   int8 res;

   if (sectors) {
      sector_start=address/MMC_BLOCK_SIZE;
      sector_end=sector_start + sectors - 1;

         mmc_select();
         res=mmc_send_cmd(MMC_CMD_TAG_SECTOR_START, sector_start, 1);
         mmc_deselect();
         if (res) {return(MMC_EC_ERASE_SECTOR_START_FAIL);}

         mmc_select();
         res=mmc_send_cmd(MMC_CMD_TAG_SECTOR_END, sector_end, 1);
         mmc_deselect();
         if (res) {return(MMC_EC_ERASE_SECTOR_END_FAIL);}

      mmc_select();
      res=mmc_send_cmd(MMC_CMD_ERASE,0,1);
      mmc_deselect();
      if (res) {return(MMC_EC_ERASE_SECTORS_FAIL);}
   }
   return(MMC_EC_OK);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// mmc_read_enable()
//                Enables streamming read mode for the mmc
//
// Paramaters:    address:    start address
//                block_size: size of one block
//
// Returns:       TRUE if enabled
//                FALSE if error
//
// Calls:         send_cmd
//                read_cmd
//
// Special Info:  Use this to enable multiple reads, starting at
//                the address passed in.
//
//                NOT TESTED
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MMC_EC mmc_read_enable(int32 address, int16 block_size)  {
   int16 ret;

   if(mmc_state!= MMC_STANDBY_STATE) {   // make sure not currently in other state
      MMC_DEBUG(" ERR: not in good state for multi-read\r\n");
      return(MMC_EC_BAD_STATE);
   }

   mmc_state = MMC_READ_STATE;          // set to correct state

   mmc_blk_size = block_size;

   mmc_select();
   ret=mmc_send_cmd(MMC_CMD_SET_BLOCKLEN,block_size,1);
   mmc_deselect();
   if(ret) {        // sets the block size
      MMC_DEBUG(" ERR: multi read didnt set block\r\n");
      return(MMC_EC_SET_MULTI_BLOCKSIZE_FAIL);
   }

   mmc_select();
   ret=mmc_send_cmd(MMC_CMD_READ_MULTITPLE_BLOCK,address,1);
   if(ret) {           // sets the address to read from
      MMC_DEBUG(" ERR: doesnt like multi read\r\n");
      mmc_deselect();
      return(MMC_EC_START_MULTI_READ_FAIL);
   }

   return(MMC_EC_OK);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// mmc_read_mult_block()
//                Reads a data byte from the mmc
//
// Paramaters:    <none>
//
// Returns:       TRUE if write worked
//                FALSE if error
//
// Calls:         read_data
//
// Special Info:  Use this after enabling multiple reads.  This
//                reads only a single byte.  Call multiple times
//                to read multiple bytes.
//                NOT TESTED
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
MMC_EC mmc_read_mult_block(int *ptr) {
   if(mmc_state != MMC_READ_STATE) {     // check for correct state
      MMC_DEBUG(" err: bad state\r\n");
      return(MMC_EC_BAD_STATE);
   }

   if (!mmc_receive_data(ptr,mmc_blk_size)) {
      MMC_DEBUG(" err: cant read multi block\r\n");
      mmc_deselect();
      return(MMC_EC_READ_NEXT_BLOCK_FAIL);
   }

   return(MMC_EC_OK);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// mmc_read_disable()
//                Disables multiple reads to the mmc
//
// Paramaters:    <none>
//
// Returns:       <none>
//
// Calls:         send_cmd
//
// Special Info:  Use this when done with multiple reads.  This
//                disables reading, and will cause an error if the
//                user tries another read.
//                NOT TESTED
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#define mmc_read_disable() mmc_stop_transmission()

MMC_EC mmc_stop_transmission(void) {
   int8 res;
   mmc_state = MMC_STANDBY_STATE;

   res=mmc_send_cmd(MMC_CMD_STOP_TRANSMISSION,0,1);
   mmc_deselect();
//   if (res) {
//      MMC_DEBUG(" err: couldnt stop transmission\r\n");
//      return(MMC_EC_STOP_TRANSMISSION_FAIL);
//   }
   return(MMC_EC_OK);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// mmc_write_enable()
//                Enables streamming write mode for the mmc
//
// Paramaters:    address:    start address
//                block_size: size of one block
//
// Returns:       TRUE if enabled
//                FALSE if error
//
// Calls:         send_cmd
//                read_cmd
//
// Special Info:  Use this to enable multiple writes, starting at
//                the address passed in.
//                NOT TESTED
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/* NOT TESTED
short mmc_write_enable(int32 address, int16 block_size) {

   if(mmc_state!= STANDBY_STATE)    // make sure not currently in other state
      return FALSE;

   mmc_state = WRITE_STATE;         // set to correct state

   blk_size = block_size;

   if (mmc_send_cmd(16,block_size,1))         // sets the block size
      return false;

   if(mmc_send_cmd(25,address,1))            // sets the address to write to
      return false;

   return TRUE;
}
*/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// mmc_write_mult_block()
//                Writes a data byte to the mmc
//
// Paramaters:    The byte to write
//
// Returns:       TRUE if write worked
//                FALSE if error
//
// Calls:         send_data
//
// Special Info:  Use this after enabling multiple writes.  This
//                writes only a single byte.  Call multiple times
//                to write multiple bytes.
//                NOT TESTED
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/* NOT TESTED
short mmc_write_mult_block(int *ptr)   {
   int i;

   if(mmc_state != WRITE_STATE)     // check for correct state
      return FALSE;

   mmc_send_data(0xFD);
   for(i=0; i<blk_size; i++)
      mmc_send_data(*ptr++);            // write the data byte
   mmc_send_data(0);                    // blank crc
   mmc_send_data(0);

   if(read_data_response()==5)   // if not stored correctly
      return TRUE;
   else
      return FALSE;
}
*/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// mmc_write_disable()
//                Disables multiple writes to the mmc
//
// Paramaters:    <none>
//
// Returns:       <none>
//
// Calls:         send_cmd
//
// Special Info:  Use this when done with multiple writes.  This
//                disables writing, and will cause an error if the
//                user tries another write.
//                NOT TESTED
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/* NOT TESTED
void  mmc_write_disable()  {

   mmc_state = STANDBY_STATE;

   mmc_send_cmd(12,0,1);      // stops multiple block writes
}
*/

/* NOT TESTED
void  read_csd(int *ptr)   {
   int i;

   mmc_select();
   mmc_send_cmd(9,0,1);

   mmc_read_data();
   for(i=0;i<16;i++)
      *ptr++ = mmc_read_data();

   mmc_read_data();
   mmc_read_data();
   mmc_deselect();
}
*/

/* NOT TESTED
void  write_csd(int *ptr)  {
   int i;

   mmc_select();

   mmc_send_cmd(27,0,1);

   mmc_send_data(0xFE);

   for(i=0;i<16;i++)
      mmc_send_data(*ptr++);

   mmc_send_data(0);
   mmc_send_data(0);

   i= mmc_read_data();
   MMC_DEBUG(" write csd gets %X\n\r", i);

   mmc_deselect();
}
*/

//***********************************************************************//
//************************ Private Functions ****************************//
//***********************************************************************//


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// mmc_send_cmd()
//                sends a command to the mmc, then gets the response
//
// Paramaters:    cmd:  command number
//                arg:  32 bit argument
//                mode: what kind of response should we get?
//                  if mode==1, Format R1 (one byte response)
//                  if mode==2, Format R2 (two byte response)
//                  if mode==3, Format R3 (five byte response)
//                  if mode==0 then don't get a response
//
// Returns:       Error code given by MMC (R1, R2 or R3 format)
//                0 if no error.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int16  mmc_send_cmd(int cmd, int32 arg, int8 mode)  {
   int i;
   int out[6];
   int8 to=50;
   int8 bits;

   if (!mmc_wait_for_not_busy(50)) {
      MMC_DEBUG(" ERR: mmc spi is busy\r\n");
      mmc_last_result=0xFFFF;
      return(mmc_last_result);
   }

   out[0] = cmd | 0x40; // bits 47 to 40
   out[4] = make8(arg,0);     // bits 15 to 8
   out[3] = make8(arg,1);     // bits 23 to 16
   out[2] = make8(arg,2);     // bits 31 to 24
   out[1] = make8(arg,3);     // bits 39 to 32
   if (cmd==0) {
      out[5] = 0x95;
   }
   else {
      out[5] = 0xFF; //ignore the CRC
   }

   for (i=0;i<6;i++)
      mmc_send_data(out[i]);

   MMC_DEBUG("\n\r sent cmd %U, arg %LU\n\r", cmd, arg);

   if (!mode) {return(0);}

   if(mode == 1)
      bits = 8;
   else if(mode == 2)
      bits = 16;
   else if(mode == 3)
      bits = 48;
   else
      bits = 0;

   for(i=0;i<5;i++)
      out[i] = 0;

   i=0;

   do {
      i++;
      mmc_delay_clk();
      output_low(MMC_CLK);
      mmc_delay_clk();
      output_high(MMC_CLK);
   } while((input(MMC_DI))&&(to--));

   if (input(MMC_DI)) {
      MMC_DEBUG(" err: no start bit waiting for response\r\n");
      mmc_last_result=0xFFFE;
      return(mmc_last_result);
   }


   shift_left(out,5,input(MMC_DI));  // input start bit = '0'
   mmc_delay_clk();

   for(i=0; i<(bits-1); i++)
   {
      output_low(MMC_CLK);
      shift_left(out,5,input(MMC_DI));
      mmc_delay_clk();
      output_high(MMC_CLK);
      mmc_delay_clk();
   }

   MMC_DEBUG(" i=%U\r\n",i);
   MMC_DEBUG(" cmd data in = %x", out[0]);

/*
   do {
      out[0]=mmc_read_data();
   } while ((bit_test(out[0],7))&&(to--));

   MMC_DEBUG(" cmd data in = %x", out[0]);

   if (mode==2) {
      out[1]=mmc_read_data();
   }
   if (mode==3) {
      out[2]=mmc_read_data();
      out[3]=mmc_read_data();
      out[4]=mmc_read_data();
   }
*/

   if(mode == 1) {
      MMC_DEBUG("\r\n");
      mmc_last_result=out[0];
   }
   else if(mode == 2) {
      MMC_DEBUG(", %x\r\n",out[1]);
      mmc_last_result=make16(out[1],out[0]);  //not tested
   }
   else if(mode == 3) {
      MMC_DEBUG("\r\n");
      mmc_last_result=out[0];  //not tested
   }

   return(mmc_last_result);
}


void mmc_spi_exchange(int8 out, int8 *in) {
   int8 i;

   for (i=0;i<8;i++) {
/*
      output_bit(MMC_DO, shift_left(&out,1,0));
      mmc_delay_clk();
      output_high(MMC_CLK);
      mmc_delay_clk();
      output_low(MMC_CLK);
      shift_left(in,1,input(MMC_DI));
*/

      output_low(MMC_CLK);
      output_bit(MMC_DO, shift_left(&out,1,0));
      mmc_delay_clk();
      shift_left(in,1,input(MMC_DI));
      output_high(MMC_CLK);
      mmc_delay_clk();

   }
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// mmc_send_data()
//                sends a data byte to the mmc
//
// Paramaters:    the byte sent
//
// Returns:       nothing
//
// Special Info:  You can use this function to generate 8 clocks with no data.
//                Use a paramater of 0xFF to do this.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void mmc_send_data(int8 data) {
   int8 null;
   mmc_spi_exchange(data,&null);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// mmc_read_data()
//                software SPI bit banging to read from MMC
//
// Paramaters:    none
//
// Returns:       data byte read
//
// Special Info:  used for read data respose to write
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int8 mmc_read_data(void) {
   int8 ret;
   mmc_spi_exchange(0xFF,&ret);
   return(ret);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// mmc_select()
//                Sets the MMC CS so the MMC is selected
//
// Paramaters:    none
//
// Returns:       none
//
// Special Info:  Must be called before a mmc_send_cmd()
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void mmc_select(void) {
#ifdef MMC_UNIT_ID
   int i;
   int1 old_gie;
   // If program uses interrupts disable them
   // during this function

   #if defined(__PCM__)
      old_gie=bit_test(*0x0b,7);
      bit_clear(*0x0b,7);
   #elif defined(__PCH__)
      old_gie=bit_test(*0xFF2,7);
      bit_clear(*0xFF2,7);
   #endif

   do {
      do {
         while(!input(MMC_CS));
         delay_us(10);
      } while(!input(MMC_CS));
      output_low(MMC_CS);
      for(i=1;i<=MMC_UNIT_ID;i++)
         delay_us(25);
      output_float(MMC_CS);
      delay_us(5);
   } while(!input(MMC_CS));
   output_low(MMC_CS);
   for(i=1;i<=(8-MMC_UNIT_ID);i++)
       delay_us(25);

   if (old_gie) {
      #if defined(__PCM__)
         bit_set(*0x0b,7);
      #elif defined(__PCH__)
         bit_set(*0xFF2,7);
      #endif
   }
#else
   output_low(MMC_CS);
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// mmc_deselect()
//                Sets the MMC CS so the MMC is no longer selected.
//                Also generates 8 blank clocks which the MMC uses to
//                finish the CMD.
//
// Paramaters:    none
//
// Returns:       none
//
// Special Info:  Most of the time must be called after a mmc_send_cmd(), after
//                the cmd has processed.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void mmc_deselect(void) {
   mmc_send_data(0xFF);  //send 8 clocks for MMC to get ready for next command
   output_high(MMC_CS);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// mmc_wait_for_not_busy()
//                Waits for the DI line to indicicate that the MMC is no longer
//                busy.
//
// Paramaters:    Number of 8 clocks we should wait until line goes high.
//
// Returns:       TRUE if not busy, FALSE if busy.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int1  mmc_wait_for_not_busy(int16 to) {
   int8 i;

   do {
      for (i=0;i<8;i++) {
         mmc_delay_clk();
         output_low(MMC_CLK);
         mmc_delay_clk();
         output_high(MMC_CLK);
      }
   }  while((!input(MMC_DI))&&(to--));

   if (input(MMC_DI))
      return(TRUE);

   return(FALSE);
}

int1 mmc_receive_data(int8 *ptr, int16 size) {
   int8 to=50;
   int16 i;

   do {
      if (mmc_read_data() == 0xFE)
         break;
      else {
         to--;
         delay_ms(1);
         if (!to) {
            MMC_DEBUG(" ERR: didnt get repsone when reading\r\n");
            return(FALSE);
         }
      }
   } while (to);


      for(i=0; i< size; i++) {
         *ptr = mmc_read_data();  // reads each byte
         ptr++;
      }
      mmc_read_data();
      mmc_read_data();

      return(TRUE);
}
