/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////                        ex_usb_keyboard.c                        ////
////                                                                 ////
//// An example of creating a USB HID Keyboard.  When connected and  ////
//// enumerated to the computer, characters received over serial     ////
//// port are sent to PC as keyboard messages.                       ////
////                                                                 ////
//// This file is part of CCS's PIC USB driver code.  See USB.H      ////
//// for more documentation and a list of examples.                  ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// VERSION HISTORY                                                 ////
////                                                                 ////
//// Mar 11, 2013:                                                   ////
////  Added Enhanced PIC16 support.                                  ////
////                                                                 ////
//// April 2nd, 2010:                                                ////
////  Initial release of HID keyboard demo.                          ////    ////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2009 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#include <ex_usb_common.h>

/////////////////////////////////////////////////////////////////////////////
//
// Include the CCS USB Libraries.  See the comments at the top of these
// files for more information
//
/////////////////////////////////////////////////////////////////////////////
#if __USB_PIC_PERIF__
 #if defined(__PCM__)
  #include <pic16f_usb.h>   //Microchip PIC16Fxxxx hardware layer for usb.c
 #elif defined(__PCH__)
  #include <pic18_usb.h>   //Microchip PIC18Fxx5x hardware layer for usb.c
 #elif defined(__PCD__)
  #include <pic24_usb.h>   //Microchip PIC18Fxx5x hardware layer for usb.c
 #endif
#else
 #include <usbn960x.h>   //National 960x hardware layer for usb.c
#endif
#include <usb_desc_keyboard.h>    //USB Configuration and Device descriptors for this UBS device
#include <usb.c>        //handles usb setup tokens and get descriptor reports

/////////////////////////////////////////////////////////////////////////////
//
// usb_debug_task()
//
// When called periodically, displays debugging information over serial
// to display enumeration and connection states.  Also lights LED2 and LED3
// based upon enumeration and connection status.
//
/////////////////////////////////////////////////////////////////////////////
void usb_debug_task(void) 
{
   static int8 last_connected;
   static int8 last_enumerated;
   int8 new_connected;
   int8 new_enumerated;

   new_connected=usb_attached();
   new_enumerated=usb_enumerated();

   if (new_connected)
      LED_ON(LED2);
   else
      LED_OFF(LED2);

   if (new_enumerated)
      LED_ON(LED3);
   else
      LED_OFF(LED3);

   if (new_connected && !last_connected)
      printf("\r\n\nUSB connected, waiting for enumaration...");
   if (!new_connected && last_connected)
      printf("\r\n\nUSB disconnected, waiting for connection...");
   if (new_enumerated && !last_enumerated)
      printf("\r\n\nUSB enumerated by PC/HOST");
   if (!new_enumerated && last_enumerated)
      printf("\r\n\nUSB unenumerated by PC/HOST, waiting for enumeration...");

   last_connected=new_connected;
   last_enumerated=new_enumerated;
}

typedef enum
{
   KB_MODIFIER_LEFT_CTRL = 1,
   KB_MODIFIER_LEFT_SHIFT = 2,
   KB_MODIFIER_LEFT_ALT = 4,
   KB_MODIFIER_LEFT_GUI = 8,
   KB_MODIFIER_RIGHT_CTRL = 16,
   KB_MODIFIER_RIGHT_SHIFT = 32,
   KB_MODIFIER_RIGHT_ALT = 64,
   KB_MODIFIER_RIGHT_GUI = 128
} kb_modifier_t;

typedef enum
{
   USB_KBD_SCANCODE_ESC = 0x29,
   USB_KBD_SCANCODE_CAPS_LOC = 0x39,
   USB_KBD_SCANCODE_F1 = 0x3A, //F2 is 0x3B, F3 is 0x3C, etc.  this is valid up to F12
   USB_KBD_SCANCODE_PRTSC = 0x46,
   USB_KBD_SCANCODE_SCROLL_LOCK = 0x47,
   USB_KBD_SCANCODE_PAUSE = 0x48,
   USB_KBD_SCANCODE_INS = 0x49,
   USB_KBD_SCANCODE_HOME = 0x4A,
   USB_KBD_SCANCODE_PG_UP = 0x4B,
   USB_KBD_SCANCODE_DEL = 0x4C,
   USB_KBD_SCANCODE_END = 0x4D,
   USB_KBD_SCANCODE_PG_DN = 0x4E,
   USB_KBD_SCANCODE_RIGHT = 0x4F,
   USB_KBD_SCANCODE_LEFT = 0x50,
   USB_KBD_SCANCODE_DOWN = 0x51,
   USB_KBD_SCANCODE_UP = 0x52,
   USB_KBD_SCANCODE_NUM_LOCK = 0x53,
   USB_KBD_SCANCODE_WIN_MENU = 0x65,
   USB_KBD_SCANCODE_F13 = 0x68,   //F14 is 0x69, F15 is 0x6A, etc.  this is valid up to F24
   USB_KBD_SCANCODE_HELP = 0x75,
   USB_KBD_SCANCODE_UNDO = 0x7A,
   USB_KBD_SCANCODE_CUT = 0x7B,
   USB_KBD_SCANCODE_COPY = 0x7C,
   USB_KBD_SCANCODE_PASTE = 0x7D,
   USB_KBD_SCANCODE_MUTE = 0x7F,
   USB_KBD_SCANCODE_VOL_UP = 0x80,
   USB_KBD_SCANCODE_VOL_DOWN = 0x81
} kb_scancode_t;

// convert ascii character to USB HID Keyboard scancode.
// realize that there is no difference between 'A'/'a' or '1'/'!', they
// have the same scancode.  in order to differentiate between these the SHIFT
// modifier has to be set (see kb_modifier_t).
// see kb_scancode_t for other scancodes that are not ascii.
// bit8 will be set if shift needs to be set (for upper case chars or '!' vs '1')
unsigned int16 ToHidKeyboardScancode(char c)
{
   unsigned int16 shift = 0;
   
   if ((c>='A')&&(c<='Z'))
   {
      c = tolower(c);
      shift = 0x100;
   }
   if ((c>='a')&&(c<='z'))
   {
      return(((c-='a')+4)|shift);
   }
   if ((c>='1')&&(c<='9'))
   {
      return((c-='0')+0x1D);
   }
   switch(c)
   {
      case '!':   return(0x11E);
      case '@':   return(0x11F);
      case '#':   return(0x120);
      case '$':   return(0x121);
      case '%':   return(0x122);
      case '^':   return(0x123);
      case '&':   return(0x124);
      case '*':   return(0x125);
      case '(':   return(0x126);
      case ')':   return(0x127);
      case '0':   return(0x27);
      case '\n':  return(0x28);  //enter
      case '\r':  return(0x28);  //enter
      case '\b':  return(0x2A);  //backspace
      case '\t':  return(0x2B);  //tab
      case ' ':   return(0x2C);  //space
      case '_':   return(0x12D);
      case '-':   return(0x2D);
      case '+':   return(0x12E);
      case '=':   return(0x2E);
      case '{':   return(0x12F);
      case '[':   return(0x2F);
      case '}':   return(0x130);
      case ']':   return(0x30);
      case '|':   return(0x131);
      case '\\':   return(0x31);
      case ':':   return(0x133);
      case ';':   return(0x33);
      case '"':   return(0x134);
      case '\'':   return(0x34);
      case '~':   return(0x135);
      case '`':   return(0x35);
      case '<':   return(0x136);
      case ',':   return(0x36);
      case '>':   return(0x137);
      case '.':   return(0x37);
      case '?':   return(0x138);
      case '/':   return(0x38);
   }
}

/////////////////////////////////////////////////////////////////////////////
//
// usb_keyboard_task()
//
// Sends a packet of keyboard data.  The protocol was specified in the HID
// report descriptor (see usb_desc_kbmouse.h), and is:
//     tx_msg[0] = modifier (see kb_modifier_t)
//     tx_msg[1] = const 0
//     tx_msg[2:6] = an array of held down keys.  a=4, b=5, etc.
//                   if msg[2:7]={0} then no keys are held down
//
//     rx_msg[0] = 5bit bitmap of led status
//
/////////////////////////////////////////////////////////////////////////////
void usb_keyboard_task(void) 
{
   unsigned int8 tx_msg[7];
   unsigned int8 leds;
   unsigned int16 scancode;
   
   if (usb_tbe(1))
   {
      memset(tx_msg, 0x00, sizeof(tx_msg));

      if (kbhit())
      {
         scancode = ToHidKeyboardScancode(getc());
         
         if (bit_test(scancode, 8))
            tx_msg[0] = KB_MODIFIER_LEFT_SHIFT;
         
         tx_msg[2] = scancode;
      }
      
      usb_put_packet(1, tx_msg, sizeof(tx_msg), USB_DTS_TOGGLE);
   }

   //receive NUM LOCK, CAPS LOCK, etc LED status from PC.
   //we won't do anything with it.
   if (usb_kbhit(1)) 
   {
      usb_get_packet(1, &leds, 1);
   }
}

void main(void) 
{
   HW_INIT();

   LED_ON(LED1);
   LED_OFF(LED2);
   LED_OFF(LED3);

   printf("\r\n\nCCS HID Keyboard Demo");
   printf("\r\nCharacters received over serial get sent as a HID keyboard");
   
  #ifdef __PCH__
   printf("\r\nPCH: v");
   printf(__PCH__);
  #elif defined(__PCD__)
   printf("\r\nPCD: v");
   printf(__PCD__);
  #else
   printf("\r\nPCM: v");
   printf(__PCM__);
  #endif

   usb_init_cs();

  #if !(__USB_PIC_PERIF__)
   printf("\r\nUSBN: 0x%X", usbn_get_version());
  #endif
   printf("\r\n");

   while (TRUE)
   {
      usb_task();
      usb_debug_task();

      if (usb_enumerated())
      {
         usb_keyboard_task();
      }
   }
}

