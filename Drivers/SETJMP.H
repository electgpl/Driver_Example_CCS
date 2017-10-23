///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#ifndef _SETJMP
#define _SETJMP

#ifdef __PCH__
#define _NSETJMP      5
#else
#define _NSETJMP      2
#endif

/* The buffer where the environment will be stored by setjmp
 */
typedef BYTE jmp_buf[_NSETJMP];

/* Stores information on the current calling context in a data
 * object of type jmp_buf and which marks where you want control
 * to pass on a corresponding longjmp call.
 *
 * Parameters:
 *    env
 *       [out] The data object that will receive the current environment
 *
 * Return Values:
 *    If the return is from a direct invocation, this function returns 0.
 *    If the return is from a call to the longjmp function, the setjmp
 *    function return a nonzero value and it's the same value passed to
 *      the longjmp function.
 */
#inline
int setjmp(jmp_buf env);

/* Performs the nonlocal transfer of control.
 *
 * Parameters:
 *    env
 *       [in] The data object that will be restored by this function
 *    val
 *       [in] The value that the function setjmp will return. If
 *       val is 0 then the function setjmp will return 1 instead.
 *
 * Return Values:
 *    After longjmp is completed, program execution continues as if
 *    the corresponding invocation of the setjmp function had just
 *    returned the value specified by val.
 */

#separate
int longjmp(int8 * env, int val);


//---------------------------------------------------------------------------
// Internal Declerations (Used by the library only)
//---------------------------------------------------------------------------

int __SETJMPVAL;

#ifdef __PCH__

typedef struct {
   long int address;
   int  stack;
} __jmp_environ;

#byte __SETJMP_STACK=0xffc

#inline
int setjmp(jmp_buf env)
{
   ((__jmp_environ*)env)->address = label_address(callback);
   ((__jmp_environ*)env)->stack = __SETJMP_STACK;

  return(0);

callback:
  return(__SETJMPVAL);
}

#separate
int longjmp(int8 * env, int val)
{
  int32 to;

  to = ((__jmp_environ*)env)->address;
  __SETJMP_STACK = ((__jmp_environ*)env)->stack;

  if (val == 0)
    val = 1;
  __SETJMPVAL = val;

  goto_address(to);
}

#else

#inline
int setjmp(jmp_buf env)
{
  int i;

  *((int16*)env) = label_address(callback);
  return(0);

callback:
  return(__SETJMPVAL);
}

#separate
int longjmp(int8 * env, int val)
{
  int i;

  if (val == 0)
    val = 1;
  __SETJMPVAL = val;

  goto_address(*((int16*)env));
}

#endif

#endif
