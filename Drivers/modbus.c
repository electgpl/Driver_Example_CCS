//////////////////////////////////////////////////////////////////////////////////////////
////                                     modbus.c                                     ////
////                                                                                  ////
////           MODBUS protocol driver for serial and TCP/IP communications.           ////
////                                                                                  ////
////  Refer to documentation at http://www.modbus.org for more information on MODBUS. ////
////                                                                                  ////
//////////////////////////////////////////////////////////////////////////////////////////
////                                                                                  ////
////  How to Use:                                                                     ////
////                                                                                  ////
////  Include just this file, modbus.c, in your main program. Before including this   ////
////  file define the constants below for your needs.                                 ////
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
////                (C) Copyright 1996, 2013 Custom Computer Services                 ////
////        This source code may only be used by licensed users of the CCS            ////
////        C compiler.  This source code may only be distributed to other            ////
////        licensed users of the CCS C compiler.  No other use,                      ////
////        reproduction or distribution is permitted without written                 ////
////        permission.  Derivative programs created using this software              ////
////        in object code form are not restricted in any way.                        ////
//////////////////////////////////////////////////////////////////////////////////////////

#include <modbus.h>

#if (MODBUS_PROTOCOL == MODBUS_PROTOCOL_SERIAL)
 #if (MODBUS_SERIAL_TYPE == MODBUS_RTU)
  #include <modbus_phy_layer_rtu.c>
 #elif (MODBUS_SERIAL_TYPE == MODBUS_ASCII)
  #include <modbus_phy_layer_ascii.c>
 #endif
#else
 #include <modbus_phy_layer_tcpip.c>
#endif

#if (MODBUS_PROTOCOL == MODBUS_PROTOCOL_TCPIP)
 #include <modbus_app_layer_tcpip.c>
#else
 #include <modbus_app_layer.c>
#endif

