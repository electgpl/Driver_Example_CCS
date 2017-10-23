/////////////////////////////////////////////////////////////////////////
////                         EX_WDT.C                                ////
////                                                                 ////
////  This program demonstartes the watchdog timer.  If the user     ////
////  does not hit a key in the set amount of time, the processor    ////
////  restarts, and tells the user why it restarted.                 ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCH    pin C7 to RS232 RX, pin C6 to RS232 TX               ////
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

#ifndef __PCH__
 #error This example is only for PIC18
#endif

#include <18F452.h>
#fuses HS,WDT128,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9200, xmit=PIN_C6, rcv=PIN_C7)

main()   {

   switch ( restart_cause() )
   {
      case WDT_TIMEOUT:
      {
         printf("\r\nRestarted processor because of watchdog timeout!\r\n");
         break;
      }
      case NORMAL_POWER_UP:
      {
         printf("\r\nNormal power up!\r\n");
         break;
      }
   }

   setup_wdt(WDT_ON);

   while(TRUE)
   {
      restart_wdt();
      printf("Hit any key to avoid a watchdog timeout.\r\n");
      getc();
   }
}
