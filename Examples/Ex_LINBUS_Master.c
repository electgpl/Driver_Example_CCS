////                      LIN BUS Mater                                  ////
////                                                                     ////
/////////////////////////////////////////////////////////////////////////////
////                                                                     ////
//// This example works with the LIN_BUS.h driver. The Master is a       ////
//// 16F687/689/690 chip. The program may be modified to work with       ////
//// other PIC chips that have EUSART module                             ////
//// This example demonstrates Master/Slave Communication via LIN bus    ////
//// protocol. The Master sends the slave a command byte. The slave      ////
//// responds by transmitting its ADC value from a pot connected to      ////
//// AN0. The master can display or processing this data as desired      ////
////                                                                     ////
////                                                                     ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2007 Custom Computer Services            ////
//// This source code may only be used by licensed users of the CCS      ////
//// C compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use,                ////
//// reproduction or distribution is permitted without written           ////
//// permission.  Derivative programs created using this software        ////
//// in object code form are not restricted in any way.                  ////
/////////////////////////////////////////////////////////////////////////

#include <16F690.h>
#fuses INTRC, NOWDT

#use delay(clock=8000000)
#use rs232(baud=9600, Xmit=Pin_B7, RCV=Pin_B5, ERRORS)



#include<LINBUS.h>


/////////////////////
void main()
 {
   int i,msg_length, id_byte, addr, temp;
   int16 adc_val;

   int data[8];
   int slave_addr = SLAVE_ADDRESS;
   int8 command[2]={0xCC,0xA1}; // This command is not of much significance
   msg_length = 8; // NOTE - Message Length can only be 2,4 or 8 bytes long

   setup_adc(ADC_OFF);
   setup_adc_ports(NO_ANALOGS);
   
   setup_comparator(NC_NC_NC_NC);

   
      while(1)
          {
           delay_ms(300);


           output_low(MCP201_CS);
           delay_ms(1);// Toggle the CS pin at least 1 ms after wake-up
           output_high(MCP201_CS);


          delay_ms(200);
          setup_uart(UART_SEND_BREAK);  //This sends a break character (zero for 12 bit time)
          
          temp=getc(); // Do a read after each write to clear RCREG

       // Send Dummy character - Automatically gets set to zero
       // putc(0x00); // The later version(4.???) of the compiler will send a putc automatically after UART_SEND_BREAK command
       // not required in new version of software
       // delay_ms(3);
          linbus_generate_synchfield(); // Send Sync Character immidiately after break
          temp=getc(); // Do a read after each write to clear RCREG

          delay_ms(10); // Set delay - Allow slaves to respond

          // Initialize the message length
          msg_length =2;
              while(1)
                   {
                    delay_ms(50);

                    // Send command to slave to read ADC
                    //The following function Calculates the ID byte, and
                    // transmits the data, followed by the checksum
                    Lin_bus_transmit_data(MASTER_ADDRESS, msg_length, &COMMAND[0]);
                     
                    delay_ms(5); // wait for slave to complete task

                    // Perform read routine
                    // Clear Read Interrupt
                    // clear_interrupt(INT_RDA);

                    
                                     
                    id_byte = read_ident_byte();
                   
                    msg_length = get_message_length(id_byte);

                    Errorflag =0; // Reset Error flag
                    Check_parity_bits(id_byte);
                    addr = address_match(id_byte);

                   if(addr == MASTER_ADDRESS)
                      {

                        read_data_packet(&data[0], msg_length);                      
                      
                       
                        linbus_verify_checksum(&data[0],msg_length);                      

                     

                        // Use Pin C6 and C7 to display the  received data
                        #use rs232(baud=9600, Xmit=Pin_C6, RCV=Pin_C7,ERRORS)
                        
                       if(Errorflag!=0)
                            {
                             putc(Errorflag);
                             break;
                            }
                       else
                          {
                           delay_ms(5);
                           adc_val = make16(data[1],data[0]);
                           printf("\n\r ADC Value - %lu",adc_val);
                            }
                      }
                      
                   }
          }
   }



