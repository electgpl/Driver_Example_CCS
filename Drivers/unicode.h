///////////////////////////////////////////////////////////////////////////
//                              unicode.h                                //
//             Library for Decoding and Encoding UTF-8 strings.          //
//                                                                       //
// unsigned int32 UTF8Decode(char *scr); Returns next character from the //
//                                       null terminated UTF-8 string    //
//                                       pointed to by scr.              //
//                                                                       //
// void UTF8Decode(char next, unsigned int32 **dst);                     //
//             When a complete UTF-8 string is received it stores the    //
//             decoded character into the array pointed to by the        //
//             pointer pointed to by dst and increments the pointer      //
//             pointed to by dst.                                        //
//                                                                       //
// unsigned int8 UTF8Encode(char *dst, unsigned int32 src);              //
//             Encodes src in to an UTF-8 string and stores it into the  //
//             array pointed to by dst, and returns the number of bytes  //
//             stored into the array.                                    //
//                                                                       //
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2009 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////
#ifndef __UNICODE_H__
#define __UNICODE_H__

// Purpose:    Decode an UTF-8 string.
// Inputs:     Pointer to the null terminated UTF-8 String.
// Outputs:    The UTF-8 Decode value 0x000000-0x10FFFF.
// Note:       If src is NULL it will continue from the position where the last 
//             string ended.
unsigned int32 UTF8Decode(char *scr)
{
   static char *ptr=0;
   unsigned int32 result;
   char c;
   
   if(scr!=0)
      ptr=scr;

   if(bit_test(*ptr,7))
   {
      c=(*ptr & 0xF0) >> 4;
      
      switch(c)
      {
         case 0x0C:
         case 0x0D:
            result=((unsigned int32)*ptr & 0x1F) << 6;
            ptr++;
            result|=((unsigned int32)*ptr & 0x3F);
            ptr++;
            break;
         case 0x0E:
            result=((unsigned int32)*ptr & 0x0F) << 12;
            ptr++;
            result|=((unsigned int32)*ptr & 0x3F) << 6;
            ptr++;
            result|=((unsigned int32)*ptr & 0x3F);
            ptr++;
            break;
         case 0x0F:
            result=((unsigned int32)*ptr & 0x07) << 18;
            ptr++;
            result|=((unsigned int32)*ptr & 0x3F) << 12;
            ptr++;
            result|=((unsigned int32)*ptr & 0x3F) << 6;
            ptr++;
            result|=((unsigned int32)*ptr & 0x3F);
            ptr++;
            break;
         default:
            result=0;
            ptr++;
            break;
      }
   }
   else
   {
      result=(unsigned int32)*ptr;
      ptr++;
   }
   return(result);
}

// Purpose:    Decode UTF-8 characters.
// Inputs:     1) The Next UTF-8 character to be Decoded.
//             2) A pointer to a pointer pointing to the where the result is to
//                be stored.
// Outputs:    None.
// Note:       When an entire UTF-8 string is received the Decoded value is
//             stored into the array pointed by the pointer pointed to by dst,
//             and the pointer pointed to by dst will be incremented to the next 
//             postion of the array.  Also the next postion in the array will be
//             null terminated.
void UTF8Decode(char next, unsigned int32 **dst)
{
   static char utf8[4];
   static int8 total_bytes=0;
   static int8 bytes_received=0;
   unsigned int32 result=0;
   char c;
   
   switch(bytes_received)
   {
      case 0:
         if(bit_test(next,7))
         {
            **dst=0;
            *dst=*dst+1;
            **dst=0;
            *dst=*dst+1;
            **dst=0;
            *dst=*dst+1;
            **dst=0;
            *dst=*dst-3;
            
            c=(next & 0xF0) >> 4;
            switch(c)
            {
               case 0xC:
               case 0xD:
                  total_bytes=2;
                  break;
               case 0xE:
                  total_bytes=3;
                  break;
               case 0xF:
                  total_bytes=4;
                  break;
               default:
                  total_bytes=0;
                  bytes_received=0;
                  return;
            }
            bytes_received++;
            utf8[0]=next;
            return;
         }
         else
         {
            bytes_received++;
            total_bytes=1;
         }
         break;
      case 1:
      case 2:
      case 3:
         if(bit_test(next,7) && !bit_test(next,6))
         {
            utf8[bytes_received]=next;
            bytes_received++;
            if(bytes_received<(total_bytes))
               return;
         }
         else
         {
            total_bytes=0;
            bytes_received=0;
            return;
         }
         break;
      default:
         total_bytes=0;
         bytes_received=0;
         return;
   }
   
   switch(total_bytes)
   {
      case 1:
         result=(unsigned int32)next;
         break;
      case 2:
         result=(((unsigned int32)utf8[0] & 0x1F) << 6) | ((unsigned int32)utf8[1] & 0x3F);
         break;
      case 3:
         result=(((unsigned int32)utf8[0] & 0x0F) << 12) | (((unsigned int32)utf8[1] & 0x3f) << 6) | ((unsigned int32)utf8[2] & 0x3F);
         break;
      case 4:
         result=(((unsigned int32)utf8[0] & 0x07) << 18) | (((unsigned int32)utf8[1] & 0x3F) << 12) | (((unsigned int32)utf8[2] & 0x3F) << 6) | ((unsigned int32)utf8[3] & 0x3F);
         break;
   }
   
   total_bytes=0;
   bytes_received=0;
   
   **dst=make8(result,0);
   *dst=*dst+1;
   **dst=make8(result,1);
   *dst=*dst+1;
   **dst=make8(result,2);
   *dst=*dst+1;
   **dst=make8(result,3);
   *dst=*dst+1;
   **dst=0;
   *dst=*dst+1;
   **dst=0;
   *dst=*dst+1;
   **dst=0;
   *dst=*dst+1;
   **dst=0;
   *dst=*dst-3;
}

//Purpose:     Encode a value into an UTF-8 string.
//Inputs:      1) Pointer to the array the UTF-8 string is to be stored.
//             2) The vaule to be encoded.
//Outputs:     The number of bytes needed to encode the value.
//Note:        The string is null terminated.
unsigned int8 UTF8Encode(char *dst, unsigned int32 src)
{
   char *ptr;
   
   ptr=dst;
   
   if(src<=0x7F)
   {
      *ptr++=make8(src,0);
      *ptr=0;
      return(1);
   }
   else if(src<=0x7FF)
   {
      *ptr++=((char)((src & 0x7C0) >> 6) | 0xC0);
      *ptr++=((char)(src & 0x3F) | 0x80);
      *ptr=0;
      return(2);
   }
   else if(src<=0xFFFF)
   {
      *ptr++=((char)((src & 0xF000) >> 12) | 0xE0);
      *ptr++=((char)((src & 0xFC0) >> 6) | 0x80);
      *ptr++=((char)(src & 0x3F) | 0x80);
      *ptr=0;
      return(3);
   }
   else if(src<=0x10FFFF)
   {
      *ptr++=((char)((src & 0x1C0000) >> 18) | 0xF0);
      *ptr++=((char)((src & 0x3F000) >> 12) | 0x80);
      *ptr++=((char)((src & 0xFC0) >> 6) | 0x80);
      *ptr++=((char)(src & 0x3F) | 0x80);
      *ptr=0;
      return(4);
   }
   else
   {
      *ptr=0;
      return(0);
   }
}

#endif
