//////////////////////////////////////////////////////////////////////////////////////////
////                                     modbus.h                                     ////
////                                                                                  ////
////           MODBUS protocol driver for serial and TCP/IP communications.           ////
////                                                                                  ////
////  Refer to documentation at http://www.modbus.org for more information on MODBUS. ////
////                                                                                  ////
//////////////////////////////////////////////////////////////////////////////////////////
////                                                                                  ////
//// DEFINES:                                                                         ////
////  MODBUS_PROTOCOL               MODBUS_PROTOCOL_SERIAL or MODBUS_PROTOCOL_TCPIP   ////
////                                                                                  ////
//// SERIAL DEFINES:                                                                  ////
////  MODBUS_TYPE                   MODBUS_TYPE_MASTER or MODBUS_TYPE_SLAVE           ////
////  MODBUS_SERIAL_INT_SOURCE      Source of interrupts                              ////
////                                (MODBUS_INT_EXT,MODBUS_INT_RDA,MODBUS_INT_RDA2,   ////
////                                   MODBUS_INT_RDA3,MODBUS_INT_RDA4)               ////
////  MODBUS_SERIAL_TYPE            MODBUS_RTU or MODBUS_ASCII                        ////
////  MODBUS_SERIAL_BAUD            Valid baud rate for serial                        ////
////  MODBUS_SERIAL_RX_PIN          Valid pin for serial receive                      ////
////  MODBUS_SERIAL_TX_PIN          Valid pin for serial transmit                     ////
////  MODBUS_SERIAL_ENABLE_PIN      Valid pin for serial enable, rs485 only           ////
////  MODBUS_SERIAL_RX_ENABLE       Valid pin for serial rcv enable, rs485 only       ////
////  MODBUS_SERAIL_RX_BUFFER_SIZE  Size of the receive buffer                        ////
////                                                                                  ////
//// TCP/IP DEFINES:                                                                  ////
////  MODBUS_TYPE                   MODBYS_TYPE_CLIENT or MODBUS_TYPE_SERVER          ////
////  MODBUS_SERVER_LISTEN_PORT     Port server/client listens/sends messages to      ////
////                                   (default 502)                                  ////
////  MODBUS_LISTEN_SOCKETS         Number of sockets server listens to (default 1)   ////
////  MODBUS_BUFFER_SIZE            Size of receive and transmit buffers (default 64) ////
////  MODBUS_SERVER_TIMEOUT         Time is seconds client will wait for response     ////
////                                   from server before setting TIMEOUT exception   ////
////                                   (default 5)                                    ////
////  MODBUS_SERVER_ADDR_0          TCP/IP address client connects to MSB             ////
////  MODBUS_SERVER_ADDR_1          TCP/IP address client connects to                 ////
////  MODBUS_SERVER_ADDR_2          TCP/IP address client connects to                 ////
////  MODBUS_SERVER_ADDR_3          TCP/IP address client connects to LSB             ////
////                                                                                  ////
//////////////////////////////////////////////////////////////////////////////////////////
////                                                                                  ////
//// Revision history:                                                                ////
////  May 8, 2009       Made PCD Compatible                                           ////
////  August 21, 2009   Added Modbus ASCII protocol                                   ////
////  May 20, 2010      Changed variables to unsigned for PCD and if #device ANSI is  ////
////                    used for PCM or PCH. Fixed bug when multiple UARTS are used   ////
////                    on PIC.                                                       ////
////  July 20, 2011     Seperated modbus.c into 7 files, including this one. The code ////
////                    was seperated into header and c files for the Physical and    ////
////                    Application layers, and RTU code and ASCII code.              ////
////  November 1, 2011  Added Modbus TCP/IP protocol.                                 ////
////  Janurary 9, 2013  Added support for Even and Odd Parity for RTU and ASCII       ////
////                    modes.                                                        ////
////                                                                                  ////
//////////////////////////////////////////////////////////////////////////////////////////
////                (C) Copyright 1996, 2010 Custom Computer Services                 ////
////        This source code may only be used by licensed users of the CCS            ////
////        C compiler.  This source code may only be distributed to other            ////
////        licensed users of the CCS C compiler.  No other use,                      ////
////        reproduction or distribution is permitted without written                 ////
////        permission.  Derivative programs created using this software              ////
////        in object code form are not restricted in any way.                        ////
//////////////////////////////////////////////////////////////////////////////////////////

#ifndef MODBUS_H
#define MODBUS_H

/*Some defines so we can use identifiers to set things up*/
#define MODBUS_PROTOCOL_SERIAL 0
#define MODBUS_PROTOCOL_TCPIP  100
#define MODBUS_TYPE_MASTER 99999
#define MODBUS_TYPE_SLAVE  88888
#define MODBUS_TYPE_CLIENT 20
#define MODBUS_TYPE_SERVER 21
#define MODBUS_INT_RDA     77777
#define MODBUS_INT_RDA2    66666
#define MODBUS_INT_RDA3    44444
#define MODBUS_INT_RDA4    33333
#define MODBUS_INT_EXT     55555
#define MODBUS_RTU         1
#define MODBUS_ASCII       2
#define MODBUS_TIMER_NOISR 13
#define MODBUS_TIMER_ISR   12
#define MODBUS_TIMER_T1    14
#define MODBUS_TIMER_T2    15

#ifndef MODBUS_PROTOCOL
 #define MODBUS_PROTOCOL MODBUS_PROTOCOL_SERIAL
#endif

#if (MODBUS_PROTOCOL == MODBUS_PROTOCOL_SERIAL)
 #ifndef MODBUS_TYPE
  #define MODBUS_TYPE MODBUS_TYPE_MASTER
 #endif

 #ifndef MODBUS_SERIAL_TYPE
  #define MODBUS_SERIAL_TYPE MODBUS_RTU
 #endif

 #ifndef MODBUS_SERIAL_INT_SOURCE
  #define MODBUS_SERIAL_INT_SOURCE MODBUS_INT_RDA   // Select between external interrupt
 #endif                                             // or asynchronous serial interrupt

 #ifndef MODBUS_SERIAL_BAUD
  #define MODBUS_SERIAL_BAUD 9600
 #endif

 #ifndef MODBUS_SERIAL_RX_PIN
  #define MODBUS_SERIAL_RX_PIN       PIN_C7   // Data receive pin
 #endif

 #ifndef MODBUS_SERIAL_TX_PIN
  #define MODBUS_SERIAL_TX_PIN       PIN_C6   // Data transmit pin
 #endif

 #ifndef MODBUS_SERIAL_ENABLE_PIN
  #define MODBUS_SERIAL_ENABLE_PIN   0   // Controls DE pin.  RX low, TX high.
 #endif

 #ifndef MODBUS_SERIAL_RX_ENABLE
  #define MODBUS_SERIAL_RX_ENABLE    0   // Controls RE pin.  Should keep low.
 #endif

 #ifndef MODBUS_PARITY
  #define MODBUS_PARITY "EVEN"
 #endif

 #ifndef MODBUS_SERIAL_TIMEOUT
  #if (MODBUS_SERIAL_TYPE == MODBUS_ASCII)
   #define MODBUS_SERIAL_TIMEOUT    1000000
  #else
   #define MODBUS_SERIAL_TIMEOUT      10000     //in us
  #endif
 #endif

 #ifndef MODBUS_SERIAL_RX_BUFFER_SIZE
  #define MODBUS_SERIAL_RX_BUFFER_SIZE  64      //size of send/rcv buffer
 #endif

 #ifndef MODBUS_TIMER_UPDATE
  #define MODBUS_TIMER_UPDATE MODBUS_TIMER_ISR
 #endif

 #ifndef MODBUS_TIMER_USED
  #define MODBUS_TIMER_USED MODBUS_TIMER_T1
 #endif

#else ///////////////////////// MODBUS TCP/IP PROTOCOL /////////////////////////
 #ifndef MODBUS_TYPE
  #define MODBUS_TYPE   MODBUS_TYPE_CLIENT
 #endif

 #ifndef MODBUS_SERVER_LISTEN_PORT
  #define MODBUS_SERVER_LISTEN_PORT    502   //this is the default listen port for MODBUS TCP/IP protocol
 #endif

 #if (MODBUS_TYPE == MODBUS_TYPE_SERVER)
  #ifndef MODBUS_LISTEN_SOCKETS
   #define MODBUS_LISTEN_SOCKETS     1
  #endif
 #endif

 #ifndef MODBUS_BUFFER_SIZE
  #define MODBUS_BUFFER_SIZE   64
 #endif

 #ifndef MODBUS_SERVER_TIMEOUT
  #define MODBUS_SERVER_TIMEOUT  5  //time in seconds that client will wait for server response before timeout
 #endif

 #if (MODBUS_TYPE == MODBUS_TYPE_CLIENT)
  #ifndef MODBUS_SERVER_ADDR_0
   #define MODBUS_SERVER_ADDR_0  192
   #define MODBUS_SERVER_ADDR_1  168
   #define MODBUS_SERVER_ADDR_2  100
   #define MODBUS_SERVER_ADDR_3  140
  #endif
 #endif

 #ifndef debug_printf
  #define debug_printf(a,b,c,d,e,f,g,h,i,k,l,m,n,o,p,q,r,s,t,u)
 #endif

#endif

#if (MODBUS_PROTOCOL == MODBUS_PROTOCOL_TCPIP)
 #include <modbus_phy_layer_tcpip.h>
#else
 #include <modbus_phy_layer.h>
#endif

#if (MODBUS_PROTOCOL == MODBUS_PROTOCOL_TCPIP)
 #include <modbus_app_layer_tcpip.h>
#else
 #include <modbus_app_layer.h>
#endif

#endif //MODBUS_H
