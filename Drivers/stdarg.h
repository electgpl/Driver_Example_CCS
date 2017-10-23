///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#ifndef _STDARG

#define _STDARG

typedef char * va_list;

#define va_start(a,b)  a=_VA_LIST
#define va_arg(a,b)    *(b *)_va_arg(a)
#define va_end
#define nargs          _nargs(&_va_list)

int8 _nargs(int8 * ptr) {
  int8 n;

  n=0;
  while(*ptr) {
    n++;
    ptr+=*ptr;
  }
  return(n);
}

#endif
