////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services            ////
//// This source code may only be used by licensed users of the CCS C   ////
//// compiler.  This source code may only be distributed to other       ////
//// licensed users of the CCS C compiler.  No other use, reproduction  ////
//// or distribution is permitted without written permission.           ////
//// Derivative programs created using this software in object code     ////
//// form are not restricted in any way.                                ////
////////////////////////////////////////////////////////////////////////////

#ifndef _CTYPE
#define _CTYPE

#define islower(x)  isamong(x,"abcdefghijklmnopqrstuvwxyz")
#define isupper(x)  isamong(x,"ABCDEFGHIJKLMNOPQRSTUVWXYZ")
#define isalnum(x)  isamong(x,"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz")
#define isalpha(x)  isamong(x,"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz")
#define isdigit(x)  isamong(x,"0123456789")
#define isspace(x)  ((x)==' ')
#define isxdigit(x) isamong(x,"0123456789ABCDEFabcdef")
#define iscntrl(x)  ((x)<' ')
#define isprint(x)  ((x)>=' ')
#define isgraph(x)  ((x)>' ')
#define ispunct(x)  (((x)>' ')&&!isalnum(x))

#endif

