//------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### SIGNALS.C ################################
//------------------------------------------------

// Includes --------------------------------------------------------------------
#include "signals.h"
#include "signals_defs.h"
#include "treatment.h"
#include "meas.h"

#include "hard.h"
#include "tim.h"
#include "timer_signals.h"

#include "adc.h"
#include "dac.h"


#include "answers_defs.h"


#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    SQUARE_INIT,
    SQUARE_FIRST_EDGE,
    SQUARE_POSITIVE_PLATEAU,
    SQUARE_SECOND_EDGE,
    SQUARE_NEGATIVE_PLATEAU
        
} signals_square_states_e;


typedef enum {
    SINE_INIT,
    SINE_HIGH_HALF,
    SINE_LOW_HALF
        
} signals_sine_states_e;


typedef enum {
    SQUARE_NO_CUTTING,
    SQUARE_CUTTING
        
} signals_square_cut_e;


// -- Module Configs ----------
// -- Led1 config --
// #define LED1_ON_SIGNAL_ENDED    // edges each signal ended


// -- End of Module Configs ----


// Externals -------------------------------------------------------------------
//del ADC
// extern volatile unsigned char seq_ready;
extern volatile unsigned short adc_ch[];

// externals from tim
// extern volatile unsigned char timer1_seq_ready;



// Globals ---------------------------------------------------------------------
#define TABLE_SIZE    256
const unsigned short sinusoidal_table [] = {12,25,37,50,62,75,87,100,112,125,
                                            137,150,162,174,187,199,211,224,236,248,
                                            260,272,284,296,308,320,332,344,356,368,
                                            379,391,403,414,426,437,448,459,471,482,
                                            493,504,515,525,536,547,557,568,578,589,
                                            599,609,619,629,639,648,658,668,677,687,
                                            696,705,714,723,732,740,749,757,766,774,
                                            782,790,798,806,814,821,829,836,843,850,
                                            857,864,870,877,883,890,896,902,908,913,
                                            919,924,930,935,940,945,949,954,958,963,
                                            967,971,975,978,982,985,989,992,995,998,
                                            1000,1003,1005,1007,1010,1011,1013,1015,1016,1018,
                                            1019,1020,1021,1021,1022,1022,1022,1023,1022,1022,
                                            1022,1021,1021,1020,1019,1018,1016,1015,1013,1011,
                                            1010,1007,1005,1003,1000,998,995,992,989,985,
                                            982,978,975,971,967,963,958,954,949,945,
                                            940,935,930,924,919,913,908,902,896,890,
                                            883,877,870,864,857,850,843,836,829,821,
                                            814,806,798,790,782,774,766,757,749,740,
                                            732,723,714,705,696,687,677,668,658,648,
                                            639,629,619,609,599,589,578,568,557,547,
                                            536,525,515,504,493,482,471,459,448,437,
                                            426,414,403,391,379,368,356,344,332,320,
                                            308,296,284,272,260,248,236,224,211,199,
                                            187,174,162,150,137,125,112,100,87,75,
                                            62,50,37,25,12,0,};


const unsigned short dac_offset = 350;
volatile unsigned short signals_timeout = 0;


// Module Private Functions ----------------------------------------------------
unsigned short Signals_Current_to_VoltagePts (unsigned short current_ua);
void Signals_Calc_Square_Outputs (treatment_conf_t * pconf);
void Signals_Set_Rising_Ouput (treatment_conf_t * pconf);
void Signals_Set_Falling_Ouput (treatment_conf_t * pconf);
void Signals_Get_High_Current (void);
void Signals_Get_Low_Current (void);

void Signals_Calc_Sinusoidal_Outputs (treatment_conf_t * pconf);
unsigned short Signals_Set_Sinusoidal_High (treatment_conf_t * pconf, unsigned short value);
unsigned short Signals_Set_Sinusoidal_Low (treatment_conf_t * pconf, unsigned short value);
unsigned short Signals_Calc_Sinusoidal_High (treatment_conf_t * pconf, unsigned short value);
unsigned short Signals_Calc_Sinusoidal_Low (treatment_conf_t * pconf, unsigned short value);

void Signal_Set_Sinusoidal_Cut (void);
void Signal_Reset_Sinusoidal_Cut (void);
unsigned char Signal_Get_Sinusoidal_Cut (void);
void Signal_Set_Sinusoidal_Cut_Last (unsigned short last_value);

// void Signals_Set_Frequency_Intensity_Change_Flag (void);
void Signals_Reset_Frequency_Intensity_Change_Flag (void);
unsigned char Signals_Get_Frequency_Intensity_Change_Flag (void);


// Module Functions ------------------------------------------------------------
void Signals_Timeouts (void)
{
    if (signals_timeout)
        signals_timeout--;
}


signals_square_states_e square_state = SQUARE_INIT;
void Signals_Square_Reset (void)
{
    square_state = SQUARE_INIT;
}


resp_e Signals_Square (treatment_conf_t * pconf)
{
    resp_e resp = resp_continue;
    timers_data_st td;
    
    switch (square_state)
    {
    case SQUARE_INIT:

        // calc timers registers
        td.freq_int = pconf->freq_int;
        td.freq_dec = pconf->freq_dec;
        resp = Signals_Timers_Calculation (&td);

        if (resp != resp_ok)    // frequency errors? go with defaults
        {
            td.freq_int = 40;
            td.freq_dec = 0;
            Signals_Timers_Calculation (&td);
        }
        
        // set output values
        Signals_Calc_Square_Outputs (pconf);

        // set timers here, check arr != 0
        Timer_Square_Signal_Reset();
        Timer_Square_Set_Registers (td.psc, td.arr_div_4);    //and start timer

        // check and set polarity
        // if POLARITY_ALT, set timer and start
        // freq = 0.3Hz psc: 6399 arr: 33333 arr_4: 8333
        Timer_Polarity (pconf->polarity);

        // all updates done!
        Signals_Reset_Frequency_Intensity_Change_Flag ();

        resp = resp_continue;
        square_state++;
        break;

    case SQUARE_FIRST_EDGE:
        if (Timer_Square_Signal_Ended())
        {
            Timer_Square_Signal_Reset();
            Signals_Set_Rising_Ouput (pconf);
            square_state++;
        }
        break;

    case SQUARE_POSITIVE_PLATEAU:
        if (Timer_Square_Signal_Ended())
        {
            Timer_Square_Signal_Reset();
            Signals_Get_High_Current ();
            square_state++;
        }

        // get meas every second on high edge
        if (!signals_timeout)
        {
            signals_timeout = 1000;
            Meas_Online (pconf->itov_high);
        }
        break;

    case SQUARE_SECOND_EDGE:
        if (Timer_Square_Signal_Ended())
        {
            Timer_Square_Signal_Reset();
            Signals_Set_Falling_Ouput (pconf);
            square_state++;
        }
        
        break;

    case SQUARE_NEGATIVE_PLATEAU:
        if (Timer_Square_Signal_Ended())
        {
            Timer_Square_Signal_Reset();
            Signals_Get_Low_Current ();

            if (Signals_Get_Frequency_Intensity_Change_Flag ())
                square_state = SQUARE_INIT;
            else
                square_state = SQUARE_FIRST_EDGE;
        }                
        break;
        
    default:
        square_state = SQUARE_INIT;
        break;
    }

    return resp;
}


// frequency of sinusoidal its fixed for all configurations
const unsigned short * psine;
signals_sine_states_e sine_state = SINE_INIT;
signals_square_cut_e square_cut_state = SQUARE_NO_CUTTING;
unsigned char meas_taken = 0;
resp_e Signals_Sinusoidal (treatment_conf_t * pconf)
{
    resp_e resp = resp_continue;
    timers_data_st td;
    
    switch (sine_state)
    {
    case SINE_INIT:

        // init the sinusoidal part of the signal
        psine = sinusoidal_table;

        // init the square cut part of signal
        td.freq_int = pconf->freq_int;
        td.freq_dec = pconf->freq_dec;
        resp = Signals_Timers_Calculation (&td);

        if (resp != resp_ok)    // frequency errors? go with defaults
        {
            td.freq_int = 40;
            td.freq_dec = 0;
            Signals_Timers_Calculation (&td);
        }

        // set values here
        Signals_Calc_Sinusoidal_Outputs (pconf);

        // set timers here, square_cut & sinusoidal (always same freq)!!!
        Timer_Square_Set_Registers (td.psc, td.arr >> 1);
        Timer_Sine_Set_Registers (639, 651);    // 0.6Hz * 256 points -> 6.51ms

        if (pconf->polarity != POLARITY_ALT)
            Timer_Polarity(pconf->polarity);
        else
            Timer_Polarity_Always_Left();

        // all updates done!
        Signals_Reset_Frequency_Intensity_Change_Flag ();

        meas_taken = 0;
        square_cut_state = SQUARE_NO_CUTTING;
        resp = resp_continue;
        sine_state++;
        break;

    case SINE_HIGH_HALF:
        if (Timer_Sine_Signal_Ended())
        {
            Timer_Sine_Signal_Reset();
            if (!Signal_Get_Sinusoidal_Cut())
            {
                unsigned short dac_value;
                dac_value = Signals_Set_Sinusoidal_High (pconf, *psine);
                // get a meas point on peak
                if ((psine >= &sinusoidal_table[127]) &&
                    (!meas_taken))
                {
                    meas_taken = 1;
                    Meas_Sine (dac_value);                    
                }
            }
            else
            {
                unsigned short dac_saved = 0;
                dac_saved = Signals_Calc_Sinusoidal_High (pconf, *psine);
                Signal_Set_Sinusoidal_Cut_Last(dac_saved + dac_offset);
            }

            if (psine < &sinusoidal_table[TABLE_SIZE - 1])
                psine++;
            else
            {
                psine = sinusoidal_table;
                meas_taken = 0;

                if (pconf->polarity == POLARITY_ALT)
                    Timer_Polarity_Always_Right();
                
                sine_state = SINE_LOW_HALF;
            }
        }
        break;

    case SINE_LOW_HALF:
        if (Timer_Sine_Signal_Ended())
        {
            Timer_Sine_Signal_Reset();
            if (!Signal_Get_Sinusoidal_Cut())
            {
                unsigned short dac_value;
                dac_value = Signals_Set_Sinusoidal_Low (pconf, *psine);
                // get a meas point on peak
                if ((psine >= &sinusoidal_table[127]) &&
                    (!meas_taken))
                {
                    meas_taken = 1;
                    Meas_Sine (dac_value);                    
                }                
            }
            else
            {
                unsigned short dac_saved = 0;
                dac_saved = Signals_Calc_Sinusoidal_Low (pconf, *psine);
                Signal_Set_Sinusoidal_Cut_Last(dac_saved + dac_offset);
            }

            if (psine < &sinusoidal_table[TABLE_SIZE - 1])
                psine++;
            else
            {
                psine = sinusoidal_table;
                meas_taken = 0;                

                if (pconf->polarity == POLARITY_ALT)
                    Timer_Polarity_Always_Left();

                if (Signals_Get_Frequency_Intensity_Change_Flag ())
                    sine_state = SINE_INIT;
                else
                    sine_state = SINE_HIGH_HALF;
                
            }
        }
        break;
        
    default:
        sine_state = SINE_INIT;
        break;
    }

    
    switch (square_cut_state)
    {
    case SQUARE_NO_CUTTING:
        if (Timer_Square_Signal_Ended())
        {
            Timer_Square_Signal_Reset();
            Signal_Set_Sinusoidal_Cut();
            square_cut_state++;
        }
        break;

    case SQUARE_CUTTING:
        if (Timer_Square_Signal_Ended())
        {
            Timer_Square_Signal_Reset();
            Signal_Reset_Sinusoidal_Cut();
            square_cut_state = SQUARE_NO_CUTTING;
        }
        break;
    }
    
    return resp;
}


void Signals_Sinusoidal_Reset (void)
{
    sine_state = SINE_INIT;    
}


resp_e Signals_Timers_Calculation (timers_data_st * td)
{
    // first check limits
    // no more than 1000
    if (td->freq_int > 1000)
        return resp_error;

    // decimal error?
    if (td->freq_dec > 99)
        return resp_error;

    // two decimal points? go to only one
    if (td->freq_dec > 9)
        td->freq_dec = td->freq_dec / 10;
    
    // no less than 0.1
    if ((td->freq_int == 0) && (td->freq_dec == 0))
        return resp_error;

    
    unsigned int tot_t = 0;
    unsigned short freq_sel = td->freq_int * 10 + td->freq_dec;
    
    if (td->freq_int > 10)    // from 1000 to 10
    {
        tot_t = 10000000;    //1e6 * 10
        tot_t = tot_t / freq_sel;
        td->arr = tot_t;
        td->arr_div_4 = tot_t >> 2;
        td->psc = 63;
            
    }
    else    //from 9.9 to 0.1    
    {
        tot_t = 100000;    //1e4 * 10
        tot_t = tot_t / freq_sel;
        td->arr = tot_t;
        td->arr_div_4 = tot_t >> 2;
        td->psc = 6399;
        
    }

    return resp_ok;
    
}


unsigned short Signals_Current_to_VoltagePts (unsigned short current_ua)
{
    unsigned int calc = 0;

    // FOR 3.3K POWER GEN
    // current on 3k3 resistor to voltage on 3.3V
    // ex. 25uA * 3300 * 4095 / 3.3V = 102pts
    // algo 25 * 4095 / 1000 = 102pts
    // calc = current_ua * 4095;
    // calc = calc / 1000;

    // FOR 2.7K POWER GEN
    // current on 2k7 resistor to voltage on 3.3V
    // ex. 25uA * 2700 * 4095 / 3.3V = 83pts
    // algo 25 * 819 * 4095 / 1000000 = 83pts
    calc = current_ua * 819;
    calc = calc / 10;
    calc = calc * 4095;
    calc = calc / 100000;
    
    return (unsigned short) calc;
}


void Signals_Calc_Square_Outputs (treatment_conf_t * pconf)
{
    int calc = 0;
    unsigned short bottom = 0;
    unsigned short top = 0;

    calc = Signals_Current_to_VoltagePts (pconf->intensity);
    
    // current <= 100, bottom half top the selected current
    if (pconf->intensity <= 100)
    {
        bottom = calc >> 1;
        top = calc + bottom;
    }
    else
    {
        // bottom in 100uA top double of selected current
        // ex. 600uA bottom 100uA; top = (600 - 100) * 2 + 100
        bottom = Signals_Current_to_VoltagePts (100);
        top = (calc - bottom) * 2 + bottom;
    }
    
    if (top > 4095)
        top = 4095;

    pconf->itov_low = bottom;    
    pconf->itov_high = top;    
}


void Signals_Set_Rising_Ouput (treatment_conf_t * pconf)
{
    unsigned short to_dac;
    to_dac = pconf->itov_high + dac_offset + 90;

    if (to_dac > 4095)
        to_dac = 4095;
    
    DAC_Output1(to_dac);
}


void Signals_Set_Falling_Ouput (treatment_conf_t * pconf)
{
    unsigned short to_dac;
    to_dac = pconf->itov_low + dac_offset + 90;

    if (to_dac > 4095)
        to_dac = 4095;
    
    DAC_Output1(to_dac);
}


void Signals_Get_High_Current (void)
{
}


void Signals_Get_Low_Current (void)
{
}


void Signals_Calc_Sinusoidal_Outputs (treatment_conf_t * pconf)
{
    int calc = 0;
    int peak_curr = 0;

    // limits checks

    // ex. 100uA -> intensity = 100
    // peak current = iset(mean) * 1.41 * 2 / 0.9
    // ex. algo iset * 3.13
    calc = pconf->intensity * 313;
    calc = calc / 100;
    peak_curr = Signals_Current_to_VoltagePts (calc);

    if (pconf->polarity == POLARITY_ALT)
    {
        // special case high & low are the same
        if (peak_curr > 4095)
            peak_curr = 4095;

        pconf->itov_low = peak_curr;
        pconf->itov_high = peak_curr;
    }
    else
    {
        calc = peak_curr * 75;
        calc = calc / 100;    //75% of peak
    
        if (calc > 4095)
            calc = 4095;

        pconf->itov_low = calc;

        calc = peak_curr * 125;
        calc = calc / 100;    //125% of peak

        if (calc > 4095)
            calc = 4095;

        pconf->itov_high = calc;
    }
}


unsigned short Signals_Set_Sinusoidal_High (treatment_conf_t * pconf, unsigned short value)
{
    unsigned short dac_value = 0;

    dac_value = Signals_Calc_Sinusoidal_High (pconf, value);
    dac_value += dac_offset;

    if (dac_value > 4095)
        dac_value = 4095;
    
    DAC_Output1 (dac_value);

    return dac_value;
}


unsigned short Signals_Calc_Sinusoidal_High (treatment_conf_t * pconf, unsigned short value)
{
    int calc = 0;

    calc = pconf->itov_high * value;
    calc >>= 10;

    if (calc > 4095)
        calc = 4095;
        
    return (unsigned short) calc;
}


unsigned short Signals_Set_Sinusoidal_Low (treatment_conf_t * pconf, unsigned short value)
{
    unsigned short dac_value = 0;

    dac_value = Signals_Calc_Sinusoidal_Low (pconf, value);
    dac_value += dac_offset;

    if (dac_value > 4095)
        dac_value = 4095;
    
    DAC_Output1 (dac_value);
    
    // dac_value = Signals_Calc_Sinusoidal_Low (pconf, value);
    // DAC_Output1 (dac_value);

    return dac_value;
}


unsigned short Signals_Calc_Sinusoidal_Low (treatment_conf_t * pconf, unsigned short value)
{
    int calc = 0;

    calc = pconf->itov_low * value;
    calc >>= 10;

    if (calc > 4095)
        calc = 4095;
        
    return (unsigned short) calc;
}


unsigned char sinusoidal_cut = 0;
unsigned short sinusoidal_cut_last_value = 0;
void Signal_Set_Sinusoidal_Cut (void)
{
    sinusoidal_cut = 1;
    DAC_Output1 (0);
}


void Signal_Reset_Sinusoidal_Cut (void)
{
    sinusoidal_cut = 0;
    DAC_Output1 (sinusoidal_cut_last_value);
}


unsigned char Signal_Get_Sinusoidal_Cut (void)
{
    return sinusoidal_cut;
}


void Signal_Set_Sinusoidal_Cut_Last (unsigned short last_value)
{
    sinusoidal_cut_last_value = last_value;
}


void Signals_Stop (void)
{
    Timer_Square_Signal_Stop();
    Signals_Square_Reset();

    Timer_Sine_Signal_Stop();
    Signals_Sinusoidal_Reset();

    DAC_Output1(0);
    DAC_Output2(0);    
}


unsigned char signal_freq_intensity_flag = 0;
void Signals_Set_Frequency_Intensity_Change_Flag (void)
{
    signal_freq_intensity_flag = 1;
}


void Signals_Reset_Frequency_Intensity_Change_Flag (void)
{
    signal_freq_intensity_flag = 0;
}


unsigned char Signals_Get_Frequency_Intensity_Change_Flag (void)
{
    return signal_freq_intensity_flag;
}

//--- end of file ---//
