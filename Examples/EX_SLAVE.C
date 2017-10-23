///////////////////////////////////////////////////////////////////////////
////                         EX_SLAVE.C                                ////
////                                                                   ////
////  This program uses the PIC in I2C slave mode to emulate the       ////
////  24LC01 EEPROM. You can write to addresses 00h to 0Fh with it.    ////
////                                                                   ////
////  This program is to be used in conjunction with the ex_extee.c    ////
////  sample.  Use the "#include <2402.C>" or "#include <2401.c>".     ////
////  Only 16 bytes of address space are implemented, however.         ////
////                                                                   ////
////  If using a compiler version before 2.639 add "*0x14 = 0x3E;" to  ////
////  the begining of main(), and add "NOFORCE_SW" as the last         ////
////  parameter in the #use i2c directive.                             ////
////                                                                   ////
////  Jumpers:                                                         ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX             ////
////                                                                   ////
////  This example will work with the PCM and PCH compilers.  The      ////
////  following conditional compilation lines are used to include a    ////
////  valid device for each compiler.  Change the device, clock and    ////
////  RS232 pins for your hardware if needed.                          ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS    ////
//// C compiler.  This source code may only be distributed to other    ////
//// licensed users of the CCS C compiler.  No other use,              ////
//// reproduction or distribution is permitted without written         ////
//// permission.  Derivative programs created using this software      ////
//// in object code form are not restricted in any way.                ////
///////////////////////////////////////////////////////////////////////////


#if defined(__PCM__)
#include <16F877.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCH__)
#include <18F452.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined (__PCD__)
#include <24FJ128GA006.h>
#fuses PR,HS,NOWDT
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_F3, rcv=PIN_F2)
#endif

#use i2c(SLAVE, I2C1, address=0xA0)

unsigned int8 address, buffer[16];

#if defined(__PCD__)
#INT_SI2C
void si2c_interrupt()
#else
#INT_SSP
void ssp_interrupt ()
#endif
{
   unsigned int8 incoming, state;

   state = i2c_isr_state();

   if(state <= 0x80)                      //Master is sending data
   {
      if(state == 0x80)
         incoming = i2c_read(2);          //Passing 2 as parameter, causes the function to read the SSPBUF without releasing the clock
      else
         incoming = i2c_read();

      if(state == 1)                      //First received byte is address
         address = incoming;
      else if(state >= 2 && state != 0x80)   //Received byte is data
         buffer[address++] = incoming;
   }

   if(state >= 0x80)                      //Master is requesting data
   {
      i2c_write(buffer[address++]);
   }
}

void main ()
{
   #if defined(__PCD__)
   enable_interrupts(INTR_GLOBAL);
   enable_interrupts(INT_SI2C);
   #else
   enable_interrupts(GLOBAL);
   enable_interrupts(INT_SSP);
   #endif

   while (TRUE)
   {
   }
}
