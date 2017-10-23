/////////////////////////////////////////////////////////////////////////
////                          FAT_PIC.C                              ////
////                                                                 ////
//// Driver/Library for a FAT filesystem with a PIC                  ////
////                                                                 ////
//// This Library was designed to resemble standard ANSI C I/O as    ////
////  much as possible. There are, however, some caveats to this.    ////
////  Please read the comments to make sure the inputs and outputs   ////
////  to each function are understood before using anything in       ////
////  this library.                                                  ////
////                                                                 ////
//// This library supports FAT16 and FAT32, but not both at the same ////
//// time (this is a compile option, see options below).  It is      ////
//// recommended to use FAT32, FAT32 also has been tested more.      ////
////                                                                 ////
//// Any function with an argument taking in a file name must be in  ////
////  the form of...                                                 ////
////  "/filename.fil" for a file in the root directory               ////
////  "/Directory/filename.fil" for a file in a subdirectory of root ////
////  "/Directory/Subdirectory/filename.fil" and so on...            ////
////                                                                 ////
//// Any function with an argument taking in a directory name must   ////
////  be in the form of...                                           ////
////  "/Dirname/" for a directory in the root directory              ////
////  "/Dirname/Subdirname/" for a directory in a subdirectory of    ////
////  root and so on...                                              ////
////                                                                 ////
//// A compatable media library must be provided.  This is           ////
//// documented after the User Functions.                            ////
////                                                                 ////
//// -- User Functions --                                            ////
////                                                                 ////
//// fat_init()                                                      ////
////  Initializes the FAT library, also initializes the media.       ////
////                                                                 ////
//// fatopen(char *name, char *mode, FILE *fstream)                  ////
////  Opens up a FILE stream to a specified file with the specified  //// 
////  permission mode:                                               ////
////             Permissions: "r" = read                             ////
////                          "w" = write                            ////
////                          "a" = append                           ////
////                          "rb" = read binarily                   ////
////             "w" will erase all of the data in the file upon     ////
////              the opening of the file.                           ////
////             "a" will tack on all of the data to the end of the  ////
////              file.                                              ////
////             "r" will keep on reading until the stream           ////
////              hits an '\0'                                       ////
////             "rb" will keep on reading until the amount of       ////
////              bytes read equals the size of the file.            ////
////                                                                 ////
////  Unlike standard C fopen(), this does not malloc a FILE -       ////
////  instead the caller will have to have allready allocated a      ////
////  a FILE and pass a pointer to it.                               ////
////                                                                 ////
//// fatreopen(char *name, char *mode, FILE *fstream)                ////
////  Closes a FILE stream, then reopens the stream with a new file  ////
////  and new permissions.                                           ////
////                                                                 ////
//// fatclose(FILE *fstream)                                         ////
////  Closes a FILE stream. It is very important to call this        //// 
////  function when you're done reading or writing to a file.        ////                             ////
////                                                                 ////
//// fatgetc(FILE *fstream)                                          ////
////  Gets a character from a stream. An EOF will be returned at     //// 
////  different times depending on whether or not the stream is      //// 
////  reading binarily.  If not reading binarily: EOF when the       //// 
////  stream reads a '\0'.  If reading binarily: EOF when the amount //// 
////  of bytes read equals the size of the file (end of file).       ////
////                                                                 ////
//// fatputc(char c, FILE *fstream)                                  ////
////  Puts a character into a stream (write to the file).            ////
////  Writes are buffered, so the media may not be written to until  ////
////  a fatclose().                                                  ////
////                                                                 ////
//// char* fatgets(char* str, int num, FILE *fstream)                //// 
////  Gets characters from a stream until either a '\r',  EOF, or    //// 
////  num - 1 is hit.                                                ////
////                                                                 ////
//// fatputs(char* str, FILE *fstream)                               ////
////  Puts a string into a stream (write a string to the file).      ////
////                                                                 ////
//// fatprintf(FILE *stream): Printfs the entire stream.             ////
////  printf()'s the entire stream (printf()'s the contents of the
////  file).
////                                                                 ////
//// fatgetpos(FILE *fstream, fatpos_t *pos)                         ////
////  Gets the current position of the stream/file, saves to pos.    ////
////                                                                 ////
//// fatsetpos(FILE *fstream, fatpos_t *pos)                          ////
////  Sets the current position of the stream/file.                  ////
////                                                                 ////
//// fatseek(FILE *fstream, int32 offset, int origin)                 ////
////  Sets the current position of the stream according to the       ////
////  origin parameter:                                              ////
////             SEEK_CUR: Set position relative to the              ////
////              current stream position.                           ////
////             SEEK_END: Set position relative to the              ////
////              end of the stream.                                 ////
////             SEEK_SET: Set position relative to the              ////
////              beginning of the stream.                           ////
////                                                                 ////
//// fateof(FILE *fstream)                                           ////
////  Returns non-zero if the stream/file position is at EOF,        //// 
////  non-zero if there are still data left in the stream.           ////
////                                                                 ////
//// faterror(FILE *fstream):                                        ////
////  Returns non-zero if there have been errors with the stream,    ////
////  zero if the stream has been operating correctly since it has   ////
////  been opened.                                                   ////
////                                                                 ////
//// fatread(void* buffer, int size, int32 num, FILE* fstream)       ////
////  Reads size*num chars from the stream, saves to buffer.         ////
////                                                                 ////
//// fatwrite(void* buffer, int size, int32 num, FILE* fstream)      //// 
////  Writes size*num chars from buffer to the stream.               ////
////                                                                 ////
//// fatflush(FILE *fstream)                                         ////
////  Flushes the buffer in a stream.                                ////
////                                                                 ////
//// clearerr(FILE *fstream)                                         ////
////  Clears any error flags in the stream.                          ////
////                                                                 ////
//// rewind(FILE *fstream)                                           ////
////  Send the stream back to the beginning of the file.             ////
////                                                                 ////
//// fatpos_t fattell(FILE *fstream)                                 ////
////  Returns the current position of the stream.                    ////
////                                                                 ////
//// rm_file(char *fname)                                            ////
////  Removes a file.                                                ////
////                                                                 ////
//// rm_dir(char *dirname)                                           ////
////  Removes a directory.                                           ////
////                                                                 ////
//// mk_file(char *fname)                                            ////
////  Makes a file, file will be blank.                              ////
////                                                                 ////
//// mk_dir(char *dirname)                                           ////
////  Makes a directory.                                             ////
////                                                                 ////
//// format(int32 mediaSize)                                         ////
////  Formats the media into a FAT32 or FAT16 file system.           ////
////  If you specify a mediaSize larger than the actual media bad    ////
////  things will happen.  If you specify a mediaSize smaller than   ////
////  the actual media size will simply limit the filesystem from    ////
////  using 0 to mediaSize-1.  Anything after mediaSize can be used  ////
////  by the application (perhaps as a general purpose EEPROM?)      ////
////  NOTE: Windows thinks the filesystem is RAW.                    ////
////  NOTE: This may be a little buggy.                              ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// This library was written to use CCS's MMC/SD library as the     ////
//// media source.  If you want to use a different media source,     ////
//// you must provide the following 4 functions:                     ////
////                                                                 ////
//// int8 mmcsd_init(void);                                          ////
////  Initializes the media.  This will be called by fat_init().     ////
////                                                                 ////
//// int8 mmcsd_read_bytes(int32 a, int16 s, char *p);               ////
////  Read s bytes from p to the media starting at address a.        ////
////                                                                 ////
//// int8 mmcsd_write_data(int32 a, int16 s, char *p);               ////
////  Write s bytes from p to the media starting at address a.       ////
////  To maximize throughput on some medias, it's a good idea to     ////
////  buffer writes in this function.                                ////
////                                                                 ////
//// int8 mmcsd_flush_buffer(void);                                  ////
////  If your write function is buffering writes, this will flush    ////
////  the buffer and write it to the media.                          ////
////                                                                 ////
//// All four functions should return 0 if OK, non-zero if error.    ////
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

// NOTE This library has no concept of what time and date it currently is.
//       All files and folders created or modified using this library
//       will have invalid/inaccurate timestamps and datestamps.

// NOTE To save on ROM and RAM space, the user of this library will have to 
//       define what type of FAT they will be working with. The defines are 
//       in the Useful Defines section below.

// NOTE For faster writing or appending for an application such as a logger, 
//       uncomment #FAST_FAT below.  This will make the FAT library assume 
//       there is one file on the card to write or append to, thereby
//       making writing and appending much faster. Reading is impossible in 
//       this mode.
//       THIS IS NOT TESTED VERY WELL YET!

// NOTE The current maximum file name length (full path) is 32 characters 
//       long. If longer file names are desired, change the 
//       MAX_FILE_NAME_LENGTH define below. Creating a file whose full path 
//       is longer than MAX_FILE_NAME_LENGTH may lead to weird operation. Keep
//       in mind that making this define larger will make your RAM usage go 
//        up.

#ifndef FAT_PIC_C
#define FAT_PIC_C

#include <ctype.h>
#include <string.h>
#case

//////////////////////
///                ///
/// Useful Defines ///
///                ///
//////////////////////

/// Define your FAT type here ///
//#define FAT16
#define FAT32

/// For faster single-file writing, uncomment this line below ///
//#define FAST_FAT

/// Everything else ///
#define MAX_FILE_NAME_LENGTH 0x20  // the maximum length of a file name for our FAT, including /0 terminator
#define STREAM_BUF_SIZE 0x20       // how big the FILE buffer is. 0x20 is optimal

//////////////////////////////////////////////////////////////////

#define EOF -1
#define GOODEC 0
#define fatpos_t int32
#define SEEK_CUR 0
#define SEEK_END 1
#define SEEK_SET 2

////////////////////////
///                  ///
/// Global Variables ///
///                  ///
////////////////////////

int16
   Bytes_Per_Cluster,   // number of addressable bytes per cluster
   FAT_Start;           // when the first FAT begins

int32
   Data_Start,          // when data starts
   FAT_Length,          // the length of one FAT
   Next_Free_Clust,     // where the next free cluster is
   Root_Dir;            // when the root directory starts

enum filetype
{
   Data_File,  // the stream is pointing to a binary, data file
   Directory,  // the stream is pointing to a directory
   None        // the stream isn't currently pointing to anything
};

enum ioflags
{
   Closed = 0x00,
   Read = 0x01,
   Write = 0x02,
   Append = 0x04,
   Binary = 0x08,
   EOF_Reached = 0x10,
   Read_Error = 0x20,
   Write_Error = 0x40,
   File_Not_Found = 0x80
};

struct iobuf
{
   fatpos_t
      Bytes_Until_EOF,     // how many bytes until the stream's end of file
      Cur_Char,            // the current byte that the stream is pointing at
      Entry_Addr,          // the entry address of the file that is associated with the stream
      Parent_Start_Addr,   // the parent's start adddress of the file that is associated with the stream
      Size,                // the size of the file that is associated with the stream
      Start_Addr;          // the beginning of the data in the file that is associated with the stream

   enum filetype File_Type;   // the type of file that is associated with the stream

   enum ioflags Flags;        // any associated input/output flag

   int Buf[STREAM_BUF_SIZE];  // this is a buffer so that during fatputc() or fatgetc()
                              //  the media won't have to be read at every character
};
typedef struct iobuf FILE;

///////////////////////////
///                     ///
/// Function Prototypes ///
///                     ///
///////////////////////////

/// Standard C Functions ///
signed int fatopen(char fname[], char mode[], FILE* stream);
signed int fatreopen(char fname[], char mode[], FILE* stream);
signed int fatclose(FILE* stream);
signed int fatgetc(FILE* stream);
signed int fatputc(int ch, FILE* stream);
char* fatgets(char* str, int num, FILE* stream);
signed int fatputs(char* str, FILE* stream);
signed int fatprintf(FILE* stream);
signed int fatgetpos(FILE* stream, fatpos_t* position);
signed int fatsetpos(FILE* stream, fatpos_t* position);
signed int fatseek(FILE* stream, int32 offset, int origin);
signed int fateof(FILE* stream);
signed int faterror(FILE* stream);
signed int fatread(void* buffer, int size, int32 num, FILE* stream);
signed int fatwrite(void* buffer, int size, int32 count, FILE* stream );
signed int fatflush(FILE* stream);
signed int remove(char* fname);
void clearerr(FILE* stream);
void rewind(FILE* stream);
fatpos_t fattell(FILE* stream);

/// Non-Standard C Functions ///
signed int rm_file(char fname[]);
signed int rm_dir(char dname[]);
signed int mk_file(char fname[]);
signed int mk_dir(char dname[]);

/// Functions' Utility Functions ///
signed int set_file(char fname[], int attrib, FILE* stream);
signed int get_file_name(int32 file_entry_addr, char name[]);
signed int set_file_name(int32 parent_dir_addr, int32* entry_addr, char name[]);
signed int get_short_file_name(int32 file_entry_addr, char sname[], int type);
signed int make_short_file_name(int32 parent_dir_addr, char fname[], char sname[]);
int long_name_chksum (int* pFcbName);
signed int check_invalid_char(char fname[]);
#ifdef FAT32
signed int get_next_free_cluster(int32* my_cluster);
signed int dealloc_clusters(int32 start_cluster);
signed int alloc_clusters(int32 start_cluster, int32* new_cluster_addr);
signed int clear_cluster(int32 cluster);
signed int write_fat(int32 cluster, int32 data);
#else // FAT16
signed int get_next_free_cluster(int16* my_cluster);
signed int dealloc_clusters(int16 start_cluster);
signed int alloc_clusters(int16 start_cluster, int32* new_cluster_addr);
signed int clear_cluster(int16 cluster);
signed int write_fat(int16 cluster, int16 data);
#endif // #ifdef FAT32
signed int get_next_file(FILE* stream);
signed int get_prev_file(FILE* stream);
signed int get_next_free_addr(int32* my_addr);
signed int get_next_free_entry(int32* start_addr);
signed int get_next_entry(int32* start_addr);
signed int get_prev_entry(int32* start_addr);
signed int read_buffer(FILE* stream, int* val);
signed int write_buffer(FILE* stream, int val);
void fill_entry(char the_entry[], char val, int8 start_ind);
void disp_timestamp(int16 timestamp);
void disp_datestamp(int16 datestamp);

/// Data Utility Functions ///
signed int fat_init();
#ifdef FAT32
signed int get_next_cluster(int32* my_cluster);
signed int get_prev_cluster(int32* my_cluster);
int32 cluster_to_addr(int32 cluster);
int32 addr_to_cluster(int32 addr);
#else // FAT16
signed int get_next_cluster(int16* my_cluster);
signed int get_prev_cluster(int16* my_cluster);
int32 cluster_to_addr(int16 cluster);
int16 addr_to_cluster(int32 addr);
#endif // #ifdef FAT32
signed int get_next_addr(int32* my_addr);
signed int get_prev_addr(int32* my_addr);
signed int format(int32 DskSize);

/// Debugging Utility Functions ///
signed int disp_folder_contents(char foldername[]);
signed int dump_addr(int32 from, int32 to);
signed int dump_clusters(int32 from, int32 to);
void disp_fat_stats();
signed int fatprintfinfo(FILE* stream);

////////////////////////////////
///                          ///
/// Function Implementations ///
///                          ///
////////////////////////////////

/// Standard C Functions ///

/*
signed int fatopen(char fname[], char mode[], FILE* stream)
Summary: This will open up a file stream for reading, writing, or appending.
Param fname: The full path of the file to open.
Param mode: The mode to open up the stream into.
             "r" = Read
             "w" = Write
             "a" = Append
             "rb", "wb", "ab" = Read, Write, or Append in Binary mode
Param stream: The stream to open up.
Returns: EOF if there was a problem, GOODEC if everything went okay.
Note: fname must be in the form of /filename.fil for a file in the root directory
       /Directory/filename.fil for a file in a subdirectory of root
       /Directory/Subdirectory/filename.fil and so on...
Note: Standard C will make a file in case a file isn't found,
       however due to recursion this is not possible in CCSC.
*/
signed int fatopen(char fname[], char mode[], FILE* stream)
{
   int fname_parse_pos = 1;    // the current index of the fname character

   char target_file[MAX_FILE_NAME_LENGTH];   // temporary buffer to hold names of files

   FILE cur_stream;     // this will   be the stream that will be returned if all goes well

#ifndef FAST_FAT
   int
      depth = 0,              // how many subdirectories deep the file is
      target_file_parse_pos;  // the current index of the target_file character
#endif // #ifndef FAST_FAT

   // set flags
#ifdef FAST_FAT
   switch(mode[0])
   {
      case 'w':
         cur_stream.Flags = Write;
         break;
      case 'a':
         cur_stream.Flags = Append;
         break;
      default:
         return EOF;
   }

   // start looking for the file, start at root
   cur_stream.Start_Addr = cur_stream.Parent_Start_Addr = Root_Dir;

   while(fname[fname_parse_pos] != '\0')
   {
      target_file[fname_parse_pos - 1] = fname[fname_parse_pos];
      fname_parse_pos += 1;
   }

   target_file[fname_parse_pos] = '\0';

   // find the file inside of its subdirectory
   if(set_file(target_file, 0x20, &cur_stream) != GOODEC)
   {
      cur_stream.Flags |= File_Not_Found;
      *stream = cur_stream;
      return EOF;
   }

   // at this point, we've found the file
   *stream = cur_stream;
   return GOODEC;
#else // NO FAST_FAT
   switch(mode[0])
   {
      case 'r':
         cur_stream.Flags = Read;
         break;
      case 'w':
         cur_stream.Flags = Write;
         break;
      case 'a':
         cur_stream.Flags = Append;
         break;
      default:
         return EOF;
   }

   if(mode[1] == 'b')
      cur_stream.Flags |= Binary;

   // start looking for the file, start at root
   cur_stream.Start_Addr = cur_stream.Parent_Start_Addr = Root_Dir;

   // figure out how deep we have to go, count how many '/' we have in the string
   while(fname[fname_parse_pos] != '\0')
   {
      if(fname[fname_parse_pos] == '/')
         depth++;
      fname_parse_pos += 1;
   }

   // start the fname index at 1 to skip over the '/'
   fname_parse_pos = 1;

   // open up to the subdirectory, if possible
   while(depth > 0)
   {
      // find the name of our next target directory
      target_file_parse_pos = 0;
      while(fname[fname_parse_pos] != '/')
      {
         // check to make sure that we're not at the end of a poorly formatted string
         if(fname[fname_parse_pos] == '\0')
            return EOF;

         // fill up the buffer and increment the indexes
         target_file[target_file_parse_pos] = fname[fname_parse_pos];
         fname_parse_pos += 1;
         target_file_parse_pos += 1;
      }

      // increment the fname index one more because it's currently pointing at the '/'
      fname_parse_pos += 1;

      // tack on a \0 to the end of the target file to terminate the string
      target_file[target_file_parse_pos] = '\0';

      // check to see if the directory exists and open it if possible, otherwise exit because the directory doesn't exist
      if(set_file(target_file, 0x10, &cur_stream) != GOODEC)
      {
         cur_stream.Flags |= File_Not_Found;
         *stream = cur_stream;
         return EOF;
      }
      depth -= 1;
   }

   // check to see if we're trying to open just a directory
   if(fname[fname_parse_pos] == '\0')
   {
      *stream = cur_stream;
      return GOODEC;
   }

   // now that we have the location of the subdirectory that the file is in, attempt to open the file
   target_file_parse_pos = 0;
   while(fname[fname_parse_pos] != '\0')
   {
      // fill up the buffer and increment the indexes
      target_file[target_file_parse_pos] = fname[fname_parse_pos];
      fname_parse_pos += 1;
      target_file_parse_pos += 1;
   }

   // tack on a \0 to the end of the target file to terminate the string
   target_file[target_file_parse_pos] = '\0';

   // find the file inside of its subdirectory
   if(set_file(target_file, 0x20, &cur_stream) != GOODEC)
   {
      cur_stream.Flags |= File_Not_Found;
      *stream = cur_stream;
      return EOF;
   }

   // at this point, we've found the file
   *stream = cur_stream;
   return GOODEC;
#endif // #ifdef FAST_FAT
}

/*
signed int fatreopen(char fname[], char mode[], FILE* old_stream, FILE* new_stream)
Summary: This will close a stream and then reopen it using new parameters.
Param fname: The full path of the file to open.
Param mode: The mode to open up the stream into.
             "r" = Read
             "w" = Write
             "a" = Append
             "rb", "wb", "ab" = Read, Write, or Append in Binary mode
Param stream: The stream to close and reopen.
Returns: EOF if there was a problem, GOODEC if everything went okay.
Note: fname must be in the form of /filename.fil for a file in the root directory
         /Directory/filename.fil for a file in a subdirectory of root
         /Directory/Subdirectory/filename.fil and so on...
Note: Standard C will make a file in case a file isn't found,
       however due to recursion this is not possible in CCSC.
*/
signed int fatreopen(char fname[], char mode[], FILE* stream)
{
   // close the old stream
   if(fatclose(stream) == EOF)
     return EOF;

   // open the new stream
   if(fatopen(fname, mode, stream) == EOF)
      return EOF;

   return GOODEC;
}

/*
signed int fatclose(FILE* stream)
Summary: Closes a stream and commits any changes done to the file.
Param: The stream to close.
Returns: EOF if there was a problem, 0 if everything went okay.
*/
signed int fatclose(FILE* stream)
{
   int ec = 0;

   int32 first_cluster;

   // commit data back to the stream's entry, if needed
   if((stream->Flags & Write) || (stream->Flags & Append))
   { 
      // write the new size of the file
      if(mmcsd_write_data(stream->Entry_Addr + 0x1C, 4, &(stream->Size)) != GOODEC)
      {
         stream->Flags |= Write_Error;
         return EOF;
      }

      // check to see if the first cluster is already linked in the file
      ec += mmcsd_read_data(stream->Entry_Addr + 0x14, 2, (int16*)&first_cluster + 1);
      ec += mmcsd_read_data(stream->Entry_Addr + 0x1A, 2, &first_cluster);

      if(ec != GOODEC)
      {
         stream->Flags |= Read_Error;
         return EOF;
      }

      // write the first cluster to the entry if needed
      if(first_cluster == 0)
      {
         // convert the start address to a cluster number
         first_cluster = addr_to_cluster(stream->Start_Addr);

         ec += mmcsd_write_data(stream->Entry_Addr + 0x14, 2, (int16*)&first_cluster + 1);
         ec += mmcsd_write_data(stream->Entry_Addr + 0x1A, 2, &first_cluster);

         if(ec != GOODEC)
         {
            stream->Flags |= Write_Error;
            return EOF;
         }
      }
      
      // dump the remaining buffer to the card
      if(fatflush(stream) == EOF)
         return EOF;
   }
   // nullify the data
   stream->Cur_Char = 0;
   stream->Entry_Addr = 0;
   stream->Size = 0;
   stream->Start_Addr = 0;
   stream->Flags = 0;
   return 0;
}

/*
signed int fatgetc(FILE* stream)
Summary: Gets a character from a stream.
Param: The stream to get a character from.
Returns: The character that was gotten from the stream,
          EOF if the stream has reached the end of the file or doesn't have permissions to read,
*/
signed int fatgetc(FILE* stream)
{
   char ch; // character read in

   // check to see if the stream has proper permissions to read
   if(stream->Flags & Read)
   {
      // when the number of bytes until eof hit zero, we know we are at the end of any file
      if(stream->Bytes_Until_EOF == 0)
      {
         stream->Flags |= EOF_Reached;
         return EOF;
      }

      // read in the next byte in the buffer
      if(read_buffer(stream, &ch) == EOF)
         return EOF;

      // a 0x00 will signify the end of a non-binary file
      if((ch == '\0') && !(stream->Flags & Binary))
      {
         stream->Flags |= EOF_Reached;
         return EOF;
      }

      // get the next contiguous address of the stream
      if(get_next_addr(&(stream->Cur_Char)) != GOODEC)
         return EOF;
         
      // we just got 1 byte closer to the end of the file
      stream->Bytes_Until_EOF -= 1;
      return ch;
   }

   // if the stream doesn't have proper permissions to read, return an EOF
   else
      return EOF;
}

/*
signed int fatputc(int ch, FILE* stream)
Summary: Puts a character into a stream.
Param ch: The character to put into the stream.
Param stream: The stream to put a character into.
Returns: The character that was put into the stream,
          EOF if the stream doesn't have permissions to write, or if a problem happened.
*/
signed int fatputc(int ch, FILE* stream)
{
   // check to see if the stream has proper permissions to write
   if(((stream->Flags & Write) || (stream->Flags & Append)) && (stream->File_Type == Data_File))
   {
      // if there isn't any space allocated yet, allocate some
      if(stream->Cur_Char < Data_Start)
      {
         if(get_next_free_cluster(&Next_Free_Clust) == EOF)
            return EOF;
#ifdef FAT32
         if(write_fat(Next_Free_Clust, 0x0FFFFFFF) == EOF)
            return EOF;
#else // FAT16
         if(write_fat(Next_Free_Clust, 0xFFFF) == EOF)
            return EOF;
#endif // #ifdef FAT32
         if(clear_cluster(Next_Free_Clust) == EOF)
            return EOF;
         stream->Cur_Char = stream->Start_Addr = cluster_to_addr(Next_Free_Clust);
      }

      // write the next character to the buffer
      if(write_buffer(stream, ch) == EOF)
         return EOF;

      // get the next address, increment Cur_Char
      if(get_next_addr(&(stream->Cur_Char)) == EOF)
      {
         // write the current buffer to the end of the current cluster
         if(mmcsd_write_data(stream->Cur_Char - STREAM_BUF_SIZE + 1, STREAM_BUF_SIZE, stream->Buf) != GOODEC)
         {
            stream->Flags |= Write_Error;
            return EOF;
         }
         // start looking for a new cluster to allocate
         if(alloc_clusters(addr_to_cluster(stream->Cur_Char), &(stream->Cur_Char)) == EOF)
            return EOF;
      }

      // our file just got bigger by 1 byte
      stream->Size += 1;

      return ch;
   }

   // if the stream doesn't have proper permissions to write, return an EOF
   else
      return EOF;
}

/*
char* fatgets(char* str, int num, FILE* stream)
Summary: Reads characters from a stream into a string.
Param str: A pointer to the beginning of the string to put characters into.
Param num: The number of characters to put into the string - 1.
Param stream: The stream to read from.
Returns: A pointer to the most recently added character, or NULL if there was an error.
Note: If a newline is read from the stream, then str will be terminated with a newline.
       If num - 1 or EOF is reached, then str will be null terminated.
*/
char* fatgets(char* str, int num, FILE* stream)
{
   int i;   // counter for loops

   // loop until num - 1
   for(i = 0; i < num - 1; i += 1)
   {
      str[i] = fatgetc(stream);
      if(str[i] == '\n')
         return str;
      if(str[i] == EOF)
         break;
   }

   // close off str with a null terminator
   str[i] = '\0';

   return str;
}

/*
signed int fatputs(char* str, FILE* stream)
Summary: Writes characters from a string into a stream.
Param str: A pointer to the beginning of the string to write into the stream.
Param stream: The stream to write into.
Returns: EOF if there was a problem, GOODEC if everything went okay.
*/
signed int fatputs(char* str, FILE* stream)
{
   int i = 0;   // counter for loops

   // fatputc every character in the stream
   while(str[i] != '\0')
   {
      if(fatputc(str[i], stream) == EOF)
        return EOF;
      i += 1;
   }

   return GOODEC;
}

/*
signed int fatprintf(FILE* stream)
Summary: This will print off the entire contents of the stream to the console.
Param: The stream to print off.
Returns: The last character printed off to the console.
*/
signed int fatprintf(FILE* stream)
{
   signed int ch; // character read in

   // keep on printf any characters read in as long as we don't run into an end of file or a media error
   do
   {
      ch = fatgetc(stream);
      printf("%c", ch);
   } while(ch != EOF);

   return ch;
}

/*
signed int fatgetpos(FILE* stream, fatpos_t* position)
Summary: Returns the current position of where the stream is pointing to relative to the beginning of the stream.
Param stream: The stream to get the position of.
Param position: A pointer to a variable put the current position of the pointer into.
Returns: 0 on success.
*/
signed int fatgetpos(FILE* stream, fatpos_t* position)
{
   *position = stream->Size - stream->Bytes_Until_EOF;
   return 0;
}

/*
signed int fatsetpos(FILE* stream, fatpos_t* position)
Summary: Sets the current position of where the stream is pointing to in memory relative to the beginning of the stream.
Param stream: The stream to set the position of.
Param position: A pointer the a variable that has the value of the new position.
Returns: 0 on success, or EOF if there was error.
*/
signed int fatsetpos(FILE* stream, fatpos_t* position)
{
#ifndef FAST_FAT
#ifdef FAT32
   int32 cur_cluster; // the current cluster we're pointing to
#else // FAT16
   int16 cur_cluster; // the current cluster we're pointing to
#endif // #ifdef FAT32
   int32 i;    // pointer to memory
#endif // #ifndef FAST_FAT

   // check to see if we want to just rewind the file
   if(*position == 0)
   {
      rewind(stream);
      return GOODEC;
   }
   
   // this whole process is much different and easier if we're writing or appending at a spot after EOF
   //  this will essentially write null characters to the file from EOF to the desired position
   if(((stream->Flags & Write) || (stream->Flags & Append)) && (stream->Size < *position))
   {
      while(stream->Size < *position)
         if(fatputc('\0', stream) == EOF)
            return EOF;
      
      return 0;
   }

#ifdef FAST_FAT
   stream->Cur_Char = stream->Start_Addr + *position;
#else // NO FAST_FAT
   // figure out how many clusters into the file the position is to be set to
   i = *position / Bytes_Per_Cluster;
   cur_cluster = addr_to_cluster(stream->Start_Addr);

   // head to that cluster
   while(i > 0)
   {
      if(get_next_cluster(&cur_cluster) != GOODEC)
         return EOF;
      i -= 1;
   }

   // head to the correct cluster
   stream->Cur_Char = cluster_to_addr(cur_cluster);

   // now that we're in the correct cluster, tack on the remaining position
   stream->Cur_Char += (*position % Bytes_Per_Cluster);

   if(stream->Flags & Read)
   {
      // we now need to change how far it is until EOF
      stream->Bytes_Until_EOF = stream->Size - *position;

      // fill up the buffer
      if(mmcsd_read_data(stream->Cur_Char, STREAM_BUF_SIZE, stream->Buf) != GOODEC)
      {
         stream->Flags |= Read_Error;
         return EOF;
      }
   }

   else
      stream->Size = *position;
#endif // #ifdef FAST_FAT
   return 0;
}

/*
signed int fatseek(FILE* stream, int32 offset, int origin)
Summary: This will set the position of the file stream according to the input. The EOF flag will also be cleared.
Param stream: The stream to set the position of.
Param offset: How many bytes relative of origin the file stream position will be set.
Param origin: This will be one of 3 values...
               SEEK_CUR: Set position relative to the current stream position.
               SEEK_END: Set position relative to the end of the stream.
               SEEK_SET: Set position relative to the beginning of the stream.
Returns: 0 on success, or EOF if there was error.
*/
signed int fatseek(FILE* stream, int32 offset, int origin)
{
   int32 myoffset;   // since fatsetpos requires a pointer to a variable, we need this here

   switch(origin)
   {
      case SEEK_CUR:
         myoffset = stream->Cur_Char + offset;
         if(fatsetpos(stream, &myoffset) != 0)
            return EOF;
         break;
      case SEEK_END:
         myoffset = stream->Size - offset;
         if(fatsetpos(stream, &myoffset) != 0)
            return EOF;
         break;
      case SEEK_SET:
         myoffset = offset;
         if(fatsetpos(stream, &myoffset) != 0)
            return EOF;
         break;
      default:
         return EOF;
   }

   // clear the EOF flag
   stream->Flags &= 0xEF;

   return GOODEC;
}

/*
signed int fateof(FILE* stream)
Summary: Determines whether or not the stream is at the end of the file.
Param: The stream to query for EOF.
Returns: A non-zero value if the file is at EOF,
          0 if the file is not at EOF.
*/
signed int fateof(FILE* stream)
{
   return stream->Flags & EOF_Reached;
}

/*
signed int fatread(void* buffer, int size, int32 num, FILE* stream)
Summary: Fills up an array with data from a stream.
Param buffer: A pointer to the beginning of an array of any type.
Param size: How many bytes each element in the array is.
Param num: How many elements to fill in the array.
Param stream: The stream to read from.
Returns: How many values were written to the array.
*/
signed int fatread(void* buffer, int size, int32 num, FILE* stream)
{
   int32 i; // counter for loop

   // fill up every byte
   for(i = 0; i < (num * size); i += 1)
      buffer[i] = fatgetc(stream);

   return i;
}

/*
signed int fatwrite(void* buffer, int size, int32 count, FILE* stream )
Summary: Fills up a stream with data from an array
Param buffer: A pointer to the beginning of an array of any type.
Param size: How many bytes each element in the array is.
Param num: How many elements to write to the stream.
Param stream: The stream to write to.
Returns: How many values were written to the stream.
*/
signed int fatwrite(void* buffer, int size, int32 count, FILE* stream )
{
   int32 i; // counter for loop

   // write every byte
   for(i = 0; i < (count * (int32)size); i += 1)
      if(fatputc(buffer[i], stream) == EOF)
         return EOF;

   return i;
}

/*
signed int fatflush(FILE* stream)
Summary: Flushes the buffer of a given stream.
Param: The stream to flush the buffer of.
Returns: EOF if there was a problem, 0 if everything went okay
*/
signed int fatflush(FILE* stream)
{
   // check to see if we have a buffer
   if((stream->Flags & Write) || (stream->Flags & Append))
   {
      // check to see if we need to flush the buffer
      if(stream->Cur_Char % STREAM_BUF_SIZE == 0)
      {
         // flush the buffer to the card
         if(mmcsd_write_data(stream->Cur_Char - STREAM_BUF_SIZE, STREAM_BUF_SIZE, stream->Buf) != GOODEC)
         {
            stream->Flags |= Write_Error;
            return EOF;
         }
      }
      else
      {
         // flush the buffer to the card
         //  we need to make sure that the buffer gets flushed into the proper location, hence all this weird % math
         if(mmcsd_write_data(stream->Cur_Char - (stream->Cur_Char % STREAM_BUF_SIZE), STREAM_BUF_SIZE, stream->Buf) != GOODEC)
         {
            stream->Flags |= Write_Error;
            return EOF;
         }
      }
      return(mmcsd_flush_buffer());
   }
   return 0;
}

/*
signed int remove(char* fname)
Summary: Removes a file from disk.
Param: The full path of the file to remove.
Returns: 0 on success, or EOF if there was error.
Note: This function does not work for removing directories, use rm_dir instead.
Note: fname must be in the form of /filename.fil for a file in the root directory
       /Directory/filename.fil for a file in a subdirectory of root
       /Directory/Subdirectory/filename.fil and so on...
*/
signed int remove(char* fname)
{
   if(rm_file(fname) == EOF)
       return EOF;

   return 0;
}

/*
signed int faterror(FILE* stream)
Summary: Checks for an error in a given stream.
Param: The stream to check for an error in.
Returns: A non-zero value of there is an error in the stream,
          0 if there is no error in the stream
*/
signed int faterror(FILE* stream)
{
   return stream->Flags & 0xF0;
}

/*
void clearerr(FILE* stream)
Summary: Clears off all error in a given stream.
Param: The stream to clear off the errors in.
Returns: Nothing.
*/
void clearerr(FILE* stream)
{
   stream->Flags &= 0x0F;
}

/*
void rewind(FILE* stream)
Summary: Sets the stream to point back to the beginning of a file.
Param: The stream to rewind.
Returns: Nothing.
*/
void rewind(FILE* stream)
{
   // set the stream back to the beginning
   stream->Cur_Char = stream->Start_Addr;
   stream->Bytes_Until_EOF = stream->Size;
}

/*
fatpos_t fattell(FILE* stream)
Summary: Returns the current position of where the stream is pointing to relative to the beginning of the stream.
Param: The stream to return the position of.
Returns: The position of where the stream is pointing to relative to the beginning of the stream, or 0 if there was a problem.
*/
fatpos_t fattell(FILE* stream)
{
   fatpos_t retval;

   if(fatgetpos(stream, &retval) != 0)
      return 0;

   return retval;
}

/// Non-Standard C Functions ///

/*
signed int rm_file(char fname[])
Summary: Deletes a file.
Param: The full path of the file to delete.
Returns: GOODEC if everything went okay, EOF if there was a problem.
Note: fname must be in the form of /filename.fil for a file in the root directory
       /Directory/filename.fil for a file in a subdirectory of root
       /Directory/Subdirectory/filename.fil and so on...
*/
signed int rm_file(char fname[])
{
   int
      order,
      ulinked_entry = 0xE5; // 0xE5 is put into the file's entry to indicate unlinking

   int32 i;

   char mode[] = "r";        // r is the safest mode to remove files with

   FILE stream;              // the stream that we'll be working with

   // attempt to open the stream
   if(fatopen(fname, mode, &stream) == EOF)
      return EOF;

   // we need to un-link the file's clusters from the FAT if there are clusters allocated
   if(stream.Start_Addr > Root_Dir)
   {
      if(dealloc_clusters(addr_to_cluster(stream.Start_Addr)) == EOF)
         return EOF;
   }
   // get rid of the first entry
   i = stream.Entry_Addr;
   if(mmcsd_write_data(i, 1, &ulinked_entry) == EOF)
      return EOF;
   
   // check to see if there is a long file name
   get_prev_entry(&i);
   if(mmcsd_read_data(i + 11, 1, &order) == EOF)
      return EOF;

   // get rid of all of the long file name entries if they're there
   while(order == 0x0F)
   {
      if(mmcsd_write_data(i, 1, &ulinked_entry) == EOF)
         return EOF;

      if(get_prev_entry(&i) == EOF)
         return EOF;

      if(mmcsd_read_data(i + 11, 1, &order) == EOF)
         return EOF;
   }
   
   if(fatclose(&stream) == EOF)
      return EOF;

   return GOODEC;
}

/*
signed int rm_dir(char dname[])
Summary: Deletes a directory.
Param: The full path of the directory to delete.
Returns: GOODEC if everything went okay, EOF if there was a problem.
Note: dname must be in the form of /Dirname/ for a directory in the root directory
       /Dirname/Subdirname/ for a directory in a subdirectory of root and so on...
Note: This function cannot remove all of the files
       and subdirectories of the directory. Manually remove all subdirectories
       and files before calling this command.
*/
signed int rm_dir(char dname[])
{
   char mode[] = "r";        // r is the safest mode to remove files with

   FILE stream;              // the stream that we'll be working with

   // attempt to open the stream
   if(fatopen(dname, mode, &stream) == EOF)
      return EOF;

   // jump over the . and .. entries in the directory
   stream.Entry_Addr = stream.Start_Addr + 64;
   
   // check to make sure that there isn't stuff in this directory
   if(get_next_file(&stream) != EOF)
      return EOF;

   // since removing files is a lot like removing directories, we
   //  can just call rm_file
   if(rm_file(dname) == EOF)
      return EOF;

   return GOODEC;
}

/*
signed int mk_file(char fname[])
Summary: Creates a file.
Param: The full path of the file to create.
Returns: GOODEC if everything went okay, EOF if there was a problem.
Note: This function will not create directories if parent directories don't exist.
Note: fname must be in the form of /filename.fil for a file in the root directory
       /Directory/filename.fil for a file in a subdirectory of root
       /Directory/Subdirectory/filename.fil and so on...
*/
signed int mk_file(char fname[])
{
   char
      filename[MAX_FILE_NAME_LENGTH],   // the file name we're trying to make
      mode[] = "r";                     // reading is the safest mode to work in

   int
      buf,               // buffer to hold values
      entire_entry[0x20],// entire first entry
      filename_pos = 0,  // the current parse position of the file name we're trying to create
      fname_pos;         // the current parse position of the input the the function

   int32 i;      // pointer to memory

   FILE stream;   // the stream that we'll be working with

   // attempt to open up to the directory
   if(fatopen(fname, mode, &stream) == GOODEC)
      return EOF; // we shouldn't get an GOODEC back from fatopen()

   // check to see if the file is already there.
   if(!(stream.Flags & File_Not_Found))
      return EOF;

   // make a file name
   fname_pos = strrchr(fname, '/') - fname + 1;
   while((fname[fname_pos] != '\0') && (filename_pos < MAX_FILE_NAME_LENGTH))
   {
      filename[filename_pos] = fname[fname_pos];
      fname_pos += 1;
      filename_pos += 1;
   }
   filename[filename_pos] = '\0';

   // write the name
   if(set_file_name(stream.Start_Addr, &i, filename) == EOF)
      return EOF;

   // throw in some values in the file's first entry
   for(buf = 0; buf < 0x20; buf += 1)
      entire_entry[buf] = 0;

   // this is a file
   entire_entry[0x0B] = 0x20;

   // read what set_file_name gave us for the short name
   if(mmcsd_read_data(i, 11, entire_entry) != GOODEC)
      return EOF;

   // write the entry
   if(mmcsd_write_data(i, 0x20, entire_entry) != GOODEC)
      return EOF;

   return GOODEC;
}

/*
signed int mk_dir(char dname[])
Summary: Creates a directory.
Param: The full path of the directory to create.
Returns: GOODEC if everything went okay, EOF if there was a problem.
Note: This function will not create directories if parent directories don't exist.
Note: dname must be in the form of /Dirname/ for a directory in the root directory
       /Dirname/Subdirname/ for a directory in a subdirectory of root and so on...
*/
signed int mk_dir(char dname[])
{
   char
      dirname[MAX_FILE_NAME_LENGTH],    // the directory name we're trying to make
      entire_entry[0x20],               // used to hold the link entries (. and ..) to the directory and its parent
      mode[] = "r";                     // reading is the safest mode to work in

   int
      dirname_pos = 0,   // the current parse position of the directory name we're trying to create
      dname_pos,         // the current parse position of the input the the function
      j;                 // counter for loops

   int32 i;   // pointer to memory

   FILE stream;   // the stream that we'll be working with

   // attempt to open up to the directory
   if(fatopen(dname, mode, &stream) == GOODEC)
      return EOF; // we shouldn't get an GOODEC back from fatopen()

   // check to see if the directory is already there.
   if(!(stream.Flags & File_Not_Found))
      return EOF;

   // make the directory name
   dname_pos = strrchr(dname, '/') - dname;
   dname[dname_pos] = '\0';
   dname_pos = strrchr(dname, '/') - dname + 1;
   while((dname[dname_pos] != '\0') && (dirname_pos < MAX_FILE_NAME_LENGTH))
   {
      dirname[dirname_pos] = dname[dname_pos];
      dname_pos += 1;
      dirname_pos += 1;
   }
   dirname[dirname_pos] = '\0';
   dname[dname_pos] = '/';

   // write the file's name
   if(set_file_name(stream.Start_Addr, &i, dirname) == EOF)
      return EOF;

   // find and allocate an open cluster
   if(get_next_free_cluster(&Next_Free_Clust) == EOF)
      return EOF;
   if(clear_cluster(Next_Free_Clust) == EOF)
      return EOF;
#ifdef FAT32
   if(write_fat(Next_Free_Clust, 0x0FFFFFFF) == EOF)
      return EOF;
#else // FAT16
   if(write_fat(Next_Free_Cluster, 0xFFFF) == EOF)
      return EOF;
#endif // #ifdef FAT32

   // throw in some values in the file's first entry
   for(j = 0; j < 0x20; j += 1)
      entire_entry[j] = 0;

   // this is a directory
   entire_entry[0x0B] = 0x10;

   entire_entry[0x1A] = make8(Next_Free_Clust, 0);
   entire_entry[0x1B] = make8(Next_Free_Clust, 1);
#ifdef FAT32
   entire_entry[0x14] = make8(Next_Free_Clust, 2);
   entire_entry[0x15] = make8(Next_Free_Clust, 3);
#endif // #ifdef FAT32

   if(mmcsd_read_data(i, 11, entire_entry) != GOODEC)
      return EOF;

   // write the file's first entry
   if(mmcsd_write_data(i, 0x20, entire_entry) != GOODEC)
      return EOF;

   // make the two links that point to the directory and the directory's parent
   i = cluster_to_addr(Next_Free_Clust);

   // put in the first link that points to the directory itself
   for(j = 0; j < 0x20; j += 1)
   {
      if(j < 0x01)
         entire_entry[j] = '.';
      else if(j < 0x0B)
         entire_entry[j] = 0x20;
      else if(j == 0x0B)
         entire_entry[j] = 0x10;
      else
         entire_entry[j] = 0x00;
   }

   entire_entry[0x1A] = make8(Next_Free_Clust, 0);
   entire_entry[0x1B] = make8(Next_Free_Clust, 1);
#ifdef FAT32
   entire_entry[0x14] = make8(Next_Free_Clust, 2);
   entire_entry[0x15] = make8(Next_Free_Clust, 3);
#endif // #ifdef FAT32

   if(mmcsd_write_data(i, 0x20, entire_entry) != GOODEC)
      return EOF;

   for(j = 0; j < 0x0C; j += 1)
   {
      if(j < 0x02)
         entire_entry[j] = '.';
      else if(j < 0x0B)
         entire_entry[j] = 0x20;
      else
         entire_entry[j] = 0x10;
   }

   if(stream.Parent_Start_Addr == Root_Dir)
   {
      entire_entry[0x14] = 0x00;
      entire_entry[0x15] = 0x00;
      entire_entry[0x1A] = 0x00;
      entire_entry[0x1B] = 0x00;
   }
   else
   {
      entire_entry[0x1A] = make8(addr_to_cluster(stream.Parent_Start_Addr), 0);
      entire_entry[0x1B] = make8(addr_to_cluster(stream.Parent_Start_Addr), 1);
#ifdef FAT32
      entire_entry[0x14] = make8(addr_to_cluster(stream.Parent_Start_Addr), 2);
      entire_entry[0x15] = make8(addr_to_cluster(stream.Parent_Start_Addr), 3);
#endif // #ifdef FAT32
   }

   if(mmcsd_write_data(i + 0x20, 0x20, entire_entry) != GOODEC)
      return EOF;

   return GOODEC;
}

/// Functions' Utility Functions ///
/// NOTE: A library user should not need to use any of the functions in this section ///

/*
signed int set_file(char fname[], int attrib, FILE* stream)
Summary: This will set the stream to point to the specified file.
Param fname: The file name to search for.
Param attrib: The file attributes to search for. 0x10 is a directory, 0x20 is a file.
Param stream: The stream to set.
Returns: EOF if there was a problem, GOODEC if everything went okay.
Note: The stream has to be pointing to the parent directory's start address when coming in to this function.
*/
signed int set_file(char fname[], int attrib, FILE* stream)
{
   int
      cur_attrib,    // the attribute of the most recently read entry
      cur_state,     // the state of the most recently read entry
      ec = 0;        // error checking byte

   int32 i;   // pointer to memory
#ifndef FAST_FAT
   char name_buffer[MAX_FILE_NAME_LENGTH];   // buffer to hold in the most recently read in name
#endif // #ifndef FAST_FAT

   // set the memory pointer to the parent start address
   i = stream->Start_Addr;

   // search for the name of our target file inside of the parent directory
   do
   {
      // read the state and the attribute of the current entry
      ec += mmcsd_read_data(i, 1, &cur_state);
      ec += mmcsd_read_data(i + 0x0B, 1, &cur_attrib);
      if(ec != GOODEC)
      {
         stream->Flags |= Read_Error;
         return EOF;
      }

       // check to make sure that this entry exists and the entry is the type we're looking for
      if((cur_state != 0xE5) && (cur_attrib == attrib))
      {
#ifndef FAST_FAT
         // get the long file name of the current entry
         if(get_file_name(i, name_buffer) == EOF)
            return EOF;

         // if the target entry and the long file name are equal, strcmp will return a zero
         if(strcmp(fname, name_buffer) == 0)
#endif // #ifndef FAST_FAT
         {
            // we have found our target entry, set the current entry and break out of this function
            // set stream's parent address
            stream->Parent_Start_Addr = stream->Start_Addr;

            ec += mmcsd_read_data(i + 0x1C, 4, &(stream->Size));

            // stream->Start_Addr is going to temporarily have a cluster number
            ec += mmcsd_read_data(i + 0x14, 2, (int16*)&stream->Start_Addr + 1);
            ec += mmcsd_read_data(i + 0x1A, 2, &stream->Start_Addr);

            if(ec != GOODEC)
            {
               stream->Flags |= Read_Error;
               return EOF;
            }

            // convert stream->Start_Addr to an address
            stream->Start_Addr = cluster_to_addr(stream->Start_Addr);

            stream->Entry_Addr = i;
            stream->Bytes_Until_EOF = stream->Size;

            // set up some permission-specific parameters if we're at a file
            if(attrib == 0x20)
            {
               stream->File_Type = Data_File;
               if(stream->Flags & Write)
               {
                  // delete all previous data in the file
                  stream->Bytes_Until_EOF = stream->Size = 0;

                  // if there is already space allocated, get rid of it
                  if(stream->Start_Addr >= Data_Start)
                     if(dealloc_clusters(addr_to_cluster(stream->Start_Addr)) == EOF)
                        return EOF;
                  stream->Cur_Char = 0;
               }

               if((stream->Flags & Append) && (stream->Size != 0))
               {
                  // set the position to the end of the file and fill the buffer with the contents of the end of the file
                  ec = fatsetpos(stream, &(stream->Size));
                  if(stream->Cur_Char % STREAM_BUF_SIZE == 0)
                     ec += mmcsd_read_data(stream->Cur_Char - STREAM_BUF_SIZE, STREAM_BUF_SIZE, stream->Buf);
                  else
                     ec += mmcsd_read_data(stream->Cur_Char - (stream->Cur_Char % STREAM_BUF_SIZE), STREAM_BUF_SIZE, stream->Buf);
               }
#ifndef FAST_FAT
               if(stream->Flags & Read)
               {
                  stream->Cur_Char = stream->Start_Addr;

                  // fill up the read buffer for reading
                  ec = mmcsd_read_data(stream->Cur_Char, STREAM_BUF_SIZE, stream->Buf);
               }
#endif // #ifndef FAST_FAT
               if(ec != GOODEC)
               {
                  stream->Flags |= Read_Error;
                  return EOF;
               }               
            }
            else
               stream->File_Type = Directory;
            return GOODEC;
         }
      }

      // check to make sure that the next iteration will give us a contiguous cluster
      if(get_next_entry(&i) == EOF)
         return EOF;

   } while(cur_state != 0x00);

   // if we reach this point, we know that the file won't be found
   stream->Flags |= File_Not_Found;
   return EOF;
}

/*
signed int get_file_name(int32 file_entry_addr, char name[])
Summary: This will get a name of a file.
Param file_entry_addr: The position in memory that the file's entry is.
Param name[]: The place to put the name of the file into.
Returns: EOF if there was a problem, GOODEC if everything went okay.
*/
signed int get_file_name(int32 file_entry_addr, char name[])
{
   int
      j,       // counter for loops
      k = 0,   // current character in array
      order,   // byte to hold the current long file name order
      type;    // the type of entry that was just read in

   int32 i;          // pointer for memory

   // the long file name isn't fragmented across clusters
   i = file_entry_addr;

   // check to make sure that this file has a long file name
   if(mmcsd_read_data(i - 0x15, 1, &type) != GOODEC)
      return EOF;

   if(type != 0x0F)
   {
      // this file doesn't have a long file name
      if(get_short_file_name(i, name, type) == EOF)
         return EOF;
      return GOODEC;
   }

   do
   {
      // head to the previous entry
      if(get_prev_entry(&i) == EOF)
         return EOF;

      for(j = 1; j < 0x20; j += 2, k += 1)
      {
         if(j == 11)
            j = 14;
         else if(j == 26)
            j = 28;
         if(mmcsd_read_data(j + i, 1, &(name[k])) != GOODEC)
            return EOF;
      }

      // now that that's done with, get the entry's order
      if(mmcsd_read_data(i, 1, &order) != GOODEC)
         return EOF;

   } while(!(order & 0x40));  // the last entry will be 0b01xxxxxx

   // end the name[] buffer with a \0
   name[k] = '\0';

   return GOODEC;
}

/*
signed int set_file_name(int32 parent_dir_addr, int32 entry_addr, char name[])
Summary: Creates both a short and long file name at the first free entry in a directory.
Param parent_dir_addr: The address of the parent directory to create the short file name in.
Param entry_addr: The address the function put the short file name entry.
Param name: The full file name.
Returns: EOF if there was a problem, GOODEC if everything went okay.
*/
signed int set_file_name(int32 parent_dir_addr, int32* entry_addr, char name[])
{
   char sname[12];   // place to hold the short file name

   signed int name_pos = 0;   // the current parse position of name[]

   int
      chksum,               // the long file name checksum
      entire_entry[0x20],   // the entire entry to put write onto the media
      entry_pos,            // the current position inside of entire_entry
      long_entry = 1;       // the current long entry number we're on

   int32 i;   // pointer to memory

   // check for invalid characters
   if(check_invalid_char(name) == EOF)
      return EOF;

   // make a short file name of this
   if(make_short_file_name(parent_dir_addr, name, sname) == EOF)
      return EOF;

   // get a checksum for the long file name entries
   chksum = long_name_chksum(sname);

   // start writing the long file name
   // zero out entry[]
   for(entry_pos = 0; entry_pos < 0x20; entry_pos += 1)
      entire_entry[entry_pos] = 0;   

   i = parent_dir_addr;
   if(get_next_free_entry(&i) == EOF)
      return EOF;

   // 0x0F signifies an file entry
   entire_entry[11] = 0x0F;

   // since we're working in reverse, write the final long name entry
   name_pos = strlen(name);
   name_pos %= 13;

   if(name_pos != 0)
   {
      // add 1 to account for the \0 terminator
      name_pos += 1;

      fill_entry(entire_entry, 0xFF, name_pos);
   }

   // start writing the long file name entries
   name_pos = strlen(name);

   // figure out how many entries this name will take up
   long_entry = (name_pos / 13) + 1;

   if(name_pos % 13 == 0)
      long_entry -= 1;

   // set the bit to signify this is the final entry
   long_entry |= 0x40;

   while(name_pos >= 0)
   {
      entry_pos = name_pos % 13;

      if(entry_pos < 5)
         entire_entry[(entry_pos << 1) + 1] = name[name_pos];

      else if(entry_pos < 11)
         entire_entry[(entry_pos << 1) + 4] = name[name_pos];

      else
         entire_entry[(entry_pos << 1) + 6] = name[name_pos];

      if((entry_pos == 0)
         && (name_pos != strlen (name)))
      {
         entire_entry[0] = long_entry;

         // clear off the bits at positions 6 and 7 if the most recent entry was the final one.
         if(name_pos != 0)
            long_entry &= 0x3F;

         long_entry -= 1;

         entire_entry[13] = chksum;
         if(mmcsd_write_data(i, 0x20, entire_entry) != GOODEC)
            return EOF;
         if(get_next_free_entry(&i) == EOF)
            return EOF;
         fill_entry(entire_entry, 0x00, 0);
      }
      name_pos -= 1;
   }

   // write the short file name to the entry
   if(mmcsd_write_data(i, 11, sname) != GOODEC)
      return EOF;

   // set the new entry addr
   *entry_addr = i;

   return GOODEC;
}

/*
signed int get_short_file_name(int32 file_entry_addr, char sname[], int type)
Summary: Reads a file's short file name, puts all the characters into lower case, and puts it into a buffer.
Param file_entry_addr: Where the file's entry address is located.
Param sname: The buffer to put the short file name.
Returns: EOF if there was a problem, GOODEC if everything went okay.
*/
signed int get_short_file_name(int32 file_entry_addr, char sname[], int type)
{
   int
      buf,
      i,
      j = 0;

   // one short file name has, at the most, 11 characters
   for(i = 0; i < 11; i += 1)
   {      
      // read in a character
      if(mmcsd_read_data(i + file_entry_addr, 1, &buf) != GOODEC)
         return EOF;
      
      // convert the character
      if(buf != ' ')
      {         
         sname[j] = tolower(buf);
         j += 1;
      }
      
   }   

   // tack on a null terminator
   sname[j] = '\0';
   
   //printf("\r\n%s, %u\r\n",sname, strlen(sname));

   if(type != 0x10 && strlen(sname) > 3)
   {
      j = strlen(sname);
      for(i=j; i > j-3; --i)
         sname[i] = sname[i-1];
      sname[i] = '.';
   }
   
   sname[j+1] = '\0';

   return GOODEC;
}

/*
signed int make_short_file_name(int32 parent_dir_addr, char fname[], char sname[])
Summary: Creates a unique short file name in a directory.
Param parent_dir_addr: The address of the parent directory to create the short file name in.
Param fname: The full file name.
Param sname: Character array that will hold the short file name upon function completion.
Returns: EOF if there was a problem, GOODEC if everything went okay.
*/
signed int make_short_file_name(int32 parent_dir_addr, char fname[], char sname[])
{
   char
      val[12] = "           ",
      cur_fname[12] = "           ",
      cur_fnum[7] = "      ";

   int
      buf,
      ext_pos,
      fname_parse_pos = 0,
      val_parse_pos = 0;

   int32
      fnum = 0,
      i;

   // figure out where the extension position is
   ext_pos = strchr(fname, '.');

   // check to see if this file has an extension
   if(ext_pos == 0)
   {
      while((val_parse_pos < 8) && (fname[fname_parse_pos] != '\0'))
      {
         val[val_parse_pos] = toupper(fname[fname_parse_pos]);
         val_parse_pos += 1;
         fname_parse_pos += 1;

         // we can't have a '.' or ' ' in the short name
         while((fname[fname_parse_pos] == '.') || (fname[fname_parse_pos] == ' '))
            fname_parse_pos += 1;
      }
   }
   else
   {
      ext_pos -= fname - 1;
      while((val_parse_pos < 11) && (fname[fname_parse_pos] != '\0'))
      {
         val[val_parse_pos] = toupper(fname[fname_parse_pos]);
         val_parse_pos += 1;
         fname_parse_pos += 1;

         // we can't have a '.' or ' ' in the short name
         while((fname[fname_parse_pos] == '.') || (fname[fname_parse_pos] == ' '))
            fname_parse_pos += 1;

         // check to see if it's time to skip val_parse_pos ahead to the file extension
         if(fname_parse_pos == ext_pos)
            val_parse_pos = 8;

         // check to see if it's time to skip name_parse_pos ahead to the file extension
         else if(val_parse_pos == 8)
            fname_parse_pos = ext_pos;
      }
   }

   // now that we've got the short file name, we need to make it unique
   i = parent_dir_addr;
   if(mmcsd_read_data(i + 0x0B, 1, &buf) != GOODEC)
      return EOF;

   // keep reading until we hit empty space
   while(buf != 0x00)
   {
      // check to see if this is a short file name entry
      if((buf == 0x20) || (buf == 0x10))
      {
         // read in the short file name that we're currently pointing at
         if(mmcsd_read_data(i, 11, cur_fname) != GOODEC)
            return EOF;

         cur_fname[11] = '\0';

         // strcmp will return a 0 if the file name we're currently pointing at and the file name that we created above are the same
         if(strcmp(cur_fname, val) == 0)
         {
            // we now need to create a unique file name
            //  increment the unique file number by one
            fnum += 1;

            // convert the unique file number to a string
            sprintf(cur_fnum, "%lu", fnum);

            // put the unique file number, along with a '~' into our short file name
            fname_parse_pos = 0;

            // find out the last posiiton of a space
            val_parse_pos = strchr(val, ' ');
            if(val_parse_pos == 0)
               // if there isn't a space, then we're going to have to put the ~x at the end of the short name
               val_parse_pos = 7 - strlen(cur_fnum);
            else
               // if there is a space, then put the ~x there
               val_parse_pos -= val + 2;

            // make some room for extra digits
            buf = 10;
            while(fnum >= buf)
            {
               val_parse_pos -= 1;
               buf *= 10;
            }

            // write in the ~
            val[val_parse_pos] = '~';

            // write in the number
            val_parse_pos += 1;
            while(cur_fnum[fname_parse_pos] != '\0')
            {
               val[val_parse_pos] = cur_fnum[fname_parse_pos];
               val_parse_pos += 1;
               fname_parse_pos += 1;
            }

            // start the search over again to see if that unique file name/number combination is still taken up
            i = parent_dir_addr;
         }
      }

      // head to the next entry
      if(get_next_entry(&i) == EOF)
      {
         // we're going to have to allocate another cluster
         if(alloc_clusters(addr_to_cluster(i), &i) == EOF)
            return EOF;         
      }
      if(mmcsd_read_data(i + 0x0B, 1, &buf) != GOODEC)
         return EOF;
   }

   // copy the short name into the input buffer
   for(i = 0; i < 12; i += 1)
      sname[i] = val[i];

   return GOODEC;
}

/*
int long_name_chksum (int* FcbName)
Summary: Returns an unsigned byte checksum computed on an unsigned byte
          array. The array must be 11 bytes long and is assumed to contain
          a name stored in the format of a MS-DOS directory entry.
Param: Pointer to an unsigned byte array assumed to be 11 bytes long.
Returns: Sum An 8-bit unsigned checksum of the array pointed to by pFcbName.
*/
int long_name_chksum (int* pFcbName)
{
   int
      FcbNameLen,
      Sum = 0;

   for(FcbNameLen = 11; FcbNameLen != 0; FcbNameLen -= 1)
      // The operation is an unsigned char rotate right
      Sum = ((Sum & 1) ? 0x80 : 0) + (Sum >> 1) + *pFcbName++;

   return Sum;
}

/*
signed int check_invalid_char(char fname[])
Summary: Checks the filename for any invalid characters.
Param: The name of the file to check.
Returns: EOF if an invalid character was found, GOODEC otherwise.
*/
signed int check_invalid_char(char fname[])
{
   int fname_pos;

   for(fname_pos = 0; (fname[fname_pos] != '\0') && (fname_pos < MAX_FILE_NAME_LENGTH); fname_pos += 1)
      if(isamoung(fname[fname_pos], "\\/:*?\"<>|"))
         return EOF;

   return GOODEC;
}

/*
signed int get_next_free_cluster(int16* my_cluster)
Summary: Will go through the FAT and find the first unallocated cluster.
Param: Pointer to a variable that will the the starting cluster of the serach.
        When a free cluster is found, the cluster number will be put into this variable.
Returns: EOF if there was a problem, GOODEC if everything went okay.
Note: This gets a little slow when dealing with a card with lots of stuff on it; sorry about that.
*/
#ifdef FAT32
signed int get_next_free_cluster(int32* my_cluster)
#else
signed int get_next_free_cluster(int16* my_cluster)
#endif
{
#ifdef FAST_FAT
   *my_cluster += 1;
   return GOODEC;
#else // NO FAST_FAT

#ifdef FAT32
   int val[4];            // buffer to hold values

   int32 cur_cluster;

   int32
      FAT_addr,           // the current address that the algorithm is on
      j;

   // first, convert *my_cluster to an addressable location in the FAT
   FAT_addr = (*my_cluster << 2) + FAT_Start;

   // the most logical place for the next free cluster would be next to the current cluster
   for(j = 0; j < FAT_Length; j += 4)
   {
      if(mmcsd_read_data(FAT_addr + j, 4, val) != GOODEC)
         return EOF;

      cur_cluster = make32(val[3], val[2], val[1], val[0]);

      if(cur_cluster == 0)
      {
         // add on the last iteration of j, this is how far into the buffer we were when the algorithm terminated
         FAT_addr += j;

         // convert *my_cluster back into a cluster number
         *my_cluster = (FAT_addr - FAT_Start) >> 2;
         return GOODEC;
      }
   }
#else // FAT16
   int val[2];

   int16 cur_cluster;

   int32
      FAT_addr,           // the current address that the algorithm is on
      j;

   // first, convert *my_cluster to an addressable location in the FAT
   FAT_addr = (*my_cluster << 1) + FAT_Start;

   // the most logical place for the next free cluster would be next to the current cluster
   for(j = 0; j < FAT_Length; j += 2)
   {
      if(mmcsd_read_data(FAT_addr + j, 2, val) != GOODEC)
         return EOF;

      cur_cluster = make16(val[1], val[0]);

      if(cur_cluster == 0)
      {
         // add on the last iteration of j, this is how far into the buffer we were when the algorithm terminated
         FAT_addr += j;

         // convert *my_cluster back into a cluster number
         *my_cluster = (FAT_addr - FAT_Start) >> 1;
         return GOODEC;
      }
   }

#endif // #ifdef FAT32
   // if we reach this point, we are out of disk space
   return EOF;
#endif // #ifdef FAST_FAT
}

/*
signed int get_next_file(FILE* stream)
Summary: Will point the stream to the next file in the directory.
Param: The stream to move.
Returns: EOF if there was a problem, GOODEC if everything went okay.
Note: This will not set the Buf or Flag parameters.
*/
signed int get_next_file(FILE* stream)
{
   int32
      cluster,
      cur_addr,
      size;
    
   int
      fileentry,
      filetype;
   
   cur_addr = stream->Entry_Addr;
    
   do
   {     
      // go forward an entry
      if(get_next_entry(&cur_addr) == EOF)
      {
         stream->File_Type = None;
         return EOF;
      }
      
      mmcsd_read_data(cur_addr, 1, &fileentry);
      mmcsd_read_data(cur_addr + 0x0B, 1, &filetype);

      if(fileentry == 0)
      {
         stream->File_Type = None;
         return EOF;
      }
 
   } while((fileentry == 0xE5) || (filetype == 0x0F));
   
   // change the stream's file type
   if(filetype == 0x10)
      stream->File_Type = Directory;
   else
      stream->File_Type = Data_File;

   // change the stream's entry address
   stream->Entry_Addr = cur_addr;
   
   // read in the new starting cluster
   mmcsd_read_data(cur_addr + 0x1A, 2, &cluster);
   mmcsd_read_data(cur_addr + 0x14, 2, (int16*)&cluster + 1);
   
   // change the stream's start adress and cur char to the beginning of the first cluster
   stream->Start_Addr = stream->Cur_Char =  cluster_to_addr(cluster);
   
   // read in the new file size
   mmcsd_read_data(cur_addr + 0x1C, 4, &size);
   
   // change the stream's size and bytes until EOF 
   stream->Size = stream->Bytes_Until_EOF = size;

   return GOODEC;
}

/*
signed int get_next_file(FILE* stream)
Summary: Will point the stream to the previous file in the directory.
Param: The stream to move.
Returns: EOF if there was a problem, GOODEC if everything went okay.
Note: This will not set the Buf or Flag parameters.
*/
signed int get_prev_file(FILE* stream)
{
   int32
      cluster,
      cur_addr,
      size;
    
   int
      fileentry = 0xE5,
      filetype = 0;
   
   cur_addr = stream->Entry_Addr;
    
   do
   {     
      // go backward an entry
      if(get_prev_entry(&cur_addr) == EOF)
      {
         stream->File_Type = None;
         return EOF;
      }
      
      mmcsd_read_data(cur_addr, 1, &fileentry);
      mmcsd_read_data(cur_addr + 0x0B, 1, &filetype);

      if(fileentry == 0)
      {
         stream->File_Type = None;
         return EOF;
      }
 
   } while((fileentry == 0xE5) || (filetype == 0x0F));

   // change the stream's file type
   if(filetype == 0x10)
      stream->File_Type = Directory;
   else
      stream->File_Type = Data_File;

   // change the stream's entry address
   stream->Entry_Addr = cur_addr;
   
   // read in the new starting cluster
   mmcsd_read_data(cur_addr + 0x1A, 2, &cluster);
   mmcsd_read_data(cur_addr + 0x14, 2, (int16*)&cluster + 1);
   
   // change the stream's start adress and cur char to the beginning of the first cluster
   stream->Start_Addr = stream->Cur_Char =  addr_to_cluster(cluster);

   // read in the new file size
   mmcsd_read_data(cur_addr + 0x1C, 4, &size);
   
   // change the stream's size and bytes until EOF 
   stream->Size = stream->Bytes_Until_EOF = size;

   return GOODEC;
}

/*
signed int get_next_free_addr(int32* my_addr)
Summary: Finds the next unallocated address.
Param: Pointer to a variable that will the the starting address of the serach.
        When a free address is found, the address will be put into this variable.
Returns: EOF if there was a problem, GOODEC if everything went okay.
*/
signed int get_next_free_addr(int32* my_addr)
{
   int val;   // buffer to hold values

   int32 cur_addr;   // pointer to memory

   // make a copy of *my_addr
   cur_addr = *my_addr;

   // keep on getting addresses until we hit a free one
   do
   {
      if(mmcsd_read_data(cur_addr, 1, &val) != GOODEC)
         return EOF;

      if(get_next_addr(&cur_addr) == EOF)
         return EOF;
   } while(val != 0);

   *my_addr = cur_addr;

   return GOODEC;
}

/*
signed int get_next_entry(int32* start_addr)
Summary: Gets the next entry in a directory.
Param: The address to start looking for an entry.
   If an entry is found, it will be put into this variable.
Returns: EOF if there was a problem with the media, GOODEC if everything went okay.
*/
signed int get_next_entry(int32* start_addr)
{
   int32 i;
   
   i = *start_addr;
   
   i += 0x1F;
   
   if(get_next_addr(&i) == EOF)
      return EOF;
   
   *start_addr = i;
   
   return GOODEC;
}

/*
signed int get_prev_entry(int32* start_addr)
Summary: Finds the next free entry in a directory.
Param: The address to start looking for a free entry.
   If a free entry is found, it will be put into this variable.
Returns: EOF if there was a problem with the media, GOODEC if everything went okay.
*/
signed int get_prev_entry(int32* start_addr)
{
   int32 i;
   
   i = *start_addr;

   i -= 0x1F;

   if(get_prev_addr(&i) == EOF)
      return EOF;

   *start_addr = i;
   
   return GOODEC;
}

/*
signed int get_next_free_entry(int32* start_addr)
Summary: Finds the next free entry in a directory.
Param: The address to start looking for a free entry.
   If a free entry is found, it will be put into this variable.
Returns: EOF if there was a problem with the media, GOODEC if everything went okay.
*/
signed int get_next_free_entry(int32* start_addr)
{
   int buf;

   int32 i;

   i = *start_addr;

   if(mmcsd_read_data(i, 1, &buf) != GOODEC)
     return EOF;

   while(buf != 0)
   {
      i += 0x1F;
      // get the next address
      if(get_next_addr(&i) == EOF)
         if(alloc_clusters(addr_to_cluster(i), &i) == EOF)
            return EOF;

      if(mmcsd_read_data(i, 1, &buf) != GOODEC)
        return EOF;
   }

   *start_addr = i;

   return GOODEC;
}

/*
signed int alloc_clusters(int16 start_cluster, int32* new_cluster_addr)
Summary: Find, allocate, and link a free cluster.
Param start_cluster: The cluster to begin looking for free clusters. This cluster will be linked to the newfound cluster in the FAT.
Param new_cluster_addr: The address of the newly allocated cluster.
Returns: EOF if there was a problem with the media, GOODEC if everything went okay.
*/
#ifdef FAT32
signed int alloc_clusters(int32 start_cluster, int32* new_cluster_addr)
#else
signed int alloc_clusters(int16 start_cluster, int32* new_cluster_addr)
#endif
{
#ifdef FAT32
   int32
      cur_cluster,
      next_cluster;
#else // FAT16
   int16
      cur_cluster,
      next_cluster;
#endif // #ifdef FAT32

   // if we're at the end of the file's allocated space, then we need to allocate some more space
   //  figure out where the current character is pointing to
   next_cluster = cur_cluster = start_cluster;

   // get the next free cluster
   if(get_next_free_cluster(&next_cluster) == EOF)
      return EOF;

   if(write_fat(cur_cluster, next_cluster) == EOF)
      return EOF;

#ifdef FAT32
   if(write_fat(next_cluster, 0x0FFFFFFF) == EOF)
      return EOF;

#else // FAT16
   if(write_fat(next_cluster, 0xFFFF) == EOF)
      return EOF;
#endif // #ifdef FAT32

   // erase all of the data in the newly linked cluster
   if(clear_cluster(next_cluster) == EOF)
      return EOF;

   // put the current character to this position
   *new_cluster_addr = cluster_to_addr(next_cluster);

   return GOODEC;
}

/*
signed int dealloc_clusters(int16 start_cluster)
Summary: De-allocates linked clusters from the FAT.
Param: The starting cluster to deallocate.
Returns: EOF if there was a problem with the media, GOODEC if everything went okay.
*/
#ifdef FAT32
signed int dealloc_clusters(int32 start_cluster)
#else
signed int dealloc_clusters(int16 start_cluster)
#endif
{
#ifdef FAT32
   int32
      cur_cluster,              // the current cluster we're pointing to
      next_cluster;             // the next cluster we're going to point to
#else // FAT16
   int16
      cur_cluster,              // the current cluster we're pointing to
      next_cluster;             // the next cluster we're going to point to
#endif // #ifdef FAT32

   //  figure out where the first cluster is
   next_cluster = cur_cluster = start_cluster;
   do
   {
      // get the next cluster
      if(get_next_cluster(&next_cluster) == EOF)
         return EOF;

      // unlink the current cluster in the FAT
      if(write_fat(cur_cluster, 0) == EOF)
         return EOF;

      cur_cluster = next_cluster;
   }
#ifdef FAT32
    while(cur_cluster != 0x0FFFFFFF);
#else // FAT16
    while(cur_cluster != 0xFFFF);
#endif // #ifdef FAT32

   return GOODEC;
}

/*
signed int clear_cluster(int16 cluster)
Summary: Clears out all of the data in a given cluster.
Param: The cluster to clear out.
Returns: EOF if there was a problem with the media, GOODEC if everything went okay.
*/
#ifdef FAT32
signed int clear_cluster(int32 cluster)
#else
signed int clear_cluster(int16 cluster)
#endif
{
   int
      clear_entry[0x20],
      j;

   int16 i;

   int32 start_addr;

   start_addr = cluster_to_addr(cluster);
   
   for(j = 0; j < 0x20; j += 1)
      clear_entry[j] = 0;

   for(i = 0; i < Bytes_Per_Cluster; i += 0x20)
      if(mmcsd_write_data(start_addr + i, 0x20, clear_entry) != GOODEC)
         return EOF;

   return GOODEC;
}

/*
signed int write_fat(int32 cluster, int32 data)
Summary: Writes specified data about a cluster to the FAT.
Param cluster: The cluster to modify the in the FAT.
Param data: The data about the cluster to put into the FAT.
Returns: EOF if there was a problem with the media, GOODEC if everything went okay.
*/
#ifdef FAT32
signed int write_fat(int32 cluster, int32 data)
{
   if(mmcsd_write_data((cluster << 2) + FAT_Start, 4, &data) != GOODEC)
      return EOF;

   return GOODEC;
}
#else // FAT16
signed int write_fat(int16 cluster, int16 data)
{
   if(mmcsd_write_data((cluster << 1) + FAT_Start, 2, &data) != GOODEC)
      return EOF;

   return GOODEC;
}
#endif // #ifdef FAT32

/*
signed int read_buffer(FILE* stream, int* val)
Summary: Reads from the buffer.
Param stream: The stream whose buffer to read from.
Param val: A pointer to a varaible to put the read data into.
Returns: EOF if there was a problem with the media, GOODEC if everything went okay.
*/
signed int read_buffer(FILE* stream, int* val)
{
   int i;    // counter for loops

   // check to see if we need to populate the buffer
   if((stream->Cur_Char % STREAM_BUF_SIZE) == 0)
   {
      if(mmcsd_read_data(stream->Cur_Char, STREAM_BUF_SIZE, stream->Buf) != GOODEC)
      {
         stream->Flags |= Read_Error;
         return EOF;
      }
   }

   // grab the value at the top of the buffer
   *val = stream->Buf[0];

   // shift everything over 1 byte to put a new value at the top of the buffer
   for(i = 0; i < 8; i += 1)
      rotate_right(stream->Buf, STREAM_BUF_SIZE);

   return GOODEC;
}

/*
signed int write_buffer(FILE* stream, int val)
Summary: Writes to the buffer.
Param stream: The stream whose buffer to write to.
Param val: A variable to write to the buffer.
Returns: EOF if there was a problem with the media, GOODEC if everything went okay.
*/
signed int write_buffer(FILE* stream, int val)
{
   // check to see if we should dump the buffer to the card
   if(((stream->Cur_Char % STREAM_BUF_SIZE) == 0)
      && (stream->Cur_Char != stream->Start_Addr))
   {
      // dump the buffer to the card
      if(mmcsd_write_data(stream->Cur_Char - STREAM_BUF_SIZE, STREAM_BUF_SIZE, stream->Buf) != GOODEC)
      {
         stream->Flags |= Write_Error;
         return EOF;
      }
   }

   // fill up a byte on the buffer
   stream->Buf[stream->Cur_Char % STREAM_BUF_SIZE] = val;

   return GOODEC;
}

/*
void fill_entry(char the_entry[], char val, int8 start_ind)
Summary: This will fill up the unused spots in a standard FAT entry with a value.
Param the_entry[]: The entry that will be modified.
Param val: The value to fill the entry with.
Param start_ind: The beginning index to start filling the_entry.
Returns: Nothing.
*/
void fill_entry(char the_entry[], char val, int8 start_ind)
{
   int8 i;

   for(i = start_ind; i < 13; i += 1)
   {
      if(i < 5)
      {
         the_entry[(i << 1) + 1] = val;
         the_entry[(i << 1) + 2] = val;
      }

      else if(i < 11)
      {
         the_entry[(i << 1) + 4] = val;
         the_entry[(i << 1) + 5] = val;
      }

      else
      {
         the_entry[(i << 1) + 6] = val;
         the_entry[(i << 1) + 7] = val;
      }
   }
}

/*
void disp_timestamp(int16 timestamp)
Summary: Parses an timestamp from a file entry and displays it to the console
Param: The 16-bit timestamp code from a file entry
Returns: Nothing.
*/
void disp_timestamp(int16 timestamp)
{
   // Hours:Minutes:Seconds
   printf("%lu:%lu:%lu",
      timestamp >> 11,
      (timestamp & 0x07E0) >> 5,
      (timestamp & 0x001F) << 1);
}

/*
void disp_datestamp(int16 datestamp)
Summary: Parses an datestamp from a file entry and displays it to the console
Param: The 16-bit datestamp code from a file entry
Returns: Nothing.
*/
void disp_datestamp(int16 datestamp)
{
   // Month/Day/Year
   printf("%lu/%lu/%lu",
      (datestamp & 0x01F0) >> 5,
      datestamp & 0x001F,
      (1980 + (datestamp >> 9)));
}

/// Data Utility Functions ///

/*
signed int fat_init()
Summary: Initializes global variables that are essential for this library working
Returns: EOF if there was a problem with the media, GOODEC if everything went okay.
Note: This must be called before any other function calls in this library.
*/
signed int fat_init()
{
   int ec = 0;

   int
      FATs,
      Sectors_Per_Cluster;

   int16
      Bytes_Per_Sector,
      Reserved_Sectors,
      Small_Sectors;

   int32
      Hidden_Sectors,
      Large_Sectors;

#ifdef FAT32
   int32 Sectors_Per_FAT;
#else // FAT16
   int16
      Root_Entries,
      Sectors_Per_FAT;
#endif // #ifdef FAT32

   // initialize the media
   ec += mmcsd_init();

   // start filling up variables
   ec += mmcsd_read_data(11, 2, &Bytes_Per_Sector);
   ec += mmcsd_read_data(13, 1, &Sectors_Per_Cluster);
   ec += mmcsd_read_data(14, 2, &Reserved_Sectors);
   ec += mmcsd_read_data(16, 1, &FATs);
#ifdef FAT16
   ec += mmcsd_read_data(17, 2, &Root_Entries);
#endif // #ifdef FAT16
   ec += mmcsd_read_data(19, 2, &Small_Sectors);
#ifdef FAT32
   ec += mmcsd_read_data(36, 4, &Sectors_Per_FAT);
#else // FAT16
   ec += mmcsd_read_data(22, 2, &Sectors_Per_FAT);
#endif // #ifdef FAT32
   ec += mmcsd_read_data(28, 4, &Hidden_Sectors);
   ec += mmcsd_read_data(32, 4, &Large_Sectors);
#ifdef FAT16
   Next_Free_Clust = 2;
#else
   ec += mmcsd_read_data(0x3EC, 4, &Next_Free_Clust);
#endif
   if(ec != GOODEC)
      return EOF;

   // figure out the size of a cluster
   Bytes_Per_Cluster = Sectors_Per_Cluster * Bytes_Per_Sector;

   // figure out how long one FAT is
   FAT_Length = Sectors_Per_FAT * (int32)Bytes_Per_Sector;

   // figure out where the FAT starts
   FAT_Start = Reserved_Sectors * Bytes_Per_Sector;

   // figure out where the root directory starts
   Root_Dir = FAT_Start + (FATs * FAT_Length);

   // figure out where data for files in the root directory starts
#ifdef FAT32
   Data_Start = Bytes_Per_Cluster + Root_Dir;
#else // FAT16
   Data_Start = (Root_Entries * 0x20) + (Bytes_Per_Sector - 1);
   Data_Start /= Bytes_Per_Sector;
   Data_Start += Reserved_Sectors + (FATs * Sectors_Per_FAT);
   Data_Start *= Bytes_Per_Sector;
#endif // #ifdef FAT32

   return GOODEC;
}

/*
signed int get_next_cluster(int16* my_cluster)
Summary: Gets the next linked cluster from the FAT.
Param: A pointer to a variable that holds a cluster.
        This variable will then have the next linked cluster when the function returns.
Returns: EOF if there was a problem with the media, GOODEC if everything went okay.
*/
#ifdef FAT32
signed int get_next_cluster(int32* my_cluster)
#else
signed int get_next_cluster(int16* my_cluster)
#endif
{
   // convert the current cluster into the address of where information about
   //  the cluster is stored in the FAT, and put this value into the current cluster
#ifdef FAT32
   if(mmcsd_read_data((*my_cluster << 2) + FAT_Start, 4, my_cluster) != GOODEC)
      return EOF;
#else // FAT16
   if(mmcsd_read_data((*my_cluster << 1) + FAT_Start, 2, my_cluster) != GOODEC)
      return EOF;
#endif // #ifdef FAT32
   return GOODEC;
}

/*
signed int get_prev_cluster(int32* my_cluster)
Summary: Gets the previously linked cluster in the FAT.
Param: A pointer to a variable that holds a cluster.
        This variable will then have the previous linked cluster when the function returns.
Returns: EOF if there was a problem with the media, GOODEC if everything went okay.
*/
#ifdef FAT32
signed int get_prev_cluster(int32* my_cluster)
#else
signed int get_prev_cluster(int16* my_cluster)
#endif // #ifdef FAT32
{
#ifdef FAT32
   int32
      cur_cluster = 1,
      target_cluster = 0;
#else
   int16
      cur_cluster = 1,
      target_cluster = 0;
#endif // #ifdef FAT32
   
   while(target_cluster != *my_cluster)
   {   
      cur_cluster += 1;
#ifdef FAT32
      if(mmcsd_read_data((cur_cluster << 2) + FAT_Start, 4, &target_cluster) != GOODEC)
         return EOF;
#else // FAT16
      if(mmcsd_read_data((cur_cluster << 1) + FAT_Start, 2, &target_cluster) != GOODEC)
         return EOF;
#endif // #ifdef FAT32
   }
   
#ifdef FAT32
   *my_cluster = cur_cluster;                        
#else // FAT16
   *my_cluster = cur_cluster;
#endif // #ifdef FAT32   
   
   return GOODEC;
}

/*
signed int get_next_addr(int32* my_addr)
Summary: Get the next linked address.
Param: A pointer to a variable that holds an address.
        This variable will then have the next linked address when the function returns.
Returns: EOF if there was a problem with the media or we've reached the last linked cluster in the FAT, GOODEC if everything went okay.
*/
signed int get_next_addr(int32* my_addr)
{
#ifdef FAT32
   int32 temp;
#else // FAT16
   int16 temp;
#endif // #ifdef FAT32

   // check to make sure that the next iteration will give us a contiguous address
#ifdef FAT32
   if((*my_addr + 1) % Bytes_Per_Cluster == 0)
#else // FAT16
   // we have to handle this differently because of the way FAT16 handles the root directory
   if(((*my_addr + 1) % Bytes_Per_Cluster == 0)
      && (*my_addr >= Data_Start))
#endif // #ifdef FAT32
   {
      // convert the current address into the address of where information about
      //  the address is stored in the FAT, and put this value into the current address
      temp = addr_to_cluster(*my_addr);
      if(get_next_cluster(&temp) == EOF)
         return EOF;
#ifdef FAT32
      if((temp == 0xFFFFFFFF)
         || (temp == 0x0FFFFFFF)) // WinXP will format the root directory's FAT entry to 0x0FFFFFFF.
         return EOF;
#else // FAT16
      if(temp == 0xFFFF)
         return EOF;
#endif // #ifdef FAT32

      *my_addr = cluster_to_addr(temp);
   }
   else
      *my_addr += 1;

   return GOODEC;
}

/*
signed int get_prev_addr(int32* my_addr)
Summary: Get the next linked address.
Param: A pointer to a variable that holds an address.
        This variable will then have the next linked address when the function returns.
Returns: EOF if there was a problem with the media or we've reached the last linked cluster in the FAT, GOODEC if everything went okay.
*/
signed int get_prev_addr(int32* my_addr)
{
#ifdef FAT32
   int32 temp;
#else // FAT16
   int16 temp;
#endif // #ifdef FAT32

   // if we're trying to go backwards one entry from the beginning of the root,
   //  we won't be able to...
   if(*my_addr <= Root_Dir)
      return GOODEC;

   // check to make sure that the next iteration will give us a contiguous address
#ifdef FAT32
   if(*my_addr % Bytes_Per_Cluster == 0)
#else // FAT16
   // we have to handle this differently because of the way FAT16 handles the root directory
   if((*my_addr % Bytes_Per_Cluster == 0)
      && (*my_addr >= Data_Start))
#endif // #ifdef FAT32
   {
      temp = addr_to_cluster(*my_addr);
      if(get_prev_cluster(&temp) == EOF)
         return EOF;

      *my_addr = cluster_to_addr(temp) + (Bytes_Per_Cluster - 1);
   }
   else
      *my_addr -= 1;

   return GOODEC;
}

/*
int32 cluster_to_addr(int32 cluster)
Summary: Converts a cluster number to an address.
Param: The cluster to convert.
Returns: The cluster's address.
*/
#ifdef FAT32
int32 cluster_to_addr(int32 cluster)
{
   // in unit math:
   //  return  Bytes+(Bytes  /  Cluster * (Clusters - Clusters))
   return Root_Dir + (Bytes_Per_Cluster * (cluster - 2));
}
#else
int32 cluster_to_addr(int16 cluster)
{
   if(cluster < 2)
      return 0;
   // in unit math:
   //  return  Bytes + (       Bytes  /  Cluster * (Clusters- Clusters))
   return Data_Start + ((int32)Bytes_Per_Cluster * (cluster - 2));
}
#endif

/*
int32 addr_to_cluster(int32 addr)
Summary: Converts an address to a cluster number.
Param: The address to convert.
Returns: The address's cluster.
*/
#ifdef FAT32
int32 addr_to_cluster(int32 addr)
{
   // in unit math:
   //  return (Bytes -Bytes) / Bytes  /  Cluster) + Clusters
   return ((addr - Root_Dir) / Bytes_Per_Cluster) + 2;
}
#else
int16 addr_to_cluster(int32 addr)
{
   if(addr < Data_Start)
      return 0;
   // in unit math:
   //  return (Bytes -Bytes)   /(Bytes  /  Cluster) + Clusters
   return ((addr - Data_Start) / Bytes_Per_Cluster) + 2;
}
#endif
/*
signed int format(int32 DskSize)
Summary: Formats media with a FAT filesystem.
Param: The size of the filesystem to create in kB.
Returns: EOF if there was a problem with the media, GOODEC if everything went okay.
Note: There are certain minimum and maximum size restrictions on the card and type of file system. The restrictions are as follows:
       FAT16: DskSize < 2GB
       FAT32: 33MB < DskSize < 32GB
       In order to change the way that the drive is formatted, select the proper #define(FAT16 or FAT32) way up at the top of this file.
Note: In this context, 1kB = 1024B = 2^10B. Please don't confuse this with 10^3B, we don't want to be wasting thousands of bytes of information now, do we?
Note: DskSize has a lower limit of 64, anything lower becomes highly inefficient and runs the risk of very quick corruption.
Note: If this is called on an MMC/SD card, Windows will recognize it as a RAW filesystem.
*/
signed int format(int32 DskSize)
{
   int
      BPB_Media = 0xF8,
      BPB_NumFATs = 1,
      BPB_NumHeads = 2,
      BPB_SecPerClus,
      BPB_SecPerTrk = 0x20;

   int16
      BPB_BytsPerSec = 0x200,
      i;

   int32
      BPB_TotSec,
      BS_VolID = 0,
      RootDirSectors,
      TmpVal1,
      TmpVal2;

   char               
      BS_OEMName[] = "MSDOS5.0",
      BS_VolLab[] = "NO NAME    ";

#ifdef FAT32
   int
      BPB_BkBootSec = 6,
      BPB_FSInfo = 1,
      BPB_RootClus = 2,
      BS_BootSig = 0x29,
      BS_jmpBoot = 0x58,
      data[0x5A];

   int16
      BPB_RootEntCnt = 0,
      BPB_RsvdSecCnt = 32;
   
   int32 BPB_FATSz;
   
   char BS_FilSysType[] = "FAT32   ";
#else
   int
      BS_BootSig = 0x29,
      BS_jmpBoot = 0x3C,
      data[0x3E];
      
   int16
      BPB_FATSz,
      BPB_RootEntCnt = 512,
      BPB_RsvdSecCnt = 1;
      
   char BS_FilSysType[] = "FAT12   ";
#endif // #ifdef FAT32

   // initialize variables
   // figure out total sectors
   BPB_TotSec = (DskSize * 0x400) / BPB_BytsPerSec;
   
   // use the magical table on page 20 of fatgen103.pdf to determine sectors per cluster
#ifdef FAT32
   if(DskSize < 0x8400) // < 33 MB; this is too small
      return EOF;
   else if(DskSize < 0x41000) // 260 MB
      BPB_SecPerClus = 1;
   else if(DskSize < 0X800000) // 8 GB
      BPB_SecPerClus = 8;
   else if(DskSize < 0x1000000) // 16 GB
      BPB_SecPerClus = 16;
   else if(DskSize < 0x2000000) // 32 GB
      BPB_SecPerClus = 32;
   else // > 32 GB; this is too big
      return EOF;
#else
   if(DskSize < 0x1400) // < 5 MB
      BPB_SecPerClus = 1;
   else if(DskSize < 0x4000) // 16 MB
      BPB_SecPerClus = 2;
   else if(DskSize < 0X20000) // 128 MB
      BPB_SecPerClus = 4;
   else if(DskSize < 0x40000) // 256 MB
      BPB_SecPerClus = 8;
   else if(DskSize < 0x80000) // 512 MB
      BPB_SecPerClus = 16;
   else if(DskSize < 0x100000) // 1 GB
      BPB_SecPerClus = 32;
   else if(DskSize < 0x200000) // 2 GB
      BPB_SecPerClus = 64;
   else // > 2 GB; this is too big
      return EOF;
#endif // #ifdef FAT32

   // figure out how many sectors one FAT takes up
   RootDirSectors = ((BPB_RootEntCnt * 32) + (BPB_BytsPerSec - 1)) / BPB_BytsPerSec; 
   TmpVal1 = DskSize - (BPB_RsvdSecCnt + RootDirSectors); 
   TmpVal2 = (256 * BPB_SecPerClus) + BPB_NumFATs; 
#ifdef FAT32
   TmpVal2 = TmpVal2 / 2;
#endif // #ifdef FAT32 
   BPB_FATSz = (TmpVal1 + (TmpVal2 - 1)) / TmpVal2;

   // zero data
   for(i = 0; i < sizeof(data); i += 1)
      data[i] = 0;

   // start filling up data
   data[0] = 0xEB;
   data[1] = BS_jmpBoot;
   data[2] = 0x90;   
   sprintf(data + 3, "%s", BS_OEMName);
   data[11] = make8(BPB_BytsPerSec, 0);
   data[12] = make8(BPB_BytsPerSec, 1);
   data[13] = BPB_SecPerClus;
   data[14] = BPB_RsvdSecCnt;
   data[16] = BPB_NumFATs;
   data[21] = BPB_Media;
   data[24] = BPB_SecPerTrk; 
   data[26] = BPB_NumHeads;
#ifdef FAT32
   data[32] = make8(BPB_TotSec, 0);
   data[33] = make8(BPB_TotSec, 1);
   data[34] = make8(BPB_TotSec, 2);
   data[35] = make8(BPB_TotSec, 3);
   data[36] = make8(BPB_FATSz, 0);
   data[37] = make8(BPB_FATSz, 1);
   data[38] = make8(BPB_FATSz, 2);
   data[39] = make8(BPB_FATSz, 3);
   data[44] = BPB_RootClus;
   data[48] = BPB_FSInfo;
   data[50] = BPB_BkBootSec;
   data[66] = BS_BootSig;
   data[67] = make8(BS_VolID, 0);
   data[68] = make8(BS_VolID, 1);
   data[69] = make8(BS_VolID, 2);
   data[70] = make8(BS_VolID, 3);
   sprintf(data + 71, "%s", BS_VolLab);
   sprintf(data + 82, "%s", BS_FilSysType);

   // put data onto the card
   // first, all the partition parameters
   if(mmcsd_write_data(0, sizeof(data), data) != GOODEC)
      return EOF;

   // figure out where the first FAT starts
   TmpVal1 = BPB_BytsPerSec * BPB_RsvdSecCnt;

   // figure out where the root directory starts
   TmpVal2 = TmpVal1 + (BPB_NumFATs * BPB_FATSz);

   // clear out some values in data
   for(i = 0; i < 0x20; i += 1)
      data[i] = 0;

   // get rid of everything in the root directory
   clear_cluster(2);
   
   // clear out the FAT
   for(i = 0; i < BPB_FATSz; i += 0x20)
      if(mmcsd_write_data(TmpVal1 + i, 0x20, data) != GOODEC)
         return EOF;

   // insert the first 12 entries into the FAT(s)
   data[0] = 0xF8;
   data[1] = 0xFF;
   data[2] = 0xFF;
   data[3] = 0x0F;
   data[4] = 0xFF;
   data[5] = 0xFF;
   data[6] = 0xFF;
   data[7] = 0x0F;
   data[8] = 0xFF;
   data[9] = 0xFF;
   data[10] = 0xFF;
   data[11] = 0x0F;
   if(mmcsd_write_data(TmpVal1, 0x20, data) != GOODEC)
      return EOF;
      
   // reset the last cluster
   i = 2;
   if(mmcsd_write_data(0x3EC, 4, &i) != GOODEC)
      return EOF;
#else
   data[17] = make8(BPB_RootEntCnt, 0);
   data[18] = make8(BPB_RootEntCnt, 1);
   data[19] = make8(BPB_TotSec, 0);
   data[20] = make8(BPB_TotSec, 1);
   data[22] = make8(BPB_FATSz, 0);
   data[23] = make8(BPB_FATSz, 1);
   data[38] = BS_BootSig;
   data[39] = make8(BS_VolID, 0);
   data[40] = make8(BS_VolID, 1);
   data[41] = make8(BS_VolID, 2);
   data[42] = make8(BS_VolID, 3);
   sprintf(data + 43, "%s", BS_VolLab);
   sprintf(data + 54, "%s", BS_FilSysType);

   // put data onto the card
   // first, all the partition parameters
   if(mmcsd_write_data(0, sizeof(data), data) != GOODEC)
      return EOF;

   // figure out where the first FAT starts
   TmpVal1 = BPB_BytsPerSec * BPB_RsvdSecCnt;

   // figure out where the root directory starts
   TmpVal2 = TmpVal1 + (BPB_NumFATs * BPB_FATSz);

   // clear out some values in data
   for(i = 0; i < 0x20; i += 1)
      data[i] = 0;

   // get rid of everything in the root directory
   for(i = 0; i < (0x20 * BPB_RootEntCnt); i += 0x20)
      if(mmcsd_write_data(TmpVal2 + i, 0x20, data) != GOODEC)
         return EOF;
   
   // clear out the FAT
   for(i = 0; i < BPB_FATSz; i += 0x20)
      if(mmcsd_write_data(TmpVal1 + i, 0x20, data) != GOODEC)
         return EOF;

   // insert the first 3 entries into the FAT(s)
   data[0] = 0xF8;
   data[1] = 0xFF;
   data[2] = 0xFF;
   if(mmcsd_write_data(TmpVal1, 0x20, data) != GOODEC)
      return EOF;
      
#endif // #ifdef FAT32

   i = 0xAA55;

   if(mmcsd_write_data(0x1FE, 2, &i) != GOODEC)
      return EOF;   

   // we're going to have to re-initialize the FAT, a bunch of parameters probably just changed
   fat_init();

   return GOODEC;
}

/// Debugging Utility Functions ///

/*
signed int disp_folder_contents(char foldername[])
Summary: Displays the contents of a folder.
Param: The folder to display the contents of.
Returns: EOF if there was a problem with the media, GOODEC if everything went okay.
*/
signed int disp_folder_contents(char foldername[])
{
   char filename[MAX_FILE_NAME_LENGTH]; // a place to hold a file name
   
   FILE stream; // the stream that we're going to be working with
   
   char mode[] = "r";
   
   if(fatopen(foldername, mode, &stream) != GOODEC)
      return EOF;

   // printf off a header
   printf("\r\n--%s--", foldername);

   // start off at the root directory
   stream.Entry_Addr = stream.Start_Addr;

   while(get_next_file(&stream) != EOF)
   {
      // get the name of the file that we are at
      if(get_file_name(stream.Entry_Addr, filename) != GOODEC)
         return EOF;
     
      // make cool little "tree" branches
      printf("\r\n%s", filename);
      if (stream.File_Type == Directory)
         putc('/');
   }

   fatclose(&stream);

   return GOODEC;
}

/*
signed int dump_addr(int32 from, int32 to)
Summary: Display a series of addresses in a hex editor type fashion.
Param from: The beginning address to display.
Param to: The end address to display.
Returns: EOF if there was a problem with the media, GOODEC if everything went okay.
*/
signed int dump_addr(int32 from, int32 to)
{
   int
      j,          // counter for loops
      val[0x10];  // buffer to hold values

   int32 i;       // pointer to memory

   // print off header
   printf("\r\n\r\n         ");
   for(i = 0; i < 0x10; i += 1)
      printf("%2X ", i);

   // note that the to and from values are being rounded up and down
   //  this makes a nice "block" map in case someone inputs a number that
   //  isn't evenly divisible by 0x10
   for(i = (from - (from % 0x10)); i <= (to + (to % 0x10)); i += 0x10)
   {
      // printf memory block
      printf("\r\n%lX ", i);

      // fill up buffer
      if(mmcsd_read_data(i, 0x10, val) != GOODEC)  
         return EOF;

      // printf RAM in hex
      for(j = 0; j < 0X10; j += 1)
         printf("%X ", val[j]);

      // printf spacer
      printf("; ");

      // printf RAM in char
      for(j = 0; j < 0X10; j += 1)
      {
         // check for characters that will mess up the nice-looking map
         if(val[j] < ' ')
            val[j] = '.';

         printf("%c", val[j]);
      }
   }
   return GOODEC;
}

/*
signed int dump_clusters(int32 from, int32 to)
Summary: Display a series of clusters in a memory map.
Param from: The beginning clusters to display.
Param to: The end clusters to display.
Returns: EOF if there was a problem with the media, GOODEC if everything went okay.
*/
signed int dump_clusters(int32 from, int32 to)
{
   // convert the clusters to addresses and dump
   if(dump_addr(cluster_to_addr(from), cluster_to_addr(to)) != GOODEC)
      return EOF;
}

/*
void disp_fat_stats()
Summary: Display essential statistics about the FAT to the console.
Returns: Nothing.
*/
void disp_fat_stats()
{
   printf("\r\n\r\n--FAT Stats--\r\n");
   printf("First FAT starts at: 0x%lX\r\n", FAT_Start);
   printf("Data Starts At: 0x%lX\r\n", Data_Start);
   printf("Root Directory Is At: 0x%lX\r\n", Root_Dir);
   printf("Bytes Per Cluster: 0x%lX\r\n", Bytes_Per_Cluster);
}

/*
signed int fatprintfinfo(FILE* stream)
Summary: Display essential statistics about the file that a stream is pointing to.
Param: The stream to print off information about.
Returns: EOF if there was a problem with the media, GOODEC if everything went okay.
*/
signed int fatprintfinfo(FILE* stream)
{
   int ec = 0;

   int32 val = 0; // buffer to hold values

   char name[MAX_FILE_NAME_LENGTH];

   // get name
   if(get_file_name(stream->Entry_Addr, name) != GOODEC)
      return EOF;

   // printf header
   printf("\r\n\r\n--");
   printf(name);
   printf(" Info--");

   // printf attributes
   ec += mmcsd_read_data(stream->Entry_Addr + 0x0B, 1, &val);
   printf("\r\nAttributes: 0x%X", val);

   // printf creation date
   printf("\r\nCreated: ");
   ec += mmcsd_read_data(stream->Entry_Addr + 0x10, 2, &val);
   disp_datestamp(val);
   printf(" ");

   // printf creation time
   ec += mmcsd_read_data(stream->Entry_Addr + 0x0E, 2, &val);
   disp_timestamp(val);

   // printf modification date
   printf("\r\nModified: ");
   ec += mmcsd_read_data(stream->Entry_Addr + 0x18, 2, &val);
   disp_datestamp(val);
   printf(" ");

   // printf modification time
   ec += mmcsd_read_data(stream->Entry_Addr + 0x16, 2, &val);
   disp_timestamp(val);

   // printf starting cluster
   ec += mmcsd_read_data(stream->Entry_Addr + 0x14, 2, (int16*)&val + 1);
   ec += mmcsd_read_data(stream->Entry_Addr + 0x1A, 2, &val);

   printf("\r\nStarting cluster: %lX", val);

   // printf starting address
   printf("\r\nStarting address: %lX", cluster_to_addr(val));

   // printf size
   ec += mmcsd_read_data(stream->Entry_Addr + 0x1C, 4, &val);
   printf("\r\nSize: %lu Bytes\r\n", val);

   if(ec != GOODEC)
      return EOF;

   return GOODEC;
}

#endif // #ifndef FAT_PIC_C

