/////////////////////////////////////////////////////////////////////////
////                           EX_PSP.C                              ////
////                                                                 ////
////  This program demonstrates how the parallel slave port works.   ////
////  This example allows the user to print text from a PC to a PIC. ////
////  To run this example correctly, an IBM parallel printer cable   ////
////  is needed, with the connections shown below.  After installing ////
////  the generic text printer in Windows, text can be printed to    ////
////  the PIC using a text editor.  The text will be displayed via   ////
////  the RS-232.                                                    ////
////                                                                 ////
////  This example also shows how to use banks 2 and 3 in 4 bank     ////
////  parts without going to 16 bit pointers (saving ROM).           ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////     Connect pin E2 to GND and pin E0 to +5V.                    ////
////     Connect one end of the parrallel printer cable to the PC.   ////
////     Also make the following connections:                        ////
////          IBM parallel                                           ////
////           cable pin     Protoboard                              ////
////               1           pin E1                                ////
////               2           pin D0                                ////
////               3           pin D1                                ////
////               4           pin D2                                ////
////               5           pin D3                                ////
////               6           pin D4                                ////
////               7           pin D5                                ////
////               8           pin D6                                ////
////               9           pin D7                                ////
////              11           pin C0                                ////
////              12           gnd                                   ////
////              13           +5V                                   ////
////              15           +5V                                   ////
////              18           gnd                                   ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////                                                                 ////
////  This example will work with the PCM and PCH compilers.  The    ////
////  following conditional compilation lines are used to include a  ////
////  valid device for each compiler.  Change the device, clock and  ////
////  RS232 pins for your hardware if needed.                        ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////


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
#error  For PIC18 change read/write _bank to use a buffer
#endif

#define BUSY_LINE    PIN_C0
#define BUFFER_SIZE  96


int next_in = 0;
int next_out = 0;
short data_lost = TRUE;

#int_psp
void psp_isr()  {

   if(psp_overflow())
     data_lost=TRUE;

   if(psp_input_full()) {
      write_bank(2, next_in++, input_D());
      if(next_in == BUFFER_SIZE)
         next_in = 0;
      if(next_in == next_out)
         output_high(BUSY_LINE);
   }
}


void main() {

   setup_adc_ports(NO_ANALOGS);
   setup_psp(PSP_ENABLED);
   enable_interrupts(GLOBAL);
   enable_interrupts(INT_PSP);

   output_low(BUSY_LINE);

   printf("Waiting for print data... \r\n\n");

   while(true)
   {
      if(next_in!=next_out)
      {
         putc(read_bank(2,next_out));
         if(++next_out==BUFFER_SIZE)
            next_out=0;
         if(data_lost) {
            printf("\r\nData Lost!!!\r\n");
            data_lost = FALSE;
         }
         output_low(BUSY_LINE);
      }
   }
}

