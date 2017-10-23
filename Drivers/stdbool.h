///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                          stdbool.h                                ////
////                                                                   ////
//// Standard boolean definitions                                      ////
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

#ifndef __STDBOOL_H__
#define __STDBOOL_H__

//remove TRUE and FALSE added by CCS's device .h file
#if defined(TRUE)
#undef TRUE
#endif
#if defined(FALSE)
#undef FALSE
#endif

typedef int1   bool;
#define true   1
#define false  0
#define __bool_true_false_are_defined  1

#endif