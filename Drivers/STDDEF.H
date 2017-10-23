///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#ifndef _STDDEF

#define _STDDEF

#if sizeof(unsigned int8 *)==1
#define ptrdiff_t unsigned int8
#else
#define ptrdiff_t unsigned int16
#endif

#if defined(__PCB__)
#define size_t unsigned int8
#else
#define size_t unsigned int16
#endif
#define wchar_t char
#define NULL 0

#define offsetof(s,f) (offsetofbit(s,f)/8)

#endif
