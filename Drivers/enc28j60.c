///////////////////////////////////////////////////////////////////////////////
//
//                             enc28j60.c
//
// This driver is compatable with MAC.C from Microchip's TCP/IP stack, and as
// such the API is documented in Microchip's AN note AN833 for their TCP/IP 
// stack.  This driver only provides you with the MAC/PHY layers of a TCP/IP
// stack.
//
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2003 Custom Computer Services          ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////


/*~*~*~*~*~*~*~*~*~*~*~*~ MAC PHY CONFIG *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

#ifndef debug_printf
 #define debug_printf(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u)
 #define DO_DEBUG FALSE
#endif

#define MAC_USE_FULL_DUPLEX   TRUE
#define MAC_USE_LOOPBACK      FALSE
#define MAC_HALF_DUPLEX_ECHO  FALSE //if using half duplex, do you want a transmit echo?

//if you are not using the Microchip TCP/IP stack, this will define functions
//that would have been defined normally by MAC.H
#ifndef MAC_H
   #define MAC_IP      0
   #define MAC_ARP     0x6
   #define ETHER_IP        0x00
   #define ETHER_ARP       0x06

   #define MAC_UNKNOWN 0x0ff

   #define INVALID_BUFFER  0xff

   //int8 NICCurrentRdPtr;
   //#define MACGetRxBuffer()        (NICCurrentRdPtr)

   int8 NICCurrentTxBuffer;
   #define NICGetTxBuffer()        (NICCurrentTxBuffer)
   #define MACGetTxBuffer()        (NICCurrentTxBuffer)

   typedef struct _MAC_ADDR_ {
      int8 v[6];
   } MAC_ADDR;

   #define MY_MAC_BYTE1 0x11
   #define MY_MAC_BYTE2 0x22
   #define MY_MAC_BYTE3 0x33
   #define MY_MAC_BYTE4 0x44
   #define MY_MAC_BYTE5 0x55
   #define MY_MAC_BYTE6 0x66
#endif

typedef struct _ETH_HDR_ {
   MAC_ADDR dest;
   MAC_ADDR source;
   int16 type;
} ETHERNET_HEADER;

/*~*~*~*~*~*~*~*~*~*~*~*~ SPI CONFIG *~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

//if you use SPI, you still need to define clock and reset pins
#ifndef ENC_MAC_USE_SPI
#define ENC_MAC_USE_SPI FALSE
#endif

#ifndef PIN_ENC_MAC_SO
   #define PIN_ENC_MAC_SO  PIN_B2   // PIC <<<< ENC
   #define PIN_ENC_MAC_SI  PIN_D3   // PIC >>>> ENC
   #define PIN_ENC_MAC_CLK PIN_D2
   #define PIN_ENC_MAC_CS  PIN_D1
   #define PIN_ENC_MAC_RST PIN_D0
   #define PIN_ENC_MAC_INT PIN_B0
   #define PIN_ENC_MAC_WOL PIN_B1
   //#use fast_io(D)
   //#define mac_enc_spi_tris_init()  set_tris_d(0b00010000)
   #define mac_enc_spi_tris_init()  *0xF93=*0xF93 | 7; *0xF95=*0xF95 & 0xF0
#endif

/*~*~*~*~*~*~*~*~*~*~*~*~ BUFFER CONFIG *~*~*~~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

#define ENC_MAC_BUFFER_SIZE   1024*8

#ifndef MAC_TX_BUFFER_SIZE
 #define MAC_TX_BUFFER_SIZE          1500
#endif

#ifndef MAC_TX_BUFFER_COUNT
 #define MAC_TX_BUFFER_COUNT         1
#endif

#define ENC_MAC_TX_SIZE    ((int16)(MAC_TX_BUFFER_SIZE + 8)*MAC_TX_BUFFER_COUNT)
#define ENC_MAC_RX_SIZE  (ENC_MAC_BUFFER_SIZE-ENC_MAC_TX_SIZE)

#ifndef MAC_RX_BUFFER_SIZE
 #define MAC_RX_BUFFER_SIZE         MAC_TX_BUFFER_SIZE
#endif

#if (ENC_MAC_TX_SIZE>ENC_MAC_BUFFER_SIZE)
 #error You defined more transmit buffers than the MAC has space for
#endif

#if ((ENC_MAC_RX_SIZE+6)<MAC_RX_BUFFER_SIZE)
  #error Not enough space to receive a packet
#endif

#if MAC_RX_BUFFER_SIZE!=MAC_TX_BUFFER_SIZE
 #error The ENC28J60 must have the same packet size for TX and RX
#endif

#define ENC_MAC_TX_START   (int16)0
#define ENC_MAC_TX_END     ((int16)ENC_MAC_TX_START + ENC_MAC_TX_SIZE - 1)
#define ENC_MAC_RX_START   ((int16)ENC_MAC_TX_END + 1)
#define ENC_MAC_RX_END     ((int16)ENC_MAC_BUFFER_SIZE - 1)

#define ENC_MAC_TX_BUFFER_START(x)  ((int16)ENC_MAC_TX_START+(MAC_TX_BUFFER_SIZE+7)*x)

/*~*~*~*~*~*~*~*~*~*~*~*~ REGISTER LOCATIONS ~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*/

#define ENC_MAC_ERDPTL     0x00
#define ENC_MAC_ERDPTH     0x01
#define ENC_MAC_EWRPTL     0x02
#define ENC_MAC_EWRPTH     0x03
#define ENC_MAC_ETXSTL     0x04
#define ENC_MAC_ETXSTH     0x05
#define ENC_MAC_ETXNDL     0x06
#define ENC_MAC_ETXNDH     0x07
#define ENC_MAC_ERXSTL     0x08
#define ENC_MAC_ERXSTH     0x09
#define ENC_MAC_ERXNDL     0x0A
#define ENC_MAC_ERXNDH     0x0B
#define ENC_MAC_ERXRDPTL   0x0C
#define ENC_MAC_ERXRDPTH   0x0D
#define ENC_MAC_ERXWRPTL   0x0E
#define ENC_MAC_ERXWRPTH   0x0F
#define ENC_MAC_EIR        0x1C
#define ENC_MAC_ESTAT      0x1D
#define ENC_MAC_ECON2      0x1E
#define ENC_MAC_ECON1      0x1F
#define ENC_MAC_ERXFCON    0x38
#define ENC_MAC_EPKTCNT    0x39
#define ENC_MAC_MACON1     0x40
#define ENC_MAC_MACON2     0x41
#define ENC_MAC_MACON3     0x42
#define ENC_MAC_MABBIPG    0x44
#define ENC_MAC_MAIPGL     0x46
#define ENC_MAC_MAIPGH     0x47
#define ENC_MAC_MAMXFLL    0x4A
#define ENC_MAC_MAMXFLH    0x4B
#define ENC_MAC_MICMD      0x52
#define ENC_MAC_MIREGADR   0x54
#define ENC_MAC_MIWRL      0x56
#define ENC_MAC_MIWRH      0x57
#define ENC_MAC_MIRDL      0x58
#define ENC_MAC_MIRDH      0x59
#define ENC_MAC_MAADR1     0x60
#define ENC_MAC_MAADR0     0x61
#define ENC_MAC_MAADR3     0x62
#define ENC_MAC_MAADR2     0x63
#define ENC_MAC_MAADR5     0x64
#define ENC_MAC_MAADR4     0x65
#define ENC_MAC_MISTAT     0x6A
#define ENC_MAC_ECOCON     0x75
#define ENC_MAC_EFLOCON    0x77

#define ENC_MAC_PHY_PHCON1    0x00
#define ENC_MAC_PHY_PHSTAT1   0x01
#define ENC_MAC_PHY_PHID1     0x02
#define ENC_MAC_PHY_PHID2     0x03
#define ENC_MAC_PHY_PHCON2    0x10
#define ENC_MAC_PHY_PHSTAT2   0x11
#define ENC_MAC_PHY_PHLCON    0x14

struct {
   int16 next_packet;   //next packet pointer (first 2 bytes of received packet)
   int16 len;           //length of packet (bits0-15 of status vector)
   int16 status;        //status flags (bits16-31 of status vector)
   int8 active;         //flag for the driver to know we are currently processing a packet
   int16 packet_start;  //marker for the driver to know where the packet starts
} __last_enc_header;

struct {
   int8 isReserved;
   int8 isActive;
   int8 isFlushing;
   int16 len;
} __mac_tx_buffers[MAC_TX_BUFFER_COUNT];


//spi functions
void mac_spi_init(void);
void mac_reg_init(void);
void enc_mac_spi_out_byte(int8 d);  //use spi to write a byte
int8 enc_mac_spi_in_byte(void);  //use spi to read a byte
void enc_mac_write_bytes(int8 oa, int8 *ptr, int16 len);  //write command (oa) then bytes to SPI
void enc_mac_read_bytes(int8 oa, int8 *ptr, int16 len); //write command (oa) then read bytes from spi
void enc_mac_soft_reset(void);   //issue a spi command to reset the unit
void enc_mac_control_bit_set(int8 address, int8 bit); //set a bit in a control register
void enc_mac_control_bit_clear(int8 address, int8 bit);  //clear a bit in a control register
void enc_mac_control_reg_address(int8 &address);   //set paging mode to read/write a control byte
void enc_mac_write_control_byte(int8 address, int8 data);   //write a control byte
int8 enc_mac_read_control_byte(int8 address, int8 needs_dummy_read); //read a control byte
void enc_mac_write_phy_word(int8 address, int16 data);   //write a phy register
int16 enc_mac_read_phy_word(int8 address);   //read a phy register
void enc_mac_write_control_word(int8 address, int16 data);
int16 enc_mac_read_control_word(int8 address, int8 needs_dummy_read);

void debug_print_mac(MAC_ADDR *mac) {
   debug_printf("%X:%X:%X:%X:%X:%X", mac->v[0], mac->v[1], mac->v[2],
      mac->v[3], mac->v[4], mac->v[5]
   );
}

void debug_print_eth_header(ETHERNET_HEADER *header) {
   debug_printf("DST:");
   debug_print_mac(&header->dest);
   debug_printf(" SRC:");
   debug_print_mac(&header->source);
   debug_printf(" PRT:%X%X", make8(header->type,0), make8(header->type,1));
}

//prints out the 7byte transmit status.
//assumes that etxnd points to the last byte of the data (so etxnd+1 is firsy
//byte of status).
void debug_display_tx_status(void);

int8 enc_mac_spi_exchange(int8 out) {
   int8 in;
 #if ENC_MAC_USE_SPI
   in=spi_read(out);
 #else
   int8 i;
   for (i=0;i<8;i++) {
      output_bit(PIN_ENC_MAC_SI, shift_left(&out, 1, 0));
      output_high(PIN_ENC_MAC_CLK);
      shift_left(&in, 1, input(PIN_ENC_MAC_SO));
      output_low(PIN_ENC_MAC_CLK);
   }
 #endif
   return(in);
}

////////////////////////////////////////////////////////////////////////////
//
// enc_mac_spi_out_byte(int8 d)
//
// Clock out an 8bit byte on the SPI lines.
// Assumes that CS has already be lowered.
// Note: ENC28J60 only supports SPI mode0,0
//
// Input: d - byte to write
//
////////////////////////////////////////////////////////////////////////////
#define enc_mac_spi_out_byte enc_mac_spi_exchange
/*
void enc_mac_spi_out_byte(int8 d) {
   int8 i;
   for (i=0;i<8;i++) {
      output_bit(PIN_ENC_MAC_SI, shift_left(&d, 1, 0));
      output_high(PIN_ENC_MAC_CLK);
      output_low(PIN_ENC_MAC_CLK);
   }
}
*/

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
/*
int8 enc_mac_spi_in_byte(void) {
   int8 d, i;
   for (i=0;i<8;i++) {
      shift_left(&d, 1, input(PIN_ENC_MAC_SO));
      output_high(PIN_ENC_MAC_CLK);
      output_low(PIN_ENC_MAC_CLK);
   }
   return(d);
}
*/
#define enc_mac_spi_in_byte() enc_mac_spi_exchange(0)

////////////////////////////////////////////////////////////////////////////
//oa is opcode/address
////////////////////////////////////////////////////////////////////////////
void enc_mac_write_bytes(int8 oa, int8 *ptr, int16 len) {
   output_low(PIN_ENC_MAC_CS);

   enc_mac_spi_out_byte(oa);
   while(len--) {
      enc_mac_spi_out_byte(*ptr);
      ptr++;
   }

   output_high(PIN_ENC_MAC_CS);
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void enc_mac_read_bytes(int8 oa, int8 *ptr, int16 len) {
   char c;
   output_low(PIN_ENC_MAC_CS);

   enc_mac_spi_out_byte(oa);
   while(len--) {
      c=enc_mac_spi_in_byte();
      if (ptr) {
         *ptr=c;
         ptr++;
      }
   }

   output_high(PIN_ENC_MAC_CS);
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void enc_mac_soft_reset(void) {
   enc_mac_write_bytes(0xFF, 0, 0);
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void enc_mac_control_bit_set(int8 address, int8 bit) {
   int8 d;
   d=1<<bit;
   address&=0x1F;
   enc_mac_write_bytes(address | 0x80, &d, 1);
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void enc_mac_control_bit_clear(int8 address, int8 bit) {
   int8 d;
   d=1<<bit;
   address&=0x1F;
   enc_mac_write_bytes(address | 0xA0, &d, 1);
}

////////////////////////////////////////////////////////////////////////////
//set the paging bits if necessary
////////////////////////////////////////////////////////////////////////////
void enc_mac_control_reg_address(int8 &address) {
   if ((address & 0x1F) < 0x1A) {
      if (bit_test(address,5))
         enc_mac_control_bit_set(ENC_MAC_ECON1, 0);
      else
         enc_mac_control_bit_clear(ENC_MAC_ECON1, 0);
      if (bit_test(address,6))
         enc_mac_control_bit_set(ENC_MAC_ECON1, 1);
      else
         enc_mac_control_bit_clear(ENC_MAC_ECON1, 1);
   }

   address &= 0x1F;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void enc_mac_write_control_byte(int8 address, int8 data) {
   enc_mac_control_reg_address(address);
   enc_mac_write_bytes(address | 0x40, &data, 1);
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void enc_mac_write_control_word(int8 address, int16 data) {
   enc_mac_write_control_byte(address++,make8(data,0));
   enc_mac_write_control_byte(address,make8(data,1));
}

////////////////////////////////////////////////////////////////////////////
//set needs_dummy_read to TRUE if you are reading a MAC or MII control
//register that needs an 8bit dummy read before the data.  why did microchip
//do this?
////////////////////////////////////////////////////////////////////////////
int8 enc_mac_read_control_byte(int8 address, int8 needs_dummy_read) {
   int8 ret[2];
   enc_mac_control_reg_address(address);
   if (needs_dummy_read)
      enc_mac_read_bytes(address, &ret[0], 2);
   else
      enc_mac_read_bytes(address, &ret[1], 1);
   return(ret[1]);
}

////////////////////////////////////////////////////////////////////////////
//set needs_dummy_read to TRUE if you are reading a MAC or MII control
//register that needs an 8bit dummy read before the data.  why did microchip
//do this?
////////////////////////////////////////////////////////////////////////////
int16 enc_mac_read_control_word(int8 address, int8 needs_dummy_read) {
   /*
   union {
      int16 v[2];
      int8 b[4];
   } ret;
   enc_mac_control_reg_address(address);
   if (needs_dummy_read)
      enc_mac_read_bytes(address, &ret.b[1], 3);
   else
      enc_mac_read_bytes(address, &ret.v[1], 2);
   return(ret.v[1]);
   */
   union {
      int8 b[2];
      int16 v;
   } ret;

   ret.b[0]=enc_mac_read_control_byte(address++, needs_dummy_read);
   ret.b[1]=enc_mac_read_control_byte(address, needs_dummy_read);
   return(ret.v);
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void enc_mac_write_phy_word(int8 address, int16 data) {
   int8 stat,i;

   enc_mac_write_control_byte(ENC_MAC_MIREGADR, address&0x1F);   //set PHY address
   enc_mac_write_control_byte(ENC_MAC_MIWRL, make8(data,0));
   enc_mac_write_control_byte(ENC_MAC_MIWRH, make8(data,1));

   delay_us(11);

   for (i=0;i<50;i++) { //wait 50us for busy to be clear
      stat=enc_mac_read_control_byte(ENC_MAC_MISTAT,1);
      if (!bit_test(stat,0))
         break;
      delay_us(1);
   }
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
int16 enc_mac_read_phy_word(int8 address) {
   int8 stat,i;
   union {
      int8 b[2];
      int16 v;
   } ret;
   enc_mac_write_control_byte(ENC_MAC_MIREGADR, address&0x1F);   //set PHY address
   enc_mac_write_control_byte(ENC_MAC_MICMD, 1);  //set MIIRD (read phy)
   delay_us(11);

   for (i=0;i<100;i++) { //wait 100us for busy to be clear
      stat=enc_mac_read_control_byte(ENC_MAC_MISTAT,1);
      if (!bit_test(stat,0))
         break;
      delay_us(1);
   }

   for (i=0;i<100;i++) { //wait 100us for nvalid to be clear
      stat=enc_mac_read_control_byte(ENC_MAC_MISTAT,1);
      if (!bit_test(stat,2))
         break;
      delay_us(1);
   }


   enc_mac_write_control_byte(ENC_MAC_MICMD, 0);  //clear MICMD.MIIRD
   ret.b[0]=enc_mac_read_control_byte(ENC_MAC_MIRDL,1);
   ret.b[1]=enc_mac_read_control_byte(ENC_MAC_MIRDH,1);

   return(ret.v);
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void mac_spi_init(void) {
   int8 i;

   output_high(PIN_ENC_MAC_CS);
 #if ENC_MAC_USE_SPI
   setup_spi(SPI_MASTER | SPI_L_TO_H | SPI_XMIT_L_TO_H);
 #else
   mac_enc_spi_tris_init();
   output_low(PIN_ENC_MAC_CLK);
   output_float(PIN_ENC_MAC_SO);
 #endif
   output_low(PIN_ENC_MAC_RST);
   delay_ms(10);
   output_high(PIN_ENC_MAC_RST);

   __last_enc_header.active=FALSE;

   //wait until start-up oscillator is stable
   delay_us(500);

   //take mac out of reset
   enc_mac_write_control_byte(ENC_MAC_MACON2, 0);

   //disable reception
   enc_mac_write_control_byte(ENC_MAC_MACON1, 0);
   enc_mac_write_control_byte(ENC_MAC_ECON1, 0);

   for (i=0;i<MAC_TX_BUFFER_COUNT;i++) {
      __mac_tx_buffers[i].isReserved=FALSE;
      __mac_tx_buffers[i].isActive=FALSE;
      __mac_tx_buffers[i].isFlushing=FALSE;
   }
}

void mac_reg_init(void) {
   //NICCurrentRdPtr=0;
   NICCurrentTxBuffer=0;

   ////set buffer access
   //set rx start location
   enc_mac_write_control_word(ENC_MAC_ERXSTL, ENC_MAC_RX_START);
   //set rx end location.  rest of buffer is for transmit
   enc_mac_write_control_word(ENC_MAC_ERXNDL, ENC_MAC_RX_END);
   //set rx rd fifo protection to the end of the buffer for now
   enc_mac_write_control_word(ENC_MAC_ERXRDPTL, ENC_MAC_RX_END);
   //set start location for first read
   __last_enc_header.next_packet=ENC_MAC_RX_START;

   //always pad to 60 bytes, apend a valid crc,
  #if MAC_USE_FULL_DUPLEX
   enc_mac_write_control_byte(ENC_MAC_MACON3, 0x31);  //enable frame length status reporting, full duplex
  #else
   enc_mac_write_control_byte(ENC_MAC_MACON3, 0x30);  //enable frame length status reporting, half duplex
  #endif


   //we will leave macon4 to default

   //set maximum frame length
   enc_mac_write_control_word(ENC_MAC_MAMXFLL, MAC_TX_BUFFER_SIZE);

   //back-to-back inter-pakcet-gap to 0x15h in full duplex
   //back-to-back inter-pakcet-gap to 0x12h in half duplex
  #if MAC_USE_FULL_DUPLEX
   enc_mac_write_control_byte(ENC_MAC_MABBIPG, 0x15);
  #else
   enc_mac_write_control_byte(ENC_MAC_MABBIPG, 0x12);
  #endif

   //non-back-toback inter-packet gap register
   enc_mac_write_control_byte(ENC_MAC_MAIPGL, 0x12);

   //if half-duplex, non-back-to-back inter-packet gap register high should be 0x12
  #if !MAC_USE_FULL_DUPLEX
   enc_mac_write_control_byte(ENC_MAC_MAIPGH, 0x0C);
  #endif

   //maclcon1 and maclcon2 ???

   //receive filter control, default
   //(accept unicast, accept broadcast, reject if bad crc)
   enc_mac_write_control_byte(ENC_MAC_ERXFCON, 0xA1);
   //enc_mac_write_control_byte(ENC_MAC_ERXFCON, 0);

   //mac address
   enc_mac_write_control_byte(ENC_MAC_MAADR0, MY_MAC_BYTE6);
   enc_mac_write_control_byte(ENC_MAC_MAADR1, MY_MAC_BYTE5);
   enc_mac_write_control_byte(ENC_MAC_MAADR2, MY_MAC_BYTE4);
   enc_mac_write_control_byte(ENC_MAC_MAADR3, MY_MAC_BYTE3);
   enc_mac_write_control_byte(ENC_MAC_MAADR4, MY_MAC_BYTE2);
   enc_mac_write_control_byte(ENC_MAC_MAADR5, MY_MAC_BYTE1);

   //disable clockout
   enc_mac_write_control_byte(ENC_MAC_ECOCON, 0);

   //for proper duplex config, make phcon1.pdpxmd [bit8] == macon3.fuldpx [bit0]
  #if MAC_USE_LOOPBACK
   #if MAC_USE_FULL_DUPLEX
    enc_mac_write_phy_word(ENC_MAC_PHY_PHCON1, 0x4100);
   #else
    enc_mac_write_phy_word(ENC_MAC_PHY_PHCON1, 0x4000);
   #endif
  #elif MAC_USE_FULL_DUPLEX
   enc_mac_write_phy_word(ENC_MAC_PHY_PHCON1, 0x0100);
  #else
   enc_mac_write_phy_word(ENC_MAC_PHY_PHCON1, 0x0000);  //we are using half duplex, so claer pdpxmd
  #endif

  #if !MAC_USE_FULL_DUPLEX && !MAC_HALF_DUPLEX_ECHO && !MAC_USE_LOOPBACK
   enc_mac_write_phy_word(ENC_MAC_PHY_PHCON2, 0x0100);
  #endif

  #if MAC_USE_FULL_DUPLEX
   enc_mac_write_control_byte(ENC_MAC_EFLOCON,2);
  #endif

   //set LEDA to link status, LEDB to tx/rx with 140ms stretch
   enc_mac_write_phy_word(ENC_MAC_PHY_PHLCON, 0x0476);

   //set receive enable bit
  #if MAC_USE_LOOPBACK
   #if MAC_USE_FULL_DUPLEX
    enc_mac_write_control_byte(ENC_MAC_MACON1, 0x1D);  //full duplex should set TXPAUS and RXPAUS
   #else
    enc_mac_write_control_byte(ENC_MAC_MACON1, 0x11);  //full duplex should set TXPAUS and RXPAUS
   #endif
  #elif MAC_USE_FULL_DUPLEX
   enc_mac_write_control_byte(ENC_MAC_MACON1, 0x0D);  //full duplex should set TXPAUS and RXPAUS
  #else
   enc_mac_write_control_byte(ENC_MAC_MACON1, 1);
  #endif

  enc_mac_write_control_byte(ENC_MAC_ECON1, 4);
}

////////////////////////////////////////////////////////////////////////////
///                                                                      ///
///  Higher level NIC functions for reading and writing packets          ///
///                                                                      ///
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
// NICGetArray(char *ptr, int16 size)
//
// Reads from the NIC buffer as specified by the read pointer.  You can set
// the write pointer by using NICSetRxBuffer().  Also, after a NICGetHeader()
// the read pointer will point to the first data byte of the received ethernet
// packet.  AUTOINC is set, so the read pointer will increment after each call.
//
////////////////////////////////////////////////////////////////////////////
int16 NICGetArray(char *ptr, int16 size) {
   enc_mac_read_bytes(0x3A,ptr,size);
   return(size);
}

////////////////////////////////////////////////////////////////////////////
//
// NICGet()
//
// Reads from the NIC buffer as specified by the read pointer.  You can set
// the write pointer by using NICSetRxBuffer().  Also, after a NICGetHeader()
// the read pointer will point to the first data byte of the received ethernet
// packet.  AUTOINC is set, so the read pointer will increment after each call.
//
////////////////////////////////////////////////////////////////////////////
int8 NICGet(void) {
   int8 b;
   enc_mac_read_bytes(0x3A, &b, 1);
   return(b);
}

////////////////////////////////////////////////////////////////////////////
//
// NICPutArray(char *ptr, int16 size)
//
// Writes the specified array to the NIC transmit buffer as specified by
// the NIC write pointer.  You can set the NIC write pointer by using
// NICSetTxBuffer().  AUTOINC is set, so the write pointer will increment after
// each call.
//
////////////////////////////////////////////////////////////////////////////
#define NICPutArray(ptr,size)  enc_mac_write_bytes(0x7A, ptr, size)

////////////////////////////////////////////////////////////////////////////
//
// NICPut(char c)
//
// Writes c to the NIC buffer as specified by the write pointer.  You can set
// the write pointer by using NICSetTxBuffer().  AUTOINC is set, so the
// write pointer will increment after each call.
//
////////////////////////////////////////////////////////////////////////////
void NICPut(char c) {
   enc_mac_write_bytes(0x7A, &c, 1);
}

////////////////////////////////////////////////////////////////////////////
//
// __NICSetRxBuffer(int16 offset)
//
// Set read pointer to a specific byte in the received packet.  If offset
// is zero, goes to the first byte of the receive packet which is the 6byte
// status header.  If offset is 6, goes to the first byte of the received
// ethernet packet.
//
// NOTE: NICGetHeader() must have been called and returned TRUE first,
//  this routine relies on having NICGetHeader() setting up the start
//  location of the packet.
//
// NOTE: It also sets the write pointer.  Just in case you want to write over
//  data in the receive buffer - this is unlikely to happen though.
//
////////////////////////////////////////////////////////////////////////////
void __NICSetRxBuffer(int16 offset) {
   int16 location;
   location=__last_enc_header.packet_start + offset;
   if (location > ENC_MAC_RX_END) {
      location-=ENC_MAC_RX_END - __last_enc_header.packet_start + 1;
   }
   enc_mac_write_control_word(ENC_MAC_ERDPTL,location);
   enc_mac_write_control_word(ENC_MAC_EWRPTL,location);
}

////////////////////////////////////////////////////////////////////////////
//
// NICSetRxBuffer(int16 offset)
//
// Set read and write pointer to a specific byte in the received packet.
// If offset is 0, goes to the first byte of the received ethernet packet.
// Skips the first 6bytes of the received packet which is the header.
//
// NOTE: NICGetHeader() must have been called and returned TRUE first,
//  this routine relies on having NICGetHeader() setting up the start
//  location of the packet.
//
// NOTE: It also sets the write pointer.  Just in case you want to write over
//  data in the receive buffer - this is unlikely to happen though.
//
////////////////////////////////////////////////////////////////////////////
void NICSetRxBuffer(int16 offset) {
   __NICSetRxBuffer(offset+6);
}

////////////////////////////////////////////////////////////////////////////
//
// NICDiscardRx()
//
// After a successful NICGetHeader(), this will toss the rest of the packet
// making space for more received packets.
//
// NOTE: NICGetHeader() must have been called and returned TRUE first,
//  this routine relies on having NICGetHeader() setting up the end
//  location of the packet.
//
////////////////////////////////////////////////////////////////////////////
void NICDiscardRx(void) {
   int8 econ2;
   __last_enc_header.active=FALSE;
   enc_mac_write_control_word(ENC_MAC_ERXRDPTL, __last_enc_header.next_packet);
   econ2=enc_mac_read_control_byte(ENC_MAC_ECON2,0);
   enc_mac_write_control_byte(ENC_MAC_ECON2, econ2|0x40);
}

////////////////////////////////////////////////////////////////////////////
//
// NICGetFreeRxSize()
//
// Returns the free space available in the receive buffer.
//
////////////////////////////////////////////////////////////////////////////
int16 NICGetFreeRxSize(void) {
   int16 ERXWRPT;
   int16 ERXRDPT;
   int16 ERXND;
   int16 ERXST;
   int16 free;

   ERXWRPT=enc_mac_read_control_word(ENC_MAC_ERXWRPTL,0);
   ERXRDPT=enc_mac_read_control_word(ENC_MAC_ERXRDPTL,0);
   ERXND=enc_mac_read_control_word(ENC_MAC_ERXNDL,0);
   ERXST=enc_mac_read_control_word(ENC_MAC_ERXSTL,0);

   if (ERXWRPT > ERXRDPT)
      free=(ERXND-ERXST)-(ERXWRPT-ERXRDPT);
   else if (ERXWRPT == ERXRDPT)
      free=ERXND-ERXST;
   else
      free=ERXRDPT - ERXWRPT - 1;

   return(free);
}

////////////////////////////////////////////////////////////////////////////
//
// NICGetHeader(ETHERNET_HEADER *header)
//
// Checks the receive buffer for an incoming ethernet packet.  If an
// ethernet packet exists in the receive buffer, the header pointer is
// updated with that ethernet header and the function returns TRUE.
// Any subsequent calls to NICGet() and NICGetArray() will then get the
// data fields of the ethernet packet.  Also, NICSetRxBuffer() can be
// used to set the read pointer to the specified byte of the ethernet packet,
// and NICDiscardRx() can be used to discard the packet to make room for
// more packets.
//
// If NICGetHeader() returns FALSE then NICGet(), NICGetArray(),
// NICSetRxBuffer() and NICDiscardRx() should not be called.
//
// NOTE: If NICGetHeader() is called while an existing packet is in the
//  receive buffer, it will discard that previous ethernet packet.
//
// NOTE: NICGetHeader() will swap the type field so it is little endian.
//
// NOTE: If curious, the global __last_enc_header.status saves the 4byte
//  ENC28J60 header status for the current packet.
//
////////////////////////////////////////////////////////////////////////////
int NICGetHeader(ETHERNET_HEADER *header) {
   union {
      int8 v[2];
      int16 val;
   } type,tmp;

   if (__last_enc_header.active)
      NICDiscardRx();

   if (bit_test(enc_mac_read_control_byte(ENC_MAC_EIR,0),6)) {
      __last_enc_header.packet_start=__last_enc_header.next_packet;
      __NICSetRxBuffer(0);
      NICGetArray(&__last_enc_header, 6);
      __last_enc_header.active=TRUE;
      NICGetArray(header,12);
      NICGetArray(&type,2);
      tmp.v[0]=type.v[1];
      tmp.v[1]=type.v[0];
      header->type=tmp.val;
      debug_printf("\r\n\nGOT %LX", tmp.val);
      return(TRUE);
   }
   return(FALSE);
}


////////////////////////////////////////////////////////////////////////////
//
// NICIsTransmitting()
//
// Returns TRUE if the ENC28J60 is currently transmitting a packet
//
////////////////////////////////////////////////////////////////////////////
int NICIsTransmitting(void) {
   int8 econ1;
   econ1=enc_mac_read_control_byte(ENC_MAC_ECON1, FALSE);
   return(bit_test(econ1,3));
}

////////////////////////////////////////////////////////////////////////////
//
// NICIsLinked()
//
// Returns TRUE if the ENC28J60 is currently connected to a ethernet cable
// and connected to atleast one more unit (PC, hub, router, etc) via ethernet.
//
////////////////////////////////////////////////////////////////////////////
int NICIsLinked(void) {
   int16 phstat2;
   phstat2=enc_mac_read_phy_word(ENC_MAC_PHY_PHSTAT2);
   return(bit_test(phstat2,10));
}

////////////////////////////////////////////////////////////////////////////
//
// NICGetFreeTxBuffer
//
// Returns an identifier of a free buffer we can use to transmit ethernet
// packets.  If no buffer is free it will return INVALID_BUFFER.  Also
// checks flushing status of last NICFlush()
//
// NOTE: The buffer is not marked as being used until NICSetTxBuffer() is
//  called.
//
////////////////////////////////////////////////////////////////////////////
int NICGetFreeTxBuffer(void) {
   int8 i;

   //first, clear any flushing status:
   if (!NICIsTransmitting()) {
      for (i=0;i<MAC_TX_BUFFER_COUNT;i++) {
         if (__mac_tx_buffers[i].isFlushing) {
            debug_printf("\r\n\n*** TX ********************");
            debug_printf("\r\nDONE TRANSMIT ");
            debug_display_tx_status();
            __mac_tx_buffers[i].isFlushing=FALSE;
            __mac_tx_buffers[i].isActive=FALSE;
         }
      }
   }

   //now, search for an open (isActive==FALSE and isReserved==FALSE) buffer
   for (i=0;i<MAC_TX_BUFFER_COUNT;i++) {
      if (!__mac_tx_buffers[i].isReserved && !__mac_tx_buffers[i].isActive) {
         debug_printf("\r\n\n*** TX ********************");
         return(i);
      }
   }

   return(INVALID_BUFFER);
}

////////////////////////////////////////////////////////////////////////////
//
// NICIsPutReady()
//
// Returns TRUE if any transmit buffer is available for transmit.
// NICCurrentTxBuffer is set to the next free buffer, meaning NICGetTxBuffer()
// will return the next free transmit buffer that will be used.
//
////////////////////////////////////////////////////////////////////////////
int NICIsTxReady(void) {
   int8 buf;
   buf=NICGetFreeTxBuffer();
   if (buf!=INVALID_BUFFER) {
      NICCurrentTxBuffer=buf;
      return(TRUE);
   }
   else {
      return(FALSE);
   }
}

////////////////////////////////////////////////////////////////////////////
//
// __NICSetTxBuffer(int8 buffer, int16 offset)
//
// Set the read and write pointer to the specified location (+offset) on the
// ENC28J60.
//
// NOTE - The read pointer is also set so you can use NICGet() and
//  NICGetArray() to read from the transmit buffer - useful when you need to
//  reread your packet to determine the checksum.
//
// NOTE - assumes that the transmit buffer starts at 0
//
// NOTE - don't forget that the first byte of the transmit buffer is a control
//   byte and is unrelated to the ETHERNET packet contents.
//
////////////////////////////////////////////////////////////////////////////
void __NICSetTxBuffer(int8 buffer, int16 offset) {
   int16 location;
   NICCurrentTxBuffer=buffer;
   __mac_tx_buffers[buffer].isActive=TRUE;
   location=ENC_MAC_TX_BUFFER_START(buffer);
   location+=offset;
   enc_mac_write_control_word(ENC_MAC_ERDPTL,location);
   enc_mac_write_control_word(ENC_MAC_EWRPTL,location);
   debug_printf("\r\nNIC SET TX %U %LU -> %LX", buffer, offset, location);
}

////////////////////////////////////////////////////////////////////////////
//
// NICSetTxBuffer(int8 buffer, int16 offset)
//
// Set the read and write pointer to the specified location (+offset) on the
// ENC28J60.  Same as __NICSetTxBuffer(), but skips over the control byte
// at the beginning of the packet. See __NICSetTxBuffer() documentation.
//
////////////////////////////////////////////////////////////////////////////
#define NICSetTxBuffer(b,o) __NICSetTxBuffer(b,o+1)

////////////////////////////////////////////////////////////////////////////
//
// NICReserveTxBuffer(int8 buffer)
//
// Marks the transmit buffer as reserved.  Even after the buffer is flushed,
// the contents of the buffer will still be locked.  Useful in TCP where
// a packet needs to be transmitted if there was no ACK.  NICDiscardTx()
// must be called to unreserve the buffer.
//
////////////////////////////////////////////////////////////////////////////
void NICReserveTxBuffer(int8 buffer) {
   __mac_tx_buffers[buffer].isReserved=TRUE;
}

////////////////////////////////////////////////////////////////////////////
//
// NICDiscardTx(int8 buffer)
//
// Marks the transmit buffer as available, and if it was reserved will
// also un-reserve that buffer.  You must use the buffer that was
// returned from NICGetFreeTxBuffer() or NICGetTxBuffer()
//
////////////////////////////////////////////////////////////////////////////
void NICDiscardTx(int8 buffer) {
   __mac_tx_buffers[buffer].isReserved=FALSE;
   __mac_tx_buffers[buffer].isActive=FALSE;
}

////////////////////////////////////////////////////////////////////////////
//
// NICPutHeader(ETHERNET_HEADER *header)
//
// Puts the specified ETHERNET_HEADER into the current transmit buffer.
// Also sets the control byte (that is placed before the ethernet packet).
// Subsequent calls to NICPut() and NICPutArray() can be used to stuff
// data into the transmit buffer.  You can use NICSetTxBuffer() to randomly
// place data into the transmit buffer.
//
// NOTE: Assumes that header.type is in big endian
//
////////////////////////////////////////////////////////////////////////////
void NICPutHeader(ETHERNET_HEADER *header) {
   debug_printf("\r\nPUT ETHERNET ");
   debug_print_eth_header(header);
   __NICSetTxBuffer(NICCurrentTxBuffer,0);
   NICPut(0);  //set control byte
   NICPutArray(header,sizeof(ETHERNET_HEADER));
}

////////////////////////////////////////////////////////////////////////////
//
// NICFlush(int16 len)
//
// Marks the current buffer as ready for transmit.  Length is the number
// of bytes of data in the ethernet data.  Doesn't include the ethernet header,
// the one control byte, or the 7 status byte footer.
//
// NOTE: assumes that the first transmit buffer starts at location 0
//
////////////////////////////////////////////////////////////////////////////
void NICFlush(int16 len) {
   int16 debugi;
   char debugc;
   int16 ETXST;
   int16 ETXND;

   ETXST=ENC_MAC_TX_BUFFER_START(NICCurrentTxBuffer);
   ETXND=ETXST+len+sizeof(ETHERNET_HEADER);  //point to the last byte

   //debug
   debug_printf("\r\nMAC FLUSH LEN=%LU ETXST=0x%LX ETXND=0x%LX ", len, ETXST, ETXND);

   //debug
  #if DO_DEBUG
   __NICSetTxBuffer(NICCurrentTxBuffer,0);
   debug_printf("\r\nNIC PACKET REVIEW:\r\n");
   for (debugi=ETXST;debugi<=ETXND;debugi++) {
      debugc=NICGet();
      debug_printf("%X ", debugc);
   }
   debug_printf("\r\n");
  #endif
   enc_mac_write_control_word(ENC_MAC_ETXSTL, ETXST);
   enc_mac_write_control_word(ENC_MAC_ETXNDL, ETXND);

   //debug
   ETXST=enc_mac_read_control_word(ENC_MAC_ETXSTL,0);
   ETXND=enc_mac_read_control_word(ENC_MAC_ETXNDL,0);
   //debug_printf("ETXST=0x%LX ETXND=0x%LX", ETXST, ETXND);

   __mac_tx_buffers[NICCurrentTxBuffer].isFlushing=TRUE;

   enc_mac_control_bit_clear(ENC_MAC_EIR, 3);
   enc_mac_control_bit_set(ENC_MAC_ECON1, 3);

   printf("\r\nNIC: Flushed");
}

void debug_display_mac(MAC_ADDR *mac) {
   debug_printf("%X:%X:%X:%X:%X:%X", mac->v[0], mac->v[1], mac->v[2],
      mac->v[3], mac->v[4], mac->v[5]
   );
}


void debug_display_tx_status(void) {
#if DO_DEBUG
   int16 location;
   int8 status[7];
   location=enc_mac_read_control_word(ENC_MAC_ETXNDL,0);
   location+=1;
   enc_mac_write_control_word(ENC_MAC_ERDPTL,location);
   NICGetArray(status,7);
   debug_printf("LOC=0x%LX TX_CNT=%LU 0x%X 0x%X TX_WIRE_CNT=%LU 0x%X", location,
      make16(status[1],status[0]), status[2], status[3],
      make16(status[5], status[4]), status[6]);
#endif
}


////////////////////////////////////////////////////////////////////////////
/*~*~*~ THE FOLLOWING FUNCTIONS ARE PORTS OF MICROCHIP'S REALTEK API ~*~*~*/
///  Therefore this driver should be a drop-in for the original Realtek  ///
/// driver for the Microchip TCP/IP stack.                               ///
////////////////////////////////////////////////////////////////////////////

#define MACGetArray     NICGetArray
#define MACGet          NICGet
#define MACPutArray     NICPutArray
#define MACPut          NICPut
#define MACSetRxBuffer(a)  NICSetRxBuffer(a+sizeof(ETHERNET_HEADER))
#define MACGetFreeRxSize   NICGetFreeRxSize
#define MACDiscardRx    NICDiscardRx
#define MACIsTxReady    !NICIsTransmitting
//#define MACIsTxReady    NICIsTxReady
#define MACIsLinked     NICIsLinked
#define MACDiscardTx    NICDiscardTx
#define MACSetTxBuffer(b,a)  NICSetTxBuffer(b,a+sizeof(ETHERNET_HEADER))
#define MACReserveTxBuffer NICReserveTxBuffer

int MACGetHeader(MAC_ADDR *remote, int8 *type) {
   ETHERNET_HEADER header;
   int16 debugi;

   if (NICGetHeader(&header)) {
      if (header.type==0x0800)
         *type=MAC_IP;
      else if (header.type==0x0806)
         *type=MAC_ARP;
      else {
         *type=MAC_UNKNOWN;
         NICDiscardRx();
         return(FALSE);
      }

      memcpy(remote, &header.source, 6);

    #IF DO_DEBUG
      debug_printf("\r\n--- RX\r\n");
      //debug_printf("\r\nSRC: ");
      //debug_display_mac(remote);
      //debug_printf(" PROT:%X\r\n", *type);
      NICSetRxBuffer(0);
      for(debugi=0;debugi<__last_enc_header.len;debugi++) {
         debug_printf("%X ", NICGet());
      }
      debug_printf("\r\n---");
    #endif

      MACSetRxBuffer(0);  //before returning, read/write pointer should point to first byte of payload
      return(TRUE);
   }
   return(FALSE);
}

void MACFlush(void) {
   NICFlush(__mac_tx_buffers[NICCurrentTxBuffer].len);
}

void MACPutHeader(MAC_ADDR *remote, int8 type, int16 len) {
   ETHERNET_HEADER header;

   memcpy(&header.dest, remote, sizeof(MAC_ADDR));

   if (type==MAC_IP)
      header.type=0x0008;
   else if (type==MAC_ARP)
      header.type=0x0608;
   else
      return;

   header.source.v[0]=MY_MAC_BYTE1;
   header.source.v[1]=MY_MAC_BYTE2;
   header.source.v[2]=MY_MAC_BYTE3;
   header.source.v[3]=MY_MAC_BYTE4;
   header.source.v[4]=MY_MAC_BYTE5;
   header.source.v[5]=MY_MAC_BYTE6;

   NICPutHeader(&header);

   __mac_tx_buffers[NICCurrentTxBuffer].len=len;
}

void MACInit(void) {
    mac_spi_init();
    mac_reg_init();
}

