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
#include "dsp.h"
#include "tests_know_antennas.h"
#include "tim.h"    // for TIM timers

// helper modules
#include "tests_ok.h"
#include "tests_vector_utils.h"
#include "tests_recursive_utils.h"

#include <stdio.h>
#include <math.h>

// Types Constants and Macros --------------------------------------------------
// #define SIZEOF_SIGNALS    (256 * 6)    // six cycles
// #define SIZEOF_SIGNALS    512    // two cycles
#define SIZEOF_SIGNALS    256    // only one cycle

#define CH1    0
#define CH2    1
#define CH3    2
#define CH4    3


#define IS_CH4    adc_ch[0]
#define IS_CH3    adc_ch[1]
#define IS_CH2    adc_ch[2]
#define IS_CH1    adc_ch[3]



// Externals -------------------------------------------------------------------
volatile unsigned short adc_ch [10];
volatile unsigned char timer1_seq_ready = 0;


// Globals ---------------------------------------------------------------------
short v_duty [SIZEOF_SIGNALS] = { 0 };
short v_error [SIZEOF_SIGNALS] = { 0 };
short v_sp [SIZEOF_SIGNALS] = { 0 };
short v_adc [SIZEOF_SIGNALS] = { 0 };

recursive_filter_t plant;

float max_antenna_current = 0.0;
float fsampling = 7000.0;
float Vin = 192.0;
float Rsense = 0.055;
float Ao = 13.0;
float La = 0.142;
float Ra = 11.0;


// Module Functions to Test ----------------------------------------------------
// -- set recursive
void Plant_Out_Recursive_Reset (recursive_filter_t * f);
float Plant_Out_Recursive (recursive_filter_t * f, short duty_in);
void Plant_Step_Response (recursive_filter_t *f);
void Plant_Out_PI_Flush (unsigned char which_channel);
int Plant_Out_Recursive_From_Know_Antennas_Reset (recursive_filter_t * f);
void Plant_Out_Recursive_From_Single_Antenna_Reset (recursive_filter_t * f, unsigned char antenna_index);


// -- tests functions
void Test_Single_Channel_Single_Freq (void);
void Test_Generate_All_Channels (void);
void Test_Plant_Step_Response (void);
void Test_All_Signals_All_Antennas (void);
void Test_Single_Signal_Single_Antenna (void);
void Test_Single_Signal_Single_Antenna_Single_Freq (void);



unsigned short Adc12BitsConvertion (float sample);
unsigned short Adc10BitsConvertion (float sample);


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    printf("Start of Analog simulations...\n");
    
    printf("testing signals and simulation...\n");
    Test_Single_Channel_Single_Freq ();
    printf("end generation test\n");    
    
    return 0;
}


// Plant Output parameters vinput = 1 or vinput = 192
// Check ../control_to_output.py
// TransferFunctionDiscrete(
// array([0.62806727]),
// array([ 1.        , -0.94942247]),
// dt: 0.0006666666666666666
// )
#define B_SIZE    1
#define A_SIZE    2
// ch1 plant
// float b_vector_ch1 [B_SIZE] = { 0.62806727 };
// float a_vector_ch1 [A_SIZE] = { 1., -0.94942247 };
float b_vector [B_SIZE] = { 0.6445 };
float a_vector [A_SIZE] = { 1., -0.94809 };
float ins_vector [B_SIZE] = { 0.0 };
float outs_vector [A_SIZE] = { 0.0 };

void Plant_Out_Recursive_Reset (recursive_filter_t * f)
{
    f->b_params = b_vector;
    f->a_params = a_vector;
    f->b_size = B_SIZE;
    f->a_size = A_SIZE;
    f->last_inputs = ins_vector;
    f->last_outputs = outs_vector;    
    
    Recursive_Filter_Float_Reset(f);
}


float Plant_Out_Recursive (recursive_filter_t * f, short duty)
{
    return Recursive_Filter_Float(f, (float) duty * Vin/ 1000.0);
}


extern pi_data_obj_t pi_ch1;
extern pi_data_obj_t pi_ch2;
extern pi_data_obj_t pi_ch3;
extern pi_data_obj_t pi_ch4;
extern const unsigned short * p_table_inphase;
extern const unsigned short * p_table_outphase;
extern unsigned short signal_index;
extern unsigned short phase_accum;
void Test_Single_Channel_Single_Freq (void)
{
    unsigned char antenna_index = 0;
    
    printf("-- setting plant from antenna...\n");
    Plant_Out_Recursive_From_Single_Antenna_Reset (&plant, antenna_index);

    printf("-- setting channel 1 for treatment...\n");
    Signals_Set_Reset_Channel_For_Treatment(0, 1);

    printf("-- setting signals pi params channel 1...\n");
    antenna_st my_antenna;
    TSP_Get_Know_Single_Antenna (&my_antenna, antenna_index);
    Signals_Set_Channel_PI_Parameters (0, &my_antenna);

    printf("-- setup all channels for treatment with global_signal data...\n");
    Signals_Setup_All_Channels();

    for (int i = 0; i < (SIZEOF_SIGNALS - 1); i++)
    {
        timer1_seq_ready = 1;
        Signals_Generate_All_Channels ();

        // save ch1 data
        v_duty[i] = pi_ch1.last_d;
        v_sp[i] = pi_ch1.setpoint;
        v_error[i] = pi_ch1.setpoint - pi_ch1.sample;
        v_adc[i] = pi_ch1.sample;
        
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

    Vector_Short_To_File (file, "duty", v_duty, SIZEOF_SIGNALS);
    Vector_Short_To_File (file, "adc", v_adc, SIZEOF_SIGNALS);
    // Vector_Short_To_File (file, "error1", v_error_ch1, SIZEOF_SIGNALS);    
    Vector_Short_To_File (file, "setpoint", v_sp, SIZEOF_SIGNALS);
    
    printf("\nRun by hand python3 simul_outputs.py\n");
    
}


void Plant_Out_Recursive_From_Single_Antenna_Reset (recursive_filter_t * f, unsigned char antenna_index)
{
    antenna_st my_antenna;
    
    f->b_params = b_vector;
    f->a_params = a_vector;
    f->b_size = B_SIZE;
    f->a_size = A_SIZE;
    f->last_inputs = ins_vector;
    f->last_outputs = outs_vector;

    TSP_Get_Know_Single_Antenna (&my_antenna, antenna_index);

    La = my_antenna.inductance_int + my_antenna.inductance_dec / 100.0;
    La = La / 1000.0;    // convert mHy to Hy
    Ra = my_antenna.resistance_int + my_antenna.resistance_dec / 100.0;
    max_antenna_current = my_antenna.current_limit_int + my_antenna.current_limit_dec / 100.0;
    
    float b0 = Rsense * Ao;
    b0 = b0 / (La * fsampling);

    float a0 = 1.0;
    float a1 = -1.0 + (Ra + Rsense)/(La * fsampling);

    b_vector[0] = b0;
    a_vector[0] = a0;
    a_vector[1] = a1;        

    for (int i = 0; i < B_SIZE; i++)
        ins_vector[i] = 0.0;

    for (int i = 0; i < A_SIZE; i++)
        outs_vector[i] = 0.0;

    printf("b params: %f\n", f->b_params[0]);
    printf("a params: %f %f\n", f->a_params[0], f->a_params[1]);
    
}

extern unsigned char treat_in_ch1;
extern signals_struct_t global_signals;


float v_dummy [SIZEOF_SIGNALS] = { 0 };
// void Test_Plant_Step_Response (void)
// {
//     int length = 200;

//     printf("testing only for ch1!\n");
//     for (int i = 0; i < length; i++)
//     {
//         v_duty_ch1[i] = 950;
//         v_dummy[i] = Plant_Out_Recursive (&plant, v_duty[i]);
//         v_error_ch1[i] = Adc12BitsConvertion(v_dummy[i]);        
//     }

//     ///////////////////////////
//     // Backup Data to a file //
//     ///////////////////////////
//     FILE * file = fopen("data.txt", "w");

//     if (file == NULL)
//     {
//         printf("data file not created!\n");
//         return;
//     }

//     Vector_Short_To_File (file, "duty", v_duty_ch1, length);
//     Vector_Short_To_File (file, "adc_sample", v_error_ch1, length);
//     Vector_Float_To_File (file, "plant_out", v_dummy, length);
//     printf("\nRun by hand python3 simul_outputs.py\n");
    
// }


unsigned short Adc12BitsConvertion (float sample)
{
    if (sample > 0.0001)
    {
        sample = sample / 3.3;
        sample = sample * 4095;
        
        if (sample > 4095)
            sample = 4095;
    }
    else
        sample = 0.0;

    return (unsigned short) sample;
    
}


unsigned short Adc10BitsConvertion (float sample)
{
    if (sample > 0.0001)
    {
        sample = sample / 3.3;
        sample = sample * 1023;
        
        if (sample > 1023)
            sample = 1023;
    }
    else
        sample = 0.0;

    return (unsigned short) sample;
    
}


void Plant_Out_PI_Flush (unsigned char which_channel)
{
    switch (which_channel)
    {
    case CH1:
        pi_ch1.sample = 0;
        pi_ch1.setpoint = 0;
        break;

    case CH2:
        pi_ch2.sample = 0;
        pi_ch2.setpoint = 0;        
        break;
        
    case CH3:
        pi_ch3.sample = 0;
        pi_ch3.setpoint = 0;        
        break;
        
    case CH4:
        pi_ch4.sample = 0;
        pi_ch4.setpoint = 0;
        break;
    }
}


// Mocked Module Functions -----------------------------------------------------
void TIM8_Update_CH3 (unsigned short a)
{
    // printf("HL CH1: %d\n", a);
    if (a)
    {
        float output = 0.0;
        output = Plant_Out_Recursive(&plant, a);
        // printf("input: %d output: %f ", a, output);
        IS_CH1 = Adc12BitsConvertion (output);
        // printf("sample: %d\n", IS_CH1);        
    }
}

void TIM8_Update_CH4 (unsigned short a)
{
    if (a == 0)    // no emition no adc data
    {
        IS_CH1 = 0;
        Plant_Out_Recursive_Reset (&plant);
        Plant_Out_PI_Flush (CH1);
    }
    else if (a < 950)    // regulation on negative
    {
        float output = 0.0;
        output = Plant_Out_Recursive(&plant, -a);
        // output = Plant_Out_Recursive(&plant_ch1, -a << 5);    // simulate a fast discharge     
        IS_CH1 = Adc12BitsConvertion (output);
    }
    else
    {
        // regulation by positive, do nothing in here
    }
}

void TIM8_Update_CH2 (unsigned short a)
{
}

void TIM8_Update_CH1 (unsigned short a)
{
}

void TIM4_Update_CH2 (unsigned short a)
{
}

void TIM4_Update_CH3 (unsigned short a)
{
}

void TIM5_Update_CH1 (unsigned short a)
{
}

void TIM5_Update_CH2 (unsigned short a)
{
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
//--- end of file ---//


