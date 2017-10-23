///////////////////////////////////////////////////////////////////////////////
////                            DS2432.c                             		////
////                                                                 		////
////   				This file contains drivers for a									////
////     DS2432 1k-bit Protected 1-Wire EEPROM with SHA-1 Engine				////
///////////////////////////////////////////////////////////////////////////////
////																								////
//// init_ds2432()																			////
////	*Should call this function before any others.								////
////	*Will reset most 1-Wire devices on the network at once.					////
////	*No returns.																			////
////																								////
//// 														                       			////
//// int8 read_rom(int* chip)																////
////	*Reads in registration data of chip.       									////
////	*Will error if multiple chips on bus.											////
////	*If multiple chips on bus use search_rom.										////
////	*Parameters: 																			////
////		- chip - pointer to an array in which the chip registration data	////
////					will be read into (array must be 8 bytes at least).		////
////					The order in which the data will be placed is as follows:////
////					FAMILY CODE, SERIAL NUMBER 0(LSB), SERIAL NUMBER 1			////
////					SERIAL NUMBER 2, SERIAL NUMBER 3, SERIAL NUMBER 4			////
////					SERIAL NUMBER 5(MSB), CYCLIC REDUNDANCY CHECK NUMBER		////
////	*Returns: ERR_OK, ERR_COLLISION, or ERR_NODEVICE.							////
////																								////
////																								////
//// int8 search_rom(int8* chip_buffer)												////
////	*Reads in registration data of all DS2432 chips on bus.					////
////	*Parameters:																			////
////		- chip_buffer - pointer to an array in which the chip					////
////							 registration data for all chips will be placed		////
////							 (array must contain 8 bytes per DS2432 chip			////
////							 on bus)															////
////							 The order in which the data shall be placed is		////
////							 the same as that specified in read_rom				////
////	*Returns: ERR_OK, ERR_COLLISION, ERR_NODEVICE, or ERR_COMM.				////
////																								////
////																								////
//// int8 read_byte(int8 address, int8* chip, int8* data)						////
////	*Reads the byte specified by address into data.								////
////	*Address can be 0-127, 136-151.													////
////	*Parameters:																			////
////		- address - specifies the address to be read								////
////		- chip - pointer to an array in which the chip registration data	////
////					for the chip to be read is contained.							////
////					The order this data should be in is the same as that		////
////					specified by read_rom.												////
////		- data - pointer which points to the variable that the data read	////
////					from the chip will be placed										////
////	*Returns: ERR_OK, ERR_COLLISION, ERR_RANGE, ERR_SECRET_READ				////
////	 ERR_NODEVICE, or ERR_DEVICE_ID.													////
////																								////
////																								////
//// int8 write_byte(int8 address, int8* chip, int8* secret, int8 data)		////
////	*Writes a byte to the data memory specified by address.					////
////	*Address can be 0-127.																////
////	*Parameters:																			////
////		- address - specifies the address to write to							////
////		- chip - pointer to an array in which the chip registration data	////
////					for the chip to be written to is contained.					////
////					The order for this data should be that same as that		////
////					specified by read_rom												////
////		- secret - pointer to an array that contains the secret of the		////
////					  chip.																	////
////					  This array should be from LSB to MSB and 8 bytes.		////
////		- data - specifies the byte to be written to the address on the	////
////					chip																		////
////	*Returns: ERR_OK, ERR_COMM, ERR_COLLISION, ERR_DEVICE_ID,				////
////	 ERR_NODEVICE, ERR_RANGE, ERR_SECRET_OW, ERR_BAD_AUTH,					////
////	 or ERR_NO_WRITE.																		////
////																								////
////	 																							////
//// int8 load_secret(int8* chip, int8* secret)										////
////	*Loads a user specified secret into a user specified chip				////
////	*Parameters:																			////
////		- chip - pointer to an array which contains the registration data	////
////					of the chip to be loaded with the secret.						////
////					This chip should follow the order specified in read_rom	////
////		- secret - pointer to an array containing the secret to be loaded	////
////					  onto the specified chip.											////
////					  This array should be from LSB to MSB and 8 bytes.		////
////	*Returns: ERR_OK, ERR_COMM, ERR_COLLISION, ERR_DEVICE_ID,				////
////	 ERR_NODEVICE, or ERR_BAD_AUTH.													////
////																								////
////																								////
//// int8 read_auth_page(int8 page, int8* chip, int8* secret,					////
////								int8* data, int8* challenge)							////
////	*Reads a complete page of validated data memory.							////
////	*Page can be 0-3.																		////
////	*Parameters:																			////
////		- page - specifies which page to read										////
////		- chip - pointer to an array containing the chip registration		////
////					data of the chip to be read from.								////
////					The order of this array should be the same as the one		////
////					specified in read_rom												////
////		- secret - pointer to an array containing the secret of the chip.	////
////					  This array should be from LSB to MSB and 8 bytes.		////
////		- data - pointer to an array where the page data will be placed	////
////					if there are no errors (should be at least 32 bytes)		////
////		- challenge - pointer to an array containing 3 bytes					////
////	*Returns: ERR_OK, ERR_COMM, ERR_COLLISION, ERR_DEVICE_ID, 				////
////	 ERR_NODEVICE, or ERR_PAGE_RANGE													////
////																								////
////																								////
//// int8 compute_secret(int8 page, int8* chip, int8* secret, int8* data)	////
////	*Computes a new secret from information contained on chip and			////
////	 user specified challenge															////
////	*Page can be 0-3.																		////
////	*Parameters:																			////
////		- page - specifies which page to use to compute new secret			////
////		- chip - pointer to an array which contains the chip registration	////
////					data for the chip having a secret calculated.				////
////					The order for this array should follow that specified in	////
////					read_rom																	////
////		- secret - pointer to an array which contains the current secret	////
////					  of the chip.															////
////					  This array should be from LSB to MSB	and 8 bytes.		////
////					  If no errors occur the new secret will be placed in		////
////					  the array LSB to MSB.												////
////		- data - pointer to an array containing 8 bytes which will be		////
////					used to calculate a new secret									////
////	*Returns: ERR_OK, ERR_COLLISION, ERR_DEVICE_ID, ERR_NODEVICE,			////
////	 ERR_PAGE_RANGE, or ERR_PROTECTED												////
////																								////
////																								////
//// int8 protect_secret(int8* chip, int8* secret)									////
////	*Write protects secret of specified chip.										////
////	*CAN ONLY BE DONE ONCE PER CHIP AND CANNOT BE UNDONE!						////
////	*Parameters:																			////
////		- chip - pointer to an array containing the chip registration		////
////					data of the chip to be protected									////
////					This array should follow the order specified in read_rom	////
////		- secret - pointer to an array which contains the secret of the 	////
////					  specified chip.														////
////					  This array should be from LSB to MSB and 8 bytes			////
////	*Returns: ERR_OK, ERR_COMM, ERR_COLLISION, ERR_DEVICE_ID,				////
////	 ERR_NODEVICE, ERR_RANGE, ERR_SECRET_OW, ERR_BAD_AUTH,					////
////	 or ERR_NO_WRITE.																		////
////																								////
////																								////
//// int8 protect_page_0(int8* chip, int8* secret)									////
////	*Write protects data memory page 0 of specified chip.						////
////	*CAN ONLY BE DONE ONCE PER CHIP AND CANNOT BE UNDONE!						////
////	*Parameters:																			////
////		- chip - pointer to an array containing the chip registration		////
////					data of the chip to be protected									////
////					This array should follow the order specified in read_rom	////
////		- secret - pointer to an array which contains the secret of the 	////
////					  specified chip.														////
////					  This array should be from LSB to MSB and 8 bytes			////
////	*Returns: ERR_OK, ERR_COMM, ERR_COLLISION, ERR_DEVICE_ID,				////
////	 ERR_NODEVICE, ERR_RANGE, ERR_SECRET_OW, ERR_BAD_AUTH,					////
////	 or ERR_NO_WRITE.																		////
////																								////
////																								////
//// int8 protect_all_pages(int8* chip, int8* secret)								////
////	*Write protects all data memory pages of the specified chip.			////
////	*CAN ONLY BE DONE ONCE PER CHIP AND CANNOT BE UNDONE!						////
////	*Parameters:																			////
////		- chip - pointer to an array containing the chip registration		////
////					data of the chip to be protected									////
////					This array should follow the order specified in read_rom	////
////		- secret - pointer to an array which contains the secret of the 	////
////					  specified chip.														////
////					  This array should be from LSB to MSB and 8 bytes			////
////	*Returns: ERR_OK, ERR_COMM, ERR_COLLISION, ERR_DEVICE_ID,				////
////	 ERR_NODEVICE, ERR_RANGE, ERR_SECRET_OW, ERR_BAD_AUTH,					////
////	 or ERR_NO_WRITE.																		////
////																								////
////																								////
//// int8 page_1_EPROM_mode(int8* chip, int8* secret)								////
////	*Activates EPROM mode for data memory page 1 of specified chip.		////
////	*CAN ONLY BE DONE ONCE PER SHIP AND CANNOT BE UNDONE!						////
////	*Parameters:																			////
////		- chip - pointer to an array containing the chip registration		////
////					data of the chip to be changed									////
////					This array should follow the order specified in read_rom	////
////		- secret - pointer to an array which contains the secret of the 	////
////					  specified chip.														////
////					  This array should be from LSB to MSB and 8 bytes			////
////	*Returns: ERR_OK, ERR_COMM, ERR_COLLISION, ERR_DEVICE_ID,				////
////	 ERR_NODEVICE, ERR_RANGE, ERR_SECRET_OW, ERR_BAD_AUTH,					////
////	 or ERR_NO_WRITE.																		////
////																								////
////																								////
//// int8 activate_overdrive()															////
////	*Activates overdrive mode which accomplishes read/write					////
////	 operations in about 1/10th the time of normal operating speed.		////
////	*Returns: ERR_OK, ERR_COLLISION, or ERR_NODEVICE.							////
////																								////
////																								////
//// int8 deactivate_overdrive()															////
////	*Deactivates overdrive returning read/write operations to normal		////
////	 speed.																					////	
////	*Returns: ERR_OK or ERR_NODEVICE.												////
////																								////
///////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2004 Custom Computer Services        			////
//// This source code may only be used by licensed users of the CCS  		////
//// C compiler.  This source code may only be distributed to other  		////
//// licensed users of the CCS C compiler.  No other use,            		////
//// reproduction or distribution is permitted without written       		////
//// permission.  Derivative programs created using this software    		////
//// in object code form are not restricted in any way.              		////
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////
//User Defined Variables
#ifndef TOUCH_PIN
   #define TOUCH_PIN  PIN_B0
#endif
///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
//Constants
///////////////////////////////////////////////////////
#define CRC_POLY		0x8C		//Polynomial used to generate 8-bit CRCs
#define CRC_16_POLY	0xA001	//Polynomial used to generate 16-bit CRCs
#define ADDRESS_MSB	0x00		//Upper byte of the DS2432 address - will always be 0
int8 FAMILY_CODE = 	0x33;		//Family code of the DS2432 chip
///////////////////////////////////////////////////////
//DS2432 ROM Command Codes
#define READ_ROM_CMD			0x33
#define MATCH_ROM_CMD		0x55
#define SEARCH_ROM_CMD		0xF0
#define SKIP_ROM_CMD			0xCC
#define OD_SKIP_ROM_CMD		0x3C
#define 0D_MATCH_ROM_CMD	0x69
#define RESUME_CMD			0xA5
///////////////////////////////////////////////////////
//DS2432 Memory and SHA Command Codes
#define WRITE_SP_CMD			0x0F
#define READ_SP_CMD			0xAA
#define LOAD_SECRET_CMD		0x5A
#define COMPUTE_SECRET_CMD	0x33
#define COPY_SP_CMD			0x55
#define READ_AUTH_PAGE_CMD	0xA5
#define READ_MEMORY_CMD		0xF0
///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
//Error Codes
#define ERR_OK				0x00	//No errors
#define ERR_COMM			0x01	//Communication error
#define ERR_NODEVICE		0x02	//No touch device present on touch pin
#define ERR_RANGE			0x03	//Trying to access a byte not allowed by the function
#define ERR_SECRET_READ	0x04	//Trying to read the secret
#define ERR_DEVICE_ID	0x05	//No device chosen to write to
#define ERR_SECRET_OW	0x06	//A byte of the function would be overwritten
#define ERR_NO_WRITE		0x07	//The byte was not written successfully
#define ERR_BAD_AUTH		0x08	//The authorization bytes did not match the chips
#define ERR_COLLISION	0x09	//Data collision during communication with device
#define ERR_PROTECTED	0x0A	//Area being written to is write protected
#define ERR_PAGE_RANGE	0x0B	//Given page number doesn't exist
///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
//Interface Function Prototypes
void init_ds2432();
int8 read_rom(int8* buffer);
int8 search_rom(int8* chip_buffer);
int8 read_byte(int8 address, int8* chip, int8* data);
int8 write_byte(int8 address, int8* chip, int8* secret, int8 data);
int8 load_secret(int8* chip, int8* secret);
int8 read_auth_page(int8 page, int8* chip, int8* secret, int8* data, int8* challenge);
int8 compute_secret(int8 page, int8* chip, int8* secret, int8* data);
int8 protect_secret(int8* chip, int8* secret);
int8 protect_page_0(int8* chip, int8* secret);
int8 protect_all_pages(int8* chip, int8* secret);
int8 page_1_EPROM_mode(int8* chip, int8* secret);
int8 activate_overdrive();
int8 deactivate_overdrive();
///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
//ROM Function Prototypes
int8 match_rom(int8* buffer);
int8 resume();
///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
//Memory Function Prototypes
int8 read_sp(int8* data);
int8 write_sp(int8 address, int8* data, int8 bytes);
int8 read_block(int8 address, int8* data, int8 bytes);
///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
//Read/Write Function Prototypes
BYTE touch_read_byte();
BYTE touch_write_byte(BYTE data);
int1 touch_present();
int1 read_bit();
int1 write_bit(int1 bit);
///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
//Miscellaneous Function Prototypes
int8 get_auth(int8* bytes);
int1 verify_CRC(int8 crc, int8 test);
int1 verify_CRC16(int16 crc, int16 test);
int8 calc_CRC(int8* data, int8 bytes);
int16 calc_CRC16(int8* data, int8 bytes);
void gen_MAC(int8* data, int8* result);
int32 NLF(int32 b, int32 c, int32 d, int8 num);
int32 KTN(int8 num);
///////////////////////////////////////////////////////


///////////////////////////////////////////////////////
//Internal Variables
int1 override = FALSE;
int1 overdrive = FALSE;
///////////////////////////////////////////////////////


//init_ds2432
//Purpose:	To reset the ds2432 chip(s) by depriving them of power
//Inputs:	NONE
//Outputs:	NONE
void init_ds2432()
{
	output_low(TOUCH_PIN);
	delay_ms(30);
	output_float(TOUCH_PIN);
	delay_ms(5);
} //init_ds2432


//read_byte
//Purpose: 	To read a byte of memory from the DS2432 chip.  Any byte can be read
//				but those belonging to the secret.
//Inputs:	The address of the byte to be read, A pointer to the chip registration
//				data, and a pointer to the variable to have the information read to.
//\\//\\//\\//\\//\\//\\//\/\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\\//\//\//\\//
//				NOTE:  If a chip's registration data is passed it must be in the following format:
//				Family ID Byte, Serial Byte 0, Serial Byte 1, Serial Byte 2,
//				Serial Byte 3, Serial Byte 4, Serial Byte 5, CRC Byte
//Outputs:	An int representing an error code.
int8 read_byte(int8 address, int8* chip, int8* data)
{
	int8 err = ERR_OK;

	//Check to make sure address is in range and then also that it is not a byte
	//of the secret
	if(address >= 0x98)
		err = ERR_RANGE;
	else if(address >= 0x80 && address <= 0x87)
		err = ERR_SECRET_READ;

	//Match given chip
	if(!err)
	{
		if(*chip == FAMILY_CODE)
			err = match_rom(chip);
		else
			err = ERR_DEVICE_ID;

		//If contact made with chip then read memory address
		if(!err)
			err = read_block(address, data, 1);
	}

	touch_present();

	return err;
} //read_byte


//write_byte
//Purpose:  To write a byte to the DS2432
//Inputs: 	A byte containing the address to be written to, a pointer to the an array
//				containing the chip registration (This array must adhere to the following standard:
//				Family ID Byte, Serial Byte 0, Serial Byte 1, Serial Byte 2,
//				Serial Byte 3, Serial Byte 4, Serial Byte 5, CRC Byte),
//				a pointer to an array containing the secret (this array must contain the
//				secret in the from LSB to MSB), and an int containing the byte of data
//				to be written
//Outputs:	An int representing an error code
int8 write_byte(int8 address, int8* chip, int8* secret, int8 data)
{
	int8 sha_input[64], err = ERR_OK, i, page, temp, mac_array[20], scratch[11];

	page = address & 0xE0;

	if(!override)
		if(address >= 0x88)
			err = ERR_RANGE;
		else if(address >= 0x80 && address <= 0x87)
			err = ERR_SECRET_OW;

	if(!err)
	{
		if(*chip == FAMILY_CODE)
			err = match_rom(chip);
		else
			err = ERR_DEVICE_ID;

		if(!err)
		{
			//Build array needed for MAC calculation
			//See DS2432 datasheet for details
			//M0
			for(i = 0; i < 4; ++i)
				sha_input[i] = *(secret + i);

			//M1 - M7
			if(page == 0x80)
			{
				for(i = 0; i < 8; i++)
					sha_input[i + 4] = *(secret + i);
				if(!err)
					err = resume();
				if(!err)
						err = read_block((page + 8), (sha_input + 12), 20);
				sha_input[28] = 0xFF; sha_input[29] = 0xFF; sha_input[30] = 0xFF; sha_input[31] = 0xFF;
			}
			else
			{
				if(!err)
					err = resume();
				if(!err)
					err = read_block(page, (sha_input + 4), 28);
			}

			if(!err)
				err = resume();

			//Write data to the scratch pad
			if(!err)
				err = write_sp(address, &data, 1);

			if(!err)
				err = resume();

			//M8 - M9
			if(!err)
			{
				err = read_sp(scratch);

				if(!err)
				{
					for(i = 3; i < 11; i++)
						sha_input[i + 29] = scratch[i];

					//sha_input[address - page] = data;

					//M10[31:24]
					sha_input[40] = 0x00 | (address >> 5);

					//M10[23:0] - M11
					for(i = 0; i < 7; i++)
						sha_input[i + 41] = *(chip + i);

					//M12
					for(i = 4; i < 8; i++)
						sha_input[i + 44] = *(secret + i);

					//M13-M15
					sha_input[52] = 0xFF; sha_input[53] = 0xFF; sha_input[54] = 0xFF; sha_input[55] = 0x80;
					sha_input[56] = 0x00; sha_input[57] = 0x00; sha_input[58] = 0x00; sha_input[59] = 0x00;
					sha_input[60] = 0x00; sha_input[61] = 0x00; sha_input[62] = 0x01; sha_input[63] = 0xB8;

					//Generate the MAC using SHA-1 algorithm
					gen_MAC(sha_input, mac_array);

					err = resume();

					if(!err)
					{
						//Copy the scratchpad to memory
						if(!touch_write_byte(COPY_SP_CMD))
							err = ERR_COLLISION;

						if(!err)
							for(i = 0; i < 3; i++)
								if(!touch_write_byte(scratch[i]))
									err = ERR_COLLISION;

						if(!err)
							for(i = 0; i < 20; i++)
								if(!touch_write_byte(mac_array[i]))
									err = ERR_COLLISION;


						delay_ms(15);

						temp = touch_read_byte();
						//If all 0's are read then the MAC address did not match
						//the one the chip calculated
						if(temp == 0 && !err)
							err = ERR_NO_WRITE;
						else if(temp == 0xFF && !err)
							err = ERR_BAD_AUTH;
					}
				}
			}
		}
	}

	touch_present();

	return err;
}


//load_secret
//Purpose:	To load a secret into the DS2432 chip
//Inputs:	A pointer to an array that holds the 8 secret bytes, and a pointer to an
//				array which contains a chip registration code
//Outputs:	An int representing an error code
int8 load_secret(int8* chip, int8* secret)
{
	int8 auth_bytes[3], i, err = ERR_OK, temp;

	if(chip == 0)
		err = resume();
	else
		if(*chip == FAMILY_CODE)
			err = match_rom(chip);
		else
			err = ERR_DEVICE_ID;

	if(!err)
		err = write_sp(0x80, secret, 8);

	if(!err)
	{
		err = resume();

		if(!err)
			err = get_auth(auth_bytes);

		if(!err)
		{
			err = resume();

			if(!err)
			{
				if(!touch_write_byte(LOAD_SECRET_CMD))
					err = ERR_COLLISION;

				if(!err)
				{
					for(i = 0; i < 3; i++)
						if(!touch_write_byte(auth_bytes[i]))
							err = ERR_COLLISION;

					if(!err)
					{
						delay_ms(11);

						temp = touch_read_byte();

						if(temp == 0xFF)
							err = ERR_BAD_AUTH;
					}
				}
			}
		}
	}

	touch_present();

	return err;
} //load_secret


//read_auth_page
//Purpose:	To read a verified page of memory.
//Inputs:	An int representing the page of memory to be read, a pointer to an
//				array of int8's (This array must adhere to the following standard:
//				Family ID Byte, Serial Byte 0, Serial Byte 1, Serial Byte 2,
//				Serial Byte 3, Serial Byte 4, Serial Byte 5, CRC Byte), a pointer
//				to an array of int8's representing the current secret of the chip
//				(This array must be from LSB to MSB), and a pointer to an array of
//				int8's that the page's data will be placed (this array must be at
//				least 32 bytes)
//Outputs:	An int8 representing an error code
int8 read_auth_page(int8 page, int8* chip, int8* secret, int8* data, int8* challenge)
{
	int8 err = ERR_OK, scratch[36], i, temp, sha_input[64], mac_array[20];
	int8 chip_MAC[20], address;
	int16 crc;

	if(page < 4)
	{
		address = page * 0x20;

      if(*chip == FAMILY_CODE)
 	   	err = match_rom(chip);
      else
   		err = ERR_DEVICE_ID;

      if(!err)
      {
			for(i = 0; i < 4; i++)
         {
    	     scratch[i] = 0x00;
         }

         scratch[4] = *challenge;
         scratch[5] = *(challenge + 1);
         scratch[6] = *(challenge + 2);
			scratch[7] = 0x00;

         err = write_sp(address, scratch, 8);

         if(!err)
         {
   	      err = resume();

            if(!err)
            {
   	         if(!touch_write_byte(READ_AUTH_PAGE_CMD))
      	         err = ERR_COLLISION;

					if(!err)
			      	if(!touch_write_byte(address))
            			err = ERR_COLLISION;

					if(!err)
						if(!touch_write_byte(ADDRESS_MSB))
               		err = ERR_COLLISION;

					if(!err)
					{
               	for(i = 0; i < 33; i++)
                  	scratch[i + 3] = touch_read_byte();

                  temp = touch_read_byte();
                  crc = make16(touch_read_byte(), temp);

                  scratch[0] = READ_AUTH_PAGE_CMD;
                  scratch[1] = address;
                  scratch[2] = ADDRESS_MSB;

                  if(!verify_CRC16(crc, calc_CRC16(scratch, 36)))
                  	err = ERR_COMM;

                  if(!err)
   	            {
      	            for(i = 0; i < 4; i++)
         	            sha_input[i] = *(secret + i);

   	               for(i = 3; i < 35; i++)
      	               sha_input[i + 1] = scratch[i];

                     sha_input[36] = 0xFF; sha_input[37] = 0xFF; sha_input[38] = 0xFF; sha_input[39] = 0xFF;

                     sha_input[40] = page | 0x40;

                     for(i = 0; i < 7; i++)
   	                  sha_input[i + 41] = *(chip + i);

                     for(i = 4; i < 8; i++)
                        sha_input[i + 44] = *(secret + i);

                     sha_input[52] = *challenge; sha_input[53] = *(challenge + 1);
                     sha_input[54] = *(challenge + 2); sha_input[55] = 0x80;
                     sha_input[56] = 0x00; sha_input[57] = 0x00; sha_input[58] = 0x00; sha_input[59] = 0x00;
                     sha_input[60] = 0x00; sha_input[61] = 0x00; sha_input[62] = 0x01; sha_input[63] = 0xB8;

                     gen_MAC(sha_input, mac_array);

							delay_ms(2);

                     for(i = 0; i < 20; i++)
               	      chip_MAC[i] = touch_read_byte();

                     temp = touch_read_byte();
                     crc = make16(touch_read_byte(), temp);

                     if(!verify_CRC16(crc, calc_CRC16(chip_MAC, 20)))
                 	   	err = ERR_COMM;

                     if(!err)
                 	   	if(!verify_CRC16(crc, calc_CRC16(mac_array, 20)))
                        	err = ERR_COMM;

                     if(!err)
                     	for(i = 3; i < 35; i++, data++)
                        	*(data) = scratch[i];
						}
					}
				}
			}
		}
	}
	else
		err = ERR_PAGE_RANGE;

	touch_present();

	return err;
} //read_auth_page


//compute_secret
//Purpose:	To compute a new secret based on the contents of a page, the current
//				secret, the contents of the scratchpad, and the registration code of
//				the chip.
//Inputs:	An int8 representing the page of the chip to use for calculating
//				the new secret, a pointer to an array of the chip's registration
//				data (This array must adhere to the following standard:
//				Family ID Byte, Serial Byte 0, Serial Byte 1, Serial Byte 2,
//				Serial Byte 3, Serial Byte 4, Serial Byte 5, CRC Byte), a pointer
//				to an array of int8's representing the chip's current secret (This
//				array must be from LSB to MSB), and a pointer to array of ints
//				representing data to be written into the scratchpad (This must be an
//				8 byte array)
//Outputs:	An int8 representing an error code
int8 compute_secret(int8 page, int8* chip, int8* secret, int8* data)
{
	int8 sha_input[64], mac_array[20], scratch[32], address, temp, i, err = ERR_OK;

	if(page < 4)
		address = page * 0x20;
	else
		err = ERR_PAGE_RANGE;

	if(!err)
	{
		if(*chip == FAMILY_CODE)
			err = match_rom(chip);
		else
			err = ERR_DEVICE_ID;

		if(!err)
			err = write_sp(address, data, 8);

		if(!err)
			err = resume();

		if(!err)
			err = read_block(address, scratch, 32);

		if(!err)
		{
			//M0
			for(i = 0; i < 4; i++)
				sha_input[i] = secret[i];

			//M1-M8
			for(; i < 36; i++)
				sha_input[i] = scratch[i - 4];

			//M9
			sha_input[36] = 0xFF; sha_input[37] = 0xFF; sha_input[38] = 0xFF; sha_input[39] = 0xFF;

			//M10 - M11
			sha_input[40] = ((*data) & 0x3F); sha_input[41] = data[1];
			sha_input[42] = data[2]; sha_input[43] = data[3];
			sha_input[44] = data[4]; sha_input[45] = data[5];
			sha_input[46] = data[6]; sha_input[47] = data[7];

			//M12
			for(i = 4; i < 8; i++)
				sha_input[i + 44] = secret[i];

			//M13-M15
			sha_input[52] = 0xFF; sha_input[53] = 0xFF; sha_input[54] = 0xFF; sha_input[55] = 0x80;
			sha_input[56] = 0x00; sha_input[57] = 0x00; sha_input[58] = 0x00; sha_input[59] = 0x00;
			sha_input[60] = 0x00; sha_input[61] = 0x00; sha_input[62] = 0x01; sha_input[63] = 0xB8;

			err = resume();

			if(!err)
			{
				if(!touch_write_byte(COMPUTE_SECRET_CMD))
					err = ERR_COLLISION;

				if(!err)
					if(!touch_write_byte(address))
						err = ERR_COLLISION;

				if(!err)
					if(!touch_write_byte(ADDRESS_MSB))
						err = ERR_COLLISION;

				if(!err)
				{
					gen_MAC(sha_input, mac_array);

					delay_ms(11);

					temp = touch_read_byte();

					if(temp == 0xFF)
						err = ERR_PROTECTED;
					else
						for(i = 0; i < 8; i++)
							*(secret + i) = mac_array[i];
				}
			}
		}
	}

	touch_present();

	return err;
} //compute_secret


//protect_secret
//Purpose:	Write protects secret of the specified chip
//Inputs:	See documentation on top of file
//Outputs:	See documentation on top of file
int8 protect_secret(int8* chip, int8* secret)
{
	int8 err = ERR_OK;

	override = TRUE;
	err = write_byte(0x88, chip, secret, 0xAA);
	override = FALSE;

	return err;
} //protect_secret


//protect_page_0
//Purpose:	Write protects page 0
//Inputs:	See documentation on top of file
//Outputs:	See documentation on top of file
int8 protect_page_0(int8* chip, int8* secret)
{
	int8 err = ERR_OK;

	override = TRUE;
	err = write_byte(0x8D, chip, secret, 0xAA);
	override = FALSE;

	return err;
} //protect_page_0


//protect_all_pages
//Purpose:	Write protects all data pages
//Inputs:	See documentation on top of file
//Outputs:	See documentation on top of file
int8 protect_all_pages(int8* chip, int8* secret)
{
	int8 err = ERR_OK;

	override = TRUE;
	err = write_byte(0x89, chip, secret, 0xAA);
	override = FALSE;

	return err;
}


//page_1_EPROM_mode
//Purpose:	Puts data memory page 1 into EPROM mode
//Inputs:	See documentation on top of file
//Outputs:	See documentation on top of file
int8 page_1_EPROM_mode(int8* chip, int8* secret)
{
	int8 err = ERR_OK;

	override = TRUE;
	err = write_byte(0x8C, chip, secret, 0xAA);
	override = FALSE;

	return err;
}


//activate_overdrive
//Purpose:	Puts DS2432 into overdrive mode
//Inputs:	See documentation on top of file
//Outputs:	See documentation on top of file
int8 activate_overdrive()
{
	int8 err = ERR_OK;
	
	if(touch_present())
	{
		if(!touch_write_byte(OD_SKIP_ROM_CMD))
			err = ERR_COLLISION;
			
		if(!err)
			overdrive = TRUE;
	}
	else
		err = ERR_NODEVICE;
	
	return err;
} //activate_overdrive


//deactivate_overdrive
//Purpose:	Brings DS2432 chips out of overdrive mode
//Inputs:	See documentation on top of file
//Outputs:	See documentation on top of file
int8 deactivate_overdrive()
{
	int8 err = ERR_OK;
	
	overdrive = FALSE;
	
	if(!touch_present())
		err = ERR_NODEVICE;
	
	return err;
} //deactivate_overdrive


//read_rom
//Purpose:  To read the laser ROM contained the family code and chip serial
//				number
//Inputs:	A pointer to an array of int8 (must contain at least 7 elements)
//				NOTE: This array will be filled to the following format:
//				Family ID Byte, Serial Byte 0, Serial Byte 1, Serial Byte 2,
//				Serial Byte 3, Serial Byte 4, Serial Byte 5, CRC Byte
//Outputs:	An int1 that indicates success: 0 = failure; 1 = success
int8 read_rom(int8* buffer)
{
	int8 i, err = ERR_OK;
	int8 *s;

	s = buffer;

	if(touch_present())
	{
		if(!touch_write_byte(READ_ROM_CMD))
			err = ERR_COLLISION;

		if(!err)
			for(i=0; i<8; ++i, ++buffer)
	   		(*buffer)=touch_read_byte();

		if(!verify_CRC(*(s + 7), calc_CRC(s, 7)) && !err)
			err = ERR_COMM;
	}
	else
		err = ERR_NODEVICE;

	return err;
} // read_rom


//match_rom - INTERNAL FUNCTION
//Purpose:	To activate a single DS2432 on a bus
//Inputs: 	A pointer to an array of ints containing a 64-bit registration address
//			 	This registration address should have the following format:
//			 	Family ID Byte, Serial Byte 0, Serial Byte 1, Serial Byte 2,
//			 	Serial Byte 3, Serial Byte 4, Serial Byte 5, CRC Byte
//Outputs: 	NONE
int8 match_rom(int8* buffer)
{
	int8 i, err = ERR_OK;

	if(touch_present())
	{
		if(!err)
			if(!touch_write_byte(MATCH_ROM_CMD))
				err = ERR_COLLISION;

		if(!err)
			for(i = 0; i < 8; ++i)
				if(!touch_write_byte(*(buffer + i)))
					err = ERR_COLLISION;
	}
	else
		err = ERR_NODEVICE;

	return err;
} //match_rom


//search_rom
//Purpose:	Searches bus for all DS2432 chips registration data
//Inputs:	See documentation on top of file
//Outputs:	See documentation on top of file
int8 search_rom(int8* chip_buffer)
{
	int8 i, z, temp, err = ERR_OK, last_conflict, max_conflict = 0, conflict_counter, conflict_transmit[48];
	int1 proceed = TRUE, bit, inverse, conflict = FALSE;

	while(!err && proceed)
	{
		conflict_counter = 0;
		last_conflict = 0;
		
		if(!touch_present())
			err = ERR_NODEVICE;

		if(!err)
			if(!touch_write_byte(SEARCH_ROM_CMD))
				err = ERR_COLLISION;

		for(i = 0; i < 8 && proceed && !err; i++)
		{
			bit = read_bit();
			inverse = read_bit();

			if(bit == bit_test(FAMILY_CODE, i) || (bit == inverse && bit == 0))
			{
				if(!write_bit(bit_test(FAMILY_CODE, i)))
					err = ERR_COLLISION;

				if(i == 7 && !err)
				{
					*chip_buffer = FAMILY_CODE;
					chip_buffer++;
				}
			}
			else if(bit == inverse && bit == 1)
				err = ERR_COMM;
			else
			{
				if(!write_bit(bit_test(FAMILY_CODE, i)))
					err = ERR_COLLISION;
				proceed = FALSE;
			}
		}

		for(i = 1; i < 8 && !err; i++)
		{
			for(z = 1; z < 9 && !err; z++)
			{
				bit = read_bit();
				inverse = read_bit();

				if(bit == inverse && bit == 0)
				{
					conflict = TRUE;
					
					if(max_conflict == 0)
					{
						max_conflict = (i * z);
						
						if(!write_bit(0))
							err = ERR_COLLISION;
							
						if(!err)
						{
							conflict_transmit[conflict_counter] = 0;
							bit = 0;
						}
					}
					else if(max_conflict == (i * z))
					{
						if(!write_bit(1))
							err = ERR_COLLISION;

						if(!err)
						{
							if(last_conflict != 0)
								max_conflict = last_conflict;
							else
								conflict = FALSE;
								
							conflict_transmit[conflict_counter] = 1;
							bit = 1;
						}
					}
					else if(max_conflict > (i * z))
					{
						if(!write_bit(conflict_transmit[conflict_counter]))
							err = ERR_COLLISION;
					}
					else if(max_conflict < (i * z))
					{
						if(!write_bit(0))
							err = ERR_COLLISION;
						
						if(!err)
						{
							last_conflict = max_conflict;
							max_conflict = i * z;
							conflict_transmit[conflict_counter] = 0;
							bit = 0;
						}
					}
					conflict_counter++;
				}
				else if(bit == inverse && bit == 1)
					err = ERR_COMM;
				else
					if(!write_bit(bit))
						err = ERR_COLLISION;

				shift_right(&temp, 1, bit);
			}
			*chip_buffer = temp;
			chip_buffer++;
		}
		
		if(!verify_CRC(*(chip_buffer - 1), calc_CRC((chip_buffer - 8), 7)))
			err = ERR_COMM;

		if(!conflict)
			proceed = FALSE;
		else
			conflict = FALSE;
	}

	touch_present();

	return err;
} //search_rom


//resume - INTERNAL FUNCTION
//Purpose:	Resume giving memory commands to DS2432 that last successfully used
//				match_rom with
//Inputs:	NONE
//Outputs:	NONE
int8 resume()
{
	int8 err = ERR_OK;

	if(touch_present())
	{
		if(!touch_write_byte(RESUME_CMD))
			err = ERR_COLLISION;
	}
	else
		err = ERR_NODEVICE;

	return err;
} //resume


//read_sp - INTERNAL FUNCTION
//Purpose:	To read the data in the TA1, TA2, ES, and scratchpad
//Inputs:	A pointer to an array of ints to hold the 11 bytes of data
//				received from the chip
//Outputs:	An int8 representing an error code
int8 read_sp(int8* data)
{
	int8 err = ERR_OK, scratch[12], i, temp;
	int16 crc;

	scratch[0] = READ_SP_CMD;

	err = resume();

	if(!err)
		if(!touch_write_byte(READ_SP_CMD))
			err = ERR_COLLISION;

	if(!err)
	{
		for(i = 1; i < 12; i++)
			scratch[i] = touch_read_byte();

		crc = touch_read_byte();
		temp = touch_read_byte();

		crc = make16(temp, crc);

		if(!verify_CRC16(crc, calc_CRC16(scratch, 12)))
			err = ERR_COMM;
		else
			for(i = 0; i < 11; i++)
				*(data + i) = scratch[i + 1];
	}

	touch_present();

	return err;
} //read_sp


//write_sp - INTERNAL FUNCTION
//Purpose:	Writes data to the scratch pad
//Inputs:	An int for the address of where the data is intended to go, a
//				pointer to an array of ints which contains the data to be written
//				(which may be no larger than 8 bytes), and an int that represents
//				how many bytes will be written
//Outputs:	An int representing an error code
int8 write_sp(int8 address, int8* data, int8 bytes)
{
	int8 err = ERR_OK, scratch[11], temp, i;
	int16 crc;

	temp = address & 0xF8;

	err = resume();

	if(!err)
		err = read_block(temp, (scratch + 3), 8);

	if(!err)
	{
		for(i = 0; i < bytes; i++)
			scratch[((address - temp + i) + 3)] = *(data + i);

		scratch[0] = WRITE_SP_CMD;
		scratch[1] = address;
		scratch[2] = ADDRESS_MSB;

		err = resume();

		if(!err)
		{
			//Send write scratchpad command
			if(!touch_write_byte(WRITE_SP_CMD))
				err = ERR_COLLISION;

			//Send lower byte of the address
			if(!err)
				if(!touch_write_byte(address))
					err = ERR_COLLISION;

			//Send upper byte of the address
			if(!err)
				if(!touch_write_byte(ADDRESS_MSB))
					err = ERR_COLLISION;

			if(!err)
			{
				for(i = 3; i < 11; i++)
					if(!touch_write_byte(scratch[i]) && !err)
						err = ERR_COLLISION;
			}

			if(!err)
			{
				crc = touch_read_byte();
				temp = touch_read_byte();

				crc = make16(temp, crc);

				if(!verify_CRC16(crc, calc_CRC16(scratch, 11)))
					err = ERR_COMM;
			}
		}
	}

	touch_present();

	return err;
} //write_sp


//read_block - INTERNAL FUNCTION
//Purpose:	Reads a number of user specified bytes
//Inputs:	An int representing the address of the byte to start reading from,
//				a pointer to an array where the read bytes will be stored, and an
//				int holding the number of bytes to be read in
//Outputs:	NONE
int8 read_block(int8 address, int8* data, int8 bytes)
{
	int8 i, err = ERR_OK;

	//Send read memory command
	if(!touch_write_byte(READ_MEMORY_CMD))
		err = ERR_COLLISION;

	//Send lower byte of the address
	if(!err)
		if(!touch_write_byte(address))
			err = ERR_COLLISION;

	//Send upper byte of the address
	if(!err)
		if(!touch_write_byte(ADDRESS_MSB))
			err = ERR_COLLISION;

	if(!err)
	{
		for(i = 0; i < bytes; i++, data++)
		{
			*data = touch_read_byte();
		}
	}

	touch_present();

	return err;
} //read_block


//touch_read_byte
//Purpose:	To read a byte of data from a touch device
//Inputs:	NONE
//Outputs:	A byte representing the data read
BYTE touch_read_byte()
{
   BYTE i,data;
	if(overdrive)
	{
		for(i=1;i<=8;++i) {
     	output_low(TOUCH_PIN);
     	delay_us(1);
     	output_float(TOUCH_PIN);
     	delay_us(1);
     	shift_right(&data,1,input(TOUCH_PIN));
     	delay_us(13);
   }
	}
	else
	{
	   for(i=1;i<=8;++i)
		{
	     output_low(TOUCH_PIN);
	     delay_us(14);
	     output_float(TOUCH_PIN);
	     delay_us(5);
	     shift_right(&data,1,input(TOUCH_PIN));
	     delay_us(100);
	   }
	}

	return(data);
} //touch_read_byte


//touch_write_byte
//Purpose:	To write a byte of data to a touch device
//Inputs:	A byte representing the data to be written
//Outputs:	A byte signifying if a data collision occured:
//				0 = Collision, TRUE = No Collision
BYTE touch_write_byte(BYTE data)
{
   BYTE i;

	if(overdrive)
	{
		for(i=1;i<=8;++i)
		{
     		output_low(TOUCH_PIN);
     		delay_us(1);
     		if(shift_right(&data,1,0))
			{
         	output_high(TOUCH_PIN);
       		delay_us(1);
		 		if(!input_state(TOUCH_PIN))
         		return(0);
			}
	  		else
	  		{
     			output_low(TOUCH_PIN);
     			delay_us(1);
		 		if(input_state(TOUCH_PIN))
            	return(0);
     		}
			
			delay_us(5);
     		output_high(TOUCH_PIN);
     		delay_us(50);
		}
	}
	else
	{
	   for(i=1;i<=8;++i)
		{
	     output_low(TOUCH_PIN);
	     delay_us(10);
	
		  if(shift_right(&data,1,0))
		  {
	       output_high(TOUCH_PIN);
	       delay_us(10);
			 if(!input_state(TOUCH_PIN))
	       	return(0);
	     }
		  else
		  {
	       output_low(TOUCH_PIN);
	       delay_us(10);
			 if(input_state(TOUCH_PIN))
	       	return(0);
	     }
	
		  delay_us(50);
	     output_high(TOUCH_PIN);
	     delay_us(200);
	   }
	}
	
	return(TRUE);
} //touch_write_bit


//touch_present
//Purpose:	To detect if a touch device in present
//Inputs: 	NONE
//Outpus:	A byte signifying if a touch device is present:
//				FALSE = No device present, TRUE = Device present
BYTE touch_present()
{
   BOOLEAN present;

	if(overdrive)
	{
		output_low(TOUCH_PIN);
    	delay_us(85);
    	output_float(TOUCH_PIN);

    	delay_us(1);

    	if(!input(TOUCH_PIN))
      	return(FALSE);
    	delay_us(7);
    	present=!input(TOUCH_PIN);
    	delay_us(20);
	}
	else
	{
		output_low(TOUCH_PIN);
	   delay_us(500);
	   output_float(TOUCH_PIN);
	
	   delay_us(5);
	
	   if(!input(TOUCH_PIN))
		   return(FALSE);
	   delay_us(65);
	   present=!input(TOUCH_PIN);
	   delay_us(240);
	}
	
	if(present)
   	return(TRUE);
   else
   	return(FALSE);
} //touch_present


//read_bit
//Purpose:	To read a bit from a touch device
//Inputs:	NONE
//Outputs:	A bit signifying what was read in
int1 read_bit()
{
	int1 bit;

   output_low(TOUCH_PIN);
   delay_us(14);
   output_float(TOUCH_PIN);
   delay_us(5);
   bit = input(TOUCH_PIN);
   delay_us(100);

	return bit;
}// read_bit


//write_bit
//Purpose:	To write a bit to a touch device
//Inputs:	A bit signifying what to write
//Outputs:	A bit signifying if a data collision occured
//				0 = Collision, TRUE = No collision
int1 write_bit(int1 bit)
{
	output_low(TOUCH_PIN);
   delay_us(10);

	if(bit)
	{
   	output_high(TOUCH_PIN);
      delay_us(10);
		if(!input_state(TOUCH_PIN))
      	return(0);
   }
	else
	{
   	output_low(TOUCH_PIN);
      delay_us(10);
		if(input_state(TOUCH_PIN))
      	return(0);
   }

   delay_us(50);
   output_float(TOUCH_PIN);
   delay_us(50);

   return(TRUE);
} //write_bit


//get_auth - INTERNAL FUNCTION
//Purpose:	To get the TA1, TA2, and E/S bytes for authentication
//Inputs:	A pointer to an array where the authentication bytes will be stored
//Outputs:	An int representing an error code
int8 get_auth(int8* bytes)
{
	int8 err = ERR_OK, scratch[11];

	err = read_sp(scratch);

   if(!err)
   {
      *bytes = scratch[0];
      *(bytes + 1) = scratch[1];
      *(bytes + 2) = scratch[2];
   }

	return err;
} //get_auth


//verify_CRC - INTERNAL FUNCTION
//Purpose:	To check a CRC against a test CRC
//Inputs:	One int that is a CRC sent from the DS2432 chip and one int that is
//				calculated by the MCU
//Outputs:	An int1 that signifies if the crc and test matched (1, TRUE) or
//				didn't match (0, FALSE)
int1 verify_CRC(int8 crc, int8 test)
{
	int1 state;

	if(crc == test)
		state =  TRUE;
	else
		state =  FALSE;

	return state;
} //verify_CRC


//verify_CRC16 - INTERNAL FUNCTION
//Purpose:	To check a CRC against a test CRC
//Inputs:	One int that is a CRC sent from the DS2432 chip and one int that is
//				calculated by the MCU
//Outputs:	An int1 that signifies if the crc and complement of the test matched
//				(1, TRUE) or didn't match (0, FALSE)
int1 verify_CRC16(int16 crc, int16 test)
{
	int1 state;

	if(crc == (~test))
		state = TRUE;
	else
		state = FALSE;

	return state;
} //verify_CRC16


//calc_CRC - INTERNAL FUNCTION
//Purpose:	To calculate an 8-bit CRC based on a polynomial and the series
//				of data bytes
//Note: 		Polynomial used x^8 + x^5 + x^4 + 1 = 10001100
//Inputs:	A pointer to an array of the data bytes and an int saying how many
//				bytes there are in the data array
//Outputs:	An int8 which is the calculated CRC
int8 calc_CRC(int8* data, int8 bytes)
{
	int8 shift_register = 0, i, datab, bits;

	for(i = 0; i < bytes; ++i)
	{
		datab = *(data + i);

		for(bits = 0; bits < 8; ++bits)
		{
			if(bit_test((shift_register ^ datab), 0))
			{
				shift_register = shift_register >> 1;
				shift_register ^= CRC_POLY;
			}
			else
			{
				shift_register = shift_register >> 1;
			}

			datab = datab >> 1;
		}
	}
	return shift_register;
} //calc_CRC


//calc_CRC16 - INTERNAL FUNCTION
//Purpose:	To calculate a 16-bit CRC based on a polynomial and the series
//				of data bytes
//NOTE: 		Polyinomial used x^16 + x^15 + x^2 + 1 = 1010000000000001
//Inputs:	A pointer to an array of the data bytes and an int saying how many
//				bytes there are in the data array
//Outputs:	An int8 which is the calculated CRC
int16 calc_CRC16(int8* data, int8 bytes)
{
	int16 shift_register = 0;
	int8 i, datab, bits;

	for(i = 0; i < bytes; ++i)
	{
		datab = *(data + i);

		for(bits = 0; bits < 8; ++bits)
		{
			if(bit_test((shift_register ^ (int16)datab), 0))
			{
				shift_register = shift_register >> 1;
				shift_register ^= CRC_16_POLY;
			}
			else
			{
				shift_register = shift_register >> 1;
			}

			datab = datab >> 1;
		}
	}
	return shift_register;
} //calc_CRC16


//The following three functions are modified versions of functions in code from
//Dallas Semiconductor Corporation and are subject to the following Copyright
//and permission notice.


//---------------------------------------------------------------------------
// Copyright (C) 2000 Dallas Semiconductor Corporation, All Rights Reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL DALLAS SEMICONDUCTOR BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
//
// Except as contained in this notice, the name of Dallas Semiconductor
// shall not be used except as stated in the Dallas Semiconductor
// Branding Policy.
//---------------------------------------------------------------------------


//gen_MAC - INTERNAL FUNCTION
//Purpose:	To use SHA-1 calculation to generate a message authentication code (MAC)
//Inputs:	A pointer to an array containing the 128-byte input data, and a pointer
//				to an array in which the 20 byte MAC will be placed
//Outputs:	NONE
void gen_MAC(int8* data, int8* result)
{
	int32 hash[5] = {0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0};
	int8 i;
	int32 *ptr;

	int32 MTword[80];
   int32 ShftTmp;
   int32 Temp;

   for(i=0;i<16;i++)
   {
      MTword[i] = (make32(*(data + (i * 4)), *(data + (i * 4) + 1),
								*(data + (i * 4) + 2), *(data + (i * 4) + 3)));
   }

   for(;i<80;i++)
   {
      ShftTmp = (MTword[i-3] ^ MTword[i-8] ^ MTword[i-14] ^ MTword[i-16]);
      MTword[i] = (((ShftTmp << 1) & 0xFFFFFFFE) | ((ShftTmp >> 31) & 0x00000001));
   }

   for(i=0;i<80;i++)
   {
      ShftTmp = (((hash[0] << 5) & 0xFFFFFFE0) | ((hash[0] >> 27) & 0x0000001F));
      Temp = (NLF(hash[1],hash[2],hash[3],i) + hash[4] + KTN(i) + MTword[i] + ShftTmp);
      hash[4] = hash[3];
      hash[3] = hash[2];
      hash[2] = (((hash[1] << 30) & 0xC0000000) | ((hash[1] >> 2) & 0x3FFFFFFF));
      hash[1] = hash[0];
      hash[0] = Temp;
   }

	for(i = 5; i > 0; i--)
	{
		*result = make8(hash[i - 1], 0);
		result++;
		*result = make8(hash[i - 1], 1);
		result++;
		*result = make8(hash[i - 1], 2);
		result++;
		*result = make8(hash[i - 1], 3);
		result++;
	}
} //gen_MAC


//NLF - INTERNAL FUNCTION
//Purpose:	To perform a minor calculation of the SHA-1 algorithm
//Inputs:	Three int32 which are numbers for the calculation, and an int8 which
//				is used to tell which calculation to perform
//Outputs:	An int32 which is the result of the performed calculation
int32 NLF(int32 b, int32 c, int32 d, int8 num)
{
	int32 ret;

	if(num < 20)
		ret = ((b & c) | ((~b) & d));
	else if(num < 40)
		ret = (b ^ c ^ d);
	else if(num < 60)
		ret = ((b & c) | (b & d) | (c & d));
	else
		ret = (b ^ c ^ d);

	return ret;
} //NLF


//KTN - INTERNAL FUNTION
//Purpose:	To return a contant based on the input of the function, used for the
//				SHA-1 algorithm
//Inputs:	An int8 which is used to decide what number to return
//Outputs:	An int32 which is one of four constants
int32 KTN(int8 num)
{
	int32 ret;

	if(num < 20)
		ret = 0x5A827999;
	else if(num < 40)
		ret = 0x6ED9EBA1;
	else if(num < 60)
		ret = 0x8F1BBCDC;
	else
		ret = 0xCA62C1D6;

	return ret;
} //KTN
