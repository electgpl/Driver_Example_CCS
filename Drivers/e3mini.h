///////////////////////////////////////////////////////////////////////////
////                                                                   ////
//// E3MINI.H                                                          ////
//// *************************************************************     ////
////                                                                   ////
//// Header file for developing applications compatible with E3mini    ////
//// and it's bootloader with integrated USB support.                  ////
////                                                                   ////
//// Include this file instead of the traditional CCS device header    ////
//// file.  See the E3mini exercise book for more information and      ////
//// examples.  This file should not be modified.                      ////
////                                                                   ////
////                                                                   ////
//// HARDWARE I/O CONFIGURATION                                        ////
//// *************************************************************     ////
////                                                                   ////
//// Pin      Analog Port    Description                               ////
//// -----    -----------    ------------------------------------      ////
//// RB4      AN10           Header pin 8                              ////
//// RB5      AN11           Header pin 7 (also UART RX)               ////
//// RB6                     Header pin 6                              ////
//// RB7                     Header pin 5 (also UART TX)               ////
//// RC0      AN4            Potentiometer, header pin 16              ////
//// RC1      AN5            Switch 1, header pin 15 (also INT0)       ////
//// RC2      AN6            Switch 2, header pin 14 (also INT1)       ////
//// RC3      AN7            Red LED, header pin 13  (also INT2)       ////
//// RC4                     Yellow LED, header pin 12                 ////
//// RC5                     Header pin 11 (also CCP1)                 ////
//// RC6      AN8            Green LED, header pin 10                  ////
//// RC7      AN9            Header pin 9                              ////
////                                                                   ////
////                                                                   ////
//// USB API                                                           ////
//// *************************************************************     ////
////                                                                   ////
//// API for adding a virtual COM port on your PC over USB using       ////
//// the standard Communication Device Class (CDC) specification.      ////
//// On Windows, a CDC device will look like a COM port and can        ////
//// be opened like a COM serial port.                                 ////
////                                                                   ////
//// This device creates a virtual RS232 link between the PC and       ////
//// the PIC, therefore the API provided will be familiar to           ////
//// anyone with standard UART stream I/O:                             ////
////                                                                   ////
//// usb_cdc_kbhit() - Returns TRUE if there is one or more            ////
////      character received and waiting in the receive buffer.        ////
////                                                                   ////
//// usb_cdc_getc() - Gets a character from the receive buffer.  If    ////
////      there is no data in the receive buffer it will wait until    ////
////      there is data in the receive buffer.  If you do not want     ////
////      to wait in an infinit loop, use usb_cdc_kbhit() first to     ////
////      check if there is data before calling usb_cdc_getc().        ////
////                                                                   ////
//// usb_cdc_putc(char c) - Puts a character into the transmit         ////
////      buffer.  If the transmit buffer is full it will wait until   ////
////      the transmit buffer is not full before putting the char      ////
////      into the transmit buffer.  The transmit buffer is read by    ////
////      the PC very quickly, and therefore the buffer should only    ////
////      be full for a few milli-seconds.  If you are concerned       ////
////      and don't want to be stuck in a long or infinite loop,       ////
////      use usb_cdc_putready() to see if there is space in the       ////
////      transmit buffer before putting data into the transmit        ////
////      buffer.                                                      ////
////                                                                   ////
//// usb_cdc_putc_safe(char c) - Similar to usb_cdc_putc(), but will   ////
////     abort TX if transmit buffer is full.                          ////
////                                                                   ////
//// usb_cdc_putready() - Returns TRUE if there is room left in the    ////
////      transmit buffer for another character.                       ////
////                                                                   ////
//// usb_enumerated() - Returns TRUE if the device was detected by     ////
////     the operating system over USB.                                ////
////                                                                   ////
//// usb_cdc_opened() - Returns TRUE if we received a                  ////
////      Set_Line_Coding.  On most serial terminal programs (such     ////
////      as Hyperterminal), they will send a Set_Line_Coding          ////
////      message when the program starts and it opens the virtual     ////
////      COM port.  This is a simple way to determine if the PC       ////
////      is ready to display data on a serial terminal program,       ////
////      but is not garaunteed to work all the time or on other       ////
////      terminal programs.                                           ////
////                                                                   ////
//// usb_cdc_serial_state(state) - Transmit to the host our            ////
////       SERIAL_STATE.  This holds information such as DCD, DSR,     ////
////       ring, break and more.  See the documentation above          ////
////       usb_cdc_serial_state() for more information, including      ////
////       format of state structure.                                  ////
////                                                                   ////
//// usb_cdc_info(*usb_cdc_info_t) - Returns information about the     ////
////     virtual serial port.  See usb_cdc_info_t for more             ////
////     information.                                                  ////
////                                                                   ////
//// The following functions are also provided, and are ports of the   ////
//// I/O functions in input.c.  See input.c and the CCS manual for     ////
//// documentation:                                                    ////
////   get_float_usb() - Read a float number from the user             ////
////   get_long_usb() - Read a long number from the user               ////
////   get_int_usb() - Read an integer number from the user            ////
////   get_string_usb(char *s, int max) - Read a string from the user. ////
////   gethex_usb() - Read a byte, in HEX, from the user               ////
////   gethex1_usb() - Read a HEX character                            ////
////                                                                   ////
//// An .INF file is provided (cdc_NTXP.inf) that will load the        ////
//// standard CDC drivers for a virtual COM port in Windows            ////
//// NT/2000/XP and above.                                             ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2011 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#ifndef __E3MINI_H__
#define __E3MINI_H__

#include <18F14K50.h>

#if !defined(__e3loader)
#device NORETFIE
#endif

#device ADC=8
#device NO_RESERVE_ERRORS
#device PASS_STRINGS=IN_RAM

#fuses NOIESO
#fuses NOFCMEN
#fuses PCLKEN
#fuses NOPUT
#fuses NOBROWNOUT
#fuses NOWDT
#fuses NOMCLR
#fuses NOHFOFST
#fuses NOLVP
#fuses NOXINST
#fuses BBSIZ2K
#fuses PROTECT
#fuses CPB
#fuses NOCPD
#fuses NOEBTR
#fuses EBTRB
#fuses NOWRT
#fuses WRTC
#fuses WRTB
#fuses NOWRTD
#fuses HS
#fuses PLLEN      //enable pll, we can now use CPUDIVx fuses
#fuses CPUDIV1    //CPU clock set to 48MHz
#fuses USBDIV2    //when using low speed USB, need to divide 12MHz by 2.  this fuse has no effect when using full speed.

#use delay(clock=48000000) //12mhz crystal running at 48mhz because of USB PLL

//how big is the bootloader?
//the bootloader will reside from address 0x0000 to this location.  the
//application will then sit at this location+1 to the end of program memory.
#define LOADER_SIZE        (0x1FFF)
#define LOADER_START       (0)
#define LOADER_END         (LOADER_SIZE)
#define APPLICATION_START  (LOADER_SIZE+1)
#define APPLICATION_END    (getenv("PROGRAM_MEMORY")-1)
#define APPLICATION_ISR    (APPLICATION_START+8)
#define LOADER_RAM_START   0x200 //this and everything after this point is reserved for loader and usb
#define SCRATCH_RAM_END    0x27    //this and everything before this is common scratch shared between loader and application

#define USB_IS_ENUMRATED_PROXY   0x220
#define USB_CDC_INFO_PROXY       0x240
#define USB_CDC_KBHIT_PROXY      0x260
#define USB_CDC_GETC_PROXY       0x280
#define USB_CDC_TBE_PROXY        0x2A0
#define USB_CDC_PUTC_PROXY       0x2C0
#define USB_CDC_STATE_PROXY      0x2E0

/*
   0-7: compiler scratch
   8-34: isr context save scratch
   35-37: reserved
   38-39: USB_RAM_API
*/
#reserve 0:SCRATCH_RAM_END

#define USB_RAM_API_START        SCRATCH_RAM_END-1
unsigned int16 USB_RAM_API;
#locate USB_RAM_API = USB_RAM_API_START

typedef struct {
        unsigned int32   dwDTERrate;   //data terminal rate, in bits per second
        unsigned int8    bCharFormat;  //num of stop bits (0=1, 1=1.5, 2=2)
        unsigned int8    bParityType;  //parity (0=none, 1=odd, 2=even, 3=mark, 4=space)
        unsigned int8    bDataBits;    //data bits (5,6,7,8 or 16)
        struct
        {
            int1 dte_present; //(aka DTR) 1=DTE present, 0=DTE not present
            int1 active;      //1=activate carrier, 0=deactivate carrier
        } carrier;
} usb_cdc_info_t;

#if !defined(__e3loader)

#build(reset=APPLICATION_START, interrupt=APPLICATION_ISR, stack=21)   //loader uses 10 stack locations in isr
#org LOADER_START, LOADER_END {}
#reserve LOADER_RAM_START:0x2FF

int1 usb_cdc_kbhit(void)
{
  #asm
   call USB_CDC_KBHIT_PROXY
  #endasm
   return(USB_RAM_API);
}

char usb_cdc_getc(void)
{
  #asm
   call USB_CDC_GETC_PROXY
  #endasm
   return(USB_RAM_API);
}

int1 usb_cdc_putready(void)
{
  #asm
   call USB_CDC_TBE_PROXY
  #endasm
   return(USB_RAM_API);
}

void usb_cdc_putc(char c)
{
  #locate c=USB_RAM_API_START
  #asm
   call USB_CDC_PUTC_PROXY
  #endasm
}

int1 usb_enumerated(void)
{
  #asm
   call USB_IS_ENUMRATED_PROXY
  #endasm
   return(USB_RAM_API);
}

void usb_cdc_putc_safe(char c)
{
   if (usb_enumerated() && usb_cdc_putready())
   {
      usb_cdc_putc(c);
   }
}

void usb_cdc_info(usb_cdc_info_t *pInfo)
{
   USB_RAM_API = pInfo;
  #asm
   call USB_CDC_INFO_PROXY
  #endasm
}

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
   
   Returns TRUE if succesfully sent, FALSE if not sent (probably because
   endpoint is full)
*/
int1 usb_cdc_serial_state(cdc_serial_state_t state)
{
   USB_RAM_API = (unsigned int16)state;
  #asm
   call USB_CDC_STATE_PROXY
  #endasm
   return(USB_RAM_API);
}

int1 usb_cdc_opened(void)
{
   usb_cdc_info_t info;
   usb_cdc_info(&info);
   return(info.carrier.active);
}

#define __USB_CDC_HELPERS_ONLY__
#include <stdlib.h>
#include <usb_cdc.h>

#endif   //#if !defined(__e3loader)

#endif   //__E3MINI_H__
