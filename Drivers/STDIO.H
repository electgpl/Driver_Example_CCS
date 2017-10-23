///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#ifndef _STDIO
#define _STDIO
#include <string.h>
#ifndef getc
#define getc getch
#define getchar getch
#define puts(s) {printf(s); putchar(13); putchar(10);}
#define putc putchar
#endif
/* maps error number to an error message. Writes a sequence of characters to
stderr stream thus: if s is not null then string pointed to by s follwed by
a colon (:) and a space and the appropriate error message returned by strerror
function with argument errno

Returns: no value
*/

#ifdef _ERRNO
void perror(char *s)
{
  if(s)
  fprintf(STDERR,"%s: ",s);
  fprintf(STDERR,"%s\r\n",strerror(errno));
}
#endif
#endif
