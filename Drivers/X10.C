///////////////////////////////////////////////////////////////////////////
////  TW523 X10 Driver                                                 ////
////                                                                   ////
////  x10_write(house_code,key_code)    Send a data burst, house_code  ////
////                                    must be 'A' to 'P'  and        ////
////                                    key_code is 0-1F               ////
////                                                                   ////
////  x10_read( house_code, key_code)   Waits for and reads the next   ////
////                                    data burst.                    ////
////                                                                   ////
////  x10_data_ready()                  Returns true if a data burst   ////
////                                    is starting.  Be sure to call  ////
////                                    faster than 1khz in order not  ////
////                                    to miss any data.              ////
////      Connect B0 to TW523 pin 1                                    ////
////              B1              3                                    ////
////              B2              4                                    ////
////              GND             2                                    ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#ifndef X10_ZERO_CROSS

#define X10_ZERO_CROSS  PIN_B0
#define X10_TO_PIC      PIN_B1
#define X10_FROM_PIC    PIN_B2

#endif

char const X10_HOUSE_CODES[16] = {'M','N','O','P','C','D','A','B','E',
                                  'F','G','H','K','L','I','J'};
BYTE const X10_KEY_CODES[16] = {13,14,15,16,3,4,1,2,5,6,7,8,11,12,9,10};


void wait_for_zero_cross() {

  if(input(X10_ZERO_CROSS))
     while(input(X10_ZERO_CROSS)) ;
  else
     while(!input(X10_ZERO_CROSS)) ;
}

void x10_write_bits(BYTE data, BYTE n, BYTE start) {
   BYTE i;
   BOOLEAN the_bit;

   for(i=1;i<=n;++i) {
     wait_for_zero_cross();
     the_bit=shift_right(&data,1,0);
     output_bit(X10_FROM_PIC, the_bit);
     delay_ms(1);
     output_low(X10_FROM_PIC);
     if(start==0) {
        wait_for_zero_cross();
        output_bit(X10_FROM_PIC, !the_bit);
        delay_ms(1);
        output_low(X10_FROM_PIC);
     }
   }
}

void x10_write(BYTE house_code, BYTE key_code) {
  BYTE i;

  i=0;
  while (X10_HOUSE_CODES[i]!=house_code)
    i++;
  house_code=i;
  if(key_code<16) {
     i=0;
     while (X10_KEY_CODES[i]!=key_code)
       i++;
     key_code=i;
  }
  x10_write_bits(7,4,1);
  x10_write_bits(house_code,4,0);
  x10_write_bits(key_code,5,0);
  x10_write_bits(0,6,1);
}

BYTE x10_data_ready() {
  port_b_pullups(TRUE);
  return(!input(X10_TO_PIC));
}

BYTE x10_read_bits(BYTE n) {
   BYTE data,i;

   for(i=1;i<=n;++i) {
     wait_for_zero_cross();
     delay_us(300);
     shift_right(&data,1,input(X10_TO_PIC));
     wait_for_zero_cross();
     delay_us(300);
   }
   data>>=8-n;
   return(data);
}

void x10_read(BYTE *house_code,BYTE *key_code) {

  port_b_pullups(TRUE);
  x10_read_bits(2);
  *house_code=x10_read_bits(4);
  *house_code=X10_HOUSE_CODES[*house_code];
  *key_code=x10_read_bits(5);
  if(*key_code<16)
    *key_code=X10_KEY_CODES[*key_code];
}

