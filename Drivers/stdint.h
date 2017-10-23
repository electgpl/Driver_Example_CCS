///////////////////////////////////////////////////////////////////////////
////                                                                   ////
////                           stdint.h                                ////
////                                                                   ////
//// Standard integer definitions.                                     ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2008 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#ifndef _STDINT

#define _STDINT

//////////// exact width

typedef signed int8 int8_t;
typedef unsigned int8 uint8_t;
typedef signed int16 int16_t;
typedef unsigned int16 uint16_t;
typedef signed int32 int32_t;
typedef unsigned int32 uint32_t;

#if defined(__PCD__)
//typedef signed int24 int24_t;
//typedef unsigned int24 uint24_t;
typedef signed int64 int64_t;
typedef unsigned int64 uint64_t;
#endif

#define INT8_MAX  (128)
#define INT8_MIN  (-127)
#define UINT8_MAX (255)

#define INT16_MAX  (32767)
#define INT16_MIN  (-32768)
#define UINT16_MAX (65535)

#define INT32_MAX  (2147483647)
#define INT32_MIN  (-2147483648)
#define UINT32_MAX (4294967295)

#if defined(__PCD__)
//#define INT24_MAX  (8388607)
//#define INT24_MIN  (-8388608)
//#define UINT24_MAX (16777215)

#define INT64_MAX  (9223372036854775807)
#define INT64_MIN  (-9223372036854775808)
#define UINT64_MAX (18446744073709551615)
#endif

///////// minimum width

typedef signed int8 int_least8_t;
typedef unsigned int8 uint_least8_t;
typedef signed int16 int_least16_t;
typedef unsigned int16 uint_least16_t;
typedef signed int32 int_least32_t;
typedef unsigned int32 uint_least32_t;

#if defined(__PCD__)
//typedef signed int24 int_least24_t;
//typedef unsigned int24 uint_least24_t;
typedef signed int64 int_least64_t;
typedef unsigned int64 uint_least64_t;
#endif

#define INT_LEAST8_MAX  (128)
#define INT_LEAST8_MIN  (-127)
#define UINT_LEAST8_MAX (255)

#define INT_LEAST16_MAX  (32767)
#define INT_LEAST16_MIN  (-32768)
#define UINT_LEAST16_MAX (65535)

#define INT_LEAST32_MAX  (2147483647)
#define INT_LEAST32_MIN  (-2147483648)
#define UINT_LEAST32_MAX (4294967295)

#if defined(__PCD__)
//#define INT_LEAST24_MAX  (8388607)
//#define INT_LEAST24_MIN  (-8388608)
//#define UINT_LEAST24_MAX (16777215)

#define INT_LEAST64_MAX  (9223372036854775807)
#define INT_LEAST64_MIN  (-9223372036854775808)
#define UINT_LEAST64_MAX (18446744073709551615)
#endif

///////// fastest width

#if defined(__PCD__)
typedef signed int16 int_fast8_t;
typedef unsigned int16 uint_fast8_t;
#define INT_FAST8_MAX  (32767)
#define INT_FAST8_MIN  (-32768)
#define UINT_FAST8_MAX (65535)
typedef signed int16 int_fast16_t;
typedef unsigned int16 uint_fast16_t;
//typedef signed int24 int_fast24_t;
//typedef unsigned int24 uint_fast24_t;
typedef signed int64 int_fast64_t;
typedef unsigned int64 uint_fast64_t;
#else
typedef signed int8 int_fast8_t;
typedef unsigned int8 uint_fast8_t;
#define INT_FAST8_MAX  (128)
#define INT_FAST8_MIN  (-127)
#define UINT_FAST8_MAX (255)
typedef signed int16 int_fast16_t;
typedef unsigned int16 uint_fast16_t;
#endif

typedef signed int32 int_fast32_t;
typedef unsigned int32 uint_fast32_t;

#define INT_FAST16_MAX  (32767)
#define INT_FAST16_MIN  (-32768)
#define UINT_FAST16_MAX (65535)

#define INT_FAST32_MAX  (2147483647)
#define INT_FAST32_MIN  (-2147483648)
#define UINT_FAST32_MAX (4294967295)

#if defined(__PCD__)
//#define INT_FAST24_MAX  (8388607)
//#define INT_FAST24_MIN  (-8388608)
//#define UINT_FAST24_MAX (16777215)

#define INT_FAST64_MAX  (9223372036854775807)
#define INT_FAST64_MIN  (-9223372036854775808)
#define UINT_FAST64_MAX (18446744073709551615)
#endif

//////////// big enough to hold pointers (OPTIONAL)

/// TODO

/// intptr_t uintptr_t

/// INTPTRN_MIN INTPTRN_MAX UINTPTRN_MAX


/////////// greatest width (OPTIONAL)

/// TODO

/// intmax_t uintmax_t

/// INTMAXN_MIN INTMAXN_MAX UINTMAXN_MAX

/// INTMAX_C(value) UINTMAX_C(value)


#endif
