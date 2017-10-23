///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2007 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#ifndef _STDLIB
#define _STDLIB

//---------------------------------------------------------------------------
// Definitions and types
//---------------------------------------------------------------------------

#ifndef RAND_MAX
#define RAND_MAX  32767    // The value of which is the maximum value
                           // ... returned by the rand function
#endif


#IF (sizeof(int16*)>1)
#DEFINE LONG_POINTERS 1
#ELSE 
#DEFINE LONG_POINTERS 0
#ENDIF

typedef struct {
   signed int quot;
   signed int rem;
} div_t;

typedef struct {
   signed long quot;
   signed long rem;
} ldiv_t;

#include <stddef.h>

//---------------------------------------------------------------------------
// String conversion functions
//---------------------------------------------------------------------------

/* Standard template: signed int  atoi(char * s)
 * converts the initial portion of the string s to a signed int
 * returns the converted value if any, 0 otherwise
 */
signed int atoi(char *s);

/* Syntax: signed int32  atoi32(char * s)
   converts the initial portion of the string s to a signed int32
   returns the converted value if any, 0 otherwise*/
#if (sizeof(long)==4)
 #define atoi32(s) atol(s)
#else 
 signed int32 atoi32(char *s);
#endif

#if defined(__PCD__)
// The following functions only work on the 24 bit compiler
// for the 30F, 33F, 24F and 24H parts
/* Syntax: signed int48  atoi48(char * s)
   converts the initial portion of the string s to a signed int48
   returns the converted value if any, 0 otherwise*/
   
signed int48 atoi48(char *s);

/* Syntax: signed int64  atoi64(char * s)
   converts the initial portion of the string s to a signed int64
   returns the converted value if any, 0 otherwise*/
signed int64 atoi64(char *s);
#endif

/* Syntax: char *  itoa(signed int32 num, int8 base, char * s)
   converts the signed int32 to a string and
   returns the converted value if any, 0 otherwise*/
char * itoa(signed int32 num, unsigned int base, char * s);

/* Standard template: signed int16  atol(char * s)
 * converts the initial portion of the string s to a signed int16
 * returns the converted value if any, 0 otherwise
 */
signed long atol(char *s);

/* Standard template: int16 strtoul(char * s,char *endptr,signed int base)
 * converts the initial portion of the string s, represented as an
 * integral value of radix base  to a signed long.
 * Returns the converted value if any, 0 otherwise
 * the final string is returned in the endptr, if endptr is not null
 */
signed long strtol(char *s,char *endptr, signed int base);

/* Standard template: int16 strtoul(char * s,char *endptr,signed int base)
 * converts the initial portion of the string s, represented as an
 * integral value of radix base to a unsigned long.
 * returns the converted value if any, 0 otherwise
 * the final string is returned in the endptr, if endptr is not null
 */
unsigned long strtoul(char *s,char *endptr, signed int base);

/* Standart template: float strtof(char * s,char *endptr)
                      float48 strtof48(char *s,char *endptr);
                      float64 strtod(char *s,char *endptr);
 * converts the initial portion of the string s to a float32, float48 or float64,
 * returns the converted value if any, 0 otherwise
 * the final string is returned in the endptr, if endptr is not null                   
*/
float strtof(char *s,char *endptr);
#if defined(__PCD__)
float48 strtof48(char *s,char *endptr);
float64 strtod(char *s,char *endptr);
#else
//provided for compatibility
#define strtof48(s, e) strtof(s, e)
#define strtod(s, e) strtof(s, e)
#endif

/* Standard template: float32 atof(char * s)
 * converts the initial portion of the string s to a float.
 * returns the converted value if any, 0 otherwise
 */
#define atof(s)   strtof(s, 0)

#if defined(__PCD__)
// The following functions only work on the 24 bit compiler
// for the 30F, 33F, 24F and 24H parts

/* Standard template: float48 atof48(char * s)
 * converts the initial portion of the string s to a float.
 * returns the converted value if any, 0 otherwise
 */
#define atof48(s) strtof48(s, 0)

/* Standard template: float64 atof64(char * s)
 * converts the initial portion of the string s to a float.
 * returns the converted value if any, 0 otherwise
 */
#define atof64(s) strtod(s, 0)
#endif

/* Standard template: float32 atoe(char * s)
 * converts the initial portion of the string s to a float.
 * returns the converted value if any, 0 otherwise
 * also handles E format numbers
 */
#if !defined(__PCD__)
float atoe(char * s);
#endif

#if defined(__PCD__)
float32 atoe(char * s);
#endif

//---------------------------------------------------------------------------
// Pseudo-random sequence generation functions
//---------------------------------------------------------------------------

/* The rand function computes a sequence of pseudo-random integers in
 * the range 0 to RAND_MAX
 *
 * Parameters:
 *       (none)
 *
 * Returns:
 *       The pseudo-random integer
 */
unsigned int16 rand(void);

/* The srand function uses the argument as a seed for a new sequence of
 * pseudo-random numbers to be returned by subsequent calls to rand.
 *
 * Parameters:
 *       [in] seed: The seed value to start from. You might need to pass
 *
 * Returns:
 *       (none)
 *
 * Remarks
 *          The srand function sets the starting point for generating
 *       a series of pseudorandom integers. To reinitialize the
 *       generator, use 1 as the seed argument. Any other value for
 *       seed sets the generator to a random starting point. rand
 *       retrieves the pseudorandom numbers that are generated.
 *       Calling rand before any call to srand generates the same
 *       sequence as calling srand with seed passed as 1.
 *          Usually, you need to pass a time here from outer source
 *       so that the numbers will be different every time you run.
 */
void srand(unsigned int32 seed);

//---------------------------------------------------------------------------
// Memory management functions
//---------------------------------------------------------------------------

// Comming soon

//---------------------------------------------------------------------------
// Communication with the environment
//---------------------------------------------------------------------------

/* The function returns 0 always
 */
signed int8 system(char *string);

//---------------------------------------------------------------------------
// Searching and sorting utilities
//---------------------------------------------------------------------------

/* Performs a binary search of a sorted array..
 *
 * Parameters:
 *       [in] key: Object to search for
 *       [in] base: Pointer to base of search data
 *       [in] num: Number of elements
 *       [in] width: Width of elements
 *       [in] compare: Function that compares two elements
 *
 * Returns:
 *       bsearch returns a pointer to an occurrence of key in the array pointed
 *       to by base. If key is not found, the function returns NULL. If the
 *       array is not in order or contains duplicate records with identical keys,
 *       the result is unpredictable.
 */
//void *bsearch(const void *key, const void *base, size_t num, size_t width,
//              int (*compare)(const void *, const void *));

/* Performs the shell-metzner sort (not the quick sort algorithm). The contents
 * of the array are sorted into ascending order according to a comparison
 * function pointed to by compar.
 *
 * Parameters:
 *       [in] base: Pointer to base of search data
 *       [in] num: Number of elements
 *       [in] width: Width of elements
 *       [in] compare: Function that compares two elements
 *
 * Returns:
 *       (none)
 */
//void *qsort(const void *base, size_t num, size_t width,
//              int (*compare)(const void *, const void *));

//---------------------------------------------------------------------------
// Integer arithmetic functions
//---------------------------------------------------------------------------

#define labs abs

div_t div(signed int numer, signed int denom);
ldiv_t ldiv(signed long numer, signed long denom);

//---------------------------------------------------------------------------
// Multibyte character functions
//---------------------------------------------------------------------------

// Not supported

//---------------------------------------------------------------------------
// Multibyte string functions
//---------------------------------------------------------------------------

// Not supported


//---------------------------------------------------------------------------
// Internal implementation
//---------------------------------------------------------------------------

#include <stddef.h>
#include <string.h>

div_t div(signed int numer, signed int denom)
{
   div_t val;
   val.quot = numer / denom;
   val.rem = numer - (denom * val.quot);
   return (val);
}

ldiv_t ldiv(signed long numer, signed long denom)
{
   ldiv_t val;

   val.quot = numer / denom;
   val.rem = numer - (denom * val.quot);
   return (val);
}

#if defined(__PCD__)
float32 atoe(char * s)
{
   float32 pow10 = 1.0;
   float32 result = 0.0;
   unsigned int8 sign = 0;
   unsigned int8 expsign = 0;
   char c;
   unsigned int8 ptr = 0;
   unsigned int8 i;
   float32 exp = 1.0;
   unsigned int8 expcnt = 0;

   c = s[ptr++];

   if ((c>='0' && c<='9') || c=='+' || c=='-' || c=='.' || c=='E' || c=='e') {
      if(c == '-') {
         sign = 1;
         c = s[ptr++];
      }
      if(c == '+')
         c = s[ptr++];

      while((c >= '0' && c <= '9')) {
         result = 10*result + c - '0';
         c = s[ptr++];
      }

      if (c == '.') {
         c = s[ptr++];
         while((c >= '0' && c <= '9')) {
             pow10 = pow10*10;
             result += (c - '0')/pow10;
             c = s[ptr++];
         }
      }

      // Handling the exponent
      if (c=='e' || c=='E') {
         c = s[ptr++];

         if(c == '-') {
            expsign = 1;
            c = s[ptr++];
         }
         if(c == '+')
            c = s[ptr++];

         while((c >= '0' && c <= '9')) {
            expcnt = 10*expcnt + c - '0';
            c = s[ptr++];
         }

         for(i=0;i<expcnt;i++)
            exp*=10;

         if(expsign==1)
            result/=exp;
         else
            result*=exp;
      }
   }

   if (sign == 1)
      result = -1*result;
   return(result);
}
#endif

#if !defined(__PCD__)
float atoe(char * s)
{
   float pow10 = 1.0;
   float result = 0.0;
   unsigned int8 sign = 0;
   unsigned int8 expsign = 0;
   char c;
   unsigned int8 ptr = 0;
   unsigned int8 i;
   float exp = 1.0;
   unsigned int8 expcnt = 0;

   c = s[ptr++];

   if ((c>='0' && c<='9') || c=='+' || c=='-' || c=='.' || c=='E' || c=='e') {
      if(c == '-') {
         sign = 1;
         c = s[ptr++];
      }
      if(c == '+')
         c = s[ptr++];

      while((c >= '0' && c <= '9')) {
         result = 10*result + c - '0';
         c = s[ptr++];
      }

      if (c == '.') {
         c = s[ptr++];
         while((c >= '0' && c <= '9')) {
             pow10 = pow10*10;
             result += (c - '0')/pow10;
             c = s[ptr++];
         }
      }

      // Handling the exponent
      if (c=='e' || c=='E') {
         c = s[ptr++];

         if(c == '-') {
            expsign = 1;
            c = s[ptr++];
         }
         if(c == '+')
            c = s[ptr++];

         while((c >= '0' && c <= '9')) {
            expcnt = 10*expcnt + c - '0';
            c = s[ptr++];
         }

         for(i=0;i<expcnt;i++)
            exp*=10;

         if(expsign==1)
            result/=exp;
         else
            result*=exp;
      }
   }

   if (sign == 1)
      result = -1*result;
   return(result);
}
#endif

signed int atoi(char *s)
{
   signed int result;
   unsigned int sign, base, index;
   char c;

   index = 0;
   sign = 0;
   base = 10;
   result = 0;

   if (!s)
      return 0;
   // Omit all preceeding alpha characters
   c = s[index++];

   // increase index if either positive or negative sign is detected
   if (c == '-')
   {
      sign = 1;         // Set the sign to negative
      c = s[index++];
   }
   else if (c == '+')
   {
      c = s[index++];
   }

   if (c >= '0' && c <= '9')
   {

      // Check for hexa number
      if (c == '0' && (s[index] == 'x' || s[index] == 'X'))
      {
         base = 16;
         index++;
         c = s[index++];
      }

      // The number is a decimal number
      if (base == 10)
      {
         while (c >= '0' && c <= '9')
         {
            result = 10*result + (c - '0');
            c = s[index++];
         }
      }
      else if (base == 16)    // The number is a hexa number
      {
         c = toupper(c);
         while ( (c >= '0' && c <= '9') || (c >= 'A' && c<='F'))
         {
            if (c >= '0' && c <= '9')
               result = (result << 4) + (c - '0');
            else
               result = (result << 4) + (c - 'A' + 10);

            c = s[index++];
            c = toupper(c);
         }
      }
   }

   if (sign == 1 && base == 10)
       result = -result;

   return(result);
}

signed long atol(char *s)
{
   signed long result;
   unsigned int sign, base, index;
   char c;

   index = 0;
   sign = 0;
   base = 10;
   result = 0;

   if (!s)
      return 0;
   c = s[index++];

   // increase index if either positive or negative sign is detected
   if (c == '-')
   {
      sign = 1;         // Set the sign to negative
      c = s[index++];
   }
   else if (c == '+')
   {
      c = s[index++];
   }

   if (c >= '0' && c <= '9')
   {
      if (c == '0' && (s[index] == 'x' || s[index] == 'X'))
      {
         base = 16;
         index++;
         c = s[index++];
      }

      // The number is a decimal number
      if (base == 10)
      {
         while (c >= '0' && c <= '9')
         {
            result = 10*result + (c - '0');
            c = s[index++];
         }
      }
      else if (base == 16)    // The number is a hexa number
      {
         c = toupper(c);
         while ( (c >= '0' && c <= '9') || (c >= 'A' && c <='F'))
         {
            if (c >= '0' && c <= '9')
               result = (result << 4) + (c - '0');
            else
               result = (result << 4) + (c - 'A' + 10);

            c = s[index++];c = toupper(c);
         }
      }
   }

   if (base == 10 && sign == 1)
      result = -result;

   return(result);
}

/* A fast routine to multiply by 10
 */
signed int32 mult_with10(int32 num)
{
   return ( (num << 1) + (num << 3) );
}

#if sizeof(long)==2
signed int32 atoi32(char *s)
{
   signed int32 result;
   int8 sign, base, index;
   char c;

   index = 0;
   sign = 0;
   base = 10;
   result = 0;

   if (!s)
      return 0;
   c = s[index++];

   // increase index if either positive or negative sign is detected
   if (c == '-')
   {
      sign = 1;         // Set the sign to negative
      c = s[index++];
   }
   else if (c == '+')
   {
      c = s[index++];
   }

   if (c >= '0' && c <= '9')
   {
      if (c == '0' && (s[index] == 'x' || s[index] == 'X'))
      {
         base = 16;
         index++;
         c = s[index++];
      }

      // The number is a decimal number
      if (base == 10)
      {
         while (c >= '0' && c <= '9') {
            result = (result << 1) + (result << 3);  // result *= 10;
            result += (c - '0');
            c = s[index++];
         }
      }
      else if (base == 16)    // The number is a hexa number
      {
         c = toupper(c);
         while ((c >= '0' && c <= '9') || (c >= 'A' && c <='F'))
         {
            if (c >= '0' && c <= '9')
               result = (result << 4) + (c - '0');
            else
               result = (result << 4) + (c - 'A' + 10);

            c = s[index++];c = toupper(c);
         }
      }
   }

   if (base == 10 && sign == 1)
      result = -result;

   return(result);
}
#endif

#if defined(__PCD__)

signed int48 atoi48(char *s)
{
   signed int48 result;
   int8 sign, base, index;
   char c;

   index = 0;
   sign = 0;
   base = 10;
   result = 0;

   if (!s)
      return 0;
   c = s[index++];

   // increase index if either positive or negative sign is detected
   if (c == '-')
   {
      sign = 1;         // Set the sign to negative
      c = s[index++];
   }
   else if (c == '+')
   {
      c = s[index++];
   }

   if (c >= '0' && c <= '9')
   {
      if (c == '0' && (s[index] == 'x' || s[index] == 'X'))
      {
         base = 16;
         index++;
         c = s[index++];
      }

      // The number is a decimal number
      if (base == 10)
      {
         while (c >= '0' && c <= '9') {
            result = (result << 1) + (result << 3);  // result *= 10;
            result += (c - '0');
            c = s[index++];
         }
      }
      else if (base == 16)    // The number is a hexa number
      {
         c = toupper(c);
         while ((c >= '0' && c <= '9') || (c >= 'A' && c <='F'))
         {
            if (c >= '0' && c <= '9')
               result = (result << 4) + (c - '0');
            else
               result = (result << 4) + (c - 'A' + 10);

            c = s[index++];c = toupper(c);
         }
      }
   }

   if (base == 10 && sign == 1)
      result = -result;

   return(result);
}

signed int64 atoi64(char *s)
{
   signed int64 result;
   int8 sign, base, index;
   char c;

   index = 0;
   sign = 0;
   base = 10;
   result = 0;

   if (!s)
      return 0;
   c = s[index++];

   // increase index if either positive or negative sign is detected
   if (c == '-')
   {
      sign = 1;         // Set the sign to negative
      c = s[index++];
   }
   else if (c == '+')
   {
      c = s[index++];
   }

   if (c >= '0' && c <= '9')
   {
      if (c == '0' && (s[index] == 'x' || s[index] == 'X'))
      {
         base = 16;
         index++;
         c = s[index++];
      }

      // The number is a decimal number
      if (base == 10)
      {
         while (c >= '0' && c <= '9') {
            result = (result << 1) + (result << 3);  // result *= 10;
            result += (c - '0');
            c = s[index++];
         }
      }
      else if (base == 16)    // The number is a hexa number
      {
         c = toupper(c);
         while ((c >= '0' && c <= '9') || (c >= 'A' && c <='F'))
         {
            if (c >= '0' && c <= '9')
               result = (result << 4) + (c - '0');
            else
               result = (result << 4) + (c - 'A' + 10);

            c = s[index++];c = toupper(c);
         }
      }
   }

   if (base == 10 && sign == 1)
      result = -result;

   return(result);
}
#endif

char * itoa(signed int32 num, unsigned int base, char * s)
{
     unsigned int32 temp=1;
     unsigned int8 i,sign=0,cnt=0;
     char c;

     if(num<0) {
         sign=1;        // Check for negative number
         num*=-1;
     }

     while(temp>0) {
         temp=(num/base);
         s[cnt]=(num%base)+'0';    // Conversion

         if(s[cnt]>0x39)
            s[cnt]+=0x7;

         cnt++;
         num=temp;
     }

     if(sign==1) {
         s[cnt]=0x2D;      // Negative sign
         cnt++;
     }

     for(i = 0;i<(int8)(cnt/2);i++) {

         c=s[i];
         s[i]=s[cnt-i-1];        // Reverse the number
         s[cnt-i-1]=c;
     }
     s[cnt]='\0';     // End the string
     return s;
}

#if defined(__PCD__)

char * itoa(signed int48 num, unsigned int base, char * s)
{
     unsigned int48 temp=1;
     unsigned int8 i,sign=0,cnt=0;
     char c;

     if(num<0) {
         sign=1;        // Check for negative number
         num*=-1;
     }

     while(temp>0) {
         temp=(num/base);
         s[cnt]=(num%base)+'0';    // Conversion

         if(s[cnt]>0x39)
            s[cnt]+=0x7;

         cnt++;
         num=temp;
     }

     if(sign==1) {
         s[cnt]=0x2D;      // Negative sign
         cnt++;
     }

     for(i = 0;i<(int8)(cnt/2);i++) {

         c=s[i];
         s[i]=s[cnt-i-1];        // Reverse the number
         s[cnt-i-1]=c;
     }
     s[cnt]='\0';     // End the string
     return s;
}

char * itoa(signed int64 num, unsigned int base, char * s)
{
     unsigned int64 temp=1;
     unsigned int8 i,sign=0,cnt=0;
     char c;
     
     if(num<0) {
         sign=1;        // Check for negative number
         num*=-1;        
     }

     while(temp>0) {
         temp=(num/base);
         s[cnt]=(num%base)+'0';    // Conversion

         if(s[cnt]>0x39)
            s[cnt]+=0x7;
            
         cnt++;
         num=temp;
     }

     if(sign==1) {
         s[cnt]=0x2D;      // Negative sign
         cnt++;
     }

     for(i = 0;i<(int8)(cnt/2);i++) {

         c=s[i];
         s[i]=s[cnt-i-1];        // Reverse the number
         s[cnt-i-1]=c;
     }
     s[cnt]='\0';     // End the string
     return s;
}

#endif

float strtof(char *s, char *endptr)
{
   float pow10 = 1.0;
   float result = 0.0;
   int1 skip = 1, sign = 0, point = 0;
   char c;
   unsigned int8 ptr = 0;

   if (!s)
      return 0;

   for(c=s[ptr++]; c!=0; c=s[ptr++])
   {
      if (skip && !isspace(c))
      {
         skip = 0;
         if (c == '+')
         {
            sign = 0;
            continue;
         }            
         else if (c == '-')
         {
            sign = 1;
            continue;
         }
      }
      if (!skip && (c == '.') && !point)
         point = 1;
      else if (!skip && isdigit(c))
      {
         c -= '0';
         if (point)
         {
            pow10 = pow10 * 10.0;
            result += (float)c / pow10;   
         }
         else
         {
            result = 10.0 * result + (float)c;
         }
      }
      else if (!skip)
         break;
   }

   if (sign)
      result = -1*result;
      
   if(endptr)
   {
      if (ptr) {
         ptr--;
       #IF LONG_POINTERS 
         *((int16 *)endptr)= s+ptr; 
       #ELSE
         *((char *)endptr)=s+ptr;
       #ENDIF
      }
      else
      {
      #IF LONG_POINTERS
      *((int16 *)endptr)= s; 
      #ELSE
      *((char *)endptr)=s;
      #ENDIF
      }
   }

   return(result);
}

#if defined(__PCD__)
float48 strtof48(char *s, char *endptr)
{
   float48 pow10 = 1.0;
   float48 result = 0.0;
   int1 skip = 1, sign = 0, point = 0;
   char c;
   unsigned int8 ptr = 0;

   if (!s)
      return 0;

   for(c=s[ptr++]; c!=0; c=s[ptr++])
   {
      if (skip && !isspace(c))
      {
         skip = 0;
         if (c == '+')
         {
            sign = 0;
            continue;
         }            
         else if (c == '-')
         {
            sign = 1;
            continue;
         }
      }
      if (!skip && (c == '.') && !point)
         point = 1;
      else if (!skip && isdigit(c))
      {
         c -= '0';
         if (point)
         {
            pow10 = pow10 * 10.0;
            result += (float48)c / pow10;   
         }
         else
         {
            result = 10.0 * result + (float48)c;
         }
      }
      else if (!skip)
         break;
   }

   if (sign)
      result = -1*result;
      
   if(endptr)
   {
      if (ptr) {
         ptr--;
       #IF LONG_POINTERS 
         *((int16 *)endptr)= s+ptr; 
       #ELSE
         *((char *)endptr)=s+ptr;
       #ENDIF
      }
      else
      {
      #IF LONG_POINTERS
      *((int16 *)endptr)= s; 
      #ELSE
      *((char *)endptr)=s;
      #ENDIF
      }
   }

   return(result);
}

float64 strtod(char *s, char *endptr)
{
   float64 pow10 = 1.0;
   float64 result = 0.0;
   int1 skip = 1, sign = 0, point = 0;
   char c;
   unsigned int8 ptr = 0;

   if (!s)
      return 0;

   for(c=s[ptr++]; c!=0; c=s[ptr++])
   {
      if (skip && !isspace(c))
      {
         skip = 0;
         if (c == '+')
         {
            sign = 0;
            continue;
         }            
         else if (c == '-')
         {
            sign = 1;
            continue;
         }
      }
      if (!skip && (c == '.') && !point)
         point = 1;
      else if (!skip && isdigit(c))
      {
         c -= '0';
         if (point)
         {
            pow10 = pow10 * 10.0;
            result += (float64)c / pow10;   
         }
         else
         {
            result = 10.0 * result + (float64)c;
         }
      }
      else if (!skip)
         break;
   }

   if (sign)
      result = -1*result;
      
   if(endptr)
   {
      if (ptr) {
         ptr--;
       #IF LONG_POINTERS 
         *((int16 *)endptr)= s+ptr; 
       #ELSE
         *((char *)endptr)=s+ptr;
       #ENDIF
      }
      else
      {
      #IF LONG_POINTERS
      *((int16 *)endptr)= s; 
      #ELSE
      *((char *)endptr)=s;
      #ENDIF
      }
   }

   return(result);
}
#endif

unsigned long strtoul(char *s, char *endptr, signed int base)
{
   char *sc,*s1,*sd;
   unsigned long x=0;
   char sign;
   char digits[]="0123456789abcdefghijklmnopqstuvwxyz";
   for(sc=s;isspace(*sc);++sc);
   sign=*sc=='-'||*sc=='+'?*sc++:'+';
   if(sign=='-' || base <0 || base ==1|| base >36) // invalid base
   goto StrtoulGO;

   else if (base)
   {
      if(base==16 && *sc =='0'&&(sc[1]=='x' || sc[1]=='X'))
         sc+=2;
      if(base==8 && *sc =='0')
         sc+=1;
      if(base==2 && *sc =='0'&&sc[1]=='b')
         sc+=2;

   }
   else if(*sc!='0') // base is 0, find base
      base=10;
   else if (sc[1]=='x' || sc[1]=='X')
      base =16,sc+=2;
   else if(sc[1]=='b')
      base=2,sc+=2;
   else
      base=8;
   for (s1=sc;*sc=='0';++sc);// skip leading zeroes
   sd=memchr(digits,tolower(*sc),base);
   for(; sd!=0; )
   {
      x=x*base+(int16)(sd-digits);
      ++sc;
      sd=memchr(digits,tolower(*sc),base);
   }
   if(s1==sc)
   {
   StrtoulGO:
      if (endptr)
      {
         #IF LONG_POINTERS
         *((int16 *)endptr)= s; 
         #ELSE
         *((char *)endptr)=s;
         #ENDIF
         }
   return 0;
   }
   if (endptr)
   {
         #IF LONG_POINTERS
         *((int16 *)endptr)= sc; 
         #ELSE
         *((char *)endptr)=sc; 
         #ENDIF
   }
   return x;
}


signed long strtol(char *s,char *endptr, signed int base)
{
   char *sc,*s1,*sd;
   signed long x=0;
   char sign;
   char digits[]="0123456789abcdefghijklmnopqstuvwxyz";
   for(sc=s;isspace(*sc);++sc);
   sign=*sc=='-'||*sc=='+'?*sc++:'+';
   if (base <0 || base ==1|| base >36) // invalid base
   goto StrtolGO;
   else if (base)
   {
      if(base==16 && *sc =='0'&&(sc[1]=='x' || sc[1]=='X'))
         sc+=2;
      if(base==8 && *sc =='0')
         sc+=1;
      if(base==2 && *sc =='0'&&sc[1]=='b')
         sc+=2;

   }
   else if(*sc!='0') // base is 0, find base
      base=10;
   else if (sc[1]=='x' || sc[1]=='X')
      base =16,sc+=2;
   else if(sc[1]=='b')
      base=2,sc+=2;
   else
      base=8;
   for (s1=sc;*sc=='0';++sc);// skip leading zeroes

   sd=memchr(digits,tolower(*sc),base);
   for(;sd!=0;)
   {
      x=x*base+(int16)(sd-digits);
      ++sc;
      sd=memchr(digits,tolower(*sc),base);
   }
   if(s1==sc)
   {
   StrtolGO:
      if (endptr)
      {
         #IF LONG_POINTERS
         *((int16 *)endptr)= s; 
         #ELSE
         *((char *)endptr)=s;
         #ENDIF
      }
   return 0;
   }
   if(sign=='-')
      x  =-x;
   if (endptr)
   {
        #IF LONG_POINTERS
         *((int16 *)endptr)= sc; 
        #ELSE
        *((char *)endptr)=sc;
        #ENDIF
   }
   return x;
}

signed int8 system(char *string)
{
   return 0;
}

int8 mblen(char *s,size_t n)
{
   return strlen(s);
}

int8 mbtowc(wchar_t *pwc,char *s,size_t n)
{
   *pwc=*s;
   return 1;
}

int8 wctomb(char *s,wchar_t wchar)
{
   *s=wchar;
   return 1;
}

size_t mbstowcs(wchar_t *pwcs,char *s,size_t n)
{
   strncpy(pwcs,s,n);
   return strlen(pwcs);
}

size_t wcstombs(char *s,wchar_t *pwcs,size_t n)
{
   strncpy(s,pwcs,n);
   return strlen(s);
}

//---------------------------------------------------------------------------
// The random number implementation
//---------------------------------------------------------------------------

unsigned int32 _Randseed;

unsigned int16 rand(void)
{
   _Randseed = _Randseed * 1103515245 + 12345;
   return ((unsigned int16)(_Randseed >> 16) % RAND_MAX);
}

void srand(unsigned int32 seed)
{
   _Randseed = seed;
}

//---------------------------------------------------------------------------
// Searching and sorting utilities implementation
//---------------------------------------------------------------------------

#if !defined(__PCD__)
typedef signed int8 (*_Cmpfun)(char * p1,char * p2); 
#else 
typedef signed int16 (*_Cmpfun)(char * p1,char * p2); 
#endif



void qsort(char * qdata, unsigned int qitems, unsigned int qsize, _Cmpfun cmp) {
   unsigned int m,j,i,l;
   int1 done;
   unsigned int8 t[16];

   m = qitems/2;
   while( m > 0 ) {
     for(j=0; j<(qitems-m); ++j) {
        i = j;
        do
        {
           done=1;
           l = i+m;
           if( (*cmp)(qdata+i*qsize, qdata+l*qsize) > 0 ) {
              memcpy(t, qdata+i*qsize, qsize);
              memcpy(qdata+i*qsize, qdata+l*qsize, qsize);
              memcpy(qdata+l*qsize, t, qsize);
              if(m <= i)
                i -= m;
                done = 0;
           }
        } while(!done);
     }
     m = m/2;
   }
}


char *bsearch(char *key, char *base, size_t num, size_t width,_Cmpfun cmp)
{
   char *p, *q;
   size_t n;
   size_t pivot;
   signed int val;

   p = base;
   n = num;

   while (n > 0)
   {
      pivot = n >> 1;
      q = p + width * pivot;

      val = (*cmp)(key, q);

      if (val < 0)
         n = pivot;
      else if (val == 0)
         return ((char *)q);
      else {
         p = q + width;
         n -= pivot + 1;
      }
   }

   return NULL;      // There's no match
}


#endif
