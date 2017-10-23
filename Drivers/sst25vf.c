////////////////////////////////////////////////////////////////////////////////
////  Library for the SST25VF0XXB family of flash memory.                   ////
////                                                                        ////
////  ext_flash_init()              Call once before any other functions    ////
////                                                                        ////
////  b = ext_eeprom_busy()         Returns TRUE if the flash is busy       ////
////                                                                        ////
////  ext_flash_write(a, d, l)      Write l bytes from d to the address a   ////
////  ext_flash_write_byte(a, d)    Write byte d to address a               ////
////  ext_flash_write_word(a, d)    Write word d to address a               ////
////  ext_flash_write_dword(a, d)   Write dword d to address a              ////
////                                                                        ////
////  ext_flash_read(a, d, l)       Read l bytes from address a into d      ////
////  d = ext_flash_read_byte(a)    Read a byte from address a into d       ////
////  d = ext_flash_read_word(a)    Read a word from address a into d       ////
////  d = ext_flash_read_dword(a)   Read a dword from address a into d      ////
////                                                                        ////
////  ext_flash_bulk_erase()        Erase entire flash memory               ////
////  ext_flash_address_erase(a, s) Erase s size block of memory around a   ////
////  ext_flash_block_erase(b, s)   Erase s size block of memory at block b ////
////                                (Block index dependent on block size)   ////
////                                                                        ////
////  ext_flash_read_id(*id)        Stores flash chip id info in *id        ////
////                                                                        ////
////  ext_flash_protect(r)          Adjust write protection ranges          ////
////  (Note that this flash protects all locations on power up)             ////
////                                                                        ////
////  The main program may define FLASH_STREAM to override the default      ////
////  #use spi stream below.                                                ////
////                                                                        ////
////  The main program may define FLASH_SELECT_PIN to override the default  ////
////  pin used to control CE.                                               ////
////                                                                        ////
////                         Pin Layout                                     ////
////  -----------------------------------------------------------           ////
////  |                                                         |           ////
////  | 1: /CE  Enable Pin       | 8: VCC   +5V                 |           ////
////  |                          |                              |           ////
////  | 2: SDO  SPI Data Out     | 7: /HOLD Hold Serial         |           ////
////  |                          |                              |           ////
////  | 3: /WP  Write Protect    | 6: SCK   SPI Clock           |           ////
////  |                          |                              |           ////
////  | 4: VSS  GND              | 5: SDI   SPI Data In         |           ////
////  -----------------------------------------------------------           ////
////                                                                        ////
////////////////////////////////////////////////////////////////////////////////
////                                                                        ////
////        (C) Copyright 1996, 2011 Custom Computer Services               ////
////  This source code may only be used by licensed users of the CCS C      ////
////  compiler.  This source code may only be distributed to other          ////
////  licensed users of the CCS C compiler.  No other use, reproduction     ////
////  or distribution is permitted without written permission.              ////
////  Derivative programs created using this software in object code        ////
////  form are not restricted in any way.                                   ////
////                                                                        ////
////////////////////////////////////////////////////////////////////////////////

#ifndef SST25VF_H
#define SST25VF_H

#include <stdint.h>

typedef enum _FLASH_ERASE_BLOCK_SIZE
{
  FLASH_ERASE_BLOCK_4KB   = 0,  // 4KB
  FLASH_ERASE_BLOCK_32KB  = 1,  // 32KB
  FLASH_ERASE_BLOCK_64KB  = 2   // 64KB
} FLASH_ERASE_BLOCK_SIZE;

#define FLASH_PROTECT_LOCK 0x80
#define FLASH_PROTECT_UNLOCK 0x00
typedef enum _FLASH_PROTECT_RANGE
{
  FLASH_PROTECT_NONE  = 0,
  FLASH_PROTECT_64KB  = 1,
  FLASH_PROTECT_128KB = 2,
  FLASH_PROTECT_256KB = 3,
  FLASH_PRTOECT_512KB = 4,
  FLASH_PROTECT_1MB   = 5,
  FLASH_PROTECT_2MB   = 6,
  FLASH_PROTECT_ALL   = 7
} FLASH_PROTECT_RANGE;

typedef struct _FLASH_ID
{
   uint8_t manufacturer;
   uint8_t type;
   uint8_t capacity;
} FLASH_ID;

#ifndef FLASH_SIZE
#define FLASH_SIZE         2097152
#endif

#ifndef FLASH_SECTOR_SIZE
#define FLASH_SECTOR_SIZE  4096 
#endif

void ext_flash_init();
int1 ext_flash_busy();

void ext_flash_write(uint32_t addr, uint8_t *data, uint32_t len);
void ext_flash_write_byte(uint32_t addr, uint8_t data);
void ext_flash_write_word(uint32_t addr, uint16_t data);
void ext_flash_write_dword(uint32_t addr, uint32_t data);

void ext_flash_read(uint32_t addr, uint8_t *data, uint32_t len);
uint8_t ext_flash_read_byte(uint32_t addr);
uint16_t ext_flash_read_word(uint32_t addr);
uint32_t ext_flash_read_dword(uint32_t addr);

void ext_flash_bulk_erase();
void ext_flash_address_erase(uint32_t addr, FLASH_ERASE_BLOCK_SIZE size);
void ext_flash_block_erase(uint32_t block, FLASH_ERASE_BLOCK_SIZE size);

void ext_flash_read_id(FLASH_ID *id);

void ext_flash_protect(FLASH_PROTECT_RANGE range);

// Commands
#define SST25VF_READ_DATA        0x03
#define SST25VF_FAST_READ_DATA   0x0B
#define SST25VF_SECTOR_ERASE     0x20
#define SST25VF_BLOCK_32_ERASE   0x52
#define SST25VF_BLOCK_64_ERASE   0xD8
#define SST25VF_BULK_ERASE       0xC7
#define SST25VF_BYTE_PROG        0x02
#define SST25VF_WORD_PROG        0xAD
#define SST25VF_READ_STATUS      0x05
#define SST25VF_WRITE_STATUS_EN  0x50
#define SST25VF_WRITE_STATUS     0x01
#define SST25VF_WRITE_EN         0x06
#define SST25VF_WRITE_DIS        0x04
#define SST25VF_READ_ID          0x9F
#define SST25VF_HW_WR_STATUS     0x70
#define SST25VF_POLL_WR_STATUS   0x80

#ifndef FLASH_SELECT_PIN
#define FLASH_SELECT_PIN PIN_A0
#endif

#define FLASH_SELECT()          output_low(FLASH_SELECT_PIN)
#define FLASH_DESELECT()        output_high(FLASH_SELECT_PIN)

#ifndef FLASH_STREAM
  #use spi(spi1, MODE=0, BITS=8, STREAM=FLASH_STREAM, FORCE_HW)
#endif

#ifndef FLASH_XFER
#define FLASH_XFER(x) spi_xfer(FLASH_STREAM, x)
#endif

#define FLASH_ADDR_INVALID(addr) (addr > FLASH_SIZE)

// Must match FLASH_BLOCK_SIZE enum
static uint8_t _ext_flash_block_erase_cmds[3] =
{
  SST25VF_SECTOR_ERASE,
  SST25VF_BLOCK_32_ERASE,
  SST25VF_BLOCK_64_ERASE
};

// Must match FLASH_BLOCK_SIZE enum
static uint8_t _ext_flash_block_sizes[3] = {4096, 32768, 65536};

static void _ext_flash_send(uint8_t cmd, int1 end);
static void _ext_flash_end();
static void _ext_flash_send_address(uint32_t addr);

void ext_flash_init()
{
   FLASH_DESELECT();
   output_drive(FLASH_SELECT_PIN);
   FLASH_DESELECT();
   
   // Make sure chip is not in the write state
   _ext_flash_send(SST25VF_WRITE_DIS, TRUE);

   // Tell chip that we will poll the busy flag instead of using SDO
   _ext_flash_send(SST25VF_POLL_WR_STATUS, TRUE);
}

int1 ext_flash_busy()
{
   uint8_t status;

   FLASH_SELECT();
   FLASH_XFER(SST25VF_READ_STATUS);
   status = FLASH_XFER(0); 
   FLASH_DESELECT();

   return (bit_test(status, 0));
}

void ext_flash_write(uint32_t addr, uint8_t *data, uint32_t len)
{
   uint8_t word[2];

   if (FLASH_ADDR_INVALID(addr))
     return;
     
   if (len < 1)
     return;

   // Enable writing to the flash
   _ext_flash_send(SST25VF_WRITE_EN, TRUE);

   // Do intial write separately to allow the rest of the writes
   // to be done as quickly as possible.
   word[0] = 0xFF;
   word[1] = 0xFF;
   if (!bit_test(addr, 0))
   {
     word[0] = *data++;
     len -= 1;
   }
   
   if (len > 0)
   {
      word[1] = *data++;
      len -= 1;
   }

   bit_clear(addr, 0);
   _ext_flash_send(SST25VF_WORD_PROG, FALSE);
   _ext_flash_send_address(addr);
   FLASH_XFER(word[0]);
   FLASH_XFER(word[1]);
   _ext_flash_end();

   // Write remaining bytes
   while(len)
   {
      word[0] = *data++;
      len -= 1;

      if (len > 0)
      {
         word[1] = *data++;
         len -= 1;
      }
      else
        word[1] = 0xFF;

      _ext_flash_send(SST25VF_WORD_PROG, FALSE);
      FLASH_XFER(word[0]);
      FLASH_XFER(word[1]);
      _ext_flash_end();
   }

   // Disable Writes
   _ext_flash_send(SST25VF_WRITE_DIS, TRUE);
}

void ext_flash_write_byte(uint32_t addr, uint8_t data)
{
   if (FLASH_ADDR_INVALID(addr))
     return;

   // We do a byte write since it's more optimal than
   // calling write_ext_flash.
   _ext_flash_send(SST25VF_WRITE_EN, TRUE);

   _ext_flash_send(SST25VF_BYTE_PROG, FALSE);
   _ext_flash_send_address(addr);
   FLASH_XFER(data);
   _ext_flash_end();
}

void ext_flash_write_word(uint32_t addr, uint16_t data)
{
   ext_flash_write(addr, &data, 2);
}

void ext_flash_write_dword(uint32_t addr, uint32_t data)
{
   ext_flash_write(addr, &data, 4);
}

void ext_flash_read(uint32_t addr, uint8_t *data, uint32_t len)
{
   if (FLASH_ADDR_INVALID(addr))
     return;

   _ext_flash_send(SST25VF_FAST_READ_DATA, FALSE);
   _ext_flash_send_address(addr);
   FLASH_XFER(0);  // dummy byte necessary for fast read
   while(len--) { *data++ = FLASH_XFER(0); }
   _ext_flash_end();
}

uint8_t ext_flash_read_byte(uint32_t addr)
{
   uint8_t data = 0;
   ext_flash_read(addr, &data, 1);
   return data;
}

uint16_t ext_flash_read_word(uint32_t addr)
{
   uint16_t data = 0;
   ext_flash_read(addr, &data, 2);
   return data;
}

uint32_t ext_flash_read_dword(uint32_t addr)
{
   uint32_t data = 0;
   ext_flash_read(addr, &data, 4);
   return data;
}

void ext_flash_bulk_erase()
{
   _ext_flash_send(SST25VF_WRITE_EN, TRUE);
   _ext_flash_send(SST25VF_BULK_ERASE, TRUE);
}

void ext_flash_address_erase(uint32_t addr, FLASH_ERASE_BLOCK_SIZE size)
{
   if (FLASH_ADDR_INVALID(addr))
     return;

   _ext_flash_send(SST25VF_WRITE_EN, TRUE);

   _ext_flash_send(_ext_flash_block_erase_cmds[size], FALSE);
   _ext_flash_send_address(addr);
   _ext_flash_end();
}

void ext_flash_block_erase(uint32_t block, FLASH_ERASE_BLOCK_SIZE size)
{
   ext_flash_address_erase((block * _ext_flash_block_sizes[size]), size);
}

void ext_flash_read_id(FLASH_ID *id)
{
   _ext_flash_send(SST25VF_READ_ID, FALSE);
   id->manufacturer = FLASH_XFER(0);
   id->type = FLASH_XFER(0);
   id->capacity = FLASH_XFER(0);
   _ext_flash_end();
}

void ext_flash_protect(FLASH_PROTECT_RANGE range)
{
   // Enable writes to the status register
   _ext_flash_send(SST25VF_WRITE_EN, TRUE);

   // Unlock the protection bits
   _ext_flash_send(SST25VF_WRITE_STATUS, FALSE);
   FLASH_XFER(FLASH_PROTECT_UNLOCK);
   _ext_flash_end();

   // Enable writes to the status register
   _ext_flash_send(SST25VF_WRITE_EN, TRUE);

   // Write the new protection bits and lock them
   _ext_flash_send(SST25VF_WRITE_STATUS, TRUE);
   FLASH_XFER(range | FLASH_PROTECT_LOCK);
   _ext_flash_end();
}

static void _ext_flash_send(uint8_t cmd, int1 end)
{
   while(ext_flash_busy());

   FLASH_SELECT();
   FLASH_XFER(cmd);
   if (end) _ext_flash_end();
}

static void _ext_flash_end()
{
   FLASH_DESELECT();
}

static void _ext_flash_send_address(uint32_t addr)
{
   FLASH_XFER(((addr >> 16) & 0xff));
   FLASH_XFER(((addr >> 8) & 0xff));
   FLASH_XFER((addr & 0xff));
}

#endif
