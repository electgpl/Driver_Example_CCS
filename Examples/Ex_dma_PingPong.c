/////////////////////////////////////////////////////////////////////////
////                    EX_DMA_PING_PONG.C                           ////
////                                                                 ////
////  This program reads 100 A/D samples captured from channel       ////
////  AN8 and stores them in DMA RAM. The next set of 1000 samples   ////
////  that are read are  transfered to a seperate data buffer.       ////
////  The DMA is setup in Ping Pong mode in which it will alternate  ////
////  between two buffers to fill in data read from the ADC channel  ////
////  The user can process data in buffer 1 while the second buffer  ////
////  is getting filled up. The second buffer can then be processed  ////
////  as the first gets filled up.                                   ////
////                                                                 ////
////  The code is setup to read data from ADC channel 8. A timer     ////
////  interrupt is called every 1 ms which triggers a conversion.    ////
////  At the end of every conversion, the DMA transfer takes place   ////
////  The DMA count register is set to 100 using the dma_start()     ////
////  function call which uses the sizeof buffer to set the count    ////
////  When 'count' number of conversions and transfers are complete  ////
////  the DMA interrupt gets triggered. The user can toggle between  ////
////  these two buffers by toggling the 
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

#if !defined(__PCD__)
#error This example only compiles with PCD compiler
#endif

#include <33FJ128GP706.h>
#device ADC=16
#fuses HS,NOWDT,NOPROTECT, PR, NOCOE
#use delay(clock=12000000)
#use rs232(baud=9600, UART2)


void ProcessInput(unsigned int *);

BOOLEAN DMA_Buffer; // Flag to test if DMA interupt has been serviced.

#define BUFFER_SIZE 100  //DMA BUFFER Size

/*The BANK_DMA directive will place the following variable, array or
     structure in the DMA RAM*/     
#BANK_DMA
unsigned int DMA_BUFFER_A[BUFFER_SIZE];
#BANK_DMA
unsigned int DMA_BUFFER_B[BUFFER_SIZE];
   
#INT_DMA0
void DMA0_ISR(void)
{
   
   if(DMA_Buffer)
      ProcessInput(&DMA_BUFFER_A[0]);
   else
      ProcessInput(&DMA_BUFFER_B[0]);
      
      DMA_Buffer ^=1; // Toggle between buffers     
}   

#int_timer1                          
void Timer_ISR()
{
   /* call the function to read ADC value
      This value is transfered to DMA buffer directly*/
    read_adc(); 
}


void main() {

   unsigned int16 value;
   
   setup_dma(0, DMA_IN_ADC1, DMA_WORD);
   
   /*
   Built-in function dma_start options are as follows -
    dma_start(channel, options, buffera, bufferb, size)
               // bufferb is optional depending on mode
               // size is optional, if omitted the size of buffera is used
               // buffers must be declared with "#bank_dma"

   */   
   dma_start(0, DMA_CONTINOUS|DMA_PING_PONG, &DMA_BUFFER_A[0], &DMA_BUFFER_B[0],BUFFER_SIZE );   
     
   
   setup_port_a( sAN8 );
   setup_adc( ADC_CLOCK_INTERNAL );
   set_adc_channel( 8 );
    
   
   printf("\n\rSampling:");
   /*Call first conversion, later conversions are called using Timer interrupt*/   
   value = read_adc(); 

   // Sample every 1 ms, 1000 samples in 1 second to fill buffer
   setup_timer1(TMR_INTERNAL ,600);
   
   /* Enable the peripheral and global interrupts */
   enable_interrupts(INT_TIMER1);
   enable_interrupts(INT_DMA0);
   enable_interrupts(INTR_GLOBAL);
   
   while (TRUE);

}


void ProcessInput(unsigned int *ptr)
{
   unsigned int i, ADC_Buffer[BUFFER_SIZE];

   for (i=0; i<BUFFER_SIZE; i++)
   {
      ADC_Buffer[i]=*ptr++;      
   }
   //TODO: user code to process the values
}
