///////////////////////////////////////////////////////////////////////////////
////                                                                       ////
////                                   ZRS232.H                            ////
////                                                                       ////
//// ZRS232Init() - Initializes the ZRS232 API.  Should be called before   ////
////     anything else.                                                    ////
////                                                                       ////
//// ZRS232SetMyAddress(address) - Sets our address.  (ZRS232 uses         ////
////     802.15.4 short addressing)                                        ////
////                                                                       //// 
//// ZRS232SetMyPanId(x) - Sets our PAN ID.                                ////
////                                                                       ////
//// ZSS232Connect(address) - Sets the address of the other node that we   ////
////     wish to link to.  We only transmit and receive ZRS232 with this   ////
////     node.  Also turns on the receiver, will stay on until             ////
////     ZRS232Stop().                                                     ////
////                                                                       ////
//// ZRS232Task() - Checks the 802.15.4 radio buffer for packets in the    ////
////     FIFO, if there is then lets the API get access to them using      ////
////     ZRS232kbhit() and ZRS232getc().  You must receive all data in the ////
////     buffer before next time you call ZRS232Task().  Checks the        ////
////     transmit buffer, and if it's not empty attempts to send it using  ////
////     802.15.4.  If the transmit failied, it will retry later or will   ////
////     stop retrying if too mainy failures.                              ////
////                                                                       //// 
//// ZRS232tbe() - Returns TRUE if the ZRS232 transmit buffer is empty.    ////
////                                                                       ////
//// ZRS232putc(c) - Puts the specified character into the transmit        ////
////     buffer.  Retruns TRUE if there was room in the transmit buffer    ////
////     for this character.  If it returns FALSE then you need to wait    ////
////     until it is free (after ZRS232Task() has been able to send the    ////
////     packet), or force a transmit using ZRS232Flush().                 ////
////                                                                       ////
//// ZRS232Flush() - Forces a transmit of the transmit buffer.  Will retry ////
////     several times, so this routine may take several hundred           ////
////     milliseconds to complete.  Returns TRUE if successful.            ////
////                                                                       ////
//// ZRS232kbhit() - Returns TRUE if there is data in the ZRS232 receive   ////
////     buffer to handle.  If it returns TRUE, you need to read all data  ////
////     from the receive buffer before the next ZRS232Task() time.        ////
////                                                                       ////
//// c=ZRS232getc() - Gets a character from the receive buffer.  You       ////
////     shouldn't use this until ZRS232kbhit() returns TRUE.  Once        ////
////     ZRS232kbhit() returns TRUE you need to read all data out of the   ////
////     receive buffer before the next ZRS232Task() time.                 ////
////                                                                       ////
//// ZRS232Stop() - Stops the ZRS232 API.  Also turns off the receiver.    ////
////                                                                       ////
///////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2006 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#define ZRS232_FORCE_RETRIES  20
#define ZRS232_IDLE_RETRIES   50

#define ZRS232_TX_BUFFER_SIZE (64)
#define ZRS232_RX_BUFFER_SIZE (64)

#define ZRS232_COMMAND_IDENT  (0x01)

//user api
#define ZRS232SetMyAddress(x) ZMac_SetMyShortAddress(x)
#define ZRS232SetMyPanId(x)   ZMac_SetMyPanId(x)
#define ZRS232tbe()           (g_ZRS232Tx.index==1)
#define ZRS232kbhit()         (g_ZRS232Rx.count)
#define ZRS232Stop            ZMac_StopListen

void ZRS232Init(void);
void ZSS232Connect(int16 address);
void ZRS232Task(void);
int1 ZRS232putc(char c);
int1 ZRS232Flush(void);

//non-user functions
void ZRS232TxBufferPrepare(void);
void ZRS232RxBufferPrepare(void);

ZMAC_NODE_INFO g_ZRS232RemoteNode;

struct
{
   char buffer[ZRS232_TX_BUFFER_SIZE];
   char index;
} g_ZRS232Tx, g_ZRS232TxInTransit;

struct
{
   char buffer[ZRS232_RX_BUFFER_SIZE];
   int count;
   int index;
} g_ZRS232Rx;

void ZRS232Init(void)
{
   ZMac_UseMyShortAddress();
   ZMac_StartListen();

   ZRS232TxBufferPrepare();
   ZRS232RxBufferPrepare();
}

void ZRS232Connect(int16 address)
{
   g_ZRS232RemoteNode.addressMode=ZMAC_ADDRESS_MODE_SHORT;
   g_ZRS232RemoteNode.shortAddress=address;
   g_ZRS232RemoteNode.panId=ZMac_GetMyPanId();
}

void ZRS232Task(void)
{
   static enum {ZRS232_TX_IDLE=0, ZRS232_TX_DELAY=1, ZRS232_TX_RETRY=2} tx_state=0;
   static int lastSeqn=0xFF;
   static int txRetry, txDelay;
   ZMAC_HEADER header;
   char count, ident, ret;

   if (ZMac_IsRXReady())
   {
      count=ZMac_GetHeader(&header) - 1;
      ident=ZMac_Getc();
      if (ZMac_AddressMatch(&header.sourceNode, &g_ZRS232RemoteNode) &&
            (ident == ZRS232_COMMAND_IDENT) &&
            (lastSeqn != header.seq)
         )
      {
         lastSeqn = header.seq;
         g_ZRS232Rx.index=0;
         g_ZRS232Rx.count=count;

         do
         {
            g_ZRS232Rx.buffer[g_ZRS232Rx.index++]=ZMac_Getc();
         } while(count--);

         g_ZRS232Rx.index=0;
      }
      ZMac_DiscardRX();
   }

   switch (tx_state)
   {
      case ZRS232_TX_IDLE:
         if (!ZRS232tbe())
         {
            memcpy(&g_ZRS232TxInTransit, &g_ZRS232Tx, sizeof(g_ZRS232Tx));
            ret=ZMac_PutPacket(&g_ZRS232RemoteNode, &g_ZRS232TxInTransit.buffer[0], g_ZRS232TxInTransit.index);
            ZRS232TxBufferPrepare();
            if (ret!=0)
            {
               tx_state=ZRS232_TX_DELAY;
               txDelay=0;
               txRetry=0;
            }
         }
         break;

      case ZRS232_TX_DELAY:
         delay_us(100);
         txDelay++;
         if (txDelay<200)
            break;

      case ZRS232_TX_RETRY:
         txDelay=0;
         txRetry++;
         ret=ZMac_FlushPacket(&g_ZRS232TxInTransit.buffer[0], g_ZRS232TxInTransit.index, TRUE);
         if ( (ret==0) || (txRetry >= ZRS232_IDLE_RETRIES) )
            tx_state=ZRS232_TX_IDLE;
         break;
   }
}

int1 ZRS232putc(char c)
{
   if (g_ZRS232Tx.index < ZRS232_TX_BUFFER_SIZE)
   {
      g_ZRS232Tx.buffer[g_ZRS232Tx.index++]=c;
      return(TRUE);
   }
   return(FALSE);
}


int1 ZRS232Flush(void)
{
   int8 retries=ZRS232_FORCE_RETRIES;
   int8 ret;

   ret=ZMac_PutPacket(&g_ZRS232RemoteNode, &g_ZRS232Tx.buffer[0], g_ZRS232Tx.index);

   while(!ret && retries--)
   {
      ret=ZMac_FlushPacket(&g_ZRS232RemoteNode, &g_ZRS232Tx.buffer[0], TRUE);
      delay_ms(1);
   }

   ZRS232TxBufferPrepare();

   return(ret==0);
}

void ZRS232TxBufferPrepare(void)
{
   g_ZRS232Tx.buffer[0]=ZRS232_COMMAND_IDENT;
   g_ZRS232Tx.index=1;
}

void ZRS232RxBufferPrepare(void)
{
   g_ZRS232Rx.index=0;
   g_ZRS232Rx.count=0;
}

char ZRS232getc(void)
{
   char c;

   if (g_ZRS232Rx.count)
   {
      g_ZRS232Rx.count--;
      c=g_ZRS232Rx.buffer[g_ZRS232Rx.index++];
   }

   return(c);
}
