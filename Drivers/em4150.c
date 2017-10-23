/////////////////////////////////////////////////////////////////////////
////                            EM4150.c                             ////
////                                                                 ////
////  This file contains drivers for an EM4150/4050 RF transponder   ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// int8 read_4150(int32* data, int8 address)                       ////
////     * Fills an int32 or 4 byte array with data read from address////
////     * Address can be 1 - 33.                                    ////
////     * Returns ERR_OK, ERR_LIW, ERR_NAK, ERR_PARITY              ////
////                                                                 ////
//// int8 write_4150(int32 data, int8 address)                       ////
////     * Write 32 bits of data to address                          ////
////     * Address can be from 3 to 32                               ////
////     * Returns ERR_OK, ERR_LIW, ERR_NAK                          ////
////                                                                 ////
//// int8 login_4150(int32 password)                                 ////
////     * Login with a 32 bit password                              ////
////     * Returns ERR_OK, ERR_LIW, ERR_NAK                          ////
////                                                                 ////
//// int8 setPassword_4150(int32 oldPassword, int32 newPassword)     ////
////     * Changes the password from oldPassword to newPassword      ////
////     * Returns ERR_OK, ERR_LIW, ERR_NAK, ERR_NAK_OLDPW,          ////
////       ERR_NAK_NEWPW, ERR_LIW_NEWPW                              ////
////                                                                 ////
//// int8 setBroadcastedWords_4150(int8 addr1, int8 addr2)           ////
////     * Sets which block of words is continuously broadcasted     ////
////     * addr1 is the address of the first broadcasted word        ////
////     * addr2 is the address of the last broadcasted word         ////
////     * Returns ERR_OK, ERR_LIW, ERR_NAK                          ////
////     * NOTE * A successful login must first occur                ////
////                                                                 ////
//// int8 PWprotect_4150(int1 PWbit)                                 ////
////     * TRUE turn password protection on, FALSE turns it off      ////
////     * Returns ERR_OK, ERR_LIW, ERR_NAK                          ////
////     * NOTE * Words 1 and 2 are always password protected        ////
////     * NOTE * A successful login must first occur                ////
////                                                                 ////
//// int8 readProtect_4150(int8 addr1, int8 addr2)                   ////
////     * Sets which block of words is read protected               ////
////     * addr1 is the address of the first read protected word     ////
////     * addr2 is the address of the last read protected word      ////
////     * Returns ERR_OK, ERR_LIW, ERR_NAK                          ////
////     * NOTE * A successful login must first occur                ////
////                                                                 ////
//// int8 writeProtect_4150(int8 addr1, int8 addr2)                  ////
////     * Sets which block of words is write protected              ////
////     * addr1 is the address of the first write protected word    ////
////     * addr2 is the address of the last write protected word     ////
////     * Returns ERR_OK, ERR_LIW, ERR_NAK                          ////
////     * NOTE * A successful login must first occur                ////
////                                                                 ////
//// int8 reset_4150()                                               ////
////     * A reset will do the following:                            ////
////        - Logout                                                 ////
////        - Load the changes for the broadcast, read protected,    ////
////          or write protected block                               ////
////     * Returns ERR_OK, ERR_LIW, ERR_NAK                          ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2004 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#ifndef TRANSPONDER_4150_DRIVERS
#define TRANSPONDER_4150_DRIVERS


/////////////////////////////////////////////////////////////////////////
// Return Codes
/////////////////////////////////////////////////////////////////////////
#define ERR_OK          0x00  // No errors
#define ERR_LIW         0x01  // Did not find a listen window
#define ERR_NAK         0x02  // Received a NAK
#define ERR_NAK_OLDPW   0x03  // Received a NAK sending old password (setPassword)
#define ERR_NAK_NEWPW   0x04  // Received a NAK sending new password (setPassword)
#define ERR_LIW_NEWPW   0x05  // Did not find a listen window after sending old password (setPassword)
#define ERR_PARITY      0x06  // Parity error when reading data
/////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
// Function Prototypes
/////////////////////////////////////////////////////////////////////////
int8 read_4150(int32* data, int8 address);
int8 write_4150(int32 data, int8 address);
int8 setPassword_4150(int32 oldPassword, int32 newPassword);
int8 setBroadcastedWords_4150(int8 addr1, int8 addr2);
int8 PWprotect_4150(int1 PWbit);
int1 readProtect_4150(int8 addr1, int8 addr2);
int8 writeProtect_4150(int8 addr1, int8 addr2);
int1 reset_4150();
int1 findLIW_4150();
int1 checkACK_4150();
int1 byteParity(BYTE data);
int1 validRowParity(int index, int bitPosition);
int1 validColumnParity(int index, int bitPosition);
void createPacket_4150(int32 data);
int1 valid_data_4150(int32* data);
int8 sendWord_4150(int32 data, int8 cmd);
void sendSetup_4150(int8 cmd);
/////////////////////////////////////////////////////////////////////////


// *NOTE*         If the word is read protected or password protected,
//                the data read will be all 0s.
// Purpose:       Read a word of data from the transponder at specified address
// Inputs:        1) A pointer to a 32 bit buffer or int32 to be filled
//                2) An address from 1 to 33
// Outputs:       ERR_OK, ERR_LIW, ERR_NAK, ERR_PARITY
int8 read_4150(int32* data, int8 address)
{
   int8 err;

   // Try to send the selective read command to read the
   // word at the address.
   err = sendWord_4150(make32(0, 0, address, address), 0x0A);

   // Check for errors sending the selective read command
   if(err == ERR_OK)
   {
      delay_us(200);
      findLIW_4150();
      
      // Get a word of data with parity bits,
      // and a few extra bits from the transponder
      RF_get(56, TRUE);

      // Do the parity checks
      if(!valid_data_4150(data))
      {
         // Invert the data in the buffer
         // Manchester encoded data could have been read inversely
         RFbuffer_invert();

         // Do the parity checks
         if(!valid_data_4150(data))
         {
            err = ERR_PARITY;
         }
      }
   }
   return err;
}


// Purpoes:    Write a word of data at an address
// Inputs:     1) A 32 bit integer containing the data to send
//             2) The address at which to write the data
// Outputs:    ERR_OK, ERR_LIW, ERR_NAK
// Note:       This function does not read back the data it writes
//             confirm a successful read. It only checks for an
//             ACK from the transponder. You may want to do a read
//             to confirm data was written correctly.
int8 write_4150(int32 data, int8 address)
{
   int8 err = ERR_LIW;

   sendSetup_4150(0x12);
   RFbuffer_setByte(address);                 // The address to store the word
   RFbuffer_setBit(byteParity(address));      // Set the address parity
   createPacket_4150(data);                   // Set data with row and column parity

   if(findLIW_4150())
   {
      RF_send(65, 0, 0);                      // Send the data to the transponder
      if(checkACK_4150())
         err = ERR_OK;
      else
         err = ERR_NAK;
   }

   return err;
}


// Purpose:       Send the login password to the transponder
// Inputs:        A 32 bit password
// Outputs:       ERR_OK, ERR_LIW, ERR_NAK
#define login_4150(password) sendWord_4150(password, 0x01)


// Purpose:       Set the login password
// Inputs:        1) A 32 bit integer containing the old password
//                2) A 32 bit integer containing the new password
// Outputs:       ERR_OK, ERR_LIW, ERR_NAK, ERR_NAK_OLDPW,
//                ERR_NAK_NEWPW, ERR_LIW_NEWPW
int8 setPassword_4150(int32 oldPassword, int32 newPassword)
{
   int8 err = ERR_LIW;

   sendSetup_4150(0x11);
   createPacket_4150(oldPassword);        // Set old password with row and column parity

   RFbuffer_setBit(0);                    // Send two zeros in LIW to begin writing
   RFbuffer_setBit(0);
   createPacket_4150(newPassword);        // Set new password with row and column parity

   if(findLIW_4150())                     // Find a listen window before transmitting
   {
      RF_send(56, 0, 0);                  // Send the data to the transponder
      if(checkACK_4150())
      {
         RF_findWidth(128, 10, FALSE);    // Find low pulse of LIW
         RF_send(47, 7, 0);               // Send the new password

         if(checkACK_4150())              // Check for acknowledgement
         {
            delay_ms(20);                 // Wait a moment for write sequence
            err = login_4150(newPassword);// Test the new password

            if(err == ERR_LIW)
               err = ERR_LIW_NEWPW;
         }
         else
         {
            err = ERR_NAK_NEWPW;
         }
      }
      else
      {
         err = ERR_NAK_OLDPW;
      }
   }
   return err;
}


// *NOTE*         To change the control word,
//                a successful login must first occur
// Purpose:       Enable or disable password protection
// Inputs:        1) The address of the first broadcasted word
//                2) The address of the last broadcasted word
// Outputs:       ERR_OK, ERR_LIW, ERR_NAK
int8 setBroadcastedWords_4150(int8 addr1, int8 addr2)
{
   int8  err;
   int32 controlWord;

   err = read_4150(&controlWord, 1);

   // Read the control word and check for errors
   if(err == ERR_OK)
   {
      *((int8*)&controlWord+0) = addr1;   // Set the first broadcasted word
      *((int8*)&controlWord+1) = addr2;   // Set the last broadcasted word

      err = write_4150(controlWord, 2);   // Return any errors
   }

   return err;
}


// *NOTE*         To change password protect,
//                a successful login must first occur
// Purpose:       Enable or disable password protection
// Inputs:        TRUE to enable password protection
//                FALSE to disable password protection
// Outputs:       ERR_OK, ERR_LIW, ERR_NAK, ERR_PARITY
int8 PWprotect_4150(int1 PWbit)
{
   int8  err;
   int32 controlWord;

   err = read_4150(&controlWord, 2);      // Read the old control word

   if(err == ERR_OK)                      // Check for read errors
   {
      if(PWbit)
         bit_set(controlWord, 16);        // Set the password bit if TRUE
      else
         bit_clear(controlWord, 16);      // Clear the password bit if FALSE

      err = write_4150(controlWord, 2);   // Save the change
   }

   return err;                            // Return any errors
}


// *NOTE*         To change the protection word,
//                a successful login must first occur.
//                A read protected word can only be read or
//                modified after a successful login.
// Purpose:       Enable or disable password protection
// Inputs:        1) The first read protected word
//                2) The last read protected word
// Outputs:       ERR_OK, ERR_LIW, ERR_NAK, ERR_PARITY
int1 readProtect_4150(int8 addr1, int8 addr2)
{
   int8  err;
   int32 protectWord;

   err = read_4150(&protectWord, 1);      // Read the protection word

   if(err == ERR_OK)                      // Check for read errors
   {
      *((int8*)&protectWord+0) = addr1;   // Set the first read protected word
      *((int8*)&protectWord+1) = addr2;   // Set the last read protected word

      err = write_4150(protectWord, 1);   // Save changes
   }

   return err;                            // Return any errors
}


// *NOTE*         To change the protection word,
//                a successful login must first occur.
//                A write protected word can only be changed
//                after modifying the protection word.
// Purpose:       Enable or disable password protection
// Inputs:        1) The first write protected word
//                2) The last write protected word
// Outputs:       ERR_OK, ERR_LIW, ERR_NAK, ERR_PARITY
int8 writeProtect_4150(int8 addr1, int8 addr2)
{
   int8  err;
   int32 protectWord;

   err = read_4150(&protectWord, 1);      // Read the protection word

   if(err == ERR_OK)                      // Check for read errors
   {
      *((int8*)&protectWord+2) = addr1;   // Set the first write protected word
      *((int8*)&protectWord+3) = addr2;   // Set the last write protected word

      err = write_4150(protectWord, 1);   // Save changes
   }

   return err;                            // Return any errors
}


// Purpose:       Reset the transponder perfoming a logout and updating
//                changes to either of the control registers
// Inputs:        None
// Outputs:       ERR_OK, ERR_LIW, ERR_NAK
int1 reset_4150()
{
   int8 err = ERR_LIW;

   sendSetup_4150(0x80);

   if(findLIW_4150())             // Find a listen window before transmitting
   {
      RF_send(11, 0, 0);          // Send the data to the transponder
      if(checkACK_4150())
         err = ERR_OK;
      else
         err = ERR_NAK;
   }

   return err;
}


// Purpose:    Find a listen window
// Inputs:     None
// Outputs:    TRUE if found, else FALSE
int1 findLIW_4150()
{
   int1 found = FALSE;
   int8 LIWpattern[] = {32, 128}; // The listen window for the 64 bit period
                                  // transponder is 32, 32, 128, 64, 64. A
                                  // send must begin in the first 64 RF period
                                  // pulse.

   if(RF_findPattern(LIWpattern, sizeof(LIWpattern), 10, TRUE))
   {
      found = TRUE;
   }

   return found;
}


// Purpose:       Check for the ACK response
// Inputs:        None
// Outputs:       TRUE if acknowledgement pattern found, else FALSE
int1 checkACK_4150()
{
   int1 ACK = FALSE;
   int8 ackPattern[] = {96, 32, 96, 32};

   if(RF_findPattern(ackPattern, sizeof(ackPattern), 10, TRUE))
   {
      ACK = TRUE;
   }

/*   if(RF_measureWidths(6, TRUE) == 6)
   {
      // Look for the acknowledgement pattern
      for(i=1; i<=5 && ACK; i+=2)
      {
         // Check for valid short widths
         if(RFbuffer[i] < 30 || RFbuffer[i] > 42)
         {
            ACK = FALSE;
         }
      }

      for(i=2; i<=4 && ACK; i+=2)
      {
         // Check for valid long widths
         if(RFbuffer[i] < 86 || RFbuffer[i] > 102)
         {
            ACK = FALSE;
         }
      }
   }*/

   return ACK;
}


// Purpose:    Find the parity of a byte
// Inputs:     A byte of data to check
// Outputs:    1 if number of 1s is odd. 0 if number of 1s is even
int1 byteParity(BYTE data)
{
   int1 parity = 0;
   int8 i;

   for(i=0; i<8; ++i)
   {
      parity ^= bit_test(data, 0);
      rotate_right(&data, 1);
   }
   return parity;
}


// Purpose:    Check the row parity bits
// Inputs:     1) The index into the global buffer to start at
//             2) The bit position at the index
// Outputs:    TRUE if good parity, otherwise FALSE
int1 validRowParity(int8 index, int8 bitPosition)
{
   int i, j;
   int1 parity;
   int1 pass = TRUE;

   RFbuffer_index = index;
   RFbuffer_bitIndex = bitPosition;

   // Loop through the 4 bytes
   for(i=0; i<4 && pass; ++i)
   {
      parity = 0;

      // Parity check 8 bits
      for(j=0; j<8; ++j)
      {
         parity ^= RFbuffer_getBit();
      }

      // Check the parity bit
      if(parity != RFbuffer_getBit())
      {
         pass = FALSE;
      }
   }

   // Reset the buffer indexes
   RFbuffer_index = index;
   RFbuffer_bitIndex = bitPosition;

   return pass;
}


// Purpose:    Check column parity bits
// Inputs:     1) The index into the global buffer to start at
//             2) The bit position at the index
// Outputs:    TRUE if good parity, otherwise FALSE
int1 validColumnParity(int8 index, int8 bitPosition)
{
   int8 i, j;
   int1 parity;
   int8 local_index;
   int8 local_bitPosition;

   for(i=0; i<8; ++i)
   {
      if(bitPosition+i == 8)
      {
         ++index;
      }

      local_bitPosition = (bitPosition+i)%8;
      local_index = index;
      parity = 0;

      for(j=1; j<5; ++j)
      {
         parity ^= bit_test(RFbuffer[local_index++], local_bitPosition);

         if(++local_bitPosition == 8)
         {
            local_bitPosition = 0;
            ++local_index;
         }
      }

      if(parity != bit_test(RFbuffer[local_index], local_bitPosition))
      {
         return FALSE;
      }
   }

   return TRUE;
}


// Purpose:    Sets a word in the buffer with row and column parity bits
// Inputs:     A 32 bit integer containing the data
// Outputs:    None
void createPacket_4150(int32 data)
{
   int8 columnParity = 0;
   int8 *p;

   p = &data;

   // Set a byte and its row parity bit
   for(p=((int8*)&data)+3; p>=&data; --p)
   {
      RFbuffer_setByte(*p);
      RFbuffer_setBit(byteParity(*p));
      columnParity ^= *p;
   }

   // Set the column parity
   RFbuffer_setByte(columnParity);

   // Set 0 for the stop bit
   RFbuffer_setBit(0);
}


// Purpose:       Check the data in the buffer and store it
// Inputs:        A pointer to a 32 bit integer or array to fill
// Outputs:       TRUE if parity checks passed, otherwise FALSE
//                If TRUE is returned, then there is good data in data,
//                otherwise data is untouched.
int1 valid_data_4150(int32* data)
{
   int1 err = FALSE;
   int8 bitCounter = 0;

   // Do the bit parity checks and return FALSE if either fails
   if(validRowParity(0, 0) && validColumnParity(0, 0))
   {
      // Copy the data to the return word
      for(bitCounter = 1; bitCounter < 36; ++bitCounter)
      {
         if(bitCounter%9 == 0)
            RFbuffer_getBit();  // Throw away the parity bit
         else
            shift_right(data, 4, RFbuffer_getBit());
      }
      err = TRUE;
   }

   return err;
}


// Purpose:    Send a command and a word to the transponder
// Inputs:     1) A 32 bit integer containing the data to send
//             2) The command to send before sending the data
// Outputs:    ERR_OK, ERR_LIW, ERR_NAK
int8 sendWord_4150(int32 data, int8 cmd)
{
   int8 err = ERR_LIW;

   sendSetup_4150(cmd);
   createPacket_4150(data);            // Set data with row and column parity

   if(findLIW_4150())                  // Find a listen window before transmitting
   {
      RF_send(56, 0, 0);               // Send the data to the transponder
      if(checkACK_4150())
         err = ERR_OK;
      else
         err = ERR_NAK;
   }

   return err;
}


// Purpose:    Setup routine for sending a word
// Inputs:     The command to send
// Outputs:    None
void sendSetup_4150(int8 cmd)
{
   RFbuffer_index = 0;                 // Set buffer indices to the beginning
   RFbuffer_bitIndex = 0;

   RFbuffer_setBit(0);                 // Send two zeros in LIW to begin writing
   RFbuffer_setBit(0);
   RFbuffer_setByte(cmd);              // Send the command
   RFbuffer_setBit(byteParity(cmd));   // Parity for the command
}
#endif
