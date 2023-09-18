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
#include "tim.h"
#include "dsp.h"

#include "antennas_defs.h"

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
short v_duty_ch1 [SIZEOF_SIGNALS] = { 0 };
short v_error_ch1 [SIZEOF_SIGNALS] = { 0 };
short v_sp_ch1 [SIZEOF_SIGNALS] = { 0 };

short v_duty_ch2 [SIZEOF_SIGNALS] = { 0 };
short v_error_ch2 [SIZEOF_SIGNALS] = { 0 };
short v_sp_ch2 [SIZEOF_SIGNALS] = { 0 };

short v_duty_ch3 [SIZEOF_SIGNALS] = { 0 };
short v_error_ch3 [SIZEOF_SIGNALS] = { 0 };
short v_sp_ch3 [SIZEOF_SIGNALS] = { 0 };

short v_duty_ch4 [SIZEOF_SIGNALS] = { 0 };
short v_error_ch4 [SIZEOF_SIGNALS] = { 0 };
short v_sp_ch4 [SIZEOF_SIGNALS] = { 0 };


// Module Functions to Test ----------------------------------------------------
void Test_PI_Functions (void);
void Test_PI_Simul (void);
void Test_Generate_Setup (void);
void Test_Generate_All_Channels (void);
void Test_Set_Channel_PI_Parameters (void);
void Test_Setup_Phase_And_Increment (void);


// Module Auxiliary Functions --------------------------------------------------


// Tests Module Auxiliary or General Functions ---------------------------------


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    // Test_PI_Functions ();
    // Test_PI_Simul ();
    // Test_Generate_Setup ();
    // Test_Generate_All_Channels ();
    // Test_Set_Channel_PI_Parameters ();
    Test_Setup_Phase_And_Increment ();    

    return 0;
}


extern const unsigned short * p_table_inphase;
extern const unsigned short * p_table_outphase;
extern pi_data_obj_t pi_ch1;
extern pi_data_obj_t pi_ch2;
extern pi_data_obj_t pi_ch3;
extern pi_data_obj_t pi_ch4;
extern unsigned short signal_index;
void Test_Generate_Setup (void)
{
    int pointers_are_null = 0;
    int some_error = 0;
    
    if ((p_table_inphase == NULL) &&
        (p_table_outphase == NULL))
        pointers_are_null = 1;

    signal_index = 1;
    Signals_Setup_All_Channels ();

    printf("pointer signal assigment: ");
    if ((p_table_inphase != NULL) &&
        (p_table_outphase != NULL) &&
        (pointers_are_null))
        PrintOK();
    else
        PrintERR();

    printf("pointer index: ");
    if (!signal_index)
        PrintOK();
    else
        PrintERR();    
    
}


extern signals_struct_t global_signals;
void Test_Set_Channel_PI_Parameters (void)
{
    antenna_st my_ant;
    // unsigned char antenna_index = 0;
    // unsigned char antenna_index = 6;
    unsigned char antenna_index = 8;
    
    TSP_Get_Know_Single_Antenna (&my_ant, antenna_index);
    
    Signals_Set_Channel_PI_Parameters (0, &my_ant);

    printf("global_signals params:\n");
    printf(" ki: %d\n", global_signals.ki_ch1);
    printf(" kp: %d\n", global_signals.kp_ch1);
    printf(" max_c: %d\n", global_signals.max_c_ch1);           
}


void Test_Setup_Phase_And_Increment (void)
{
    unsigned char freq_int = 0;
    unsigned char freq_dec = 50;
    unsigned short phase_a = 0;
    
    for (int i = 0; i < 200; i += 10)
    {
        freq_int = i;
        Signals_Setup_Phase_Accumulator(freq_int, freq_dec, &phase_a);

        float eff_freq = phase_a * 7000.0 / (256 * 256);
        printf("estimated for sampling 7000Hz freq: %f\n", eff_freq);
        printf("with %02d.%02dHz phase acc: %d\n\n",
               freq_int,
               freq_dec,
               phase_a);
    }

}


void Test_Generate_All_Channels (void)
{
    
    Signals_Setup_All_Channels();

    for (int i = 0; i < (SIZEOF_SIGNALS - 1); i++)
    {
        timer1_seq_ready = 1;
        Signals_Generate_All_Channels ();

        // save ch1 data
        v_duty_ch1[i] = pi_ch1.last_d;
        v_sp_ch1[i] = pi_ch1.setpoint;
        v_error_ch1[i] = pi_ch1.setpoint - pi_ch1.sample;

        // save ch2 data
        v_duty_ch2[i] = pi_ch2.last_d;
        v_sp_ch2[i] = pi_ch2.setpoint;
        v_error_ch2[i] = pi_ch2.setpoint - pi_ch2.sample;

        // save ch3 data
        v_duty_ch3[i] = pi_ch3.last_d;
        v_sp_ch3[i] = pi_ch3.setpoint;
        v_error_ch3[i] = pi_ch3.setpoint - pi_ch3.sample;

        // save ch4 data
        v_duty_ch4[i] = pi_ch4.last_d;
        v_sp_ch4[i] = pi_ch4.setpoint;
        v_error_ch4[i] = pi_ch4.setpoint - pi_ch4.sample;
        
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

    Vector_Short_To_File (file, "duty", v_duty_ch1, SIZEOF_SIGNALS);
    Vector_Short_To_File (file, "error", v_error_ch1, SIZEOF_SIGNALS);
    Vector_Short_To_File (file, "setpoint", v_sp_ch1, SIZEOF_SIGNALS);
    printf("\nRun by hand python3 simul_outputs.py\n");
    
}


void Test_PI_Functions (void)
{
    printf("Testing PI roof settings: ");

    pi_data_obj_t pi;
    pi.kp = 5;
    pi.ki = 5;
    pi.last_d = 100;
    pi.error_z1 = 100;

    PI_roof_Flush(&pi);
        
    if ((pi.last_d != 0) ||
        (pi.error_z1 != 0))
    {
        PrintERR();
    }
    else
        PrintOK();

    pi.kp = 5;
    pi.ki = 5;
    PI_roof_Flush(&pi);
        
    pi.setpoint = 1000;        
    for (int i = 0; i < 1000; i++)
    {
        pi.sample = pi.last_d;
        PI_roof(&pi);
    }
    printf("pi roof last_d: %d error_z1: %d\n", pi.last_d, pi.error_z1);
    
    printf("Testing PI roof performance: ");    
    if ((pi.last_d != 0) ||
        (pi.error_z1 != 1))
    {
        PrintERR();
    }
    else
        PrintOK();
    
}


void Test_PI_Simul (void)
{
    printf("\nTest PI and simulate\n");

    short vduty [1000] = { 0 };
    short verror [1000] = { 0 };
    short vsp [1000] = { 0 };

    pi_data_obj_t pi;    
    pi.kp = 10;
    pi.ki = 10;
    PI_roof_Flush(&pi);
        
    pi.setpoint = 1000;        
    for (int i = 0; i < 1000; i++)
    {
        pi.sample = pi.last_d;
        PI_roof(&pi);

        vduty[i] = pi.last_d;
        verror[i] = pi.error_z1;
        vsp[i] = pi.setpoint;
    }
    
    printf("pi roof last_d: %d error_z1: %d\n", pi.last_d, pi.error_z1);
    
    
    ///////////////////////////
    // Backup Data to a file //
    ///////////////////////////
    FILE * file = fopen("data.txt", "w");

    if (file == NULL)
    {
        printf("data file not created!\n");
        return;
    }

    Vector_Short_To_File (file, "duty", vduty, 1000);
    Vector_Short_To_File (file, "error", verror, 1000);
    Vector_Short_To_File (file, "setpoint", vsp, 1000);
    printf("\nRun by hand python3 simul_outputs.py\n");

}


// Mocked Module Functions -----------------------------------------------------
void TIM8_Update_CH3 (unsigned short a)
{
    printf("HL CH1: %d\n", a);
}

void TIM8_Update_CH4 (unsigned short a)
{
    printf("LR CH1: %d\n", a);
}

void TIM8_Update_CH2 (unsigned short a)
{
    printf("HL CH2: %d\n", a);
}

void TIM8_Update_CH1 (unsigned short a)
{
    printf("LR CH2: %d\n", a);
}

void TIM4_Update_CH2 (unsigned short a)
{
    printf("HL CH3: %d\n", a);
}

void TIM4_Update_CH3 (unsigned short a)
{
    printf("LR CH3: %d\n", a);
}

void TIM5_Update_CH1 (unsigned short a)
{
    printf("HL CH4: %d\n", a);
}

void TIM5_Update_CH2 (unsigned short a)
{
    printf("LR CH4: %d\n", a);
}

void UpdateLed (void)
{
}

void UpdateBuzzer (void)
{
}

void UpdateRaspberryMessages (void)
{
}

void Error_SetStatus (unsigned char error, unsigned char channel)
{
    error <<= 4;
    error += channel + 1;
    printf("error: 0x%02x\n", error);
}

void Led1_On (void)
{
    // printf("Led1 -> ON\n");
}


void Led1_Off (void)
{
    // printf("Led1 -> OFF\n");    
}

//--- end of file ---//


