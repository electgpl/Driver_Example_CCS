/////////////////////////////////////////////////////////////////////////
////                     fat_ex_shell.c                              ////
////                                                                 ////
////    This is a neat little shell utility that resembles a DOS or  ////
//// UNIX type shell in order to manipulate files on the FAT file    ////
//// system. This is mostly for demonstration purposes on how to use ////
//// some of the basic functionality of the FAT library.             ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 2007 Custom Computer Services              ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

//These settings are for the CCS PICEEC development kit which contains
//an MMC/SD connector.
#include <18F67J60.h>
#device PASS_STRINGS = IN_RAM
#fuses NOWDT, HS, NOPROTECT
#use delay(clock=25M)

#use rs232(baud=9600, UART1, errors)

#include <stdlib.h> // for atoi32

//media library, a compatable media library is required for FAT.
#use fast_io(c)
#define MMCSD_PIN_SCL     PIN_C3 //o
#define MMCSD_PIN_SDI     PIN_C4 //i
#define MMCSD_PIN_SDO     PIN_C5 //o
#define MMCSD_PIN_SELECT  PIN_C2 //o
#include <mmcsd.c>

//FAT library.
#include <fat.c>

//////////////////////
///                ///
/// Useful Defines ///
///                ///
//////////////////////

#define COMMAND_SIZE 10
#define NUM_COMMANDS 11

////////////////////////
///                  ///
/// Global Variables ///
///                  ///
////////////////////////

char g_CWD[200] = "/"; //current working directory

char commands[NUM_COMMANDS][COMMAND_SIZE]=
{
   "del",      //option1=filename.  delete file.
   "make",     //option1=filename.  create an empty file, give error if file already exists
   "append",   //option1=filename, option2=string.  append string to end of file
   "cd",    //option1=new cwd.  change working directory.  / is root.
   "dir",    //show files in directory
   "cat",    //option1=filename.  display full contents in ascii
   "tail",  //option1=filename.  display the last 20 lines of file.
   "mkdir", //option1=dir.  create directory.  see 'cd' for rules on dir
   "rmdir",  //option1=dir.  remove directory.  see 'cd' for rules on dir.
   "format",   // option1=media size in bytes. formats the media.
   "help"   // help!
};

////////////////////////////////
///                          ///
/// Function Implementations ///
///                          ///
////////////////////////////////

/*
Summary: Finds a command from the global list of commands.
Param: A pointer to the command string.
Returns: The command number if the command is found in the command list.
         0xFF if the command isn't found
*/
int FindCommand(char *cmd)
{
   char buf[COMMAND_SIZE];
   int i;
   
   for (i=0; i<NUM_COMMANDS; i++)
   {
      strcpy(buf, &commands[i][0]);
      if (stricmp(buf, cmd)==0)
         return(i);
   }
   
   return(0xFF);
}

/*
Summary: Displays the current working directory.
Param: None.
Returns: None.
*/
void DisplayPrompt(void)
{
   printf("\r\n\n%s> ", g_CWD);
}

/*
Summary: Deletes a file.
Param: The full path of the file to delete.
Returns: None.
*/
void DeleteFile(char *fileName)
{
   printf("\r\nDeleting '%s': ", fileName);
   if(rm_file(fileName) != GOODEC)
   {
      printf("Error deleting file");
      return;
   }
   printf("OK");
}

/*
Summary: Creates a file.
Param: The full path of the file to create.
Returns: None.
Example Usage: \> make "Log.txt"
*/
void MakeFile(char *fileName)
{
   printf("\r\nMaking file '%s': ", fileName);
   if(mk_file(fileName) != GOODEC)
   {
      printf("Error creating file");
      return;
   }
   printf("OK");
}

/*
Summary: Append a string to a file.
Param: The full path of the file to append to.
Param: A pointer to a string to append to the file.
Returns: None.
Example Usage: \> append "Log.txt" "This will be appended to the end of Log.txt"
Note: A "\r\n" will be appended after the appendString.
*/
void AppendFile(char *fileName, char *appendString)
{
   FILE stream;
   printf("\r\nAppending '%s' to '%s': ", appendString, fileName);
   if(fatopen(fileName, "a", &stream) != GOODEC)
   {
      printf("Error opening file");
      return;
   }
   
   fatputs(appendString, &stream);
   fatputs("\r\n", &stream);

   if(fatclose(&stream) != GOODEC)
   {
      printf("Error closing file");
      return;
   }
   printf("OK");
}

/*
Summary: Change the working directory.
Param: The new working directory to switch to.
Returns: None.
Example Usage: \> cd ftp/     -> /ftp/
               \ftp\> cd files/  -> /ftp/files/
               \ftp\files> cd..  -> /ftp/
               \ftp\> cd ..      -> /
               \> cd /ftp/files/ -> /ftp/files/
               
Note: Changing the directory to .. will go up a directory.
*/
void ChangeDirectory(char *newCWD)
{
   FILE stream;
   
   //append a / to the end of the filename if it doesn't exist
   //making an assumption here that newCWD can hold 1 more character
   if (newCWD[strlen(newCWD)-1] != '/')
     strcat(newCWD, "/");

   if((strstr(newCWD, "../") != 0) && (strcmp(g_CWD, "/") != 0))
   {
      g_CWD[strlen(g_CWD) - 1] = '\0';
            
      g_CWD[strrchr(g_CWD, '/') - g_CWD + 1] = '\0';      
   }
   else
   {
      if(fatopen(newCWD, "r", &stream) != GOODEC)
      {
         printf("\r\nError changing directory");
         return;
      }
      strcpy(g_CWD, newCWD);
   }
}

/*
Summary: Display the contents of the working directory.
Param: The full path of the directory contents to display.
Returns: None.
Example Usage: /> dir 
*/
void DisplayDirectory(char *dir)
{
   disp_folder_contents(dir);
}

/*
Summary: Create a directory.
Param: The full path of the directory to create.
Returns: None.
Example Usage: /> mkdir "Backlog"
*/
void MakeDirectory(char *dir)
{
   //append a / to the end of the filename if it doesn't exist
   //making an assumption here that newCWD can hold 1 more character
   if (dir[strlen(dir)-1] != '/')
     strcat(dir, "/");

   printf("\r\nMaking directory '%s': ", dir);

   if(mk_dir(dir) != GOODEC)
   {
      printf("Error creating directory");
      return;
   }
   printf("OK");
}

/*
Summary: Remove a directory.
Param: The full path of the directory to remove.
Returns: None.
Example Usage: /> rmdir "Backlog"
Note: The directory needs to be empty in order for this command to work.
*/
void RemoveDirectory(char *dir)
{
   printf("\r\nRemoving directory '%s': ", dir);

   //append a / to the end of the filename if it doesn't exist
   //making an assumption here that newCWD can hold 1 more character
   if (dir[strlen(dir)-1] != '/')
     strcat(dir, "/");

   if(rm_dir(dir) != GOODEC)
   {
      printf("Error removing directory");
      return;
   }
   printf("OK");
}

#define CAT_FROM_START  FALSE
#define CAT_FROM_END    TRUE
/*
Summary: Prints either all of or the last 80 characters in a file.
Param: The full path of the file to print off.
Param: If true, this function will print off the last 80 characters in the file.
       If false, this funciton will print off the entire file.
Returns: None.
Example Usage: /> cat "Logs.txt" (this will display the entire file)
Example Usage: /> tail "Logs.txt" (this will display the last 80 characters in the file)
*/
void PrintFile(char *fileName, int1 startFromEnd)
{
   FILE stream;

   if(fatopen(fileName, "r", &stream) != GOODEC)
   {
      printf("\r\nError opening file");
      return;
   }

   printf("\r\n");

   if(startFromEnd)
      fatseek(&stream, 80, SEEK_END);

   fatprintf(&stream);
   fatclose(&stream);
}

/*
Summary: Formats the media to a specified size.
Param: The size of the media, in kB, in string form.
Returns: None.
Example Usage: /> format 524288 (format a 512MB card)
*/
void FormatMedia(char *mediaSize)
{
   int32 size;
   
   size = atoi32(mediaSize);
   
   printf("\r\nFormatting media (size=%LU): ", size);
  
   if(format(size) != GOODEC)
   {
      printf("Error formatting media");
      return;
   }
   printf("OK");
}

/*
Summary: Shows a help prompt.
Param: None.
Returns: None.
Example Usage: /> help 
*/
void ShowHelp()
{
   printf("\r\nFAT Shell Help");
   printf("\r\n del filename --- Deletes the file");
   printf("\r\n make filename --- Creates an empty file");
   printf("\r\n append filename string --- Appends string to the end of the file");
   printf("\r\n cd dir --- Change the working directory");
   printf("\r\n dir --- Shows the contents of the directory");
   printf("\r\n cat filename --- Displays content of file");
   printf("\r\n tail filename --- Displays the last 80 characters of file");
   printf("\r\n mkdir dir --- Create a directory");
   printf("\r\n rmdir dir --- Deletes the directory");
   printf("\r\n format size --- Format card.  (Example: 'format 5524288' formats a 512MB card)");
   printf("\r\n help\tYou are here");
   printf("\r\n\n Put a parameter in quotes if it has spaces");
}

char * GetCMD(char *in)
{
   char tokens[]=" \r\n";
   return(strtok(in,tokens));
}

char * GetOption(char *in)
{
   char tokensSpace[]=" \r\n";
   char tokensQuote[]="\"\r\n";
   
   //trim leading spaces
   while (*in==' ')
      in++;
   
   //if first char is a quote, then end token on a quote.  ELSE end token on a space
   if (*in == '\"')
      return(strtok(in,tokensQuote));
   else
      return(strtok(in,tokensSpace));
}

void main(void)
{
   char buffer[255];
   char opt_buffer[255];
   char *cmd, *option1, *option2;
   
   int i;   // pointer to the buffer

   // initialize the FAT
   //  keep in mind that this will automagically initialize the media
   i = fat_init();
   if (i)
      printf("\r\n\nERROR INITIALIZING FAT\r\n\n");
    
   // main loop
   while(TRUE)
   {
      i = 0;
      
      DisplayPrompt();
      
      do
      {
         buffer[i] = getch();
         
         // check for a backspace
         if(buffer[i] != 8)
         {
            printf("%c", buffer[i]);
            i++;
         }
         else if(i > 0)
         {
            // delete the last character
            i--;
            putc(8);
            putc(' ');
            putc(8);
         }
         buffer[i] = '\0';
      } while(buffer[i - 1] != '\r');
      
      // parse the command and options
      cmd = GetCMD(buffer);
      option1 = GetOption(cmd + strlen(cmd) + 1);
      option2 = GetOption(option1 + strlen(option1) + 1);

      //if option1 starts with a '/', that means the file in the option includes
      //the full path to the file.  if the file doesn't start with a '/', the 
      //current working directory must be added.
      if (option1 && (option1[0]=='/'))
      {
         //option1 is a full path
         strcpy(opt_buffer, option1);
      }
      else if (option1)
      {
         // tack on the current working directory to option1
         strcpy(opt_buffer, g_CWD);
         strcat(opt_buffer, option1);
      }         
      
      if (cmd)
      {
         switch(FindCommand(cmd))
         {
            case 0:  //del
               DeleteFile(opt_buffer);
               break;
            
            case 1:  //make
               MakeFile(opt_buffer);
               break;
            
            case 2:  //append
               AppendFile(opt_buffer, option2);
               break;
            
            case 3:  //change directory
               ChangeDirectory(opt_buffer);
               break;
            
            case 4:  //show directory contents
               DisplayDirectory(g_CWD);
               break;
               
            case 5:  //cat, display file
               PrintFile(opt_buffer, CAT_FROM_START);
               break;
            
            case 6:  //tail, display last 80 charachters
               PrintFile(opt_buffer, CAT_FROM_END);
               break;
            
            case 7: //mkdir, make a directory
               MakeDirectory(opt_buffer);
               break;

            case 8: //rmdir, make a directory
               RemoveDirectory(opt_buffer);
               break;

            case 9: //format, format the card
               FormatMedia(option1);
               break;

            case 10: //help, display help
               ShowHelp();
               break;

            default:
               printf("\r\nUnkown Command '%s'", cmd);
               break;
         }
      }
   }
}

