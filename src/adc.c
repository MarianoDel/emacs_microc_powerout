//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### ADC.C #################################
//---------------------------------------------

// Includes --------------------------------------------------------------------
#include "adc.h"
#include "stm32f10x.h"
#include "hard.h"


// Private Types Constants and Macros ------------------------------------------
#define RCC_ADC1_CLK    (RCC->APB2ENR & 0x00000200)
#define RCC_ADC1_CLKEN    (RCC->APB2ENR |= 0x00000200)
#define RCC_ADC1_CLKDIS    (RCC->APB2ENR &= ~0x00000200)

#define RCC_ADC2_CLK    (RCC->APB2ENR & 0x00000400)
#define RCC_ADC2_CLKEN    (RCC->APB2ENR |= 0x00000400)
#define RCC_ADC2_CLKDIS    (RCC->APB2ENR &= ~0x00000400)

#define RCC_ADC3_CLK    (RCC->APB2ENR & 0x00008000)
#define RCC_ADC3_CLKEN    (RCC->APB2ENR |= 0x00008000)
#define RCC_ADC3_CLKDIS    (RCC->APB2ENR &= ~0x00008000)

#define RCC_ADC_PRESCALER_DIV_2    (RCC->CFGR &= ~RCC_CFGR_ADCPRE)
#define RCC_ADC_PRESCALER_DIV_4    (RCC->CFGR |= RCC_CFGR_ADCPRE_0)
#define RCC_ADC_PRESCALER_DIV_6    (RCC->CFGR |= RCC_CFGR_ADCPRE_1)
#define RCC_ADC_PRESCALER_DIV_8    (RCC->CFGR |= RCC_CFGR_ADCPRE_1 | RCC_CFGR_ADCPRE_0)


// Externals -------------------------------------------------------------------
extern volatile unsigned short adc_ch [];


#ifdef ADC_WITH_INT
extern volatile unsigned char seq_ready;
#endif

#ifdef ADC_WITH_TEMP_SENSE
extern volatile unsigned short tt_take_temp_sample;
#endif


// Globals ---------------------------------------------------------------------
#ifdef ADC_WITH_INT
volatile unsigned short * p_channel;
#endif

#ifdef ADC_WITH_TEMP_SENSE
// ------- del sensor de Temperatura -------
unsigned short board_temp [SIZEOF_BOARD_TEMP];
unsigned short last_temp = 0;
unsigned char board_temp_index = 0;
unsigned char new_temp_sample = 0;
#endif


// Module Functions ------------------------------------------------------------
//Single conversion mode (CONT=0)
//In Single conversion mode, the ADC performs a single conversion, and stops

//Continuous conversion mode (CONT=1)
//In continuous conversion mode, when a software or hardware trigger event occurs,
//the ADC performs a continuos conversion of a single channel

//Discontinuous mode (DISCEN)
//In this mode (DISCEN=1), converts a short sequence of channels n<=8, part of a
//regular sequencea hardware or software trigger event is required to start
//each conversion defined in the sequence. Only with (CONT=0)

// Scan mode
// converts all channels in the group and stops (CONT=0), if CONT=1 not stops and
// restarts the conversion sequence

void AdcConfig (void)
{
    // RCC_ADC_PRESCALER_DIV_8;    //72MHz / 8 = 9MHz
    // RCC_ADC_PRESCALER_DIV_6;    //72MHz / 6 = 12MHz
    RCC_ADC_PRESCALER_DIV_8;    //64MHz / 8 = 8MHz
    
    if (!RCC_ADC1_CLK)
        RCC_ADC1_CLKEN;

    // preseteo los registros a default, la mayoria necesita tener ADC apagado
    ADC1->CR1 = 0x00000000;
    ADC1->CR2 = 0x00000000;
    ADC1->SMPR1 = 0x00000000;
    ADC1->SMPR2 = 0x00000000;
    ADC1->SQR1 = 0x00000000;
    ADC1->SQR2 = 0x00000000;    
    
    //set trigger (if it's soft look at the end) & Continuos or Discontinuous
    // ADC1->CR1 |= ADC_CR1_SCAN;
    // ADC1->CR2 |= ADC_CR2_CONT | ADC_CR2_EXTTRIG;    // with external trigger (can be soft trigger)

    // converts all channels in sequence and stops
    ADC1->CR1 |= ADC_CR1_SCAN;
    ADC1->CR2 |= ADC_CR2_EXTTRIG;    // with external trigger (can be soft trigger)    
    
    //set sampling time for each channel
    AdcSetChannelSampleTime(ADC_Channel_0, ADC_SampleTime_41_5Cycles);
    AdcSetChannelSampleTime(ADC_Channel_1, ADC_SampleTime_41_5Cycles);
    AdcSetChannelSampleTime(ADC_Channel_2, ADC_SampleTime_41_5Cycles);
    AdcSetChannelSampleTime(ADC_Channel_3, ADC_SampleTime_41_5Cycles);    
    AdcSetChannelSampleTime(ADC_Channel_6, ADC_SampleTime_41_5Cycles);
    AdcSetChannelSampleTime(ADC_Channel_7, ADC_SampleTime_41_5Cycles);

    //set regular channel selection, start with 1
    AdcSetChannelSamplePosition(ADC_Channel_0, 1);
    AdcSetChannelSamplePosition(ADC_Channel_1, 2);
    AdcSetChannelSamplePosition(ADC_Channel_2, 3);
    AdcSetChannelSamplePosition(ADC_Channel_3, 4);
    AdcSetChannelSamplePosition(ADC_Channel_6, 5);    
    AdcSetChannelSamplePosition(ADC_Channel_7, 6);

    //set the quantity of channels to convert
    AdcSetChannelsQuantity(ADC_Channels_Qtty_6);
    
#ifdef ADC_WITH_INT        
    //set interrupts
    ADC1->IER |= ADC_IT_EOC;

    //set pointer
    p_channel = &adc_ch[0];

    NVIC_EnableIRQ(ADC1_IRQn);
    NVIC_SetPriority(ADC1_IRQn, 3);
#endif

#ifdef ADC_WITH_TEMP_SENSE
    ADC->CCR |= ADC_CCR_TSEN;
#endif

    //Enable and ADC calibration
    ADC1->CR2 |= ADC_CR2_ADON;    
    //	ADC calibration (optional, but recommended at power on)
    ADC1->CR2 |= ADC_CR2_RSTCAL;    // Reset previous calibration
    while (ADC1->CR2 & ADC_CR2_RSTCAL);
    ADC1->CR2 |= ADC_CR2_CAL;    // Start new calibration (ADC must be off at that time)
    while (ADC1->CR2 & ADC_CR2_CAL);

    //trigger by soft
    ADC1->CR2 |= ADC_CR2_EXTSEL;    //remember start ADC on main!!!

#ifdef ADC_WITH_DMA
    ADC1->CR2 |= ADC_CR2_DMA;
#endif
    
}

#ifdef ADC_WITH_INT
void ADC1_COMP_IRQHandler (void)
{
    if (ADC1->ISR & ADC_IT_EOC)
    {
        if (ADC1->ISR & ADC_IT_EOSEQ)	//seguro que es channel4 en posicion 3 en ver_1_1, 3 y 2 en ver_1_0
        {
            p_channel = &adc_ch[ADC_LAST_CHANNEL_QUANTITY];
            *p_channel = ADC1->DR;
            p_channel = &adc_ch[0];
            seq_ready = 1;
        }
        else
        {
            *p_channel = ADC1->DR;		//
            if (p_channel < &adc_ch[ADC_LAST_CHANNEL_QUANTITY])
                p_channel++;
        }
        //clear pending
        ADC1->ISR |= ADC_IT_EOC | ADC_IT_EOSEQ;
    }
}
#endif



#ifdef ADC_WITH_TEMP_SENSE
void UpdateTemp(void)
{
    //hago update cada 1 seg
    if (!tt_take_temp_sample)
    {
        tt_take_temp_sample = 1000;

        board_temp [board_temp_index] = ReadADC1_SameSampleTime(ADC_CH16);
        //board_temp [0] = ReadADC1_SameSampleTime(ADC_CH16);

        if (board_temp_index < (SIZEOF_BOARD_TEMP - 1))
            board_temp_index++;
        else
            board_temp_index = 0;

        new_temp_sample = 1;
    }
}

//devuelve el valor promedio de la temperatura
//si existen nuevas muestras hace la cuenta, sino contesta el ultimo valor calculado
unsigned short GetTemp (void)
{
    unsigned char i;
    unsigned int t = 0;

    if (new_temp_sample)
    {
        for (i = 0; i < SIZEOF_BOARD_TEMP; i++)
            t += board_temp[i];

        last_temp = t >> 3;
        new_temp_sample = 0;
    }

    return ConvertTemp(last_temp);
}

void FillTempBuffer (void)
{
    unsigned char i;
    unsigned short dummy;

    dummy = ReadADC1_SameSampleTime(ADC_CH16);

    for (i = 0; i < SIZEOF_BOARD_TEMP; i++)
        board_temp[i] = dummy;

}

short ConvertTemp (unsigned short adc_temp)
{
    int32_t temperature; /* will contain the temperature in degree Celsius */
    //temperature = (((int32_t) ADC1->DR * VDD_APPLI / VDD_CALIB) - (int32_t) *TEMP30_CAL_ADDR );
    temperature = (int32_t) *TEMP30_CAL_ADDR - adc_temp;
    temperature *= 1000;
    temperature = temperature / 5336;	//4.3mV / °C
    temperature = temperature + 30;

    return (short) temperature;
}
#endif //ADC_WITH_TEMP_SENSE

void AdcSetChannelSampleTime (unsigned char ADC_Channel, unsigned char ADC_SampleTime)
{
    uint32_t tmpreg1, tmpreg2;
    
    /* if ADC_Channel_10 ... ADC_Channel_17 is selected */
    if (ADC_Channel > ADC_Channel_9)
    {
        /* Get the old register value */
        tmpreg1 = ADC1->SMPR1;
        /* Calculate the mask to clear */
        tmpreg2 = ADC_SMPR1_SMP10 << (3 * (ADC_Channel - 10));
        /* Clear the old channel sample time */
        tmpreg1 &= ~tmpreg2;
        /* Calculate the mask to set */
        tmpreg2 = (uint32_t)ADC_SampleTime << (3 * (ADC_Channel - 10));
        /* Set the new channel sample time */
        tmpreg1 |= tmpreg2;
        /* Store the new register value */
        ADC1->SMPR1 = tmpreg1;
    }
    else /* ADC_Channel include in ADC_Channel_[0..9] */
    {
        /* Get the old register value */
        tmpreg1 = ADC1->SMPR2;
        /* Calculate the mask to clear */
        tmpreg2 = ADC_SMPR2_SMP0 << (3 * ADC_Channel);
        /* Clear the old channel sample time */
        tmpreg1 &= ~tmpreg2;
        /* Calculate the mask to set */
        tmpreg2 = (uint32_t)ADC_SampleTime << (3 * ADC_Channel);
        /* Set the new channel sample time */
        tmpreg1 |= tmpreg2;
        /* Store the new register value */
        ADC1->SMPR2 = tmpreg1;
    }
}


void AdcSetChannelSamplePosition (unsigned char ADC_Channel, unsigned char Rank)
{
    uint32_t tmpreg1, tmpreg2;

    /* For Rank 1 to 6 */
    if (Rank < 7)
    {
        /* Get the old register value */
        tmpreg1 = ADC1->SQR3;
        /* Calculate the mask to clear */
        tmpreg2 = ADC_SQR3_SQ1 << (5 * (Rank - 1));
        /* Clear the old SQx bits for the selected rank */
        tmpreg1 &= ~tmpreg2;
        /* Calculate the mask to set */
        tmpreg2 = (uint32_t)ADC_Channel << (5 * (Rank - 1));
        /* Set the SQx bits for the selected rank */
        tmpreg1 |= tmpreg2;
        /* Store the new register value */
        ADC1->SQR3 = tmpreg1;
    }
    /* For Rank 7 to 12 */
    else if (Rank < 13)
    {
        /* Get the old register value */
        tmpreg1 = ADC1->SQR2;
        /* Calculate the mask to clear */
        tmpreg2 = ADC_SQR2_SQ7 << (5 * (Rank - 7));
        /* Clear the old SQx bits for the selected rank */
        tmpreg1 &= ~tmpreg2;
        /* Calculate the mask to set */
        tmpreg2 = (uint32_t)ADC_Channel << (5 * (Rank - 7));
        /* Set the SQx bits for the selected rank */
        tmpreg1 |= tmpreg2;
        /* Store the new register value */
        ADC1->SQR2 = tmpreg1;
    }
    /* For Rank 13 to 16 */
    else
    {
        /* Get the old register value */
        tmpreg1 = ADC1->SQR1;
        /* Calculate the mask to clear */
        tmpreg2 = ADC_SQR1_SQ13 << (5 * (Rank - 13));
        /* Clear the old SQx bits for the selected rank */
        tmpreg1 &= ~tmpreg2;
        /* Calculate the mask to set */
        tmpreg2 = (uint32_t)ADC_Channel << (5 * (Rank - 13));
        /* Set the SQx bits for the selected rank */
        tmpreg1 |= tmpreg2;
        /* Store the new register value */
        ADC1->SQR1 = tmpreg1;
    }
}


void AdcSetChannelsQuantity (unsigned int qtty)
{
    ADC1->SQR1 &= ~ADC_SQR1_L;
    ADC1->SQR1 |= qtty;
}


void AdcConvertChannel (unsigned char ADC_Channel)
{
    ADC1->SQR1 &= ~ADC_SQR1_L;    //convert 1 channel
    
    ADC1->SQR3 &= ~ADC_SQR3_SQ1;
    ADC1->SQR3 |= ADC_Channel;
    ADC1->CR2 |= ADC_CR2_SWSTART | ADC_CR2_EXTTRIG;    
}


unsigned char AdcConvertSingleChannelFinishFlag (void)
{
    return (ADC1->SR & ADC_SR_EOC);
}


unsigned short AdcConvertChannelResult (void)
{
    return ADC1->DR;
}


//--- end of file ---//


