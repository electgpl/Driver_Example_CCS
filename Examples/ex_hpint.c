/////////////////////////////////////////////////////////////////////////
////                           EX_HIGH_INTS.C                        ////
////                                                                 ////
////  This program demonstrates the use of high-priority interrupts  ////
////  on the PIC18.  High-priority interrupts are enabled with the   ////
////  #device HIGH_INTS=TRUE directive and the word HIGH following   ////
////  the interrupt service routine identifier.                      ////
////                                                                 ////
////  Compile and run this program twice, once with high-priority    ////
////  interrupts and once without.  With high-priority interrupts    ////
////  enabled, a count will appear on the RS232 approximately once   ////
////  per second; with high-priority interrupts disabled, this rate  ////
////  is reduced to once every four seconds.                         ////
////                                                                 ////
////  This example will work with the PCM compiler.  Change the      ////
////  device and clock for your hardware if needed.                  ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2005 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#include <18f452.h>
//#device HIGH_INTS=true
#fuses HS, NOWDT, PUT, NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

int32 count = 0;

#INT_TIMER1
void timer1_isr()
{
	delay_ms(10);		//should block about 195 out of every 256 timer2 interrupts
}

#INT_TIMER2 //HIGH
void timer2_isr()
{
	count++;
}

void main()
{

	setup_timer_1(T1_INTERNAL | T1_DIV_BY_1);			//13.1072ms
	setup_timer_2(T2_DIV_BY_1, 0xff, 1);				//51.2us
	set_timer1(0);
	set_timer2(0);

	enable_interrupts(INT_TIMER1);
	enable_interrupts(INT_TIMER2);

	enable_interrupts(GLOBAL);
	
	printf("\r\nStarting...");

	while(true)
	{
		if(count >= 20000)
		{
			printf("\r\ncount = %lu", count);
			count = 0;
		}
	}
}
