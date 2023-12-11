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

#include "hard.h"
#include "tim.h"
// #include "dsp.h"
#include "adc.h"
// #include "pwm.h"
#include "channels_defs.h"

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


// Module Private Functions ----------------------------------------------------
// void Signals_Generate_Channel (unsigned char which_channel, unsigned short new_sp);
// void Signals_Setup_Phase_Accumulator (unsigned char freq_int,
//                                       unsigned char freq_dec,
//                                       unsigned short * phase_accum);


// Module Functions ------------------------------------------------------------
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

        td.freq_int = pconf->freq_int;
        td.freq_dec = pconf->freq_dec;
        resp = Signals_Timers_Calculation (&td);

        if (resp == resp_ok)
        {
            resp = resp_continue;
            // set timers here!!!
            Timer_Set_Registers (td.psc, td.arr_div_4);
            square_state++;
        }
        break;

    case SQUARE_FIRST_EDGE:
        if (Timer_Signal_Ended())
        {
            Timer_Signal_Reset();
            Signals_Set_Rising_Ouput (pconf);
            square_state++;
        }
        break;

    case SQUARE_POSITIVE_PLATEAU:
        if (Timer_Signal_Ended())
        {
            Timer_Signal_Reset();
            Signals_Get_High_Current ();
            square_state++;
        }        
        break;

    case SQUARE_SECOND_EDGE:
        if (Timer_Signal_Ended())
        {
            Timer_Signal_Reset();
            Signals_Set_Falling_Ouput (pconf);
            square_state++;
        }
        
        break;

    case SQUARE_NEGATIVE_PLATEAU:
        if (Timer_Signal_Ended())
        {
            Timer_Signal_Reset();
            Signals_Get_Low_Current ();
            square_state = SQUARE_FIRST_EDGE;
        }                
        break;
        
    defautl:
        square_state = SQUARE_INIT;
        break;
    }

    return resp;
}


// frequency of sinusoidal its fixed for all configurations
const unsigned short * psine;
signals_sine_states_e sine_state = SINE_INIT;
signals_square_cut_e square_cut_state = SQUARE_NO_CUTTING;
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

        if (resp == resp_ok)
        {
            resp = resp_continue;
            // set timers square_cut here!!!
            Timer_Set_Registers (td.psc, td.arr);

            square_cut_state = SQUARE_NO_CUTTING;
            sine_state++;
        }
        break;

    case SINE_HIGH_HALF:
        if (Timer_Sine_Signal_Ended())
        {
            Timer_Sine_Signal_Reset();
            if (!Signal_Get_Sinusoidal_Cut())
                Signals_Set_Sinusoidal_High (pconf, *psine);

            if (psine < &sinusoidal_table[TABLE_SIZE - 1])
                psine++;
            else
            {
                psine = sinusoidal_table;
                sine_state = SINE_LOW_HALF;
            }
        }
        break;

    case SINE_LOW_HALF:
        if (Timer_Sine_Signal_Ended())
        {
            Timer_Sine_Signal_Reset();
            if (!Signal_Get_Sinusoidal_Cut())
                Signals_Set_Sinusoidal_Low (pconf, *psine);

            if (psine < &sinusoidal_table[TABLE_SIZE - 1])
                psine++;
            else
            {
                psine = sinusoidal_table;
                sine_state = SINE_HIGH_HALF;
            }
        }
        break;
        
    defautl:
        sine_state = SINE_INIT;
        break;
    }

    
    switch (square_cut_state)
    {
    case SQUARE_NO_CUTTING:
        if (Timer_Signal_Ended())
        {
            printf("  sq cutting on\n");            
            Timer_Signal_Reset();
            Signal_Set_Sinusoidal_Cut();
            square_cut_state++;
        }
        break;

    case SQUARE_CUTTING:
        if (Timer_Signal_Ended())
        {
            printf("  sq cutting off\n");
            Timer_Signal_Reset();
            Signal_Reset_Sinusoidal_Cut();
            square_cut_state = SQUARE_NO_CUTTING;
        }
        break;
    }
    
    return resp;
}


resp_e Signals_Timers_Calculation (timers_data_st * td)
{
    // first check limits
    // no more than 1000
    if (td->freq_int > 1000)
        return resp_error;

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




//--- end of file ---//
