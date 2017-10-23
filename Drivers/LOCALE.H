///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

/*locale.h*/

#ifndef _LOCALE
#define _LOCALE
#ifndef _LIMITS
#include <limits.h>
#endif
            /*macros*/
        /*locale codes*/
#define LC_ALL 0
#define LC_COllATE 1
#define LC_CTYPE 2
#define LC_MONETARY 3
#define LC_NUMERIC 4
#define LC_TIME 5

            /* type definitions*/
struct lconv{
            /* controlled by LC_MONETARY*/
   char *currency_symbol;
   char *int_curr_symbol;
   char *mon_decimal_point;
   char *mon_grouping;
   char *mon_thousands_sep;
   char *positive_sign;
   char *negative_sign;
   char frac_digits;
   char int_frac_digits;
   char n_cs_precedes;
   char n_sep_by_space;
   char n_sign_posn;
   char p_cs_precedes;
   char p_sep_by_space;
   char p_sign_posn;
            /* controlled by LC_NUMERIC*/
   char *decimal_point;
   char *grouping;
   char *thousands_sep;
   };

            /*declarations*/
static char null[]="";
static char dot[]=".";

struct lconv _Clocale =
{
            /*  LC_MONETARY  */
   null,
   null,
   null,
   null,
   null,
   null,
   null,
   CHAR_MAX,
   CHAR_MAX,
   CHAR_MAX,
   CHAR_MAX,
   CHAR_MAX,
   CHAR_MAX,
   CHAR_MAX
   CHAR_MAX,
            /*  LC_NUMERIC   */
   dot,
   null,
   null
};

typedef struct lconv *  plconv;
plconv localeconv()
{
   return (&_Clocale);
}

typedef char * pchar;
pchar setlocale(unsigned int8 category, char *locale)
{
   return null; /*localization not supported */
}

#endif
