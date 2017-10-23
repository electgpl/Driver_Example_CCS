////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2008 Custom Computer Services            ////
//// This source code may only be used by licensed users of the CCS C   ////
//// compiler.  This source code may only be distributed to other       ////
//// licensed users of the CCS C compiler.  No other use, reproduction  ////
//// or distribution is permitted without written permission.           ////
//// Derivative programs created using this software in object code     ////
//// form are not restricted in any way.                                ////
////////////////////////////////////////////////////////////////////////////

#ifndef _STRING
#define _STRING
#include <stddef.h>
#include <ctype.h>



//////////////////////////////////////////////
//// Uncomment the following define to    ////
//// allow some functions to use a        ////
//// quicker algorithm, but use more ROM  ////
////                                      ////
//// #define FASTER_BUT_MORE_ROM          ////
//////////////////////////////////////////////



/*Copying functions*/
/* standard template:
   void *memmove(void *s1, void *s2, size_t n).
   Copies max of n characters safely (not following ending '\0')
   from s2 in s1; if s2 has less than n characters, appends 0 */

char *memmove(void *s1,char *s2,size_t n)
{
   char *sc1;
   char *sc2;
   sc1=s1;
   sc2=s2;
   if(sc2<sc1 && sc1 <sc2 +n)
      for(sc1+=n,sc2+=n;0<n;--n)
         *--sc1=*--sc2;
   else
      for(;0<n;--n)
         *sc1++=*sc2++;
  return s1;
  }

/* Standard template: char *strcpy(char *s1, const char *s2)
   copies the string s2 including the null character to s1.
   This is a compiler built in to handle the different address
   spaces */

#define strcopy strcpy

/* standard template:
   char *strncpy(char *s1, const char *s2, size_t n).
   Copies max of n characters (not following ending '\0')
   from s2 in s1; if s2 has less than n characters, appends 0 */

char *strncpy(char *s1, char *s2, size_t n)
{
  char *s;

  for (s = s1; n > 0 && *s2 != '\0'; n--)
     *s++ = *s2++;
  for (; n > 0; n--)
     *s++ = '\0';

  return(s1);
}
/***********************************************************/

/*concatenation functions*/
/* standard template: char *strcat(char *s1, const char *s2)
appends s2 to s1*/

char *strcat(char *s1, char *s2)
{
   char *s;

   for (s = s1; *s != '\0'; ++s);
   while(*s2 != '\0')
   {
      *s = *s2;
      ++s;
      ++s2;
   }

   *s = '\0';
   return(s1);
}
/* standard template: char *strncat(char *s1, char *s2,size_t n)
appends not more than n characters from s2 to s1*/

char *strncat(char *s1, char *s2, size_t n)
{
   char *s;

   for (s = s1; *s != '\0'; ++s);
   while(*s2 != '\0' && 0<n)
   {
      *s = *s2;
      ++s;
      ++s2;
      --n;
   }

   *s = '\0';
   return(s1);
}

/***********************************************************/


/*comparison functions*/
/* standard template: signed int memcmp(void *s1, void *s2).
   Compares s1 & s2; returns -1 if s1<s2, 0 if s1=s2, 1 if s1>s2 */

signed int8 memcmp(void * s1,char *s2,size_t n)
{
char *su1, *su2;
for(su1=s1, su2=s2; 0<n; ++su1, ++su2, --n)
{
   if(*su1!=*su2)
      return ((*su1<*su2)?-1:+1);
}
return 0;
}

/* standard template: int strcmp(const char *s1, const char *s2).
   Compares s1 & s2; returns -1 if s1<s2, 0 if s1=s2, 1 if s1>s2 */

signed int8 strcmp(char *s1, char *s2)
{
   for (; *s1 == *s2; s1++, s2++)
      if (*s1 == '\0')
         return(0);
   return((*s1 < *s2) ? -1: 1);
}
/* standard template: int strcoll(const char *s1, const char *s2).
   Compares s1 & s2; returns -1 if s1<s2, 0 if s1=s2, 1 if s1>s2 */

signed int8 strcoll(char *s1, char *s2)
{
   for (; *s1 == *s2; s1++, s2++)
      if (*s1 == '\0')
         return(0);
   return((*s1 < *s2) ? -1: 1);
}

/* standard template:
   int strncmp(const char *s1, const char *s2, size_t n).
   Compares max of n characters (not following 0) from s1 to s2;
   returns same as strcmp */

signed int8 strncmp(char *s1, char *s2, size_t n)
{
   for (; n > 0; s1++, s2++, n--)
      if (*s1 != *s2)
         return((*s1 <*s2) ? -1: 1);
      else if (*s1 == '\0')
         return(0);
   return(0);
}
/* standard template:
   int strxfrm(const char *s1, const char *s2, size_t n).
   transforms maximum of n characters from s2 and places them into s1*/
size_t strxfrm(char *s1, char *s2, size_t n)
{
  char *s;
  unsigned int8 n1;
  n1=n;
  for (s = s1; n > 0 && *s2 != '\0'; n--)
     *s++ = *s2++;
  for (; n > 0; n--)
     *s++ = '\0';

  return(n1);
}





/***********************************************************/
/*Search functions*/
/* standard template: void *memchr(const char *s, int c).
   Finds first occurrence of c in n characters of s */

char *memchr(void *s,unsigned int8 c,size_t n)
{
   char uc;
   char *su;
   uc=c;
   for(su=s;0<n;++su,--n)
      if(*su==uc)
      return su;
   return NULL;
}

/* standard template: char *strchr(const char *s, int c).
   Finds first occurrence of c in s */

char *strchr(char *s, unsigned int8 c)
{
   for (; *s != c; s++)
      if (*s == '\0')
         return(0);
   return(s);
}
/* standard template:
   size_t strcspn(const char *s1, const char *s2).
   Computes length of max initial segment of s1 that
   consists entirely of characters NOT from s2*/

unsigned int8  strcspn(char *s1, char *s2)
{
   char *sc1, *sc2;

   for (sc1 = s1; *sc1 != 0; sc1++)
      for (sc2 = s2; *sc2 != 0; sc2++)
         if (*sc1 == *sc2)
            return(sc1 - s1);
   return(sc1 - s1);
}
/* standard template:
   char *strpbrk(const char *s1, const char *s2).
   Locates first occurence of any character from s2 in s1;
   returns s1 if s2 is empty string */

char *strpbrk(char *s1, char *s2)
{
   char *sc1, *sc2;

   for (sc1 = s1; *sc1 != 0; sc1++)
      for (sc2 = s2; *sc2 != 0; sc2++)
         if (*sc1 == *sc2)
            return(sc1);
   return(0);
}


/* standard template: char *strrchr(const char *s, int c).
   Finds last occurrence of c in s */

char *strrchr(char *s, unsigned int8 c)
{
   char *p;

   for (p = 0; ; s++)
   {
      if (*s == c)
         p = s;
      if (*s == '\0')
         return(p);
   }
}
/* computes length of max initial segment of s1 consisting
   entirely of characters from s2 */

unsigned int8  strspn(char *s1, char *s2)
{
   char *sc1, *sc2;

   for (sc1 = s1; *sc1 != 0; sc1++)
      for (sc2 = s2; ; sc2++)
    if (*sc2 == '\0')
       return(sc1 - s1);
         else if (*sc1 == *sc2)
            break;
   return(sc1 - s1);
}
/* standard template:
   char *strstr(const char *s1, const char *s2);
   Locates first occurence of character sequence s2 in s1;
   returns 0 if s2 is empty string

   Uncomment #define FASTER_BUT_MORE_ROM at the top of the
   file to use the faster algorithm */
char *strstr(char *s1, char *s2)
{
   char *s, *t;

   #ifdef FASTER_BUT_MORE_ROM
   if (*s2 == '\0')
         return(s1);
   #endif

   while (*s1)
   {
      for(s = s1, t = s2; *t && (*s == *t); ++s, ++t);

      if (*t == '\0')
         return s1;
      ++s1;
      #ifdef FASTER_BUT_MORE_ROM
         while(*s1 != '\0' && *s1 != *s2)
            ++s1;
      #endif
   }
   return 0;
}

/* standard template: char *strtok(char *s1, const char *s2).

   Finds next token in s1 delimited by a character from separator
   string s2 (which can be different from call to call).  First call
   starts at beginning of s1 searching for first character NOT
   contained in s2; returns 0 if none is found.
   If one is found, it is the start of first token (return value).
   Function then searches from there for a character contained in s2.
   If none is found, current token extends to end of s1, and subsequent
   searches for a token will return 0.  If one is found, it is
   overwritten by '\0', which terminates current token.  Function saves
   pointer to following character from which next search will start.
   Each subsequent call, with 0 as first argument, starts searching
   from saved pointer */

char *strtok(char *s1, char *s2)
{
   char *beg, *end;
   static char *save;

   beg = (s1)? s1: save;
   beg += strspn(beg, s2);
   if (*beg == '\0')
   {
      *save = ' ';
      return(0);
   }
   end = strpbrk(beg, s2);
   if (*end != '\0')
   {
      *end = '\0';
      end++;
   }
   save = end;
   return(beg);
}

/*****************************************************************/
/*Miscellaneous functions*/
/* standard template
maps error number in errnum to an error message string
Returns: Pointer to string
*/
#ifdef _ERRNO
char * strerror(unsigned int8 errnum)
{
static char s[13];
switch( errnum)
{
case 0:
   strcpy(s,"no errors");
   return s;
case EDOM :
   strcpy(s,"domain error");
   return s;
case ERANGE:
   strcpy(s,"range error");
   return s;
}
}
#ENDIF
/* standard template: size_t strlen(const char *s).
   Computes length of s1 (preceding terminating 0) */

unsigned int8 strlen(char *s)
{
   char *sc;

   for (sc = s; *sc != 0; sc++);
   return(sc - s);
}

/* standard template: size_t stricmp(const char *s1, const char *s2).
   Compares s1 to s2 ignoring case (upper vs. lower) */

signed int8 stricmp(char *s1, char *s2)
{
 for(; *s1==*s2||(isalpha(*s1)&&isalpha(*s2)&&(*s1==*s2+32||*s2==*s1+32));
    s1++, s2++)
    if (*s1 == '\0')
       return(0);
 return((*s1 < *s2) ? -1: 1);
}


/* standard template: char *strlwr(char *s).
   Replaces uppercase letters by lowercase;
   returns pointer to new string s */

char *strlwr(char *s)
{
   char *p;

   for (p = s; *p != '\0'; p++)
      if (*p >= 'A' && *p <='Z')
         *p += 'a' - 'A';
   return(s);
}

/* standard template: char *strupr(char *s).
   Replaces lowercase letters by upercase;
   returns pointer to new string s */

char *strupr(char *s)
{
   char *p;

   for (p = s; *p != '\0'; p++)
      if (*p >= 'a' && *p <='z')
         *p -= 'a' - 'A';
   return(s);
}


/************************************************************/


#endif
