/////////////////////////////////////////////////////////////////////////
////                        EX_ADC_DMA.C                             ////
////                                                                 ////
////  This program displays the A/D samples captured from channel    ////
////  AN8 over the RS-232 interface.  The DMA controller is used to  ////
////  transfer data captured by the ADC into DMA RAM located at      ////
////     0x4000 for the dsPIC analog board (dsPIC33FJ128GP706)       ////
////  The code is setup to read data from ADC channel 8. A timer     ////
//// interrupt is called every 10 ms which triggers a conversion.    ////
//// At the end of every conversion, the DMA transfer takes place    ////
//// The DMA count register is set to 100 using the dma_start()      ////
//// function call which uses the sizeof buffer to set the count     ////
//// When 'count' number of conversions and transfers are complete   ////
//// the DMA interrupt gets triggered which sets the DMA_DONE flag.  ////
//// This flag can be checked in the user main code and this ADC     ////
//// can then be processed.                                          ////
////                                                                 ////
////  Configure the CCS prototype card as follows for PCD:           ////
////     Use the POT labeled AN8.                                    ////
////                                                                 ////
////                                                                 ////
////                                                                 ////
////  This example will work with the PCD compiler.                  ////
////  Change the device, clock and  RS232 pins for your hardware     ////
////  if needed.                                                     ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2009 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////



#include <33FJ128GP706.h>
#device ADC=16
#fuses HS,NOWDT,NOPROTECT, PR, NOCOE
#use delay(clock=12000000)
#use rs232(baud=9600, UART2)



BOOLEAN DMA_DONE; // Flag to test if DMA interupt has been serviced.

//This interrupt gets triggered after 100 DMA transfers(DMACOUNT) are complete
#INT_DMA0
void DMA0_ISR(void)
{
   DMA_DONE = TRUE;  
}   

//This timer interrupt is used for triggering the ADC conversions.
#int_timer1                          
void Timer_ISR()
{
   /* call the function to read ADC value
      This value is transfered to DMA buffer directly*/
    read_adc(); 
}

#define BUFFER_SIZE 100  //DMA BUFFER Size
void main() {

   unsigned int8 i;   
   
   /*The BANK_DMA directive will place the following variable, array or
     structure in the DMA RAM*/
   #BANK_DMA
   unsigned int DMA_BUFFER_CH1[BUFFER_SIZE];
   

   DMA_DONE = FALSE;   
   
   setup_dma(0, DMA_IN_ADC1, DMA_WORD);
   
   dma_start(0, DMA_CONTINOUS, &DMA_BUFFER_CH1[0] );
   enable_interrupts(INT_DMA0);
   
   
   setup_port_a( sAN8 );
   setup_adc( ADC_CLOCK_INTERNAL );
      
   /*Set channel 8 of the dspAnalog prototyping board (POT near LED's)*/
   set_adc_channel( 8 );
   
   delay_ms(50);
   printf("\n\rSampling.");
   /* Call first conversion, later conversions are called by timer interrupt */   
   read_adc(); 

   // Sample every 10 ms, 100 samples in 1 second to fill buffer
   setup_timer1(TMR_INTERNAL ,60000);
   enable_interrupts(INT_TIMER1);
   enable_interrupts(INTR_GLOBAL);
   while (TRUE)
   {
      putc('.');
      delay_ms(100);
      if(DMA_DONE)
      {
         disable_interrupts(INTR_GLOBAL);
         disable_interrupts(INT_DMA0);
         for(i=0;i<BUFFER_SIZE; i++)
         printf("\n\r ADC Val: %lx", DMA_BUFFER_CH1[i]);
         
         while(1);  //Print out all the values and wait here. TODO: user code.
      }
   }

}

