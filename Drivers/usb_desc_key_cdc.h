///////////////////////////////////////////////////////////////////////////
////                     usb_desc_key_cdc.h                            ////
////                                                                   ////
//// An example set of device / configuration descriptors.             ////
//// This creates a composite USB device containing both CDC (virtual  ////
//// COM port) and HID interface.                                      ////
////                                                                   ////
//// Interface 1 and 2 (Endpoint 1 and 2) are used for CDC.            ////
//// Interface 3 (Endpoint 3) is used for HID Keyboard.                ////
////                                                                   ////
//// This file is part of CCS's PIC USB driver code.  See USB.H        ////
//// for more documentation and a list of examples.                    ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////                                                                   ////
//// Version History:                                                  ////
////                                                                   ////
////  April 2nd, 2012                                                  ////
////     Initial version of CDC+HID composite device support           ////
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

///////// config options, although it's best to leave alone for this demo /////
#ifndef  USB_CONFIG_PID
   #define  USB_CONFIG_PID       0x0007
#endif
#ifndef  USB_CONFIG_VID
   #define  USB_CONFIG_VID       0x2405
#endif
#ifndef  USB_CONFIG_BUS_POWER
   #define  USB_CONFIG_BUS_POWER 100   //100mA  (range is 0..500)
#endif
#ifndef  USB_CONFIG_VERSION
   #define  USB_CONFIG_VERSION   0x0100      //01.00  //range is 00.00 to 99.99
#endif
//////// end config ///////////////////////////////////////////////////////////

#DEFINE USB_HID_DEVICE  TRUE
#DEFINE USB_CDC_DEVICE  TRUE

#define USB_CDC_COMM_IN_ENDPOINT       1
#ifndef USB_CDC_COMM_IN_SIZE
#define USB_CDC_COMM_IN_SIZE           11
#endif
#define USB_EP1_TX_ENABLE  USB_ENABLE_INTERRUPT
#define USB_EP1_TX_SIZE  USB_CDC_COMM_IN_SIZE

//pic to pc endpoint config
#define USB_CDC_DATA_IN_ENDPOINT       2
#ifndef USB_CDC_DATA_IN_SIZE
#define USB_CDC_DATA_IN_SIZE           64
#endif
#define USB_EP2_TX_ENABLE  USB_ENABLE_BULK
#define USB_EP2_TX_SIZE  USB_CDC_DATA_IN_SIZE

//pc to pic endpoint config
#define USB_CDC_DATA_OUT_ENDPOINT       2
#ifndef USB_CDC_DATA_OUT_SIZE
#define USB_CDC_DATA_OUT_SIZE           64
#endif
#define USB_EP2_RX_ENABLE  USB_ENABLE_BULK
#define USB_EP2_RX_SIZE  USB_CDC_DATA_OUT_SIZE

// hid keyboard endpoint config
#define USB_HID_ENDPOINT   3

#define USB_EP3_TX_ENABLE  USB_ENABLE_INTERRUPT   //turn on EP1 for IN bulk/interrupt transfers
#define USB_EP3_TX_SIZE 8

#define USB_EP3_RX_ENABLE  USB_ENABLE_INTERRUPT   //turn on EP1 for IN bulk/interrupt transfers
#define USB_EP3_RX_SIZE 4

#define USB_HID_INTERFACE  2

#include <usb.h>

   //the maximum number of interfaces seen on any config
   //for example, if config 1 has 1 interface and config 2 has 2 interfaces you must define this as 2
   #define USB_MAX_NUM_INTERFACES   3

   //////////////////////////////////////////////////////////////////
   ///
   ///  HID Report.  Tells HID driver how to handle and deal with
   ///  received data.  HID Reports can be extremely complex,
   ///  see HID specifcation for help on writing your own.
   ///
   //////////////////////////////////////////////////////////////////

   const char USB_CLASS_SPECIFIC_DESC[] = 
   {
    //hid report descriptor for interface 1 (keyboard)
      0x05, 0x01, //usage page (generic desktop)   //1,2
      0x09, 0x06, //usage (keyboard)   //3,4
      0xA1, 0x01, //collection (application) //5,6
      0x05, 0x07, //usage page (key codes)   //7,8
      0x19, 0xE0, //usage min (224) //9,10
      0x29, 0xE7, //usage max (231) //11,12
      0x15, 0x00, //logical min (0) //13,14
      0x25, 0x01, //logical max (1) //15,16
      0x75, 0x01, //report size (1) //17,18
      0x95, 0x08, //report count (8)   //19,20
      0x81, 0x02, //input (data, variable, absolute) [modifier byte] //21,22
      0x95, 0x01, //report count (1)   //23,24
      0x75, 0x08, //report size (8)    //25,26
      0x81, 0x01, //input (constant) [reserved byte]  //27,28
      0x95, 0x05, //report count (5)   //29,30
      0x75, 0x01, //report size (1)    //31,32
      0x05, 0x08, //usage page (page# for leds) //33,34
      0x19, 0x01, //usage min (1)   //35,36
      0x29, 0x05, //usage max (5)   //37,38
      0x91, 0x02, //output (data, var, abs) [led report] //39,40
      0x95, 0x01, //report count (1)   //41,42
      0x75, 0x03, //report size (3) //43,44
      0x91, 0x01, //output (constant) [led report padding]  //45,46
      0x95, 0x05, //report count (5)   //47,48
      0x75, 0x08, //report size (8) //49,50
      0x15, 0x00, //logical min (0) //51,52
      0x25, 0x65, //logical max (101)  //53,54
      0x05, 0x07, //usage page (key codes)   //55,56
      0x19, 0x00, //usage min (0)   //57,58
      0x29, 0x65, //usage max (101) //59,60
      0x81, 0x00, //input (data, array)   //61,62
      0xC0        //end collection  //63
   };

   //if a class has an extra descriptor not part of the config descriptor,
   // this lookup table defines where to look for it in the const
   // USB_CLASS_SPECIFIC_DESC[] array.
   //first element is the config number (if your device has more than one config)
   //second element is which interface number
   //set element to 0xFFFF if this config/interface combo doesn't exist
   const int16 USB_CLASS_SPECIFIC_DESC_LOOKUP[USB_NUM_CONFIGURATIONS][USB_MAX_NUM_INTERFACES] =
   {
   //config 1
      //interface 0
         0,
      //interface 1
         0,
      //interface 2
         0
   };

   //if a class has an extra descriptor not part of the config descriptor,
   // this lookup table defines the size of that descriptor.
   //first element is the config number (if your device has more than one config)
   //second element is which interface number
   //set element to 0xFFFF if this config/interface combo doesn't exist
   const int16 USB_CLASS_SPECIFIC_DESC_LOOKUP_SIZE[USB_NUM_CONFIGURATIONS][USB_MAX_NUM_INTERFACES] =
   {
   //config 1
      //interface 0
         0,
      //interface 1
         0,
      //interface 2
         63
   };

//////////////////////////////////////////////////////////////////
///
///   start config descriptor
///   right now we only support one configuration descriptor.
///   the config, interface, class, and endpoint goes into this array.
///
//////////////////////////////////////////////////////////////////

   #DEFINE USB_TOTAL_CONFIG_LEN      107  //config+interface+class+endpoint+endpoint (2 endpoints)

   const char USB_CONFIG_DESC[] = {
   //IN ORDER TO COMPLY WITH WINDOWS HOSTS, THE ORDER OF THIS ARRAY MUST BE:
      //    config(s)
      //    interface(s)
      //    class(es)
      //    endpoint(s)

   //config_descriptor for config index 1
         USB_DESC_CONFIG_LEN, //length of descriptor size          ==0
         USB_DESC_CONFIG_TYPE, //constant CONFIGURATION (CONFIGURATION 0x02)     ==1
         USB_TOTAL_CONFIG_LEN,0, //size of all data returned for this config      ==2,3
         USB_MAX_NUM_INTERFACES, //number of interfaces this device supports       ==4
         0x01, //identifier for this configuration.  (IF we had more than one configurations)      ==5
         0x00, //index of string descriptor for this configuration      ==6
        #if USB_CONFIG_BUS_POWER
         0x80, //bit 6=1 if self powered, bit 5=1 if supports remote wakeup (we don't), bits 0-4 unused and bit7=1         ==7
        #else
         0xC0, //bit 6=1 if self powered, bit 5=1 if supports remote wakeup (we don't), bits 0-4 unused and bit7=1         ==7
        #endif
         USB_CONFIG_BUS_POWER/2, //maximum bus power required (maximum milliamperes/2)  (0x32 = 100mA)   ==8

 // interface association descriptor (tie 2 cdc interfaces together)
         USB_DESC_IAD_LEN,    //9
         USB_DESC_IAD_TYPE,   //10
         0, //first interface //11
         2, //number of tied interfaces   //12
         2, //class code of first inteface (2 = comm interface class)   //13
         2, //subclass code (2 = absctract)  //14
         1, //protocol code (1 = v.25ter) //15
         0, //string index //16
   
   //interface descriptor 0 (comm class interface)
         USB_DESC_INTERFACE_LEN, //length of descriptor      =17
         USB_DESC_INTERFACE_TYPE, //constant INTERFACE (INTERFACE 0x04)       =18
         0x00, //number defining this interface (IF we had more than one interface)    ==19
         0x00, //alternate setting     ==20
         1, //number of endpoints   ==21
         0x02, //class code, 02 = Comm Interface Class     ==22
         0x02, //subclass code, 2 = Abstract     ==23
         0x01, //protocol code, 1 = v.25ter      ==24
         0x00, //index of string descriptor for interface      ==25

   //class descriptor [functional header]
         5, //length of descriptor    ==26
         0x24, //dscriptor type (0x24 == )      ==27
         0, //sub type (0=functional header) ==28
         0x10,0x01, //      ==29,30 //cdc version

   //class descriptor [acm header]
         4, //length of descriptor    ==31
         0x24, //dscriptor type (0x24 == )      ==32
         2, //sub type (2=ACM)   ==33
         2, //capabilities    ==34  //we support Set_Line_Coding, Set_Control_Line_State, Get_Line_Coding, and the notification Serial_State.

   //class descriptor [union header]
         5, //length of descriptor    ==35
         0x24, //dscriptor type (0x24 == )      ==36
         6, //sub type (6=union)    ==37
         0, //master intf     ==38  //The interface number of the Communication or Dat a Cl ass interface, designated as the masteror controlling interface for the union.
         1, //save intf0      ==39  //Interface number of first slave or associated interface in the union. *

   //class descriptor [call mgmt header]
         5, //length of descriptor    ==40
         0x24, //dscriptor type (0x24 == )      ==41
         1, //sub type (1=call mgmt)   ==42
         0, //capabilities          ==43  //device does not handle call management itself
         1, //data interface        ==44  //interface number of data class interface

   //endpoint descriptor
         USB_DESC_ENDPOINT_LEN, //length of descriptor                   ==45
         USB_DESC_ENDPOINT_TYPE, //constant ENDPOINT (ENDPOINT 0x05)          ==46
         USB_CDC_COMM_IN_ENDPOINT | 0x80, //endpoint number and direction  //47
         0x03, //transfer type supported (0x03 is interrupt)         ==48
         USB_CDC_COMM_IN_SIZE,0x00, //maximum packet size supported                  ==49,50
         250,  //polling interval, in ms.  (interrupt endpoint cant be smaller than 10 for slow speed devices)      ==51

   //interface descriptor 1 (data class interface)
         USB_DESC_INTERFACE_LEN, //length of descriptor      =52
         USB_DESC_INTERFACE_TYPE, //constant INTERFACE (INTERFACE 0x04)       =53
         0x01, //number defining this interface (IF we had more than one interface)    ==54
         0x00, //alternate setting     ==55
         2, //number of endpoints   ==56
         0x0A, //class code, 0A = Data Interface Class     ==57
         0x00, //subclass code      ==58
         0x00, //protocol code      ==59
         0x00, //index of string descriptor for interface      ==60

   //endpoint descriptor
         USB_DESC_ENDPOINT_LEN, //length of descriptor                   ==61
         USB_DESC_ENDPOINT_TYPE, //constant ENDPOINT (ENDPOINT 0x05)          ==62
         USB_CDC_DATA_OUT_ENDPOINT, //endpoint number and direction (0x02 = EP2 OUT)       ==63
         0x02, //transfer type supported (0x02 is bulk)         ==64
         USB_CDC_DATA_OUT_SIZE & 0xFF, (USB_CDC_DATA_OUT_SIZE >> 8) & 0xFF, //maximum packet size supported                  ==65, 66
         1,  //polling interval, in ms.   ==67

   //endpoint descriptor
         USB_DESC_ENDPOINT_LEN, //length of descriptor                   ==68
         USB_DESC_ENDPOINT_TYPE, //constant ENDPOINT (ENDPOINT 0x05)          ==69
         USB_CDC_DATA_IN_ENDPOINT | 0x80, //endpoint number and direction (0x82 = EP2 IN)       ==70
         0x02, //transfer type supported (0x02 is bulk)         ==71
         USB_CDC_DATA_IN_SIZE & 0xFF, (USB_CDC_DATA_IN_SIZE >> 8) & 0xFF, //maximum packet size supported                  ==72,73
         1,  //polling interval, in ms.   ==74

   //interface descriptor 2 (KEYBOARD)
         USB_DESC_INTERFACE_LEN, //length of descriptor      =75
         USB_DESC_INTERFACE_TYPE, //constant INTERFACE (INTERFACE 0x04)       =76
         USB_HID_INTERFACE, //number defining this interface (IF we had more than one interface)    ==77
         0x00, //alternate setting     ==78
         2, //number of endpoints for this interface  //79
         0x03, //class code, 03 = HID     ==80
         0x01, //subclass code //boot     ==81
         0x01, //protocol code (keyboard)      ==82
         0x00, //index of string descriptor for interface      ==83

   //class descriptor 1  (HID)
         USB_DESC_CLASS_LEN, //length of descriptor    ==84
         USB_DESC_CLASS_TYPE, //dscriptor type (0x21 == HID)      ==85
         0x10,0x01, //hid class release number (1.0) (try 1.10)      ==86,87
         0x00, //localized country code (0 = none)       ==88
         0x01, //number of hid class descrptors that follow (1)      ==89
         0x22, //report descriptor type (0x22 == HID)                ==90
         USB_CLASS_SPECIFIC_DESC_LOOKUP_SIZE[0][USB_HID_INTERFACE], 0x00, //length of report descriptor            ==91,92

   //endpoint descriptor 3 IN
         USB_DESC_ENDPOINT_LEN, //length of descriptor                   ==93
         USB_DESC_ENDPOINT_TYPE, //constant ENDPOINT (ENDPOINT 0x05)          ==94
         0x83, //endpoint number and direction (0x83 = EP3 IN)       ==95
         USB_ENDPOINT_TYPE_INTERRUPT, //transfer type supported (0x03 is interrupt)         ==96
         USB_EP3_TX_SIZE,0x00, //maximum packet size supported                  ==97,98
         10,  //polling interval, in ms.  (cant be smaller than 10 for slow speed devices)     ==99

   //endpoint descriptor 3 OUT
         USB_DESC_ENDPOINT_LEN, //length of descriptor                   ==100
         USB_DESC_ENDPOINT_TYPE, //constant ENDPOINT (ENDPOINT 0x05)          ==101
         0x03, //endpoint number and direction (0x03 = EP3 OUT)       ==102
         USB_ENDPOINT_TYPE_INTERRUPT, //transfer type supported (0x03 is interrupt)         ==103
         USB_EP3_RX_SIZE,0x00, //maximum packet size supported                  ==104,105
         10  //polling interval, in ms.  (cant be smaller than 10 for slow speed devices)     ==106
   };

   //****** BEGIN CONFIG DESCRIPTOR LOOKUP TABLES ********
   //since we can't make pointers to constants in certain pic16s, this is an offset table to find
   //  a specific descriptor in the above table.

   //define how many interfaces there are per config.  [0] is the first config, etc.
   const char USB_NUM_INTERFACES[USB_NUM_CONFIGURATIONS]={3};

   //define where to find class descriptors
   //first dimension is the config number
   //second dimension specifies which interface
   //last dimension specifies which class in this interface to get, but most will only have 1 class per interface
   //if a class descriptor is not valid, set the value to 0xFFFF
   const int8 USB_CLASS_DESCRIPTORS[USB_NUM_CONFIGURATIONS][USB_MAX_NUM_INTERFACES][4]=
   {
   //config 1
      //interface 0
         //class 1-4
         26,31,35,40,
      //interface 1
         //no classes for this interface
         0xFF,0xFF,0xFF,0xFF,
      //interface 2
         84, 0xFF, 0xFF, 0xFF
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
         USB_DESC_DEVICE_LEN, //the length of this report   ==0
         0x01, //the constant DEVICE (DEVICE 0x01)  ==1
         0x10,0x01, //usb version in bcd  ==2,3
         0x00, //class code. 0x00 (interface defines class)
         0x00, //subclass code ==5
         0x00, //protocol code ==6
         USB_MAX_EP0_PACKET_LENGTH, //max packet size for endpoint 0. (SLOW SPEED SPECIFIES 8) ==7
         USB_CONFIG_VID & 0xFF, ((USB_CONFIG_VID >> 8) & 0xFF), //vendor id       ==9, 10
         USB_CONFIG_PID & 0xFF, ((USB_CONFIG_PID >> 8) & 0xFF), //product id, don't use 0xffff       ==11, 12
         USB_CONFIG_VERSION & 0xFF, ((USB_CONFIG_VERSION >> 8) & 0xFF), //device release number  ==13,14
         0x01, //index of string description of manufacturer. therefore we point to string_1 array (see below)  ==14
         0x02, //index of string descriptor of the product  ==15
         0x00, //index of string descriptor of serial number  ==16
         USB_NUM_CONFIGURATIONS  //number of possible configurations  ==17
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

// Here is where the "CCS" Manufacturer string and "SERIAL DEMO" are stored.
// Strings are saved as unicode.
// These strings are mostly only displayed during the add hardware wizard.
// Once the operating system drivers have been installed it will usually display
// the name from the drivers .INF.
char const USB_STRING_DESC[]={
   //string 0
         4, //length of string index
         USB_DESC_STRING_TYPE, //descriptor type 0x03 (STRING)
         0x09,0x04,   //Microsoft Defined for US-English
   //string 1  - manufacturer
         8, //length of string index
         USB_DESC_STRING_TYPE, //descriptor type 0x03 (STRING)
         'C',0,
         'C',0,
         'S',0,
   //string 2 - product
         30, //length of string index
         USB_DESC_STRING_TYPE, //descriptor type 0x03 (STRING)
         'C',0,
         'D',0,
         'C',0,
         ' ',0,
         '&',0,
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
#endif   //!defined(USB_STRINGS_OVERWRITTEN)

#ENDIF
