#include <16f818.h>
#device ICD=TRUE
#use delay(clock=4M)
#fuses INTRC_IO,NOWDT,NOLVP

#define PIN_LED1  PIN_B5
#define PIN_LED2  PIN_B4
#define PIN_LED3  PIN_B3
#define PIN_LED4  PIN_B2
#define PIN_LED5  PIN_B1
#define PIN_LED6  PIN_B0

#define PIN_SW1  PIN_A3
#define PIN_SW2  PIN_A2
#define PIN_SW3  PIN_A1
#define PIN_SW4  PIN_A0

#define PIN_BUTTON   PIN_A2

#use rs232(debugger, xmit=PIN_A7, rcv=PIN_A7)

#define DISPLAY_PROMPT()   printf("\r\nR)ead ADC, T)oggle LED5\r\n")

void main(void) {
   setup_oscillator(OSC_4MHZ);
   delay_ms(144);                         //wait for oscillator to get stable
   
   setup_adc(ADC_CLOCK_INTERNAL);
   setup_adc_ports(AN0);
   set_adc_channel(0);

   output_low(PIN_LED5);   //turn off LED
   output_low(PIN_LED6);   //turn off LED

   DISPLAY_PROMPT();

   while(TRUE) {
      output_bit(PIN_LED1, !input(PIN_SW1));   //light led if button press
      output_bit(PIN_LED2, !input(PIN_SW2));   //light led if button press
      output_bit(PIN_LED3, !input(PIN_SW3));   //light led if button press
      output_bit(PIN_LED4, !input(PIN_SW4));   //light led if button press

      if (kbhit()) {
         char c;
         c = toupper(getc());
        
         if (c == 'R')
            printf("ADC = %X\r\n", read_adc());
         if (c == 'T') {
            output_toggle(PIN_LED5);
            printf("LED5 TOGGLED\r\n");
         }
                  
         DISPLAY_PROMPT();
      }      
   }
}
