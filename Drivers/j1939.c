////////////////////////////////////////////////////////////////////////////////
////                                J1939.c                                 ////
//// J1939 Library for the Data Link Layer of the SAE J1939 specification.  ////
//// Refer to the SAE J1939-21 for more information on spec.                ////
////                                                                        ////
//// This library provides the following functions:                         ////
////  (for more information on these functions see the comment              ////
////   header above each function)                                          ////
////                                                                        ////
//// J1939Init() - Initializes the CAN for J1939 communication              ////
////                                                                        ////
//// J1939ReceiveTask() - Checks for new CAN messages and places them in    ////
////                      J1939 receive buffer.                             ////
////                                                                        ////
//// J1939XmitTask() - Places J1939 into CAN transmit buffers and starts    ////
////                   transmission.                                        ////
////                                                                        ////
//// J1939Kbhit() - Checks for new messages in J1939 receive buffer.        ////
////                                                                        ////
//// J1939GetMessage() - Retrieves new message from J1939 receive buffer.   ////
////                                                                        ////
//// J1939PutMessage() - Loads message into J1939 transmit buffer.          ////
////                                                                        ////
//// J1939RequestAddress() - Request used to see if specified address has   ////
////                         been claimed.  Use address global address 255  ////
////                         to receive a list of all claimed address.      ////
////                                                                        ////
////  Requires:                                                             ////
////     J1939InitAddress - Macro to initialize the g_MyJ1939Adddress       ////
////                        variable, which is the preferred J1939 address  ////
////                        for this unit.                                  ////
////                                                                        ////
////     J1939InitName - Macro to initialize the g_J1939Name array, which   ////
////                     is J1939 Name of this unit.                        ////
////                                                                        ////
////   Driver also requires a tick timer with the following macros and      ////
////   defines.  The tick timer should be setup for a rate of 1 tick per    ////
////   millisecond or faster.                                               ////
////                                                                        ////
////     J1939GetTick() - Macro to return the current tick time.            ////
////     J1939GetTickDifference(a,b) - Macro to calculate the difference    ////
////                                   between tick time a and b.           ////
////     J1939_TICKS_PER_SECOND - Define specifying number of ticks per     ////
////                              second.                                   ////
////     J1939_TICK_TYPE - Typedef specifying variable type that the tick   ////
////                       timer uses.                                      ////
////                                                                        ////
//////////////////////////////////////////////////////////////////////////////// 
////        (C) Copyright 1996,2012 Custom Computer Services                ////
//// This source code may only be used by licensed users of the CCS         ////
//// C compiler.  This source code may only be distributed to other         ////
//// licensed users of the CCS C compiler.  No other use,                   ////
//// reproduction or distribution is permitted without written              ////
//// permission.  Derivative programs created using this software           ////
//// in object code form are not restricted in any way.                     ////
////////////////////////////////////////////////////////////////////////////////

#include <j1939.h>

// include the CAN drivers
#if (USE_INTERNAL_CAN == TRUE)
 #if defined(__PCD__)
  #if (getenv("DEVICE") == "DSPIC30F6010A") || (getenv("DEVICE") == "DSPIC30F6011A") || (getenv("DEVICE") == "DSPIC30F6012A") || (getenv("DEVICE") == "DSPIC30F6013A") || (getenv("DEVICE") == "DSPIC30F6014A") || (getenv("DEVICE") == "DSPIC30F6015") || \
      (getenv("DEVICE") == "DSPIC30F5011") || (getenv("DEVICE") == "DSPIC30F5013") || (getenv("DEVICE") == "DSPIC30F5015") || (getenv("DEVICE") == "DSPIC30F5016") || \
      (getenv("DEVICE") == "DSPIC30F4011") || (getenv("DEVICE") == "DSPIC30F4012") || (getenv("DEVICE") == "DSPIC30F4013") 
    #include <can-dsPIC30.c>  //dsPIC30
  #else
   #include <can-PIC24.c>     //PIC24 and dsPIC33
  #endif  
 #elif defined(__PCH__)
  #include <can-18F4580.c>    //PIC18
 #else
  #error Device does not have CAN/ECAN peripheral.
 #endif
#else
 #include <can-mcp251x.c>     //External CAN Controller
#endif

////////////////////////////////////////////////////////////////////////////////  API

////////////////////////////////////////////////////////////////////////////////
//J1939Init()
// Initializes the CAN for J1939 Baud Rate and sets up the CAN filters, and 
// initial J1939 Address Claim.
//  Parameters: None
//  Returns:    Nothing
////////////////////////////////////////////////////////////////////////////////
void J1939Init(void)
{
   memset(&g_J1939Flags,0,sizeof(J1939_FLAGS_STRUCT));   //clear the J1939 Flag structure
   
   J1939InitAddress();  //Initialize unit's J1939 Preferred Address
   J1939InitName();     //Initialize unit's J1939 Name
   
   rand_seed = 128;  //Initialize random generator seed number

   can_init();    //Initialize the CAN, sets up Baud Rate and puts it in normal mode
   
   #if (USE_INTERNAL_CAN == TRUE)
    #if defined(__PCD__)  //dsPIC30
     #if (getenv("DEVICE") == "DSPIC30F6010A") || (getenv("DEVICE") == "DSPIC30F6011A") || (getenv("DEVICE") == "DSPIC30F6012A") || (getenv("DEVICE") == "DSPIC30F6013A") || (getenv("DEVICE") == "DSPIC30F6014A") || (getenv("DEVICE") == "DSPIC30F6015") || \
         (getenv("DEVICE") == "DSPIC30F5011") || (getenv("DEVICE") == "DSPIC30F5013") || (getenv("DEVICE") == "DSPIC30F5015") || (getenv("DEVICE") == "DSPIC30F5016") || \
         (getenv("DEVICE") == "DSPIC30F4011") || (getenv("DEVICE") == "DSPIC30F4012") || (getenv("DEVICE") == "DSPIC30F4013")
      
      can_set_mode(CAN_OP_CONFIG);  //put CAN in Config mode
      
      can_set_id(&C1RXM0, 0x0000FF00, CAN_MASK_ACCEPT_ALL);    //Set Mask 0 to look at Destination Address of PDU
      can_set_id(&C1RXM1, 0x00F00000, CAN_MASK_ACCEPT_ALL);    //Set Mask 1 to look at upper nibble of PDU Format
      
      can_set_id(&C1RXF0, 0x0000FF00, CAN_USE_EXTENDED_ID);    //Filter 0 set to look for messages to the Global Address 255
      can_set_id(&C1RXF1, 0x0000FF00, CAN_USE_EXTENDED_ID);    //Filter 1 set to look for messages to the Global Address 255 will change to it's address once it gets one
      can_set_id(&C1RXF2, 0x00F00000, CAN_USE_EXTENDED_ID);    //Filter 2 set to look for Broadcast messages PDU 240 to 255
      can_set_id(&C1RXF3, 0x00F00000, CAN_USE_EXTENDED_ID);    //Filter 3 set to look for Broadcast messages PDU 240 to 255
      can_set_id(&C1RXF4, 0x00F00000, CAN_USE_EXTENDED_ID);    //Filter 4 set to look for Broadcast messages PDU 240 to 255
      can_set_id(&C1RXF5, 0x00F00000, CAN_USE_EXTENDED_ID);    //Filter 5 set to look for Broadcast messages PDU 240 to 255
      
      can_set_mode(CAN_OP_NORMAL);  //put CAN in Normal mode
     #else //PIC24 and dsPIC33
      //Initialize the CAN filters, each function puts CAN in config mode
      //makes changes and puts back in normal mode
      can_set_id(&C1RXM0, 0x0000FF00, CAN_USE_EXTENDED_ID);    //Set Mask 0 to look at Destination Address of PDU
      can_set_id(&C1RXM1, 0x00F00000, CAN_USE_EXTENDED_ID);    //Set Mask 1 to look at upper nibble of PDU Format
      
      can_set_id(&C1RXF0, 0x0000FF00, CAN_USE_EXTENDED_ID);    //Filter 0 set to look for messages to the Global Address 255
      can_set_id(&C1RXF1, 0x0000FF00, CAN_USE_EXTENDED_ID);    //Filter 1 set to look for messages to the Global Address 255 will change to unit's address once it gets one
      can_set_id(&C1RXF2, 0x00F00000, CAN_USE_EXTENDED_ID);    //Filter 2 set to look for Broadcast messages PDU 240 to 255
      
      can_associate_filter_to_mask(ACCEPTANCE_MASK_0, F0BP);   //Associate Mask 0 with filter 0
      can_associate_filter_to_mask(ACCEPTANCE_MASK_0, F1BP);   //Associate Mask 0 with filter 1
      can_associate_filter_to_mask(ACCEPTANCE_MASK_1, F2BP);   //Associate Mask 1 with filter 2
      
      can_associate_filter_to_buffer(AFIFO, F0BP);    //Associate Filter 0 to FIFO buffer
      can_associate_filter_to_buffer(AFIFO, F1BP);    //Associate Filter 1 to FIFO buffer
      can_associate_filter_to_buffer(AFIFO, F2BP);    //Associate Filter 2 to FIFO buffer
      
      can_enable_filter(FLTEN0);    //Enable Filter 0
      can_enable_filter(FLTEN1);    //Enable Filter 1
      can_enable_filter(FLTEN2);    //Enable Filter 2
      
      can_enable_b_transfer(TRB0);  //make buffer 0 a transmit buffer
      can_enable_b_transfer(TRB1);  //make buffer 1 a transmit buffer
     #endif
    #else //PIC18
      can_set_mode(CAN_OP_CONFIG);  //put CAN in Config mode
    
      can_set_id(RX0MASK, 0x0000FF00, CAN_USE_EXTENDED_ID);       //Set Mask 0 to look at Destination Address of PDU
      can_set_id(RX1MASK, 0x00F00000, CAN_USE_EXTENDED_ID);       //Set Mask 1 to look at upper nibble of PDU Format
      
      can_set_id(RXFILTER0, 0x0000FF00, CAN_USE_EXTENDED_ID);     //Filter 0 set to look for messages to the Global Address 255
      can_set_id(RXFILTER1, 0x0000FF00, CAN_USE_EXTENDED_ID);     //Filter 1 set to look for messages to the Global Address 255 will change to unit's address once it gets one
      can_set_id(RXFILTER2, 0x00F00000, CAN_USE_EXTENDED_ID);     //Filter 2 set to look for Broadcast messages PDU 240 to 255
      can_set_id(RXFILTER3, 0x00F00000, CAN_USE_EXTENDED_ID);     //Filter 3 set to look for Broadcast messages PDU 240 to 255
      can_set_id(RXFILTER4, 0x00F00000, CAN_USE_EXTENDED_ID);     //Filter 4 set to look for Broadcast messages PDU 240 to 255
      can_set_id(RXFILTER5, 0x00F00000, CAN_USE_EXTENDED_ID);     //Filter 5 set to look for Broadcast messages PDU 240 to 255
      can_set_id(RXFILTER6, 0x00F00000, CAN_USE_EXTENDED_ID);     //Filter 6 set to look for Broadcast messages PDU 240 to 255
      can_set_id(RXFILTER7, 0x00F00000, CAN_USE_EXTENDED_ID);     //Filter 7 set to look for Broadcast messages PDU 240 to 255
      can_set_id(RXFILTER8, 0x00F00000, CAN_USE_EXTENDED_ID);     //Filter 8 set to look for Broadcast messages PDU 240 to 255
      can_set_id(RXFILTER9, 0x00F00000, CAN_USE_EXTENDED_ID);     //Filter 9 set to look for Broadcast messages PDU 240 to 255
      can_set_id(RXFILTER10, 0x00F00000, CAN_USE_EXTENDED_ID);    //Filter 10 set to look for Broadcast messages PDU 240 to 255
      can_set_id(RXFILTER11, 0x00F00000, CAN_USE_EXTENDED_ID);    //Filter 11 set to look for Broadcast messages PDU 240 to 255
      can_set_id(RXFILTER12, 0x00F00000, CAN_USE_EXTENDED_ID);    //Filter 12 set to look for Broadcast messages PDU 240 to 255
      can_set_id(RXFILTER13, 0x00F00000, CAN_USE_EXTENDED_ID);    //Filter 13 set to look for Broadcast messages PDU 240 to 255
      can_set_id(RXFILTER14, 0x00F00000, CAN_USE_EXTENDED_ID);    //Filter 14 set to look for Broadcast messages PDU 240 to 255
      can_set_id(RXFILTER15, 0x00F00000, CAN_USE_EXTENDED_ID);    //Filter 15 set to look for Broadcast messages PDU 240 to 255
      
      can_set_mode(CAN_OP_NORMAL);  //put CAN in Normal mode
    #endif
   #else //External CAN Controller
      can_set_mode(CAN_OP_CONFIG);     //put CAN in Config mode
      
      can_set_id(RX0MASK, 0x0000FF00, CAN_USE_EXTENDED_ID);       //Set Mask 0 to look at Destination Address of PDU
      can_set_id(RX1MASK, 0x00F00000, CAN_USE_EXTENDED_ID);       //Set Mask 1 to look at upper nibble of PDU Format
      
      can_set_id(RX0FILTER0, 0x0000FF00, CAN_USE_EXTENDED_ID);    //Filter 0 set to look for messages to the Global Address 255
      can_set_id(RX0FILTER1, 0x0000FF00, CAN_USE_EXTENDED_ID);    //Filter 1 set to look for messages to the Global Address 255 will change to unit's address once it gets one
      can_set_id(RX1FILTER2, 0x00F00000, CAN_USE_EXTENDED_ID);    //Filter 2 set to look for Broadcast messages PDU 240 to 255
      can_set_id(RX1FILTER3, 0x00F00000, CAN_USE_EXTENDED_ID);    //Filter 3 set to look for Broadcast messages PDU 240 to 255
      can_set_id(RX1FILTER4, 0x00F00000, CAN_USE_EXTENDED_ID);    //Filter 4 set to look for Broadcast messages PDU 240 to 255
      can_set_id(RX1FILTER5, 0x00F00000, CAN_USE_EXTENDED_ID);    //Filter 5 set to look for Broadcast messages PDU 240 to 255
      
      can_set_mode(CAN_OP_NORMAL);     //put CAN in Normal mode
   #endif
   
   J1939ClaimAddress();  //Attempt to Claim unit's address
}

////////////////////////////////////////////////////////////////////////////////
//J1939ReceiveTask()
// Checks for new CAN messages and loads into J1939 Receive Buffer
//  Parameters: None
//  Returns:    Nothing
//
// Warning - This function will continue to CAN buffers until all messages are
//           retrieved from CAN buffers, if J1939 Receive buffer isn't large
//           enough it will throw away any messages that will overflow the 
//           buffer.  This should only be a problem for PIC24 and dsPIC33 chips
//           which can have up to 32 CAN receive buffers, but should be OK as
//           long as J1939_RECEIVE_BUFFERS is set high enough and
//           J1939GetMessage() is called frequently to clear data.
////////////////////////////////////////////////////////////////////////////////
void J1939ReceiveTask(void)
{
   J1939_PDU_STRUCT ReceivedPDU;
   uint8_t Data[8];
   uint8_t length;
   struct rx_stat Status;
   
   rand_seed++;
   
   while(can_kbhit())
   {
      can_getd(ReceivedPDU,Data,length,Status);
      
      if(g_J1939Flags.ReceiveBufferCount < J1939_RECEIVE_BUFFERS)
      {
         switch(ReceivedPDU.PDUFormat)
         {
            case J1939_PF_ADDR_CLAIMED:
               J1939HandleAddressClaim(ReceivedPDU,Data);
               
               if((ReceivedPDU.SourceAddress != g_MyJ1939Address) && (ReceivedPDU.SourceAddress != J1939_NULL_ADDRESS))
               {
                  J1939LoadReceiveBuffer(ReceivedPDU,Data,length);  //so you can keep a list of J1939Names to J1939Addresses, if desired
               }
               break;
            case J1939_PF_REQUEST:
               if((Data[0] == 0x00) && (Data[1] == 0xEE) && (Data[2] == 0x00))
               {
                  J1939HandleAddressRequest(ReceivedPDU);
                  break;
               }
            default:
               J1939LoadReceiveBuffer(ReceivedPDU,Data,length);
               break;
         }
      }
   }
   
   if((g_J1939Flags.AddressClaimed == FALSE) && (g_J1939Flags.AddressClaimSent == TRUE) && (g_J1939Flags.AddressCannotClaim == FALSE))
   {
      g_J1939CurrentClaimTick = J1939GetTick();
      
      if(J1939GetTickDifference(g_J1939CurrentClaimTick, g_J1939PreviousClaimTick) >= (J1939_TICK_TYPE)J1939_TICKS_PER_SECOND/4)
      {
         g_J1939Flags.AddressClaimed = TRUE;
         J1939SetCANFilter(g_MyJ1939Address);      //unit claimed address so setup filter to start looking for 
                                                   //J1939 Messages sent to unit's address 
      }
   }
}

////////////////////////////////////////////////////////////////////////////////
//J1939XmitTask()
// Checks for message in Xmit Buffer and loads into CAN buffers to transmit.
//  Parameters: None
//  Returns:    Nothing
////////////////////////////////////////////////////////////////////////////////
void J1939XmitTask(void)
{
   J1939_TICK_TYPE CurrentTick;

   while((g_J1939Flags.XmitBufferCount > 0) && can_tbe())
   {
      if((g_J1939Flags.AddressClaimed == TRUE) || (g_J1939XmitBuffer[g_J1939XmitNextOut].PDU.PDUFormat == J1939_PF_ADDR_CLAIMED) || 
         ((g_J1939XmitBuffer[g_J1939XmitNextOut].PDU.PDUFormat == J1939_PF_REQUEST) && (g_J1939XmitBuffer[g_J1939XmitNextOut].Data[0] == 0x00) &&
          (g_J1939XmitBuffer[g_J1939XmitNextOut].Data[1] == 0xEE) && (g_J1939XmitBuffer[g_J1939XmitNextOut].Data[2] == 0x00)))
      {
         if((g_J1939XmitBuffer[g_J1939XmitNextOut].PDU.PDUFormat == J1939_PF_ADDR_CLAIMED) && (g_J1939XmitBuffer[g_J1939XmitNextOut].PDU.DestinationAddress == J1939_NULL_ADDRESS))
         {
            CurrentTick = J1939GetTick();
            
            if(J1939GetTickDifference(CurrentTick, g_J1939PreviousCannotClaimTick) <= g_J1939CannotClaimDelay)
               break;
         }
               
         can_putd(g_J1939XmitBuffer[g_J1939XmitNextOut].PDU,g_J1939XmitBuffer[g_J1939XmitNextOut].Data,g_J1939XmitBuffer[g_J1939XmitNextOut].Length,3,TRUE,FALSE);
         
         if((g_J1939Flags.AddressClaimed == FALSE) && (g_J1939Flags.AddressNewClaim == TRUE) && (g_J1939XmitBuffer[g_J1939XmitNextOut].PDU.PDUFormat == J1939_PF_ADDR_CLAIMED) && (g_J1939XmitBuffer[g_J1939XmitNextOut].PDU.DestinationAddress != J1939_NULL_ADDRESS))
         {
            if((bit_test(g_J1939Name[7],7) == FALSE) && ((g_J1939XmitBuffer[g_J1939XmitNextOut].PDU.DestinationAddress <= 128) || 
               ((g_J1939XmitBuffer[g_J1939XmitNextOut].PDU.DestinationAddress >= 248) && (g_J1939XmitBuffer[g_J1939XmitNextOut].PDU.DestinationAddress <=253))))
            {
               g_J1939Flags.AddressClaimed = TRUE;
               g_J1939Flags.AddressClaimSent = TRUE;
               g_J1939Flags.AddressNewClaim = FALSE;
               
               J1939SetCANFilter(g_MyJ1939Address);   //unit claimed address so setup filter to start looking for 
                                                      //J1939 Messages sent to unit's address
            }
            else
            {
               g_J1939PreviousClaimTick = J1939GetTick();
               g_J1939Flags.AddressClaimSent = TRUE;
               g_J1939Flags.AddressNewClaim = FALSE;
            }
         }            
      }
               
      if(++g_J1939XmitNextOut >= J1939_TRANSMIT_BUFFERS)
         g_J1939XmitNextOut = 0;
         
       g_J1939Flags.XmitBufferCount--;
   }
}

////////////////////////////////////////////////////////////////////////////////
//J1939Kbhit()
// Checks for a new message in receive buffer
//  Parameters: None
//  Returns: True - if a new message is in buffer
//           False - if no new message was in buffer
////////////////////////////////////////////////////////////////////////////////
int1 J1939Kbhit(void)
{
   if(g_J1939Flags.ReceiveBufferCount > 0)
      return(TRUE);
   else
      return(FALSE);
}

////////////////////////////////////////////////////////////////////////////////
//J1939GetMessage()
// Retrieves a message from buffer
//  Parameters: PDU - PDU structure to return message's PDU to
//              Data - pointer to return data to
//              Length - variable to return message length to
//  Returns:    True - if new message was retrieved
//              False - if there was no new message to retrieve
////////////////////////////////////////////////////////////////////////////////
int1 J1939GetMessage(J1939_PDU_STRUCT &PDU, uint8_t *Data, uint8_t &Length)
{
   uint8_t i;

   if(g_J1939Flags.ReceiveBufferCount > 0)
   {
      Length = g_J1939ReceiveBuffer[g_J1939ReceiveNextOut].Length;
      memcpy(&PDU,&g_J1939ReceiveBuffer[g_J1939ReceiveNextOut].PDU,sizeof(J1939_PDU_STRUCT));
      
      for(i=0;i<Length;i++)
         Data[i] = g_J1939ReceiveBuffer[g_J1939ReceiveNextOut].Data[i];
         
      if(++g_J1939ReceiveNextOut >= J1939_RECEIVE_BUFFERS)
         g_J1939ReceiveNextOut = 0;
         
      g_J1939Flags.ReceiveBufferCount--;
      
      return(TRUE);
   }
   else
      return(FALSE);
}

////////////////////////////////////////////////////////////////////////////////
//J1939PutMessage()
// Load message into transmit buffer
//  Parameters: PDU - PDU to send with message
//              Data - pointer to data to send
//              Bytes - number of bytes to send
//  Returns:    True - if message was successfully loaded into an empty xmit buffer
//              False - if xmit buffer was full
////////////////////////////////////////////////////////////////////////////////
int1 J1939PutMessage(J1939_PDU_STRUCT PDU, uint8_t *Data, uint8_t Bytes)
{
   uint8_t i;

   if(g_J1939Flags.XmitBufferCount < J1939_TRANSMIT_BUFFERS)
   {
      memcpy(&g_J1939XmitBuffer[g_J1939XmitNextIn].PDU,&PDU,sizeof(J1939_PDU_STRUCT));
      g_J1939XmitBuffer[g_J1939XmitNextIn].Length = Bytes;
      for(i=0;i<Bytes;i++)
        g_J1939XmitBuffer[g_J1939XmitNextIn].Data[i] = Data[i];
      
      if(++g_J1939XmitNextIn >= J1939_TRANSMIT_BUFFERS)
         g_J1939XmitNextIn = 0;
         
      g_J1939Flags.XmitBufferCount++;
      
      return(TRUE);
   }
   else
      return(FALSE);
}

void J1939RequestAddress(uint8_t address)
{
   J1939_PDU_STRUCT PDU;
   uint8_t data[3];
   
   if(g_J1939Flags.AddressClaimed == FALSE)
      PDU.SourceAddress = J1939_NULL_ADDRESS;
   else
      PDU.SourceAddress = g_MyJ1939Address;
      
   PDU.DestinationAddress = address;
   PDU.PDUFormat = J1939_PF_REQUEST;
   PDU.DataPage = 0;
   PDU.ExtendedDataPage = 0;
   PDU.Priority = J1939_REQUEST_PRIORITY;
   
   data[0] = 0;
   data[1] = 0xEE;
   data[2] = 0;
   
   J1939PutMessage(PDU,data,3);
}

////////////////////////////////////////////////////////////////////////////////  Internal Functions

////////////////////////////////////////////////////////////////////////////////
//J1939ClaimAddress()
// Sends an Address Claimed message to claim address in g_MyJ1939Address.
//  Parameters: None
//  Returns:    Nothing
////////////////////////////////////////////////////////////////////////////////
void J1939ClaimAddress(void)
{
   J1939_PDU_STRUCT RequestPDU;

   RequestPDU.SourceAddress = g_MyJ1939Address;
   RequestPDU.DestinationAddress = J1939_GLOBAL_ADDRESS;
   RequestPDU.PDUFormat = J1939_PF_ADDR_CLAIMED;
   RequestPDU.DataPage = 0;
   RequestPDU.ExtendedDataPage = 0;
   RequestPDU.Priority = J1939_REQUEST_PRIORITY;
   
   g_J1939Flags.AddressNewClaim = TRUE;
   
   J1939PutMessage(RequestPDU,g_J1939Name,8);
}

////////////////////////////////////////////////////////////////////////////////
//J1939CompareName()
// Compares our name with received name to determine which has priority
//  Parameters: data - pointer to received name
//  Returns:    True - if our name is high priority
//              False - if our name is lower priority
////////////////////////////////////////////////////////////////////////////////
int1 J1939CompareName(uint8_t *data)
{
   uint8_t i;
   
   for(i=0;i<8;i++)
   {
      if(g_J1939Name[i] > data[i])
         return(FALSE);
   }
   
   return(TRUE);
}

////////////////////////////////////////////////////////////////////////////////
//J1939HandleAddressRequest()
// Generates response to J1939 Address Requests
//  Parameters: None
//  Returns:    Nothing
////////////////////////////////////////////////////////////////////////////////
void J1939HandleAddressRequest(J1939_PDU_STRUCT PDU)
{
   J1939_PDU_STRUCT RequestPDU;
   
   if(g_J1939Flags.AddressClaimed != TRUE)
      RequestPDU.SourceAddress = J1939_NULL_ADDRESS;
   else
      RequestPDU.SourceAddress = g_MyJ1939Address;
   
   RequestPDU.DestinationAddress = J1939_GLOBAL_ADDRESS;
   RequestPDU.PDUFormat = J1939_PF_ADDR_CLAIMED;            //value is the same for both Address Claimed and Cannot Claim Address
   RequestPDU.DataPage = 0;
   RequestPDU.ExtendedDataPage = 0;
   RequestPDU.Priority = J1939_REQUEST_PRIORITY;
   
   if((g_J1939Flags.AddressClaimed == TRUE) || (g_J1939Flags.AddressClaimSent == TRUE))  //don't respond to request if an address claim hasn't been sent
      J1939PutMessage(RequestPDU,g_J1939Name,8);
}

////////////////////////////////////////////////////////////////////////////////
//J1939LoadReceiveBuffer()
// Loads g_J1939ReceiveBuffer with passed data and updates global indexes.
//  Parameters: ReceivedPDU - the PDU of the received CAN message
//              Data - pointer to the received CAN data
//              length - number of bytes received in CAN message
//  Returns:    Nothing
////////////////////////////////////////////////////////////////////////////////
void J1939LoadReceiveBuffer(J1939_PDU_STRUCT ReceivedPDU,uint8_t *Data,uint8_t length)
{
   uint8_t i;
   
   memcpy(&g_J1939ReceiveBuffer[g_J1939ReceiveNextIn].PDU,&ReceivedPDU,sizeof(J1939_PDU_STRUCT));
   g_J1939ReceiveBuffer[g_J1939ReceiveNextIn].Length = length;
   for(i=0;i<length;i++)
      g_J1939ReceiveBuffer[g_J1939ReceiveNextIn].Data[i] = Data[i];
   
   if(++g_J1939ReceiveNextIn >= J1939_RECEIVE_BUFFERS)
      g_J1939ReceiveNextIn = 0;
      
   g_J1939Flags.ReceiveBufferCount++;
}

////////////////////////////////////////////////////////////////////////////////
//J1939HandleAddressClaim()
// Responses to a J1939 Address Claim message.  Compares unit's Address and Name
// to the Source Address and Name of the received J1939 Address Claim message,
// and either response with Address Claimed, Cannot Claim Address or if unit is
// Arbitrary Address Capable and if received Name is higher priority then unit's
// name it sends a new Address Request with a randomly generated address from
// 128 to 247.
//  Parameters: ReceivedPDU - the PDU of received Address Claim message
//              Name - pointer to the J1939 Name of Address Claimer
//  Returns:    Nothing
////////////////////////////////////////////////////////////////////////////////
void J1939HandleAddressClaim(J1939_PDU_STRUCT ReceivedPDU, uint8_t *Name)
{
   J1939_PDU_STRUCT RequestPDU;
   
   if((ReceivedPDU.SourceAddress != J1939_NULL_ADDRESS) && (g_J1939Flags.AddressClaimSent))
   {
      RequestPDU.DestinationAddress = J1939_GLOBAL_ADDRESS;
      RequestPDU.PDUFormat = J1939_PF_ADDR_CLAIMED;            //value is the same for both Address Claimed and Cannot Claim Address
      RequestPDU.DataPage = 0;
      RequestPDU.ExtendedDataPage = 0;
      RequestPDU.Priority = J1939_REQUEST_PRIORITY;

      if((ReceivedPDU.SourceAddress == g_MyJ1939Address) && (g_J1939Flags.AddressCannotClaim == FALSE))
      {
         if(J1939CompareName(Name))
         {
            RequestPDU.SourceAddress = g_MyJ1939Address;
         }
         else
         {
            if(g_J1939Flags.AddressClaimed)
               J1939SetCANFilter(J1939_GLOBAL_ADDRESS);  //Only do this if unit already claimed address,
                                                         //because this switches CAN to CONFIG mode.
            //Clear Address Claim Flags
            g_J1939Flags.AddressClaimed = FALSE;
            
            //Clear Transmit Buffer
            g_J1939XmitNextOut = 0;
            g_J1939XmitNextIn = 0;
            g_J1939Flags.XmitBufferCount = 0;
            
            if(bit_test(g_J1939Name[7],7) == FALSE)   //If not Arbitrary Address Capable send Cannot Claim Address
            {
               RequestPDU.SourceAddress = J1939_NULL_ADDRESS;
               g_J1939Flags.AddressCannotClaim = TRUE;
            }
            else  //If Arbitrary Address Capable Generate Random address from 128 to 247 and request
            {
               g_MyJ1939Address = (((uint32_t)xor8() * 46875) / 100000) + 128;
               RequestPDU.SourceAddress = g_MyJ1939Address;
               g_J1939Flags.AddressNewClaim = TRUE;
            }
         }
      }
      else
         RequestPDU.SourceAddress = J1939_NULL_ADDRESS;
      
      if(RequestPDU.SourceAddress == J1939_NULL_ADDRESS)
      {
         g_J1939PreviousCannotClaimTick = J1939GetTick();
         g_J1939CannotClaimDelay = ((uint32_t)xor8() * 53125) / 100000;    //Generate Random delay from 0 to 135ms
      }
         
      J1939PutMessage(RequestPDU,g_J1939Name,8);
   }
}
      
////////////////////////////////////////////////////////////////////////////////
//J1939SetCANFilter()
// Sets filter 1 of CAN module to receive unit's address after unit it has
// successfully claimed an address.
//  Parameters: address - address to set filter to
//  Returns:    Nothing
////////////////////////////////////////////////////////////////////////////////
void J1939SetCANFilter(uint8_t address)
{
   can_set_mode(CAN_OP_CONFIG);  //put CAN in Config mode

   #if (USE_INTERNAL_CAN == TRUE)
    #if defined(__PCD__)   //PIC24, dsPIC33 and dsPIC30
      can_set_id(&C1RXF1, (uint32_t)address << 8, CAN_USE_EXTENDED_ID);       //Set Filter 1
    #else
      can_set_id(RXFILTER1, (uint32_t)address << 8, CAN_USE_EXTENDED_ID);     //Set Filter 1
    #endif
   #else
      can_set_id(RX0FILTER1, (uint32_t)address << 8, CAN_USE_EXTENDED_ID);    //Set Filter 1
   #endif
   
   can_set_mode(CAN_OP_NORMAL);  //put CAN in Normal mode
}

////////////////////////////////////////////////////////////////////////////////
//xor8()
// Generates a pseudo-random 8-bit number.  rand_seed is used as a seed
//   for the algorithm.
//  Parameters: None
//  Returns:    uint8_t - Pseudo-random value
////////////////////////////////////////////////////////////////////////////////
uint8_t xor8(void)
{
   static uint8_t y = 69;
   static uint8_t z = 29;
   static uint8_t w = 23;
   uint8_t t;

   t = rand_seed ^ (rand_seed << 3);
   rand_seed = y; y = z; z = w;
   w = w ^ (w >> 5) ^ (t ^ (t >> 2));
   return (w);
}
     
