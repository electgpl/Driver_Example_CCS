///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 2008 Custom Computer Services                ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#ifndef TLV320AIC12K
#define TLV320AIC12K

#ifndef CODEC_SCL
#define CODEC_SCL PIN_G2
#define CODEC_SDA PIN_G3
#endif

#define codec_address   0x80

#use I2C(MASTER, SCL=CODEC_SCL, SDA=CODEC_SDA, STREAM=codec_i2c)

#define  REG_5A   0x00
#define  REG_5B   0x40
#define  REG_5C   0x80
#define  REG_5D   0xC0

/* The following are valid register addresses */
#define  NO_OPERATION   0
#define  REGISTER_1     1
#define  REGISTER_2     2
#define  REGISTER_3     3
#define  REGISTER_4     4
#define  REGISTER_5     5
#define  REGISTER_6     6

////////////////////////////////////////////////////////////////// Control Reg 1
//
// Constants used in codec_write_register(REGISTER_1, data) are:
//      (or (via |) together one constant from each group)
#define  ADOVF    (1 << 7)    // A/D Converter overflow (active high, read only)

#define  CONT_TX  (1 << 6)    // Continuous transfer mode 
#define  PROG_TX  (0 << 6)    // Programming mode 

#define  IIR      (1 << 5)    // IIR select 
#define  FIR      (0 << 5)    // FIR filter select 

#define  DAOVF    (1 << 4)    // D/A Converter overflow (active high, read only)

#define  BIAS_135 (1 << 3)    // BIAS pin output voltage = 1.35V
#define  BIAS_235 (0 << 3)    // BIAS pin output voltage = 2.35V 

#define  ALB_ON   (1 << 2)    // Analog Loop back on
#define  ALB_OFF  (0 << 2)    // Analog Loop back off 

#define  DLB_ON   (1 << 1)    // Digital Loop back on
#define  DLB_OFF  (0 << 1)    // Digital Loop back off 

#define  DAC16    (1 << 0)    // DAC 16-bit data format 
#define  DAC15    (0 << 0)    // DAC 15-bit data format 


////////////////////////////////////////////////////////////////// Control Reg 2
//
// Constants used in codec_write_register(REGISTER_2, data) are:
//      (or (via |) together one constant from each group)
#define  TURBO_MODE  (1 << 7)    // SCLK = MCLK/P
#define  NORMAL_MODE (0 << 7)    // SCLK = (16 * FX * #Devices * mode) 

#define  DIF_OFF     (1 << 6)    // Decimation/Interpolation filter bypassed 
#define  DIF_ON      (0 << 6)    // Decimation/Interpolation filter enabled 

#define  I2C_ADDR    0x20        // I2C address begins with 0b100 (must be used on each write!)

#define  I2C_INT     0x00        // SDA and SCL are used for I2C interface 
#define  S2C_INT     0x01        // SDA and SCL are used for S2C interface
#define  I_O_INT     0x02        // SDA => GPO, input into SCL is output to DOUT
#define  FRAME       0x03        // Control Frame Flag


////////////////////////////////////////////////////////////////// Control Reg 3
//
// Constants used in codec_write_register(REGISTER_3, data) are:
//      (or (via |) together one constant from each group)
#define  AD_OFF   (1 << 7)    // A/D Power off 
#define  AD_ON    (0 << 7)    // A/D Power on 

#define  DA_OFF   (1 << 6)    // D/A Power off 
#define  DA_ON    (0 << 6)    // D/A Power on 

#define  RESET    (1 << 5)    // S/W reset the codec
#define  NO_RESET (0 << 5)    // Do no S/W reset the codec

#define  DAC_512  0x18        // OSR for DAC Channel is 512 (Max Fs = 8 ksps)
#define  DAC_256  0x10        // OSR for DAC Channel is 256 (Max Fs = 16 ksps)
#define  DAC_128  0x00        // OSR for DAC Channel is 128 (Max Fs = 26 ksps)

#define  ASRF1    0x01        // Asynchronous Sampling Rate Factor is 1 (use this if only one codec is present)
#define  ASRF2    0x02        // Asynchronous Sampling Rate Factor is 2
#define  ASRF3    0x03        // Asynchronous Sampling Rate Factor is 3
#define  ASRF4    0x04        // Asynchronous Sampling Rate Factor is 4
#define  ASRF5    0x05        // Asynchronous Sampling Rate Factor is 5
#define  ASRF6    0x06        // Asynchronous Sampling Rate Factor is 6
#define  ASRF7    0x07        // Asynchronous Sampling Rate Factor is 7
#define  ASRF8    0x00        // Asynchronous Sampling Rate Factor is 8


////////////////////////////////////////////////////////////////// Control Reg 5
//
// Constants used in codec_setup_gains() are:
#define  ADC_NO_GAIN    0x2A                 // 0 dB (unity) gain
#define  ADC_MAX_GAIN   (ADC_NO_GAIN + 20)   // Do not use a value > 20 dB
#define  ADC_MIN_GAIN   (ADC_NO_GAIN - 42)   // Do not use a value < -42 dB

#define  DAC_NO_GAIN    0x2A                 // 0 dB (unity) gain
#define  DAC_MAX_GAIN   (DAC_NO_GAIN + 20)   // Do not use a value > 20 dB
#define  DAC_MIN_GAIN   (DAC_NO_GAIN - 42)   // Do not use a value < -42 dB

#define  ST_MUTE_ATTEN  0x38     
#define  ST_21dB_ATTEN  0x30
#define  ST_18dB_ATTEN  0x28
#define  ST_15dB_ATTEN  0x20
#define  ST_12dB_ATTEN  0x18
#define  ST_9dB_ATTEN   0x10
#define  ST_6dB_ATTEN   0x08
#define  ST_3dB_ATTEN   0x00

#define  IN_24dB_GAIN   0x03
#define  IN_12dB_GAIN   0x02
#define  IN_6dB_GAIN    0x01
#define  IN_NO_GAIN     0x00


////////////////////////////////////////////////////////////////// Control Reg 6
//
// Constants used in codec_write_register(REGISTER_6, data) are:
//      (or (via |) together one constant from each group)
#define  SINGLE   (1 << 7)    // Outputs OUTP2 and OUTP3 are single-ended
#define  DIFF     (0 << 7)    // Outputs OUTP2 and OUTP3 are differential w/ OUTMV common inverting output

#define  MUTE2    (1 << 6)    // OUTP2 is muted
#define  NO_MUTE2 (0 << 6)    // OUTP2 is not muted

#define  MUTE3    (1 << 5)    // OUTP3 is muted
#define  NO_MUTE3 (0 << 5)    // OUTP3 is not muted 

#define  ODR3_ON  (1 << 4)    // OUTP3 driver is ON
#define  ODR3_OFF (0 << 4)    // OUTP3 driver is OFF

#define  ODR2_ON  (1 << 3)    // OUTP2 driver is ON
#define  ODR2_OFF (0 << 3)    // OUTP2 driver is OFF

#define  INPUT1   0x00        // Analog input is INP/M1
#define  MIC_INT  0x02        // Analog input is MICIN self-biased at 1.35 V
#define  MIC_EXT  0x04        // Analog input is MICIN with external common mode
#define  INPUT2   0x06        // Analog input is INP/M2

/* Function prototypes */
int1 codec_initialize(void);
int1 codec_setup_gains(unsigned int8 ADC_gain, unsigned int8 DAC_gain, unsigned int8 ST_gain, unsigned int8 IN_gain);
int1 codec_setup_sample_rate_div(unsigned int8 M, unsigned int8 P, unsigned int8 N);
int1 codec_write_register(unsigned int8 address, unsigned int8 data);
int1 codec_read_register(unsigned int8 address, unsigned int8 * data);
int1 codec_soft_power_down(void);

/* C O D E C  I N I T I A L I Z E 
 * This function initializes all of the associated registers and peripherals associated
 * with the codec functionality for basic communication with a dsPIC via I2C.
 * It is recommended that this function be called first when the codec is first
 * powered up.  Individual register settings may be changed with subsequent
 * calls to the codec_write_register() function. */
int1 codec_initialize(void)
{
   int1 ack=0;
   
   ack |= codec_write_register(REGISTER_1, ( CONT_TX | IIR | ALB_OFF | DLB_OFF | DAC16 ) ); //CR1
   ack |= codec_write_register(REGISTER_2, ( NORMAL_MODE | DIF_ON | I2C_ADDR | I2C_INT ) ); //CR2
   ack |= codec_write_register(REGISTER_3, ( AD_ON | DA_ON | NO_RESET | DAC_128 | ASRF1 )); //CR3
   //ack |= codec_setup_sample_rate_div( 8, 1, 8);                                            //CR4
   ack |= codec_setup_sample_rate_div( 10, 1, 8 );
   //ack |= codec_setup_sample_rate_div( 12, 4, 8 );
   //ack |= codec_setup_sample_rate_div( 8, 2, 8 );
   ack |= codec_setup_gains( ADC_NO_GAIN, DAC_NO_GAIN+6, ST_MUTE_ATTEN, IN_24dB_GAIN );         //CR5
   ack |= codec_write_register(REGISTER_6, ( SINGLE | NO_MUTE2 | NO_MUTE3 | ODR3_ON | ODR2_ON | INPUT1 ) ); //CR6
   
   return ack;
}

/* Hardware-specific power options.
 * 
 * The following 2 functions turn the codec on/off using the hardware control
 * pin.  The PIN used in these functions must be changed to reflect your
 * specific hardware setup.
*/

#inline
void codec_hard_power_down(void)
{
   output_low(PIN_D1);
}

#inline
void codec_hard_power_up(void)
{
   output_high(PIN_D1);
}

/* Software power options
 * 
 * The following two functions turn the codec's AD and DA converter on or off
 * using software control via I2C.  When both the AD and DA converter are off,
 * the codec is placed into a low-power, standby mode.  When the ADC and DAC
 * are turned back on, the codec continues operation according to the last
 * programmed settings.
*/
int1 codec_soft_power_down(void)
{
   int1 ack=0;
   
   ack |= codec_write_register(REGISTER_3, (AD_OFF | DA_OFF | NO_RESET | DAC_128 | ASRF1));
   
   return ack;
}

int1 codec_soft_power_up(void)
{
   int1 ack = 0;
   
   ack |= codec_write_register(REGISTER_3, (AD_ON | DA_ON | NO_RESET | DAC_128 | ASRF1));
   
   return ack;
}

/* C O D E C  S E T U P  G A I N S
 * This function setups up the four user programmable amplifiers that are internal
 * to the codec.  Be sure to the use the defines from above to ensure the proper
 * values are passed to this function. 
 *
 * example call:
 * int1 check = codec_setup_gains(ADC_NO_GAIN - 6, DAC_NO_GAIN + 12, ST_MUTE_ATTEN, IN_6dB_GAIN); 
 * */
int1 codec_setup_gains(unsigned int8 ADC_gain, unsigned int8 DAC_gain, unsigned int8 ST_gain, unsigned int8 IN_gain)
{
   int1 ack=0;
   
   ack |= codec_write_register(REGISTER_5, ADC_gain);
   ack |= codec_write_register(REGISTER_5, (0x40 | DAC_gain));
   ack |= codec_write_register(REGISTER_5, (0x80 | ST_gain | IN_gain));
   
   return ack;
}

/* C O D E C  S E T U P  S A M P L E  R A T E  D I V 
 * This function setups up the codecs internal frequency dividers, the input
 * paramters have the following minimal restrictions:
 *
 * 1 <= M <= 128
 * 1 <= N <= 16
 * 1 <= P <= 8
 * 
 * Other restrictions may apply, please see the codec datasheet for restrictions
 * for your particular hardware setup.  In order to determine what values to use
 * for M, N, and P, use the following formula:
 *
 * M*N*P = INPUT_CLOCK / (16 * DESIRED_SAMPLING_FREQUENCY).
 *
 * i.e. if INPUT_CLOCK = 20.48 Mhz, and DESIRED_SAMPLING_FREQUENCY = 8 ksps:
 *
 * M*N*P = 20.48 MHz / (16 * 8 ksps) = 160.
 * 
 * Using the restrictions above, setting M = 10, N = 16, and P = 1 gives:
 *
 * M*N*P = 10*16*1 = 160.
 **/
int1 codec_setup_sample_rate_div(unsigned int8 M, unsigned int8 P, unsigned int8 N)
{
   int1 ack=0;
  
   if ( (M <= 128) && (N <= 16) && (P <= 8) && (M > 0) && (N >0) && (P > 0) )
   {
      ack |= codec_write_register(REGISTER_4, ((1 << 7) | M) );
      ack |= codec_write_register(REGISTER_4, ( (N << 3) | P ) );
      return ack;
   }
   else
      return 1;
}

/* C O D E C  W R I T E  R E G I S T E R  
 * Use the control interface to the TLV320 to write one of its registers.
 * Requires the register address (defined above) and a value to set the register too.
 * The common volume levels are provided, however the datasheet for the codec 
 * should be used to set these values. */
int1 codec_write_register(unsigned int8 address, unsigned int8 data)
{
   int1 ack=0;
   
   i2c_start();
   ack |= i2c_write(codec_i2c, codec_address);
   ack |= i2c_write(codec_i2c, address);
   ack |= i2c_write(codec_i2c, data);
   i2c_stop();
   
   return ack;
}

int1 codec_write_all_registers(int8 data1, int8 data2, int8 data3, int8 data4, int8 data5, int8 data6)
{
   int1 ack = 0;
   
   i2c_start();
   ack |= i2c_write(codec_i2c, codec_address);
   ack |= i2c_write(codec_i2c, REGISTER_1);
   ack |= i2c_write(codec_i2c, data1);
   ack |= i2c_write(codec_i2c, data2);
   ack |= i2c_write(codec_i2c, data3);
   ack |= i2c_write(codec_i2c, data4);
   ack |= i2c_write(codec_i2c, data5);
   ack |= i2c_write(codec_i2c, data6);
   i2c_stop();
   
   return ack;
}

/* C O D E C  R E A D   R E G I S T E R  
 * Use the control interface to the TLV320 to read one of its registers.
 * Requires the register address (defined above) and a value to set the register too.
 * The common volume levels are provided, however the datasheet for the codec 
 * should be used to set these values. */
int1 codec_read_register(unsigned int8 address, unsigned int8 * data)
{
   int1 ack=0;
   i2c_start();
   ack |= i2c_write(codec_i2c, codec_address);
   ack |= i2c_write(codec_i2c, address);
   i2c_stop();
   if (!ack)
   {
      i2c_start();
      ack |= i2c_write(codec_i2c, codec_address | 1);
      *data = i2c_read(codec_i2c, 0);
      i2c_stop();
   }
   
   return ack;
}

int1 codec_read_all_registers(unsigned int8 * data)
{
   int1 ack = 0;
   i2c_start();
   ack |= i2c_write(codec_i2c, codec_address);
   ack |= i2c_write(codec_i2c, REGISTER_1);
   i2c_stop();
   if (!ack)
   {
      i2c_start();
      ack |= i2c_write(codec_i2c, codec_address | 1);
      data[0] = i2c_read(codec_i2c, 1);
      data[1] = i2c_read(codec_i2c, 1);
      data[2] = i2c_read(codec_i2c, 1);
      data[3] = i2c_read(codec_i2c, 1);
      data[4] = i2c_read(codec_i2c, 1);
      data[5] = i2c_read(codec_i2c, 0);
      i2c_stop();
   }
   
   return ack;
}

#endif
