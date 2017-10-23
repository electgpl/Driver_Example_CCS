///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 2012 Custom Computer Services                ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#ifndef _IOS
#define _IOS

// Streams ///////////////////////////////////////////////////////////////////
// Any stream name defined in a #use RS232 may be used in place of cout or cin
// Zero indicates the default stream
// Also supported is the name of any char string or a function 

#define cout 0
#define cin  0

// Useful constants //////////////////////////////////////////////////////////

#define endl "\r\n"               // Linux users might just want "\n"
#define ends  _iosctrl('C')'\000' // Outputs a null char

// Manipulators //////////////////////////////////////////////////////////////

#define hex             _iosctrl('H')    // Hex format numbers
#define dec             _iosctrl('D')    // Decimal format numbers
#define setprecision(x) _iosctrl('P',x)  // Set number of places after the decimal point
#define setw(x)         _iosctrl('W',x)  // Set total number of characters output for numbers
#define strspace        _iosctrl('S')    // Allow spaces to be input into strings
#define nostrspace      _iosctrl('s')    // Spaces terminate string entry
#define boolalpha       _iosctrl('B')    // Output int1 as true and false
#define noboolalpha     _iosctrl('b')    // Output int1 as 1 and 0
#define fixed           _iosctrl('e')    // Floats in decimal format
#define scientific      _iosctrl('E')    // Floats use E notation
#define iosdefault      _iosctrl('R')    // All manipulators to default settings
#define echo            _iosctrl('O')    // Input is echoed
#define noecho          _iosctrl('o')    // Input is not echoed


#endif

