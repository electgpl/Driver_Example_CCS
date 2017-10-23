///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

/* assert.h*/

#undef assert
#ifdef NDEBUG
   #define assert(test) (0)
#else
   #define assert(test) if(!(test)) { fprintf(stderr,"Assertion failed:"#test" ,file ");\
                                    fprintf(stderr,__FILE__);\
                                    fprintf(stderr," ,line %lu\r\n",(unsigned int16)__LINE__);}

#endif
