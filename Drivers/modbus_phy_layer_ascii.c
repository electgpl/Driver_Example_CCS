//////////////////////////////////////////////////////////////////////////////////////////
////                            modbus_phy_layer_ascii.c                              ////
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

#ifndef MODBUS_PHY_LAYER_ASCII_C
#define MODBUS_PHY_LAYER_ASCII_C

#include <modbus_phy_layer.h>

#if( MODBUS_SERIAL_INT_SOURCE == MODBUS_INT_RDA )
  #if defined(__PCD__)
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART1, bits=8, stop=1, parity=N, stream=MODBUS_SERIAL, errors)
  #elif MODBUS_PARITY == "EVEN"
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART1, bits=7, stop=1, parity=E, stream=MODBUS_SERIAL, errors)
  #elif MODBUS_PARITY == "ODD"
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART1, bits=7, stop=1, parity=O, stream=MODBUS_SERIAL, errors)
  #else
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART1, bits=7, stop=2, parity=N, stream=MODBUS_SERIAL, errors)
  #endif
   #define RCV_OFF() {disable_interrupts(INT_RDA);}
#elif( MODBUS_SERIAL_INT_SOURCE == MODBUS_INT_RDA2 )
  #if defined(__PCD__)
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART2, bits=8, stop=1, parity=N, stream=MODBUS_SERIAL, errors)
  #elif MODBUS_PARITY == "EVEN"
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART2, bits=7, stop=1, parity=E, stream=MODBUS_SERIAL, errors)
  #elif MODBUS_PARITY == "ODD"
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART2, bits=7, stop=1, parity=O, stream=MODBUS_SERIAL, errors)
  #else
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART2, bits=7, stop=2, parity=N, stream=MODBUS_SERIAL, errors)
  #endif
   #define RCV_OFF() {disable_interrupts(INT_RDA2);}
#elif( MODBUS_SERIAL_INT_SOURCE == MODBUS_INT_RDA3 )
  #if defined(__PCD__)
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART3, bits=8, stop=1, parity=N, stream=MODBUS_SERIAL, errors)
  #elif MODBUS_PARITY == "EVEN"
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART3, bits=7, stop=1, parity=E, stream=MODBUS_SERIAL, errors)
  #elif MODBUS_PARITY == "ODD"
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART3, bits=7, stop=1, parity=O, stream=MODBUS_SERIAL, errors)
  #else
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART3, bits=7, stop=2, parity=N, stream=MODBUS_SERIAL, errors)
  #endif
   #define RCV_OFF() {disable_interrupts(INT_RDA3);}
#elif( MODBUS_SERIAL_INT_SOURCE == MODBUS_INT_RDA4 )
  #if defined(__PCD__)
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART4, bits=8, stop=1, parity=N, stream=MODBUS_SERIAL, errors)
  #elif MODBUS_PARITY == "EVEN"
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART4, bits=7, stop=1, parity=E, stream=MODBUS_SERIAL, errors)
  #elif MODBUS_PARITY == "ODD"
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART4, bits=7, stop=1, parity=O, stream=MODBUS_SERIAL, errors)
  #else
   #use rs232(baud=MODBUS_SERIAL_BAUD, UART4, bits=7, stop=2, parity=N, stream=MODBUS_SERIAL, errors)
  #endif
   #define RCV_OFF() {disable_interrupts(INT_RDA4);}
#elif( MODBUS_SERIAL_INT_SOURCE == MODBUS_INT_EXT )
   #use rs232(baud=MODBUS_SERIAL_BAUD, xmit=MODBUS_SERIAL_TX_PIN, rcv=MODBUS_SERIAL_RX_PIN, bits=8, stop=1, parity=N, stream=MODBUS_SERIAL, disable_ints)
   #if defined(__PCD__)
   #define RCV_OFF() {disable_interrupts(INT_EXT0);}
   #else
   #define RCV_OFF() {disable_interrupts(INT_EXT);}
   #endif
#else
   #error Please define a correct interrupt source
#endif

unsigned int32 modbus_serial_wait=MODBUS_SERIAL_TIMEOUT;

enum {MODBUS_START=0, MODBUS_GETADDY, MODBUS_GETFUNC, MODBUS_GETDATA, MODBUS_STOP} modbus_serial_state=0;

unsigned int8 modbus_serial_lrc;

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

// Purpose:    Calculate crc of data and updates global crc
// Inputs:     Character
// Outputs:    None
void modbus_calc_crc(char data)
{
   modbus_serial_lrc+=data;
}

#if defined(__PCD__)
// Purpose :   Calculate the parity bit (bit 7) when using PCD Compiler
// Input:      Character to send before parity bit is set
// Outputs:    Character to send with parity bit set
unsigned int8 calculate_parity(unsigned int8 value)
{
 #if MODBUS_PARITY == "NONE"
   bit_set(value,7);
 #else
   unsigned int8 count=0;
   unsigned int8 i;

   for(i=0;i<7;i++)
   {
      if(bit_test(value,i))
         count++;
   }

  #if MODBUS_PARITY == "EVEN"
   if(bit_test(count,0))
      bit_clear(value,7);
   else
      bit_set(value,7);
  #elif MODBUS_PARITY == "ODD"
   if(bit_test(count,0))
      bit_set(value,7);
   else
      bit_clear(value,7);
  #endif
 #endif

   return(value);
}
#endif

// Purpose:    Puts a character onto the serial line
// Inputs:     Character
// Outputs:    None
void modbus_serial_putc(unsigned int8 c)
{
   unsigned int8 asciih,asciil;

   asciih=c>>4;
   if(asciih>9)
      asciih+=0x37;
   else
      asciih+=0x30;
   asciil=c&0xF;
   if(asciil>9)
      asciil+=0x37;
   else
      asciil+=0x30;
  #if defined(__PCD__)
   asciih = calculate_parity(asciih);
  #endif
   fputc(asciih,MODBUS_SERIAL);
  #if defined(__PCD__)
   asciil = calculate_parity(asciil);
  #endif
   fputc(asciil,MODBUS_SERIAL);
   modbus_calc_crc(c);
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
   modbus_serial_lrc=0;
   modbus_serial_new=FALSE;

   RCV_OFF();

#if (MODBUS_SERIAL_ENABLE_PIN!=0)
   output_high(MODBUS_SERIAL_ENABLE_PIN);
#endif

   fputc(':',MODBUS_SERIAL);

   modbus_serial_putc(to);
   modbus_serial_putc(func);
}

void modbus_serial_send_stop()
{
   int8 i;

   for(i=0;i<8;i++)
   {
      if(bit_test(modbus_serial_lrc,i))
         bit_clear(modbus_serial_lrc,i);
      else
         bit_set(modbus_serial_lrc,i);
   }
   modbus_serial_lrc++;

   modbus_serial_putc(modbus_serial_lrc);
   fputc('\r',MODBUS_SERIAL);
   fputc('\n',MODBUS_SERIAL);

#if (MODBUS_SERIAL_INT_SOURCE!=MODBUS_INT_EXT)
   WAIT_FOR_HW_BUFFER();
#endif

   RCV_ON();

#if (MODBUS_SERIAL_ENABLE_PIN!=0)
   output_low(MODBUS_SERIAL_ENABLE_PIN);
#endif
   modbus_serial_lrc=0;
}

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
#int_ext0
#else
#int_ext
#endif
#else
#error Please define a correct interrupt source
#endif
void incomming_modbus_serial() {
   char c;
   static int1 two_characters=0;
   static unsigned int8 datah,datal,data;

   c = fgetc(MODBUS_SERIAL) & 0x7F;

   if (!modbus_serial_new)
   {
      if(modbus_serial_state == MODBUS_START)
      {
         if(c==':')
            modbus_serial_state++;
      }
      else if(modbus_serial_state == MODBUS_GETADDY)
      {
         if(!two_characters)
         {
            if(c>=0x41)
               datah=((c-0x37)<<4);
            else
               datah=((c-0x30)<<4);
            modbus_serial_lrc=0;
         }
         else
         {
            if(c>=0x41)
               datal=c-0x37;
            else
               datal=c-0x30;
            data=(datah | datal);
            modbus_rx.address=data;
            modbus_calc_crc(data);
            modbus_serial_state++;
         }
         two_characters++;
      }
      else if(modbus_serial_state == MODBUS_GETFUNC)
      {
         if(!two_characters)
         {
            if(c>=0x41)
               datah=((c-0x37)<<4);
            else
               datah=((c-0x30)<<4);
         }
         else
         {
            if(c>=0x41)
               datal=c-0x37;
            else
               datal=c-0x30;
            data=(datah | datal);
            modbus_rx.func=data;
            modbus_calc_crc(data);
            modbus_serial_state++;
            modbus_rx.len=0;
            modbus_rx.error=0;
         }
         two_characters++;
      }
      else if(modbus_serial_state == MODBUS_GETDATA)
      {
         if(c=='\r')
         {
            modbus_serial_state++;
            modbus_rx.len--;
            modbus_serial_lrc-=data;
         }
         else if(!two_characters)
         {
            if(c>=0x41)
               datah=((c-0x37)<<4);
            else
               datah=((c-0x30)<<4);
            two_characters++;
         }
         else
         {
            if(c>=0x41)
               datal=c-0x37;
            else
               datal=c-0x30;
            data=(datah | datal);
            if (modbus_rx.len>=MODBUS_SERIAL_RX_BUFFER_SIZE)
               modbus_rx.len=MODBUS_SERIAL_RX_BUFFER_SIZE-1;
            modbus_rx.data[modbus_rx.len]=data;
            modbus_rx.len++;
            modbus_calc_crc(data);
            two_characters++;
         }
      }
      else if(modbus_serial_state==MODBUS_STOP)
      {
         if(c=='\n')
         {
            modbus_serial_lrc=((0xFF-modbus_serial_lrc)+1);
            if(modbus_serial_lrc==data)
               modbus_serial_new=TRUE;
         }
         modbus_serial_state=MODBUS_START;
         two_characters=0;
      }
   }
   #if (MODBUS_TYPE == MODBUS_TYPE_MASTER)
      modbus_serial_wait=MODBUS_SERIAL_TIMEOUT;
   #endif
}
#endif //MODBUS_PHY_LAYER_ASCII_C
