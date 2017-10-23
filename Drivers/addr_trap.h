#ifndef addr_trap
#define addr_trap

#ifndef WREGS
#define WREGS

#word WREG0=0
#word WREG1=2
#word WREG2=4
#word WREG3=6
#word WREG4=8
#word WREG5=10
#word WREG6=12
#word WREG7=14
#word WREG8=16
#word WREG9=18
#word WREG10=20
#word WREG11=22
#word WREG12=24
#word WREG13=26
#word WREG14=28
#word WREG15=30

#endif

#inline
void print_stack_and_hold(void)
{
   int16 w[16];
   int16 h,l,i;
   int32 val;
   #asm
   POP.s
   POP h
   POP l
   #endasm
   
   w[0] = WREG0;
   w[1] = WREG1;
   w[2] = WREG2;
   w[3] = WREG3;
   w[4] = WREG4;
   w[5] = WREG5;
   w[6] = WREG6;
   w[7] = WREG7;
   w[8] = WREG8;
   w[9] = WREG9;
   w[10] = WREG10;
   w[11] = WREG11;
   w[12] = WREG12;
   w[13] = WREG13;
   w[14] = WREG14;
   w[15] = WREG15;
   
   //h &= 0x00FF;
   
   val = make32(h, l);
   val -= 2;
   printf("\r\nWREGS: ");
   for(i=0;i<16;i++){
      printf("\r\nW%i: %Lx",i,w[i]);
   }
   printf("\r\n\nPopped values: %Lx%LX",h,l);
   while(TRUE);
}

#int_addrerr FAST
void handle_addrerr(void)
{
   printf("\r\n\nAddress Exception Error!");
   disable_interrupts(INTR_GLOBAL);
   delay_ms(200);
   
   print_stack_and_hold();
}

#int_matherr FAST
void handle_matherr(void)
{
   printf("\r\n\nMath Exception Error!");
   disable_interrupts(INTR_GLOBAL);
   delay_ms(200);
   
   print_stack_and_hold();
}

#int_dmaerr FAST
void handle_dmaerr(void)
{
   printf("\r\n\nDMA Exception Error!");
   disable_interrupts(INTR_GLOBAL);
   delay_ms(200);
   
   print_stack_and_hold();
}

#int_stackerr FAST
void handle_stackerr(void)
{
   printf("\r\n\nStack Exception Error!");
   disable_interrupts(INTR_GLOBAL);
   delay_ms(200);
   
   print_stack_and_hold();
}

#int_oscfail FAST
void handle_oscfail(void)
{
   printf("\r\n\nSystem Oscillator Failed!");
   disable_interrupts(INTR_GLOBAL);
   delay_ms(200);
   
   print_stack_and_hold();
}

#endif
