/////////////////////////////////////////////////////////////////////////
////                          EX_VOICE.C                             ////
////                                                                 ////
////  This example converts text typed by the user into speech.      ////
////  After the user types in the sentance, the words are searched   ////
////  for in the dictionary.  If the word is known, the address for  ////
////  the speech pattern in the ISD chip is put in an array.  If the ////
////  word is not known, the user must add it to the dictionary, and ////
////  then the program continues like the word was already known.    ////
////  The dictionary is stored in the internal eeprom.               ////
////                                                                 ////
////  Configure the CCS prototype card as follows:                   ////
////     Make the following connections:                             ////
////     ISD4003 Chip   Prototype Card                               ////
////          Pin            Pin                                     ////
////           1              C2                                     ////
////           2              C5                                     ////
////           3              C4                                     ////
////           4              GND with 0.22uF cap to ISD pin 27      ////
////         5-10             NC                                     ////
////          11              GND                                    ////
////          12              GND                                    ////
////          13              1uF cap to external pot                ////
////          14              1uF cap to to ISD pin 12               ////
////          15              NC                                     ////
////          16              0.1uF to + side of MIC (see diagram)   ////
////          17              0.1uF to - side of MIC (see diagram)   ////
////          18              +3V with 0.22uF cap to ISD pin 23      ////
////         19-22            NC                                     ////
////          23              GND                                    ////
////          24              5K pull-up resistor                    ////
////          25              5K pull-up resistor                    ////
////          26              GND                                    ////
////          27              +3V                                    ////
////          28              C3                                     ////
////                                                                 ////
////      LM386 Chip      Connect To                                 ////
////          Pin            Pin                                     ////
////           1              8 (LM386) with 10uF cap                ////
////           2              GND (Prototype card)                   ////
////           3              Wiper of external pot                  ////
////           4              GND (Prototype card)                   ////
////           5              Positive lead of speaker               ////
////           6              +5V (Prototype card)                   ////
////           7              GND (Prototype card) with 0.05uF cap   ////
////           8              1 (LM386) with 10uF cap                ////
////                                                                 ////
////                       MICROPHONE CIRCUIT                        ////
////                                                                 ////
////                            +5V                  SPEAKER CIRCUIT ////
////                             |                                   ////
////                            1K R                   LM386 pin 5   ////
////                             |---------|                |        ////
////                           10K R       |               +|        ////
////     ISD pin 16 --- 0.1uF ---|       220uF           speaker     ////
////                            MIC        |               -|        ////
////     ISD pin 17 --- 0.1uF ---|         |                |        ////
////                           10K R      GND              GND       ////
////                             |                                   ////
////                            GND                                  ////
////                                        POT CIRCUIT              ////
////                                                                 ////
////                                         ISD PIN 14              ////
////                                              |                  ////
////                                             1uF                 ////
////                                              |                  ////
////                      LM386 pin 3 --- (wiper)POT                 ////
////                                              |                  ////
////                                             GND                 ////
////                                                                 ////
////  Jumpers:                                                       ////
////     PCM,PCH    pin C7 to RS232 RX, pin C6 to RS232 TX           ////
////                                                                 ////
////  This example will work with the PCM and PCH compilers.  The    ////
////  following conditional compilation lines are used to include a  ////
////  valid device for each compiler.  Change the device, clock and  ////
////  RS232 pins for your hardware if needed.                        ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2003 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////


#if defined(__PCM__)
#include <16F877.h>
#fuses HS,WDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)

#elif defined(__PCH__)
#include <18F452.h>
#fuses HS,NOWDT,NOPROTECT,NOLVP
#use delay(clock=20000000)
#use rs232(baud=9600, xmit=PIN_C6, rcv=PIN_C7)
#endif

#include <isd4003.c>
#include <stdio.h>
#include <string.h>
#include <input.c>

#define SENTANCE_LENGTH       50
#define WORD_LENGTH           15
#define ADDRESS_LIST_LENGTH   10



// adds the word to the eeprom and returns voice address
long add_word_eeprom(char *s)   {
   char cmd;
   long address_speech,retval;
   int address_eeprom;
   int i;

   address_eeprom = read_eeprom(0);       // get free eeprom space
   address_speech = ((long)read_eeprom(1)<<8)|read_eeprom(2);  //get free isd space

   if(address_eeprom>250)                 // if close to no memory left, error and restart
   {
      printf("Memory is full.  Erase eeprom please and retry.\r\n");
      reset_cpu();
   }
   printf("\r\nThe word \"");                 // display the word
   printf(s);
   printf("\" was not found in memory.  You will have to add it.\r\n");


   i=0;
   retval = address_speech;
   while(s[i]!=0)                         // add each character of the word to eeprom
      write_eeprom(address_eeprom++,s[i++]);
   write_eeprom(address_eeprom++,0);      // add the null char to eeprom
   write_eeprom(address_eeprom+1,address_speech);  // add the 16bit voice address
   write_eeprom(address_eeprom,address_speech>>8);

   do {
      printf("\r\nPush any key to begin recording, and then push a key to stop.\r\n");
      getc();
      printf("recording message at address %LX\r\n", address_speech);
      record_message(retval);             // record word at desired address

      getc();
      address_speech = stop_message()+1;  // stop recording and save address

      delay_ms(200);
      printf("Playing back the message...\r\n");
      play_message(retval);               // play back the recorded word

      delay_ms(500);
      printf("Does it sound ok? <Y/N>\r\n");
      cmd = getc();
   } while((cmd=='N')||(cmd=='n'));       // loop if needed

   write_eeprom(0,address_eeprom+2);      // save next free eeprom space
   write_eeprom(1,address_speech>>8);     // save next free isd mem space
   write_eeprom(2,address_speech);
   return(retval);
}


// gets the isd address where the word is stored
long get_address(char *s)  {
   int i,j;

   s=strlwr(s);
   for(i=3;i<255;i++)                     // go through all of eeprom
   {
      j=0;

      if(s[0]!=read_eeprom(i))            // if the first letters aren't the same
      {
         if(read_eeprom(i)==0)            // if the end of saved words, just exit
            return(0);
         while(read_eeprom(++i)!=0);      // skip over word and address space
         i+=2;
      }
      else
      {
         while(s[j]==read_eeprom(i+j))    // keep going as long as its a match
         {
            if(read_eeprom(i+j)==0)       // when total match, return address
               return(((long)read_eeprom(i+j+1)<<8)|(read_eeprom(i+j+2)));
            else
               j++;
         }
      }
   }
   return(0);                             // return 0 if no matches found
}


void get_address_list(char *s, long *l) { // parses the sentance and gets the address list
   int i,j,w_begin,count;
   char word[WORD_LENGTH];

   w_begin=0;
   count=0;
   for(i=0;i<SENTANCE_LENGTH;i++)         // go through all letters of the sentance
   {
      if((s[i]==' ')||(s[i]==0))          // go until the end of the word is found
      {
         if((i==0)||(i==w_begin))         // if end==begin of word, just increment begin ptr
            w_begin=i+1;
         else
         {
            for(j=0;j<(i-w_begin);j++)    // copy word to it's own string
               word[j] = s[w_begin+j];

            word[j]=0;                    // add null char
            w_begin=i+1;                  // reset begin ptr

            l[count]= get_address(word);  // get the address for the word

            if(l[count]==0)               // if the word is not in list...
               l[count]=add_word_eeprom(word);  // add the word to the dictionary

            count+=2;
         }
         if(s[i]==0)                      // quit when end of sentance
            break;

      }
   }
}



void main()   {
   char sentance[SENTANCE_LENGTH];
   long address_list[ADDRESS_LIST_LENGTH];
   int i;

   init_isd();

   printf("\r\nThis program converts text to speech.");
   printf("\r\nDo you want to erase the current dictionary? <Y/N>");
   i=getc();
   if((i=='Y')||(i=='y'))
   {
      printf("\r\nErasing now...\n");
      write_eeprom(0,0x03);   // reset the first free spot in eeprom
      write_eeprom(1,0x00);   // reset the first free spot in isd chip mem
      write_eeprom(2,0x01);   // 1 = msb, 2 = lsb
      for(i=3; i<255; i++)    // erase all of the rest
         write_eeprom(i,0);
   }

   while(TRUE)
   {
      for(i=0; i<ADDRESS_LIST_LENGTH; i++)
         address_list[i]=0;

      printf("\r\nEnter a sentance for me to speak.  Push ENTER when done.\r\n");
      printf("*NOTE*  Please do not use any punctuation.\r\n");
      get_string(sentance,SENTANCE_LENGTH);     // gets the string

      get_address_list(sentance,address_list);  // gets the address list

      delay_ms(500);

      printf("\r\nNow speaking sentance:\n");
      i=0;
      while(address_list[i]!=0)                 // plays the words together
      {
         play_message(address_list[i]);
         i++;
         delay_ms(600);
      }
   }
}
