//////////////////////////////////////////////////////////////////////////////////////////
////                              modbus_phy_layer_rtu.c                              ////
////                                                                                  ////
////      Physical layer of the MODBUS protocol driver for serial communications.     ////
////                                                                                  ////
////  Refer to documentation at http://www.modbus.org for more information on MODBUS. ////
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

#ifndef MODBUS_PHY_LAYER_RTU_C
#define MODBUS_PHY_LAYER_RTU_C

#include <modbus_phy_layer.h>

#if (MODBUS_TIMER_USED == MODBUS_TIMER_T1)
  #if (MODBUS_TIMER_UPDATE == MODBUS_TIMER_ISR)
    #USE TIMER(TIMER=1,TICK=.1ms,BITS=16, ISR)
  #else
    #USE TIMER(TIMER=1,TICK=.1ms,BITS=16, NOISR)
  #endif
#elif (MODBUS_TIMER_USED == MODBUS_TIMER_T2)
  #if (MODBUS_TIMER_UPDATE == MODBUS_TIMER_ISR)
    #USE TIMER(TIMER=2,TICK=.1ms,BITS=16, ISR)
  #else
    #USE TIMER(TIMER=2,TICK=.1ms,BITS=16, NOISR)
  #endif
#endif

#define MODBUS_GETDATA_TIMEOUT 40

#if( MODBUS_SERIAL_INT_SOURCE == MODBUS_INT_RDA )
  #if MODBUS_PARITY == "EVEN"
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART1, bits=8, stop=1, parity=E, stream=MODBUS_SERIAL, errors)
  #elif MODBUS_PARITY == "ODD"
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART1, bits=8, stop=1, parity=O, stream=MODBUS_SERIAL, errors)
  #else
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART1, bits=8, stop=2, parity=N, stream=MODBUS_SERIAL, errors)
  #endif
   #define RCV_OFF() {disable_interrupts(INT_RDA);}
#elif( MODBUS_SERIAL_INT_SOURCE == MODBUS_INT_RDA2 )
  #if MODBUS_PARITY == "EVEN"
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART2, bits=8, stop=1, parity=E, stream=MODBUS_SERIAL, errors)
  #elif MODBUS_PARITY == "ODD"
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART2, bits=8, stop=1, parity=O, stream=MODBUS_SERIAL, errors)
  #else
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART2, bits=8, stop=2, parity=N, stream=MODBUS_SERIAL, errors)
  #endif
   #define RCV_OFF() {disable_interrupts(INT_RDA2);}
#elif( MODBUS_SERIAL_INT_SOURCE == MODBUS_INT_RDA3 )
  #if MODBUS_PARITY == "EVEN"
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART3, bits=8, stop=1, parity=E, stream=MODBUS_SERIAL, errors)
  #elif MODBUS_PARITY == "ODD"
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART3, bits=8, stop=1, parity=O, stream=MODBUS_SERIAL, errors)
  #else
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART3, bits=8, stop=2, parity=N, stream=MODBUS_SERIAL, errors)
  #endif
   #define RCV_OFF() {disable_interrupts(INT_RDA3);}
#elif( MODBUS_SERIAL_INT_SOURCE == MODBUS_INT_RDA4 )
  #if MODBUS_PARITY == "EVEN"
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART4, bits=8, stop=1, parity=E, stream=MODBUS_SERIAL, errors)
  #elif MODBUS_PARITY == "ODD"
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART4, bits=8, stop=1, parity=O, stream=MODBUS_SERIAL, errors)
  #else
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART4, bits=8, stop=2, parity=N, stream=MODBUS_SERIAL, errors)
  #endif
   #define RCV_OFF() {disable_interrupts(INT_RDA4);}
#elif( MODBUS_SERIAL_INT_SOURCE == MODBUS_INT_EXT )
  #if MODBUS_PARITY == "EVEN"
   #use rs232(baud=MODBUS_SERIAL_BAUD, xmit=MODBUS_SERIAL_TX_PIN, rcv=MODBUS_SERIAL_RX_PIN, bits=8, stop=1, parity=E, stream=MODBUS_SERIAL, disable_ints)
  #elif MODBUS_PARITY == "ODD"
   #use rs232(baud=MODBUS_SERIAL_BAUD, xmit=MODBUS_SERIAL_TX_PIN, rcv=MODBUS_SERIAL_RX_PIN, bits=8, stop=1, parity=O, stream=MODBUS_SERIAL, disable_ints)
  #else
   #use rs232(baud=MODBUS_SERIAL_BAUD, xmit=MODBUS_SERIAL_TX_PIN, rcv=MODBUS_SERIAL_RX_PIN, bits=8, stop=2, parity=N, stream=MODBUS_SERIAL, disable_ints)
  #endif
   #if defined(__PCD__)
   #define RCV_OFF() {disable_interrupts(INT_EXT0);}
   #else
   #define RCV_OFF() {disable_interrupts(INT_EXT);}
   #endif
#else
   #error Please define a correct interrupt source
#endif

/* status of between byte timeout */
int1 modbus_timeout_enabled = false;

#if (MODBUS_TYPE == MODBUS_TYPE_MASTER)
unsigned int32 modbus_serial_wait=MODBUS_SERIAL_TIMEOUT;
#endif

/*Stages of MODBUS reception.  Used to keep our ISR fast enough.*/
enum {MODBUS_GETADDY=0, MODBUS_GETFUNC=1, MODBUS_GETDATA=2} modbus_serial_state = 0;


/*Global value holding our current CRC value.*/
union
{
   unsigned int8 b[2];
   unsigned int16 d;
} modbus_serial_crc;


/* Table of CRC values for high–order byte */
const unsigned char modbus_auchCRCHi[] = {
0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,
0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,
0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,
0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,
0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,
0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,
0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,
0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,
0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,
0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,
0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,
0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x01,0xC0,
0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,
0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,
0x40
};

/* Table of CRC values for low–order byte */
const unsigned char modbus_auchCRCLo[] = {
0x00,0xC0,0xC1,0x01,0xC3,0x03,0x02,0xC2,0xC6,0x06,0x07,0xC7,0x05,0xC5,0xC4,
0x04,0xCC,0x0C,0x0D,0xCD,0x0F,0xCF,0xCE,0x0E,0x0A,0xCA,0xCB,0x0B,0xC9,0x09,
0x08,0xC8,0xD8,0x18,0x19,0xD9,0x1B,0xDB,0xDA,0x1A,0x1E,0xDE,0xDF,0x1F,0xDD,
0x1D,0x1C,0xDC,0x14,0xD4,0xD5,0x15,0xD7,0x17,0x16,0xD6,0xD2,0x12,0x13,0xD3,
0x11,0xD1,0xD0,0x10,0xF0,0x30,0x31,0xF1,0x33,0xF3,0xF2,0x32,0x36,0xF6,0xF7,
0x37,0xF5,0x35,0x34,0xF4,0x3C,0xFC,0xFD,0x3D,0xFF,0x3F,0x3E,0xFE,0xFA,0x3A,
0x3B,0xFB,0x39,0xF9,0xF8,0x38,0x28,0xE8,0xE9,0x29,0xEB,0x2B,0x2A,0xEA,0xEE,
0x2E,0x2F,0xEF,0x2D,0xED,0xEC,0x2C,0xE4,0x24,0x25,0xE5,0x27,0xE7,0xE6,0x26,
0x22,0xE2,0xE3,0x23,0xE1,0x21,0x20,0xE0,0xA0,0x60,0x61,0xA1,0x63,0xA3,0xA2,
0x62,0x66,0xA6,0xA7,0x67,0xA5,0x65,0x64,0xA4,0x6C,0xAC,0xAD,0x6D,0xAF,0x6F,
0x6E,0xAE,0xAA,0x6A,0x6B,0xAB,0x69,0xA9,0xA8,0x68,0x78,0xB8,0xB9,0x79,0xBB,
0x7B,0x7A,0xBA,0xBE,0x7E,0x7F,0xBF,0x7D,0xBD,0xBC,0x7C,0xB4,0x74,0x75,0xB5,
0x77,0xB7,0xB6,0x76,0x72,0xB2,0xB3,0x73,0xB1,0x71,0x70,0xB0,0x50,0x90,0x91,
0x51,0x93,0x53,0x52,0x92,0x96,0x56,0x57,0x97,0x55,0x95,0x94,0x54,0x9C,0x5C,
0x5D,0x9D,0x5F,0x9F,0x9E,0x5E,0x5A,0x9A,0x9B,0x5B,0x99,0x59,0x58,0x98,0x88,
0x48,0x49,0x89,0x4B,0x8B,0x8A,0x4A,0x4E,0x8E,0x8F,0x4F,0x8D,0x4D,0x4C,0x8C,
0x44,0x84,0x85,0x45,0x87,0x47,0x46,0x86,0x82,0x42,0x43,0x83,0x41,0x81,0x80,
0x40
};

// Purpose:    Enable data reception
// Inputs:     None
// Outputs:    None
void RCV_ON(void)
{
   #if (MODBUS_SERIAL_INT_SOURCE!=MODBUS_INT_EXT)
      while(kbhit(MODBUS_SERIAL)) {fgetc(MODBUS_SERIAL);}  //Clear RX buffer. Clear RDA interrupt flag. Clear overrun error flag.
      #if (MODBUS_SERIAL_INT_SOURCE==MODBUS_INT_RDA)
         clear_interrupt(INT_RDA);
      #elif (MODBUS_SERIAL_INT_SOURCE==MODBUS_INT_RDA2)
         clear_interrupt(INT_RDA2);
      #elif (MODBUS_SERIAL_INT_SOURCE==MODBUS_INT_RDA3)
         clear_interrupt(INT_RDA3);
      #else
         clear_interrupt(INT_RDA4);
      #endif

      #if (MODBUS_SERIAL_RX_ENABLE!=0)
         output_low(MODBUS_SERIAL_RX_ENABLE);
      #endif

      #if (MODBUS_SERIAL_INT_SOURCE==MODBUS_INT_RDA)
        enable_interrupts(INT_RDA);
      #elif (MODBUS_SERIAL_INT_SOURCE==MODBUS_INT_RDA2)
        enable_interrupts(INT_RDA2);
      #elif (MODBUS_SERIAL_INT_SOURCE==MODBUS_INT_RDA3)
        enable_interrupts(INT_RDA3);
      #else
        enable_interrupts(INT_RDA4);
      #endif
   #else
      #if defined(__PCD__)
         clear_interrupt(INT_EXT0);
      #else
         clear_interrupt(INT_EXT);
      #endif

      ext_int_edge(H_TO_L);

      #if (MODBUS_SERIAL_RX_ENABLE!=0)
         output_low(MODBUS_SERIAL_RX_ENABLE);
      #endif

      #if defined(__PCD__)
         enable_interrupts(INT_EXT0);
      #else
         enable_interrupts(INT_EXT);
      #endif
   #endif
}

// Purpose:    Start our timeout timer
// Inputs:     Enable, used to turn timer on/off
// Outputs:    None
// Not used for ASCII mode
void modbus_enable_timeout(int1 enable)
{
   modbus_timeout_enabled = enable;
   set_ticks(0);
}

// Purpose:    Handles a timeout when waiting for a response
// Inputs:     None
// Outputs:    None
// Not used for ASCII mode
void modbus_timeout_now(void)
{
   if((modbus_serial_state == MODBUS_GETDATA) && (modbus_serial_crc.d == 0x0000) && (!modbus_serial_new))
   {
      modbus_rx.len-=2;
      modbus_serial_new=TRUE;
   } else {
      modbus_serial_new=FALSE;
   }

   modbus_serial_crc.d=0xFFFF;
   modbus_serial_state=MODBUS_GETADDY;
   modbus_enable_timeout(FALSE);
}

// Purpose:    Check if we have timed out waiting for a response
// Inputs:     None
// Outputs:    None
// Not used for ASCII mode
void modbus_check_timeout(void)
{
   #if (MODBUS_TIMER_UPDATE == MODBUS_TIMER_NOISR)
   //get_ticks() must be called more often than the timer overflow
   //rate, and the get_ticks() below will not always be called
   //due to short circuit evaluation
   get_ticks();
   #endif
   //modbus_timeout_enabled must be checked before get_ticks()
   //so that if an interrupt happens it cannot be enabled after
   //an old timer value is used in comparison
   if(modbus_timeout_enabled && (get_ticks() > MODBUS_GETDATA_TIMEOUT))
   {
     modbus_timeout_now();
   }
}

// Purpose:    Calculate crc of data and updates global crc
// Inputs:     Character
// Outputs:    None
void modbus_calc_crc(char data)
{
   unsigned int8 uIndex ; // will index into CRC lookup table

   uIndex = (modbus_serial_crc.b[1]) ^ data; // calculate the CRC
   modbus_serial_crc.b[1] = (modbus_serial_crc.b[0]) ^ modbus_auchCRCHi[uIndex];
   modbus_serial_crc.b[0] = modbus_auchCRCLo[uIndex];
}

// Purpose:    Puts a character onto the serial line
// Inputs:     Character
// Outputs:    None
void modbus_serial_putc(unsigned int8 c)
{
   fputc(c, MODBUS_SERIAL);
   modbus_calc_crc(c);
   delay_us(1000000/MODBUS_SERIAL_BAUD); //one stop bit.  not exact
}

// Purpose:    Send a message over the RS485 bus
// Inputs:     1) The destination address
//             2) The number of bytes of data to send
//             3) A pointer to the data to send
//             4) The length of the data
// Outputs:    TRUE if successful
//             FALSE if failed
// Note:       Format:  source | destination | data-length | data | checksum
void modbus_serial_send_start(unsigned int8 to, unsigned int8 func)
{
   modbus_serial_crc.d=0xFFFF;
   modbus_serial_new=FALSE;

   RCV_OFF();

#if (MODBUS_SERIAL_ENABLE_PIN!=0)
   output_high(MODBUS_SERIAL_ENABLE_PIN);
#endif

   delay_us(3500000/MODBUS_SERIAL_BAUD); //3.5 character delay

   modbus_serial_putc(to);
   modbus_serial_putc(func);
}

// Purpose:    Ends a message over the RS485 Bus
// Inputs:     Character
// Outputs:    None
void modbus_serial_send_stop()
{
   unsigned int8 crc_low, crc_high;

   crc_high=modbus_serial_crc.b[1];
   crc_low=modbus_serial_crc.b[0];

   modbus_serial_putc(crc_high);
   modbus_serial_putc(crc_low);

#if (MODBUS_SERIAL_INT_SOURCE!=MODBUS_INT_EXT)
   WAIT_FOR_HW_BUFFER();
#endif

   delay_us(3500000/MODBUS_SERIAL_BAUD); //3.5 character delay

   RCV_ON();

#if (MODBUS_SERIAL_ENABLE_PIN!=0)
   output_low(MODBUS_SERIAL_ENABLE_PIN);
#endif

   modbus_serial_crc.d=0xFFFF;
}

//////////////////////////////////////////////////////////////////////////////////////////
//// Interrupts                                                                       ////
//////////////////////////////////////////////////////////////////////////////////////////

#if (MODBUS_SERIAL_INT_SOURCE==MODBUS_INT_RDA)
#int_rda
#elif (MODBUS_SERIAL_INT_SOURCE==MODBUS_INT_RDA2)
#int_rda2
#elif (MODBUS_SERIAL_INT_SOURCE==MODBUS_INT_RDA3)
#int_rda3
#elif (MODBUS_SERIAL_INT_SOURCE==MODBUS_INT_RDA4)
#int_rda4
#elif (MODBUS_SERIAL_INT_SOURCE==MODBUS_INT_EXT)
#if defined(__PCD__)
#int_ext1
#else
#int_ext
#endif
#else
#error Please define a correct interrupt source
#endif
void incomming_modbus_serial() {
   char c;

   c=fgetc(MODBUS_SERIAL);

   if (!modbus_serial_new)
   {
      if(modbus_serial_state == MODBUS_GETADDY)
      {
         modbus_serial_crc.d = 0xFFFF;
         modbus_rx.address = c;
         modbus_serial_state++;
         modbus_rx.len = 0;
         modbus_rx.error=0;
      }
      else if(modbus_serial_state == MODBUS_GETFUNC)
      {
         modbus_rx.func = c;
         modbus_serial_state++;
      }
      else if(modbus_serial_state == MODBUS_GETDATA)
      {
         if (modbus_rx.len>=MODBUS_SERIAL_RX_BUFFER_SIZE)
       {
         modbus_rx.len=MODBUS_SERIAL_RX_BUFFER_SIZE-1;
       }
         modbus_rx.data[modbus_rx.len]=c;
         modbus_rx.len++;
     }
     modbus_enable_timeout(TRUE);
     modbus_calc_crc(c);
   }

   #if (MODBUS_TYPE == MODBUS_TYPE_MASTER)
      modbus_serial_wait=MODBUS_SERIAL_TIMEOUT;
   #endif
}
#endif //MODBUS_PHY_LAYER_RTU_C
