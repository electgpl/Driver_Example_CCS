/////////////////////////////////////////////////////////////////////////
////                            EX_CRC.C                             ////
////                                                                 ////
////  This example program shows how to send messages between two    ////
////  PICs using CRC error checking.  Pushing a button on the        ////
////  prototype card sends a message from that PIC to any other PICs ////
////  that are connected.  The receiving PIC then sends back an ACK  ////
////  after it correctly receives the message.                       ////
////                                                                 ////
////  Two seperate PICs are needed for this example.  Compile the    ////
////  code as is and program the first PIC.  Then switch the         ////
////  MACHINE_ADDRESS and SEND_ADDRESS and program the second PIC.   ////
////                                                                 ////
////                                                                 ////
////  Packet Protocol:                                               ////
////  +------------+--------------+---------+--------+------+-----+  ////
////  | Address TO | Address FROM | Control | Length | Data | CRC |  ////
////  +------------+--------------+---------+--------+------+-----+  ////
////                                                                 ////
////  Address TO:    1 byte         Address of sending PIC           ////
////  Address FROM:  1 byte         Address of receiving PIC         ////
////  Control:       1 byte         Used for ACK and NACK            ////
////  Length:        2 bytes        Number of bytes in Data field    ////
////  Data:          0 to N bytes   Data being sent                  ////
////  CRC:           2 bytes        16 Bit CRC                       ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////     Jumper from PIC 1 pin B0 to PIC 2 pin B1                    ////
////     Jumper from PIC 1 pin B1 to PIC 2 pin B0                    ////
////     Jumper from PIC 1 GND to PIC 2 GND                          ////
////     Jumper from PIC 1 Switch to PIC 1 pin B2                    ////
////     Jumper from PIC 2 Switch to PIC 2 pin B2                    ////
////     See additional connections below.                           ////
////                                                                 ////
////  This example will work with the PCM and PCH compilers.  The    ////
////  following conditional compilation lines are used to include a  ////
////  valid device for each compiler.  Change the device, clock and  ////
////  RS232 pins for your hardware if needed.                        ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////


#if defined(__PCM__)
#include <16F877.H>
#device *=16
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_B1, rcv=PIN_B0)

#elif defined(__PCH__)
#include <18F458.H>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_B1, rcv=PIN_B0)
#endif

#include <crc.c>

// CONSTANTS
#define MACHINE_ADDRESS 0x01
#define SEND_ADDRESS    0x02
#define ACK             0x01
#define NACK            0xFF
#define BUFFER_SIZE     64

// GLOBAL VARIABLES
int packet_buffer[BUFFER_SIZE];
int ext_buffer[BUFFER_SIZE];
int ext_buffer_next_in;
int ext_buffer_next_out;

#define MESSAGE_SEND    (!input(PIN_B2))
#define DATA_IN         (ext_buffer_next_in != ext_buffer_next_out)


// EXTERNAL INTERRUPT
// function for reading in bytes from other PIC
#INT_EXT
void ext_isr()
{
   ext_buffer[ext_buffer_next_in] = getc();     // get a byte, put it in buffer

   if(++ext_buffer_next_in == BUFFER_SIZE)      // increment counter
      ext_buffer_next_in = 0;
}

// GET_BUFF_INT
// function to extract bytes from the buffer
int get_buff_int()
{
   int retval;

   while(!DATA_IN);                             // wait until data available

   retval = ext_buffer[ext_buffer_next_out];    // get the byte
   if(++ext_buffer_next_out == BUFFER_SIZE)     // increment counter
      ext_buffer_next_out = 0;

   return retval;
}

// SEND_PACKET
// function to send a packet of data to another PIC
void send_packet(int* packet_ptr, int16 packet_length)
{
   int *ptr;
   int16 CRC,i;

   ptr = packet_ptr;                            // set pointer

   CRC = generate_16bit_crc(ptr, packet_length, CRC_CCITT);
                                                // make CRC
   for(i=0; i<packet_length; i++)               // send packet
      putc(packet_ptr[i]);

   putc((int)(CRC>>8));                         // send CRC
   putc((int)(CRC));
}

// GET_PACKET
// function to get a packet from the buffer and read the data
short get_packet(int* packet_ptr)
{
   short retval;
   int16 length;
   int16 CRC;
   int16 i;

   retval = TRUE;

   packet_ptr[0] = get_buff_int();              // get the address of send to
   packet_ptr[1] = get_buff_int();              // get the address of send from

   if(packet_ptr[0] != MACHINE_ADDRESS)
      retval = FALSE;

   packet_ptr[2] = get_buff_int();              // get the control byte
   if(packet_ptr[2] == NACK)
      retval = FALSE;

   packet_ptr[3] = get_buff_int();              // get the length of the data
   packet_ptr[4] = get_buff_int();

   length = (int16)(packet_ptr[3])<<8;
   length += packet_ptr[4];

   for(i=5; i<(length+5); i++)                  // get the data
      packet_ptr[i] = get_buff_int();

   packet_ptr[length+5] = get_buff_int();       // get the CRC
   packet_ptr[length+6] = get_buff_int();

   CRC = (int16)(packet_ptr[length+5])<<8;
   CRC += packet_ptr[length+6];

   if(CRC != generate_16bit_crc(packet_ptr, length+5, CRC_CCITT))
      retval = FALSE;

   return retval;
}

// Change RS-232 IO pins
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)  // Jumpers: 8 to 11, 6 to 14

void main()   {

   ext_buffer_next_in = 0;                      // init variables
   ext_buffer_next_out = 0;

   ext_int_edge(H_TO_L);                        // init interrupts
   enable_interrupts(INT_EXT);
   enable_interrupts(GLOBAL);

   while(TRUE)                                  // loop always
   {
      if(MESSAGE_SEND)                          // if button pushed
      {
         packet_buffer[0] = SEND_ADDRESS;
         packet_buffer[1] = MACHINE_ADDRESS;
         packet_buffer[2] = 0;
         packet_buffer[3] = 0;
         packet_buffer[4] = 9;
         packet_buffer[5] = 'H';
         packet_buffer[6] = 'i';
         packet_buffer[7] = ' ';
         packet_buffer[8] = 't';
         packet_buffer[9] = 'h';
         packet_buffer[10] = 'e';
         packet_buffer[11] = 'r';
         packet_buffer[12] = 'e';
         packet_buffer[13] = '!';
         send_packet(packet_buffer, 14);        // send message
      }

      delay_ms(100);

      if(!DATA_IN)                              // if no data in
         continue;                              // loop back

      if(get_packet(packet_buffer))             // if valid packet
      {
         int16 length,i;
         // int16 CRC;

         printf("Message from unit# %U\r\n",packet_buffer[1]);

         length = ((int16)(packet_buffer[3]<<8)) + packet_buffer[4];

         if(packet_buffer[2] == ACK)
            printf("Previous message sent was received by unit.\r\n");

         if(length)                             // display message
         {
            printf("Message is:\r\n\n");
            for(i=0; i<length; ++i)
               putc(packet_buffer[i+5]);
         }
         printf("\r\n\n... Message End ...\r\n\n\n");

         if(length)
         {
            packet_buffer[0] = packet_buffer[1];//send an ACK
            packet_buffer[1] = MACHINE_ADDRESS;
            packet_buffer[2] = ACK;
            packet_buffer[3] = 0;
            packet_buffer[4] = 0;
            send_packet(packet_buffer, 5);
         }
      }
      else                                      // if not valid packet
      {
         if(packet_buffer[0] != MACHINE_ADDRESS)
            break;                              // message is not for this PIC
         else if(packet_buffer[2] == NACK)      // tried to send and failed error
            printf("Previous message sent was not received by unit.\r\n");
         else
         {                                      // tried to receive and failed error
            printf("Message received was corrupted.\r\n");

            packet_buffer[0] = packet_buffer[1];//send a NACK
            packet_buffer[1] = MACHINE_ADDRESS;
            packet_buffer[2] = NACK;
            packet_buffer[3] = 0;
            packet_buffer[4] = 0;
            send_packet(packet_buffer, 5);
         }
      }
   }
}
