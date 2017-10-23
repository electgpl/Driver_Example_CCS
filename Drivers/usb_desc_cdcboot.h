///////////////////////////////////////////////////////////////////////////
////                         usb_desc_cdc.h                            ////
////                                                                   ////
//// An example set of device / configuration descriptors for use with ////
//// CCS's CDC Virtual COM Port driver (see usb_cdc.h)                 ////
////                                                                   ////
//// This file is part of CCS's PIC USB driver code.  See USB.H        ////
//// for more documentation and a list of examples.                    ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////                                                                   ////
//// Version History:                                                  ////
////                                                                   ////
//// 10/28/05:                                                         ////
////    Bulk endpoint sizes updated to allow more than 255 byte        ////
////    packets.                                                       ////
////    Changed device to USB 1.10                                     ////
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

#include <usb.h>

#ifndef  USB_CONFIG_PID
   #define  USB_CONFIG_PID       0x000c
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


//////////////////////////////////////////////////////////////////
///
///   start config descriptor
///   right now we only support one configuration descriptor.
///   the config, interface, class, and endpoint goes into this array.
///
//////////////////////////////////////////////////////////////////

   #DEFINE USB_TOTAL_CONFIG_LEN      67  //config+interface+class+endpoint+endpoint (2 endpoints)

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
         2, //number of interfaces this device supports       ==4
         0x01, //identifier for this configuration.  (IF we had more than one configurations)      ==5
         0x00, //index of string descriptor for this configuration      ==6
         0xC0, //bit 6=1 if self powered, bit 5=1 if supports remote wakeup (we don't), bits 0-4 unused and bit7=1         ==7
         USB_CONFIG_BUS_POWER/2, //maximum bus power required (maximum milliamperes/2)  (0x32 = 100mA)  ==8

   //interface descriptor 0 (comm class interface)
         USB_DESC_INTERFACE_LEN, //length of descriptor      =9
         USB_DESC_INTERFACE_TYPE, //constant INTERFACE (INTERFACE 0x04)       =10
         0x00, //number defining this interface (IF we had more than one interface)    ==11
         0x00, //alternate setting     ==12
         1, //number of endpoints   ==13
         0x02, //class code, 02 = Comm Interface Class     ==14
         0x02, //subclass code, 2 = Abstract     ==15
         0x01, //protocol code, 1 = v.25ter      ==16
         0x00, //index of string descriptor for interface      ==17

   //class descriptor [functional header]
         5, //length of descriptor    ==18
         0x24, //dscriptor type (0x24 == )      ==19
         0, //sub type (0=functional header) ==20
         0x10,0x01, //      ==21,22 //cdc version

   //class descriptor [acm header]
         4, //length of descriptor    ==23
         0x24, //dscriptor type (0x24 == )      ==24
         2, //sub type (2=ACM)   ==25
         2, //capabilities    ==26  //we support Set_Line_Coding, Set_Control_Line_State, Get_Line_Coding, and the notification Serial_State.

   //class descriptor [union header]
         5, //length of descriptor    ==27
         0x24, //dscriptor type (0x24 == )      ==28
         6, //sub type (6=union)    ==29
         0, //master intf     ==30  //The interface number of the Communication or Dat a Cl ass interface, designated as the masteror controlling interface for the union.
         1, //save intf0      ==31  //Interface number of first slave or associated interface in the union. *

   //class descriptor [call mgmt header]
         5, //length of descriptor    ==32
         0x24, //dscriptor type (0x24 == )      ==33
         1, //sub type (1=call mgmt)   ==34
         0, //capabilities          ==35  //device does not handle call management itself
         1, //data interface        ==36  //interface number of data class interface

   //endpoint descriptor
         USB_DESC_ENDPOINT_LEN, //length of descriptor                   ==37
         USB_DESC_ENDPOINT_TYPE, //constant ENDPOINT (ENDPOINT 0x05)          ==38
         USB_CDC_COMM_IN_ENDPOINT | 0x80, //endpoint number and direction
         0x03, //transfer type supported (0x03 is interrupt)         ==40
         USB_CDC_COMM_IN_SIZE,0x00, //maximum packet size supported                  ==41,42
         250,  //polling interval, in ms.  (cant be smaller than 10)      ==43

   //interface descriptor 1 (data class interface)
         USB_DESC_INTERFACE_LEN, //length of descriptor      =44
         USB_DESC_INTERFACE_TYPE, //constant INTERFACE (INTERFACE 0x04)       =45
         0x01, //number defining this interface (IF we had more than one interface)    ==46
         0x00, //alternate setting     ==47
         2, //number of endpoints   ==48
         0x0A, //class code, 0A = Data Interface Class     ==49
         0x00, //subclass code      ==50
         0x00, //protocol code      ==51
         0x00, //index of string descriptor for interface      ==52

   //endpoint descriptor
         USB_DESC_ENDPOINT_LEN, //length of descriptor                   ==60
         USB_DESC_ENDPOINT_TYPE, //constant ENDPOINT (ENDPOINT 0x05)          ==61
         USB_CDC_DATA_OUT_ENDPOINT, //endpoint number and direction (0x02 = EP2 OUT)       ==62
         0x02, //transfer type supported (0x02 is bulk)         ==63
//         make8(USB_CDC_DATA_OUT_SIZE,0),make8(USB_CDC_DATA_OUT_SIZE,1), //maximum packet size supported                  ==64, 65
         USB_CDC_DATA_OUT_SIZE & 0xFF, (USB_CDC_DATA_OUT_SIZE >> 8) & 0xFF, //maximum packet size supported                  ==64, 65
         1,  //polling interval, in ms.

   //endpoint descriptor
         USB_DESC_ENDPOINT_LEN, //length of descriptor                   ==53
         USB_DESC_ENDPOINT_TYPE, //constant ENDPOINT (ENDPOINT 0x05)          ==54
         USB_CDC_DATA_IN_ENDPOINT | 0x80, //endpoint number and direction (0x82 = EP2 IN)       ==55
         0x02, //transfer type supported (0x02 is bulk)         ==56
//         make8(USB_CDC_DATA_IN_SIZE,0),make8(USB_CDC_DATA_IN_SIZE,1), //maximum packet size supported                  ==57, 58
         (USB_CDC_DATA_IN_SIZE+1) & 0xFF, ((USB_CDC_DATA_IN_SIZE+1) >> 8) & 0xFF, //maximum packet size supported                  ==64, 65
         1,  //polling interval, in ms.
   };

   //****** BEGIN CONFIG DESCRIPTOR LOOKUP TABLES ********
   //since we can't make pointers to constants in certain pic16s, this is an offset table to find
   //  a specific descriptor in the above table.

   //the maximum number of interfaces seen on any config
   //for example, if config 1 has 1 interface and config 2 has 2 interfaces you must define this as 2
   #define USB_MAX_NUM_INTERFACES   2

   //define how many interfaces there are per config.  [0] is the first config, etc.
   const char USB_NUM_INTERFACES[USB_NUM_CONFIGURATIONS]={2};

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
         18,23,27,32,
      //interface 1
         //no classes for this interface
         0xFF,0xFF,0xFF,0xFF
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
         0x02, //class code. 0x02=Communication Device Class ==4
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

//the offset of the starting location of each string.  offset[0] is the start of string 0, offset[1] is the start of string 1, etc.
char USB_STRING_DESC_OFFSET[]={0,4,12};

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
         38, //length of string index
         USB_DESC_STRING_TYPE, //descriptor type 0x03 (STRING)
         'C',0,
         'C',0,
         'S',0,
         ' ',0,
         'C',0,
         'D',0,
         'C',0,
         ' ',0,
         'B',0,
         'O',0,
         'O',0,
         'T',0,
         'L',0,
         'O',0,
         'A',0,
         'D',0,
         'E',0,
         'R',0
};

#ENDIF
