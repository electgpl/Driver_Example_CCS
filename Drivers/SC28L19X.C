////////////////////////////////////////////////////////////////////////////
////                Library for a Philips SC28L19x UART                 ////
////                                                                    ////
//// This driver was written and tested specifically with the SC28L194. ////
//// The constants provided in this driver assume you are clocking the  ////
//// UART with a 3.6864MHz clock.  If you wish to use a differnt clock  ////
//// you must change the defines below.                                 ////
////                                                                    ////
//// LIBRARY FUNCTIONS:                                                 ////
////                                                                    ////
//// ext_uart_init(port, baud, parity, bits)                            ////
////   Configure specified port to user baud, parity, and bit length.   ////
////   This funciton must be run for each port before use.              ////
////                                                                    ////
//// ext_uart_kbhit(port)                                               ////
////   Returns true if there is received data on the specified port.    ////
////   Equivalent to ANSI C's kbhit().                                  ////
////                                                                    ////
//// char ext_uart_getc(port)                                           ////
////    Gets a character from the UART's specified port.  Equivalent to ////
////    ANSI C's getc().  Like getc(), this function will wait          ////
////    indefinately until data is available.                           ////
////                                                                    ////
//// ext_uart_putc(port, c)                                             ////
////    Sends a character, c, out the UART's specified port.            ////
////    Equivalent to ANSI C's putc().                                  ////
////                                                                    ////
////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services            ////
//// This source code may only be used by licensed users of the CCS C   ////
//// compiler.  This source code may only be distributed to other       ////
//// licensed users of the CCS C compiler.  No other use, reproduction  ////
//// or distribution is permitted without written permission.           ////
//// Derivative programs created using this software in object code     ////
//// form are not restricted in any way.                                ////
////////////////////////////////////////////////////////////////////////////

//how your pic is connected to the philips uart
#ifndef SC28L19x_CEN
   #define SC28L19x_CEN          PIN_C5
   #define SC28L19x_RW           PIN_C3
   #define SC28L19x_ADDR(x)      output_b(x)
   #define SC28L19x_DATA(x)      output_d(x)
   #define SC28L19x_TRIS_DATA(x) set_tris_d(x)
   #define SC28L19x_INPUT        input_d()
#endif

//ext_uart_init() baud settings when uart is clocked by a 3.6864MHz source
//if you want to use a different clock then these values change
#define BAUD_50                  0x00
#define BAUD_75                  0x01
#define BAUD_150                 0x02
#define BAUD_200                 0x03
#define BAUD_300                 0x04
#define BAUD_450                 0x05
#define BAUD_600                 0x06
#define BAUD_900                 0x07
#define BAUD_1200                0x08
#define BAUD_1800                0x09
#define BAUD_2400                0x0A
#define BAUD_3600                0x0B
#define BAUD_4800                0x0C
#define BAUD_7200                0x0D
#define BAUD_9600                0x0E
#define BAUD_14400               0x0F
#define BAUD_19200               0x10
#define BAUD_28800               0x11
#define BAUD_38400               0x12
#define BAUD_57600               0x13
#define BAUD_115200              0x14
#define BAUD_234000              0x15

//ext_uart_init() bit settings
#define BITS_5                   0x00
#define BITS_6                   0x01
#define BITS_7                   0x02
#define BITS_8                   0x03

//ext_uart_init() parity settings
#define PARITY_NONE              0x10
#define PARITY_EVEN              0x00
#define PARITY_ODD               0x04

//sc28l19x registers - do not change
#define SC28L19x_GCCR            0x0F
#define SC28L19x_MR0             0x00
#define SC28L19x_MR1             0x01
#define SC28L19x_MR2             0x80
#define SC28L19x_RXCSR           0x0C
#define SC28L19x_TXCSR           0x0E
#define SC28L19x_CR              0x81
#define SC28L19x_RXFIFO          0x83
#define SC28L19x_TXFIFO          0x83
#define SC28L19x_SR              0x81
#define SC28L19x_IOPCR           0x02
#define SC28L19x_IOPIOR          0x85

#define SC28L19x_PORTS           8

//writes a value to a sc28l19x register
void sc28l19x_write(BYTE reg, BYTE value) {
   output_high(SC28L19x_RW);
   SC28L19x_TRIS_DATA(0);
   SC28L19x_DATA(value);
   SC28L19x_ADDR(reg);
   output_low(SC28L19x_CEN);
   output_high(SC28L19x_CEN);
   SC28L19x_TRIS_DATA(0xFF);
}

//reads a value from a sc28l19x register
BYTE sc28l19x_read(BYTE reg) {
   BYTE value;

   output_low(SC28L19x_RW);
   SC28L19x_ADDR(reg);
   output_low(SC28L19x_CEN);
   delay_cycles(1);
   value=SC28L19x_INPUT;
   output_high(SC28L19x_CEN);

   return(value);
}

//initializes the external uart
void ext_uart_init(int port, int baud, int parity, int bits) {
   BYTE i,n;

   sc28l19x_write(SC28L19x_GCCR,0);          /// Need to add parity and bits to init
   sc28l19x_write(SC28L19x_GCCR,0);
   n=port<<4;
   sc28l19x_write(SC28L19x_MR0 | n, 0x30);            //txint on not full
   sc28l19x_write(SC28L19x_MR1 | n, parity | bits);
   sc28l19x_write(SC28L19x_MR2 | n, 0);               //stop=1 bit
   sc28l19x_write(SC28L19x_RXCSR | n, baud);
   sc28l19x_write(SC28L19x_TXCSR | n, baud);
   sc28l19x_write(SC28L19x_CR | n, 3);                //enable TX and RX
}

//macro returns true if data is received on specified port
#define ext_uart_kbhit(port)     (sc28l19x_read(SC28L19x_SR | (port<<4)) & 1)

//get a character from specified port
char ext_uart_getc(int port) {
   while (!ext_uart_kbhit(port)) {}
   return( sc28l19x_read(SC28L19x_RXFIFO | (port<<4)) );
}

//send a character out the specified port
void ext_uart_putc(int port,char c) {

   while ( !(sc28l19x_read(SC28L19x_SR | (port<<4) ) & 4) ) {} //wait until TxRDY is set
   sc28l19x_write(SC28L19x_TXFIFO |  (port<<4), c);
}
