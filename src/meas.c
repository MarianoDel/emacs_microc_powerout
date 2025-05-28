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
#include "dma.h"
#include "stm32f10x.h"
#include "adc.h"
#include "dac.h"
#include "dsp.h"

#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
#define Sense_Meas    SENSE_MEAS
#define Sense_Power    SENSE_POWER


// Externals -------------------------------------------------------------------
//del ADC
extern volatile unsigned short adc_ch[];


// Globals ---------------------------------------------------------------------
unsigned char dac_gain_saved = 0;

unsigned short meas_filtered = 0;
unsigned short power_filtered = 0;
ma16_u16_data_obj_t sense_meas_filter;
ma16_u16_data_obj_t sense_power_filter;


// Module Private Functions ----------------------------------------------------
void Meas_Set_Dac_V2 (unsigned short dac_value);
unsigned char Meas_Calc_Display_Value_V2 (unsigned char gain, unsigned short meas);


// Module Functions ------------------------------------------------------------
void Meas_Module_Init (void)
{
    MA16_U16Circular_Reset (&sense_meas_filter);
    MA16_U16Circular_Reset (&sense_power_filter);
}


void Meas_Module_Update (void)
{
    if (sequence_ready)
    {
	sequence_ready_reset;
	
	meas_filtered = MA16_U16Circular(&sense_meas_filter, Sense_Meas);
	power_filtered = MA16_U16Circular(&sense_power_filter, Sense_Power);
	if (Led_Is_On())
	    Led_Off();
	else
	    Led_On();
    }
}


void Meas_Square_V3_Set_Ref (void)
{
    // Meas_Set_Dac_V2 (310);    // reference for meas in 2uA 144mV on Rmeas
    // Meas_Set_Dac_V2 (620);    // reference for meas in 4uA 144mV on Rmeas
    Meas_Set_Dac_V2 (2048);    // reference for meas in 4uA 144mV on Rmeas    
}


void Meas_Square_V3 (unsigned char * result, unsigned short * meas_raw, unsigned short * power_raw)
{
    *result = Meas_Calc_Display_Value_V2 (dac_gain_saved, meas_filtered);
    *meas_raw = meas_filtered;
    *power_raw = power_filtered;
    // *meas_raw = Sense_Meas;
    // *power_raw = Sense_Power;
}


void Meas_Square_Set_Dac_Gain (unsigned char dac_gain)
{
    if (dac_gain > 100)
        dac_gain = 100;

    dac_gain_saved = dac_gain;
}


void Meas_Set_Dac_V2 (unsigned short dac_value)
{
    if (dac_value > 4095)
        dac_value = 4095;

    DAC_Output2(dac_value);
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


#define CURRENT_POWER_RESISTENCE    2700
// #define MULTIPLIER_LOW    11
// #define MULTIPLIER_LOW    7.66    // 22k & 3k3
#define MULTIPLIER_HIGH    2
unsigned int online_resistance = 0;
unsigned short online_current_set = 0;
unsigned short online_meas = 0;
unsigned char online_resistance_flag = 0;
void Meas_Online_Calc_Resistance_V2 (unsigned short current_itov)
{
    if (!current_itov)
        return;

    unsigned short meas_hg = meas_filtered;
    unsigned short meas_lg = power_filtered;
    // voltage over 3k3 is output current
    // meas_lg is voltage over Rx multiplied by 0.0909 (1/11)
    // or meas_hg is voltage over Rx multiplied by 0.5 (1/2)
    unsigned int calc = 0;
    
    if (meas_hg < 3500)    // use high gain circuit for calcs
    {
        // calc = meas_hg * MULTIPLIER_HIGH * CURRENT_POWER_RESISTENCE;
	calc = meas_hg * MULTIPLIER_HIGH * 2700;
        calc = calc / current_itov;
    }
    else    // use low gain circuits
    {
	// calc = meas_lg * MULTIPLIER_LOW * 2700;
	calc = meas_lg * 20682;	
        calc = calc / current_itov;        
    }

    online_resistance = calc;
    online_current_set = current_itov;
    online_meas = meas_filtered;
    online_resistance_flag = 1;
}


unsigned char Meas_Online_Flag_Get (void)
{
    return online_resistance_flag;
}


void Meas_Online_Flag_Reset (void)
{
    online_resistance_flag = 0;
}


// unsigned int Meas_Online_Get_Value (void)
// {
//     return online_resistance;
// }


unsigned int Meas_Online_Get_Value (unsigned short * current, unsigned short * meas)
{
    *current = online_current_set;
    *meas = online_meas;
    return online_resistance;
}







//--- end of file ---//
