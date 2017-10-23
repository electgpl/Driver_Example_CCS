///////////////////////////////////////////////////////////////////////////
////                             SIM.C                                 ////
////                       Sim Card Driver and                         ////
////                        helper functions                           ////
////                                                                   ////
////  sim_init()     Must be called before any other function.         ////
////                                                                   ////
////  getNumber()    Reads Phone Number, Phone Number Name, and Phone  ////
////                 Number Length into phone number structure from    ////
////                 Sim Card                                          ////
////                                                                   ////
////  writeNunber()  Writes Phone Number, Phone Number Name, and Phone ////
////                 Number Length to Sim Card                         ////
////                                                                   ////
////  The SIM.C driver uses the PIC rs232 hardware and RDA interrupt   //// 
////  to communicate with the SIM Card, it uses the Timer2 and the     //// 
////  CCP1 module to generate the SIM Card Clock, and it uses the I/O  //// 
////  pins C3 and C5 for SIM_RST, and SIM_VCC.                         ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2009 Custom Computer Services           ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#ifndef CLOCK_PWM
   #define CLOCK_PWM 5000000
#endif

#define ATR_TIMEOUT   (40000*10000)/CLOCK_PWM  ///timeout (in 100us) for ATR, which is 40,000 clocks

#ifndef SIM_RST
#define SIM_RST   PIN_C3
#endif

#ifndef SIM_VCC
#define SIM_VCC   PIN_C5
#endif

#if defined(__PCH__)
 #bit RS232_TBE=0xFAC.1  //TXSTA.TRMT
#else
 #error GET THE TSR EMPTY BIT FOR PIC16
#endif

#if CLOCK_PWM==5000000
 #use rs232(baud=13441, xmit=PIN_C6, rcv=PIN_C7, PARITY=E, stream=SIMIO)
#elif CLOCK_PWM==2500000
 #use rs232(baud=6720, xmit=PIN_C6, rcv=PIN_C7, PARITY=E, stream=SIMIO)
#else
 #error Define SIM card baud rate for your PWM clock here
#endif
//57600
#if defined(USE_DEBUGGER)
   #use rs232(DEBUGGER,stream=DEBUG)
#endif

#ifndef SIMIO_BUFFER_SIZE
#define SIMIO_BUFFER_SIZE  80
#endif

#define record_size 30           //Size of Records on SIM Card
#define num_records 250          //Total number of records
#define alpha_size 16            //Number of Bytes the Name can be on Sim Card
                                 //These numbers can be found with sim_getResponse_EF() function
                                 
typedef struct {
   char name[alpha_size];
   int8 number[11];
   int8 length;
}phone_number;

int8 simio_next=0, simio_last=0;
int8 simio_buffer[SIMIO_BUFFER_SIZE];

#define bkbhit() (simio_next!=simio_last)

#int_rda
void isr_rda(void) {
   char c;
   c=fgetc(SIMIO);
   simio_buffer[simio_next++]=c;
   if (simio_next >= SIMIO_BUFFER_SIZE) {simio_next=0;}
}

char bgetc(void) {
   char c;
   c=simio_buffer[simio_last++];
   if (simio_last >= SIMIO_BUFFER_SIZE) {simio_last=0;}
   return(c);
}

//puts a character to the SIM one-wire interface.  since it's a one-wire
//interface this routine prevents the echo from getting into the receive
//buffer.
void simputc(char c) {
   disable_interrupts(INT_RDA);
   fputc(c,SIMIO);

   while(!RS232_TBE) {} //wait until transmission is complete

   #if (CLOCK_PWM==5000000)
      // 893
      delay_us(893);    //12bit gaurd time  (5.0MHz PWM)
   #elif (CLOCK_PWM==2500000)
      // 1786
      delay_us(1786);   //12bit gaurd time  (2.5MHz PWM)
   #else
      #error Calculate 12bit gaurd time for your PWM here
   #endif

   fgetc(SIMIO);
   enable_interrupts(INT_RDA);
}

int8 sim_swapbits(int8 d) {
   int8 temp=0;
   if (bit_test(d,0)) {bit_set(temp,7);}
   if (bit_test(d,1)) {bit_set(temp,6);}
   if (bit_test(d,2)) {bit_set(temp,5);}
   if (bit_test(d,3)) {bit_set(temp,4);}
   if (bit_test(d,4)) {bit_set(temp,3);}
   if (bit_test(d,5)) {bit_set(temp,2);}
   if (bit_test(d,6)) {bit_set(temp,1);}
   if (bit_test(d,7)) {bit_set(temp,0);}
   return(temp);
}

//we got an ATR.
int8 got=0;

//list to SIM card and print any results
//if we are waiting for the ATR, switch RST after 40,000 clock cycles.
//if we switched RST, if after 40,000 clocks we don't get an ATR then giveup.
void sim_read_task(void) {
   static int16 to=ATR_TIMEOUT;  //40,000 clock timeout
   static int8 low;
   char c;

   while (bkbhit()) {
      c=bgetc();
      if (!got && (c==0x3B)) {got=TRUE;}
      #if defined(USE_DEBUGGER)
         fprintf(DEBUG,"%X",c);
      #endif
   }
   if (to) {
      to--;
      if (!to && !got) {
         if (!low) {
            low=1;
            output_high(SIM_RST);
            #if defined(USE_DEBUGGER)
               fprintf(DEBUG, "\r\n\nSETTING RST HIGH\r\n");
            #endif
            to=ATR_TIMEOUT;
         }
         else
         {
            #if defined(USE_DEBUGGER)
               fprintf(DEBUG, "\r\n\nGIVING UP!\r\n");
            #endif
         }
      }
   }
}

//2 seconds after an ATR, send a FF00FF to the sim card.  the sim card should
//respond with a FF00FF.  don't forget that since this is one wire interface
//you will receive an echo immediately, so flush it.
void sim_write_task(void) {
   static int16 to=20000;
   if (got) {
      if (to) {
         to--;
         if (!to) {
            #if defined(USE_DEBUGGER)
               fprintf(DEBUG,"\r\nSending SEQ\r\n");
            #endif
            simputc(0xFF);
            simputc(0);
            simputc(0xFF);
         }
      }
   }
}

//initliaze PIC and SIM contacts.
void sim_init(void) {
   int32 ATR;
   
   ATR = 20000;

   output_low(SIM_RST);
   output_high(SIM_VCC);

   #if defined(USE_DEBUGGER)
      fprintf(DEBUG,"\r\nSIMCARD TEST (new init)\r\nListening....\r\n\n");
   #endif

   enable_interrupts(GLOBAL);
   enable_interrupts(INT_RDA);

   //make sure to configure PWM to output at CLOCK_PWM frequency
   setup_timer_2(T2_DIV_BY_1,1,1);
   setup_ccp1(CCP_PWM);
   set_pwm1_duty(1);

   // this is the sim initialization code
   while(ATR > 0) {
      ATR--;
      restart_wdt();
      sim_read_task();
      sim_write_task();
      delay_us(100);
   }
}

//deactivate the SIM contacts
void sim_deinit(void) {
   output_low(SIM_RST);
   setup_ccp1(CCP_OFF);
   output_low(PIN_C2);
   output_high(PIN_C6);
   output_low(SIM_VCC);
   //while(TRUE) {}
}

//write a commmand to the SIM card
void sim_write_command(int8 cla, int8 ins, int8 p1, int8 p2, int8 p3)
{
   simputc(cla);
   simputc(ins);
   simputc(p1);
   simputc(p2);
   simputc(p3);
}

// listen to the IO line
// returns the number of bytes in the buffer
int8 sim_listen(int8 * buffer)
{
   int8 index;        // contains the number of bytes heard
   int16 timer;        // holds the count down

   index = 0;
   timer = 50000;
   while(timer > 0)
   {
      timer--;
      if(bkbhit())
      {
         buffer[index]=bgetc();
         timer=50000;
         index++;
      }

      if(index==0)
         timer=50000;
   }
   return index;
}

// calls the get response command
int8 sim_getResponse(char* buffer,int8 length)
{
   int8 index;

   sim_write_command(0xA0,0xC0,0x00,0x00,length);

   index = sim_listen(buffer);

   return index;
}

// calls the get status command
int8 sim_getStatus(char* buffer, int8 length)
{
   int8 index;

   sim_write_command(0xA0,0xF2,0x00,0x00,length);

   index = sim_listen(buffer);

   return index;
}

// Changes the address of the current file
int16 sim_switchAddr(int16 address)
{
   int8 high;   // hold high part of address
   int8 low;    // hold low part of address
   int8 index;
   int8 inputArray[100];
   int16 returnValue;

   high = make8(address,1);
   low = make8(address,0);

   sim_write_command(0xA0,0xA4,0x00,0x00,0x02);

   index = sim_listen ( inputArray );

   simputc(high);
   simputc(low);

   index = sim_listen ( inputArray );

   returnValue=make16(inputArray[index-2],inputArray[index-1]);
   return returnValue;
}

// reads the binary file,
// buffer is a pointer to a buffer
// offset is the offset from the beginning of the file
// length is the number of bytes to read
int8 sim_readBinary(char* buffer,int16 offset,int8 length)
{
   int8 index;
   int8 high;
   int8 low;

   high = make8(offset,1);
   low = make8(offset,0);

   sim_write_command(0xA0,0xB0,high,low,length);

   index=sim_listen(buffer);

   return index;
}

// reads a record
// buffer is a poniter to a character buffer
// number is the record number
// mode is the mode of reading
// length is the length to read
int8 sim_readRecord(char* buffer, int8 number, int8 mode, int8 length)
{
   int8 index;

   sim_write_command(0xA0,0xB2,number,mode,length);

   index=sim_listen(buffer);

   return index;
}

// updates a record
// buffer is a pointer to a character buffer
// number is the record number
// mode is the mode of writing
// length is the length to write
void sim_updateRecord(char* buffer, int8 number, int8 mode, int8 length)
{
   int8 index;
   int8 tempbuffer[5];
   int8 i;

   sim_write_command(0xA0,0xDC,number,mode,length);

   index=sim_listen(tempbuffer);

   for(i=0;i<length;i++)
   {
      simputc(buffer[i]);
   }
   delay_ms(75);
}

// writes the pin number
int8 sim_writePin(char* PIN)
{
   int8 buffer[20];
   int8 index;

   sim_write_command(0xA0,0x20,0x00,0x01,0x08);

   sim_listen(buffer);

   simputc(PIN[0]);
   simputc(PIN[1]);
   simputc(PIN[2]);
   simputc(PIN[3]);
   simputc(PIN[4]);
   simputc(PIN[5]);
   simputc(PIN[6]);
   simputc(PIN[7]);

   index=sim_listen(buffer);
   return index;
}

void sim_unblockCHV(char* PIN, char* newPIN)
{
   int8 buffer[1];
   int8 i;

   sim_write_command(0xA0,0x2C,0x00,0x00,0x10);

   sim_listen(buffer);

   for(i=0;i<8;i++)
      simputc(PIN[i]);
   for(i=0;i<8;i++)
      simputc(newPIN[i]);
}

// get specifed number and return it in the structure specified
int1 getNumber(phone_number &number,int8 slot)
{
   int8 i;
   int8 temp[record_size+1];
   
   // move to phone number address
   sim_switchAddr(0x3f00);
   sim_switchAddr(0x7f10);
   sim_switchAddr(0x6f3a);

   if(slot==0 || slot>num_records)
      return(0);
      
   sim_readRecord(temp,slot,0x04,record_size);

   if(temp[1]!=0xFF)
   {
      #if defined(USE_DEBUGGER)
         fprintf(DEBUG,"\n\r");
      
         for(i=0;i<=record_size;i++)
         {
            fprintf(DEBUG,"%X ",temp[i]);
         }
      #endif
      
      for(i=1;i<(alpha_size+1);i++)
      {
         if(temp[i]!=0xff)
            number.name[i-1]=temp[i];
         else
            number.name[i-1]=0;
      }
      
      if(temp[alpha_size+1]==7)
      {
         for(i=(alpha_size+3);i<(alpha_size+8);i++)
         {
            number.number[((int16)i*2)-((int16)(alpha_size+3)*2)]=(temp[i] & 0x0F);
            number.number[((int16)i*2)-(((int16)(alpha_size+3)*2)-1)]=((temp[i] & 0xF0) >> 4);
         }
         number.number[10]=(temp[alpha_size+8] & 0x0F);
         number.length=11;
      }
      else
      {
         for(i=(alpha_size+3);i<(alpha_size+6);i++)
         {
            number.number[((int16)i*2)-((int16)(alpha_size+3)*2)]=(temp[i] & 0x0F);
            number.number[((int16)i*2)-(((int16)(alpha_size+3)*2)-1)]=((temp[i] & 0xF0) >> 4);
         }
         number.number[6]=(temp[alpha_size+6] & 0x0F);
         for(i=7;i<11;i++)
            number.number[i]=0xFF;
         number.length=7;
      }
      return TRUE;
   }
   return FALSE;
}

// write phone number to specified record
void writeNumber(phone_number &number, unsigned int8 slot)
{
   int1 flag=0;
   int8 i;
   int8 temp[record_size];
   
   sim_switchAddr(0x3f00);
   sim_switchAddr(0x7f10);
   sim_switchAddr(0x6f3a);
   
   for(i=0;i<alpha_size;i++)
   {
      if(number.name[i]==0 || flag==1)
      {
         temp[i]=0xff;
         flag=1;
      }
      else
         temp[i]=number.name[i];
   }
   
   if(number.length==11)
   {
      temp[alpha_size]=7;
      temp[alpha_size+1]=0x81;
      
      for(i=0;i<9;i+=2)
      {
         temp[((i)/2)+(alpha_size+2)]=((number.number[i+1] << 4) | number.number[i]);
      }
      
      temp[alpha_size+7]=(0xf0 | number.number[10]);
      
      for(i=(alpha_size+8);i<record_size;i++)
      {
         temp[i]=0xff;
      }
   }
   else
   {
      temp[alpha_size]=5;
      temp[alpha_size+1]=0x81;
      
      for(i=0;i<5;i+=2)
      {
         temp[((i)/2)+(alpha_size+2)]=((number.number[i+1] << 4) | number.number[i]);
      }
      
      temp[alpha_size+5]=(0xF0 | number.number[6]);
      
      for(i=(alpha_size+6);i<record_size;i++)
      {
         temp[i]=0xFF;
      }
   }
   
   sim_updateRecord(temp,slot,0x04,record_size);
   sim_listen(temp);
}

int8 sim_getResponse_EF(char* buffer,int8 length)
{
   int8 index;
   
   sim_switchAddr(0x3f00);
   sim_switchAddr(0x7f10);
   sim_switchAddr(0x6f3a);

   sim_write_command(0xA0,0xC0,0x00,0x00,length);

   index = sim_listen(buffer);

   return index;
}
