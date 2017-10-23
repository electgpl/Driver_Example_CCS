////////////////////////////////////////////////////////////////////////////////
/////                                                                      /////
/////                  Wii_Nunchuck.c - Library Functions                  /////
/////                                                                      /////
////////////////////////////////////////////////////////////////////////////////
/////                                                                      /////
///// This is a basic driver file for a Wii Nunchuck that allows a user to /////
///// access the data from the analog stick, accelerometer, and buttons    /////
///// from a struct in which the decoded data has been stored.             /////
/////                                                                      /////
////////////////////////////////////////////////////////////////////////////////
/////                                                                      /////
///// Pinout for Wii Nunchuck:                                             /////
/////                                                                      /////
/////    _________       Purpose:                   Wire Color:            /////
/////   /         \   1. SDA                        Green                  /////
/////   |  1 2 3  |   2. N.C. / Connection Detect   Copper                 /////
/////   |  -----  |   3. 3.3V                       Red                    /////
/////   |  4 5 6  |   4. GND                        White                  /////
/////   |   ___   |   5. N.C.                                              /////
/////   |__|   |__|   6. SCL                        Yellow                 /////
/////                                                                      /////
/////   *Note*: The wire colors may be different on different versions of  /////
/////           the Wii Nunchuck.  If cutting the plug off to attach a     /////
/////           different header, make sure to check which wire colors     /////
/////           connect to which pins on the pinout above.                 /////
/////                                                                      /////
////////////////////////////////////////////////////////////////////////////////
/////                                                                      /////
///// Wii Nunchuck Data Buffer:                                            /////
/////                                                                      /////
/////    Buffer Postion:     Data:                                         /////
/////       data_buffer[0]      X Analog (8-bits)                          /////
/////       data_buffer[1]      Y Analog (8-bits)                          /////
/////       data_buffer[2]      X Axis (Upper 8-bits of 10-bits)           /////
/////       data_buffer[3]      Y Axis (Upper 8-bits of 10-bits)           /////
/////       data_buffer[4]      Z Axis (Upper 8-bits of 10-bits)           /////
/////       data_buffer[5]      Z Axis (Lower 2-bits of 10-bits)           /////
/////                           Y Axis (Lower 2-bits of 10-bits)           /////
/////                           X Axis (Lower 2-bits of 10-bits)           /////
/////                           C Button (1-bit)                           /////
/////                           Z Button (1-bit)                           /////
/////                                                                      /////
////////////////////////////////////////////////////////////////////////////////
/////                                                                      /////
/////                            User Functions                            /////
/////                                                                      /////
///// wii_nunchuck_init():                                                 /////
/////    This function initializes the Wii Nunchuck.                       /////
/////                                                                      /////
///// wii_nunchuck_data():                                                 /////
/////    This function reads the data buffer on the Wii Nunchuck to        /////
/////    retrieve the values from the analog stick, accelerometer, and     /////
/////    buttons.  The values are then decoded and stored in a struct.     /////
/////                                                                      /////
/////    *Note*: The wii_nunchuck_data() function needs to be called in    /////
/////            order to read new data from the Wii Nunchuck and update   /////
/////            the values stored in the struct.                          /////
/////                                                                      /////
///// wii_nunchuck_address():                                              /////
/////    This function can be used to verify that the predefined addresses /////
/////    are correct for the version of the Wii Nunchuck being used if the /////
/////    device is unresponsive after being connected to the system.  This /////
/////    function runs through a range of valid addresses checking for a   /////
/////    set that returns an ACK, that address is returned from the        /////
/////    function.  The returned address is the write address, adding one  /////
/////    to this address is the read address.                              /////
/////                                                                      /////
////////////////////////////////////////////////////////////////////////////////
/////                                                                      /////
/////          (C) Copyright 1996, 2003 Custom Computer Services           /////
/////                                                                      /////
///// This source code may only be used by licensed users of the CCS C     /////
///// compiler.  This source code may only be distributed to other or      /////
///// licensed users of the CCS C compiler.  No other use, reproduction    /////
///// distribution is permitted without written permission.  Derivative    /////
///// programs created using this software in object code form are not     /////
///// restricted in any way.                                               /////
/////                                                                      /////
////////////////////////////////////////////////////////////////////////////////

// Define Wii Nunchuck Read and Write Addresses
#define WII_NUNCHUCK_WRITE  0xA4
#define WII_NUNCHUCK_READ   0xA5

// Define I2C Clock and Data Pins
#ifndef WII_SCL
   #if defined(__PCD__)
      #define WII_SCL  PIN_G2
      #define WII_SDA  PIN_G3
   #else
      #define WII_SCL  PIN_C3
      #define WII_SDA  PIN_C4
   #endif
#endif

// Setup I2C
#use I2C(MASTER, SCL = WII_SCL, SDA = WII_SDA)

// Structure that Contains Decoded Wii Nunchuck Data
struct wii_struct{
   unsigned int8 x_analog;
   unsigned int8 y_analog;
   unsigned int16 x_axis;
   unsigned int16 y_axis;
   unsigned int16 z_axis;
   unsigned int1 c_button;
   unsigned int1 z_button;
} wii_data;

// Temporary Buffer to Hold Data Read from Wii Nunchuck
unsigned int8 data_buffer[6];

// Function Prototypes
void wii_nunchuck_init(void);
void wii_nunchuck_data(void);
unsigned int8 wii_nunchuck_address(void);
unsigned int8 decode_data(unsigned int8 data);
void decode_buffer(void);
void update_data(void);
void update_x_analog(void);
void update_y_analog(void);
void update_x_axis(void);
void update_y_axis(void);
void update_z_axis(void);
void update_c_button(void);
void update_z_button(void);

void wii_nunchuck_init(void) {
   output_high(WII_SCL);
   output_high(WII_SDA);
   
   i2c_start();
   i2c_write(WII_NUNCHUCK_WRITE);
   i2c_write(0x40);
   i2c_write(0x00);
   i2c_stop();
   
   wii_nunchuck_data();
}

void wii_nunchuck_data(void) {
   i2c_start();
   i2c_write(WII_NUNCHUCK_WRITE);
   i2c_write(0x00);
   i2c_stop();
   
   delay_ms(1);
   
   i2c_start();
   i2c_write(WII_NUNCHUCK_READ);
   data_buffer[0] = i2c_read();
   data_buffer[1] = i2c_read();
   data_buffer[2] = i2c_read();
   data_buffer[3] = i2c_read();
   data_buffer[4] = i2c_read();
   data_buffer[5] = i2c_read(0x00);
   i2c_stop();
   
   decode_buffer();
   
   update_data();
}

unsigned int8 wii_nunchuck_address(void) {
   unsigned int8 address, result;
   
   for(address = 0x10; address < 0xF0; address += 2) {
      i2c_start();
      result = i2c_write(address);
      i2c_stop();
      
      if(!result) {
         return address;
      }
   }
}

unsigned int8 decode_data(unsigned int8 data) {
   return ((data ^ 0x17) + 0x17);
}

void decode_buffer(void) {
   data_buffer[0] = decode_data(data_buffer[0]);
   data_buffer[1] = decode_data(data_buffer[1]);
   data_buffer[2] = decode_data(data_buffer[2]);
   data_buffer[3] = decode_data(data_buffer[3]);
   data_buffer[4] = decode_data(data_buffer[4]);
   data_buffer[5] = decode_data(data_buffer[5]);
}

void update_data(void) {
   update_x_analog();
   update_y_analog();
   update_x_axis();
   update_y_axis();
   update_z_axis();
   update_c_button();
   update_z_button();
}

void update_x_analog(void) {
   wii_data.x_analog = data_buffer[0];
}

void update_y_analog(void) {
   wii_data.y_analog = data_buffer[1];
}

void update_x_axis(void) {
   wii_data.x_axis = ((unsigned int16)data_buffer[2] << 2)
                        | ((data_buffer[5] & 0x0C) >> 2);
}

void update_y_axis(void) {
   wii_data.y_axis = ((unsigned int16)data_buffer[3] << 2)
                        | ((data_buffer[5] & 0x30) >> 4);
}

void update_z_axis(void) {
   wii_data.z_axis = ((unsigned int16)data_buffer[4] << 2)
                        | ((data_buffer[5] & 0xC0) >> 6);
}

void update_c_button(void) {
   wii_data.c_button = ~((data_buffer[5] & 0x02) >> 1);
}

void update_z_button(void) {
   wii_data.z_button = ~(data_buffer[5] & 0x01);
}
