///////////////////////////////////////////////////////////////////////////
////   Library for the servo motors on the Picrobot                    ////
////                                                                   ////
////                                                                   ////
////   * Note: This driver uses timer one to control pulses. Edit the  ////
////           line "#define TIMER_1_DIV" to configure timer one       ////
////           division.                                               ////
////                                                                   ////
////   init_servos()                                                   ////
////     Call before the set_servo() function is used                  ////
////                                                                   ////
////   void set_servo(int1 side, int1 direction, int8 speed)           ////
////     Call to set the speed and direction of a servo                ////
////        Inputs:                                                    ////
////           1) LEFT or RIGHT                                        ////
////           2) FORWARD or BACKWARD                                  ////
////           3) 0-4 for a speed. 0 is halt in either direction.      ////
////                                                                   ////
////   void stop_servos()                                              ////
////     Call to stop both servos from turning                         ////
////                                                                   ////
////   void pause_servos()                                             ////
////     Call to stop the servos and remember their speed setting      ////
////                                                                   ////
////   void resume_servos()                                            ////
////     Call to resume movement after a pause                         ////
////                                                                   ////
////   The main program may define LEFT_SERVO_PIN and RIGHT_SERVO_PIN  ////
////   to override the defaults below.                                 ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 1996, 2003 Custom Computer Services          ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////


#ifndef LEFT_SERVO_PIN
#define LEFT_SERVO_PIN     PIN_D6
#define RIGHT_SERVO_PIN    PIN_D7
#endif


///////////////////////////////////////////////////////////////////////////
//// Configure the timer one division setting
///////////////////////////////////////////////////////////////////////////
#ifndef TIMER_1_DIV
#define TIMER_1_DIV     2           // Number of divisions for timer 1
#endif
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//// The following may be configured to control the pulse width timing
//// All time units are seconds
///////////////////////////////////////////////////////////////////////////
#define SHORT_TIME      0.0009      // Shortest pulse width high time
#define CENTER_TIME     0.0015      // The high time for center
#define LONG_TIME       0.0021      // Longest pulse width high time
#define PULSE_TIME      0.0200      // The total time for the pulse
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
#define LEFT      0
#define RIGHT     1

#define FORWARD   0
#define BACKWARD  1
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//// Uncomment to allow the stop pulse to be generated for
//// calibrating the servos. Comment this line again after
//// calibrate to ensure the servos stop when requested.
///////////////////////////////////////////////////////////////////////////
//// #define CALIBRATE_SERVOS
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
#ifndef TIMER_RATE
#define TIMER_RATE      getenv("CLOCK") / 4 / TIMER_1_DIV
#endif
#define SHORT_TICKS     (int16)((float)TIMER_RATE * SHORT_TIME)
#define CENTER_TICKS    (int16)((float)TIMER_RATE * CENTER_TIME)
#define LONG_TICKS      (int16)((float)TIMER_RATE * LONG_TIME)
#define LOW_TICKS       (int16)(((float)TIMER_RATE * (PULSE_TIME - CENTER_TIME)) - 42)
#define PULSE_CHANGE    (int16)(LONG_TICKS - CENTER_TICKS)

const signed int16 servo_speeds[] = {0,
                                     PULSE_CHANGE / 15,
                                     PULSE_CHANGE / 4,
                                     PULSE_CHANGE / 3,
                                     PULSE_CHANGE};
signed int16 right_adjust;
signed int16 left_adjust;
int1 stop_right_servo = TRUE;
int1 stop_left_servo = TRUE;
void stop_servos();
///////////////////////////////////////////////////////////////////////////


// Purpose:    Initializes timer1, enables necessary interrupts,
//             and sets the servos to halt.
//             This must be called before set_servo()
void init_servos()
{
   #if TIMER_1_DIV == 1
      setup_timer_1(T1_DIV_BY_1 | T1_INTERNAL);
   #elif TIMER_1_DIV == 2
      setup_timer_1(T1_DIV_BY_2 | T1_INTERNAL);
   #elif TIMER_1_DIV == 4
      setup_timer_1(T1_DIV_BY_4 | T1_INTERNAL);
   #elif TIMER_1_DIV == 8
      setup_timer_1(T1_DIV_BY_8 | T1_INTERNAL);
   #endif
   setup_ccp1(CCP_COMPARE_INT);
   setup_ccp2(CCP_COMPARE_INT);

   stop_servos();

   #ifdef CALIBRATE_SERVOS
   stop_right_servo = FALSE;
   stop_left_servo  = FALSE;
   #endif

   enable_interrupts(INT_CCP1);
   enable_interrupts(INT_CCP2);
   enable_interrupts(GLOBAL);
}


// Purpose:    Control the direction and speed of a servo
// Inputs:     1) 0 for left side, 1 for right side
//             2) 0 for forward, 1 for backward
//             3) 0-4 for a speed setting
//                Speeds can be configured in the lookup table.
void set_servo(int1 side, int1 direction, int8 speed)
{
   if(speed < sizeof(servo_speeds)/2)
   {
      if(side)
      {
         if(speed == 0)
            stop_right_servo = TRUE;
         else
            stop_right_servo = FALSE;

         right_adjust = -servo_speeds[speed];

         if(direction)
            right_adjust = -right_adjust;
      }
      else
      {
         if(speed == 0)
            stop_left_servo = TRUE;
         else
            stop_left_servo = FALSE;

         left_adjust = servo_speeds[speed];

         if(direction)
            left_adjust = -left_adjust;
      }
   }
}


// Purpose:    Stop the servos from moving
void stop_servos()
{
   right_adjust = 0;
   left_adjust  = 0;
   stop_right_servo = TRUE;
   stop_left_servo  = TRUE;
}


// Purpose:    Stop the servos from moving,
//             but remember their speed settings
void pause_servos()
{
   stop_right_servo = TRUE;
   stop_left_servo  = TRUE;
}


// Purpose:    Resume the servos after a pause
void resume_servos()
{
   stop_right_servo = FALSE;
   stop_left_servo  = FALSE;
}


// Purpose:    Interrupt sevice routine for ccp1 is used to control the left servo
#int_ccp1
void isr_ccp1()
{
   static int1 TOGGLE_LEFT = 0;

   if(stop_left_servo == FALSE)
   {
      if(TOGGLE_LEFT)
      {
         output_low(LEFT_SERVO_PIN);         // Set the servo control pin to low
         CCP_1 += LOW_TICKS - left_adjust;   // Set CCP1 to interrupt for next high pulse
         TOGGLE_LEFT = 0;
      }
      else
      {
         output_high(LEFT_SERVO_PIN);        // Set the servo control pin to high
         CCP_1 += CENTER_TICKS + left_adjust;// Set CCP1 to interrupt for next low pulse
         TOGGLE_LEFT = 1;
      }
   }
}


// Purpose:    Interrupt sevice routine for ccp2 is used to control the right servo
#int_ccp2
void isr_ccp2()
{
   static int1 TOGGLE_RIGHT = 0;

   if(stop_right_servo == FALSE)
   {
      if(TOGGLE_RIGHT)
      {
         output_low(RIGHT_SERVO_PIN);
         CCP_2 += LOW_TICKS - right_adjust;
         TOGGLE_RIGHT = 0;
      }
      else
      {
         output_high(RIGHT_SERVO_PIN);
         CCP_2 += CENTER_TICKS + right_adjust;
         TOGGLE_RIGHT = 1;
      }
   }
}
