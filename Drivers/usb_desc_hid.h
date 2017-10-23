///////////////////////////////////////////////////////////////////////////
///                          usb_desc_hid.h                            ////
////                                                                   ////
//// An example set of device / configuration descriptors for use with ////
//// CCS's HID Demo example (see ex_usb_hid.c)                         ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////                                                                   ////
//// Version History:                                                  ////
////                                                                   ////
//// September 9th, 2010:                                              ////
////  USB_CONFIG_HID_TX_SIZE and USB_CONFIG_HID_RX_SIZE were backwards ////
////     in HID report descriptor of usb_desc_hid.h                    ////
////                                                                   ////
//// March 5th, 2009:                                                  ////
////   Cleanup for Wizard.                                             ////
////   PIC24 Initial release.                                          ////
////                                                                   ////
//// June 20th, 2005:                                                  ////
////   PIC18Fxx5x initial release                                      ////
////                                                                   ////
//// March 21st, 2005:                                                 ////
////   EP 0x01 and EP 0x81 now use USB_EP1_TX_SIZE and USB_EP1_RX_SIZE ////
////      to define max packet size, to make it easier for dynamically ////
////      changed code.                                                ////
////   EP 0x01 and EP 0x81 will now use 1ms polling interval if using  ////
////      a full speed device.                                         ////
////                                                                   ////
//// May 4th, 2004: Optimization and cleanup.                          ////
////                Some definitions may have changed.                 ////
////                                                                   ////
//// May 6th, 2003: Fixed non-HID descriptors pointing to faulty       ////
////                strings                                            ////
////                                                                   ////
//// August 2nd, 2002: Initial Public Release                          ////
////                                                                   ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2005 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS    ////
//// C compiler.  This source code may only be distributed to other    ////
//// licensed users of the CCS C compiler.  No other use,              ////
//// reproduction or distribution is permitted without written         ////
//// permission.  Derivative programs created using this software      ////
//// in object code form are not restricted in any way.                ////
///////////////////////////////////////////////////////////////////////////

#IFNDEF __USB_DESCRIPTORS__
#DEFINE __USB_DESCRIPTORS__

#ifndef USB_CONFIG_PID
   #define USB_CONFIG_PID  0x0020
#endif

#ifndef USB_CONFIG_VID
   #define  USB_CONFIG_VID 0x0461
#endif

#ifndef USB_CONFIG_BUS_POWER
   //valid range is 0..500
   #define  USB_CONFIG_BUS_POWER 100   //100mA
#endif

#ifndef USB_CONFIG_VERSION
   //version number that is stored into descriptor, in bcd.
   //range is 00.00 to 99.99
   #define  USB_CONFIG_VERSION   0x0100      //01.00
#endif

#ifndef USB_CONFIG_HID_TX_SIZE
   //valid range is 0-255
   #define USB_CONFIG_HID_TX_SIZE   2     //compatible with hiddemo.exe
#endif

#ifndef USB_CONFIG_HID_RX_SIZE
   //valid range is 0-255
   #define USB_CONFIG_HID_RX_SIZE   2     //compatible with hiddemo.exe
#endif

#ifndef USB_CONFIG_HID_TX_POLL
   // for full speed devices, valid range is 1-255
   // for slow speed devices, valid range is 10-255
   #define USB_CONFIG_HID_TX_POLL   10
#endif

#ifndef USB_CONFIG_HID_RX_POLL
   // for full speed devices, valid range is 1-255
   // for slow speed devices, valid range is 10-255
   #define USB_CONFIG_HID_RX_POLL   10
#endif

//Tells the CCS PIC USB firmware to include HID handling code.
#ifdef USB_HID_DEVICE
#undef USB_HID_DEVICE
#endif

#DEFINE USB_HID_DEVICE  TRUE

//the following defines needed for the CCS USB PIC driver to enable the TX endpoint 1
// and allocate buffer space on the peripheral
#ifdef USB_EP1_TX_ENABLE
#undef USB_EP1_TX_ENABLE
#endif
#define USB_EP1_TX_ENABLE  USB_ENABLE_INTERRUPT   //turn on EP1 for IN bulk/interrupt transfers

#define USB_HID_ENDPOINT   1

#ifndef USB_EP1_TX_SIZE
 #if (USB_CONFIG_HID_TX_SIZE >= 64)
   // interrupt endpoint max packet size is 64.
   #define USB_EP1_TX_SIZE    64
 #else
   // by making EP packet size larger than message size, we can send message in one packet.
   #define USB_EP1_TX_SIZE    (USB_CONFIG_HID_TX_SIZE+1)
 #endif
#endif

#ifdef USB_EP1_RX_ENABLE
#undef USB_EP1_RX_ENABLE
#endif
#define USB_EP1_RX_ENABLE  USB_ENABLE_INTERRUPT   //turn on EP1 for OUT bulk/interrupt transfers

#ifndef USB_EP1_RX_SIZE
 #if (USB_CONFIG_HID_RX_SIZE >= 64)
   // interrupt endpoint max packet size is 64.
   #define USB_EP1_RX_SIZE    64
 #else
   // by making EP packet size larger than message size, we can send message in one packet.
   #define USB_EP1_RX_SIZE    (USB_CONFIG_HID_RX_SIZE+1)
 #endif
#endif

#include <usb.h>

   //////////////////////////////////////////////////////////////////
   ///
   ///  HID Report.  Tells HID driver how to handle and deal with
   ///  received data.  HID Reports can be extremely complex,
   ///  see HID specifcation for help on writing your own.
   ///
   ///  CCS example uses a vendor specified usage, that sends and
   ///  receives 2 absolute bytes ranging from 0 to 0xFF.
   ///
   //////////////////////////////////////////////////////////////////

   const char USB_CLASS_SPECIFIC_DESC[] = 
   {
      6, 0, 255,        // Usage Page = Vendor Defined
      9, 1,             // Usage = IO device
      0xa1, 1,          // Collection = Application
      0x19, 1,          // Usage minimum
      0x29, 8,          // Usage maximum

      0x15, 0x80,       // Logical minimum (-128)
      0x25, 0x7F,       // Logical maximum (127)

      0x75, 8,          // Report size = 8 (bits)
      0x95, USB_CONFIG_HID_TX_SIZE,        // Report count = 16 bits (2 bytes)
      0x81, 2,          // Input (Data, Var, Abs)
      0x19, 1,          // Usage minimum
      0x29, 8,          // Usage maximum
      0x75, 8,          // Report size = 8 (bits)
      0x95, USB_CONFIG_HID_RX_SIZE,        // Report count = 16 bits (2 bytes)
      0x91, 2,          // Output (Data, Var, Abs)
      0xc0              // End Collection
   };

   //if a class has an extra descriptor not part of the config descriptor,
   // this lookup table defines where to look for it in the const
   // USB_CLASS_SPECIFIC_DESC[] array.
   //first element is the config number (if your device has more than one config)
   //second element is which interface number
   //set element to 0xFFFF if this config/interface combo doesn't exist
   const int8 USB_CLASS_SPECIFIC_DESC_LOOKUP[USB_NUM_CONFIGURATIONS][1] =
   {
   //config 1
      //interface 0
         0
   };

   //if a class has an extra descriptor not part of the config descriptor,
   // this lookup table defines the size of that descriptor.
   //first element is the config number (if your device has more than one config)
   //second element is which interface number
   //set element to 0xFFFF if this config/interface combo doesn't exist
   const int8 USB_CLASS_SPECIFIC_DESC_LOOKUP_SIZE[USB_NUM_CONFIGURATIONS][1] =
   {
   //config 1
      //interface 0
         32
   };


//////////////////////////////////////////////////////////////////
///
///   start config descriptor
///   right now we only support one configuration descriptor.
///   the config, interface, class, and endpoint goes into this array.
///
//////////////////////////////////////////////////////////////////

   #DEFINE USB_TOTAL_CONFIG_LEN      41  //config+interface+class+endpoint+endpoint (2 endpoints)

   const char USB_CONFIG_DESC[] = {
   //IN ORDER TO COMPLY WITH WINDOWS HOSTS, THE ORDER OF THIS ARRAY MUST BE:
      //    config(s)
      //    interface(s)
      //    class(es)
      //    endpoint(s)

   //config_descriptor for config index 1
         USB_DESC_CONFIG_LEN, //length of descriptor size          ==1
         USB_DESC_CONFIG_TYPE, //constant CONFIGURATION (CONFIGURATION 0x02)     ==2
         USB_TOTAL_CONFIG_LEN,0, //size of all data returned for this config      ==3,4
         1, //number of interfaces this device supports       ==5
         0x01, //identifier for this configuration.  (IF we had more than one configurations)      ==6
         0x00, //index of string descriptor for this configuration      ==7
        #if USB_CONFIG_BUS_POWER
         0x80, //bit 6=1 if self powered, bit 5=1 if supports remote wakeup (we don't), bits 0-4 unused and bit7=1         ==8
        #else
         0xC0, //bit 6=1 if self powered, bit 5=1 if supports remote wakeup (we don't), bits 0-4 unused and bit7=1         ==8
        #endif
         USB_CONFIG_BUS_POWER/2, //maximum bus power required (maximum milliamperes/2)  (0x32 = 100mA)

   //interface descriptor 1
         USB_DESC_INTERFACE_LEN, //length of descriptor      =10
         USB_DESC_INTERFACE_TYPE, //constant INTERFACE (INTERFACE 0x04)       =11
         0x00, //number defining this interface (IF we had more than one interface)    ==12
         0x00, //alternate setting     ==13
         2, //number of endpoins, except 0 (pic167xx has 3, but we dont have to use all).       ==14
         0x03, //class code, 03 = HID     ==15
         0x00, //subclass code //boot     ==16
         0x00, //protocol code      ==17
         0x00, //index of string descriptor for interface      ==18

   //class descriptor 1  (HID)
         USB_DESC_CLASS_LEN, //length of descriptor    ==19
         USB_DESC_CLASS_TYPE, //dscriptor type (0x21 == HID)      ==20
         0x00,0x01, //hid class release number (1.0)      ==21,22
         0x00, //localized country code (0 = none)       ==23
         0x01, //number of hid class descrptors that follow (1)      ==24
         0x22, //report descriptor type (0x22 == HID)                ==25
         USB_CLASS_SPECIFIC_DESC_LOOKUP_SIZE[0][0], 0x00, //length of report descriptor            ==26,27

   //endpoint descriptor
         USB_DESC_ENDPOINT_LEN, //length of descriptor                   ==28
         USB_DESC_ENDPOINT_TYPE, //constant ENDPOINT (ENDPOINT 0x05)          ==29
         0x81, //endpoint number and direction (0x81 = EP1 IN)       ==30
         USB_EP1_TX_ENABLE, //transfer type supported (0x03 is interrupt)         ==31
         USB_EP1_TX_SIZE,0x00, //maximum packet size supported                  ==32,33
         USB_CONFIG_HID_TX_POLL,  //polling interval, in ms.  (cant be smaller than 10 for slow speed)      ==34

   //endpoint descriptor
         USB_DESC_ENDPOINT_LEN, //length of descriptor                   ==35
         USB_DESC_ENDPOINT_TYPE, //constant ENDPOINT (ENDPOINT 0x05)          ==36
         0x01, //endpoint number and direction (0x01 = EP1 OUT)      ==37
         USB_EP1_RX_ENABLE, //transfer type supported (0x03 is interrupt)         ==38
         USB_EP1_RX_SIZE,0x00, //maximum packet size supported                  ==39,40
         USB_CONFIG_HID_RX_POLL //polling interval, in ms.  (cant be smaller than 10 for slow speed)    ==41
   };

   //****** BEGIN CONFIG DESCRIPTOR LOOKUP TABLES ********
   //since we can't make pointers to constants in certain pic16s, this is an offset table to find
   //  a specific descriptor in the above table.

   //NOTE: DO TO A LIMITATION OF THE CCS CODE, ALL HID INTERFACES MUST START AT 0 AND BE SEQUENTIAL
   //      FOR EXAMPLE, IF YOU HAVE 2 HID INTERFACES THEY MUST BE INTERFACE 0 AND INTERFACE 1
   #define USB_NUM_HID_INTERFACES   1

   //the maximum number of interfaces seen on any config
   //for example, if config 1 has 1 interface and config 2 has 2 interfaces you must define this as 2
   #define USB_MAX_NUM_INTERFACES   1

   //define how many interfaces there are per config.  [0] is the first config, etc.
   const char USB_NUM_INTERFACES[USB_NUM_CONFIGURATIONS]={1};

   //define where to find class descriptors
   //first dimension is the config number
   //second dimension specifies which interface
   //last dimension specifies which class in this interface to get, but most will only have 1 class per interface
   //if a class descriptor is not valid, set the value to 0xFFFF
   const int8 USB_CLASS_DESCRIPTORS[USB_NUM_CONFIGURATIONS][1][1]=
   {
   //config 1
      //interface 0
         //class 1
         18
   };

   #if (sizeof(USB_CONFIG_DESC) != USB_TOTAL_CONFIG_LEN)
      #error USB_TOTAL_CONFIG_LEN not defined correctly
   #endif


//////////////////////////////////////////////////////////////////
///
///   start device descriptors
///
//////////////////////////////////////////////////////////////////

   const char USB_DEVICE_DESC[USB_DESC_DEVICE_LEN] ={
      //starts of with device configuration. only one possible
         USB_DESC_DEVICE_LEN, //the length of this report   ==1
         0x01, //the constant DEVICE (DEVICE 0x01)  ==2
         0x10,0x01, //usb version in bcd ==3,4
         0x00, //class code ==5
         0x00, //subclass code ==6
         0x00, //protocol code ==7
         USB_MAX_EP0_PACKET_LENGTH, //max packet size for endpoint 0. (SLOW SPEED SPECIFIES 8) ==8
         USB_CONFIG_VID & 0xFF, ((USB_CONFIG_VID >> 8) & 0xFF), //vendor id       ==9, 10
         USB_CONFIG_PID & 0xFF, ((USB_CONFIG_PID >> 8) & 0xFF), //product id, don't use 0xffff       ==11, 12
         USB_CONFIG_VERSION & 0xFF, ((USB_CONFIG_VERSION >> 8) & 0xFF), //device release number  ==13,14
         0x01, //index of string description of manufacturer. therefore we point to string_1 array (see below)  ==15
         0x02, //index of string descriptor of the product  ==16
         0x00, //index of string descriptor of serial number  ==17
         USB_NUM_CONFIGURATIONS  //number of possible configurations  ==18
   };


//////////////////////////////////////////////////////////////////
///
///   start string descriptors
///   String 0 is a special language string, and must be defined.  People in U.S.A. can leave this alone.
///
///   You must define the length else get_next_string_character() will not see the string
///   Current code only supports 10 strings (0 thru 9)
///
//////////////////////////////////////////////////////////////////

#if !defined(USB_STRINGS_OVERWRITTEN)
//the offset of the starting location of each string.  offset[0] is the start of string 0, offset[1] is the start of string 1, etc.
char USB_STRING_DESC_OFFSET[]={0,4,12};

// Here is where the "CCS" Manufacturer string and "CCS HID Demo" are stored.
// Strings are saved as unicode.
// These strings are mostly only displayed during the add hardware wizard.
// Once the operating system drivers have been installed it will usually display
// the name from the drivers .INF.
char const USB_STRING_DESC[]={
   //string 0
         4, //length of string index
         USB_DESC_STRING_TYPE, //descriptor type 0x03 (STRING)
         0x09,0x04,   //Microsoft Defined for US-English
   //string 1
         8, //length of string index
         USB_DESC_STRING_TYPE, //descriptor type 0x03 (STRING)
         'C',0,
         'C',0,
         'S',0,
   //string 2
         26, //length of string index
         USB_DESC_STRING_TYPE, //descriptor type 0x03 (STRING)
         'C',0,
         'C',0,
         'S',0,
         ' ',0,
         'H',0,
         'I',0,
         'D',0,
         ' ',0,
         'D',0,
         'e',0,
         'm',0,
         'o',0
};
#endif

#ENDIF
