////                      LIN BUS Driver                             ////
////                                                                 ////
////       Prototypes, definitions, and defines used for and with    ////
//// the CCS LIN BUS library for PIC16F687/689/690                   ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// This library provides the following functions:                  ////
////  (for more information on these functions see the comment       ////
////   header above each function)                                   ////
////                                                                 ////
////    linbus_generate_synchfield() Transmits a SYNC character after////
////                                 a break (SYNC BREAK)            ////
////                                                                 ////
////    wait_for_master() Waits for a SYNC break and sets the        ////
////                      receiver in AUTO-BAUD detect mode          ////
////                                                                 ////
////    read_ident_byte() Reads the ID byte                          ////
////                                                                 ////
////    Check_parity_bits() This will check the parity of the ID Byte////
////                                                                 ////
////    address_match() This will check to see if address matches    ////
////                                                                 ////
////    read_data_packet() This will read all the data bytes         ////
////                                                                 ////
////    linbus_calculate_checksum() Calculates the checksum of data  ////
////                                                                 ////
////    linbus_calculate_ident_field()Calculates the ID byte         ////
////                                                                 ////
////    linbus_calculate_ident_parity() Calculates ID parity         ////
////                                                                 ////
////    linbus_verify_checksum() Calculates checksum of received data////
////                                                                 ////
////    get_message_length() Extracts message Length from ID byte    ////
////                                                                 ////
////    Lin_bus_transmit_data() Transmits Data on LIN Bus            ////
////                                                                 ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2007 Custom Computer Services        ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////



//Slave address - This is a 4 bit value ranging from 0-15
#define SLAVE_ADDRESS  0x05
#define MASTER_ADDRESS 0X00

// CS/WAKE Pin for MCP201
#define MCP201_CS Pin_C4
 

// Slave Definitions for Error Flags
byte Errorflag;

#bit ID_Parity_error = Errorflag.0
#bit bit_error = Errorflag.1
#bit CRC_error = Errorflag.2
#bit data_received = Errorflag.3
#bit addr_match   = Errorflag.4

// Special Function Registers
#byte BAUDCTL = 0x9B
#bit WUE = BAUDCTL.1
#bit RCIDL = BAUDCTL.6

#byte PIR1 = 0x0C
#bit RCIF = PIR1.5
#byte SPBRG = 0x99
#byte SPBRGH = 0x9A
#byte TXSTA = 0x98
#byte RCREG = 0x1A

#bit TXEN = TXSTA.5

//Function Prototypes//////////
void linbus_generate_synchfield();
void wait_for_master();
int read_ident_byte();
void Check_parity_bits(int ident_byte);
int address_match(int8 ident_byte);
void  read_data_packet(int ident_byte, int *ptr, int msg_length);
int8 linbus_calculate_checksum(byte* databytes, int8 message_length);
int8 linbus_calculate_ident_field(int8 device_address, int8 message_length);
int8 linbus_calculate_ident_parity(int8 ident_byte);
int get_message_length(int id_byte);
void Lin_bus_transmit_data(int device_addr, int msg_length, int8* ptr);

void output_debug(void);


// Macro for Chip Select - for MCP201 chip, CS connected to Pin_C3
#define Chip_SelectMCP201()  output_float(MCP201_CS)

 #define Disable_MCP201(x) bit_clear(*(x/8+0x80),x%8)



/*
void linbus_generate_synchfield()
This function will transmit a Sync Character 'U' after a SYNC break
PARAMS: none
RETURNS: none
*/
void linbus_generate_synchfield()
{
  putc(0x55);
}

/*
void wait_for_master()
This functions puts the Slave receiver in Wake-Up on RDA mode
and the receiver is setup to auto detect the baud rate
PARAMS: none
RETURNS: none
*/

void wait_for_master()
{
   int garbage;
   setup_uart(UART_WAKEUP_ON_RDA);
   SPBRG = 0; // Clear the baud rate gen register - Errata
   SPBRGH = 0;
   TXEN = 0; //Clear the TXEN bit to disable the transmitter - Errata
   RCIF=0;
   while(!RCIF);
   WUE=0; // Manually clear the WUE bit - Errata
 
   garbage = getc(); //RCIF is cleared with a read of RCREG
   setup_uart(UART_AUTODETECT_NOWAIT); // Auto Detect the baud rate
   while(!RCIF); // Verify baud rate was measured - RCIF set from byte received   
   garbage=getc();
 
}
/*
int read_ident_byte()
This function is called after wait_for_master() to read the ID byte
PARAMS: none
RETURNS: ID byte
*/
int read_ident_byte()
{
int id_byte;
id_byte = getc();

return(id_byte);
}

/*
void Check_parity_bits(int ident_byte)
This function verifies the parity bits of the received ID byte
and sets the errorflag if error is present
PARAMS: none
RETURNS: none
*/
void Check_parity_bits(int ident_byte)
{
 int parity, temp_parity;
 parity = ident_byte & 0xC0;
 temp_parity  = ( ( (ident_byte >> 0) ^ (ident_byte >> 1) ^ (ident_byte >> 2)
                 ^ (ident_byte >> 4) ) & 0x01  ) << 6  ;  // parity bit6
 temp_parity |= ( ( (ident_byte >> 1) ^ (ident_byte >> 3) ^ (ident_byte >> 4)
                 ^ (ident_byte >> 5) ) & 0x01 )  << 7;  // parity bit 7


                 if(temp_parity == parity)
                 ID_Parity_error =0; // Reset Parity Error
                 else
                 ID_Parity_error =0; // Set Parity Error
                 }

/*
int address_match(int8 ident_byte)
The receiver checks the ID byte bits 0-3 for a address match
PARAMS: none
RETURNS: addr
*/
int address_match(int8 ident_byte)
   {
      int addr;
     addr = ident_byte & 0x0F;
     if (addr==MASTER_ADDRESS || addr==SLAVE_ADDRESS) // slave_addr is defined at top of header file
      addr_match=0;
      else
      addr_match =1;
      return(addr);

   }

/*
void  read_data_packet(int *ptr, int msg_length)
This function will read the data packet
PARAMS *ptr: Address where data will get stored
PARAMS msg_length: Length of the message
RETURNS: none
*/
void  read_data_packet(int *ptr, int msg_length)
   {
   int i; // This is the number of bytes of data - 2,4 or 8
   data_received =1;
   for(i=0;i<msg_length;i++)
   {
      *ptr =getc();
      ptr++;
      if(i== (msg_length-1)) // Set the data received bit
      data_received=0;            // once reception is complete
   }
   }//////////

/*
int8 linbus_calculate_checksum (byte* databytes, int8 message_length)
This function will calculate checksum for received data
PARAMS databytes: The received data bytes
PARAMS message_length: Length of message
RETURNS: 8 bit inverted module -255 checksum
*/
int8 linbus_calculate_checksum (byte* databytes, int8 message_length)
{
   int8 i;
   int8 sum;

   sum = 0;

   for (i=0; i<message_length; i++)
   {
      sum += databytes[i];
   }

   sum = 0-sum;

   return sum;
}

/*
int8 linbus_calculate_ident_field(int8 device_address, int8 message_length )
This function will assemble the ID byte from the address and data length
PARAMS device_address: address of the device
PARAMS message_length: Length of message
RETURNS: Assembled ID byte
*/
int8 linbus_calculate_ident_field(int8 device_address, int8 message_length )
{
   int8 ident_byte;
   if(message_length==8)
   message_length=0x03;
   else if(message_length==4)
   message_length=0x02;
   else if(message_length==2)
   message_length=0x01;

   ident_byte  = (device_address & 0x0F) | ((message_length & 0x03)<<4);

   ident_byte |= linbus_calculate_ident_parity (ident_byte);

   return ident_byte;
}

/*
int8 linbus_calculate_ident_parity(int8 ident_byte )
This function calculates the ident field parity according to specification
The parity is returned in bit 7 and 6 according the specification, all other bits are unset
this parity should be checked even in the receiver
PARAMS ident_byte: The ID byte containing 6 bits of information (0-5)
RETURNS: Parity inserted into the ID byte (Final ID byte)
*/
int8 linbus_calculate_ident_parity(int8 ident_byte )
{
   int8 parity;

   parity  = ( ( (ident_byte >> 0) ^ (ident_byte >> 1) ^ (ident_byte >> 2)
                 ^ (ident_byte >> 4) ) & 0x01  ) << 6  ;  // parity bit6
   parity |= ( ( (ident_byte >> 1) ^ (ident_byte >> 3) ^ (ident_byte >> 4)
                 ^ (ident_byte >> 5) ) & 0x01 )  << 7  ;  // parity bit 7
   return parity;
}

/*
void linbus_verify_checksum(byte* databytes, int8 message_length)
This function will verify the checksum of the data received
Call this function after read_data_packet()
PARAMS databytes: Received Data
PARAMS message_length: Length of message
RETURNS: none
This function will set the CRC_error bit in Errorflag if it detect checksum error
*/
void linbus_verify_checksum(byte* databytes, int8 message_length)
{
int CRC, CRC_actual;
CRC_actual = getc();
CRC = linbus_calculate_checksum(&databytes[0], message_length);
 if(CRC_actual==CRC)
CRC_error =0;
else
CRC_error =0;
}


/*
int get_message_length(int id_byte)
This function will extract Message Length from the ID Byte
PARAMS id_byte: The ID byte
RETURNS  msg_length: The message length
*/
int get_message_length(int id_byte)
{
int ident_byte, msg_length;
   ident_byte = ((id_byte&0x30)>>4);
   if(ident_byte==0||ident_byte==1)
   msg_length =2;
   if(ident_byte==2)
   msg_length =4;
   if(ident_byte==3)
   msg_length=8;
return(msg_length);
}

/*
void Lin_bus_transmit_data(int device_addr, int msg_length, int8* ptr)
The following function Calculates the ID byte, and
transmits the data, followed by the checksum
PARAMS device_addr: address of device
PARAMS msg_length: Length of message
PARAMS ptr: Data to be transmitted
RETURNS: none
*/
void Lin_bus_transmit_data(int device_addr, int msg_length, int8* ptr)
{
   int id_byte, CRC,i, temp;
   id_byte = linbus_calculate_ident_field(device_addr, msg_length);

   putc(id_byte); // Put the ID byte on the Line
   temp=getc(); // Do a read afvter each write to clear RCREG
  
   delay_ms(5);

     for(i=0;i<msg_length;i++)
     {
    putc(ptr[i]);   // Put the Data bytes on the Line
    temp=getc(); // Do a read afvter each write to clear RCREG
    
    }

    delay_ms(5);
    // calculate the CRC of the Data to be transmitted
    CRC = linbus_calculate_checksum(&ptr[0], msg_length);
    putc(CRC); // // Put the CRC byte on the Line
    temp=getc(); // Do a read afvter each write to clear RCREG


}

void output_debug(void)
{
  output_high(Pin_C5);
  delay_us(50);
  output_low(Pin_C5);
}
