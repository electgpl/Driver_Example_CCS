/////////////////////////////////////////////////////////////////////////
////                           EX_SIM.C                              ////
////                                                                 ////
////  This program uses the sim.c driver to read and write phone     ////
////  book entries to a SIM Card. The sim.c driver uses the PIC      ////
////  rs232 hardware and RDA interrupt to communicate with the SIM   ////
////  Card, it uses the Timer2 and the CCP1 module to generate the   ////
////  SIM Card Clock, and it uses the I/O pins C3 and C5 for         ////
////  SIM_RST, and SIM_VCC.                                          ////
////                                                                 ////
////  This example will work with PCH compilers. Change the device,  ////
////  clock, CCP1 settings, Timer2 settings, and I/O pins for your   ////
////  hardware if needed.                                            ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2009 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#include <18F4520.H>
#fuses H4, NOLVP, NOWDT, PUT, BROWNOUT, BORV42
#device ICD=true
#use delay(clock=40000000)

#define CLOCK_PWM 5000000

#include "sim.c"

#use rs232(DEBUGGER)

#zero_ram

void main()
{
   int8 i;
   int8 result;
   phone_number write_test;
   phone_number read_test;
   
   printf("\n\rInitializing...");
      
   sim_init();          //Must be called before any other function
   
   delay_ms(100);
   
   strcpy(write_test.name,"Test Name");
   write_test.length=11;
   write_test.number[0]=1;
   write_test.number[1]=2;
   write_test.number[2]=3;
   write_test.number[3]=4;
   write_test.number[4]=5;
   write_test.number[5]=6;
   write_test.number[6]=7;
   write_test.number[7]=8;
   write_test.number[8]=9;
   write_test.number[9]=0;
   write_test.number[10]=1;
   
   writeNumber(write_test,1);       //Writes Phone Number Name and Number to SIM Card
   
   printf("\n\r");
   
   result=getNumber(read_test,1);   //Reads Phone Number Name and Number from SIM Card
   
   if(result==1)
   {
      printf("\n\r%s ",read_test.name);
      for(i=0;i<read_test.length;i++)
         printf("%u",read_test.number[i]);
   }
   else
   {
      printf("\n\rNo Number at Location");
   }
   
   printf("\n\r");
   
   result=getNumber(read_test,2);      //Reads Phone Number Name and Number from SIM Card
   
   if(result==1)
   {
      printf("\n\r%s ",read_test.name);
      for(i=0;i<read_test.length;i++)
         printf("%u",read_test.number[i]);
   }
   else
   {
      printf("\n\rNo Number at Location");
   }
   
   while (true)
   {
   }
   

}
