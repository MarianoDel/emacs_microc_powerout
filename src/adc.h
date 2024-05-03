//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### ADC.H #################################
//---------------------------------------------
#ifndef _ADC_H_
#define _ADC_H_

//----------- Defines For Configuration --------------//
//----------- Some ADC Configurations ----------------//
// #define ADC_WITH_INT
// #define USE_ADC_SAMPLE_BY_SAMPLE
// #define ADC_WITH_DMA

#ifdef WITH_TEMP_CONTROL
#define ADC_WITH_TEMP_SENSE
#endif

#ifdef ADC_WITH_TEMP_SENSE
#define SIZEOF_BOARD_TEMP		8
#endif

#define SENSE_POWER    adc_ch[0]
#define SENSE_MEAS    adc_ch[1]
#define V_SENSE_25V    adc_ch[2]
#define V_SENSE_8V    adc_ch[3]
#define V_SENSE_11V    adc_ch[4]
#define V_SENSE_28V    adc_ch[5]
#define ADC_CHANNEL_QUANTITY    6
#define ADC_LAST_CHANNEL_QUANTITY    (ADC_CHANNEL_QUANTITY - 1)
//----------- End of ADC Configurations --------------//


//-- Exported Hardware Macros ---------------------
#define ADC_START    (ADC1->CR2 |= ADC_CR2_SWSTART)

#define ADC_Channel_0                               ((uint8_t)0x00)
#define ADC_Channel_1                               ((uint8_t)0x01)
#define ADC_Channel_2                               ((uint8_t)0x02)
#define ADC_Channel_3                               ((uint8_t)0x03)
#define ADC_Channel_4                               ((uint8_t)0x04)
#define ADC_Channel_5                               ((uint8_t)0x05)
#define ADC_Channel_6                               ((uint8_t)0x06)
#define ADC_Channel_7                               ((uint8_t)0x07)
#define ADC_Channel_8                               ((uint8_t)0x08)
#define ADC_Channel_9                               ((uint8_t)0x09)
#define ADC_Channel_10                              ((uint8_t)0x0A)
#define ADC_Channel_11                              ((uint8_t)0x0B)
#define ADC_Channel_12                              ((uint8_t)0x0C)
#define ADC_Channel_13                              ((uint8_t)0x0D)
#define ADC_Channel_14                              ((uint8_t)0x0E)
#define ADC_Channel_15                              ((uint8_t)0x0F)
#define ADC_Channel_16                              ((uint8_t)0x10)
#define ADC_Channel_17                              ((uint8_t)0x11)

#define ADC_Channel_TempSensor                      ((uint8_t)ADC_Channel_16)
#define ADC_Channel_Vrefint                         ((uint8_t)ADC_Channel_17)

#define ADC_SampleTime_1_5Cycles                     ((uint8_t)0x00)
#define ADC_SampleTime_7_5Cycles                     ((uint8_t)0x01)
#define ADC_SampleTime_13_5Cycles                    ((uint8_t)0x02)
#define ADC_SampleTime_28_5Cycles                    ((uint8_t)0x03)
#define ADC_SampleTime_41_5Cycles                    ((uint8_t)0x04)
#define ADC_SampleTime_55_5Cycles                    ((uint8_t)0x05)
#define ADC_SampleTime_71_5Cycles                    ((uint8_t)0x06)
#define ADC_SampleTime_239_5Cycles                   ((uint8_t)0x07)

#define ADC_Channels_Qtty_1    ((uint8_t)0x00)
#define ADC_Channels_Qtty_2    (ADC_SQR1_L_0)
#define ADC_Channels_Qtty_3    (ADC_SQR1_L_1)
#define ADC_Channels_Qtty_4    (ADC_SQR1_L_1 | ADC_SQR1_L_0)
#define ADC_Channels_Qtty_5    (ADC_SQR1_L_2)                        
#define ADC_Channels_Qtty_6    (ADC_SQR1_L_2 | ADC_SQR1_L_0)                        
#define ADC_Channels_Qtty_7    (ADC_SQR1_L_2 | ADC_SQR1_L_1)                        
#define ADC_Channels_Qtty_8    (ADC_SQR1_L_2 | ADC_SQR1_L_1 | ADC_SQR1_L_0)
#define ADC_Channels_Qtty_9    (ADC_SQR1_L_3)                        
#define ADC_Channels_Qtty_10   (ADC_SQR1_L_3 | ADC_SQR1_L_0)                        
#define ADC_Channels_Qtty_11   (ADC_SQR1_L_3 | ADC_SQR1_L_1)
#define ADC_Channels_Qtty_12   (ADC_SQR1_L_3 | ADC_SQR1_L_1 | ADC_SQR1_L_0)
#define ADC_Channels_Qtty_13   (ADC_SQR1_L_3 | ADC_SQR1_L_2)
#define ADC_Channels_Qtty_14   (ADC_SQR1_L_3 | ADC_SQR1_L_2 | ADC_SQR1_L_0)
#define ADC_Channels_Qtty_15   (ADC_SQR1_L_3 | ADC_SQR1_L_2 | ADC_SQR1_L_1)                        
#define ADC_Channels_Qtty_16   (ADC_SQR1_L_3 | ADC_SQR1_L_2 | ADC_SQR1_L_1 | ADC_SQR1_L_0)                        


// Module Exported Functions ---------------------------------------------------
void AdcConfig (void);
void AdcSetChannelSampleTime (unsigned char, unsigned char);
void AdcSetChannelSamplePosition (unsigned char, unsigned char);
void AdcSetChannelsQuantity (unsigned int);
void AdcConvertChannel (unsigned char);
unsigned char AdcConvertSingleChannelFinishFlag (void);
unsigned short AdcConvertChannelResult (void);


#ifdef ADC_WITH_TEMP_SENSE
void UpdateTemp(void);
unsigned short GetTemp (void);
void FillTempBuffer (void);
short ConvertTemp (unsigned short);
#endif

#endif /* _ADC_H_ */
