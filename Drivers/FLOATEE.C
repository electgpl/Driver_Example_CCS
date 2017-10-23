///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

// n is an offset into the eeprom.  For floats
// you must increment it by 4.  For example if
// the first float is at 0 the second one should
// be at 4 and the third at 8.

WRITE_FLOAT_EXT_EEPROM(long int n, float data) {
   int i;

   for (i = 0; i < 4; i++)
     write_ext_eeprom(i + n, *((int8 *)(&data) + i));
}

float READ_FLOAT_EXT_EEPROM(long int n) {
   int i;
   float data;

   for (i = 0; i < 4; i++)
     *((int8 *)(&data) + i) = read_ext_eeprom(i + n);

   return(data);
}
