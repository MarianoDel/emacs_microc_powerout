//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS_SIGNALS.C #######################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "signals.h"

// helper modules
#include "tests_ok.h"
#include "tests_vector_utils.h"

#include <stdio.h>
#include <math.h>

// Types Constants and Macros --------------------------------------------------


// Externals -------------------------------------------------------------------



// Globals ---------------------------------------------------------------------



// Module Functions ------------------------------------------------------------
void Test_Generate_Square (void);
void Test_Generate_Sinusoidal (void);


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

void Signal_Reset_Sinusoidal_Cut (void);


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    // Test_Generate_Square ();

    Test_Generate_Sinusoidal ();    
    
    return 0;
}


#define SIZEOF_SIGNALS    (4 * 4)    // 4 stages and 4 loops
short vsquare [SIZEOF_SIGNALS] = { 0 };
int cnt = 0;

void Test_Generate_Square (void)
{
    printf("Square Generation...\n");
    
    // int loops = 4;

    resp_e resp;
    timers_data_st td;
    treatment_conf_t signal_config;

    signal_config.freq_int = 10;
    signal_config.freq_dec = 9;

    Timer_Signal_Set();
    for (int i = 0; i < SIZEOF_SIGNALS + 1; i++)
    {
        resp = Signals_Square (&signal_config);
        if (resp != resp_continue)
        {
            printf("looping error on: %d\n", i);
            break;
        }

        if (i)
            cnt++;
    }
    
    ///////////////////////////
    // Backup Data to a file //
    ///////////////////////////
    FILE * file = fopen("data.txt", "w");

    if (file == NULL)
    {
        printf("data file not created!\n");
        return;
    }

    // Vector_Short_To_File (file, "duty1", v_duty_ch1, SIZEOF_SIGNALS);
    Vector_Short_To_File (file, "square", vsquare, SIZEOF_SIGNALS);
    
    printf("\nRun by hand python3 simul_outputs.py\n");
    
}


#define SINUSOIDAL_LOOPS    2
#define SINUSOIDAL_SIZE    256
short vsine [SINUSOIDAL_SIZE * SINUSOIDAL_LOOPS] = { 0 };
int cutting_roof;
int cutting_cnt = 0;
void Test_Generate_Sinusoidal (void)
{
    printf("Sinusoidal Generation...\n");

    resp_e resp;
    timers_data_st td;
    treatment_conf_t signal_config;

    signal_config.freq_int = 10;
    signal_config.freq_dec = 9;

    cutting_roof = (SINUSOIDAL_SIZE >> 4);
    // cutting_roof = SINUSOIDAL_SIZE * 2;    //no cuts

    // Signal_Reset_Sinusoidal_Cut();
    Timer_Sine_Signal_Set();
    for (int i = 0; i < (SINUSOIDAL_SIZE * SINUSOIDAL_LOOPS) + 1; i++)
    {
        resp = Signals_Sinusoidal (&signal_config);
        if (resp != resp_continue)
        {
            printf("looping error on: %d\n", i);
            break;
        }

        if (i)
            cnt++;
    }
    
    ///////////////////////////
    // Backup Data to a file //
    ///////////////////////////
    FILE * file = fopen("data.txt", "w");

    if (file == NULL)
    {
        printf("data file not created!\n");
        return;
    }

    // Vector_Short_To_File (file, "duty1", v_duty_ch1, SIZEOF_SIGNALS);
    Vector_Short_To_File (file, "sine", vsine, SINUSOIDAL_SIZE * SINUSOIDAL_LOOPS);
    
    printf("\nRun by hand python3 simul_outputs.py\n");
    
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
    vsquare[cnt] = 1000;
    Timer_Signal_Set();
}


void Signals_Set_Falling_Ouput (treatment_conf_t * td)
{
    vsquare[cnt] = 0;
    Timer_Signal_Set();    
}


void Signals_Get_High_Current (void)
{
    vsquare[cnt] = 1000;    
    Timer_Signal_Set();    
}


void Signals_Get_Low_Current (void)
{
    vsquare[cnt] = 0;
    Timer_Signal_Set();    
}


unsigned char sinusoidal_cut = 0;
void Signal_Set_Sinusoidal_Cut (void)
{
    sinusoidal_cut = 1;
    vsine[cnt] = 0;
}


void Signal_Reset_Sinusoidal_Cut (void)
{
    sinusoidal_cut = 0;
}


unsigned char Signal_Get_Sinusoidal_Cut (void)
{
    // for square cuts
    if (cutting_cnt < cutting_roof)
        cutting_cnt++;
    else
    {
        // printf("  cutting loop to 0 on: %d\n", cutting_cnt);
        cutting_cnt = 0;
        Timer_Signal_Set();
    }

    if (sinusoidal_cut)    // up cnt here
        Timer_Sine_Signal_Set ();
    
    return sinusoidal_cut;
}


void Signals_Set_Sinusoidal_High (treatment_conf_t * td, unsigned short value)
{
    Timer_Sine_Signal_Set ();

    vsine[cnt] = value;

    // printf(" high cnt: %d value: %d\n", cnt, value);
}


void Signals_Set_Sinusoidal_Low (treatment_conf_t * td, unsigned short value)
{
    int val;

    Timer_Sine_Signal_Set ();    

    val = value * 75;
    val = val / 100;
    vsine[cnt] = val;

    // printf(" low cnt: %d value: %d\n", cnt, value);
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


