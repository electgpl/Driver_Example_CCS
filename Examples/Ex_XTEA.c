/////////////////////////////////////////////////////////////////////////
////                            EX_XTEA.C                            ////
////                                                                 ////
////  This is an example program for testing the XTEA library.       ////
////  Load this program onto a PIC. The host PC can talk to this PIC ////
////  over the serial port on C6,C7. This PIC will encryt the data   ////
////  and transmit it over to another PIC over the other serial port ////
////  B0,B1.
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2006 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////

#include <18F452.h>

#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8,stream=DATA)
#use rs232(baud=9600,parity=N,xmit=PIN_B0,rcv=PIN_B1,bits=8,stream=CNTL)

#include <string.h>

#include <xtea.h>

#define CIRCBUFF_LENGTH 128

struct cbuffer {
   char data [CIRCBUFF_LENGTH];
   int  first;
   int  last;
   int  used;
};

void cbuffer_init(struct cbuffer &bf) {
   bf.used = 0;
   bf.first = 0;
   bf.last = 0;
}

void cbuffer_put (struct cbuffer &bf, char ch) {

   if (bf.used < CIRCBUFF_LENGTH)
   {
      if (bf.used != 0) {
         if (bf.last == CIRCBUFF_LENGTH) {
            bf.last = 0;
         }
         else {
            bf.last++;
         }
      }
      bf.used++;
      bf.data[bf.last] = ch;
   }
}

char cbuffer_get (struct cbuffer &bf) {
   char tmp;

   if (bf.used > 0) {
      tmp = bf.data[bf.first];
      if (bf.first != bf.last) {
         if (bf.first < (CIRCBUFF_LENGTH - 1))
            bf.first++;
         else
            bf.first = 0;
      }
      bf.used--;
   }
   else
      tmp = 0;
return tmp;
}

int8 cbuffer_used (struct cbuffer &bf) {
   return bf.used;
 }


struct cbuffer buffer1;

#int_RDA
void ISR_RDA() {
   char ch;
   ch = fgetc(DATA);
   cbuffer_put(buffer1,ch);
}


void  main()
{
   char strng [128];
   char crypt1 [128];

   char key[16] = "thisismyownkey!";

   int i,L1,l2;
   char ch;

   cbuffer_init(buffer1);

   output_low(PIN_A5);
   fprintf(CNTL,"\n\n\rReady...");

   enable_interrupts(INT_RDA);
   enable_interrupts(GLOBAL);

   while(1) {
      if (kbhit(CNTL)) {      // send a message
         output_low(PIN_B5);

         fprintf(CNTL,"\n\r>");
         fgets(strng,CNTL);
         fprintf(CNTL,"%s",strng);

         L1 = strlen(strng);
         L2 = (((L1-1)>>3)+1)<<3;

         fprintf(CNTL,"\n\rL1=%u  L2=%u",L1,L2);

         XTEA_encrypt_string (strng, L1, crypt1 , L2 ,key);

         fprintf(CNTL,"\n\r(encrypted=");
         for(i=0; i<L2; i++) {
            fprintf(CNTL,"%X ",crypt1[i]);
            fputc(crypt1[i],DATA);
          }
         fprintf(CNTL,")");
         output_high(PIN_B5);

      } else if (buffer1.used>=8)// receive a message
       {

         fprintf(CNTL,"\n\rreceived=");
         for(i=0; i<8; i++)
         {
            ch = cbuffer_get(buffer1);
            crypt1[i] = ch;
            fprintf(CNTL,"%X ",ch);
         }
         XTEA_decrypt_string (strng, 8, crypt1 , 8 , key);
         fprintf(CNTL,"\n\r  => Decrypted: %s",strng);
      }
   }
}
