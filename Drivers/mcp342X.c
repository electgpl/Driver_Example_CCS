////////////// Driver for MCP342X Delta-Sigma A/D Converter /////////////
////                                                                 ////
////  Driver for Microchip's MCP3421, MCP3422, MCP3423, MCP3424,     ////
////  MCP3425, MCP3426, MCP3427 and MCP3428 Delta-Sigma A/D          ////
////  Converters.                                                    ////
////                                                                 ////
////  adc_init() - initializes the MCP342X, call after power up      ////
////                                                                 ////
////  set_adc_channel_mcp() - sets the channel of MCP342X to perform ////
////                          conversion on.  Only use for devices   ////
////                          with multiple channels.                ////
////                                                                 ////
////  read_adc_mcp() - initiates a conversion and reads adc,         ////
////                   returns raw reading.                          ////
////                                                                 ////
////  read_adc_value_mcp() - initiates a conversion and read         ////
////                         adc, returns voltage as float           ////
////                         (-2.048 to 2.048).                      ////
////                                                                 ////
////  mcp_latch_address() - causes all MCP342X device on bus to      ////
////                        latch their external address pins. Only  ////
////                        used for devices with external address   ////
////                        pins.                                    ////
////                                                                 ////
////  Defines used to setup MCP342X:                                 ////
////                                                                 ////
////    MCP342X_SCL  - specifies the I2C clock pin to use, default   ////
////                   if not specified is PIN_C3.                   ////
////                                                                 ////
////    MCP342X_SDA  - specifies the I2C data pin to use, default if ////
////                   not specified is PIN_C4.                      ////
////                                                                 ////
////    MCP342X_MODE - specifies the mode to use, default if not     ////
////                   specified is MCP342X_CONTINUOUS.  Valid       ////
////                   options are:                                  ////
////                     MCP342X_CONTINUOUS                          ////
////                     MCP342X_ONE_SHOT                            ////
////                                                                 ////
////    MCP342X_BITS - specifies the number of conversion bits to    ////
////                   use, default if not specified is              ////
////                   MCP342X_16BITS.  Valid options are:           ////
////                     MCP342X_18BITS                              ////
////                     MCP342X_16BITS                              ////
////                     MCP342X_14BITS                              ////
////                     MCP342X_12BITS                              ////
////                                                                 ////
////    MCP342X_GAIN - specifies the PGA gain to use, default if not ////
////                   specified is MCP342X_1X_GAIN.  Valid options  ////
////                   are:                                          ////
////                     MCP342X_8X_GAIN                             ////
////                     MCP342X_4X_GAIN                             ////
////                     MCP342X_2X_GAIN                             ////
////                     MCP342X_1X_GAIN                             ////
////                                                                 ////
////    MCP342X_ADDRESS - specifies the address of MCP342X devices,  ////
////                      default if not specified is 0.  Valid      ////
////                      options are 0-7.                           ////
////                                                                 ////
////    MCP342X_CHANNEL - specifies the default channel to           ////
////                      initialize MCP342X to.                     ////
////                                                                 ////
////    USE_HW_I2C - specifies that the MCP342X_SCL and MCP342X_SDA  ////
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

#define MCP342X_CONTINUOUS 0x10
#define MCP342X_ONE_SHOT   0x00

#define MCP342X_18BITS     0x0C
#define MCP342X_16BITS     0x08
#define MCP342X_14BITS     0x04
#define MCP342X_12BITS     0x00

#define MCP342X_8X_GAIN    0x03
#define MCP342X_4X_GAIN    0x02
#define MCP342X_2X_GAIN    0x01
#define MCP342X_1X_GAIN    0x00

#define MCP342X_DEVICE_CODE       0xD0
#define MCP342X_START_CONVERSTION 0x80

#define MCP342X_GENERAL_CALL_ADDRESS   0x00
#define MCP342X_GENERAL_CALL_LATCH     0x04

#ifndef MCP342X_SCL
 #define MCP342X_SCL  PIN_C3
#endif

#ifndef MCP342X_SDA
 #define MCP342X_SDA  PIN_C4
#endif

#ifndef MCP342X_MODE
 #define MCP342X_MODE MCP342X_CONTINUOUS
#endif

#ifndef MCP342X_BITS
 #define MCP342X_BITS MCP342X_16BITS
#endif

#ifndef MCP342X_GAIN
 #define MCP342X_GAIN MCP342X_1X_GAIN
#endif

#ifndef MCP342X_ADDRESS
 #define MCP342X_ADDRESS 0
#endif

#ifndef MCP342X_CHANNEL
 #define MCP342X_CHANNEL 0
#endif

#ifdef USE_HW_I2C
 #use i2c(MASTER, FAST, SCL=MCP342X_SCL, SDA=MCP342X_SDA, stream=MCP342X_STREAM, FORCE_HW)
#else
 #use i2c(MASTER, FAST, SCL=MCP342X_SCL, SDA=MCP342X_SDA, stream=MCP342X_STREAM, FORCE_SW)
#endif

//////////////////////////////////////////////////////////////////////////////////
// adc_init()
// Purpose: To initialize the MCP342X.
// Parameters: address - Optional parameter for specifying the address of the
//                       MCP342X to initialize.  Allows for initializing multiple
//                       devices on same bus.  Driver only supports one device
//                       configuration.  Defaults to MCP342X_ADDRESS if not
//                       specified.
// Returns:    Nothing.
//////////////////////////////////////////////////////////////////////////////////
void adc_init(unsigned int8 address=MCP342X_ADDRESS)
{
  i2c_start(MCP342X_STREAM);  //send I2C start
  i2c_write(MCP342X_STREAM, MCP342X_DEVICE_CODE | (address << 1));  //send write command
  i2c_write(MCP342X_STREAM, MCP342X_MODE | MCP342X_BITS | MCP342X_GAIN | (MCP342X_CHANNEL << 5));  //send device configuration
  i2c_stop(MCP342X_STREAM);  //send I2C stop
}

//////////////////////////////////////////////////////////////////////////////////
// set_adc_channel_mcp()
// Purpose: To set the channel the MCP342X performs A/D Conversion on.  Only
//          used for MCP3422, MCP3423, MCP3424, MCP3426, MCP3427 and MCP3428
//          devices.
// Parameters: channel - Channel to set MCP342X to.
//             address - Optional parameter for specifying the address of the 
//                       MCP342X to set.  Allows for using multiple devices on 
//                       same bus.  Defaults to MCP342X_ADDRESS if not specified.
// Returns:    Nothing.
//////////////////////////////////////////////////////////////////////////////////
void set_adc_channel_mcp(unsigned int8 channel, unsigned int8 address=MCP342X_ADDRESS)
{
  i2c_start(MCP342X_STREAM);  //send I2C start
  i2c_write(MCP342X_STREAM, MCP342X_DEVICE_CODE | (address << 1));  //send write command
  i2c_write(MCP342X_STREAM, MCP342X_MODE | MCP342X_BITS | MCP342X_GAIN | (channel << 5));  //send device configuration
  i2c_stop(MCP342X_STREAM);  //send I2C stop
}

//////////////////////////////////////////////////////////////////////////////////
// read_adc_mcp()
// Purpose: To read the last adc conversion from device, raw value read from
//          device. If device configured for One-Shot mode, it will initiate the
//          conversion.  Function will wait for a new conversion before returning.
// Parameters: address - Optional parameter for specifying the address of the
//                       MCP342X to read.  Allows for reading multiple devices on
//                       same bus.  Defaults to MCP342X_ADDRESS if not specified.
// Returns:   signed int32 or signed int16 value depending MCP342X_BITS value.
//////////////////////////////////////////////////////////////////////////////////
#if MCP342X_BITS == MCP342X_18BITS
signed int32 read_adc_mcp(unsigned int8 address=MCP342X_ADDRESS)
#else
signed int16 read_adc_mcp(unsigned int8 address=MCP342X_ADDRESS)
#endif
{
  union
  {
   #if MCP342X_BITS == MCP342X_18BITS
    signed int32 sint32;
    unsigned int8 b[4];
   #else
    signed int16 sint16;
    unsigned int8 b[2];
   #endif
  } result;
  unsigned int8 status = 0x80;

  #if MCP342X_MODE == MCP342X_ONE_SHOT
   i2c_start(MCP342X_STREAM);  //send I2C start
   i2c_write(MCP342X_STREAM, MCP342X_DEVICE_CODE | (address << 1));  //send write command
   i2c_write(MCP342X_STREAM, MCP342X_START_CONVERSTION | MCP342X_MODE | MCP342X_BITS | MCP342X_GAIN);  //initiate conversion
   i2c_stop(MCP342X_STREAM);  //send I2C stop
  #endif

   i2c_start(MCP342X_STREAM);  //send I2C start
   i2c_write(MCP342X_STREAM, MCP342X_DEVICE_CODE | (address << 1) | 1);  //send read command

  #if MCP342X_BITS == MCP342X_18BITS
   result.b[2] = i2c_read(MCP342X_STREAM, 1);  //read MSB 18 Bit mode
  #endif
   result.b[1] = i2c_read(MCP342X_STREAM, 1);  //read 2nd MSB 18 Bit mode, read MSB 16, 14 or 12 Bit mode
   result.b[0] = i2c_read(MCP342X_STREAM, 1);  //read LSB
   status = i2c_read(MCP342X_STREAM, 1);       //read Status

   if(bit_test(status,7))  //if RDY = 1, New conversion not ready
   {
     do
     {
       status = i2c_read(MCP342X_STREAM, 1);  //read Status
     } while(bit_test(status, 7)); //until RDY = 0

     status = i2c_read(MCP342X_STREAM, 0);  //read Status, do nack
     i2c_stop();  //send I2C stop

     i2c_start(MCP342X_STREAM);  //send I2C start
     i2c_write(MCP342X_STREAM, MCP342X_DEVICE_CODE | (address << 1) | 1);  //send read command

    #if MCP342X_BITS == MCP342X_18BITS
     result.b[2] = i2c_read(MCP342X_STREAM, 1);  //read MSB 18 Bit mode
    #endif
     result.b[1] = i2c_read(MCP342X_STREAM, 1);  //read 2nd MSB 18 Bit mode, read MSB 16, 14 or 12 Bit mode
     result.b[0] = i2c_read(MCP342X_STREAM, 1);  //read LSB
   }

   status = i2c_read(MCP342X_STREAM, 0);  //read Status, do nack
   i2c_stop();  //send I2C stop
   
  #if MCP342X_BITS == MCP342X_18BITS
   if(bit_test(result.b[2],1))  //if 18 Bit mode check sign bit
     result.b[3] = 0xFF;
   else
     result.b[3] = 0;
  #endif

  #if MCP342X_BITS == MCP342X_18BITS
   return(result.sint32);
  #else
   return(result.sint16);
  #endif
}

//////////////////////////////////////////////////////////////////////////////////
// read_adc_volts_mcp()
// Purpose: To read the last adc conversion from device, actual volt value read
//          from device.
// Parameters: address - Optional parameter for specifying the address of the
//                       MCP342X to read.  Allows for reading multiple devices on
//                       same bus.  Defaults to MCP342X_ADDRESS if not specified.
// Returns:   float32
//////////////////////////////////////////////////////////////////////////////////
float32 read_adc_volts_mcp(unsigned int8 address=MCP342X_ADDRESS)
{
 #if MCP342X_BITS == MCP342X_18BITS
  signed int32 result;
 #else
  signed int16 result;
 #endif

  float32 fresult;

  result = read_adc_mcp(address);

  #if MCP342X_BITS == MCP342X_12BITS
   fresult = (float32)result * 0.001;
  #elif MCP342X_BITS == MCP342X_14BITS
   fresult = (float32)result * 0.00025;
  #elif MCP342X_BITS == MCP342X_16BITS
   fresult = (float32)result * 0.0000625;
  #else
   fresult = (float32)result * 0.000015625;
  #endif

  #if MCP342X_GAIN == MCP342X_8X_GAIN
   fresult /= 8;
  #elif MCP342X_GAIN == MCP342X_4X_GAIN
   fresult /= 4;
  #elif MCP342X_gain == MCP342X_2X_GAIN
   fresult /= 2;
  #endif

   return(fresult);
}

//////////////////////////////////////////////////////////////////////////////////
// mcp_latch_address()
// Purpose: To latch the logic of the external address selection pins.  Only used
//          for MCP3423, MCP3424, MCP3427 and MCP3428 devices.  Latch is performed
//          automatically on power-up.  This function can be used incase Vdd rises
//          slowly and you want to ensure that the pins are latched when Vdd is 
//          stable.  Calling this function will cause all MCP342X device on bus to
//          latch their address pins.
// Parameters: None
// Returns:    Nothing
//////////////////////////////////////////////////////////////////////////////////
void mcp_latch_address(void)
{
   i2c_start(MCP342X_STREAM);
   i2c_write(MCP342X_GENERAL_CALL_ADDRESS);
   i2c_write(MCP342X_GENERAL_CALL_LATCH);
   i2c_stop(MCP342X_STREAM);
}
