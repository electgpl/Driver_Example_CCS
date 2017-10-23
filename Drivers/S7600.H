/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////   s7600.h - Driver for the Seiko iReady s7600-A TCP/IP Stack    ////
////                                                                 ////
//// This driver gives you all the necessary building blocks for     ////
//// developing TCP/IP clients or servers.  See the headers before   ////
//// each function to see how they work.  Also, CCS has provided two ////
//// example programs that use this driver:                          ////
////                                                                 ////
////   ex_email.c - A simple SMTP email client                       ////
////   ex_websv.c - A simple HTTP web server                         ////
////   ex_dnslookup.c - A simple DNS resolver client                 ////
////                                                                 ////
//// Many of the user configurable values, such as port locations    ////
//// and timeout values, are located at the beginning of this file.  ////
//// If you are using CCS's PICnet development board you do not need ////
//// to edit any of these values.                                    ////
////                                                                 ////
//// If your ISP does not use PAP than you may have to edit the      ////
//// login script in ppp_connect so it logs in properly.             ////
////                                                                 ////
//// There are some bugs with the s7600 that you should be aware of. ////
//// First, the s7600 cannot handle padded PPP packets.  Recently    ////
//// some ISPs have started using equipment that pad PPP packets     ////
//// until each packet hits a certain size of bytes.  This breaks    ////
//// the internal checksum mechanism of the s7600 and cannot be fixed////
//// in firmware.  Another bug is that the s7600 cannot handle URG   ////
//// packets.  URG packets are most commonly used in the Telnet      ////
//// protocol.  You can design a pseudo telnet client/server if you  ////
//// use RAW protocol (some Telnet clients will let you specify RAW  ////
//// protocol instead of Telnet).  Both of these bugs are fixed in   ////
//// the rarer and more expensive s7601.  This driver should work    ////
//// in the s7601 if used in s7600 emulation mode, but has not been  ////
//// tested.                                                         ////
////                                                                 ////
//// Something else to be aware of, which is not a bug, is that many ////
//// ISPs these days do not allow low speed modem connections,       ////
//// rendering all those inexpesive and enticing 2400bps modem       ////
//// modules obsolete.                                               ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// UPDATES:                                                        ////
////                                                                 ////
////    August 19th, 2003:  Some PAP Fixes                           ////
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

//change the following lines to specify which port is connect to the s7600 address data/port
//this is set to correspond with CCS's picnet board (port D)
#ifndef s7600_busout
   #define  s7600_busout(x)      output_d(x)
   #define  s7600_busin(x)       input_d(x)
   #define  s7600_busfloat()     set_tris_d(0xFF)
   #define  s7600_buscontrol()   set_tris_d(0)
#endif

//change these defines to specify which pins are connected to your parts
//these are set to correspond with CCS's picnet board
#ifndef s7600_cs
   #define  s7600_cs          PIN_B1
   #define  s7600_rs          PIN_B2
   #define  s7600_readx       PIN_B3
   #define  s7600_writex      PIN_B4
   #define  s7600_busyx       PIN_C4
   #define  s7600_resetx      PIN_C5
   #define  modem_reset       PIN_B5
   #define  modem_dcd         PIN_C3
#endif

//do not change these values, you must design your board this way if you want to have the s7600 interrupt the PIC,
// and you must leave it this way if you want the PWM to generate a clock for the s7600.
//CCS code doesn't enable s7600 interrupts, it uses a polling method on the s7600.  Therefore B0 is free, but still defined
#ifndef s7600_int1
   #define  s7600_int1        PIN_B0   //set to b0 to use int_ext to get interrupts from s7600
#endif
#ifndef s7600_clk
   #define  s7600_clk         PIN_C2   //set to c2 to use pwm to generate clock source for s7600
#endif

//set this to true if you want to send debug information out the rs232 port
#define  DO_DEBUG          TRUE

#ifndef debug
   #if DO_DEBUG
      #define  debug printf
   #else
      #define  debug
   #endif
#endif

//set true if your ISP uses PAP authentication to authenticate logins.
#ifndef  USE_PAP
#define  USE_PAP           FALSE
#endif

//set to true if your modem is connected to the s7600 with rts/cts hardware flow control
//this is set to correspond with CCS's picnet board. (no hwfc)
#define  USE_HWFC          FALSE

//this is your modem init string, sent before a dial
#if USE_HWFC
  #define  MODEM_INIT        "ATM1L3"  //speaker on, volume high
#else
  #define  MODEM_INIT        "ATM1L3&K0"  //speaker on, volume high, no hw flow control
#endif

//if you are running a board different than CCS's PICnet then you need to either recalculate
// the timing done in s7600_init_clock() or define these values correctly and let the mcu calculate
// timing in real time.
//if you decide to calculate in real time (which will eat up ROM in floating point math) then
// CLOCK_FREQ is the speed of the mcu, and s7600_CLOCK_FREQ is the clock speed of the s7600
#define  CLOCK_FREQ        19660000
#define  s7600_CLOCK_FREQ  702143

//just in case future s76xx  have more than 2 sockets, if so specify how many here
#define  MAX_SOCKET       2

//these define a few timeout and timing values, in ms
#define REDIAL_DELAY             1000
#define MODEM_CONNECT_RETRY      5
#define MODEM_RESPONSE_TIMEOUT   2000
#define MODEM_CONNECT_TIMEOUT    60000 //time for two modems to connect and handshake
#define PPP_DISCONNECT_TIMEOUT   500
#define PPP_RESET_TIMEOUT        5000  //time to wait for PPP peripheral to reset
#define PPP_CONNECT_TIMEOUT      30000 //time to wait for PPP authentication
#define TCP_TIMEOUT              20
#define TCP_CONNECT_TIME         5000

/*!*!*!*! END USER CONFIGURATION !*!*!*!*/
typedef int32 IPAddr;
IPAddr MyIPAddr;     //global variable that contains your IP address

enum s7600_ec {
   BAD,                    OK,                     TIMEOUT,                PPP_FROZEN,       //0-3
   NO_MODEM_RESPONSE,      NO_CARRIER,             NO_FREE_SOCKET,         INVALID_SOCKET,   //4-7
   SOCKET_SND_BUSY,        SOCKET_NOT_CONNECTED,   SOCKET_NOT_ACTIVATED,   NO_BUFFER_SPACE,  //8-11
   SOCKET_CLOSE_TIMEOUT,   PPP_NOT_CONNECTED,      NO_PPP_DISCONNECT,      MODEM_CONNECTED,  //12-15
   MODEM_BUSY_SIGNAL,      MODEM_NO_DIALTONE,      ID_FAIL,                INVALID_CS,       //16-19
   SERVER_ERROR,           DNS_FAIL                                                          //20
};


//high level function prototypes. functions for use in developing embedded internet applications
void     tcpip_init();
int8     tcp_connect(IPAddr ip, int16 portnum, int8 socket);
int8     tcp_find_free_socket(void);
void     tcp_listen(int8 socket, int16 port);
int8     tcp_getc(int8 socket);
int1     tcp_kbhit(int8 socket);
void     tcp_flush_socket(int8 socket);
int1     tcp_connected(int8 socket);
s7600_ec tcp_putc(int8 socket, int8 c);
s7600_ec tcp_putd(int8 socket, int8 * c, int16 len);
void     tcp_close(int8 socket);
int1     ppp_check(void);
s7600_ec ppp_connect(int8 * phone_number, int8 * username, int8 * password);
void     ppp_disconnect(void);
void     ppp_keepalive(void);

//mid level function prototypes; general purpose functions used by above high level functions
//void     modem_reset(void);
s7600_ec modem_connect(int8 * phone_number);
s7600_ec modem_disconnect(void);
s7600_ec modem_response(int16 to);
void     s7600_init(void);
void     s7600_free_sockets(void);
void     s7600_init_clock(void);
void     s7600_Serial_Write(int8 data);
int1     s7600_Serial_kbhit(void);
int8     s7600_Serial_Read(void);
void     s7600_Serial_Flush(void);
void     s7600_Socket_Write(int8 data);
int8     s7600_Socket_Read(void);

//low level function prototypes; lowest level functions used by above functions
void s7600_DMA_Write(int8 address, int8 data);
int8 s7600_DMA_Read(int8 address);

//These are register addresses on the s7600 chip
#define  IR_VERSION                    0x00
#define  IR_GENERAL                    0x01
#define  SOCKET_LOCATION_REG           0x02
#define  IR_PORT_STAT_REG              0x08
#define  IR_PORT_DATA_REG              0x0B
#define  IR_BAUD_DIVIDER               0x0C
#define  IR_OUR_IP                     0x10
#define  IR_CLOCK_DIV                  0x1C
#define  SOCKET_SELECT_REG             0x20
#define  SOCKET_TOS_REG                0x21
#define  SOCKET_STATUS_REG             0x22
#define  SOCKET_ACTIVATE_REG           0x24
#define  SOCKET_DATA_AVAILABLE         0x28
#define  SOCKET_DATA_REG               0x2E
#define  SOCKET_SEND_DATA              0x2E
#define  SOCKET_BUFFER_OUT_SIZE        0x30
#define  SOCKET_BUFFER_IN_SIZE         0x32
#define  SOCKET_URGENT_DATA_ADDR       0x34
#define  SOCKET_PEER_PORT              0x36
#define  SOCKET_OUR_PORT               0x38
#define  SOCKET_STATUS_HIGH            0x3A
#define  SOCKET_PEER_IP_ADDR           0x3C
#define  IR_PPP_STATUS_REG             0x60
#define  IR_PPP_MAX_RETRY              0x62
#define  IR_PPP_DATA                   0x63
#define  IR_PAP_STRING                 0x64
#define  IR_PPP_STATE                  0x6E

#separate
void display_error(char * string, s7600_ec error) {
debug("\r\n%s: ",string);
   switch(error) {
      case BAD:
         debug("GENERAL ERROR"); break;
      case OK:
         debug("OK"); break;
      case PPP_FROZEN:
         debug("PPP FROZEN");  break;
      case TIMEOUT:
         debug("TIMEOUT");  break;
      case NO_MODEM_RESPONSE:
         debug("NO MODEM REPSONSE"); break;
      case NO_CARRIER:
         debug("NO CARRIER"); break;
      case NO_FREE_SOCKET:
         debug("NO FREE SOCKET"); break;
      case INVALID_SOCKET:
         debug("INVALID SOCKET"); break;
      case SOCKET_SND_BUSY:
         debug("SOCKET SND BUSY"); break;
      case SOCKET_NOT_CONNECTED:
         debug("SOCKET NOT CONNECTED"); break;
      case SOCKET_NOT_ACTIVATED:
         debug("SOCKET NOT ACTIVATED"); break;
      case NO_BUFFER_SPACE:
         debug("NO BUFFER SPACE"); break;
      case SOCKET_CLOSE_TIMEOUT:
         debug("SOCKET CLOSE TIMEOUT"); break;
      case PPP_NOT_CONNECTED:
         debug("PPP NOT CONNECTED"); break;
      case NO_PPP_DISCONNECT:
         debug("NO PPP DISCONNECT"); break;
      case MODEM_CONNECTED:
         debug("MODEM CONNECTED OK"); break;
      case MODEM_BUSY_SIGNAL:
         debug("BUSY SIGNAL"); break;
      case MODEM_NO_DIALTONE:
         debug("NO DIALTONE"); break;
      case ID_FAIL:
         debug("ID FAIL"); break;
default:
         debug("OTHER %U",error); break;
   }
}

/********************************************************************
//
// function: tcpip_isr
//
// description: the tcpip_isr if you enable interrupts on the s7600.
//              CCS code has s7600 interrupts disabled by default,
//              so not only do will you have to enable them but develop this
//              ISR as well
//
// input: none
// output: none
// calls:
//
//*******************************************************************/
/*
#int_ext
void tcpip_isr()
{
   int8 old_socket;

   old_socket=s7600_DMA_Read(SOCKET_SELECT_REG);

   //TODO: ISR to check and handle s7600 interrupts

   s7600_DMA_Write(SOCKET_SELECT_REG, old_socket);
}
*/

/********************************************************************
//
// function: tcpip_init
//
// description: initializes the tcpip stack.  in this case, it initializes the s7600
//
// input: none
// output: MyIpAddr (global)
// calls: s7600_init();
//
//*******************************************************************/
void tcpip_init() {
   MyIPAddr=0;
   s7600_init();
}

/********************************************************************
//
// function: tcp_connect
//
// description: initializes a tcp data stream between your PIC and a specified host.
//              this starts a client connection.  if you want to be a server and listen
//              to people trying to connect to you then use tcp_listen().
//
// input: ip (ipaddress of host to connect to), portnum (port of host to connect to),
//        socket (which one of our sockets to attach to this connection)
// output: s7600_ec (enumerated type that details what our error was)
// calls: s7600_DMA_Write(),s7600_DMA_Read()
//
//*******************************************************************/
s7600_ec tcp_connect(IPAddr ip, int16 portnum, int8 socket)
{
   int8 tmp;
   int16 to=0;

   if (socket == 0xFF) {return(NO_FREE_SOCKET);}
   else if (socket > MAX_SOCKET) {return(INVALID_SOCKET);}

   s7600_DMA_Write(SOCKET_SELECT_REG, socket);

   while ( (to < TCP_TIMEOUT) && (s7600_DMA_Read(SOCKET_STATUS_HIGH) & 0x01) )     //ensure that SND_BUSY is 0 before reset
   {
      delay_ms(1);
      to++;
   }

   if (to == TCP_TIMEOUT) {return(SOCKET_SND_BUSY);}

   s7600_DMA_Write(SOCKET_STATUS_REG, 0x10);    //socket reset
   delay_ms(10);
   s7600_DMA_Write(SOCKET_STATUS_REG, 0x02);    //tcp client mode

   s7600_DMA_Write(SOCKET_PEER_IP_ADDR+0, make8(ip,0));  //lsb
   s7600_DMA_Write(SOCKET_PEER_IP_ADDR+1, make8(ip,1));
   s7600_DMA_Write(SOCKET_PEER_IP_ADDR+2, make8(ip,2));
   s7600_DMA_Write(SOCKET_PEER_IP_ADDR+3, make8(ip,3));  //msb

   s7600_DMA_Write(SOCKET_PEER_PORT+0, make8(portnum,0));  //lsb
   s7600_DMA_Write(SOCKET_PEER_PORT+1, make8(portnum,1));  //msb

   //if you were to enable interrupts for this socket, you would do it here

   s7600_DMA_Write(SOCKET_ACTIVATE_REG, s7600_DMA_Read(SOCKET_ACTIVATE_REG) | (0x01 << socket) );  //activate this socket

   to=TCP_CONNECT_TIME;
   tmp=0;
   while (to && !(tmp & 0x10)) {
      delay_ms(1);
      to--;
      tmp=s7600_DMA_Read(SOCKET_STATUS_REG+1);
   }

   if ( tmp & 0x10 ) {return(OK);}
   else {
      s7600_DMA_Write(SOCKET_ACTIVATE_REG, s7600_DMA_Read(SOCKET_ACTIVATE_REG) & ~(0x01 << socket) );
      return(SOCKET_NOT_ACTIVATED);
   }
}

/********************************************************************
//
// function: tcp_listen
//
// description: opens a server mode tcp connection to the socket and port.
//              after a socket is opened as a server you need to use tcp_connectd() to
//              see if someone is trying to connect with you.
//
// input: socket (socket to use), port (port to listen on)
// output: s7600_ec (enumerated type that details what our error was)
// calls: s7600_DMA_Read(), s7600_DMA_Write()
//
//*******************************************************************/
s7600_ec tcp_listen(int8 socket, int16 port)
{
   int16 to=0;

   if (socket == 0xFF) {return(NO_FREE_SOCKET);}
   else if (socket > MAX_SOCKET) {return(INVALID_SOCKET);}

   s7600_DMA_Write(SOCKET_SELECT_REG, socket);

   to=TCP_TIMEOUT;
   while ((s7600_DMA_Read(SOCKET_STATUS_HIGH) & 0x01) && to)
   {
      to--;
      delay_ms(1);
   }
   if (!to) {
      return(SOCKET_SND_BUSY);
   }

   s7600_DMA_Write(SOCKET_STATUS_REG, 0x10);           //SOCKET_MASK_RESET
   delay_ms(10);
   s7600_DMA_Write(SOCKET_STATUS_REG, 0x06);           //tcp server mode

   s7600_DMA_Write(SOCKET_OUR_PORT+0, make8(port,0));  //lsb
   s7600_DMA_Write(SOCKET_OUR_PORT+1, make8(port,1));  //msb

   s7600_DMA_Write(SOCKET_ACTIVATE_REG, s7600_DMA_Read(SOCKET_ACTIVATE_REG) | (0x01 << socket) );   //activate socket

   return(OK);
}

/********************************************************************
//
// function: tcp_getc
//
// description: works very much like the standard C getc(), except gets data from the TCP/IP
//              stack and not rs232.  NOTE: this performs no checks as to whether the socket is valid
//              or the socket is connected.  NOTE: also, this will wait indefinately until data arrives,
//              just like getc().  using tcp_kbhit() will check for valid socket, if it is connected, and will
//              not wait indefinately.  just like kbhit().
//
// input: socket (which socket to get data from)
// output: a character received from the tcpip stack, sent by the host to the PIC
// calls: s7600_DMA_Read(), s7600_DMA_Write()
//
//*******************************************************************/
int8 tcp_getc(int8 socket)
{
   int8 c,status;

   s7600_DMA_Write(SOCKET_SELECT_REG, socket);

   do {
      status=s7600_DMA_Read(SOCKET_STATUS_REG);
   } while (!(status & 0x10));

   c=s7600_DMA_Read(SOCKET_DATA_REG);

   return(c);
}

typedef struct {
   int1 to;
   char c;
} tcp_getc_data;

tcp_getc_data tcp_getc_timeout(int8 socket, int16 to) {
   tcp_getc_data ret;

   ret.to=1;

   while (to) {
      if (tcp_kbhit(socket)) {
         ret.to=0;
         ret.c=tcp_getc(socket);
         return(ret);
      }
      delay_ms(1);
      to--;
   }
   return(ret);
}

/********************************************************************
//
// function: tcp_connected
//
// description: if a socket has been opened into server mode, this will return true
//              if a host is trying to connect to our PIC.
//
// input: socket (socket which to check if there is a server connection)
// output: boolean; true if connected, false if not
// calls: s7600_DMA_Write(), s7600_DMA_Read()
//
//*******************************************************************/
int1 tcp_connected(int8 socket)
{
   int8 status;
   int1 rc;

   s7600_DMA_Write(SOCKET_SELECT_REG, socket);

   status=s7600_DMA_Read(SOCKET_STATUS_REG+1);

   if ((status & 0x10) && (!(status & 0x20)) && (!(status & 0x40)))
      rc=1;
   else
      rc=0;

   return(rc);
}

/********************************************************************
//
// function: tcp_getd
//
// description: same as tcp_getc(), but an extra parameter has been added to
//              read a variable amount of data from the tcpip receive buffer.
//              NOTE: this performs no checks as to whether the socket is valid
//              or the socket is connected.  NOTE: also, this will wait indefinately until
//              data arrives, just like getc().  using tcp_kbhit() will check for
//              valid socket, if it is connected, and will not wait indefinately.  just like kbhit().
//
//              NOTE: if less data than len is in buffer it will only pull that data, will not wait until len is filled
//
// input: socket (socket which to get data from), data (pointer to place to store received characters),
//        len (length of data to pull from the buffer).
// output: contents pointed to data modified.
// calls: s7600_DMA_Write(), s7600_DMA_Read()
//
//*******************************************************************/
void tcp_getd(int8 socket, int8 * data, int16 len)
{
   int8 status;
   int16 size=0;

   s7600_DMA_Write(SOCKET_SELECT_REG, socket);

   do {
      status=s7600_DMA_Read(SOCKET_STATUS_REG);
   } while (!(status & 0x10));

   size = s7600_DMA_Read(SOCKET_BUFFER_IN_SIZE + 1);
   size <<= 8;
   size += s7600_DMA_Read(SOCKET_BUFFER_IN_SIZE + 0);
   if( size > len )
      size = len;

   while (len--) {
      *data=s7600_DMA_Read(SOCKET_DATA_REG);
      data++;
   }
}

/********************************************************************
//
// function: tcp_flush_socket
//
// description: clears all the data from the tcpip receive buffer to make
//              room for new data
//
// input: socket (socket which buffer to clear)
// output: none
// calls: tcp_kbhit(), tcp_getc()
//
//*******************************************************************/
void tcp_flush_socket(int8 socket) {
   while (tcp_kbhit(socket)) {
      tcp_getc(socket);
   }
}

/********************************************************************
//
// function: tcp_kbhit
//
// description: checks to see if there is data in the receive buffer of the tcpip stack
//
// input: socket (socket which to check)
// output: boolean; true if valid data available for this socket, false if not
// calls: s7600_DMA_Read(), s7600_DMA_Write()
//
//*******************************************************************/
int1 tcp_kbhit(int8 socket)
{
   int8 status,tstatus;
   int1 rc=0;

   if (socket < MAX_SOCKET) {

      s7600_DMA_Write(SOCKET_SELECT_REG, socket);

      status=s7600_DMA_Read(SOCKET_STATUS_REG);
      tstatus=s7600_DMA_Read(SOCKET_STATUS_REG + 1) & 0x0F;

      //readable states are ESTABLISHED, CLOSE_WAIT, FIN_WAIT1 and FIN_WAIT2
      if ( ( (tstatus == 0x02) || (tstatus == 0x03) || (tstatus == 0x05) || (tstatus == 0x06) ) && ( status & 0x10 ) )
         rc=1; //data available
   }

   return(rc);
}

/********************************************************************
//
// function: tcp_putc
//
// description: puts out a character on the tcpip stack, PIC -> host.  just like putc()
//
// input: socket (socket which to put data out), c (character to send)
// output: s7600_ec (error code type which gives a reason for error)
// calls: s7600_DMA_Write(), s7600_DMA_Read()
//
//*******************************************************************/
s7600_ec tcp_putc(int8 socket, int8 c)
{
   int8 status;
   int16 space;

   if (socket > MAX_SOCKET) {return(INVALID_SOCKET);} //fail if an invalid socket

   s7600_DMA_Write(SOCKET_SELECT_REG,socket);     //address to proper socket

   status=s7600_DMA_Read(SOCKET_STATUS_REG+1) & 0x0F; //0x23
   if ( (status != 0x02) && (status != 0x03) ) {return(SOCKET_NOT_CONNECTED);}  //connection not established or socket closed

   //find space left on the sucket buffer
   space = s7600_DMA_Read(SOCKET_BUFFER_OUT_SIZE+1);
   space <<= 8;
   space += s7600_DMA_Read(SOCKET_BUFFER_OUT_SIZE);

   if (!space) {return(NO_BUFFER_SPACE);} //not enough space to send a char
   else {
      s7600_DMA_Write(SOCKET_DATA_REG, c);
      s7600_DMA_Write(SOCKET_BUFFER_OUT_SIZE, 0);
      return(OK);
   }
}

/********************************************************************
//
// function: tcp_putd
//
// description: puts out data onto the tcpip stack, pic -> host, like tcp_putc but has
//              this sends an array of data instead of just one byte.
//
// input: socket (socket which to put data out), c (pointer to array of char),
//        len (number of bytes to send)
// output: s7600_ec (error code type which gives a reason for error)
// calls: s7600_DMA_Write(), s7600_DMA_Read()
//
//*******************************************************************/
s7600_ec tcp_putd(int8 socket, int8 * c, int16 len)
{
   int8 status;
   int16 space;

   if (socket > MAX_SOCKET) {return(INVALID_SOCKET);} //fail if an invalid socket

   //address to proper socket
   s7600_DMA_Write(SOCKET_SELECT_REG,socket);

   status=s7600_DMA_Read(SOCKET_STATUS_REG+1) & 0x0F; //0x23
   if ( (status != 0x02) && (status != 0x03) ) {return(SOCKET_NOT_CONNECTED);}  //connection not established or socket closed

   //find space left on the sucket buffer
   space = s7600_DMA_Read(SOCKET_BUFFER_OUT_SIZE+1);
   space <<= 8;
   space += s7600_DMA_Read(SOCKET_BUFFER_OUT_SIZE);

   if (space < len) {return(NO_BUFFER_SPACE);} //not enough space to send a char
   else {
      //        debug("\r\nTCP_PUTD: ");
      while (len) {
         s7600_DMA_Write(SOCKET_DATA_REG, *c);
         //            debug("%c",*c);
         c++;
         len--;
      }
      s7600_DMA_Write(SOCKET_BUFFER_OUT_SIZE, 0);
      return(OK);
   }
}

/********************************************************************
//
// function: tcp_close
//
// description: closes a connection between PIC and Host.
//
// input: socket (socket to close)
// output: s7600_ec (error code type which gives a reason for error)
// calls: s7600_DMA_Write(), s7600_DMA_Read()
//
//*******************************************************************/
s7600_ec tcp_close(int8 socket)
{
   short int closed=0;
   int16 to=0;
   int8 state;
   int1 a_close = 0;

   if (socket > MAX_SOCKET) {return(INVALID_SOCKET);} //invalid socket

   //address to proper socket
   s7600_DMA_Write(SOCKET_SELECT_REG,socket);


   while ( (to < TCP_TIMEOUT) && (!closed) ) {
      state = s7600_DMA_Read(SOCKET_STATUS_REG + 1) & 0x0F; //0x22 + 1
      if (a_close) {
         if ( (state == 5) || (state == 6) || (state == 7) || (state == 0) || (state == 8) || (state == 4) ) {return(OK);}
      }
      else if ( (state == 0) || (state == 9) || (state == 8) ) { //not connected, but disable socket anyways
         s7600_DMA_Write(SOCKET_ACTIVATE_REG, s7600_DMA_Read(SOCKET_ACTIVATE_REG) & ~(1 << socket) );
         return(OK);
      }
      else if ( (state == 0x0A) || (state == 1) || (state == 2) || (state == 3) ) {
         s7600_DMA_Write(SOCKET_ACTIVATE_REG, s7600_DMA_Read(SOCKET_ACTIVATE_REG) & ~(1 << socket) );
         a_close=1;
      }
      else if ( (state == 5) || (state == 6) || (state == 7) || (state == 4) ) {}

      delay_ms(1);
   }

   if ( (to) && (!closed) )  {return(SOCKET_CLOSE_TIMEOUT);}  //we timed out
}

/********************************************************************
//
// function: tcp_find_free_socket
//
// description: finds the next available free socket that can be opened, or 0xFF if none is available
//
// input: none
// output: the next free socket, or 0xFF if none is available
// calls: s7600_DMA_Read()
//
//*******************************************************************/
int8 tcp_find_free_socket(void) {
   int8 avail,i;
   int8 socket=1;

   avail=s7600_DMA_Read(SOCKET_ACTIVATE_REG); //bits are set if they are closed

   for (i=0;i<MAX_SOCKET;i++) {
      if (socket & ~avail) {return(i);} //open socket
      else {socket = socket << 1;}   //move mask
   }
   return(0xFF);                      //no open socket
}

/********************************************************************
//
// function: ppp_check
//
// description: determines if there currently is a PPP connection to the internet or not
//
// input: none
// output: boolean; true if PPP connection, false if not
// calls: s7600_DMA_Read()
//
//*******************************************************************/
int1 ppp_check(void)
{
   if (s7600_DMA_Read(IR_PPP_STATUS_REG) & 0x01)
      return(1);
   else
      return(0);
}

/********************************************************************
//
// function: ppp_connect
//
// description: makes a PPP connection to the internet.  dials the modem and logs into the ISP
//
// input: phone_number (an array of characters that has the phone number to dial, null terminated),
//        username (an array of characters that has the username to login with, null terminated),
//        password (an array of characters that has the password to login with, null terminated)
//        *NOTE* - Some ISPs need a capitol P in front of the username to start a PPP connection. example Pusername
// output: s7600_ec (error code type which gives a reason for error)
//         MyIPAddr, a global variable, is also modified
// calls: modem_connect(), s7600_DMA_Write(), s7600_DMA_Read()
//
//*******************************************************************/
s7600_ec ppp_connect(char * phone_number, char * username, char * password)
{
   int16 timeout;
   int8 tmp,ip1,ip2,ip3,ip4,retry;
   s7600_ec connect_status;
   char *c;

#if USE_HWFC
   s7600_DMA_Write(IR_PORT_STAT_REG,0x20); //regain control and config communication port
#else
   s7600_DMA_Write(IR_PORT_STAT_REG,0x00); //regain control and config communication port
#endif
   s7600_DMA_Write(IR_PPP_STATUS_REG, 0x01);  //IR_PPP_CONFIG_SW_RESET

   timeout=PPP_RESET_TIMEOUT;
   while (((s7600_DMA_Read(IR_PPP_STATUS_REG) & 0x01) == 0x01) && timeout) {
      timeout--;
      delay_ms(1);
   }
   if (!timeout) {return(PPP_FROZEN);}

   s7600_DMA_Write(IR_PPP_MAX_RETRY,0x0F);


   retry=MODEM_CONNECT_RETRY;
   while(retry != 0) {
      connect_status = modem_connect(phone_number);
      if (connect_status == MODEM_CONNECTED) {break;}
      else {delay_ms(REDIAL_DELAY);retry--;}
   }

   if (connect_status != MODEM_CONNECTED) {return(connect_status);}

#IF USE_PAP
   #IF USE_HWFC
      s7600_DMA_Write(IR_PORT_STAT_REG,0x21);//release control and config communication port.
   #ELSE
      s7600_DMA_Write(IR_PORT_STAT_REG,0x01);//release control and config communication port.
   #ENDIF

   s7600_DMA_Write(IR_PPP_STATUS_REG,1);
   s7600_DMA_Write(IR_PPP_STATUS_REG,0);

   s7600_DMA_Write(0x6f,0);   //not documented
   
   s7600_DMA_Write(IR_PPP_STATUS_REG,0x20);  //IR_PPP_CONFIG_PAP_ENBL

   tmp = strlen(username);
   if (0 == tmp) {return 0;}
   s7600_DMA_WRITE(IR_PAP_STRING, tmp);
   c=username;
   while (tmp--) {
      s7600_DMA_WRITE(IR_PAP_STRING,*c);
      c++;
   }

   tmp = strlen(password);
   s7600_DMA_WRITE(IR_PAP_STRING, tmp);
   c=password;
   while (tmp--) {
      s7600_DMA_WRITE(IR_PAP_STRING,*c);
      c++;
   }

   s7600_DMA_WRITE(IR_PAP_STRING,0);

   s7600_DMA_WRITE(IR_PPP_STATUS_REG,0x62); //start PPP negotiations   (0x6A turns on PPP interrupts, which seiko does in their code.)
#ELSE
   delay_ms(10000);
   printf(s7600_Serial_Write,"%s\n",username);
   delay_ms(3000);
   printf(s7600_Serial_Write,"%s\n",password);
   delay_ms(1000);
   #IF USE_HWFC
      s7600_DMA_Write(IR_PORT_STAT_REG,0x21);//release control and config communication port.
   #ELSE
      s7600_DMA_Write(IR_PORT_STAT_REG,0x01);//release control and config communication port.
   #ENDIF
   s7600_DMA_Write(IR_PPP_STATUS_REG,0x42); //start PPP negotiations   (0x4A turns on PPP interrupts, which seiko does in their code.)
#ENDIF

   timeout=PPP_CONNECT_TIMEOUT;
   while ( (timeout != 0) && ((s7600_DMA_Read(IR_PPP_STATUS_REG) & 0x01) != 0x01) ) {
      timeout--;
      delay_ms(1);
   }
   if (timeout == 0) {modem_disconnect(); return(TIMEOUT);}  //we didn't connect
   else {  //we connected
      ip1=s7600_DMA_Read(IR_OUR_IP+0);
      ip2=s7600_DMA_Read(IR_OUR_IP+1);
      ip3=s7600_DMA_Read(IR_OUR_IP+2);
      ip4=s7600_DMA_Read(IR_OUR_IP+3);
      MyIPAddr=make32(ip4,ip3,ip2,ip1);
      s7600_free_sockets();
      return(OK);
   }
}

/********************************************************************
//
// function: modem_connect
//
// description: dials a phone number and makes a modem connection. (does not establish a PPP connection)
//
// input: phone_number (an array of char that contains the phone number to dial, null terminated)
// output: s7600_ec (error code type which gives a reason for error)
// calls: s7600_Serial_Write(), modem_response(), s7600_DMA_Write(), s7600_DMA_Read()
//
//*******************************************************************/
s7600_ec modem_connect(char * phone_number)
{
   int8 status;

   status=s7600_DMA_Read(IR_PORT_STAT_REG);
   if ((status & 0x40) != 0x40) {return(NO_CARRIER);}

   s7600_Serial_Flush();

   printf(s7600_Serial_Write,"%s\r\n",MODEM_INIT);

   if (modem_response(MODEM_RESPONSE_TIMEOUT) == OK) {
      printf(s7600_Serial_Write,"ATDT%s\r\n",phone_number);
      return(modem_response(MODEM_CONNECT_TIMEOUT));
   }
   else {return(NO_MODEM_RESPONSE);}
}

/********************************************************************
//
// function: modem_response
//
// description: listens to and figures out the modem's AT response to AT commands we sent.
//
// input: to (time to wait, in ms, for a response from the modem)
// output: s7600_ec (error code type which gives a reason for error)
// calls: s7600_Serial_kbhit(), s7600_Serial_Read(), s7600_DMA_Read()
//
//*******************************************************************/
s7600_ec modem_response(int16 to)
{
   int1 done=0;
   int1 hit;
   int1 first=1;
   int1 crlf=0;
   int8 char1=0,char2=0;
   s7600_ec ret=BAD;

   while ((to != 0) && (!done)) {
      to--;
      hit=s7600_Serial_kbhit();
      if (hit && !crlf) {  //with local echo on wait until we hear the echo, which as after a crlf
         char1=s7600_Serial_Read();
         if ((char1 == 0x0D) || (char1 == 0x0A)) {crlf=1;}
      }
      else if (hit && first) {
         char1=s7600_Serial_Read();
         if ((char1 < 0x80) && (char1 > 0x1F)) {first=0;}
      }
      else if (hit && !first) {
         char2=s7600_Serial_Read();
         done=1;
      }
      else {delay_ms(1);}
   }

   if      ((done) && (char1 == 'O') && (char2 == 'K')) {ret=OK;}
   else if ((done) && (char1 == 'C') && (char2 == 'O')) {ret=MODEM_CONNECTED;}
   else if ((done) && (char1 == 'B') && (char2 == 'U')) {ret=MODEM_BUSY_SIGNAL;}
   else if ((done) && (char1 == 'N') && (char2 == 'O')) {ret=MODEM_NO_DIALTONE;}

   s7600_Serial_Flush();
   return(ret);
}


/********************************************************************
//
// function: modem_disconnect
//
// description: hangs up the modem
//
// input: none
// output: s7600_ec (error code type which gives a reason for error)
// calls: s7600_DMA_Write()
//
//*******************************************************************/
s7600_ec modem_disconnect(void)
{
#if USE_HWFC
   s7600_DMA_Write(IR_PORT_STAT_REG,0x20);//regain control and config communication port.
#else
   s7600_DMA_Write(IR_PORT_STAT_REG,0x00);//regain control and config communication port.
#endif
   delay_ms(100);
   printf(s7600_Serial_Write,"+++");
   delay_ms(5000);
   printf(s7600_Serial_Write,"ATH\r\n");
   //    return(modem_response(MODEM_RESPONSE_TIMEOUT+MODEM_RESPONSE_TIMEOUT));//wait double time
   return(OK);
}

/********************************************************************
//
// function: ppp_disconnect
//
// description: closes the PPP connection and then hangs up the modem. terminates TCPIP connection to internet
//
// input: none
// output: s7600_ec (error code type which gives a reason for error),
//         MyIPAddr, a global variable that has our IP address, cleared.
// calls: modem_disconnect(), s7600_DMA_Write(), s7600_DMA_Read()
//
//*******************************************************************/
s7600_ec ppp_disconnect(void)
{
   int8 tmp;
   int16 to=0;

   tmp = s7600_DMA_Read(IR_PPP_STATUS_REG);
   tmp &= ~0x03;   // Write only PPP_EN and RST bit low
   s7600_DMA_Write(IR_PPP_STATUS_REG, tmp);

   while( (s7600_DMA_Read(IR_PPP_STATE) != 0x02) && (to++ < PPP_DISCONNECT_TIMEOUT) ) {delay_ms(1);}

   if (s7600_DMA_Read(IR_PPP_STATE) == 0x02) {}
   else {return(NO_PPP_DISCONNECT);}

   s7600_DMA_Write(IR_PPP_STATUS_REG, 0); //disable PPP and reset

   modem_disconnect();

   s7600_DMA_Write(IR_GENERAL, 0x01); //resets the s7600

   MyIPAddr=0;

   return(OK);
}

/********************************************************************
//
// function: ppp_keepalive
//
// description: kicks the PPP connection to keep it alive
//
// input: none
// output: none
// calls: ppp_check(), s7600_DMA_Write(), s7600_DMA_Read()
//
//*******************************************************************/
void ppp_keepalive(void)
{
   int1 alive;
   int8 status;

   alive=ppp_check();

   if (!alive) {
      status=s7600_DMA_Read(IR_PPP_STATUS_REG);
      s7600_DMA_Write(IR_PPP_STATUS_REG, status | 0x04);
   }
}

/********************************************************************
//
// function: s7600_init
//
// description: initializes the s7600 to be used as a tcp/ip stack
//
// input: none
// output: none
// calls: s7600_init_clock(), s7600_serial_flush(), tcp_flush_socket(), s7600_DMA_Write(), s7600_DMA_Read()
//
//*******************************************************************/
void s7600_init() {
   int16 to;
   int i;

   output_high(s7600_resetx);
   output_low(s7600_resetx);
   delay_ms(100);
   output_high(s7600_resetx);
   delay_ms(100);

   delay_ms(100);
   output_low(modem_reset);
   delay_ms(100);
   output_high(modem_reset);

   s7600_init_clock();

   for (i=0;i<MAX_SOCKET;i++) {
      to=TCP_TIMEOUT;
      s7600_DMA_Write(SOCKET_SELECT_REG,i);
      while ((s7600_DMA_Read(SOCKET_STATUS_HIGH) & 0x01) && (to)) {delay_ms(1);to--;}  //if not snd_busy
      if (to)  {
         s7600_DMA_Write(SOCKET_STATUS_REG,0x10);  //reset
         tcp_flush_socket(i);
      }
   }
   s7600_Serial_Flush();

   s7600_free_sockets();

   debug("\r\ns7600 Version: %X",s7600_DMA_Read(IR_VERSION));
}

/********************************************************************
//
// function: s7600_init_clock
//
// description: intializes the PWM to generate the proper clock for the s7600, and
//              sets the s7600 clock and baud rate generator SFRs.
//
// input: none
// output: none
// calls: s7600_DMA_Write(), s7600_DMA_Read()
//
//*******************************************************************/
void s7600_init_clock(void) {
   int16 divide_count,brDivVal;
   int16 duty;

   // The cycle time will be (1/clock)*4*t2div*(period+1)
   // where setup_timer_2(T2div, period, 1);
   // In this program clock=CLOCK_FREQ
   // When CLOCK_FREQ = 19.66MHz, PWM should generate 702KHz.
   setup_ccp1(CCP_PWM);               // Configure CCP1 as a PWM
   setup_timer_2(T2_DIV_BY_1, 6, 1);

   // if you change your clock speeds then you need to use this to set your duty frequency.
   // of course, s7600_CLOCK_FREQ is determined by the above PWM calculation
   //duty=(int16)((float)CLOCK_FREQ * ((float)1/s7600_CLOCK_FREQ)/2 );  //find duty setting for 50%

   // if using the picnet board by CCS then this setting is fine
   duty=14;

   set_pwm1_duty(duty);               // 50% duty cycle

   // set IR_CLOCK_DIV register to tell it that what clock speed we're at
   // if you change your clock speeds then you need to use this to set your frequency divider.
   // of course, s7600_CLOCK_FREQ is determined by the above PWM calculation
   //divide_count=(int16)((float)(s7600_CLOCK_FREQ/1000)-1);

   // if using the picnet board by CCS then this setting is fine
   divide_count=701;

   s7600_DMA_Write(IR_CLOCK_DIV + 0, make8(divide_count,0));
   s7600_DMA_Write(IR_CLOCK_DIV + 1, make8(divide_count,1));

   //regain control of serial port
   s7600_DMA_Write(IR_PORT_STAT_REG,0x00);//regain control and config communication port. (0x00 for no hw control, 0x20 for yes)

   //if not using CCS board then the following calculation will work
   // brDivVal = baud_rate;
   // brDivVal = (int16)( ( (s7600_CLOCK_FREQ ) / brDivVal ) - 1 );

   //if using CCS board you must use this setting because solsis/pctel model locked at 115.2k
   brDivVal = 5;

   s7600_DMA_Write(IR_BAUD_DIVIDER, make8(brDivVal,0));
   s7600_DMA_Write(IR_BAUD_DIVIDER+1, make8(brDivVal,1));
}


/********************************************************************
//
// function: s7600_free_sockets
//
// description: makes sure all the sockets are disabled, so that they are free for later
//
// input: none
// output: none
// calls: s7600_DMA_Write()
//
//*******************************************************************/
void s7600_free_sockets(void) {
         s7600_DMA_Write(SOCKET_ACTIVATE_REG, 0);
}

/********************************************************************
//
// function: s7600_Serial_Write
//
// description: sends a byte of data out to the modem.  Same as putc(), but the destination is the modem
//
// input: data (data to send)
// output: none
// calls: s7600_DMA_Write()
//
//*******************************************************************/
void s7600_Serial_Write(char data)
{
   s7600_DMA_Write(IR_PORT_DATA_REG,data);  //send data to Serial_Port_Data address
   delay_ms(5);
}

/********************************************************************
//
// function: s7600_serial_kbhit
//
// description: checks to see if there is data in the modem -> s7600 data buffer
//
// input: none
// output: boolean; true if there is data available from modem, false if data is not available
// calls: s7600_DMA_Read()
//
//*******************************************************************/
int1 s7600_Serial_kbhit(void)
{
   int8 status;

   status=s7600_DMA_Read(IR_PORT_STAT_REG);

   if (status & 0x80) {return(1);}
   else {return(0);}
}

/********************************************************************
//
// function: s7600_serial_read
//
// description: gets data from the modem that was sent to the s7600, just like getc().
//              NOTE: this makes no checks as to whether the data is valid or not.  use s7600_serial_kbhit() first.
//
// input: none
// output: data from the modem
// calls: s7600_DMA_Read()
//
//*******************************************************************/
char s7600_Serial_Read(void)
{
   int8 data;

   data=s7600_DMA_Read(IR_PORT_DATA_REG);

   return(data);
}

/********************************************************************
//
// function: s7600_serial_flush
//
// description: clears the modem -> s7600 receive buffer
//
// input: none
// output: none
// calls: s7600_DMA_Read()
//
//*******************************************************************/
void s7600_Serial_Flush(void)
{
   while (s7600_Serial_kbhit()) {
      s7600_Serial_Read();
   }
}

/********************************************************************
//
// function: s7600_DMA_Write
//
// description: writes data to the s7600 using the bus between the PIC and the s7600
//
// input: address (s7600 SFR to write to), data (data to write)
// output: none
// calls: none
//
//*******************************************************************/
void s7600_DMA_Write(char address, char data)
{
   while(!input(s7600_busyx)) {}   // Check if S-7600A is busy

   disable_interrupts(GLOBAL);

   output_high(s7600_writex);
   output_high(s7600_readx);
   output_low(s7600_rs);
   output_high(s7600_cs);   // 1st cycle sets register address
   s7600_buscontrol();
   s7600_busout(address);
   output_low(s7600_writex);
   delay_cycles(1);
   output_high(s7600_writex);

   s7600_busfloat();

   output_low(s7600_cs);
   output_low(s7600_readx);
   output_high(s7600_rs);

   output_high(s7600_readx);
   output_high(s7600_cs);
   s7600_buscontrol();
   s7600_busout(data);
   output_low(s7600_writex);
   delay_cycles(1);
   output_high(s7600_writex);
   s7600_busfloat();
   output_low(s7600_cs);
   output_low(s7600_readx);

   while(!input(s7600_busyx)) {}   // Check if S-7600A is busy

   enable_interrupts(GLOBAL);
}

/********************************************************************
//
// function: s7600_DMA_Read
//
// description: reads data from the s7600 using the bus between the PIC and the s7600
//
// input: address (s7600 SFR to read)
// output: none
// calls: none
//
//*******************************************************************/
char s7600_DMA_Read(char address)
{
   char data;

   while(!input(s7600_busyx)) {}   // Check if S-7600A is busy

   disable_interrupts(GLOBAL);

   output_high(s7600_writex);
   output_high(s7600_readx);
   output_low(s7600_rs);
   output_high(s7600_cs);

   s7600_buscontrol();
   s7600_busout(address);  // Write address
   output_low(s7600_writex);
   delay_cycles(1);
   output_high(s7600_writex);

   s7600_busfloat();

   output_low(s7600_cs);
   output_high(s7600_rs);
   output_low(s7600_rs);
   output_high(s7600_cs);

   output_low(s7600_readx);
   data=s7600_busin();  //read back the address (odd interface, but whatever)
   output_high(s7600_readx);

   output_low(s7600_cs);
   output_high(s7600_rs);

   while(!input(s7600_busyx)) {}   // Check if S-7600A is busy

   output_high(s7600_cs);
   output_low(s7600_readx);
   data=s7600_busin();
   output_high(s7600_readx);

   output_low(s7600_cs);

   enable_interrupts(GLOBAL);

   return (data);
}
