///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                             input.c                               ////
////                                                                   ////
//// Routines for reading user input over an RS232 stream.  User input ////
//// is in ASCII form and converted to requested binary or float       ////
//// format.                                                           ////
////                                                                   ////
//// If STREAM_SERIAL_INPUT is defined, these routines will read from  ////
//// that serial stream.  If not, uses the last #use rs232().          ////
////                                                                   ////
//// Some of these routines are not available unless you #include      ////
//// string.h and stdlib.h                                             ////
////                                                                   ////
////  int8 gethex() - read 2 char hex value from serial                ////
////                                                                   ////
////  get_string(s, max) - read max chars from serial and save to s    ////
////                                                                   ////
////  get_stringEdit(s, max) - similar to get_string(), but first it   ////
////     displays current string in s, allowing you to edit it.        ////
////                                                                   ////
////  int8 = get_Int8() -                                              ////
////  int16 = get_Int16() -                                            ////
////  int32 = get_Int32() -                                            ////
////  float = get_float() -                                            ////
////     Read value from serial.                                       ////
////                                                                   ////
////  int8 = get_Int8Edit(old) -                                       ////
////  int16 = get_Int16Edit(old) -                                     ////
////  int32 = get_Int32Edit(old) -                                     ////
////  float = get_floatEdit(old) -                                     ////
////     Similar to get_Int*() routines documented above, but first    ////
////     it displays old value allowing you to edit it.                ////
////                                                                   ////
////  int = get_int() -                                                ////
////  long = get_long() -                                              ////
////     Simlar to get_Int*() documented above, but return 'int' and   ////
////     'long' datatypes.  The size of these datatypes depends on     ////
////     the PIC architecture and compiler configuration.              ////
////                                                                   ////
////  int = get_intEdit(old) -                                         ////
////  long = get_longEdit(old) -                                       ////
////     Similar to get_int() and get_long() documented above,         ////
////     but first it displays old value allowing you to edit it.      ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2012 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#ifndef __INPUT_C__
#define __INPUT_C__

#include <ctype.h>

#if defined(STREAM_SERIAL_INPUT)
   #define InputKbhit()    kbhit(STREAM_SERIAL_INPUT)
   #define InputPutc(c)    fputc(c, STREAM_SERIAL_INPUT)
   #define InputGetc()     fgetc(STREAM_SERIAL_INPUT)
#else
   #define InputKbhit()    kbhit()
   #define InputPutc(c)    putc(c)
   #define InputGetc()     getc()
#endif

unsigned int8 gethex1() {
   char digit;

   digit = InputGetc();

   InputPutc(digit);

   if(digit<='9')
     return(digit-'0');
   else
     return((toupper(digit)-'A')+10);
}

unsigned int8 gethex() {
   unsigned int8 lo,hi;

   hi = gethex1();
   lo = gethex1();
   if(lo==0xdd)
     return(hi);
   else
     return( hi*16+lo );
}

void get_string(char* s, unsigned int8 max) {
   unsigned int8 len;
   char c;

   max-=2;
   len=0;
   do {
     c=InputGetc();
     if(c==8) {  // Backspace
        if(len>0) {
          len--;
          InputPutc(c);
          InputPutc(' ');
          InputPutc(c);
        }
     } else if ((c>=' ')&&(c<='~'))
       if(len<=max) {
         s[len++]=c;
         InputPutc(c);
       }
   } while(c!=13);
   s[len]=0;
}

#ifdef _STRING
void get_stringEdit(char* s, unsigned int8 max) {
   unsigned int8 len;
   char c;

   len = strlen(s);

   if (len)
   {
     #if defined(STREAM_SERIAL_INPUT)
      fprintf(STREAM_SERIAL_INPUT, "%s", s);
     #else
      printf("%s", s);
     #endif
   }

   max-=2;

   do {
     c=InputGetc();
     if(c==8) {  // Backspace
        if(len>0) {
          len--;
          InputPutc(c);
          InputPutc(' ');
          InputPutc(c);
        }
     } else if ((c>=' ')&&(c<='~'))
       if(len<=max) {
         s[len++]=c;
         InputPutc(c);
       }
   } while(c!=13);
   s[len]=0;
}
#endif

// stdlib.h is required for the ato_ conversions
// in the following functions
#ifdef _STDLIB
signed int8 get_Int8(void)
{
  char s[5];
  signed int8 i;

  get_string(s, sizeof(s));

  i=atoi(s);
  return(i);
}

#ifdef _STRING
signed int8 get_Int8Edit(signed int8 old)
{
  char s[5];
  signed int8 i;

  sprintf(s, "%d", old);
  get_stringEdit(s, sizeof(s));

  i=atoi(s);
  return(i);
}
#endif

signed int16 get_Int16(void)
{
  char s[7];
  signed int16 l;

  get_string(s, sizeof(s));
  l=atol(s);
  return(l);
}

#ifdef _STRING
signed int16 get_Int16Edit(signed int16 old)
{
  char s[7];
  signed int16 l;

  sprintf(s, "%ld", old);

  get_stringEdit(s, sizeof(s));
  l=atol(s);
  return(l);
}
#endif

signed int32 get_Int32(void)
{
  char s[12];
  signed int32 l;

  get_string(s, sizeof(s));
  l=atoi32(s);
  return(l);
}

#ifdef _STRING
signed int32 get_Int32Edit(signed int32 old)
{
  char s[12];
  signed int32 l;

  sprintf(s, "%ld", old);

  get_stringEdit(s, sizeof(s));
  l=atoi32(s);
  return(l);
}
#endif

float get_float() {
  char s[20];
  float f;

  get_string(s, 20);
  f = atof(s);
  return(f);
}

#ifdef _STRING
float get_floatEdit(float old) {
  char s[20];
  float f;

  sprintf(s, "%f", old);

  get_string(s, 20);
  f = atof(s);
  return(f);
}
#endif

#if defined(__PCD__)
   #define get_int()       get_Int16()
   #define get_intEdit()   get_Int16Edit()
   #define get_long()      get_Int32()
   #define get_longEdit()  get_Int32Edit()
#else
   #define get_int()       get_Int8()
   #define get_intEdit()   get_Int8Edit()
   #define get_long()      get_Int16()
   #define get_longEdit()  get_Int16Edit()
#endif

#endif   //_STDLIB

#endif   //__INPUT_C__
