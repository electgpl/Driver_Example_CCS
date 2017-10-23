///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

/* float.h  */


#ifndef _FLOAT
#define _FLOAT
    /* Float properties */

#define FLT_RADIX       2
#define FLT_MANT_DIG    24                              // # of bits in mantissa

#define FLT_DIG         6                               // # of decimal digits of precision

#define FLT_MIN_EXP     (-125)                          // min binary exponent
#define FLT_MIN_10_EXP  (-37)                           // min decimal exponent
#define FLT_MAX_EXP     128                             // max binary exponent
#define FLT_MAX_10_EXP  38                              // max decimal exponent
#define FLT_MAX         3.402823466e+38F                // max value
#define FLT_EPSILON     1.192092896e-07F                // smallest such that 1.0+FLT_EPSILON != 1.0
#define FLT_MIN         1.175494351e-38F                // min positive value


      /* Double properties */
#define DBL_MANT_DIG    53                              // # of bits in mantissa

#define DBL_DIG         15                              // # of decimal digits of precision

#define DBL_MIN_EXP     (-1021)                         // min binary exponent
#define DBL_MIN_10_EXP  (-307)                          // min decimal exponent
#define DBL_MAX_EXP     1024                            // max binary exponent
#define DBL_MAX_10_EXP  308                             // max decimal exponent
#define DBL_MAX         1.79769313486231e+308F          // max value
#define DBL_EPSILON     2.2204460492503131e-16F         // smallest such that 1.0+FLT_EPSILON != 1.0
#define DBL_MIN         2.22507385850721e-308F          // min positive value

         /*Long double properties */
         
#define LDBL_MANT_DIG    64                             // # of bits in mantissa

#define LDBL_DIG         18                             // # of decimal digits of precision

#define LDBL_MIN_EXP     (-16381)                       // min binary exponent
#define LDBL_MIN_10_EXP  (-4931)                        // min decimal exponent
#define LDBL_MAX_EXP     16384                          // max binary exponent
#define LDBL_MAX_10_EXP  4932                           // max decimal exponent
#define LDBL_MAX         1.18973149535723176502e+4932F  // max value
#define LDBL_EPSILON     1.084202172485504434e-019F     // smallest such that 1.0+FLT_EPSILON != 1.0
#define LDBL_MIN         3.36210314311209350626e-4932F  // min positive value
#endif


