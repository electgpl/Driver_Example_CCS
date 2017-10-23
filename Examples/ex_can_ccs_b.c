/////////////////////////////////////////////////////////////////////////
////                         EX_CAN_CCS_B.C                          ////
////                                                                 ////
//// Example of CCS's MCP2510 CAN library.  This example was tested  ////
//// using MCP250xxx CCS's CAN Prototype boards                      ////
////                                                                 ////
//// This example provides the firmware for the B node in CCS's      ////
//// CAN prototyping boards.  Node B responds to CAN ID 0x202        ////
//// by setting the 3 LEDs to the value transmitted by Node A.       ////
//// Node B also repsonds to requests from CAN ID 0x201 by           ////
//// transmitting an A/D reading.                                    ////
////                                                                 ////
//// Using a serial port, this example also shows all traffic on the ////
//// CAN bus as received by the MCP2510.                             ////
////                                                                 ////
//// For more documentation on the MPC2510 CAN library, see          ////
//// can-mcp2510.c                                                   ////
////                                                                 ////
//// For more documentation on the CCS Can Prototype boards see      ////
//// ex_can_ccs_a.c                                                  ////
////                                                                 ////
////  This example will work with the PCM and PCD compiler.          ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2010 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////



#if defined(__PCD__)
 #include <30F4012.h>
 #fuses FRC,NOWDT
 #device ADC=8
 #use delay(clock=7.37MHz)
 #use rs232(UART1A,baud=9600)
 #define PIN_LED1    PIN_E1
 #define PIN_LED2    PIN_E2
 #define PIN_LED3    PIN_E4
#else
 #include <16F876.h>
 #fuses HS,NOPROTECT,NOLVP,NOWDT
 #use delay(clock=2500000)
 #use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
 #define PIN_LED1  PIN_A1
 #define PIN_LED2  PIN_A2
 #define PIN_LED3  PIN_A3
#endif

#define CAN_DO_DEBUG TRUE

#include <can-mcp2510.c>

#define LED1_HIGH output_low(PIN_LED1)
#define LED1_LOW  output_high(PIN_LED1)
#define LED2_HIGH output_low(PIN_LED2)
#define LED2_LOW  output_high(PIN_LED2)
#define LED3_HIGH output_low(PIN_LED3)
#define LED3_LOW  output_high(PIN_LED3)

int16 ms;

#int_timer2
void isr_timer2(void) {
   ms++; //keep a running timer that increments every milli-second
}

#define RESPOND_TO_ID_AD   0x201
#define RESPOND_TO_ID_LED  0x202

void main() {
   struct rx_stat rxstat;
   unsigned int32 rx_id;
   unsigned int8 buffer[8];
   unsigned int8 rx_len;

   unsigned int8 i;
   
   #if defined(__PCD__)
      setup_adc(ADC_CLOCK_INTERNAL | ADC_TAD_MUL_31);
      setup_adc_ports(sAN5 | VSS_VDD);
      set_adc_channel(5);
   #else
      setup_port_a(RA0_ANALOG);
      setup_adc(ADC_CLOCK_INTERNAL);
      set_adc_channel(0);
   #endif

   for(i=0;i<8;i++) {
      buffer[i]=0;
   }

   LED1_HIGH;
   LED2_HIGH;
   LED3_HIGH;
   printf("\r\n\r\nCCS CAN EXAMPLE\r\n");
   delay_ms(1000);
   LED1_LOW;
   LED2_LOW;
   LED3_LOW;
   
   #if defined(__PCD__)
      setup_timer2(TMR_INTERNAL,1842);
   #else
      setup_timer_2(T2_DIV_BY_4,53,3);
   #endif

   can_init();

   enable_interrupts(INT_TIMER2);   //enable timer2 interrupt
   #if defined(__PCD__)
      enable_interrupts(INTR_GLOBAL);
   #else
      enable_interrupts(GLOBAL);       //enable all interrupts (else timer2 wont happen)
   #endif

   printf("\r\nRunning...");

   while(TRUE)
   {

      if ( can_kbhit() )   //if data is waiting in buffer...
      {
         if(can_getd(rx_id, &buffer[0], rx_len, rxstat)) { //...then get data from buffer
            if (rx_id == RESPOND_TO_ID_LED) {
               printf("Changing LEDs\r\n\r\n");
               if (bit_test(buffer[0],0)) {LED1_HIGH;} else {LED1_LOW;}
               if (bit_test(buffer[0],1)) {LED2_HIGH;} else {LED2_LOW;}
               if (bit_test(buffer[0],2)) {LED3_HIGH;} else {LED3_LOW;}
            }
            if (rx_id == RESPOND_TO_ID_AD) {
               i=read_adc();
               printf("Sending AD reading: %X\r\n\r\n",i);
               can_putd(RESPOND_TO_ID_AD, &i, 1,1,1,0); //put data on transmit buffer
            }
         }
      }
   }
}
