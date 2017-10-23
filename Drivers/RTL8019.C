//////////////////////////////////////////////////////////////////////////////
////                                                                      ////
////                              RTL8019.C                               ////
////                                                                      ////
//// Driver for NE2000 network interface card (NIC) chipsets,             ////
//// specifically the Realtek RTL8019.                                    ////
////                                                                      ////
//// Default MAC address and PIC pin and port specifications are          ////
//// defined at the top of this driver and should be changed for          ////
//// your application.                                                    ////
////                                                                      ////
//// The most common use of this NIC will be to send EtherNet packets.    ////
//// EtherNet encapsulates all other packets (such as TCP, IP, UDP, ARP). ////
//// An EtherNet packet contains a simple header followed by the payload. ////
//// The RTL8019 will automatically calculate the EtherNet checksum for   ////
//// you (but you may have to do your own checksums for other packets     ////
//// encapsulated in EtherNet such as TCP/IP).                            ////
//// See the structure ETHERNET_HEADER that is typedef in this file.      ////
////                                                                      ////
////  ___ FUNCTIONS ___                                                   ////
////                                                                      ////
////  nic_init() - Must be called before any other NIC function           ////
////                                                                      ////
////      PACKET RECEPTION :                                              ////
////                                                                      ////
////  int16 nic_begin_packet_rx() - Must be called before you can receive ////
////                          packets from the receive buffer.            ////
////                          Returns size of received packet if exist.   ////
////                                                                      ////
////  nic_packet_get_data(*buffer,len) - Receives len bytes from NIC's    ////
////                          receive buffer and stores into *buffer.     ////
////                          Can be called several times to read a       ////
////                          large packet in several chunks.             ////
////                          nic_begin_packet_rx() MUST be called first! ////
////                                                                      ////
////  nic_end_packet_rx() - Throws away rest of packet, returns NIC to    ////
////                        normal operation.  MUST be called last.       ////
////                                                                      ////
////      PACKET TRANSMISSION:                                            ////
////                                                                      ////
////                                                                      ////
////  nic_begin_packet_tx(len) - Configures NIC to receive data to send   ////
////                             a packet of len bytes.  MUST be called   ////
////                             first!                                   ////
////                                                                      ////
////  nic_packet_put_data(*buffer,len) - Puts len bytes into NIC's        ////
////                        transmit buffer.  Can be called several times ////
////                        to add chunks of data into transmit buffer.   ////
////                                                                      ////
////  nic_packet_end_tx() - Configures NIC to transmit packet with the    ////
////                        data stored in the transmit buffer. MUST be   ////
////                        called last!                                  ////
////                                                                      ////
////  nic_putd(*buffer,len) - This is a very simple function that uses    ////
////                     the 3 other packet transmission functions above  ////
////                     to quickly send out a packet.  This function     ////
////                     requires you to have the entire packet formatted ////
////                     inside an array pointed to by buffer.            ////
////                                                                      ////
//////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services              ////
//// This source code may only be used by licensed users of the CCS C     ////
//// compiler.  This source code may only be distributed to other         ////
//// licensed users of the CCS C compiler.  No other use, reproduction    ////
//// or distribution is permitted without written permission.             ////
//// Derivative programs created using this software in object code       ////
//// form are not restricted in any way.                                  ////
//////////////////////////////////////////////////////////////////////////////

#ifndef MAC_ADDR_0
   #define MAC_ADDR_0 1
   #define MAC_ADDR_1 2
   #define MAC_ADDR_2 3
   #define MAC_ADDR_3 4
   #define MAC_ADDR_4 5
   #define MAC_ADDR_5 6
#endif

#ifndef RTL8019_PIN_IOR
   #define  RTL8019_PIN_IOR          PIN_E0
   #define  RTL8019_PIN_IOW          PIN_E1
   #define  RTL8019_PIN_RESET        PIN_E2

   #define  rtl8019_data_out(x)      output_d(x)
   #define  rtl8019_data_in          input_d
   #define  rtl8019_address_out(x)   output_b(x)
#endif

/// *** END USER CONFIG ***

typedef struct _MAC_ADDR
{
    int8 val[6];
} MAC_ADDR;

typedef struct _ETHERNET_HEADER
{
    MAC_ADDR        mac_dest;
    MAC_ADDR        mac_source;
    int16           type;        //ip=0x8000, arp=0x0806, etc.
} ETHERNET_HEADER;

//prototypes
void rtl8019_write(int8 address, int8 data);
int8 rtl8019_read(int8 address);
void rtl8019_overrun(void);
void nic_init(void);

int16 nic_begin_packet_rx(void);
void nic_packet_get_data(int8* buffer, int16 len);
void nic_end_packet_rx(void);

void nic_putd(int8* buffer, int16 len);
void nic_begin_packet_tx(int16 len);
void nic_packet_put_data(int8 * buffer, int16 len);
void nic_packet_end_tx(void);

//configuration registers initial values
#define RTL8019_RCR_INIT      0x04  //accept broadcast packets and packets destined to this mac
#define RTL8019_IMR_INIT      0xFF  //enable all interrupts
#define RTL8019_RXSTOP_INIT   0x60  //last page of buffer in rtl8019 device
#define RTL8019_TXSTART_INIT  0x40  //start page for tx buffer
#define RTL8019_RXSTART_INIT  0x46  //end page for tx buffer (total pages = 6, total bytes = 6 * 256)
#define RTL8019_DCR_INIT      0x58  //byte wide dma, normal operation (no loopback), send packet command executed, fifo thresh select bit 1
#define RTL8019_TCR_INIT      0x00  //enable crc on tx and rx, normal operation (no loopback)

//ne2000 file registers
//page 0
#define  RTL8019_CR     0x00  // R/W   (on all pages)
#define  RTL8019_CLDA0  0x01  // R
#define  RTL8019_PSTART 0x01  // W
#define  RTL8019_CLDA1  0x02  // R
#define  RTL8019_PSTOP  0x02  // W
#define  RTL8019_BNRY   0x03  // R/W
#define  RTL8019_TSR    0x04  // R
#define  RTL8019_TPSR   0x04  // W
#define  RTL8019_NCR    0x05  // R
#define  RTL8019_TBCR0  0x05  // W
#define  RTL8019_FIFO   0x06  // R
#define  RTL8019_TBCR1  0x06  // W
#define  RTL8019_ISR    0x07  // R/W
#define  RTL8019_CRDA0  0x08  // R
#define  RTL8019_RSAR0  0x08  // W
#define  RTL8019_CRDA1  0x09  // R
#define  RTL8019_RSAR1  0x09  // W
#define  RTL8019_8019ID0   0x0A  // R
#define  RTL8019_RBCR0     0x0A  // W
#define  RTL8019_8019ID1   0x0B  // R
#define  RTL8019_RBCR1     0x0B  // W
#define  RTL8019_RSR    0x0C  // R
#define  RTL8019_RCR    0x0C  // W
#define  RTL8019_CNTR0  0x0D  // R
#define  RTL8019_TCR    0x0D  // W
#define  RTL8019_CNTR1  0x0E  // R
#define  RTL8019_DCR    0x0E  // W
#define  RTL8019_CNTR2  0x0F  // R
#define  RTL8019_IMR    0x0F  // W

//ne2000 file registers
//page 1
#define  RTL8019_PAR0   0x01  // R/W
#define  RTL8019_CURR   0x07  // R/W
#define  RTL8019_MAR0   0x08  // R/W

#define  RTL8019_RDMAPORT        0x10

#define  ETHERNET_MIN_PACKET_LENGTH  60   //rtl8019 cannot send packets smaller than this

int8 next_page;            //pointer to next page of rx buffer
int16 current_rx_address;  //pointer to next byte to get from rx buffer


//writes to the rtl8019 file register
void rtl8019_write(int8 address, int8 data) {
   rtl8019_address_out(address);
   rtl8019_data_out(data);
   output_low(RTL8019_PIN_IOW);
   output_high(RTL8019_PIN_IOW);
   rtl8019_data_in();  //float data pins
}

//reads an rtl8019 file register
int8 rtl8019_read(int8 address) {
   int8 data;

   rtl8019_address_out(address);
   output_low(RTL8019_PIN_IOR);
   data = rtl8019_data_in();
   output_high(RTL8019_PIN_IOR);

   return(data);
}

//initialize the rtl8019
void nic_init(void)
{
   int i;

   output_high(RTL8019_PIN_RESET);
   delay_ms(10);
   output_low(RTL8019_PIN_RESET);

   rtl8019_write(0x1F, rtl8019_read(0x1F) ) ;  //software reset
   delay_ms(50);
   rtl8019_write(RTL8019_ISR, rtl8019_read(RTL8019_ISR) ) ;  //clear isrs
   delay_ms(50);

   rtl8019_write(RTL8019_CR,0x21); //stop rtl8019, abort dma
   delay_ms(2);
   rtl8019_write(RTL8019_DCR, RTL8019_DCR_INIT);
   rtl8019_write(RTL8019_RBCR0,0x00); //clear byte count registers
   rtl8019_write(RTL8019_RBCR1,0x00);
   rtl8019_write(RTL8019_RCR, RTL8019_RCR_INIT);   //packets with broadcast destination address are accepted
   rtl8019_write(RTL8019_TCR,0x02);   //set to internal loopback
   rtl8019_write(RTL8019_TPSR, RTL8019_TXSTART_INIT);
   rtl8019_write(RTL8019_PSTART, RTL8019_RXSTART_INIT);
   rtl8019_write(RTL8019_PSTOP, RTL8019_RXSTOP_INIT);
   rtl8019_write(RTL8019_BNRY, RTL8019_RXSTART_INIT);
   rtl8019_write(RTL8019_CR, 0x61);   //set to page 1
   delay_ms(2);
   rtl8019_write(RTL8019_CURR, RTL8019_RXSTART_INIT);

   rtl8019_write(RTL8019_PAR0+0, MAC_ADDR_0);  //set mac address
   rtl8019_write(RTL8019_PAR0+1, MAC_ADDR_1);
   rtl8019_write(RTL8019_PAR0+2, MAC_ADDR_2);
   rtl8019_write(RTL8019_PAR0+3, MAC_ADDR_3);
   rtl8019_write(RTL8019_PAR0+4, MAC_ADDR_4);
   rtl8019_write(RTL8019_PAR0+5, MAC_ADDR_5);

   for (i=0;i<8;i++) {
      rtl8019_write(RTL8019_MAR0+i, 0xFF);
   }

   rtl8019_write(RTL8019_CR,0x21); //set to page 0
   rtl8019_write(RTL8019_DCR, RTL8019_DCR_INIT);

   rtl8019_write(RTL8019_ISR,0xFF);   //clear isr
   rtl8019_write(RTL8019_IMR, RTL8019_IMR_INIT);
   rtl8019_write(RTL8019_TCR, RTL8019_TCR_INIT);

   rtl8019_write(RTL8019_CR, 0x22); // start the NIC
}

//if there is data in the rx buffer that the rtl8019 received and accepted
// (remember, the rtl8019 will only accept packets with a matching MAC or with
// a broadcast MAC), then this routine configures the rtl8019 for dma read mode
// and returns the length of the packet.  If there is no data in the rx buffer
// this routine returns a zero and does not affect the rtl8019.  If the rx
// buffers are in an invalid state or there was an rx buffer this routine will
// reset the rx buffers.
int16 nic_begin_packet_rx(void)
{
   int8 i, bnry, header[4];
   int16 len;

   rtl8019_overrun();  //check rx overrun

   rtl8019_write(RTL8019_CR,0x62);
   i = rtl8019_read(RTL8019_CURR); //get current page pointer

   rtl8019_write(RTL8019_CR,0x22);
   bnry = rtl8019_read(RTL8019_BNRY);   //get last page pointer we have read

   if (bnry == i) {return(0);} //if pointers are same there is no new packet

   rtl8019_write(RTL8019_ISR, 1);  //clear rx interrupt flag

   if( (bnry >= RTL8019_RXSTOP_INIT) || (bnry < RTL8019_RXSTART_INIT) )   //make sure pointer is valid
   {
      rtl8019_write(RTL8019_BNRY, RTL8019_RXSTART_INIT);
      rtl8019_write(RTL8019_CR, 0x62);
      rtl8019_write(RTL8019_CURR, RTL8019_RXSTART_INIT);
      rtl8019_write(RTL8019_CR, 0x22);
      return(0);
   }

   rtl8019_write(RTL8019_RBCR0, 4);   //set dma to get 4 bytes
   rtl8019_write(RTL8019_RBCR1, 0);
   rtl8019_write(RTL8019_RSAR0, 0);   //set address to begining of packet
   rtl8019_write(RTL8019_RSAR1, bnry);
   rtl8019_write(RTL8019_CR, 0x0A);   //set dma to read
   for(i=0;i<4;i++) {
      header[i] = rtl8019_read(RTL8019_RDMAPORT);   //read 4byte header
   }

   rtl8019_write(RTL8019_CR, 0x22);   //end dma (it should have ended already, but just in case)
   for(i = 0; i <= 20; i++) {
      if (bit_test(rtl8019_read(RTL8019_ISR),6)) {break;}  //wait until dma is completed
   }
   rtl8019_write(RTL8019_ISR, 0x40);

   len = make16(header[2],header[3]); //get len from header
   next_page = header[1];

   current_rx_address = make16(bnry,0) + 4;

   if( (next_page >= RTL8019_RXSTOP_INIT) || (next_page < RTL8019_RXSTART_INIT) ) {
      return(0);   //dont continue if data is not in valid buffer
   }

   return (len-4);
}

//Reads len bytes from rtl8019 receive buffer, and stores the data into the location
// specified by the pointer buffer.  You can call this function several times to
// pull data out of the rtl8019 receive buffer in several smaller increments. (so
// you could read a 500 byte packet 25 bytes at a time, for example).
// nic_begin_packet_rx() MUST be called before this function can be used.
void nic_packet_get_data(int8 * buffer, int16 len)
{
   int16 i;

   rtl8019_write(RTL8019_RBCR0, make8(len,0)); //set dma read count
   rtl8019_write(RTL8019_RBCR1, make8(len,1));
   rtl8019_write(RTL8019_RSAR0, make8(current_rx_address,0));   //set dma read address
   rtl8019_write(RTL8019_RSAR1, make8(current_rx_address,1));
   rtl8019_write(RTL8019_CR, 0x0A);   //set dma to read

   for(i=0;i<len;i++) {
      buffer[i] = rtl8019_read(RTL8019_RDMAPORT);
   }

   rtl8019_write(RTL8019_CR, 0x22);   //end dma (it should already have ended)
   for(i = 0; i <= 20; i++) {
      if (bit_test(rtl8019_read(RTL8019_ISR),6)) {break;} //wait until dma is completed
   }
   rtl8019_write(RTL8019_ISR, 0x40);

   current_rx_address += len;   //set pointer to next byte in buffer to read for next read attempt

   if( current_rx_address >= 0x6000 )   //!
   current_rx_address = current_rx_address - (0x6000-0x4600) ;   //!
}

//Ends packet reading initiated by nic_begin_packet_rx().  Free's rtl8019
// receive buffer of this packet.  Will throw away any un-read data of the packet.
void nic_end_packet_rx(void)
{
   unsigned char i;

   rtl8019_write(RTL8019_CR, 0x22);   //end dma operation
   for(i = 0; i <= 20; i++) {
      if (bit_test(rtl8019_read(RTL8019_ISR),6)) {break;} //wait until dma is finished
   }
   rtl8019_write(RTL8019_ISR, 0x40);

   rtl8019_write(RTL8019_BNRY, next_page);   //set to start of next packet
}

//Configures the rtl8019's tx buffer to receive data to be sent as a packet.
// len specifies the total length of the packet that is going to be sent.
// If len is less than the min packet length, the rtl8019 should automatically
// add padding space.
void nic_begin_packet_tx(int16 len)
{
   int16 send_len;  //total len, including pad bytes

   if (len>ETHERNET_MIN_PACKET_LENGTH) {send_len=len;} else {send_len=ETHERNET_MIN_PACKET_LENGTH;}

   rtl8019_write(RTL8019_CR,0x22); //start, abort dma

   while ( rtl8019_read(RTL8019_CR) & 0x04 ) {}   //wait for last packet to finish

   rtl8019_write(RTL8019_TPSR,RTL8019_TXSTART_INIT); //go back to start of tx buffer
   rtl8019_write(RTL8019_RSAR0,0x00);
   rtl8019_write(RTL8019_RSAR1,RTL8019_TXSTART_INIT); //set start write/read address to start of tx buffer

   rtl8019_write(RTL8019_ISR,0x4A);   //clear tx interrupts

   rtl8019_write(RTL8019_RBCR0, make8(len,0)); //set dma count len
   rtl8019_write(RTL8019_RBCR1, make8(len,1));
   rtl8019_write(RTL8019_TBCR0, make8(send_len,0));  //set tx packet len
   rtl8019_write(RTL8019_TBCR1, make8(send_len,1));

   rtl8019_write(RTL8019_CR,0x12); //enable dma writes
}

//Puts data into the tx buffer to be sent as a packet.  This routine puts
// data into the tx buffer, it does not send the packet yet.  You can
// incrementally call this routine to incrementally add data to the buffer
// (for example, call it 25 times with 20 bytes each to add 500 bytes off
// data to buffer instead of having to put 500 bytes of data to the buffer
// at once).
// nic_begin_packet_tx() MUST be called first to configure the rtl8019
// to receive data for this packet.
void nic_packet_put_data(int8 * buffer, int16 len)
{
   int16 i;

   for(i=0;i<len;i++) {
      rtl8019_write(RTL8019_RDMAPORT, buffer[i]);
   }
}

//Sends the packet that was initiated by nic_begin_packet_tx(), with the data
// that was placed by nic_packet_put_data().  This routine must be called
// to send the packet.
void nic_packet_end_tx(void)
{
   rtl8019_write(RTL8019_ISR, 0x40);  //clear DMA interrupt
   rtl8019_write(RTL8019_CR,0x24);    //enable packet transmission
}

//This simple routine shows an example of sending a complete packet in one step.
// Large data payloads may not be feasible, requiring you to use multiple
// nic_packet_put_data() making this routine not applicaple to all applications.
void nic_putd(int8 * buffer, int16 len) {
   nic_begin_packet_tx(len);
   nic_packet_put_data(buffer,len);
   nic_packet_end_tx();
}

//reset buffer pages if there was an rx overrun
// you may want to check if you were currently tx a packet, if so you may want to resend that packet
void rtl8019_overrun(void)
{
   int8 data;

   data = rtl8019_read(RTL8019_ISR);
   if (bit_test(data,4)) {
      rtl8019_write(RTL8019_CR, 0x21);   //stop rtl8019, abort transmissions
      delay_ms(2);
      rtl8019_write(RTL8019_RBCR0, 0x00);   //clear byte count registers
      rtl8019_write(RTL8019_RBCR1, 0x00);
      rtl8019_write(RTL8019_TCR, 0x02);  //set to internal loopback
      rtl8019_write(RTL8019_CR, 0x22);   //set to remote write
      rtl8019_write(RTL8019_BNRY, RTL8019_RXSTART_INIT);   //last receive buffer
      rtl8019_write(RTL8019_CR, 0x62);   //set page to 1
      rtl8019_write(RTL8019_CURR, RTL8019_RXSTART_INIT);   //points to first page address of the first rx buffer page to be used for packet reception
      rtl8019_write(RTL8019_CR, 0x22);   //set to page 0
      rtl8019_write(RTL8019_ISR, 0x10);  //overflow (OVW) (ISR bits are cleared by setting to 1)
      rtl8019_write(RTL8019_TCR, RTL8019_TCR_INIT);  //transmit control register
      rtl8019_write(RTL8019_ISR, 0xFF);
   }
}
