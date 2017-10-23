/////////////////////////////////////////////////////////////////////////
////                         EX_COMP.C                               ////
////                                                                 ////
////  This example demonstartes the use of the built in comparator.  ////
////  The program compares the input voltage with the internal       ////
////  reference voltage.  Turn the POT to change the voltage.        ////
////                                                                 ////
////  Configure the CCS prototype card as follows for PCM:           ////
////     Connect pin A1 to GND.                                      ////
////     Connect the output of the POT to pin A0.                    ////
////                                                                 ////
////  Configure the CCS prototype card as follows for PCD:           ////
////     Connect pin A1 to GND.                                      ////
////     Connect the output of the POT to pin A2.                    ////
////                                                                 ////
////  NOTE: Make sure the POT is turned all the way counter clock    ////
////  wise before starting the program.                              ////
////                                                                 ////
////  This example will work with the PCM compiler.  The following   ////
////  conditional compilation lines are used to include a valid      ////
////  device for each compiler.  Change the device, clock and RS232  ////
////  pins for your hardware if needed.                              ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2008 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////


#if defined(__PCM__)
#include <12F675.h>
#fuses HS,WDT,NOPROTECT
#use delay(clock=20000000)
#use rs232(baud=9200, xmit=PIN_A3, rcv=PIN_A4)

#elif defined(__PCD__)
#include <24FJ128GA006.h>
#fuses HS,NOWDT,NOPROTECT, PR
#use delay(clock=20000000)
#use rs232(baud=9600, UART1)
#endif

short safe_conditions=TRUE;

#INT_COMP
void isr()  {

   safe_conditions=FALSE;
   printf("WARNING!!  Voltage level is above 3.6 V.   \r\n");
}


main()   {

   printf("\r\nRunning voltage test...\r\n\n");
   
   #if defined(__PCD__)
   setup_comparator(NC_NC_A2_A3);
   #else
   setup_comparator(A1_VR_OUT_ON_A2);
   #endif
   
   setup_vref(VREF_HIGH|15);
   enable_interrupts(INT_COMP);
   enable_interrupts(INTR_GLOBAL);


   while(TRUE)
   {
      if(safe_conditions)
         printf("Voltage level is below 3.6 V.              \r\n");
      safe_conditions=TRUE;
      delay_ms(500);
   }
}
