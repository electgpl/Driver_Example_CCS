////////////////////////////////////////////////////////////////////////////////
////                               EX_J1939.c                               ////
////                                                                        ////
//// Example of CCS's J1939 driver.  This example was tested using the CCS  ////
//// CAN Bus 24 Development kit.                                            ////
////                                                                        ////
//// This example will send a message once every second commanding Node B   ////
//// to toggle it's LED once every second.  Requires that EX_J1939B.c be    ////
//// programmed onto Node B of the development kit.  Also pressing the push ////
//// will cause Node A to send out a Global Address Request, causing all    ////
//// device to respond with their claimed address.  Which will be displayed ////
//// over RS232.                                                            ////
////                                                                        ////
//// This example will work with the PCM, PCH and PCD compilers.  The       ////
//// is written to work with the PCD and PCH compiler.  Change the device,  ////
//// clock, RS232 settings, PIN defines and tick timer setting for hardware ////
//// if needed.                                                             ////
////                                                                        ////
////////////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996,2012 Custom Computer Services                ////
//// This source code may only be used by licensed users of the CCS         ////
//// C compiler.  This source code may only be distributed to other         ////
//// licensed users of the CCS C compiler.  No other use,                   ////
//// reproduction or distribution is permitted without written              ////
//// permission.  Derivative programs created using this software           ////
//// in object code form are not restricted in any way.                     ////
////////////////////////////////////////////////////////////////////////////////

#if defined(__PCD__)
#include <24HJ256GP610.h>
#elif defined(__PCH__)
#include <18F4580.h>
#endif
#fuses NOWDT
#use delay(crystal=20MHz)
#use rs232(UART1,baud=115200)

#define LED_PIN      PIN_B1
#define PUSH_BUTTON  PIN_A4

#include <stdint.h>

void InitJ1939Address(void);
void InitJ1939Name(void);

//////////////////////////////////////////////////////////////////////////////// Tick Timer
#define TICKS_PER_SECOND 1000

typedef uint32_t TICK_TYPE;
static TICK_TYPE TimerTick;

TICK_TYPE GetTick(void)
{
   return(TimerTick);
}

TICK_TYPE GetTickDifference(TICK_TYPE current,TICK_TYPE previous)
{
   return(current - previous);
}

#INT_TIMER2
void tick_time_isr(void)
{
   TimerTick++;
}

//////////////////////////////////////////////////////////////////////////////// J1939 Settings

//Following Macros used to initialize unit's J1939 Address and Name - Required
#define J1939InitAddress()    InitJ1939Address()
#define J1939InitName()       InitJ1939Name()

//Following defines what the CAN's baud rate it, not required defaults to 250Kbit 
//only necessary if using non standard baud rate.  Was changed to 125Kbit to
//work on CCS CAN Bus and CAN Bus 24 development kit.
#define J1939_BAUD_RATE    125000

//Following defines sets up the CAN baud rate, not required if using 250Kbit or 
//500Kbit and clock rates of 8, 16, 20, 32 or 40MHz.
#if defined(__PCD__)
#define CAN_BRG_PRESCALAR           4
#define CAN_BRG_PHASE_SEGMENT_1     2
#define CAN_BRG_PHASE_SEGMENT_2     2
#define CAN_BRG_SYNCH_JUMP_WIDTH    0
#define CAN_BRG_PROPAGATION_TIME    0
#elif defined(__PCH__)
#define CAN_BRG_PRESCALAR           4
#define CAN_BRG_PHASE_SEGMENT_1     6
#define CAN_BRG_PHASE_SEGMENT_2     6
#define CAN_BRG_SYNCH_JUMP_WIDTH    0
#define CAN_BRG_PROPAGATION_TIME    0
#endif

//Following defines/macros used to associate your tick timer to J1939 tick timer
// defines/macro's - Required
#define J1939GetTick()                 GetTick()
#define J1939GetTickDifference(a,b)    GetTickDifference(a,b)
#define J1939_TICKS_PER_SECOND         TICKS_PER_SECOND
#define J1939_TICK_TYPE                TICK_TYPE

//Include the J1939 driver
#include <j1939.c>

//Defines for J1939 Commands used in this example
#define LED_ON       50
#define LED_OFF      51
#define LED_TOGGLE   52

//Define for other Node's J1939 address used in this example
#define OTHER_NODE_ADDRESS    129

//Function used to initialize this unit's J1939 Address
void InitJ1939Address(void)
{
   g_MyJ1939Address = 128;
}

//Function used to initialize this unit's J1939 Name
void InitJ1939Name(void)
{
   g_J1939Name[0] = 0;
   g_J1939Name[1] = 0;
   g_J1939Name[2] = 0;
   g_J1939Name[3] = 0;
   g_J1939Name[4] = 0;
   g_J1939Name[5] = 0;
   g_J1939Name[6] = 0;
   g_J1939Name[7] = 128;
}

//J1939 Task function for this example
void J1939Task(void)
{
   uint8_t i;
   uint8_t Data[8];
   uint8_t Length;
   J1939_PDU_STRUCT Message;
   
   TICK_TYPE CurrentTick;
   static TICK_TYPE PreviousTick;

   CurrentTick = GetTick();

   J1939ReceiveTask();  //J1939ReceiveTask() needs to be called often
   J1939XmitTask();     //J1939XmitTask() needs to be called often
   
   if(J1939Kbhit())  //Checks for new message in J1939 Receive buffer
   {
      J1939GetMessage(Message,Data,Length);  //Gets J1939 Message from receive buffer
      
      if(Message.PDUFormat == LED_ON)                       //If J1939 PDU Format is LED_ON, turn LED on
         output_low(LED_PIN);
      else if(Message.PDUFormat == LED_OFF)                 //If J1939 PDU Format is LED_OFF, turn LED off
         output_high(LED_PIN);
      else if(Message.PDUFormat == LED_TOGGLE)              //If J1939 PDU Format is LED_TOGGLE, toggle LED
         output_toggle(LED_PIN);
      else if(Message.PDUFormat == J1939_PF_ADDR_CLAIMED)   //If J1939 PDU Format is Address Claim, print name and address being claimed
      {
         printf("\r\nName: 0x");
         for(i=0;i<Length;i++)
            printf("%02X",Data[7-i]);
         printf(", Address: %03u",Message.SourceAddress);
      }
   }      
   
   if(GetTickDifference(CurrentTick,PreviousTick) >= (TICK_TYPE)TICKS_PER_SECOND)
   {
      //send message to other unit once every second to toggle pin
      Message.SourceAddress = g_MyJ1939Address;          //set PDU Source Address, this units address (g_MyJ1939Address)
      Message.DestinationAddress = OTHER_NODE_ADDRESS;   //set PDU Destination Address, address of other unit
      Message.PDUFormat = LED_TOGGLE;                    //set PDU Formate, LED_TOGGLE command
      Message.DataPage = 0;                              //set PDU Data Page can be either 0 or 1, this message uses 0
      Message.ExtendedDataPage = 0;                      //set PDU Extended Data Page, must be zero for J1939 Messages
      Message.Priority = J1939_CONTROL_PRIORITY;         //set Priority, can be 0 to 7 (0 highest priority) Control default is 3
      
      //Load PGN of Message (refer to J1939 documentation for correct format)
      Data[0] = Message.SourceAddress;
      Data[1] = Message.PDUFormat;
      Data[2] = 0;
      
      J1939PutMessage(Message,Data,3);    //loads J1939 Message into Xmit buffer
      
      PreviousTick =  CurrentTick;
   }
}

int1 CurrentPushButtonState,PreviousPushButtonState;

void InputTask(void)
{
   int1 state;
   
   state = input(PUSH_BUTTON);
   
   delay_us(50);
   
   if(state == input(PUSH_BUTTON))
     CurrentPushButtonState = state;     
}

void main()
{
  #if defined(__PCD__)
   setup_timer2(TMR_INTERNAL,10000); //1ms tick
  #else
   setup_timer_2(T2_DIV_BY_4, 250, 5); //1ms tick
  #endif
   enable_interrupts(INT_TIMER2);
  #if defined(__PCD__)
   enable_interrupts(INTR_GLOBAL);
  #else
   enable_interrupts(GLOBAL);
  #endif

   J1939Init();  //Initialize J1939 Driver must be called before any other J1939 function is used
   
   CurrentPushButtonState = PreviousPushButtonState = input(PUSH_BUTTON);
   
   while(TRUE)
   {
      J1939Task();
      
      InputTask();
      
      if(PreviousPushButtonState != CurrentPushButtonState)
      {
         if(CurrentPushButtonState == FALSE)
            J1939RequestAddress(J1939_GLOBAL_ADDRESS);   //Sends J1939 Address request, sending to global address
                                                         //causes all units on bus to respond with their address
         PreviousPushButtonState = CurrentPushButtonState;
      }
   }
}
