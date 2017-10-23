/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////                            usb_cdc.h                            ////
////                                                                 ////
//// Library for adding a virtual COM port on your PC over USB using ////
//// the standard Communication Device Class (CDC) specification.    ////
//// Including this file in your code will add all USB code,         ////
//// interrupts, descriptors and handlers required.  No other        ////
//// modifications need to be made.                                  ////
////                                                                 ////
//// This library creates a virtual RS232 link between the PC and    ////
//// the PIC, therefore the library provided will be familiar to     ////
//// anyone with standard UART stream I/O:                           ////
////                                                                 ////
//// usb_cdc_kbhit() - Returns TRUE if there is one or more          ////
////      character received and waiting in the receive buffer.      ////
////                                                                 ////
//// usb_cdc_getc() - Gets a character from the receive buffer.  If  ////
////      there is no data in the receive buffer it will wait until  ////
////      there is data in the receive buffer.  If you do not want   ////
////      to wait in an infinit loop, use usb_cdc_kbhit() first to   ////
////      check if there is data before calling usb_cdc_getc().      ////
////                                                                 ////
//// usb_cdc_putc(char c) - Puts a character into the transmit       ////
////      buffer.  If the transmit buffer is full it will wait until ////
////      the transmit buffer is not full before putting the char    ////
////      into the transmit buffer.  The transmit buffer is read by  ////
////      the PC very quickly, and therefore the buffer should only  ////
////      be full for a few milli-seconds.  If you are concerned     ////
////      and don't want to be stuck in a long or infinite loop,     ////
////      use usb_cdc_putready() to see if there is space in the     ////
////      transmit buffer before putting data into the transmit      ////
////      buffer.                                                    ////
////                                                                 ////
//// usb_cdc_puts(*ptr) - Sends the null terminated string to the    ////
////     the transmit buffer.  Unlike usb_cdc_putc() or              ////
////     usb_cdc_putc_fast(), this routine will fit the string in    ////
////     one packet (whereas the other functions will flush the      ////
////     buffer as soon as the endpoint is free).                    ////
////     Returns TRUE if the packet was sent, FALSE if the buffer    ////
////     was still in use.                                           ////
////     Will stop copying characters from ptr to the endpoint       ////
////     buffer once it is full (but it will still return TRUE).     ////
////     'ptr' needs to be smaller than the transmit buffer.         ////
////                                                                 ////
//// usb_cdc_putd(*ptr, len) - Sends 'len' byte from 'ptr' to the    ////
////     transmit buffer.  Unlike usb_cdc_putc() or                  ////
////     usb_cdc_putc_fast(), this routine will fit the string in    ////
////     one packet (whereas the other functions will flush the      ////
////     buffer as soon as the endpoint is free).                    ////
////     Returns TRUE if the packet was sent, FALSE if the buffer    ////
////     was still in use.                                           ////
////     Will stop copying characters from ptr to the endpoint       ////
////     buffer once it is full (but it will still return TRUE).     ////
////     'len' needs to be smaller than the transmit buffer.         ////
////                                                                 ////
//// usb_cdc_putready() - Returns the number of bytes available      ////
////     in the TX buffer for storing characters.  If this returns   ////
////     0 then the buffer is full and waiting for the host (PC)     ////
////     to read the buffer.                                         ////
////                                                                 ////
//// usb_cdc_putempty() - Returns TRUE if the transmit buffers are   ////
////     empty and fully flushed/transmitted to host (PC).           ////
////                                                                 ////
//// usb_cdc_connected() - Returns TRUE if we received a             ////
////      Set_Line_Coding.  On most serial terminal programs (such   ////
////      as Hyperterminal), they will send a Set_Line_Coding        ////
////      message when the program starts and it opens the virtual   ////
////      COM port.  This is a simple way to determine if the PC     ////
////      is ready to display data on a serial terminal program,     ////
////      but is not garaunteed to work all the time or on other     ////
////      terminal programs.                                         ////
////                                                                 ////
//// usb_cdc_putc_fast(char c) - Similar to usb_cdc_putc(), except   ////
////      if the transmit buffer is full it will skip the char.      ////
////                                                                 ////
//// usb_cdc_line_coding - A structure used for Set_Line_Coding and  ////
////       Get_Line_Coding.  Most of the time you can ignore this.   ////
////                                                                 ////
//// usb_cdc_break - If the PC has sent a break command, this will   ////
////       hold the break time (in milli-seconds).  If the PC sends  ////
////       a value of 0xFFFF the device is supposed to hold the      ////
////       break until it sends a value of 0                         ////
////                                                                 ////
//// usb_cdc_carrier - Where Set_Control_Line_State value is stored. ////
////       Of most relevance is the field dte_present, which is the  ////
////       DTR setting.                                              ////
////                                                                 ////
//// usb_cdc_serial_state(state) - Transmit to the host our          ////
////       SERIAL_STATE.  This holds information such as DCD, DSR,   ////
////       ring, break and more.  See the documentation above        ////
////       usb_cdc_serial_state() for more information, including    ////
////       format of state structure.                                ////
////                                                                 ////
//// The following functions are also provided, and are ports of the ////
//// I/O functions in input.c.  See input.c and the CCS manual for   ////
//// documentation:                                                  ////
////   get_float_usb() - Read a float number from the user           ////
////   get_long_usb() - Read a long number from the user             ////
////   get_int_usb() - Read an integer number from the user          ////
////   get_string_usb(char *s, int max) - Read a string from the user. ////
////   gethex_usb() - Read a byte, in HEX, from the user             ////
////   gethex1_usb() - Read a HEX character                          ////
////                                                                 ////
//// USB_CDC_ISR() can be defined if you want a specific routine to  ////
//// be called when there is incoming CDC (virtual com port) data.   ////
//// This is useful if you want to update legacy RS232 code that     ////
//// was using #int_rda to handle incoming data in the RS232 ISR.    ////
//// However, see the INTERRUPT LIMITATIONS section below.           ////
////                                                                 ////
//// Normally usb_cdc_putc() and usb_cdc_putc_fast() will attempt    ////
////  to flush the data as soon as possible.  However at times       ////
////  this can be ineffecient and flush a packet with only 1 byte.   ////
////  The USB_CDC_DELAYED_FLUSH option will attempt to flush         ////
////  at the beginning of usb_cdc_putc() and usb_cdc_putc_fast()     ////
////  if the local buffer is full.  USB_CDC_DELAYED_FLUSH will       ////
////  also attempt to flush the packet on each call to usb_task().   ////
////  It is recommended to only use USB_CDC_DELAYED_FLUSH option     ////
////  if you have a main loop that periodically calls usb_task().    ////
////                                                                 ////
//// This driver will load all the rest of the USB code, and a set   ////
//// of descriptors that will properly describe a CDC device for a   ////
//// virtual COM port (usb_desc_cdc.h)                               ////
////                                                                 ////
//// An .INF file is provided (cdc_NTXP.inf) that will load the      ////
//// standard CDC drivers for a virtual COM port in Windows          ////
//// NT/2000/XP and above.                                           ////
////                                                                 ////
////                                                                 ////
//// BUFFER SIZES                                                    ////
//// -------------------------------------------------------------   ////
//// USB_CDC_DATA_IN_SIZE controls the PIC->PC buffer size.  The     ////
////  total buffer size will be ((USB_CDC_DATA_IN_SIZE*2)-1).        ////
////  Full speed devices limit this value to be 64.  To increase     ////
////  the size of the local PIC buffer you can also define           ////
////  USB_CDC_DATA_LOCAL_SIZE.  If USB_CDC_DATA_LOCAL_SIZE is        ////
////  defined then the total PIC->PC buffer size would be            ////
////  USB_CDC_DATA_LOCAL_SIZE+USB_CDC_DATA_IN_SIZE.                  ////
////  If USB_CDC_DATA_IN_SIZE is not defined, the default value      ////
////  of 64 is used.  If USB_CDC_DATA_LOCAL_SIZE is not defined      ////
////  then this option isn't used.                                   ////
////                                                                 ////
////                                                                 ////
//// INTERRUPT LIMITATIONS                                           ////
//// -------------------------------------------------------------   ////
//// This section is only relevant if you are using USB interrupts   ////
////  and not the USB_ISR_POLLING option.                            ////
////                                                                 ////
//// USB handling is complex, often requiring several packet         ////
////  transmissions to accomplish transfer of one block of data.     ////
////  Most of this processing is done in the USB ISR.  Because       ////
////  of this you cannot call usb_cdc_putc() inside another ISR, the ////
////  USB ISR or when ISRs are disabled.  To workaround this problem,   ////
////  use usb_cdc_putc_fast() and the USB_CDC_DELAYED_FLUSH option.  ////
////  This isn't perfect, because if you use usb_cdc_putc_fast() to  ////
////  overflow the TX buffer than data will be lost.                 ////
////                                                                 ////
//// You also cannot call usb_cdc_getc() inside another ISR, the USB ////
////  ISR, USB_CDC_ISR() or when interrupts are disabled UNLESS      ////
////  usb_cdc_kbhit() returns TRUE.  It is for this reason           ////
////  ex_usb_serial3.c uses a statemachine for reception of data.    ////
////                                                                 ////
//// ex_usb_serial3.c shows an example of working around these       ////
////  ISR limitations.  Failure to follow these limitations can      ////
////  cause the PIC to lockup.  These limtations only need to be     ////
////  followed in the conditions listed above (writing code in the   ////
////  ISR or interrupts are disabled).                               ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// VERSION HISTORY                                                 ////
////                                                                 ////
//// May 31st, 2013:                                                 ////
////  usb_cdc_putready() now returns the number of bytes available.  ////
////                                                                 ////
//// May 23rd, 2013:                                                 ////
////  Added usb_cdc_putd().                                          ////
////                                                                 ////
//// February 18th, 2013:                                            ////
////  Enhanced PIC16 support added.                                  ////
////  Added USB_CDC_ISR(), USB_CDC_DELAYED_FLUSH and                 ////
////     USB_CDC_DATA_LOCAL_SIZE.                                    ////
////                                                                 ////
//// September 13th, 2012:                                           ////
////  Fixed a possible corruption that would occur on 16bit PICs     ////
////     (PIC24, dsPIC33).                                           ////
////  Added usb_cdc_putempty().                                      ////
////                                                                 ////
//// December 16th, 2011:                                            ////
////  Added __USB_CDC_HELPERS_ONLY__.  If defined, this library will ////
////     only include the get string helper functions.  This is      ////
////     a rarely used option, used only if you already wrote your   ////
////     own getc() and putc() routines.                             ////
////  usb_cdc_flush_tx_buffer() checks len buffer before flushing .  ////
////  usb_isr_tok_in_cdc_data_dne() doesn't check len, now that      ////
////     usb_cdc_flush_tx_buffer() is checking len.                  ////
////                                                                 ////
//// Oct 27th, 2011:                                                 ////
////  Added SERIAL_STATE support.  See usb_cdc_serial_state() for    ////
////     documentation.  In order for this to work,                  ////
////     USB_CDC_COMM_IN_SIZE has to be 11 bytes or bigger.          ////
////                                                                 ////
//// Oct 15th, 2010:                                                 ////
////  Potential bug in CDC that causes lost PIC->PC chars resolved?  ////
////  Added USB_ISR_POLLING support.  Define this and interrupts     ////
////     will not be used.  usb_task() must be called periodically   ////
////     in your main loop.  If it is not called faster than once    ////
////     per millisecond, USB may not work (PIC18 and PIC24 only).   ////
////                                                                 ////
//// Jan 28, 2010:                                                   ////
////  Added usb_cdc_puts(str), which will fit the entire str into    ////
////     one packet.                                                 ////
////                                                                 ////
//// Jan 25, 2010:                                                   ////
////  Ignore incoming 0 length packets.                              ////
////                                                                 ////
//// June 9th, 2009:                                                 ////
////  Some problems with Vista and fast hubs/hosts fixed on PIC18.   ////
////                                                                 ////
//// April 7th, 2009:                                                ////
////   Vista 'code 10' issues resolved.                              ////
////                                                                 ////
//// March 5th, 2009:                                                ////
////   Cleanup for Wizard.                                           ////
////   PIC24 Initial release.                                        ////
////                                                                 ////
//// Nov 26th, 2007:                                                 ////
////              usb_cdc_put_buffer_free() should be more stable.   ////
////              Fixed a hang-up that would happen if you sent      ////
////                 exactly 64 bytes.                               ////
////                                                                 ////
//// Nov 6th, 2007:                                                  ////
////              Compatabible with latest pic18_usb.h, which        ////
////                 reduces RAM footprint of USB driver.            ////
////              This driver now fits on 18F4450/2450 PICs.         ////
////                                                                 ////
//// October 27th, 2005: Changed the way incoming packets are        ////
////               handled in that CDC driver will not flush         ////
////               endpoint until user has handled all data.  This   ////
////               will prevent overflows as the USB will NAK        ////
////               incoming packets until it is ready to receive     ////
////               more.                                             ////
////            When using 18F4550 family, the RX buffer is          ////
////               mapped directly to the endpoint buffer - this     ////
////               saves a chunk of RAM.                             ////
////            When using the 18F4550 family, you can increase      ////
////               the TX and RX size over 64 bytes.                 ////
////            No longer send 0len packets in the TBE interrupt.    ////
////            Hopefully fixed bugs that caused random crashes      ////
////               if you tried sending more than 64 bytes.          ////
////                                                                 ////
//// July 6th, 2005: Global interrupts disabled when writing to TX   ////
////                   buffer.                                       ////
////                                                                 ////
//// July 1st, 2005: Initial Release.                                ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2005 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

//input.c ported to use CDC:
float get_float_usb();
signed long get_long_usb();
signed int get_int_usb();
void get_string_usb(char* s, unsigned int max);
BYTE gethex_usb();
BYTE gethex1_usb();

#if !defined(__USB_CDC_HELPERS_ONLY__)

//api for the user:
#define usb_cdc_kbhit() (usb_cdc_get_buffer_status.got)
#define usb_cdc_putempty() ((usb_cdc_put_buffer_nextin==0) && usb_cdc_put_buffer_free())
#define usb_cdc_putready() (sizeof(usb_cdc_put_buffer)-usb_cdc_put_buffer_nextin)
#define usb_cdc_connected() (usb_cdc_got_set_line_coding)
void usb_cdc_putc_fast(char c);
char usb_cdc_getc(void);
void usb_cdc_putc(char c);
void usb_cdc_get_discard(void);

//functions automatically called by USB handler code
void usb_isr_tkn_cdc(void);
void usb_cdc_init(void);
void usb_isr_tok_out_cdc_control_dne(void);
void usb_isr_tok_in_cdc_data_dne(void);
void usb_isr_tok_out_cdc_data_dne(void);

void usb_cdc_flush_tx_buffer(void);

/////////////////////////////////////////////////////////////////////////////
//
// Include the CCS USB Libraries.  See the comments at the top of these
// files for more information
//
/////////////////////////////////////////////////////////////////////////////
#ifndef __USB_PIC_PERIF__
#define __USB_PIC_PERIF__   1
#endif

#if __USB_PIC_PERIF__
   #if defined(__PCM__)
    #include <pic16f_usb.h>   //Microchip PIC16Fxxxx hardware layer for usb.c
   #elif defined(__PCH__)
    #include <pic18_usb.h>   //Microchip 18Fxx5x hardware layer for usb.c
   #else
    #include <pic24_usb.h>   //Microchip 18Fxx5x hardware layer for usb.c
   #endif
#else
   #include <usbn960x.h>
#endif
#ifndef __USB_DESCRIPTORS__
#include <usb_desc_cdc.h>   //USB Configuration and Device descriptors for this USB device
#endif

struct {
   int1 got;
  #if USB_CDC_DATA_OUT_SIZE>=0x100
   unsigned int16 len;
   unsigned int16 index;
  #else
   unsigned int8 len;
   unsigned int8 index;
  #endif
} usb_cdc_get_buffer_status;

#include <usb.c>        //handles usb setup tokens and get descriptor reports

/*
   Generally, you cannot use CDC on a slow speed USB device.  The primary
   reason for this is that CDC uses bulk transfer endpoints, and bulk
   transfer endpoints is not supported on slow speed devices per the USB
   specification.  You may be able to find unofficial drivers for your
   operating system that allows CDC to operate on a slow speed device,
   but CCS doesn't have any that they can recommend to you.
*/
#if (USB_USE_FULL_SPEED==0)
   #error CDC and slow speed is not supported.  See comments above.
#endif

struct {
        unsigned int32   dwDTERrate;   //data terminal rate, in bits per second
        unsigned int8    bCharFormat;  //num of stop bits (0=1, 1=1.5, 2=2)
        unsigned int8    bParityType;  //parity (0=none, 1=odd, 2=even, 3=mark, 4=space)
        unsigned int8    bDataBits;    //data bits (5,6,7,8 or 16)
} __attribute__((__packed__)) usb_cdc_line_coding;

//length of time, in ms, of break signal as we received in a SendBreak message.
//if ==0xFFFF, send break signal until we receive a 0x0000.
unsigned int16 usb_cdc_break;

unsigned int8 usb_cdc_encapsulated_cmd[8];

#ifndef USB_CDC_DATA_LOCAL_SIZE
unsigned int8 usb_cdc_put_buffer[USB_CDC_DATA_IN_SIZE-1];
#else
unsigned int8 usb_cdc_put_buffer[USB_CDC_DATA_LOCAL_SIZE];
#endif

#define usb_cdc_put_buffer_free()  usb_tbe(USB_CDC_DATA_IN_ENDPOINT)
#if sizeof(usb_cdc_put_buffer)>=0x100
 #error This is not supported.  That is because ISR may change this 16bit value while your non-ISR code is reading this.
 typedef unsigned int16 usb_cdc_tx_t;
#else
 typedef unsigned int8 usb_cdc_tx_t;
#endif

usb_cdc_tx_t usb_cdc_put_buffer_nextin;


#if defined(__PIC__)
 #define usb_cdc_get_buffer_status_buffer usb_ep2_rx_buffer
#else
 unsigned int8 usb_cdc_get_buffer_status_buffer[USB_CDC_DATA_OUT_SIZE];
#endif

int1 usb_cdc_got_set_line_coding;

struct  {
   int1 dte_present; //1=DTE present, 0=DTE not present
   int1 active;      //1=activate carrier, 0=deactivate carrier
   unsigned int reserved:6;
} usb_cdc_carrier;

enum {USB_CDC_OUT_NOTHING=0, USB_CDC_OUT_COMMAND=1, USB_CDC_OUT_LINECODING=2, USB_CDC_WAIT_0LEN=3} __usb_cdc_state;

/*
#if defined(__PCH__)
 #byte INTCON=0xFF2
 #bit INT_GIE=INTCON.7
#else
 #word SR=0x42
#endif
*/

//handle OUT token done interrupt on endpoint 0 [read encapsulated cmd and line coding data]
void usb_isr_tok_out_cdc_control_dne(void) {
   debug_usb(debug_putc,"CDC %X ",__usb_cdc_state);

   switch (__usb_cdc_state) {
      //printf(putc_tbe,"@%X@\r\n", __usb_cdc_state);
      case USB_CDC_OUT_COMMAND:
         //usb_get_packet(0, usb_cdc_encapsulated_cmd, 8);
         memcpy(usb_cdc_encapsulated_cmd, usb_ep0_rx_buffer,8);
        #if USB_MAX_EP0_PACKET_LENGTH==8
         __usb_cdc_state=USB_CDC_WAIT_0LEN;
         usb_request_get_data();
        #else
         usb_put_0len_0();
         __usb_cdc_state=0;
        #endif
         break;

    #if USB_MAX_EP0_PACKET_LENGTH==8
      case USB_CDC_WAIT_0LEN:
         usb_put_0len_0();
         __usb_cdc_state=0;
         break;
    #endif

      case USB_CDC_OUT_LINECODING:
         //usb_get_packet(0, &usb_cdc_line_coding, 7);
         //printf(putc_tbe,"\r\n!GSLC FIN!\r\n");
         memcpy(&usb_cdc_line_coding, usb_ep0_rx_buffer,7);
         __usb_cdc_state=0;
         usb_put_0len_0();
         break;

      default:
         __usb_cdc_state=0;
         //usb_init_ep0_setup(); //REMOVED JUN/9/2009
         break;
   }
}

//handle IN token on 0 (setup packet)
void usb_isr_tkn_cdc(void) {
   //make sure the request goes to a CDC interface
   if ((usb_ep0_rx_buffer[4] == 1) || (usb_ep0_rx_buffer[4] == 0)) {
      //printf(putc_tbe,"!%X!\r\n", usb_ep0_rx_buffer[1]);
      switch(usb_ep0_rx_buffer[1]) {
         case 0x00:  //send_encapsulated_command
            __usb_cdc_state=USB_CDC_OUT_COMMAND;
            usb_request_get_data();
            break;

         case 0x01:  //get_encapsulated_command
            memcpy(usb_ep0_tx_buffer, usb_cdc_encapsulated_cmd, 8);
            usb_request_send_response(usb_ep0_rx_buffer[6]);  //send wLength bytes
            break;

         case 0x20:  //set_line_coding
            debug_usb(debug_putc,"!GSLC!");
            __usb_cdc_state=USB_CDC_OUT_LINECODING;
            usb_cdc_got_set_line_coding=TRUE;
            usb_request_get_data();
            break;

         case 0x21:  //get_line_coding
            memcpy(usb_ep0_tx_buffer, &usb_cdc_line_coding, sizeof(usb_cdc_line_coding));
            usb_request_send_response(sizeof(usb_cdc_line_coding)); //send wLength bytes
            break;

         case 0x22:  //set_control_line_state
            (unsigned int8)usb_cdc_carrier=usb_ep0_rx_buffer[2];
            usb_put_0len_0();
            break;

         case 0x23:  //send_break
            usb_cdc_break=make16(usb_ep0_rx_buffer[2],usb_ep0_rx_buffer[3]);
            usb_put_0len_0();
            break;

         default:
            usb_request_stall();
            break;
      }
   }
}

//handle OUT token done interrupt on endpoint 2 [buffer incoming received chars]
void usb_isr_tok_out_cdc_data_dne(void) {
   usb_cdc_get_buffer_status.got=TRUE;
   usb_cdc_get_buffer_status.index=0;
#if (defined(__PIC__) && __PIC__)
    usb_cdc_get_buffer_status.len=usb_rx_packet_size(USB_CDC_DATA_OUT_ENDPOINT);
#else
   usb_cdc_get_buffer_status.len=usb_get_packet_buffer(
      USB_CDC_DATA_OUT_ENDPOINT,&usb_cdc_get_buffer_status_buffer[0],USB_CDC_DATA_OUT_SIZE);
#endif
   if (!usb_cdc_get_buffer_status.len)
   {
      usb_cdc_get_discard();
   }
   /*
  #if defined(USB_CDC_ISR)
   else
   {
      USB_CDC_ISR();
   }
  #endif
  */
}

//handle IN token done interrupt on endpoint 2 [transmit buffered characters]
void usb_isr_tok_in_cdc_data_dne(void) 
{
   usb_cdc_flush_tx_buffer();
}

#include <string.h>

void usb_cdc_flush_tx_buffer(void) 
{
  #ifdef USB_CDC_DATA_LOCAL_SIZE
   usb_cdc_tx_t n;
  #endif
  
   if (usb_cdc_put_buffer_nextin != 0)
   {
     #ifndef USB_CDC_DATA_LOCAL_SIZE
      if (usb_put_packet(USB_CDC_DATA_IN_ENDPOINT,usb_cdc_put_buffer,usb_cdc_put_buffer_nextin,USB_DTS_TOGGLE))
      {
         usb_cdc_put_buffer_nextin = 0;
      }
     #else
      n = usb_cdc_put_buffer_nextin;
      if (n > (USB_CDC_DATA_IN_SIZE-1)) //always send one less than packet size so we don't have to deal with 0 len packets
         n = USB_CDC_DATA_IN_SIZE-1;
      if (usb_put_packet(USB_CDC_DATA_IN_ENDPOINT,usb_cdc_put_buffer,n,USB_DTS_TOGGLE))
      {
         //pull the buffer back
         memmove(usb_cdc_put_buffer, &usb_cdc_put_buffer[n], usb_cdc_put_buffer_nextin-n);
         usb_cdc_put_buffer_nextin -= n;
      }      
     #endif
   }
}

void usb_cdc_init(void) 
{
   usb_cdc_line_coding.dwDTERrate = 9600;
   usb_cdc_line_coding.bCharFormat = 0;
   usb_cdc_line_coding.bParityType = 0;
   usb_cdc_line_coding.bDataBits = 8;
   (int8)usb_cdc_carrier = 0;
   usb_cdc_got_set_line_coding = FALSE;
   usb_cdc_break = 0;
   usb_cdc_put_buffer_nextin = 0;
   usb_cdc_get_buffer_status.got = 0;
   __usb_cdc_state = 0;
}

////////////////// END USB CONTROL HANDLING //////////////////////////////////

////////////////// BEGIN USB<->RS232 CDC LIBRARY /////////////////////////////

typedef struct
{
   unsigned int bRxCarrier:1; //State of receiver carrier detection mechanism of device. This signal corresponds to V.24 signal 109 and RS-232 signal DCD.
   unsigned int bTxCarrier:1; //State of transmission carrier. This signal corresponds to V.24 signal 106 and RS-232 signal DSR
   unsigned int bBreak:1;  //State of break detection mechanism of the device.
   unsigned int bRingSignal:1;   //State of ring signal detection of the device. 
   unsigned int bFraming:1;   //A framing error has occurred.
   unsigned int bParity:1;    //A parity error has occurred.
   unsigned int bOverRun:1;   //Received data has been discarded due to overrun in the device.
   unsigned int reserved0:1;   //future use
   unsigned int reserved1:8;   //future use
} cdc_serial_state_t;

/*
   Send SERIAL_STATE notification to the host.  This contains DSR, DCD, Ring, 
   break signal and more (see cdc_serial_state_t).
   Some of these values are held by the host (meaning it's value only needs
   to be sent on change), but some values are one shot (meaning you continously
   need to send value to host while being held).
*/
int1 usb_cdc_serial_state(cdc_serial_state_t state)
{
  #if __USB_PIC_PERIF__
   #define CDC_EP1_NOTIFY_BUFFER usb_ep1_tx_buffer
  #else
   unsigned int8 payload[10];
   #define CDC_EP1_NOTIFY_BUFFER payload
  #endif

   if (!usb_tbe(USB_CDC_COMM_IN_ENDPOINT))
      return(FALSE);

   //bmRequestType
   CDC_EP1_NOTIFY_BUFFER[0] = 0xA1;  //0b10100001
   //bNotification
   CDC_EP1_NOTIFY_BUFFER[1] = 0x20;  //SERIAL_STATE
   //wValue
   CDC_EP1_NOTIFY_BUFFER[2] = 0;
   CDC_EP1_NOTIFY_BUFFER[3] = 0;
   //wIndex
   CDC_EP1_NOTIFY_BUFFER[4] = 0;
   CDC_EP1_NOTIFY_BUFFER[5] = 0;
   //wLength
   CDC_EP1_NOTIFY_BUFFER[6] = 2; //sizeof(cdc_serial_state_t)
   CDC_EP1_NOTIFY_BUFFER[7] = 0;
   //data
   CDC_EP1_NOTIFY_BUFFER[8] = (unsigned int8)state;
   CDC_EP1_NOTIFY_BUFFER[9] = (unsigned int16)state >> 8;

  #if __USB_PIC_PERIF__
   usb_flush_in(USB_CDC_COMM_IN_ENDPOINT, 10, USB_DTS_TOGGLE);
  #else
   usb_put_packet(USB_CDC_COMM_IN_ENDPOINT, payload, 10, USB_DTS_TOGGLE);
  #endif
   
   return(TRUE);
}

void usb_cdc_get_discard(void)
{
   usb_cdc_get_buffer_status.got = FALSE;
   usb_flush_out(USB_CDC_DATA_OUT_ENDPOINT, USB_DTS_TOGGLE);
}

char usb_cdc_getc(void) 
{
   char c;

   while (!usb_cdc_kbhit()) 
   {
     #if defined(USB_ISR_POLLING)
      usb_task();
     #endif
   }

   c=usb_cdc_get_buffer_status_buffer[usb_cdc_get_buffer_status.index++];

   if (usb_cdc_get_buffer_status.index >= usb_cdc_get_buffer_status.len) 
   {
      usb_cdc_get_discard();
   }

   return(c);
}

#define __USB_PAUSE_ISR()  int1 old_usbie; old_usbie = USBIE; USBIE = 0
#define __USB_RESTORE_ISR() if (old_usbie) USBIE = 1

static void _usb_cdc_putc_fast_noflush(char c)
{
   __USB_PAUSE_ISR();

  #if defined(USB_CDC_DELAYED_FLUSH)
   if (usb_cdc_put_buffer_nextin >= sizeof(usb_cdc_put_buffer)) 
   {
      usb_cdc_flush_tx_buffer();
   }
  #endif

   if (usb_cdc_put_buffer_nextin >= sizeof(usb_cdc_put_buffer)) {
      usb_cdc_put_buffer_nextin = sizeof(usb_cdc_put_buffer)-1;  //we just overflowed the buffer!
   }
   
   usb_cdc_put_buffer[usb_cdc_put_buffer_nextin++] = c;

   __USB_RESTORE_ISR();
}

void usb_cdc_putc_fast(char c)
{
   _usb_cdc_putc_fast_noflush(c);

  #if defined(USB_ISR_POLLING)
   // if interrupts are disabled, we should clear all activity isrs
   // before we attempt to put any data onto an endpoint.
   if (!usb_tbe(USB_CDC_DATA_IN_ENDPOINT))
      return;
   usb_task();
  #endif
  
  #if !defined(USB_CDC_DELAYED_FLUSH)
   //if (usb_cdc_put_buffer_free()) 
   {
      //printf("FL2 %LU\r\n", (int16)usb_cdc_put_buffer_nextin);
      usb_cdc_flush_tx_buffer();
   }
  #endif

   //putc('*');
}

void usb_cdc_putc(char c)
{
   while (!usb_cdc_putready()) 
   {
     #if 0
      if (usb_cdc_put_buffer_free()) 
      {
         //printf("FL1 %LU\r\n", (int16)usb_cdc_put_buffer_nextin);
         usb_cdc_flush_tx_buffer();
      }
     #endif
     #if defined(USB_ISR_POLLING)
      usb_task();
     #endif     
      //delay_ms(500);
      //printf(putc_tbe,"TBE=%U CNT=%U LST=%U\r\n",usb_tbe(USB_CDC_DATA_IN_ENDPOINT), usb_cdc_put_buffer_nextin, usb_cdc_last_data_packet_size);
   }
   //putc('.');
   //putc(c);
   usb_cdc_putc_fast(c);
}

int1 usb_cdc_putd(unsigned char *ptr, unsigned int8 len)
{
 #if USB_EP2_TX_SIZE>=0x100
   unsigned int16 i;
 #else
   unsigned int8 i;
 #endif
   char c;
   
   i = 0;
   
   if (!usb_cdc_put_buffer_free())
      return(FALSE);
   
   while(len--)
   {
      c = *ptr++;
      _usb_cdc_putc_fast_noflush(c);
      if (++i >= USB_EP2_TX_SIZE)
         break;
   }
   
   usb_cdc_flush_tx_buffer();
   
   return(TRUE);
}

int1 usb_cdc_puts(char *ptr)
{   
   unsigned int8 len;

   len = strlen(ptr);
  
   return(usb_cdc_putd(ptr, len));
}

#endif //__USB_CDC_HELPERS_ONLY__

#include <ctype.h>

BYTE gethex1_usb() 
{
   char digit;

   digit = usb_cdc_getc();

   usb_cdc_putc(digit);

   if(digit<='9')
     return(digit-'0');
   else
     return((toupper(digit)-'A')+10);
}

BYTE gethex_usb() {
   unsigned int8 lo,hi;

   hi = gethex1_usb();
   lo = gethex1_usb();
   if(lo==0xdd)
     return(hi);
   else
     return( hi*16+lo );
}

void get_string_usb(char* s, unsigned int max) {
   unsigned int len;
   char c;

   --max;
   len=0;
   do {
     c=usb_cdc_getc();
     if(c==8) {  // Backspace
        if(len>0) {
          len--;
          usb_cdc_putc(c);
          usb_cdc_putc(' ');
          usb_cdc_putc(c);
        }
     } else if ((c>=' ')&&(c<='~'))
       if(len<max) {
         s[len++]=c;
         usb_cdc_putc(c);
       }
   } while(c!=13);
   s[len]=0;
}


// stdlib.h is required for the ato_ conversions
// in the following functions
#ifdef _STDLIB

signed int get_int_usb() {
  char s[7];
  signed int i;

  get_string_usb(s, 7);

  i=atoi(s);
  return(i);
}

signed long get_long_usb() {
  char s[13];
  signed long l;

  get_string_usb(s, 13);
  l=atol(s);
  return(l);
}

float get_float_usb() {
  char s[20];
  float f;

  get_string_usb(s, 20);
  f = atof(s);
  return(f);
}

#endif
