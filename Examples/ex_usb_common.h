/////////////////////////////////////////////////////////////////////////
////                                                                 ////
////                        ex_usb_common.h                          ////
////                                                                 ////
//// Common hardware definitions and configuration for all of CCS's  ////
//// USB example programs (ex_usb_*.c)                               ////
////                                                                 ////
//// One of the following configuration macros must be #defined:     ////
////  USB_HW_CCS_PIC18F4550   CCS PIC18F4550 USB Development kit     ////
////  USB_HW_CCS_PIC24F       CCS 24FJ256GB206 USB Development kit.  ////
////  USB_HW_CCS_16F1459      CCS Rapid USB Development kit.         ////
////                          PIC16F1459 using internal oscillator.  ////
////  USB_HW_MCHP_16F1459     Microchip low-pin count USB            ////
////                          development kit with a PIC16F1459.     ////
////                          NOTE: the ICD pins on the low pin      ////
////                          count development kit are not          ////
////                          compatible with this chip!             ////
////  USB_HW_CCS_USBN9604     CCS National USBN9604 USB development  ////
////                          kit (external USB peripheral).         ////
////  USB_HW_MCHP_18F14K50    Microchip low-pin count USB            ////
////                          development kit (PIC18F14K50).         ////
////  USB_HW_MCHP_18F46J50    Microchip USB PIM Demo Board           ////
////                          (PIC18F46J50).                         ////
////  USB_HW_GENERIC_18F67J50 Generic 18F67J50 example.              ////
////  USB_HW_GENERIC_18F27J53 Generic 18F27J53 example.              ////
////  USB_HW_GENERIC_18F45K50 Generic 18F45K50 example.              ////
////  USB_HW_MCHP_EXPLORER16_24F  Microchip Explorer16 with USB      ////
////                          OTG PICTail Plus.                      ////
////  USB_HW_MCHP_EXPLORER16_24E  Microchip Explorer16 with USB      ////
////                          OTG PICTail Plus.                      ////
////                                                                 ////
//// This file is part of CCS's PIC USB driver code.  See USB.H      ////
//// for more documentation and a list of examples.                  ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// VERSION HISTORY                                                 ////
////                                                                 ////
//// July 1, 2013:                                                   ////
////  Added USB_HW_CCS_16F1459                                       ////
////                                                                 ////
//// Feb 22, 2013:                                                   ////
////  Added USB_HW_GENERIC_18F45K50                                  ////
////                                                                 ////
//// Feb 15th, 2013:                                                 ////
////  Removed USB_HW_CCS_PIC16C765, added USB_HW_MCHP_16F1459        ////
////                                                                 ////
//// Mar 7th, 2012:                                                  ////
////  Renamed USB_HW_MCHP_EXPLORER16 to USB_HW_MCHP_EXPLORER16_24F.  ////
////  Added USB_HW_MCHP_EXPLORER16_24E.                              ////
////                                                                 ////
//// Oct 15th, 2010:                                                 ////
////  Added initial 18F47J53 family support.                         ////
////  Added USB_ISR_POLLING support.  Define this and interrupts     ////
////     will not be used.  usb_task() must be called periodically   ////
////     in your main loop.  If it is not called faster than once    ////
////     per millisecond, USB may not work (PIC18 and PIC24 only).   ////
////                                                                 ////
//// August 31st, 2010:                                              ////
////  Added USB_HW_MCHP_18F46J50, USB_HW_MCHP_18F14K50 and           ////
////  USB_HW_GENERIC_18F67J50 hardware.                              ////
////                                                                 ////
//// April 28th, 2010:                                               ////
////  Added ex_usb_common.h.                                         ////
////  Initial support for CCS PIC24USB board.                        ////
////  USB_CON_SENSE_PIN replaced with USB_CABLE_IS_ATTACHED()        ////
////     macro.  If USB_CON_SENSE_PIN is defined, it will create     ////
////     USB_CABLE_IS_ATTACHED() macro for you (for backwards        ////
////     compatibility).                                             ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2010 Custom Computer Services         ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

////// Begin User Configuration

#define USB_HW_CCS_PIC18F4550     //CCS PIC18F4550 USB Development kit
//#define USB_HW_CCS_16F1459        //CCS Rapid USB devleopment kit.  16F1459 with no oscillator
//#define USB_HW_CCS_PIC24F           //CCS 24FJ256GB206 USB Development kit
//#define USB_HW_CCS_PIC33EP           //CCS PIC24 USB Development kit, PIC replaced with a 33EP256MU806
//#define USB_HW_CCS_USBN9604       //CCS National USBN9604 USB development kit (external USB peripheral)
//#define USB_HW_MCHP_18F14K50      //Microchip low-pin count USB development kit (PIC18F14K50)
//#define USB_HW_MCHP_18F46J50      //Microchip USB PIM Demo Board (PIC18F46J50)
//#define USB_HW_GENERIC_18F67J50   //Generic 18F67J50 example
//#define USB_HW_GENERIC_18F27J53   //Generic 18F27J53 example
//#define USB_HW_MCHP_EXPLORER16_24F   //Microchip Explorer16 with USB OTG PICTail+ & 24FJ256GB110
//#define USB_HW_MCHP_EXPLORER16_24E   //Microchip Explorer16 with USB OTG PICTail+
//#define USB_HW_MCHP_16F1459        //Microchip low-pin count USB development kit with a 16F1459, using internal oscillator with active clock tuning  NOTE: the ICD pins on the low pin count development kit are not compatible with this chip!
//#define USB_HW_GENERIC_18F45K50     //Generic 18F45K50 example.  You can get this by taking a CCS 4550 board and replacing the PIC with a 45K50 and the crystal wiht a 12Mhz

// Optional configuration.
// Defining USB_ISR_POLLING will have USB library not use ISRs.  Instead you
// must periodically call usb_task().
//#define USB_ISR_POLLING

////// End User Configuration

#ifndef __EX_USB_COMMON_H__
#define __EX_USB_COMMON_H__

#if defined(USB_HW_CCS_PIC18F4550)
   #include <18F4550.h>
   #fuses HSPLL,NOWDT,NOPROTECT,NOLVP,NODEBUG,USBDIV,PLL5,CPUDIV1,VREGEN
   #use delay(clock=48000000)

   //leds ordered from bottom to top
   #DEFINE LED1 PIN_A5  //green
   #define LED2 PIN_B4  //yellow
   #define LED3 PIN_B5  //red
   #define LEDS_OFF()   LED_OFF(LED1); LED_OFF(LED2); LED_OFF(LED3)
   #define BUTTON_PRESSED() !input(PIN_A4)

   //see section below labeled USB_CABLE_IS_ATTACHED
   #define PIN_USB_SENSE   PIN_B2

   #define HW_ADC_CONFIG   ADC_CLOCK_INTERNAL
   #define HW_ADC_CHANNEL  0
   #define HW_ADC_PORTS    AN0
   
   #define HW_INIT() setup_adc_ports(HW_ADC_PORTS)
   
   #define __NO_UART__
#endif

#if defined(USB_HW_CCS_16F1459)
   #include <16F1459.h>

   #use delay(int=8MHz, clock=48MHz, USB_FULL, act=USB)

   #fuses MCLR
   #fuses PUT
   #fuses NOWDT

   //leds are ordered left to right
   #define LED_DIR1 PIN_A5
   #define LED_DIR2 PIN_C6
   #define LED_ON(x)    delay_cycles(1)
   #define LED_OFF(x)   delay_cycles(1)
   #define LEDS_OFF()      output_low(LED_DIR1);  output_low(LED_DIR2)
   #define LEDS_GREEN()    LEDS_OFF(); output_high(LED_DIR1)
   #define LEDS_RED()      LEDS_OFF(); output_high(LED_DIR2)
   #define LEDS_TOGGLE()   output_toggle(LED_DIR1);  output_toggle(LED_DIR2)

   #define BUTTON_PRESSED() !input(PIN_C1)

   #define HW_ADC_CONFIG   ADC_CLOCK_INTERNAL
   #define HW_ADC_CHANNEL  11 //PIN_B5
   #define HW_ADC_PORTS    sAN11

   #define HW_INIT() setup_adc_ports(HW_ADC_PORTS); setup_comparator(NC_NC_NC_NC)
   
   #define __NO_UART__
   
   //#define UART_IS_ICD
   
  #if defined(UART_IS_ICD)
   #warning Can't use button and ICSP streaming because they share the same pin!
   #define UART_ICD_SETTINGS  xmit=PIN_C0, rcv=PIN_C1, baud=57600
  #endif
#endif

#if defined(USB_HW_CCS_PIC24F)
   #include <24FJ256GB206.h>
   #build(stack=1024)
   #device ICD=2
   #device ADC=8
   #fuses HS,PR_PLL,NOWDT,ICSP2,PLL5,SOSC_DIG
   #use delay(clock=32M)

   #pin_select U1TX = PIN_D5
   #pin_select U1RX = PIN_D4

   #pin_select SDI2 = PIN_F5
   #pin_select SDO2 = PIN_B15
   #pin_select SCK2OUT = PIN_F4

   #define LED1 PIN_B9     //green
   #define LED2 PIN_B10    //yellow
   #define LED3 PIN_B11    //red
   #define LEDS_OFF()   LED_OFF(LED1); LED_OFF(LED2); LED_OFF(LED3)
   #define LED_ON(x) output_high(x)
   #define LED_OFF(x) output_low(x)
   #define BUTTON_PRESSED() !input(PIN_F0)

   //#define HW_ADC_PORTS    sAN2|sAN5   //wrong?
   #define HW_ADC_PORTS    sAN0|sAN2   //0=pot, 2=vbus
   #define HW_ADC_CHANNEL  0
   #define HW_ADC_CONFIG   ADC_CLOCK_INTERNAL | ADC_TAD_MUL_31

   #define LCD_ENABLE_PIN  PIN_D0
   #define LCD_RS_PIN      PIN_D1
   #define LCD_RW_PIN      PIN_D2
   #define LCD_DATA4       PIN_E6
   #define LCD_DATA5       PIN_E7
   #define LCD_DATA6       PIN_D6
   #define LCD_DATA7       PIN_D7
#endif

#if defined(USB_HW_CCS_PIC33EP)
   #include <33EP256MU806.h>
   #build(stack=1024)
   #device ADC=8
   //20MHz clock is scaled to 48mhz for usb clock.
   //20mhz clock is scaled to 120mhz for cpu operation.
   #use delay(crystal=20Mhz, clock=120Mhz, AUX:clock=48Mhz)

   #pin_select U1TX = PIN_D5
   #pin_select U1RX = PIN_D4

   #define LED1 PIN_B9     //green
   #define LED2 PIN_B10    //yellow
   #define LED3 PIN_B11    //red
   #define LEDS_OFF()   LED_OFF(LED1); LED_OFF(LED2); LED_OFF(LED3)
   #define LED_ON(x) output_high(x)
   #define LED_OFF(x) output_low(x)
   #define BUTTON_PRESSED() !input(PIN_F0)

   #define HW_ADC_PORTS    sAN0|sAN2   //0=pot, 2=vbus
   #define HW_ADC_CHANNEL  0
   #define HW_ADC_CONFIG   ADC_CLOCK_INTERNAL | ADC_TAD_MUL_31

   #define LCD_ENABLE_PIN  PIN_D0
   #define LCD_RS_PIN      PIN_D1
   #define LCD_RW_PIN      PIN_D2
   #define LCD_DATA4       PIN_E6
   #define LCD_DATA5       PIN_E7
   #define LCD_DATA6       PIN_D6
   #define LCD_DATA7       PIN_D7   
#endif

#if defined(USB_HW_GENERIC_18F45K50)
   #include <18F45K50.h>
   
   #fuses NOWDT
   
   //#use delay(crystal=12Mhz, clock=48Mhz, USB_FULL)
   #fuses NOCPUDIV, PLLEN, PLL4X, HSH, PRIMARY, NOIESO, NOFCMEN  //12mhz crystal, 48mhz clock for pic and usb
   #use delay(clock=48MHz)

   //leds ordered from bottom to top
   #DEFINE LED1 PIN_A5  //green
   #define LED2 PIN_B4  //yellow
   #define LED3 PIN_B5  //red
   #define LEDS_OFF()   LED_OFF(LED1); LED_OFF(LED2); LED_OFF(LED3)
   #define BUTTON_PRESSED() !input(PIN_A4)

   //see section below labeled USB_CABLE_IS_ATTACHED
   #define PIN_USB_SENSE   PIN_B2

   #define HW_ADC_CONFIG   ADC_CLOCK_INTERNAL
   #define HW_ADC_CHANNEL  0
   #define HW_ADC_PORTS    sAN0
#endif

#if defined(USB_HW_MCHP_EXPLORER16_24E)
   #include <24EP512GU810.h>
   #build(stack=1024)
   #device ADC=8

   #fuses NOWDT, NOIESO

   //8MHz clock is scaled to 48mhz for usb clock.
   //8mhz clock is scaled to 120mhz for cpu operation.
   #use delay(crystal=8Mhz, clock=120Mhz, AUX:clock=48Mhz)
   
   // no pin select compatible.
   #pin_select U1TX = PIN_F5
   #pin_select U1RX = PIN_F4
   #define PIN_UART1_RTS   PIN_F13  //out to PC
   #define PIN_UART1_CTS   PIN_F12  //in to PIC

   // led's ordered from right to left
   #define LED1 PIN_A0  //D3
   #define LED2 PIN_A1  //D4
   #define LED3 PIN_A2  //D5
   #define LEDS_OFF()   LED_OFF(LED1); LED_OFF(LED2); LED_OFF(LED3)
   #define LED_ON(x) output_high(x)
   #define LED_OFF(x) output_low(x)

   #define BUTTON_PRESSED() !input(PIN_D6)   //s3

   #define HW_ADC_PORTS    sAN5
   #define HW_ADC_CHANNEL  5
   #define HW_ADC_CONFIG   ADC_CLOCK_INTERNAL | ADC_TAD_MUL_31

   /*
   #byte ACLKCON3=getenv("SFR:ACLKCON3")
   #byte ACLKDIV3=getenv("SFR:ACLKDIV3")
   #bit ACLKCON3_APLLCK=ACLKCON3.14
   #bit ACLKCON3_ENAPLL=ACLKCON3.15
   //#define HW_INIT() while(!ACLKCON3_APLLCK)
   #define HW_INIT() APllInit()
   void ApllInit(void)
   {
      ACLKCON3 = 0x24C1;
      ACLKDIV3 = 0x7;
      ACLKCON3_ENAPLL = 1;
      while(!ACLKCON3_APLLCK);
   }
   */
#endif

#if defined(USB_HW_MCHP_EXPLORER16_24F)
   #include <24FJ256GB110.h>
   #build(stack=1024)
   #device ADC=8
   #fuses HS,PR_PLL,NOWDT,DISUVREG,ICSP2
   #use delay(clock=32M)  //8MHz clock is 4x because of PLL
   #fuses PLL2   //Divide 8MHz by 2 to get the 4MHz required for USB

   #pin_select U1TX = PIN_F5
   #pin_select U1RX = PIN_F4
   #define PIN_UART1_RTS   PIN_F13  //out to PC
   #define PIN_UART1_CTS   PIN_F12  //in to PIC

   // led's ordered from left to right
   #define LED1 PIN_A7
   #define LED2 PIN_A6
   #define LED3 PIN_A5
   #define LED4 PIN_A4
   #define LED5 PIN_A3
   #define LED6 PIN_A2
   #define LED7 PIN_A1
   #define LED8 PIN_A0
   #define LED_ON(x) output_high(x)
   #define LED_OFF(x) output_low(x)
   #define LEDS_OFF()   LED_OFF(LED1); LED_OFF(LED2); LED_OFF(LED3); LED_OFF(LED4);  LED_OFF(LED5); LED_OFF(LED6); LED_OFF(LED7); LED_OFF(LED8)
   #define BUTTON_PRESSED() !input(PIN_D6)

   #define HW_ADC_PORTS    sAN5
   #define HW_ADC_CHANNEL  5
   #define HW_ADC_CONFIG   ADC_CLOCK_INTERNAL | ADC_TAD_MUL_31
#endif

#if defined(USB_HW_CCS_USBN9604)
   #include <18F452.h>
   #fuses HS,NOWDT,NOPROTECT,NOLVP
   #use delay(clock=20000000)

   #define __USB_PIC_PERIF__  0

   #DEFINE LED1  PIN_B3
   #define LED2 PIN_B4
   #define LED3 PIN_B5
   #define LEDS_OFF()   LED_OFF(LED1); LED_OFF(LED2); LED_OFF(LED3)
   #define BUTTON_PRESSED() !input(PIN_A4)

   #define HW_ADC_CONFIG   ADC_CLOCK_INTERNAL
   #define HW_ADC_CHANNEL  0
   #define HW_ADC_PORTS    AN0
#endif

#if defined(USB_HW_MCHP_16F1459)
   //NOTE: the ICD pins on the low pin count development kit are not compatible with this chip!
   #include <16F1459.h>

   #use delay(int=8MHz, clock=48MHz, USB_FULL, act=USB)

   #fuses NOMCLR
   #fuses NOWDT

   //leds are ordered left to right
   #define LED1 PIN_C3
   #define LED2 PIN_C2
   #define LED3 PIN_C1
   #define LED4 PIN_C0
   #define LEDS_OFF()   LED_OFF(LED1); LED_OFF(LED2); LED_OFF(LED3); LED_OFF(LED4)
   #define LED_ON(x) output_high(x)
   #define LED_OFF(x) output_low(x)

   #if getenv("FUSE_SET:NOMCLR")
     #define BUTTON_PRESSED() !input(PIN_A3)
   #else
     #define BUTTON_PRESSED() FALSE
   #endif

   #define HW_ADC_CONFIG   ADC_CLOCK_INTERNAL
   #define HW_ADC_CHANNEL  10
   #define HW_ADC_PORTS    sAN10
#endif

#if defined(USB_HW_MCHP_18F14K50)
   #include <18F14K50.h>

   #fuses NOIESO
   #fuses NOFCMEN
   #fuses PCLKEN
   #fuses NOPUT
   #fuses NOBROWNOUT
   #fuses NOWDT
   #fuses NOMCLR
   #fuses NOHFOFST
   #fuses NOLVP
   #fuses NOXINST

   // configure 12MHz clock for USB operation and 48Mhz CPU operation
   #fuses HS
   #fuses PLLEN   //enable pll, we can now use CPUDIVx fuses
   #fuses CPUDIV1 //CPU clock set to 48MHz
   #fuses USBDIV2 //when using low speed USB, need to divide 12MHz by 2.  this fuse has no effect when using full speed.

   #use delay(clock=48000000)

   //leds are ordered left to right
   #define LED1 PIN_C3
   #define LED2 PIN_C2
   #define LED3 PIN_C1
   #define LED4 PIN_C0
   #define LEDS_OFF()   LED_OFF(LED1); LED_OFF(LED2); LED_OFF(LED3); LED_OFF(LED4)
   #define LED_ON(x) output_high(x)
   #define LED_OFF(x) output_low(x)
   #define BUTTON_PRESSED() !input(PIN_A3)

   #define HW_ADC_CONFIG   ADC_CLOCK_INTERNAL
   #define HW_ADC_CHANNEL  10
   #define HW_ADC_PORTS    sAN10
#endif

#if defined(USB_HW_MCHP_18F46J50)
   #include <18F46J50.h>

   #fuses NOWDT
   #fuses NOXINST
   #fuses NODEBUG
   #fuses NOPROTECT

   #fuses HSPLL
   #fuses PLL3       //set usb pll clock to 4MHz (from 12Mhz input)
   #fuses NOCPUDIV   //set cpu clock to 48MHz
   #use delay(clock=48MHz)

   #DEFINE LED1   PIN_E0
   #define LED2   PIN_E1
   #define LED3   PIN_D7   //requires PIC18 Explorer
   #define LEDS_OFF()   LED_OFF(LED1); LED_OFF(LED2); LED_OFF(LED3)
   #define LED_ON(x) output_high(x)
   #define LED_OFF(x) output_low(x)
   #define BUTTON_PRESSED() !input(PIN_B2)

   // pot is on PIC18 Explorer
   #define HW_ADC_PORTS    sAN0
   #define HW_ADC_CHANNEL  0
   #define HW_ADC_CONFIG   ADC_CLOCK_INTERNAL

   // uart/rs232 requires PIC18 Explorer

   //#define PIN_USB_SENSE   PIN_C2    //Jumper JP2 needs to short 'R' to 'U' position for this to be valid.

   #define HW_INIT() setup_oscillator(OSC_PLL_ON); delay_ms(50)
#endif

#if defined(USB_HW_GENERIC_18F67J50)
   #include <18F67J50.h>

   #fuses NOWDT
   #fuses NOXINST

   //8Mhz clock configured for USB operation and 48MHz CPU operation
   #fuses HSPLL
   #fuses PLL2
   #fuses NOCPUDIV
   #use delay(clock=48M)

   #DEFINE LED1   PIN_E4
   #define LED2   PIN_E5
   #define LED3   PIN_E6
   #define LEDS_OFF()   LED_OFF(LED1); LED_OFF(LED2); LED_OFF(LED3)
   #define BUTTON_PRESSED() FALSE   //no push button on this hardware

   #define HW_ADC_PORTS    sAN0
   #define HW_ADC_CHANNEL  0
   #define HW_ADC_CONFIG   ADC_CLOCK_INTERNAL

   #define PIN_USB_SENSE   PIN_B2

   #byte OSCTUNE = 0xF9B
   #bit PLLEN=OSCTUNE.6
   #define HW_INIT() PLLEN=TRUE; delay_ms(50)
#endif

#if defined(USB_HW_GENERIC_18F27J53)
   #include <18F27J53.h>

   #fuses NOWDT
   #fuses NOXINST

   //12Mhz clock configured for USB operation and 48MHz CPU operation
   #fuses HSPLL
   #fuses PLLEN
   #fuses PLL3
   #fuses NOCPUDIV
   #use delay(clock=48M)

   #DEFINE LED1   PIN_B5
   #define LED2   PIN_B4
   #define LED3   PIN_C7
   #define LEDS_OFF()   LED_OFF(LED1); LED_OFF(LED2); LED_OFF(LED3)
   #define BUTTON_PRESSED() (!input(PIN_B0))

   #define PIN_USB_SENSE   PIN_A3

   #define __NO_UART__

   #define HW_ADC_PORTS    sAN0|sAN1|sAN2
   #define HW_ADC_CHANNEL  0
   #define HW_ADC_CONFIG   ADC_CLOCK_INTERNAL

   #byte OSCTUNE = 0xF9B
   #bit PLLEN=OSCTUNE.6

   #define HW_INIT() output_low(PIN_C0); output_low(PIN_C1); output_high(PIN_C2); output_high(PIN_B2); PLLEN=TRUE; delay_ms(50)
#endif

#if defined(__NO_UART__)
   #define uart_putc(c)
   #define uart_getc()  (0)
   #define uart_kbhit() (FALSE)
   #define uart_printf(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z) { }
   #define uart_task()
#else
   #ifndef __UART_BAUD__
      #define __UART_BAUD__   9600
   #endif
   #if defined(PIN_UART_TX)
      #use rs232(baud=__UART_BAUD__, xmit=PIN_UART_TX, rcv=PIN_UART_RX, errors)
   #elif defined(UART_IS_ICD)
      #if defined(UART_ICD_SETTINGS)
       #use rs232(icd, UART_ICD_SETTINGS)
      #else
       #use rs232(icd)
      #endif
   #else
      #use rs232(baud=__UART_BAUD__, UART1, errors)
   #endif

   #define uart_getc    getc
   #define uart_kbhit   kbhit
   #define uart_printf  printf

   #if defined(UART_USE_TX_BUFFER)
      char tbeBuffer[UART_USE_TX_BUFFER];
     #if UART_USE_TX_BUFFER>=0x100
      unsigned int16 tbeIn=0, tbeOut=0, tbeCount=0;
     #else
      unsigned int8 tbeIn=0, tbeOut=0, tbeCount=0;
     #endif

      void uart_putc(char c)
      {
         if (tbeCount < sizeof(tbeBuffer))
         {
            tbeCount++;
            tbeBuffer[tbeIn++] = c;
            if (tbeIn >= sizeof(tbeBuffer))
               tbeIn = 0;
         }
      }

      void uart_task(void)
      {
         char c;
         if (tbeCount)
         {
            tbeCount--;
            c = tbeBuffer[tbeOut++];
            if (tbeOut >= sizeof(tbeBuffer))
               tbeOut = 0;
            putc(c);
         }
      }
   #else
      void uart_putc(char c) {putc(c);}
      #define uart_task()
   #endif
#endif

/////////////////////////////////////////////////////////////////////////////
//
// Required macro: USB_CABLE_IS_ATTACHED()
//
// This macro provides configuration to the library to detect if a
// USB cable is attached or not.  This is only relevant if the PIC is acting
// as a slave device.
//
// If you are using a USB connection sense method or pin, define this
// macro here.  If you are not using connection sense, comment out this line.
// Without connection sense you will not know if the device gets disconnected.
//
// If you are using a PIC24/33 with the internal USB peripheral, you can connect
// the Vbus (5V) line from the USB to the Vbus pin on the PIC - you
// can then look at the SFR bit U1OTGSTAT.SESVD to determine if USB is
// connected.
//
// If you are not using the internal USB peripheral of a PIC24, you can connect
// Vbus to a GPIO to detect connection sense.
//
// For both methods (connecting to Vbus pin or GPIO pin), a pull-down resistor
// (around 150K) and capacitor (1uF) should also be placed on the Vbus pin.
//
/////////////////////////////////////////////////////////////////////////////
#if defined(PIN_USB_SENSE)
   #define USB_CABLE_IS_ATTACHED() input(PIN_USB_SENSE)
#elif defined(__PCD__)
   #bit U1OTGSTAT_SESVD=getenv("BIT:SESVD")
   #define USB_CABLE_IS_ATTACHED() (U1OTGSTAT_SESVD)
   //#define USB_CABLE_IS_ATTACHED() bit_test(*0x484,3) //24fj/33fj support only, won't work with 24ep/33ep
#endif

#ifndef LED_ON
#define LED_ON(x) output_low(x)
#endif

#ifndef LED_OFF
#define LED_OFF(x) output_high(x)
#endif

#ifndef HW_INIT
#define HW_INIT()
#endif

#ifndef __USB_PIC_PERIF__
#define __USB_PIC_PERIF__  1
#endif

#endif
