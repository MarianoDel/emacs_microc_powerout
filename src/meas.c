//------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### SIGNALS.C ################################
//------------------------------------------------

// Includes --------------------------------------------------------------------
#include "meas.h"
#include "hard.h"
#include "adc.h"
#include "dac.h"
#include "dsp.h"

#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    SET_REFERENCE,
    START_REFERENCE_CONVERTION,
    WAIT_REFERENCE_CONVERTION,
    SET_MEAS,
    START_MEAS_CONVERTION,
    WAIT_MEAS_CONVERTION
    
} meas_state_e;

#define Sense_Meas    ADC_Channel_1
#define Sense_Power    ADC_Channel_0
// Externals -------------------------------------------------------------------
//del ADC
// extern volatile unsigned char seq_ready;
extern volatile unsigned short adc_ch[];

// externals from tim
// extern volatile unsigned char timer1_seq_ready;



// Globals ---------------------------------------------------------------------
volatile unsigned short meas_timeout = 0;
ma32_u16_data_obj_t ref_filter;
ma32_u16_data_obj_t meas_filter;
ma32_u16_data_obj_t display_filter;
ma8_u16_data_obj_t display_v2_filter;
unsigned short ref_filtered = 0;
unsigned short meas_filtered = 0;
unsigned char dac_gain_saved = 0;



// Module Private Functions ----------------------------------------------------
unsigned char Meas_Calc_Display_Value (unsigned short reference, unsigned short meas);
void Meas_Set_Dac (unsigned char dac_gain);
void Meas_Set_Dac_V2 (unsigned short dac_value);
unsigned int Meas_Online_Calc_Resistance (unsigned short current_itov,
                                          unsigned short meas_hg,
                                          unsigned short meas_lg);
unsigned int Meas_Sine_Calc_Conductivity (unsigned short dac_value,
                                          unsigned short adc_voltage,
                                          unsigned short adc_voltage_hg);

unsigned char Meas_Calc_Display_Value_V2 (unsigned char gain, unsigned short meas);

// Module Functions ------------------------------------------------------------
void Meas_Timeout (void)
{
    if (meas_timeout)
        meas_timeout--;
}


meas_state_e meas_state = SET_REFERENCE;
unsigned char meas_cnt = 0;
void Meas_Square_Reset (void)
{
    meas_state = SET_REFERENCE;
}


unsigned char Meas_Square_V2 (unsigned char * result)
{
    unsigned char new_convertion = 0;
    
    if (meas_timeout)
        return 0;

    meas_timeout = 5;    // take meas every 5ms * (64 + 2) = 330ms
    
    switch (meas_state)
    {
    case SET_REFERENCE:
        Meas_Set_Dac_V2 (310);    // reference for meas in 2uA 144mV on Rmeas
        meas_cnt = 0;
        meas_state = START_MEAS_CONVERTION;
        break;

    case START_MEAS_CONVERTION:
        AdcConvertChannel (Sense_Meas);
        meas_state = WAIT_MEAS_CONVERTION;
        break;

    case WAIT_MEAS_CONVERTION:
        if (AdcConvertSingleChannelFinishFlag ())
        {
            unsigned short new_meas = 0;
            new_meas = AdcConvertChannelResult ();
            meas_filtered = MA32_U16Circular (&meas_filter, new_meas);
            meas_state = START_MEAS_CONVERTION;

            // report in 32 meas
            if (meas_cnt < (32 - 1))
                meas_cnt++;
            else
            {
                unsigned short display_raw = 0;
                meas_cnt = 0;
                display_raw = Meas_Calc_Display_Value_V2 (dac_gain_saved, meas_filtered);
                *result = MA8_U16Circular(&display_v2_filter, display_raw);
                new_convertion = 1;

                // debug send meas
                // char buff [60];
                // sprintf(buff, "gain: %d meas: %d res: %d\r\n",
                //         dac_gain_saved,
                //         meas_filtered,
                //         *result);
                // Usart1Send(buff);
            }
        }
        break;
        
    default:
        meas_state = SET_REFERENCE;
        break;
    }
    
    return new_convertion;
}


unsigned char Meas_Square (unsigned char * result)
{
    unsigned char new_convertion = 0;
    
    if (meas_timeout)
        return 0;

    meas_timeout = 3;    // take meas every 5ms * (64 + 2) = 330ms
    
    switch (meas_state)
    {
    case SET_REFERENCE:
        Meas_Set_Dac (5);
        meas_cnt = 0;
        meas_state++;
        break;
            
    case START_REFERENCE_CONVERTION:
        AdcConvertChannel (Sense_Meas);
        meas_state++;
        break;

    case WAIT_REFERENCE_CONVERTION:
        if (AdcConvertSingleChannelFinishFlag ())
        {
            unsigned short new_ref = 0;
            new_ref = AdcConvertChannelResult ();
            ref_filtered = MA32_U16Circular (&ref_filter, new_ref);

            if (meas_cnt < (32 - 1))
            {
                meas_cnt++;
                meas_state = START_REFERENCE_CONVERTION;
            }
            else
                meas_state = SET_MEAS;
        }
        break;

    case SET_MEAS:
        Meas_Set_Dac (dac_gain_saved);
        meas_cnt = 0;
        meas_state++;
        break;

    case START_MEAS_CONVERTION:
        AdcConvertChannel (Sense_Meas);
        meas_state++;
        break;

    case WAIT_MEAS_CONVERTION:
        if (AdcConvertSingleChannelFinishFlag ())
        {
            unsigned short new_meas = 0;
            new_meas = AdcConvertChannelResult ();
            meas_filtered = MA32_U16Circular (&meas_filter, new_meas);

            if (meas_cnt < (32 - 1))
            {
                meas_cnt++;
                meas_state = START_MEAS_CONVERTION;
            }
            else
            {
                meas_state = SET_REFERENCE;
                // *result = Meas_Calc_Display_Value (ref_filtered, meas_filtered);
                // new_convertion = 1;
                unsigned char disp_to_filter;
                disp_to_filter = Meas_Calc_Display_Value (ref_filtered, meas_filtered);
                *result = MA32_U16Circular(&display_filter, disp_to_filter);
                new_convertion = 1;

                // debug send meas
                // char buff [60];
                // sprintf(buff, "ref: %d meas: %d\r\n",
                //         ref_filtered,
                //         meas_filtered);
                // Usart1Send(buff);
            }
        }
        break;
        
    default:
        meas_state = SET_REFERENCE;
        break;
    }
    
    return new_convertion;
}


void Meas_Square_Set_Dac_Gain (unsigned char dac_gain)
{
    if (dac_gain > 100)
        dac_gain = 100;

    dac_gain_saved = dac_gain;
}


void Meas_Set_Dac (unsigned char dac_gain)
{
    int calc = 0;

    if (dac_gain > 100)
        dac_gain = 100;

    calc = (dac_gain + 1) * 4095;
    calc = calc / 100;

    calc = calc + 250;
    
    if (calc > 4095)
        calc = 4095;

    DAC_Output2(calc);
}


void Meas_Set_Dac_V2 (unsigned short dac_value)
{
    if (dac_value > 4095)
        dac_value = 4095;

    DAC_Output2(dac_value);
}


void Meas_Square_Init (void)
{
    // init ADC for meas square
    // AdcConfig();

    // sample times
    AdcSetChannelSampleTime(Sense_Meas, ADC_SampleTime_239_5Cycles);

    // init meas filter
    MA32_U16Circular_Reset (&ref_filter);
    MA32_U16Circular_Reset (&meas_filter);
    MA32_U16Circular_Reset (&display_filter);
    MA8_U16Circular_Reset (&display_v2_filter);
    
}


#define OFFSET_MEAS    0
unsigned char Meas_Calc_Display_Value_V2 (unsigned char gain, unsigned short meas)
{
    const unsigned short gain_floor = 39;    // 0.032mV on opamp
    const unsigned short gain_step = 13;    // 0.0105mV on opamp

    unsigned short gain_ref = gain_floor + gain * gain_step;
    
    if (meas > gain_ref)
    {
        return 0;
    }
    else
    {
        gain_ref -= meas;
    }

    if (gain_ref > 100)
        gain_ref = 100;
    
    return (unsigned char) gain_ref;
}


unsigned char Meas_Calc_Display_Value (unsigned short reference, unsigned short meas)
{
    if (!reference)
        return 0;
    // if (meas_value < OFFSET_MEAS)
    //     meas_value = 0;

    // reference its the value for 100 in display
    int calc = meas * 100;
    calc = calc / reference;
    
    if (calc > 100)
        calc = 100;
    
    return (unsigned char) calc;
}


typedef enum {
    MEAS_ONLINE_WAIT_FOR_START,
    MEAS_ONLINE_START,
    MEAS_ONLINE_WAIT_ADC,
    MEAS_ONLINE_START_WAIT_1,
    MEAS_ONLINE_WAIT_ADC_WAIT_1,
    MEAS_ONLINE_START_WAIT_2,
    MEAS_ONLINE_WAIT_ADC_WAIT_2,
    MEAS_ONLINE_START_HG,
    MEAS_ONLINE_WAIT_ADC_HG,
    MEAS_ONLINE_CALC_CONDUCTIVITY,
    MEAS_ONLINE_SEND
    
} meas_online_e;
    
meas_online_e meas_online_state = MEAS_ONLINE_WAIT_FOR_START;
unsigned short meas_online_dac = 0;
unsigned short meas_online_voltage = 0;
unsigned short meas_online_voltage_hg = 0;
void Meas_Online_Reset (void)
{
    meas_online_state = MEAS_ONLINE_WAIT_FOR_START;
}


void Meas_Online (unsigned short dac_value)
{
    if (meas_online_state != MEAS_ONLINE_WAIT_FOR_START)
        return;

    meas_online_dac = dac_value;
    meas_online_state = MEAS_ONLINE_START;
}


unsigned char Meas_Online_Update (unsigned int * resistance)
{
    unsigned char new_convertion = 0;
    
    switch (meas_online_state)
    {
    case MEAS_ONLINE_WAIT_FOR_START:
        break;

    case MEAS_ONLINE_START:
        AdcConvertChannel (Sense_Power);
        meas_online_state++;
        break;
        
    case MEAS_ONLINE_WAIT_ADC:
        if (AdcConvertSingleChannelFinishFlag ())
        {
            meas_online_voltage = AdcConvertChannelResult ();
            meas_online_state++;
        }
        break;

    case MEAS_ONLINE_START_WAIT_1:
        AdcConvertChannel (Sense_Power);
        meas_online_state++;
        break;
        
    case MEAS_ONLINE_WAIT_ADC_WAIT_1:
        if (AdcConvertSingleChannelFinishFlag ())
        {
            meas_online_voltage = AdcConvertChannelResult ();
            meas_online_state++;
        }
        break;

    case MEAS_ONLINE_START_WAIT_2:
        AdcConvertChannel (Sense_Power);
        meas_online_state++;
        break;
        
    case MEAS_ONLINE_WAIT_ADC_WAIT_2:
        if (AdcConvertSingleChannelFinishFlag ())
        {
            meas_online_voltage = AdcConvertChannelResult ();
            meas_online_state++;
        }
        break;
        
    case MEAS_ONLINE_START_HG:
        AdcConvertChannel (Sense_Meas);
        meas_online_state++;
        break;
        
    case MEAS_ONLINE_WAIT_ADC_HG:
        if (AdcConvertSingleChannelFinishFlag ())
        {
            meas_online_voltage_hg = AdcConvertChannelResult ();
            meas_online_state++;
        }
        break;
        
    case MEAS_ONLINE_CALC_CONDUCTIVITY:
        *resistance = Meas_Online_Calc_Resistance (
                meas_online_dac,
                meas_online_voltage_hg,
                meas_online_voltage);

        // char buff [60];
        // sprintf(buff, "h: %d l: %d d: %d\r\n",
        //         meas_online_voltage_hg,
        //         meas_online_voltage,
        //         meas_online_dac);
        // Usart1Send(buff);
        
        new_convertion = 1;
        // meas_online_state++;
        meas_online_state = MEAS_ONLINE_WAIT_FOR_START;
        break;

    // case MEAS_ONLINE_SEND:
    //     char buff [40];
    //     sprintf(buff, "c %d\r\n", meas_online_conductivity);
    //     Usart1Send(buff);
    //     meas_online_state = MEAS_ONLINE_WAIT_FOR_START;
    //     break;
        
    default:
        meas_online_state = MEAS_ONLINE_WAIT_FOR_START;
        break;
    }

    return new_convertion;    
}


#define CURRENT_POWER_RESISTENCE    3300
#define MULTIPLIER_LOW    11
#define MULTIPLIER_HIGH    2
unsigned int Meas_Online_Calc_Resistance (unsigned short current_itov,
                                          unsigned short meas_hg,
                                          unsigned short meas_lg)
{
    if (!current_itov)
        return 0;

    // voltage over 3k3 is output current
    // meas_lg is voltage over Rx multiplied by 0.0909 (1/11)
    // or meas_hg is voltage over Rx multiplied by 0.5 (1/2)
    unsigned int calc = 0;

    
    if (meas_hg < 4090)    // use high gain circuit for calcs
    {
        // calc = meas_hg * MULTIPLIER_HIGH * CURRENT_POWER_RESISTENCE;
        if (meas_hg > 1000)
            calc = meas_hg * MULTIPLIER_HIGH * 5000;    // 2700 * 2
        else
            calc = meas_hg * MULTIPLIER_HIGH * 3300;    // 2700 * 1.5
        
        calc = calc / current_itov;
    }
    else    // use low gain circuits
    {
        // calc = meas_lg * MULTIPLIER_LOW * CURRENT_POWER_RESISTENCE;
        if (meas_lg > 1000)
            calc = meas_lg * MULTIPLIER_LOW * 5000;    // 2700 * 2
        else
            calc = meas_lg * MULTIPLIER_LOW * 3300;    // 2700 * 1.5

        calc = calc / current_itov;        
    }

    return  calc;
}

typedef enum {
    MEAS_SINE_WAIT_FOR_START,
    MEAS_SINE_START,
    MEAS_SINE_WAIT_ADC,
    MEAS_SINE_START_HG,
    MEAS_SINE_WAIT_ADC_HG,
    MEAS_SINE_CALC_CONDUCTIVITY,
    MEAS_SINE_SEND
    
} meas_sine_e;

meas_sine_e meas_sine_state = MEAS_SINE_WAIT_FOR_START;
unsigned short meas_sine_dac = 0;
unsigned short meas_sine_voltage = 0;
unsigned short meas_sine_voltage_hg = 0;
unsigned short meas_sine_conductivity = 0;
void Meas_Sine_Reset (void)
{
    meas_sine_state = MEAS_SINE_WAIT_FOR_START;
}


void Meas_Sine (unsigned short dac_value)
{
    if (meas_sine_state != MEAS_SINE_WAIT_FOR_START)
        return;

    meas_sine_dac = dac_value;
    meas_sine_state = MEAS_SINE_START;        
}


unsigned char Meas_Sine_Update (unsigned short * conductivity)
{
    unsigned char new_convertion = 0;
    
    switch (meas_sine_state)
    {
    case MEAS_SINE_WAIT_FOR_START:
        break;

    case MEAS_SINE_START:
        AdcConvertChannel (Sense_Power);
        meas_sine_state++;
        break;
        
    case MEAS_SINE_WAIT_ADC:
        if (AdcConvertSingleChannelFinishFlag ())
        {
            meas_sine_voltage = AdcConvertChannelResult ();
            meas_sine_state++;
        }
        break;

    case MEAS_SINE_START_HG:
        AdcConvertChannel (Sense_Meas);
        meas_sine_state++;
        break;
        
    case MEAS_SINE_WAIT_ADC_HG:
        if (AdcConvertSingleChannelFinishFlag ())
        {
            meas_sine_voltage_hg = AdcConvertChannelResult ();
            meas_sine_state++;
        }
        break;
        
    case MEAS_SINE_CALC_CONDUCTIVITY:
        meas_sine_conductivity = Meas_Sine_Calc_Conductivity (
                meas_sine_dac,
                meas_sine_voltage,
                meas_sine_voltage_hg);

        // char buff [60];
        // sprintf(buff, "h: %d l: %d d: %d\r\n",
        //         meas_sine_voltage_hg,
        //         meas_sine_voltage,
        //         meas_sine_dac);
        // Usart1Send(buff);
        
        *conductivity = meas_sine_conductivity;
        new_convertion = 1;
        // meas_sine_state++;
        meas_sine_state = MEAS_SINE_WAIT_FOR_START;
        break;

    // case MEAS_SINE_SEND:
    //     char buff [40];
    //     sprintf(buff, "c %d\r\n", meas_sine_conductivity);
    //     Usart1Send(buff);
    //     meas_sine_state = MEAS_SINE_WAIT_FOR_START;
    //     break;
        
    default:
        meas_sine_state = MEAS_SINE_WAIT_FOR_START;
        break;
    }

    return new_convertion;    
}


unsigned int Meas_Sine_Calc_Conductivity (unsigned short dac_value,
                                          unsigned short adc_voltage,
                                          unsigned short adc_voltage_hg)
{
    if (!dac_value)
        return 0;

    // dac_value over 3k3 is output current
    // adc_voltage Rx multiplied by 0.0909 (1/11)
    // calc is Rx
    unsigned int calc = 0;

    if (adc_voltage_hg < 4000)    // use high gain circuit for calcs
    {
        calc = adc_voltage_hg * MULTIPLIER_HIGH * CURRENT_POWER_RESISTENCE;
        calc = calc / dac_value;
    }
    else    // use low gain circuits
    {
        calc = adc_voltage * MULTIPLIER_LOW * CURRENT_POWER_RESISTENCE;
        calc = calc / dac_value;
    }

    if (calc > 65000)
        calc = 65000;
    
    return  calc;
}

//--- end of file ---//
