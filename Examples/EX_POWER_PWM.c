/////////////////////////////////////////////////////////////////////////
////                             EX_POWER_PWM.C                      ////
////                                                                 ////
////  This program will show how to use the built in PPOWER PWM.     ////
////  The program takes an analog input and uses the digital         ////
////  value to set the duty cycle.  The frequency is set by          ////
////  the user over the RS-232.                                      ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////      Connect a scope to Power PWM channels 0 and 1.             ////
////      Connect pin A0 to output of the POT                        ////
////                                                                 ////
////  This example will work with the PCH compiler.                  ////
////  The following conditional compilation lines are used to        ////
////  include a valid device for each compiler.  Change the device,  ////
////  clock and RS232 pins for your hardware if needed.              ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////


#include <18f2431.h>
#device adc=10
#fuses  HS,NOWDT,NOPROTECT
#use delay(clock=10000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

void main()
{
	long int adc_val, period;
	char ch;

	printf("Select frequency:\r\n 1: 610hz\r\n 2: 1220hz\r\n 3: 2441hz\r\n 4: 4882hz\r\n");
	if(kbhit())
		getc();
	do
	{
		ch = getc();
	}while(ch < '1' || ch > '4');
	putc(ch);
	switch(ch)
	{
		case '1':
			period = 4095;	//Frequency = Fosc / (4 * (period+1) * postscale)
			break;
		case '2':
			period = 2047;
			break;
		case '3':
			period = 1023;
			break;
		case '4':
			period = 511;
			break;
		default:
			printf("\r\nerror");
			while(1);
			break;
	}
	setup_power_pwm_pins(PWM_COMPLEMENTARY,PWM_OFF,PWM_OFF,PWM_OFF);
		//PPWM channels 0 and 1 are both on and always opposite values
	setup_power_pwm(PWM_CLOCK_DIV_4|PWM_FREE_RUN,1,0,period,0,1,0);
		//add dead time for reactive loads

	setup_adc_ports(ADC_CONT_A | ADC_WHEN_PPWM);	//continuously sample and sync with PPWM
	setup_adc(ADC_CLOCK_INTERNAL);
	set_adc_channel(0);

	while(true)
	{
		adc_val = read_adc();						//connect to pot to adjust duty cycle
		set_power_pwm0_duty(adc_val * ((period>>8)+1));	//max ADC value is 1023
																		//max duty cycle is (period+1)*4-1
																		//basically the same as max_adc * period/256
   }
}
