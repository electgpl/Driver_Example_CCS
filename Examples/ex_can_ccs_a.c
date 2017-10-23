/////////////////////////////////////////////////////////////////////////
////                         EX_CAN_CCS_A.C                          ////
////                                                                 ////
//// Example of CCS's CAN libraries, using the PIC18Fxx8, PIC24 or   ////
//// dsPIC33. This example was tested with and written for the CCS   ////
//// CAN Prototype boards CAN Bus and CAN Bus 24.                    ////
////                                                                 ////
//// The CCS CAN Prototype boards have four CAN nodes that           ////
//// communicate to each other.  Node A is the 18F458, CAN Bus, or   ////
//// 24HJ256GP7610, CAN Bus 24, with it's internal CAN peripheral,   ////
//// Node B is a PIC16F87x, CAN Bus, or 30F4011, CAN Bus 24,         ////
//// connected to an external MCP2510 CAN peripheral, and Node C and ////
//// Node D are both MCP250xx stand-alone CAN I/O expanders.  This   ////
//// example is the firmware for Node A.                             ////
////                                                                 ////
//// Every two seconds this firmware sends out a command to node B   ////
//// to change it's leds (CAN ID 0x202)                              ////
////                                                                 ////
//// Upon change of the A/D reading, a value of 0-9 is sent to       ////
//// Node D which is displayed on the 8-seg LCD (CAN ID 0x400)       ////
////                                                                 ////
//// Pressing the Node A button sends a request to Node B (CAN ID    ////
//// 0x201) for Node B's A/D reading, which Node B will respond      ////
//// with a CAN message with it's A/D reading (with CAN ID 0x201).   ////
//// Also, pressing the Node A button will change the LEDs on Node   ////
//// C (CAN ID 0x300)                                                ////
////                                                                 ////
//// Pressing Node C's buttons will cause Node A's buttons to change ////
//// (Node C transmits button changes with CAN ID 0x303)             ////
////                                                                 ////
//// Using a serial port, you can examine all the CAN traffic as     ////
//// seen by the 18xxx8 or PIC24HJ.                                  ////
////                                                                 ////
//// For more documentation on the CCS CAN library, see can-18xxx8.c ////
//// and can-PIC24.c                                                 ////
////                                                                 ////
//// This example will work with the PCH and PCD compilers.          ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// Baud rate settings to use to connect to the CCS CAN Prototype   ////
//// board at 20Mhz:                                                 //// 
////                                                                 ////
////  PIC18Fxx8 Devices                                              ////
////                                                                 ////
////    Baud Rate Prescalar: 4                                       ////
////    Propagation Segment: 3xTq                                    ////
////    Phase Segment 1: 6xTq                                        ////
////    Phase Segment 2: 6xTq                                        ////
////    Synchronized Jump Width: 1xTq                                ////
////    Sample Rate: 1x                                              ////
////    Wakeup Filter:  Off                                          ////
////                                                                 ////
////  PIC24HJ and dsPIC33FJ Devices                                  ////
////                                                                 ////
////    Baud Rate Prescalar: 4                                       ////
////    Propagation Segment: 3xTq                                    ////
////    Phase Segment 1: 2xTq                                        ////
////    Phase Segment 2: 2xTq                                        ////
////    Synchronized Jump Width: 1xTq                                ////
////    Sample Rate: 1x                                              ////
////    Wakeup Filter:  Off
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// Node C and D are seperate stand-alone MCP250xx CAN I/O          ////
//// expanders.  The CCS CAN Prototype board has these chips already ////
//// programmed correctly.  However, if you wish to program your own ////
//// to work with this example, then use the provided .HEX files     ////
//// a programmer capable of programming these chips.  Or, make a    ////
//// a new HEX file with these properties:                           ////
////                                                                 ////
//// NODE C: Set RX ID mask and buffers to receive ID 0x3**. (The ** ////
//// means make the least signifcant 8bits no-care in the mask).     ////
//// Set TX1 buffer to ID 0x301, TX2 buffer to ID 0x302, TX3 buffer  ////
//// to ID 0x303. Set GP0 to analog (and enable the A/D).  Set GP1,  ////
//// GP2 and GP3 to OUTPUT.  Set GP4, GP5 and GP6 as INPUT with edge ////
//// trigger enable.  Leave OPTREG2 clear, disable PWM1 and PWM2,    ////
//// and disable scheduled transmission.  Also, see the baud rate    ////
//// settings above.                                                 ////
////                                                                 ////
//// NODE D: Set RX ID mask and buffers to receive ID 0x4**. (The ** ////
//// means make the least signifcant 8bits no-care in the mask).     ////
//// Set TX1 buffer to ID 0x401, TX2 buffer to ID 0x402, TX3 buffer  ////
//// to ID 0x403. Configure all ports as OUTPUT.  Leave OPTREG2      ////
//// clear, disable PWM1 and PWM2, and disable scheduled             ////
//// transmission.  Also, see the baud rate settings above.          ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2010 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#define CAN_DO_DEBUG TRUE

#if defined(__PCD__)
 #include <24HJ256GP610.h>
 #fuses PR,HS,NOWDT
 #device adc=12
 #use delay(clock=20MHz)
 #use rs232(UART1, baud=9600)
 
 #define CAN_BRG_PRESCALAR           4    //Set CAN Baud Rate to 125K
 #define CAN_BRG_PHASE_SEGMENT_1     1    //Tq = (2(1+PRESCALAR))/(Fosc/2)
 #define CAN_BRG_PHASE_SEGMENT_2     1    //Tq = (2(1+4)/(20000000/2) = 0.000001
 #define CAN_BRG_PROPAGATION_TIME    2    //Baud Rate = 1/(((PHASE_SEGMENT_1+1)+(PHASE_SEGMENT_2+1)+(PROPAGATION_TIME+1)+(JUMP_WIDTH+1))*Tq)
 #define CAN_BRG_SYNCH_JUMP_WIDTH    0    //Baud Rate = 1/(((1+1)+(1+1)+(2+1)+(0+1))*0.000001) = 125000
 
 #include <can-PIC24.c>
 
 #define PIN_LED1 PIN_B4
 #define PIN_LED2 PIN_A5
 #define PIN_LED3 PIN_B1
#else
 #include <18F458.h>
 #fuses HS,NOPROTECT,NOLVP,NOWDT
 #use delay(clock=20000000)
 #use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
 
 #include <can-18xxx8.c>
 
 #define PIN_LED1  PIN_A5
 #define PIN_LED2  PIN_B5
 #define PIN_LED3  PIN_B4
#endif



#define LED1_HIGH output_low(PIN_LED1)
#define LED1_LOW  output_high(PIN_LED1)
#define LED2_HIGH output_low(PIN_LED2)
#define LED2_LOW  output_high(PIN_LED2)
#define LED3_HIGH output_low(PIN_LED3)
#define LED3_LOW  output_high(PIN_LED3)

#define BUTTON    PIN_A4

#define BUTTON_PRESSED  !input(BUTTON)

int16 ms;

const char lcd_seg[10]={0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};   //0 for on, 1 for off

#int_timer2
void isr_timer2(void) {
   ms++; //keep a running timer that increments every milli-second
}

#define ASK_FOR_ID_AD_B      0x201  //ask for AD info from CAN port B
#define SET_LED_ID_B         0x202  //set LEDs for CAN port B
#define RESPOND_TO_LED_C_ID  0x303
#define WRITE_REGISTER_C_ID  0x300
#define WRITE_REGISTER_D_ID  0x400

void main() {
   int8 b_leds=0;
   int8 c_leds=1;
   int8 a_leds=0;
   struct rx_stat rxstat;
   unsigned int32 rx_id;
   unsigned int8 buffer[8];
   int8 rx_len;

   unsigned int8 curr_lcd_output,last_lcd_output=0xFF;
   #if defined(__PCD__)
      unsigned int16 i;
   #else
      int8 i;
   #endif

   #if defined(__PCD__)
      setup_adc(ADC_CLOCK_INTERNAL | ADC_TAD_MUL_31);
      setup_adc_ports(sAN20 | VSS_VDD);
      set_adc_channel(20);
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
      setup_timer2(TMR_INTERNAL,10000);   //setup timer2 to interrupt every 1ms if using 20MHz clock
   #else
      setup_timer_2(T2_DIV_BY_4,79,16);   //setup timer2 to interrupt every 1ms if using 20Mhz clock
   #endif

   can_init();
   #if defined(__PCD__)
      can_enable_b_transfer(TRB0);  //make buffer 0 a transmit buffer
   #endif

   enable_interrupts(INT_TIMER2);
   #if defined(__PCD__)
      enable_interrupts(INTR_GLOBAL);
   #else
      enable_interrupts(GLOBAL);
   #endif

   printf("\r\nRunning...");

   while(TRUE)
   {
      if ( can_kbhit() )
      {
         printf("\r\n");
         if(can_getd(rx_id, &buffer[0], rx_len, rxstat)) {
            if (rx_id == ASK_FOR_ID_AD_B) {
               printf("Channel B AD: %X\r\n",buffer[0]);
            }
            else if (rx_id == RESPOND_TO_LED_C_ID) {  //node C is an mcp250x0 which sends out a message upon edge detection on IO
               printf("Chaning LEDs\r\n");            //in_data[0]=iointfl, in_data[1]=gpio
               a_leds=~(buffer[1]);
               if (bit_test(a_leds,4)) {LED1_HIGH;} else {LED1_LOW;}
               if (bit_test(a_leds,5)) {LED2_HIGH;} else {LED2_LOW;}
               if (bit_test(a_leds,6)) {LED3_HIGH;} else {LED3_LOW;}
            }
         }
      }

      if ( can_tbe() && (ms > 2000))       //every two seconds, send new data if transmit buffer is empty
      {
         ms=0;

         //change leds on port b
         printf("\r\n\r\nSet LEDs on Port B to %U",b_leds);
         can_putd(SET_LED_ID_B, &b_leds, 1, 1, 1, 0);
         b_leds++;
         if (b_leds > 7) {b_leds=0;}
      }

      if (BUTTON_PRESSED) {
         while (BUTTON_PRESSED) {}
         delay_ms(200);

         //ask for AD on port B
         printf("\r\n\r\nAsking for A/D reading on Port B...");
         can_putd(ASK_FOR_ID_AD_B, &buffer[0], 0, 1, 1, 0);

         //change LEDs on port C
         buffer[0]=0x1E;            //addr of gplat on 25050
         buffer[1]=0x0E;            //mask
         buffer[2]=~(c_leds << 1);  //new gplat values
         printf("\r\nIncrementing LED on Port C");
         can_putd(WRITE_REGISTER_C_ID, &buffer[0], 3, 1, 1, 0);
         c_leds++;
         if (c_leds > 7) {c_leds=0;}
      }

         //change lcd segment on port d
         i=read_adc();
         curr_lcd_output=i/410;   //scale to 0-9
         if (curr_lcd_output != last_lcd_output) {
            last_lcd_output=curr_lcd_output;
            printf("\r\nChanging 8-seg LCD on D to current A/D reading (%X, %X)",i,curr_lcd_output);
            buffer[0]=0x1E;                    //addr of gplat
            buffer[1]=0x7F;             //mask
            buffer[2]=lcd_seg[curr_lcd_output];                //new gplat values
            can_putd(WRITE_REGISTER_D_ID, &buffer[0], 3, 1, 1, 0);
         }
   }
}
