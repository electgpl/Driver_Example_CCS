//////////////////// Driver for MCP3421 A/D Converter ///////////////////
////                                                                 ////
////  adc_init() - initializes the MCP3421, call after power up      ////
////                                                                 ////
////  read_adc_mcp3421() - initiates a conversion and reads adc,     ////
////                       returns raw reading.                      ////
////                                                                 ////
////  read_adc_value_mcp3421() - initiates a conversion and read     ////
////                             adc, returns voltage as float       ////
////                             (-2.048 to 2.048).                  ////
////                                                                 ////
////  Defines used to setup MCP3421:                                 ////
////                                                                 ////
////    MCP3421_SCL  - specifies the I2C clock pin to use, default   ////
////                   if not specified is PIN_C3.                   ////
////                                                                 ////
////    MCP3421_SDA  - specifies the I2C data pin to use, default if ////
////                   not specified is PIN_C4.                      ////
////                                                                 ////
////    MCP3421_MODE - specifies the mode to use, default if not     ////
////                   specified is MCP3421_CONTINUOUS.  Valid       ////
////                   options are:                                  ////
////                     MCP3421_CONTINUOUS                          ////
////                     MCP3421_ONE_SHOT                            ////
////                                                                 ////
////    MCP3421_BITS - specifies the number of conversion bits to    ////
////                   use, default if not specified is              ////
////                   MCP3421_18BITS.  Valid options are:           ////
////                     MCP3421_18BITS                              ////
////                     MCP3421_16BITS                              ////
////                     MCP3421_14BITS                              ////
////                     MCP3421_12BITS                              ////
////                                                                 ////
////    MCP3421_GAIN - specifies the PGA gain to use, default if not ////
////                   specified is MCP3421_1X_GAIN.  Valid options  ////
////                   are:                                          ////
////                     MCP3421_8X_GAIN                             ////
////                     MCP3421_4X_GAIN                             ////
////                     MCP3421_2X_GAIN                             ////
////                     MCP3421_1X_GAIN                             ////
////                                                                 ////
////    MCP3421_ADDRESS - specifies the address of MCP3421 devices,  ////
////                      default if not specified is 0.  Valid      ////
////                      options are 0-7.                           ////
////                                                                 ////
////    USE_HW_I2C - specifies that the MCP3421_SCL and MCP3421_SDA  ////
////                 pins are HW I2C pins and to use PIC's HW I2C    ////
////                 peripheral.                                     ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2013 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#define MCP3421_CONTINUOUS 0x10
#define MCP3421_ONE_SHOT   0x00

#define MCP3421_18BITS     0x0C
#define MCP3421_16BITS     0x08
#define MCP3421_14BITS     0x04
#define MCP3421_12BITS     0x00

#define MCP3421_8X_GAIN    0x03
#define MCP3421_4X_GAIN    0x02
#define MCP3421_2X_GAIN    0x01
#define MCP3421_1X_GAIN    0x00

#define MCP3421_DEVICE_CODE       0xD0
#define MCP3421_START_CONVERSTION 0x80

#ifndef MCP3421_SCL
 #define MCP3421_SCL  PIN_C3
#endif

#ifndef MCP3421_SDA
 #define MCP3421_SDA  PIN_C4
#endif

#ifndef MCP3421_MODE
 #define MCP3421_MODE MCP3421_CONTINUOUS
#endif

#ifndef MCP3421_BITS
 #define MCP3421_BITS MCP3421_18BITS
#endif

#ifndef MCP3421_GAIN
 #define MCP3421_GAIN MCP3421_1X_GAIN
#endif

#ifndef MCP3421_ADDRESS
 #define MCP3421_ADDRESS 0
#endif

#ifdef USE_HW_I2C
 #use i2c(MASTER, FAST, SCL=MCP3421_SCL, SDA=MCP3421_SDA, stream=MCP3421_STREAM, FORCE_HW)
#else
 #use i2c(MASTER, FAST, SCL=MCP3421_SCL, SDA=MCP3421_SDA, stream=MCP3421_STREAM, FORCE_SW)
#endif

//////////////////////////////////////////////////////////////////////////////////
// adc_init()
// Purpose: To initialize the MCP3421.
// Parameters: address - Optional parameter for specifying the address of the
//                       MCP3421 to initialize.  Allows for initializing multiple
//                       devices on same bus.  Driver only supports one device
//                       configuration.  Defaults to MCP3421_ADDRESS if not
//                       specified.
// Returns:    Nothing.
//////////////////////////////////////////////////////////////////////////////////
void adc_init(unsigned int8 address=MCP3421_ADDRESS)
{
  i2c_start(MCP3421_STREAM);  //send I2C start
  i2c_write(MCP3421_STREAM, MCP3421_DEVICE_CODE | (address << 1));  //send write command
  i2c_write(MCP3421_STREAM, MCP3421_MODE | MCP3421_BITS | MCP3421_GAIN);  //send device configuration
  i2c_stop(MCP3421_STREAM);  //send I2C stop
}

//////////////////////////////////////////////////////////////////////////////////
// read_adc_mcp3421()
// Purpose: To read the last adc conversion from device, raw value read from
//          device. If device configured for One-Shot mode, it will initiate the
//          conversion.  Function will wait for a new conversion before returning.
// Parameter: address - Optional parameter for specifying the address of the
//                      MCP3421 to read.  Allows for reading multiple devices on
//                      same bus.  Defaults to MCP3421_ADDRESS if not specified.
// Returns:   signed int32 or signed int16 value depending MCP3421_BITS value.
//////////////////////////////////////////////////////////////////////////////////
#if MCP3421_BITS == MCP3421_18BITS
signed int32 read_adc_mcp3421(unsigned int8 address=MCP3421_ADDRESS)
#else
signed int16 read_adc_mcp3421(unsigned int8 address=MCP3421_ADDRESS)
#endif
{
  union
  {
   #if MCP3421_BITS == MCP3421_18BITS
    signed int32 sint32;
    unsigned int8 b[4];
   #else
    signed int16 sint16;
    unsigned int8 b[2];
   #endif
  } result;
  unsigned int8 status = 0x80;

  #if MCP3421_MODE == MCP3421_ONE_SHOT
   i2c_start(MCP3421_STREAM);  //send I2C start
   i2c_write(MCP3421_STREAM, MCP3421_DEVICE_CODE | (address << 1));  //send write command
   i2c_write(MCP3421_STREAM, MCP3421_START_CONVERSTION | MCP3421_MODE | MCP3421_BITS | MCP3421_GAIN);  //initiate conversion
   i2c_stop(MCP3421_STREAM);  //send I2C stop
  #endif

   i2c_start(MCP3421_STREAM);  //send I2C start
   i2c_write(MCP3421_STREAM, MCP3421_DEVICE_CODE | (address << 1) | 1);  //send read command

  #if MCP3421_BITS == MCP3421_18BITS
   result.b[2] = i2c_read(MCP3421_STREAM, 1);  //read MSB 18 Bit mode
  #endif
   result.b[1] = i2c_read(MCP3421_STREAM, 1);  //read 2nd MSB 18 Bit mode, read MSB 16, 14 or 12 Bit mode
   result.b[0] = i2c_read(MCP3421_STREAM, 1);  //read LSB
   status = i2c_read(MCP3421_STREAM, 1);       //read Status

   if(bit_test(status,7))  //if RDY = 1, New conversion not ready
   {
     do
     {
       status = i2c_read(MCP3421_STREAM, 1);  //read Status
     } while(bit_test(status, 7)); //until RDY = 0

     status = i2c_read(MCP3421_STREAM, 0);  //read Status, do nack
     i2c_stop();  //send I2C stop

     i2c_start(MCP3421_STREAM);  //send I2C start
     i2c_write(MCP3421_STREAM, MCP3421_DEVICE_CODE | (address << 1) | 1);  //send read command

    #if MCP3421_BITS == MCP3421_18BITS
     result.b[2] = i2c_read(MCP3421_STREAM, 1);  //read MSB 18 Bit mode
    #endif
     result.b[1] = i2c_read(MCP3421_STREAM, 1);  //read 2nd MSB 18 Bit mode, read MSB 16, 14 or 12 Bit mode
     result.b[0] = i2c_read(MCP3421_STREAM, 1);  //read LSB
   }

   status = i2c_read(MCP3421_STREAM, 0);  //read Status, do nack
   i2c_stop();  //send I2C stop

  #if MCP3421_BITS == MCP3421_18BITS
   if(bit_test(result.b[2],1))  //if 18 Bit mode check sign bit
     result.b[3] = 0xFF;
   else
     result.b[3] = 0;
  #endif

  #if MCP3421_BITS == MCP3421_18BITS
   return(result.sint32);
  #else
   return(result.sint16);
  #endif
}

//////////////////////////////////////////////////////////////////////////////////
// read_adc_volts_mcp3421()
// Purpose: To read the last adc conversion from device, actual volt value read
//          from device.
// Parameter: address - Optional parameter for specifying the address of the
//                      MCP3421 to read.  Allows for reading multiple devices on
//                      same bus.  Defaults to MCP3421_ADDRESS if not specified.
// Returns:   float32
//////////////////////////////////////////////////////////////////////////////////
float32 read_adc_volts_mcp3421(unsigned int8 address=MCP3421_ADDRESS)
{
  #if MCP3421_BITS == MCP3421_18BITS
   signed int32 result;
  #else
   signed int16 result;
  #endif

  float32 fresult;

  result = read_adc_mcp3421(address);

  #if MCP3421_BITS == MCP3421_12BITS
   fresult = (float32)result * 0.001;
  #elif MCP3421_BITS == MCP3421_14BITS
   fresult = (float32)result * 0.00025;
  #elif MCP3421_BITS == MCP3421_16BITS
   fresult = (float32)result * 0.0000625;
  #else
   fresult = (float32)result * 0.000015625;
  #endif

  #if MCP3421_GAIN == MCP3421_8X_GAIN
   fresult /= 8;
  #elif MCP3421_GAIN == MCP3421_4X_GAIN
   fresult /= 4;
  #elif MCP3421_gain == MCP3421_2X_GAIN
   fresult /= 2;
  #endif

   return(fresult);
}
