/////////////////////////////////////////////////////////////////////////
////                             EX_STR.C                            ////
////                                                                 ////
////  This program allows a user to enter in a string of text.  The  ////
////  program then calculates how many words were entered, how many  ////
////  unique words were entered, and how many numbers were entered.  ////
////  This program demonstrates the use of many differnent string    ////
////  functions.                                                     ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCB        pin A2 to RS232 RX, pin A3 to RS232 TX           ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////                                                                 ////
////  This example will work with the PCB, PCM and PCH compilers.    ////
////  The following conditional compilation lines are used to        ////
////  include a valid device for each compiler.  Change the device,  ////
////  clock and RS232 pins for your hardware if needed.              ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////


#if defined(__PCB__)
#include <16C56.h>
#fuses HS,NOWDT,NOPROTECT
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_A3, rcv=PIN_A2)

#elif defined(__PCM__)
#include <16F877.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCH__)
#include <18F458.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#endif


#include <string.h>
#include <input.c>

#define  STRING_SIZE    51             // The number of characters allowed to input


// This function cleans up the string. It removes all
// punctuation and all multiple spaces. It also removes
// any leading and trailing spaces.
void clean_up_str(char *str)   {
   char separators[8];
   char *ptr, *clean_str;

   strcpy(separators," ,;.\"!?");      // initialize variables
   clean_str=str;

   do
   {
      ptr = strpbrk(str,separators);   // search for punctuation or space
      if(ptr == str)                   // if first char is punct or space
      {
         ++str;                        // simply go to next char
      }
      else if(ptr == 0)                // if search returns end of str
      {
         while(*str != 0)              // loop until end of string
            *clean_str++ = *str++;     // copy characters to remove punct and spaces
         if(*--clean_str == ' ')       // if the last char is a space
            *clean_str = 0;            // remove it and terminate the string
         else
            *++clean_str = 0;          // otherwise just terminate the string
      }
      else                             // if somewhere in middle of string
      {
         while(ptr != str)             // loop until pointers match up
            *clean_str++ = *str++;     // copy chars to remove punct and spaces
         *clean_str++ = ' ';           // then add a space
      }
   } while(ptr != 0);                  // loop until through string
}


// This function return the number of words in the string.
// The words must be separated by a single space.
int get_num_words(char *str)   {
   int retval;

   retval=0;                           // initialize variable
   while(*str!=0)                      // loop through all of string
   {
      str = strchr(str,' ');           // find first space

      retval++;                        // increment counter
      if(*str == 0)                    // if at the end, quit
         break;
      else                             // otherwise increment pointer
         str++;
  }
   return(retval);
}


// This function returns the number of words in the string
// excluding any repeated words.  The words must be separated
// by a single space.
int get_num_unique_words(char *str)   {
   int retval, i;
   char temp_str[STRING_SIZE], space[2];
   char *word, *srch_string, *srch_word;

   strcpy(temp_str,str);               // copy the string to temp string
   strcpy(space," ");                  // initialize local variables
   retval=0;
   srch_string=temp_str;

   word=strtok(temp_str,space);        // find next word
   while(word!=0)                      // loop until all words looked at
   {
      retval++;

      srch_string+= strlen(word)+1;    // points to next word after 0 in temp

      srch_word=temp_str;
      while(srch_word!=0)              // loop while not end of words
      {
         srch_word=strstr(srch_string,word); // ptr points to found word or 0

         if(srch_word!=0)              // if 0, no matches found.  Otherwise..
         {
            i = srch_word + strlen(word);    // i=end of found word
            while(srch_word<i)         // insert spaces where the
               *srch_word++=' ';       // word was so no checking twice

            clean_up_str(srch_string); // remove the spaces (remove the double word)
         }
      }
      word=strtok(0,space);            // go to next word
   }
   return(retval);
}


// this function return the number of numbers in the string passed
// into it.  For example: 123 would return 3 because there are 3 numbers.
int get_num_numbers(char *str)   {
   int retval;

   retval=0;                           // initialize varialbes
   while(*str!=0)                      // loop until all characters checked
   {
      if(isdigit(*str++))              // if the character is a number
         retval++;                     // increment the counter
   }
   return(retval);
}


// this function allows users to enter in text, and then it calculates
// some statistics including the number of words, the number of unique
// words and the number of numbers in the user entered text.
void main()   {
   char input_str[STRING_SIZE];

   while(TRUE)
   {
      printf("\n\rEnter a string of text.  The maximum number of characters is %U.\n\r", STRING_SIZE-1);
      get_string(input_str,STRING_SIZE);     // gets the string

      clean_up_str(input_str);               // removes all punctuation and extra spaces

      printf("\n\n\rSTATISTICS:\n\r");
      printf("You entered %U word(s).\n\r", get_num_words(input_str));
      printf("You entered %U unique word(s).\n\r", get_num_unique_words(input_str));
      printf("You entered %U number(s).\n\r", get_num_numbers(input_str));
   }
}

