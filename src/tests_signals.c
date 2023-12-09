//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "signals.h"
#include "signals_defs.h"
#include "treatment.h"



// helper modules
#include "tests_ok.h"
#include "tests_mock_usart.h"
#include "tests_vector_utils.h"
#include "tests_know_antennas.h"

#include <stdio.h>
// #include <math.h>


// Types Constants and Macros --------------------------------------------------
#define SIZEOF_SIGNALS    512


// Externals -------------------------------------------------------------------
volatile unsigned short adc_ch [10];
volatile unsigned char timer1_seq_ready = 0;

extern void Signals_Setup_Phase_Accumulator (unsigned char freq_int,
                                             unsigned char freq_dec,
                                             unsigned short * phase_accum);



// Globals ---------------------------------------------------------------------



// Module Functions to Test ----------------------------------------------------
void Test_Signals_Timers_Calc_Single_Freq (void);
void Test_Signals_Timers_Calc_All_Allowed_Freqs (void);
void Test_Signals_Timers_Calc_All_Integer_Freqs (void);

void Test_Signals_Square (void);
void Test_Signals_Sinusoidal (void);


// Module Auxiliary Functions --------------------------------------------------


// Tests Module Auxiliary or General Functions ---------------------------------
// Mocked Functions Declaration ------------------------------------------------
unsigned char Timer_Signal_Ended (void);
void Timer_Signal_Reset (void);
void Timer_Signal_Set (void);
void Signals_Set_Rising_Ouput (treatment_conf_t * td);
void Signals_Set_Falling_Ouput (treatment_conf_t * td);
void Signals_Get_High_Current (void);
void Signals_Get_Low_Current (void);

unsigned char Timer_Sine_Signal_Ended (void);
void Timer_Sine_Signal_Reset (void);
void Timer_Sine_Signal_Set (void);



// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    // Test_Signals_Timers_Calc_Single_Freq ();

    // Test_Signals_Timers_Calc_All_Allowed_Freqs ();

    // Test_Signals_Timers_Calc_All_Integer_Freqs ();

    // Test_Signals_Square ();

    Test_Signals_Sinusoidal ();    

    return 0;
}


void Test_Signals_Timers_Calc_Single_Freq (void)
{
    printf("Testing Timers Calcs...\n micro Freq: 64MHz\n");

    resp_e resp;
    timers_data_st td;

    // set single frequency to test
    td.freq_int = 320;
    td.freq_dec = 0;

    resp = Signals_Timers_Calculation (&td);

    if (resp == resp_ok)
    {
        printf("psc: %d arr: %d arr_4: %d\n", td.psc, td.arr, td.arr_div_4);
        unsigned int clk = 64000000;
        unsigned int tclk = clk / (td.psc + 1);
        float ticks = 1. / tclk;
        float fget = 1. / (td.arr_div_4 * 4 * ticks);
        printf("frequency getted: %f\n", fget);
    }
    else
        printf("error on calcs!\n");
    
}


void Test_Signals_Timers_Calc_All_Allowed_Freqs (void)
{
    printf("Testing Timers Calcs...\n micro Freq: 64MHz\n");

    resp_e resp;
    timers_data_st td;

    for (int i = 1; i < 1000; i++)
    {
        unsigned int inte = i / 10;
        td.freq_int = inte;
        td.freq_dec = i - inte * 10;
        // printf("%d.%01d\n", td.freq_int, td.freq_dec);

        resp = Signals_Timers_Calculation (&td);

        if (resp == resp_ok)
        {
            printf("psc: %d arr: %d arr_4: %d\n", td.psc, td.arr, td.arr_div_4);
            unsigned int clk = 64000000;
            unsigned int tclk = clk / (td.psc + 1);
            float ticks = 1. / tclk;
            float fget = 1. / (td.arr_div_4 * 4 * ticks);
            printf("frequency set: %d.%01d getted: %f\n",
                   td.freq_int,
                   td.freq_dec,
                   fget);
        }
        else
        {
            printf("error on calcs!\n");
            break;
        }
    }    
}


void Test_Signals_Timers_Calc_All_Integer_Freqs (void)
{
    printf("Testing Timers Calcs...\n micro Freq: 64MHz\n");

    resp_e resp;
    timers_data_st td;

    td.freq_dec = 0;
    for (int i = 1; i < 320; i++)
    {
        td.freq_int = i;
        // printf("%d.%01d\n", td.freq_int, td.freq_dec);

        resp = Signals_Timers_Calculation (&td);

        if (resp == resp_ok)
        {
            printf("psc: %d arr: %d arr_4: %d\n", td.psc, td.arr, td.arr_div_4);
            unsigned int clk = 64000000;
            unsigned int tclk = clk / (td.psc + 1);
            float ticks = 1. / tclk;
            float fget = 1. / (td.arr_div_4 * 4 * ticks);
            printf("frequency set: %d.%01d getted: %f\n",
                   td.freq_int,
                   td.freq_dec,
                   fget);
        }
        else
        {
            printf("error on calcs!\n");
            break;
        }
    }    
}


void Test_Signals_Square (void)
{
    int loops = 2;
    printf("Testing Signals_Square, only few loops: %d\n", loops);

    resp_e resp;
    timers_data_st td;
    treatment_conf_t signal_config;

    signal_config.freq_int = 10;
    signal_config.freq_dec = 9;

    Timer_Signal_Set();
    for (int i = 0; i < (loops * 4) + 1; i++)
    {
        resp = Signals_Square (&signal_config);
        if (resp != resp_continue)
        {
            printf("looping error on: %d\n", i);
            break;
        }
    }

    printf("\n");
}


void Test_Signals_Sinusoidal (void)
{
    int loops = 4;
    int table_size = 256;
    printf("Testing on Signals_Sinusoidal, only few loops: %d\n", loops);

    resp_e resp;
    timers_data_st td;
    treatment_conf_t signal_config;

    signal_config.freq_int = 10;
    signal_config.freq_dec = 9;

    Timer_Sine_Signal_Set();
    for (int i = 0; i < (loops * table_size); i++)
    {
        resp = Signals_Sinusoidal (&signal_config);
        if (resp != resp_continue)
        {
            printf("looping error on: %d\n", i);
            break;
        }
    }

    printf("\n");
}




// Mocked Module Functions -----------------------------------------------------
unsigned char timer_ended = 0;
unsigned char Timer_Signal_Ended (void)
{
    return timer_ended;
}


void Timer_Signal_Reset (void)
{
    timer_ended = 0;
}


void Timer_Signal_Set (void)
{
    timer_ended = 1;
}


unsigned char timer_sine_ended = 0;
unsigned char Timer_Sine_Signal_Ended (void)
{
    return timer_sine_ended;
}


void Timer_Sine_Signal_Reset (void)
{
    timer_sine_ended = 0;
}


void Timer_Sine_Signal_Set (void)
{
    timer_sine_ended = 1;
}


void Signals_Set_Rising_Ouput (treatment_conf_t * td)
{
    printf("/");
    Timer_Signal_Set();
}


void Signals_Set_Falling_Ouput (treatment_conf_t * td)
{
    printf("\\");
    Timer_Signal_Set();    
}


void Signals_Get_High_Current (void)
{
    printf("-");
    Timer_Signal_Set();    
}


void Signals_Get_Low_Current (void)
{
    printf("_");
    Timer_Signal_Set();    
}


unsigned char sinusoidal_cut = 0;
void Signal_Set_Sinusoidal_Cut (void)
{
    sinusoidal_cut = 1;
}


unsigned char Signal_Get_Sinusoidal_Cut (void)
{
    return sinusoidal_cut;
}


int report_high = 0;
int high_cnt = 0;
int report_low = 0;
int low_cnt = 0;
void Signals_Set_Sinusoidal_High (treatment_conf_t * td, unsigned short value)
{
    Timer_Sine_Signal_Set ();

    if (!report_low)
    {
        printf("low_cnt: %d high value: %d\n", low_cnt, value);
        low_cnt = 0;
        report_low = 1;
        report_high = 0;
    }
    // else
    //     printf(" high_cnt: %d high value: %d\n", high_cnt, value);
    
    high_cnt++;

}


void Signals_Set_Sinusoidal_Low (treatment_conf_t * td, unsigned short value)
{
    Timer_Sine_Signal_Set ();    

    if (!report_high)
    {
        printf("high_cnt: %d low value: %d\n", high_cnt, value);
        high_cnt = 0;       
        report_high = 1;
        report_low = 0;
    }
    // else
    //     printf(" low_cnt: %d low value: %d\n", low_cnt, value);
    
    low_cnt++;
}

void Timer_Set_Registers (unsigned short psc, unsigned short arr)
{
    if (arr == 0)
        return;
    
    unsigned int fclk = 64000000;
    fclk = fclk / (psc + 1);
    unsigned int step_us = arr * 1000000 / fclk;
    
    float freq = (float) fclk / (float) arr;

    
    printf(" setting timer scalers psc: %d arr: %d\n", psc, arr);
    printf(" step time: %dus f1: %.1fHz f4: %.1fHz\n", step_us, freq, freq / 4);
}

//--- end of file ---//


