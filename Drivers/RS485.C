/////////////////////////////////////////////////////////////////////////
////                            RS485.c                              ////
////                                                                 ////
////      This file contains drivers for RS-485 communication        ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// int1 rs485_get_message(int* data_ptr, int wait)                 ////
////     * Get a message from the RS485 bus                          ////
////     * Address can be 1 - 33.                                    ////
////     * A 1 will be returned if a parity check error occurred.    ////
////                                                                 ////
//// int1 rs485_send_message(int to, int len, int* data)             ////
////     * Send a message over the RS485 bus                         ////
////     * Inputs:                                                   ////
////          to    - Destination address                            ////
////          len   - Message length                                 ////
////          *data - Pointer to message                             ////
////     * Returns TRUE if successful, FALSE if failed               ////
////                                                                 ////
//// void rs485_wait_for_bus(int1 clrwdt)                            ////
////     * Wait for the RS485 bus to open. Normally used before      ////
////       sending a message to avoid collisions.                    ////
////     * Pass in TRUE to restart the watch dog timer to prevent    ////
////       the device from resetting.                                ////
////     * This is not needed if sending an immediate response       ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2004 Custom Computer Services        ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#ifndef RS485_DRIVER
#define RS485_DRIVER

#ifndef RS485_ID
#define RS485_ID  0x10                 // The device's RS485 address or ID
#endif

#ifndef RS485_USE_EXT_INT
#define RS485_USE_EXT_INT FALSE        // Select between external interrupt
#endif                                 // or asynchronous serial interrupt


#if(RS485_USE_EXT_INT == FALSE)
   #ifndef RS485_RX_PIN
   #define RS485_RX_PIN       PIN_C7   // Data receive pin
   #endif

   #ifndef RS485_TX_PIN
   #define RS485_TX_PIN       PIN_C6   // Data transmit pin
   #endif

   #ifndef RS485_ENABLE_PIN
   #define RS485_ENABLE_PIN   PIN_B4   // Controls DE pin.  RX low, TX high.
   #endif

   #ifndef RS485_RX_ENABLE
   #define RS485_RX_ENABLE    PIN_B5   // Controls RE pin.  Should keep low.
   #endif

   #use rs232(baud=9600, xmit=RS485_TX_PIN, rcv=RS485_RX_PIN, enable=RS485_ENABLE_PIN, bits=9, long_data, errors, stream=RS485)
   #use rs232(baud=9600, xmit=RS485_TX_PIN, rcv=RS485_RX_PIN, enable=RS485_ENABLE_PIN, bits=9, long_data, force_sw, multi_master, errors, stream=RS485_CD)

   #if getenv("AUART")
      #define RCV_OFF() {setup_uart(FALSE);}
   #else
      #define RCV_OFF() {setup_uart(FALSE);}
   #endif
#else
   #ifndef RS485_RX_PIN
   #define RS485_RX_PIN       PIN_B0   // Data receive pin
   #endif

   #ifndef RS485_TX_PIN
   #define RS485_TX_PIN       PIN_B3   // Data transmit pin
   #endif

   #ifndef RS485_ENABLE_PIN
   #define RS485_ENABLE_PIN   PIN_B4   // Controls DE pin.  RX low, TX high.
   #endif

   #ifndef RS485_RX_ENABLE
   #define RS485_RX_ENABLE    PIN_B5   // Controls RE pin.  Should keep low.
   #endif

   #use rs232(baud=9600, xmit=RS485_TX_PIN, rcv=RS485_RX_PIN, enable=RS485_ENABLE_PIN, bits=9, long_data, errors, stream=RS485)
   #use rs232(baud=9600, xmit=RS485_TX_PIN, rcv=RS485_RX_PIN, enable=RS485_ENABLE_PIN, bits=9, long_data, multi_master, errors, stream=RS485_CD)

   #define RCV_OFF() {disable_interrupts(INT_EXT);}
#endif



#define RS485_wait_time 20             // Wait time in milliseconds

#bit    rs485_collision = rs232_errors.6

#ifndef RS485_RX_BUFFER_SIZE
#define RS485_RX_BUFFER_SIZE  40
#endif

int rs485_state, rs485_ni, rs485_no;
int rs485_buffer[RS485_RX_BUFFER_SIZE];


// Purpose:    Enable data reception
// Inputs:     None
// Outputs:    None
void RCV_ON(void) {
   #if (RS485_USE_EXT_INT==FALSE)
      while(kbhit(RS485)) {getc();} // Clear RX buffer. Clear RDA interrupt flag. Clear overrun error flag.
      #if getenv("AUART")
         setup_uart(UART_ADDRESS);
         setup_uart(TRUE);
      #else
         setup_uart(TRUE);
      #endif
   #else
      clear_interrupt(INT_EXT);
      enable_interrupts(INT_EXT);
   #endif
}


// Purpose:    Initialize RS485 communication. Call this before
//             using any other RS485 functions.
// Inputs:     None
// Outputs:    None
void rs485_init() {
   RCV_ON();
   rs485_state=0;
   rs485_ni=0;
   rs485_no=0;
   #if RS485_USE_EXT_INT==FALSE
   enable_interrupts(INT_RDA);
   #else
   ext_int_edge(H_TO_L);
   enable_interrupts(INT_EXT);
   #endif
   enable_interrupts(GLOBAL);
   output_low(RS485_RX_ENABLE);
}


// The index for the temporary receive buffer
int8 temp_ni;

// Purpose:    Add a byte of data to the temporary receive buffer
// Inputs:     The byte of data
// Outputs:    None
void rs485_add_to_temp(int8 b) {
   // Store the byte
   rs485_buffer[temp_ni] = b;

   // Make the index cyclic
   if(++temp_ni >= RS485_RX_BUFFER_SIZE)
   {
      temp_ni = 0;
   }
}


// Purpose:    Interrupt service routine for handling incoming RS485 data
#if (RS485_USE_EXT_INT==FALSE)
#int_rda
#else
#int_ext
#endif
void incomming_rs485() {
   int16 b;
   static int8  cs,state=0,len;
   static int16 to,source;

   b=fgetc(RS485);
   cs^=(int8)b;

   switch(state) {
      case 0:  // Get from address
         temp_ni=rs485_ni;
         source=b;
         cs=b;
         rs485_add_to_temp(source);
         break;

      case 1:  // Get to address
         to=b;
         #if (getenv("AUART")&&(RS485_USE_EXT_INT==FALSE))
            setup_uart(UART_DATA);
         #endif
         break;

      case 2:  // Get len
         len=b;
         rs485_add_to_temp(len);
         break;

      case 255:   // Get checksum
         if ((!cs)&&(bit_test(to,8))&&(bit_test(source,8))&&((int8)to==RS485_ID)) {  // If cs==0, then checksum is good
            rs485_ni=temp_ni;
         }

         #if (getenv("AUART")&&(RS485_USE_EXT_INT==FALSE))
            setup_uart(UART_ADDRESS);
         #endif

         state=0;
         return;

      default: // Get data
         rs485_add_to_temp(b);
         --len;
         break;
   }
   if ((state>=3) && (!len)) {
      state=255;
   }
   else {
      ++state;
   }
}


// Purpose:    Send a message over the RS485 bus
// Inputs:     1) The destination address
//             2) The number of bytes of data to send
//             3) A pointer to the data to send
// Outputs:    TRUE if successful
//             FALSE if failed
// Note:       Format:  source | destination | data-length | data | checksum
int1 rs485_send_message(int8 to, int8 len, int8* data) {
   int8 try, i, cs;
   int1 ret = FALSE;

   RCV_OFF();
   #if RS485_USE_EXT_INT
      disable_interrupts(GLOBAL);
   #endif

   for(try=0; try<5; ++try) {
      rs485_collision = 0;
      fputc((int16)0x100|rs485_id, RS485_CD);
      fputc((int16)0x100|to, RS485_CD);
      fputc(len, RS485_CD);

      for(i=0, cs=rs485_id^to^len; i<len; ++i) {
         cs ^= *data;
         fputc(*data, RS485_CD);
         ++data;
      }

      fputc(cs, RS485_CD);
      if(!rs485_collision) {
         ret = TRUE;
         break;
      }
      delay_ms(RS485_ID);
   }

   RCV_ON();
   #if RS485_USE_EXT_INT
      enable_interrupts(GLOBAL);
   #endif

   return(ret);
}


// Purpose:    Wait for wait time for the RS485 bus to become idle
// Inputs:     TRUE - restart the watch dog timer to prevent reset
//             FALSE - watch dog timer not restarted
// Outputs:    None
void rs485_wait_for_bus(int1 clrwdt)
{
   int16 i;

   RCV_OFF();
   for(i=0; i <= (rs485_wait_time*20); ++i)
   {
      if(!input(RS485_RX_PIN))
         i = 0;
      else
         delay_us(50);

      if(clrwdt)
         restart_wdt();
   }
}


// Purpose:    Get a message from the RS485 bus and store it in a buffer
// Inputs:     1) A pointer to a buffer to store a message
//             2) TRUE  - wait for a message
//                FALSE - only check if a message is available
// Outputs:    TRUE if a message was received
//             FALSE if wait is FALSE and no message is available
// Note:       Data will be filled in at the pointer as follows:
//             FROM_ID  DATALENGTH  DATA...
int1 rs485_get_message(int* data_ptr, int1 wait)
{
   while(wait && (rs485_ni == rs485_no)) {}

   if(rs485_ni == rs485_no)
      return FALSE;
   else {
      int n;
      n = rs485_buffer[(rs485_no+1)%sizeof(rs485_buffer)] + 2;

      for(; n>0; --n)
      {
         *data_ptr = rs485_buffer[rs485_no];
         if(++rs485_no >= sizeof(rs485_buffer))
         {
            rs485_no = 0;
         }
         ++data_ptr;
      }
      return TRUE;
   }
}

#endif
