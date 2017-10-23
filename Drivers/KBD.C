///////////////////////////////////////////////////////////////////////////
////                             KBD.C                                 ////
////                  Generic keypad scan driver                       ////
////                                                                   ////
////  kbd_init()   Must be called before any other function.           ////
////                                                                   ////
////  c = kbd_getc(c)  Will return a key value if pressed or /0 if not ////
////                   This function should be called frequently so as ////
////                   not to miss a key press.                        ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

////////////////// The following defines the keypad layout on port D

// Un-comment the following define to use port B
// #define use_portb_kbd TRUE

// Make sure the port used has pull-up resistors (or the LCD) on
// the column pins

#if defined use_portb_kbd
   #byte kbd = getenv("SFR:PORTB")
#else
   #byte kbd = getenv("SFR:PORTD")
#endif

#if defined use_portb_kbd
   #define set_tris_kbd(x) set_tris_b(x)
#else
   #define set_tris_kbd(x) set_tris_d(x)
#endif

//Keypad connection:   (for example column 0 is B2)
//                Bx:

#ifdef blue_keypad  ///////////////////////////////////// For the blue keypad
#define COL0 (1 << 2)
#define COL1 (1 << 3)
#define COL2 (1 << 6)

#define ROW0 (1 << 4)
#define ROW1 (1 << 7)
#define ROW2 (1 << 1)
#define ROW3 (1 << 5)

#else ////////////////////////////////////////////////// For the black keypad
#define COL0 (1 << 5)
#define COL1 (1 << 6)
#define COL2 (1 << 7)

#define ROW0 (1 << 1)
#define ROW1 (1 << 2)
#define ROW2 (1 << 3)
#define ROW3 (1 << 4)

#endif

#define ALL_ROWS (ROW0|ROW1|ROW2|ROW3)
#define ALL_PINS (ALL_ROWS|COL0|COL1|COL2)

// Keypad layout:
char const KEYS[4][3] = {{'1','2','3'},
                         {'4','5','6'},
                         {'7','8','9'},
                         {'*','0','#'}};

#define KBD_DEBOUNCE_FACTOR 33    // Set this number to apx n/333 where
                                  // n is the number of times you expect
                                  // to call kbd_getc each second


void kbd_init() {
}

char kbd_getc( ) {
   static BYTE kbd_call_count;
   static int1 kbd_down;
   static char last_key;
   static BYTE col;

   BYTE kchar;
   BYTE row;

   kchar='\0';
   if(++kbd_call_count>KBD_DEBOUNCE_FACTOR) {
       switch (col) {
         case 0   : set_tris_kbd(ALL_PINS&~COL0);
                    kbd=~COL0&ALL_PINS;
                    break;
         case 1   : set_tris_kbd(ALL_PINS&~COL1);
                    kbd=~COL1&ALL_PINS;
                    break;
         case 2   : set_tris_kbd(ALL_PINS&~COL2);
                    kbd=~COL2&ALL_PINS;
                    break;
       }

       if(kbd_down) {
         if((kbd & (ALL_ROWS))==(ALL_ROWS)) {
           kbd_down=FALSE;
           kchar=last_key;
           last_key='\0';
         }
       } else {
          if((kbd & (ALL_ROWS))!=(ALL_ROWS)) {
             if((kbd & ROW0)==0)
               row=0;
             else if((kbd & ROW1)==0)
               row=1;
             else if((kbd & ROW2)==0)
               row=2;
             else if((kbd & ROW3)==0)
               row=3;
             last_key =KEYS[row][col];
             kbd_down = TRUE;
          } else {
             ++col;
             if(col==3)
               col=0;
          }
       }
      kbd_call_count=0;
   }
  set_tris_kbd(ALL_PINS);
  return(kchar);
}
