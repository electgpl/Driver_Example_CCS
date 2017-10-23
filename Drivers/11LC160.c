///////////////////////////////////////////////////////////////////////////
////  Library for MicroChip's family of UNI/O external eeproms.        ////
////                                                                   ////
////  Supported devices are:  11LC010, 11AA010, 11LC020, 11AA020,      ////
////      11LC040, 11AA040, 11LC080, 11AA080, 11LC160, 11AA160         ////
////                                                                   ////
////   init_ext_eeprom();    Call before the other functions are used  ////
////                                                                   ////
////   write_ext_eeprom(a, d);  Write the byte d to the address a      ////
////                                                                   ////
////   d = read_ext_eeprom(a);  Read the byte d from the address a     ////
////                                                                   ////
////   d = read_status_ext_eeprom();  Read EEPROM Status Register into ////
////                                  byte d                           ////
////                                                                   ////
//// This driver is compatible with the ex_extee.c example.            ////
////                                                                   ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2009 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#ifndef PIN_UNIO_TRANSFER
 #define PIN_UNIO_TRANSFER  PIN_D0
#endif

#ifndef EEPROM_SIZE
 #define EEPROM_SIZE    2048
#endif

#define UNIO_DEVICE_ADDRESS   0xA0

//////////// end configuration ////////////////////////////////

#define UNIO_CMD_READ   0x03
#define UNIO_CMD_WRITE  0x6C
#define UNIO_CMD_WREN   0x96
#define UNIO_CMD_RDSR   0x05

#if EEPROM_SIZE>=0x100
 typedef unsigned int16 EEPROM_ADDRESS;
#else
 typedef unsigned int8 EEPROM_ADDRESS;
#endif

#define UNIO_APPEND_MAK    TRUE
#define UNIO_APPEND_NOMAK  FALSE

void wren_ext_eeprom(void);

unsigned int8 g_UNIOUnpackedMsg[15];

// packs an 8bit value into it's 20bit unio signal (16 bits for value, 
// 2 bits for MAK, 2 bits for SAK).
// a bit is set to 1 for floating / high.
// a bit is set to 0 for ground.
static void unio_transfer_append_byte(unsigned int8 value, int1 makValue)
{
   unsigned int bits;
   
   for (bits=0; bits<8; bits++)
   {
      shift_left(&g_UNIOUnpackedMsg[0], sizeof(g_UNIOUnpackedMsg), !bit_test(value, 7));
      shift_left(&g_UNIOUnpackedMsg[0], sizeof(g_UNIOUnpackedMsg), bit_test(value, 7));
      shift_left(&value, sizeof(value), 0);
   }

   shift_left(&g_UNIOUnpackedMsg[0], sizeof(g_UNIOUnpackedMsg), !makValue);
   shift_left(&g_UNIOUnpackedMsg[0], sizeof(g_UNIOUnpackedMsg), makValue);

   //sak
   shift_left(&g_UNIOUnpackedMsg[0], sizeof(g_UNIOUnpackedMsg), 1);
   shift_left(&g_UNIOUnpackedMsg[0], sizeof(g_UNIOUnpackedMsg), 1);
}

static int1 unio_transfer(unsigned int8 device, unsigned int8 command, unsigned int16 address, unsigned int8 *valueTx, unsigned int8 *valueRx)
{
   unsigned int8 totalBits = 0;
   unsigned int8 writeBits = 0;
   unsigned int8 readBits = 0;
   unsigned int8 bits;
   
   unio_transfer_append_byte(0x55, UNIO_APPEND_MAK);
   totalBits+=20;
   writeBits+=20;
   
   unio_transfer_append_byte(device, UNIO_APPEND_MAK);
   totalBits += 20;
   writeBits+=20;
   
   if(command == UNIO_CMD_RDSR)
   {
      unio_transfer_append_byte(command, UNIO_APPEND_MAK);
      totalBits +=20;
      writeBits+=20;
      
      unio_transfer_append_byte(0xFF, UNIO_APPEND_NOMAK);
      totalBits += 20;
      readBits += 20;
   }
   else
   {
      if (valueTx || valueRx)
      {
         unio_transfer_append_byte(command, UNIO_APPEND_MAK);
         totalBits += 20;
         writeBits+=20;
   
         unio_transfer_append_byte(make8(address, 1), UNIO_APPEND_MAK);
         totalBits += 20;
         writeBits+=20;
         
         unio_transfer_append_byte(make8(address, 0), UNIO_APPEND_MAK);
         totalBits += 20;
         writeBits+=20;
   
         if (valueTx)
         {
            unio_transfer_append_byte(*valueTx, UNIO_APPEND_NOMAK);
            writeBits+=20;
         }
         else  //valueRx
         {
            unio_transfer_append_byte(0xFF, UNIO_APPEND_NOMAK);
            readBits += 20;
         }
         totalBits += 20;
      }
      else
      {
         unio_transfer_append_byte(command, UNIO_APPEND_NOMAK);
         totalBits += 20;
         writeBits+=20;
      }
   }
      
   //left align packed
   for (bits=totalBits; bits<sizeof(g_UNIOUnpackedMsg)*8; bits++)
      shift_left(&g_UNIOUnpackedMsg[0], sizeof(g_UNIOUnpackedMsg), 0);
   
   //standby pulse
   output_float(PIN_UNIO_TRANSFER);
   delay_us(610);
   output_drive(PIN_UNIO_TRANSFER);
   delay_us(5);
   
   if(readBits==0)
   {
      for (bits=0; bits<writeBits-2; bits++)
      {
         if (bit_test(g_UNIOUnpackedMsg[sizeof(g_UNIOUnpackedMsg)-1], 7))
            output_float(PIN_UNIO_TRANSFER);
         else
         {
            output_drive(PIN_UNIO_TRANSFER);
            delay_cycles(1);
         }
   
         delay_us(10);
   
         //output_bit(PIN_UNIO_TRANSFER, shift_left(&packed[0], sizeof(packed), input(PIN_UNIO_TRANSFER)));
         shift_left(&g_UNIOUnpackedMsg[0], sizeof(g_UNIOUnpackedMsg), 0);      
      }
      output_float(PIN_UNIO_TRANSFER);
      for(bits=0;bits<2;bits++)     //SAK
      {
         delay_us(5);
         
         shift_left(&g_UNIOUnpackedMsg[0], sizeof(g_UNIOUnpackedMsg), input(PIN_UNIO_TRANSFER));
   
         delay_us(5);
   
         //output_bit(PIN_UNIO_TRANSFER, shift_left(&packed[0], sizeof(packed), input(PIN_UNIO_TRANSFER)));
         //shift_left(&g_UNIOUnpackedMsg[0], sizeof(g_UNIOUnpackedMsg), input(PIN_UNIO_TRANSFER));
      }
   }
   else
   {
      for (bits=0; bits<writeBits; bits++)
      {
         if (bit_test(g_UNIOUnpackedMsg[sizeof(g_UNIOUnpackedMsg)-1], 7))
            output_float(PIN_UNIO_TRANSFER);
         else
         {
            output_drive(PIN_UNIO_TRANSFER);
            delay_cycles(1);
         }
   
         delay_us(10);
   
         //output_bit(PIN_UNIO_TRANSFER, shift_left(&packed[0], sizeof(packed), input(PIN_UNIO_TRANSFER)));
         shift_left(&g_UNIOUnpackedMsg[0], sizeof(g_UNIOUnpackedMsg), 0);      
      }
      output_float(PIN_UNIO_TRANSFER);
      for(bits=0;bits<readBits-4;bits++)     //read byte
      {
         delay_us(5);
         
         shift_left(&g_UNIOUnpackedMsg[0], sizeof(g_UNIOUnpackedMsg), input(PIN_UNIO_TRANSFER));
   
         delay_us(5);
   
         //output_bit(PIN_UNIO_TRANSFER, shift_left(&packed[0], sizeof(packed), input(PIN_UNIO_TRANSFER)));
         //shift_left(&g_UNIOUnpackedMsg[0], sizeof(g_UNIOUnpackedMsg), input(PIN_UNIO_TRANSFER));
      }
      for(bits=0;bits<2;bits++)     //NOMAK
      {
         if (bit_test(g_UNIOUnpackedMsg[sizeof(g_UNIOUnpackedMsg)-1], 7))
            output_float(PIN_UNIO_TRANSFER);
         else
         {
            output_drive(PIN_UNIO_TRANSFER);
            delay_cycles(1);
         }
   
         delay_us(10);
   
         //output_bit(PIN_UNIO_TRANSFER, shift_left(&packed[0], sizeof(packed), input(PIN_UNIO_TRANSFER)));
         shift_left(&g_UNIOUnpackedMsg[0], sizeof(g_UNIOUnpackedMsg), 0);
      }
      output_float(PIN_UNIO_TRANSFER);
      for(bits=0;bits<2;bits++)     //SAK
      {
         delay_us(5);
         
         shift_left(&g_UNIOUnpackedMsg[0], sizeof(g_UNIOUnpackedMsg), input(PIN_UNIO_TRANSFER));
   
         delay_us(5);
   
         //output_bit(PIN_UNIO_TRANSFER, shift_left(&packed[0], sizeof(packed), input(PIN_UNIO_TRANSFER)));
         //shift_left(&g_UNIOUnpackedMsg[0], sizeof(g_UNIOUnpackedMsg), input(PIN_UNIO_TRANSFER));
      }
   }
   
   // check sak on the last transaction
   // good sak is 01
   if (
         !bit_test(g_UNIOUnpackedMsg[0], 0) ||
         bit_test(g_UNIOUnpackedMsg[0], 1)
      )
   {
      return(FALSE); //fail
   }
   
   if (valueRx || (command == UNIO_CMD_RDSR))
   {
      unsigned int8 ret;
      
      // remove sak
      shift_right(&g_UNIOUnpackedMsg[0], 3, 0);
      shift_right(&g_UNIOUnpackedMsg[0], 3, 0);
      
      // remove mak
      shift_right(&g_UNIOUnpackedMsg[0], 3, 0);
      shift_right(&g_UNIOUnpackedMsg[0], 3, 0);
      
      for (bits=0; bits<8; bits++)
      {
         shift_right(&ret, 1, shift_right(&g_UNIOUnpackedMsg[0], 3, 0));
         shift_right(&g_UNIOUnpackedMsg[0], 3, 0);
      }
      
      *valueRx = ret;
   }
   
   return(TRUE);  //success
}

void init_ext_eeprom(void)
{
   output_low(PIN_UNIO_TRANSFER);
   output_float(PIN_UNIO_TRANSFER);
}

void write_ext_eeprom(EEPROM_ADDRESS addy, unsigned int8 val)
{
   wren_ext_eeprom();
   unio_transfer(UNIO_DEVICE_ADDRESS, UNIO_CMD_WRITE, addy, &val, 0);
   
   delay_ms(5);
}

unsigned int8 read_ext_eeprom(EEPROM_ADDRESS addy)
{
   int8 val;
  
   unio_transfer(UNIO_DEVICE_ADDRESS, UNIO_CMD_READ, addy, 0, &val);
      
   return(val);
}

void wren_ext_eeprom(void)
{
   unio_transfer(UNIO_DEVICE_ADDRESS, UNIO_CMD_WREN, 0, 0, 0);
}

unsigned int8 read_status_ext_eeprom(void)
{
   unsigned int8 val;
   
   unio_transfer(UNIO_DEVICE_ADDRESS, UNIO_CMD_RDSR, 0, 0, &val);
  
   return(val);
}
