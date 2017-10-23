///////////////////////////////////////////////////////////////////////////
////   Library for a TI TLV320AIC23B Codec Chip                        ////
////                                                                   ////
////   void codec_initialize();                                        ////
////      *Default settings for the DSP Analog Board                   ////
////                                                                   ////
////   void codec_setup_line_input(left_input, right_input);           ////
////      *Volume settings for line input                              ////
////                                                                   ////
////   void codec_setup_hp_output(left_hp, right_hp);                  ////
////      *Volume settings for headphone output                        ////
////                                                                   ////
////   void codec_setup_analog_path(path);                             ////
////      *Controls the DAC, ADC, and Headphone                        ////
////                                                                   ////
////   void codec_setup_digital_path(path);                            ////
////      *Controls the DAC mute and highpass filter                   ////
////                                                                   ////      
////   void codec_shutdown(shutdown);                                  ////
////      *Controls power to all on chip modules                       ////
////                                                                   ////      
////   void codec_soft_power_down();                                   ////
////      *Power down all modules except for device, osc, and clock    ////
////                                                                   ////
////   void codec_setup_interface(interface);                          ////
////      *Controls data format,  except for device, osc, and clock    ////
////                                                                   ////
////   void codec_enable_interface();                                  ////
////      *Activates sample conversion                                 ////
////                                                                   ////
////   void codec_reset();                                             ////
////      *Resets the device                                           ////
////                                                                   ////
////   void codec_write_register(reg, value);                          ////
////      *Perform low level register access by passing:               ////
////          -Register from register map                              ////
////          -Values defined for register                             ////
////                                                                   ////
////   unsigned int1 codec_transmit_ready();                           ////
////   unsigned int1 codec_data_received();                            ////                                                                   ////
////   
////   The main program may define eeprom_select, eeprom_di, eeprom_do ////
////   and eeprom_clk to override the defaults below.                  ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 2008 Custom Computer Services                ////
//// This source code may only be used by licensed users of the CCS C  ////
//// compiler.  This source code may only be distributed to other      ////
//// licensed users of the CCS C compiler.  No other use, reproduction ////
//// or distribution is permitted without written permission.          ////
//// Derivative programs created using this software in object code    ////
//// form are not restricted in any way.                               ////
///////////////////////////////////////////////////////////////////////////

#ifndef TLV320
#define TLV320 1
#nolist

#ifndef CODEC_SPI_XFER
   #ifndef CODEC_CS
      #define CODEC_CS     PIN_C1
      #define CODEC_SDO    PIN_F3
      #define CODEC_SCK    PIN_F6
   #endif

   #use SPI(master, DO=CODEC_SDO, CLK=CODEC_SCK, bits=8, STREAM=STREAM_CODEC_SPI, mode=0)
   
   #define CODEC_SPI_XFER(x)   spi_xfer(STREAM_CODEC_SPI, x)
#endif

#ifndef CODEC_SPI_INIT
   #define CODEC_SPI_INIT()
#endif

/* TLV320AIC23B Register map */
/* First argument to codec_write_register */
#define LEFT_LINE_INPUT_CHANNEL  0x0000 //Left Line input channel volume control register
#define RIGHT_LINE_INPUT_CHANNEL 0x0001 //Right Line input channel volume control register
#define LEFT_CHANNEL_HP          0x0002 //Left channel headphone volume control register
#define RIGHT_CHANNEL_HP         0x0003 //Right channel headphone volume control register
#define ANALOG_AUDIO_PATH        0x0004 //Analog Audio path register
#define DIGITAL_AUDIO_PATH       0x0005 //Digital Audio path register
#define PWR_DOWN_CTRL            0x0006 //Power Down control register
#define DIGITAL_AUDIO_INTERFACE  0x0007 //Digital audio interfacing control register
#define SAMPLE_RATE_CTRL         0x0008 //Sample rate control register
#define INTERFACE_ACTIVATION     0x0009 //Digital interface activation register
#define RESET                    0x000F //Reset Register

/* Input arguments to CODEC_SETUP_LINE_INPUT() */
#define MUTE_CHANNEL             0x80
#define LINE_IN_NO_GAIN_VOLUME   0x17 //0dB gain (10111)
#define LINE_IN_MIN_GAIN_VOLUME  0x00 //-34.5 dB (00000)
#define LINE_IN_MAX_GAIN_VOLUME  0x1F //12 dB (11111)

/* Input arguments to CODEC_SETUP_HP_OUTPUT() */
#define SIMULTANEOUS_VOL_UPDATE  0x100
#define ZERO_CROSS_DETECT        0x80
#define HP_MAX_GAIN              0x7F  //+6 dB gain
#define HP_NO_GAIN               0x79  //0 dB gain - Default
#define HP_MUTE_GAIN             0x30  //-73 dB gain (mute)

/* Input Arguments to CODEC_SETUP_ANALOG_PATH() */
#define DAC_SELECT               0x10
#define ADC_MIC_INPUT            0x04  //default is line input
#define ADC_LINE_INPUT           0x00
#define MIC_MUTE                 0x02
#define MIC_BOOST                0x01

/* Input Arguments to CODEC_SETUP_DIGITAL_PATH() */
#define DAC_SOFT_MUTE            0x08
#define DISABLE_ADC_HIGH_PASS    0x01

/* Input Arguments to CODEC_SETUP_SAMPLE_RATE() */
#define USB_8KHZ                 0x0D
#define USB_32KHZ                0x18
#define USB_48KHZ                0x01

/* Input Arguments to CODEC_SHUTDOWN() */
#define DEVICE_PWR_OFF           0x80
#define CLOCK_PWR_OFF            0x40
#define OSC_PWR_OFF              0x20
#define OUTPUT_PWR_OFF           0x01
#define DAC_PWR_OFF              0x08
#define ADC_PWR_OFF              0x04
#define MIC_PWR_OFF              0x02
#define LINE_PWR_OFF             0x01
#define PWR_ON                   0x00

/* Input arguments to codec_setup_interface() */
#define MASTER_MODE              0x40 //Make codec master
#define SWAP_LEFT_RIGHT_DAC      0x20
#define LRP                      0x10 //PHASE for I2S, BCLK 1st or 2nd for DSP
#define SAMPLE_16BIT             0x00
#define SAMPLE_20BIT             0x08
#define SAMPLE_24BIT             0x04
#define SAMPLE_32BIT             0x0C
#define MULTICHANNEL             0x03
#define I2S                      0x02
#define LEFT_ALIGNED             0x01
#define RIGHT_ALIGNED            0x00

#define ACTIVATE                 0x01
#define DEACTIVATE               0x00
#define RESET_DEVICE             0x0000

#define CODEC_SELECT() output_low(CODEC_CS)
#define CODEC_DESELECT() output_high(CODEC_CS);

#word DCISTAT  = getenv("sfr:DCISTAT")
#bit TMPTY     = DCISTAT.0
#bit TUNF      = DCISTAT.1
#bit RFUL      = DCISTAT.2
#bit ROV       = DCISTAT.3

#word RXBUF0   = getenv("sfr:RXBUF0")
#word RXBUF1   = getenv("sfr:RXBUF1")
#word TXBUF0   = getenv("sfr:TXBUF0")
#word TXBUF1   = getenv("sfr:TXBUF1")

void codec_initialize();
void codec_setup_line_input(unsigned int16 left_input, unsigned int16 right_input);
void codec_setup_hp_output(unsigned int16 left_hp, unsigned int16 right_hp);
void codec_setup_analog_path(unsigned int16 path);
void codec_setup_digital_path(unsigned int16 path);
void codec_setup_sample_rate(unsigned int16 sr);
void codec_shutdown(unsigned int16 shutdown);
void codec_soft_power_down();
void codec_setup_interface(unsigned int16 interface);
void codec_enable_interface();
void codec_reset();

void codec_write_register(unsigned int8 a, unsigned int8 d);

#inline
unsigned int1 codec_transmit_ready();
#inline
unsigned int1 codec_data_received();

void codec_write_data16(signed int16 *leftChannel, signed int16 *rightChannel);
void codec_read_data16(signed int16 *leftChannel, signed int16 *rightChannel);
void codec_exchange_data16(signed int16 *leftChannel, signed int16 *rightChannel);

/* C O D E C  I N I T I A L I Z E 
 * This function initializes all of the associated registers and peripherals associated
 * with the codec functionality. DCI and SPI are setup here. 
 * The codec registers are not intialized here. The codec will be in its default
 * power up state after calling this function. The following code only prepares the 
 * interface to the codec. Use codec_write_register() and the codec_setup_xxx() functions
 * to setup the codec for your particular setup. The codec register may be changed at any time.
 * The default setup is I2S dsPIC master @ 16 bit sample size. If your hardware does not
 * match this configuration, you must change this setup with codec_setup_interface(). */
void codec_initialize()
{  
   /* Setup and enable SPI Control Lines */
   CODEC_DESELECT();  
 
   codec_reset();
   //codec_soft_power_down(); 
   codec_setup_analog_path(DAC_SELECT | ADC_MIC_INPUT); 
   codec_setup_digital_path(0x00);   
   codec_setup_interface(SAMPLE_16BIT | MULTICHANNEL | MASTER_MODE | LRP);
   codec_setup_sample_rate(USB_8KHZ); 
   codec_enable_interface();//enable the audio interface.   
   
   codec_shutdown(PWR_ON);
   
}

/* A S S I S T E D  C O D E C  R E G I S T E R  S E T U P  
 * The following functions; codec_setup_xxxx() are made available to help setup 
 * the codec registers with minimal difficulty. All of the following wrap 
 * codec_write_register() to make the register setup process more understandable
 * without immediate access to the device datasheet.
 
 * To minimally configure the codec, a call to codec_write_register with sample rate
 * setup information and a call to codec_enable_interface() must be made for
 * the audio interface to function. Sample size is 16 bits by default. Call
 * codec_setup_interface() to change this. See the definitions above for details on setup
 * options. */
  
void codec_setup_line_input(unsigned int16 left_input, unsigned int16 right_input)
{
   codec_write_register(LEFT_LINE_INPUT_CHANNEL, left_input);
   codec_write_register(RIGHT_LINE_INPUT_CHANNEL, right_input);
}

void codec_setup_hp_output(unsigned int16 left_hp, unsigned int16 right_hp)
{
   codec_write_register(LEFT_CHANNEL_HP, left_hp);
   codec_write_register(RIGHT_CHANNEL_HP, right_hp);
}

void codec_setup_analog_path(unsigned int16 path)
{
   codec_write_register(ANALOG_AUDIO_PATH , path);
}

void codec_setup_digital_path(unsigned int16 path)
{
   codec_write_register(DIGITAL_AUDIO_PATH, path);
}

void codec_setup_interface(unsigned int16 interface)
{
   codec_write_register(DIGITAL_AUDIO_INTERFACE, interface);
}

void codec_setup_sample_rate(unsigned int16 sr)
{
   codec_write_register(SAMPLE_RATE_CTRL, sr);
}

/* C O D E C  C O N T R O L
 * The following functions control miscallaneous features of the codec. 
 * The codec_shutdown function is used to power down portions of the chip 
 * when not in use.
 * codec_enable_interface() enables the digital audio interface, allowing you to
 * transfer sound data.
 * codec_reset() will reset the codec device to its default state. */
void codec_shutdown(unsigned int16 shutdown)
{
   codec_write_register(PWR_DOWN_CTRL, shutdown);
}

void codec_soft_power_down()
{
   codec_write_register(PWR_DOWN_CTRL, OUTPUT_PWR_OFF | DAC_PWR_OFF |
                                          ADC_PWR_OFF | LINE_PWR_OFF |
                                          MIC_PWR_OFF );
}

void codec_enable_interface()
{
   codec_write_register(INTERFACE_ACTIVATION, ACTIVATE);
}

void codec_reset()
{
   codec_write_register(RESET, RESET_DEVICE);
}

/* C O D E C  W R I T E  R E G I S T E R  
 * Use the control interface to the TLV320 to write one of its registers.
 * The codec does not support register reads.
 * Requires the register address (defined above) and a value to set the register too.
 * The common volume levels are provided, however the datasheet for the codec 
 * should be used to set these values. */
void codec_write_register(unsigned int8 a, unsigned int8 d)
{
   CODEC_SPI_INIT();
   
   output_low(CODEC_CS);
   
   CODEC_SPI_XFER((a << 1) | (d >> 7));
   CODEC_SPI_XFER(d);
   
   output_high(CODEC_CS);
}

/* W R I T E  C O D E C  D A T A  (16, 32 bit sample sizes)
 * Write sound data to the codec. This data would be most likely outgoing to the
 * codec DAC. Keep in mind that the DAC must be enabled, as well as any line outputs.
 
 * Each respective function is used with different sample sizes. Make sure that the
 * codec and this chip is set up for the correct sample size before calling these functions. */
void codec_write_data16(signed int16 *leftChannel, signed int16 *rightChannel)
{
   while(!codec_transmit_ready());//wait until the data is ready      
   TXBUF0 = *leftChannel;
   TXBUF1 = *rightChannel;
}

/* C O D E C  T R A N S M I T  R E A D Y 
 * Test whether the codec is ready to receive more data. Similar to a kbhit() function, 
 * however, since data is transmitted in a timed fashion (according to the codec sample rate)
 * this function may be used to reduce the processing load on the dsPIC */
 #inline
unsigned int1 codec_transmit_ready()
{
   return TMPTY;
}

/* R E A D  C O D E C  D A T A (16, 32 bit sample sizes)
 * Read codec data that has been buffered on the last I2S frame.
 * This data is incoming from the codec ADC. This function requires a location
 * to drop the incoming pulse code data for both the left and right 
 * Each respective function is used with different sample sizes. Make sure that the
 * codec and this chip is set up for the correct sample size before calling these functions. */
void codec_read_data16(signed int16 *leftChannel, signed int16 *rightChannel)
{
   if(codec_data_received())
   { 
      *leftChannel = RXBUF0;
      *rightChannel = RXBUF1;

      ROV = 0;
   }   
}

/* C O D E C  D A T A  R E C E I V E D 
 * Tests whether data is available from the codec; effectively a kbhit function for the codec. */
 #inline
unsigned int1 codec_data_received()
{
   return RFUL;
}

#list
#endif

