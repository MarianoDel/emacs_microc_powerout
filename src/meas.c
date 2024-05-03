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
unsigned short ref_filtered = 0;
unsigned short meas_filtered = 0;
unsigned char dac_gain_saved = 0;



// Module Private Functions ----------------------------------------------------
unsigned char Meas_Calc_Display_Value (unsigned short reference, unsigned short meas);
void Meas_Set_Dac (unsigned char dac_gain);


// Module Functions ------------------------------------------------------------
void Meas_Timeout (void)
{
    if (meas_timeout)
        meas_timeout--;
}


meas_state_e meas_state = SET_REFERENCE;
unsigned char meas_cnt = 0;
unsigned char Meas_Square (unsigned char * result)
{
    unsigned char new_convertion = 0;
    
    if (meas_timeout)
        return 0;

    meas_timeout = 5;    // take meas every 5ms * (64 + 2) = 330ms
    
    switch (meas_state)
    {
    case SET_REFERENCE:
        Meas_Set_Dac (30);
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
                *result = Meas_Calc_Display_Value (ref_filtered, meas_filtered);
                new_convertion = 1;
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

    if (calc > 4095)
        calc = 4095;

    DAC_Output2(calc);
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
    
}


#define OFFSET_MEAS    0
unsigned char Meas_Calc_Display_Value (unsigned short reference, unsigned short meas)
{
    if (!reference)
        return 0;
    // if (meas_value < OFFSET_MEAS)
    //     meas_value = 0;

    // reference its the value for 100pts
    int calc = meas * 100;
    calc = calc / reference;
    
    if (calc > 100)
        calc = 100;
    
    return (unsigned char) calc;
}
//--- end of file ---//
