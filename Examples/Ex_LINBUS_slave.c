////                      LIN BUS Slave                              ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////                                                                 ////
//// This example works with the LIN_BUS.h driver. The slave is a    ////
//// 16F687/689/690 chip. The program may be modified to work with   ////
//// other PIC chips that have EUSART module                         ////
//// This example demonstrates Master/Slave Communication via LIN bus////
//// protocol. The Master sends the slave a command byte. The slave  ////
//// responds by transmitting its ADC value from a pot connected to  ////
//// AN0. The master can display or processing this data as desired  ////
////                                                                 ////
////                                                                 ////
/////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2007 Custom Computer Services        ////
//// This source code may only be used by licensed users of the CCS  ////
//// C compiler.  This source code may only be distributed to other  ////
//// licensed users of the CCS C compiler.  No other use,            ////
//// reproduction or distribution is permitted without written       ////   
////                                                                 ////
//// permission.  Derivative programs created using this software    ////
//// in object code form are not restricted in any way.              ////
/////////////////////////////////////////////////////////////////////////

#if defined(__PCM__)
#include <16F690.h>
#device ADC=10
#fuses INTRC, NOWDT

#use delay(clock=8000000)
#use rs232(baud=9600, Xmit=Pin_B7, RCV=Pin_B5, ERRORS)




#include<LINBUS.h>


// Function Prototypes
void Transmit_adc_value();


void main()
{
int8 id_byte, msg_length, addr, SPBRG_temp, temp;
int8 data[8];

   setup_adc(ADC_OFF);
   setup_adc_ports(NO_ANALOGS);
   
   setup_comparator(NC_NC_NC_NC);
    while(1)
           {
            delay_ms(300);
            
            output_low(MCP201_CS);
            delay_ms(1); // Toggle the CS pin at least 1 ms after wake-up
            output_high(MCP201_CS);
            
            wait_for_master();
            delay_ms(10);

            //Desired Baud rate range - 1200 baud - 19200
            //  if((SPBRG < 5)|| (SPBRG>104))
            //   wait_for_master();

            // Work-around for Errata
            SPBRG= SPBRG-1; // Subtract 1 to get correct Baud Rate- Refer Errata sheet
   
            SPBRG_temp = SPBRG; // Read SPBRG and write back to SPBRG (see errata)
            SPBRG = SPBRG_temp;
            TXEN = 1; // Set the TXEN bit to enable the transmitter (See Errata)
   
   
            while(1)
                   {


                    id_byte = read_ident_byte();


                    msg_length = get_message_length(id_byte);

                    Errorflag =0; // Reset Error Flag
                    Check_parity_bits(id_byte); 
                    addr = address_match(id_byte);

                    if (Errorflag==0)
                     {


                       if(addr== SLAVE_ADDRESS || addr==MASTER_ADDRESS)

                         {

                           read_data_packet(&data[0], msg_length);

                           
                           linbus_verify_checksum(&data[0],msg_length);
               

                          if(addr==MASTER_ADDRESS)
                          
                            Transmit_adc_value();
                            delay_ms(5); // not required
                            temp=getc();
                         }
                     else
                     
                     break; // If Address match does not occur, RESET states

                     }
                     else // DEBUG Pulse on RC5
                     {
                     output_high(Pin_C5);
                     delay_ms(1);
                     output_low(Pin_C5);
                     }

                  }
             }
 }


//This function will read 10 bit ADC value from AN0 and transmit it to Master
 void Transmit_adc_value()
 {
   int16 adc_val;
   int8 reply[2];

   int8 msg_length = 2; // Length of Response

   // Init
   reply[0]=0;
   reply[1]=0;

   // ADC ports initialization
   setup_port_a(ALL_ANALOG);
   setup_adc(adc_clock_internal);
   set_adc_channel(0);
   delay_ms(1);
   adc_val = read_adc();


   reply[0] = make8(adc_val,0);
   reply[1] = make8(adc_val,1);
   reply[1]= reply[1] & 0x03; // set to 10 bit output

   delay_ms(4);
   setup_port_a(NO_ANALOGS);
   setup_adc(ADC_OFF);

   // Transmit the acquired data over LIN bus
   Lin_bus_transmit_data(MASTER_ADDRESS, msg_length, &reply[0]);

 }
