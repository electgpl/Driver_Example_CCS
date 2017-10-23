///////////////////////////////////////////////////////////////////////////
////                       bootloader_pcd.h                            ////
////                                                                   ////
////  This include file must be included by any application loaded     ////
////  by the example bootloader (ex_pcd_bootloader.c).                 ////
////                                                                   ////
////  The directives in this file relocates the reset vector as well   ////
////  as reserving space for the bootloader.                           ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2013 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS    ////
//// C compiler.  This source code may only be distributed to other    ////
//// licensed users of the CCS C compiler.  No other use,              ////
//// reproduction or distribution is permitted without written         ////
//// permission.  Derivative programs created using this software      ////
//// in object code form are not restricted in any way.                ////
///////////////////////////////////////////////////////////////////////////
#ifndef __BOOTLOADER_H__
#define __BOOTLOADER_H__

#ifndef LOADER_PAGES
 #if getenv("FLASH_ERASE_SIZE") == 128
  #define LOADER_PAGES 19
 #else
  #define LOADER_PAGES 2
 #endif
#endif

#ifndef BOOTLOADER_AT_START
 #define BOOTLOADER_AT_START
#endif

#ifdef BOOTLOADER_AT_START
 #define LOADER_SIZE ((LOADER_PAGES * (getenv("FLASH_ERASE_SIZE")/2)) - 1)

 #define LOADER_END  LOADER_SIZE
 #define LOADER_ADDR (LOADER_END - LOADER_SIZE)
#else
 #if ((getenv("PROGRAM_MEMORY") % (getenv("FLASH_ERASE_SIZE")/2)) !=0 )
  #define LOADER_SIZE (((getenv("PROGRAM_MEMORY") % (getenv("FLASH_ERASE_SIZE")/2)) - 1) + ((LOADER_PAGES - 1) * (getenv("FLASH_ERASE_SIZE")/2)))
 #else
  #define LOADER_SIZE ((LOADER_PAGES * (getenv("FLASH_ERASE_SIZE")/2)) - 1)
 #endif
 #define LOADER_END  (getenv("PROGRAM_MEMORY") - 1)
 #define LOADER_ADDR (LOADER_END - LOADER_SIZE)
#endif

#ifndef _bootloader
 #ifdef BOOTLOADER_AT_START
  #build (reset=LOADER_END+1,interrupt=LOADER_END+5)
  #org 0, LOADER_END {}
 #else
  #org LOADER_ADDR, LOADER_END {}
 #endif
#else
 #ifdef BOOTLOADER_AT_START
  #if getenv("PROGRAM_MEMORY") <= 0x10000
   #org (LOADER_END+7), (getenv("PROGRAM_MEMORY")-2) {}
  #else
   #org (LOADER_END+7), 0xFFFE {}
   #if getenv("PROGRAM_MEMORY") <= 0x20000
    #org 0x10000, (getenv("PROGRAM_MEMORY")-2) {}
   #else
    #org 0x10000, 0x1FFFE {}
    #if getenv("PROGRAM_MEMORY") <= 0x30000
     #org 0x20000, (getenv("PROGRAM_MEMORY")-2) {}
    #else
     #org 0x20000, 0x2FFFE {}
     #if getenv("PROGRAM_MEMORY") <= 0x40000
      #org 0x30000, (getenv("PROGRAM_MEMORY")-2) {}
     #else
      #org 0x30000, 0x3FFFE {}
      #if getenv("PROGRAM_MEMORY") <= 0x50000
       #org 0x40000, (getenv("PROGRAM_MEMORY")-2) {}
      #else
       #org 0x40000, 0x4FFFE {}
       #org 0x50000, (getenv("PROGRAM_MEMORY")-2) {}
      #endif
     #endif
    #endif
   #endif
  #endif
 #endif
#endif

#endif
