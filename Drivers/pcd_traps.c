///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                          pcd_traps.c                              ////
////                                                                   ////
//// On 16bit PICs (PIC24, dsPIC30, dsPIC33) there are special trap    ////
//// vectors that are called when a fault has happened.  This library  ////
//// will place routines on those vectors to display debugging         ////
//// information on the serial port about the fault.  The PC,          ////
//// working registers and other important PIC status registers        ////
//// are displayed on the serial port.                                 ////
////                                                                   ////
//// The last #use_rs232() will be used.  Alternately, if the          ////
//// STREAM_SERIAL_INPUT is defind it will use that stream.            ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2012 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS    ////
//// C compiler.  This source code may only be distributed to other    ////
//// licensed users of the CCS C compiler.  No other use,              ////
//// reproduction or distribution is permitted without written         ////
//// permission.  Derivative programs created using this software      ////
//// in object code form are not restricted in any way.                ////
///////////////////////////////////////////////////////////////////////////

#if defined(__PCD__)

#ifndef __PCD_TRAPS_C__
#define __PCD_TRAPS_C__

#if defined(STREAM_SERIAL_INPUT)
   #inline
   void TrapPutc(char c)
   {
      fputc(c, STREAM_SERIAL_INPUT);
   }
#else
   #inline
   void TrapPutc(char c)
   {
      putc(c);
   }
#endif

#word PCD_SFR_W0REG=0
#word PCD_SFR_W1REG=2
#word PCD_SFR_W2REG=4
#word PCD_SFR_W3REG=6
#word PCD_SFR_W4REG=8
#word PCD_SFR_W5REG=10
#word PCD_SFR_W6REG=12
#word PCD_SFR_W7REG=14
#word PCD_SFR_W8REG=16
#word PCD_SFR_W9REG=18
#word PCD_SFR_W10REG=20
#word PCD_SFR_W11REG=22
#word PCD_SFR_W12REG=24
#word PCD_SFR_W13REG=26
#word PCD_SFR_W14REG=28
#word PCD_SFR_W15REG=30

#if (getenv("SFR_VALID:PSVPAG"))
   #word PCD_SFR_PSVPAG=getenv("SFR:PSVPAG")
#endif

#if (getenv("SFR_VALID:DSRPAG"))
   #word PCD_SFR_DSRPAG=getenv("SFR:DSRPAG")
#endif

#if (getenv("SFR_VALID:DSWPAG"))
   #word PCD_SFR_DSWPAG=getenv("SFR:DSWPAG")
#endif

#if (getenv("SFR_VALID:TBLPAG"))
   #word PCD_SFR_TBLPAG=getenv("SFR:TBLPAG")
   unsigned int16 g_TrapTBLPAG;
#endif

#word PCD_SFR_CORCON=getenv("SFR:CORCON")

struct
{
   unsigned int16 w[16];
   unsigned int32 addy;
} g_Trap;

unsigned int16 g_TrapH, g_TrapL;

#if (getenv("SFR_VALID:TBLPAG"))
   #define PCD_TRAPS_MEMORY_USED 42
#else
   #define PCD_TRAPS_MEMORY_USED 40
#endif

#inline 
void TRAP_START(void)
{
  #asm 
   POP g_TrapH;
   POP g_TrapL;
  #endasm
      
   g_Trap.w[0] = PCD_SFR_W0REG;
   g_Trap.w[1] = PCD_SFR_W1REG;
   g_Trap.w[2] = PCD_SFR_W2REG;
   g_Trap.w[3] = PCD_SFR_W3REG;
   g_Trap.w[4] = PCD_SFR_W4REG;
   g_Trap.w[5] = PCD_SFR_W5REG;
   g_Trap.w[6] = PCD_SFR_W6REG;
   g_Trap.w[7] = PCD_SFR_W7REG;
   g_Trap.w[8] = PCD_SFR_W8REG;
   g_Trap.w[9] = PCD_SFR_W9REG;
   g_Trap.w[10] = PCD_SFR_W10REG;
   g_Trap.w[11] = PCD_SFR_W11REG;
   g_Trap.w[12] = PCD_SFR_W12REG;
   g_Trap.w[13] = PCD_SFR_W13REG;
   g_Trap.w[14] = PCD_SFR_W14REG;
   g_Trap.w[15] = PCD_SFR_W15REG;

  #if (getenv("SFR_VALID:TBLPAG"))
   g_TrapTBLPAG = PCD_SFR_TBLPAG;
  #endif

   g_TrapH &= 0x00FF;

   g_Trap.addy = make32(g_TrapH, g_TrapL);
   g_Trap.addy -= 2;
   
   printf(TrapPutc, "\r\n\n");
}

#inline 
void TRAP_DISPLAY(void)
{
   printf(TrapPutc, " PC:%LX ", g_Trap.addy);
   
   for (g_TrapL=0; g_TrapL<16; g_TrapL++)
   {
      printf(TrapPutc, "W%U:%LX ", g_TrapL, g_Trap.w[g_TrapL]);
   }
  
  #if (getenv("SFR_VALID:PSVPAG"))
   printf(TrapPutc, "PSVPAG:%LX ", PCD_SFR_PSVPAG);
  #endif

  #if (getenv("SFR_VALID:DSRPAG"))
   printf(TrapPutc, "DSRPAG:%LX ", PCD_SFR_DSRPAG);
  #endif

  #if (getenv("SFR_VALID:DSWPAG"))
   printf(TrapPutc, "DSWPAG:%LX ", PCD_SFR_DSWPAG);
  #endif

  #if (getenv("SFR_VALID:TBLPAG"))
   printf(TrapPutc, "TBLPAG:%LX ", g_TrapTBLPAG);
  #endif

   printf(TrapPutc, "CORCON:%LX ", PCD_SFR_CORCON);
  
   printf(TrapPutc, "\r\n");
}

#inline
void TRAP_END(void)
{
   while(1);
}

#if defined(INT_ADDRERR)
#INT_ADDRERR FAST
void handle_addrerr(void)
{
   TRAP_START();
   printf(TrapPutc, "ADDRESS FAULT");
   TRAP_DISPLAY();
   TRAP_END();
}
#endif

#if defined(INT_MATHERR)
#INT_MATHERR FAST
void handle_matherr(void)
{
   TRAP_START();
   printf(TrapPutc, "MATH FAULT");
   TRAP_DISPLAY();
   TRAP_END();
}
#endif

#if defined(INT_STACKERR)
#INT_STACKERR FAST
void handle_stackerr(void)
{
   TRAP_START();
   printf(TrapPutc, "STACK FAULT");
   TRAP_DISPLAY();
   TRAP_END();
}
#endif

#if defined(INT_OSCFAIL)
#INT_OSCFAIL FAST
void handle_oscfail(void)
{
   TRAP_START();
   printf(TrapPutc, "OSCILLATOR FAULT");
   TRAP_DISPLAY();
   TRAP_END();
}
#endif

#endif   //__PCD_TRAPS_C__

#endif   //defined(__PCD__)
