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
short v_duty_ch1 [SIZEOF_SIGNALS] = { 0 };
short v_error_ch1 [SIZEOF_SIGNALS] = { 0 };
short v_sp_ch1 [SIZEOF_SIGNALS] = { 0 };
short v_adc_ch1 [SIZEOF_SIGNALS] = { 0 };
short v_ref_ch1 [SIZEOF_SIGNALS] = { 0 };
short v_signal_ch1 [SIZEOF_SIGNALS] = { 0 };

short v_duty_ch2 [SIZEOF_SIGNALS] = { 0 };
short v_error_ch2 [SIZEOF_SIGNALS] = { 0 };
short v_sp_ch2 [SIZEOF_SIGNALS] = { 0 };
short v_adc_ch2 [SIZEOF_SIGNALS] = { 0 };

short v_duty_ch3 [SIZEOF_SIGNALS] = { 0 };
short v_error_ch3 [SIZEOF_SIGNALS] = { 0 };
short v_sp_ch3 [SIZEOF_SIGNALS] = { 0 };
short v_adc_ch3 [SIZEOF_SIGNALS] = { 0 };

short v_duty_ch4 [SIZEOF_SIGNALS] = { 0 };
short v_error_ch4 [SIZEOF_SIGNALS] = { 0 };
short v_sp_ch4 [SIZEOF_SIGNALS] = { 0 };
short v_adc_ch4 [SIZEOF_SIGNALS] = { 0 };

recursive_filter_t plant_ch1;
recursive_filter_t plant_ch2;
recursive_filter_t plant_ch3;
recursive_filter_t plant_ch4;

float max_antenna_current = 0.0;
float fsampling = 7000.0;


// Module Functions to Test ----------------------------------------------------
// -- set recursive
void Plant_Out_Recursive_Reset (int which_channel, recursive_filter_t * f);
float Plant_Out_Recursive (recursive_filter_t * f, short duty_in);
void Plant_Step_Response (recursive_filter_t *f);
void Plant_Out_PI_Flush (unsigned char which_channel);
int Plant_Out_Recursive_From_Know_Antennas_Reset (recursive_filter_t * f);
void Plant_Out_Recursive_From_Single_Antenna_Reset (recursive_filter_t * f, unsigned char antenna_index);


// -- tests functions
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

    // printf("testing plant step response...\n");
    // printf("recursive reset on plant...\n");
    // Plant_Out_Recursive_Reset (&plant_ch1);
    // Test_Plant_Step_Response ();
    // printf("end step response test\n");
    
    // printf("testing signals and simulation...\n");
    // printf("recursive reset on plant...\n");
    // Plant_Out_Recursive_Reset (CH1, &plant_ch1);
    // Plant_Out_Recursive_Reset (CH2, &plant_ch2);
    // Plant_Out_Recursive_Reset (CH3, &plant_ch3);
    // Plant_Out_Recursive_Reset (CH4, &plant_ch4);    
    // Test_Generate_All_Channels ();
    // printf("end generation test\n");    

    // printf("testing three signals and knowed antennas at same freq on ch1...\n");
    // printf("recursive reset on plant...\n");
    // Plant_Out_Recursive_Reset (CH1, &plant_ch1);
    // Plant_Out_Recursive_Reset (CH2, &plant_ch2);
    // Plant_Out_Recursive_Reset (CH3, &plant_ch3);
    // Plant_Out_Recursive_Reset (CH4, &plant_ch4);
    // Test_All_Signals_All_Antennas ();
    // printf("end generation test\n");    

    // printf("testing specific antenna with specific signal same freq on ch1...\n");
    // printf("recursive reset on plant...\n");
    // Plant_Out_Recursive_Reset (CH1, &plant_ch1);
    // Plant_Out_Recursive_Reset (CH2, &plant_ch2);
    // Plant_Out_Recursive_Reset (CH3, &plant_ch3);
    // Plant_Out_Recursive_Reset (CH4, &plant_ch4);
    // Test_Single_Signal_Single_Antenna ();
    // printf("end generation test\n");    

    // printf("testing specific antenna with specific signal specific freq on ch1...\n");
    // printf("recursive reset on plant...\n");
    // Plant_Out_Recursive_Reset (CH1, &plant_ch1);
    // Plant_Out_Recursive_Reset (CH2, &plant_ch2);
    // Plant_Out_Recursive_Reset (CH3, &plant_ch3);
    // Plant_Out_Recursive_Reset (CH4, &plant_ch4);
    // Test_Single_Signal_Single_Antenna_Single_Freq ();
    // printf("end generation test\n");    

    printf("testing all signals all antennas all frequencies on ch1...\n");
    printf("recursive reset on plant...\n");
    Plant_Out_Recursive_Reset (CH1, &plant_ch1);
    Plant_Out_Recursive_Reset (CH2, &plant_ch2);
    Plant_Out_Recursive_Reset (CH3, &plant_ch3);
    Plant_Out_Recursive_Reset (CH4, &plant_ch4);
    Test_All_Signals_All_Antennas_All_Freq ();
    printf("end generation test\n");    

    // printf("testing specific antenna with specific signal specific freq on ch1...\n");
    // printf("recursive reset on plant...\n");
    // Plant_Out_Recursive_Reset (CH1, &plant_ch1);
    // Plant_Out_Recursive_Reset (CH2, &plant_ch2);
    // Plant_Out_Recursive_Reset (CH3, &plant_ch3);
    // Plant_Out_Recursive_Reset (CH4, &plant_ch4);
    // Test_Single_Signal_Single_Antenna_Single_Freq_OpenLoop ();
    // printf("end generation test\n");    
    
    printf("all simulations done!\n");
    
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
float b_vector_ch1 [B_SIZE] = { 0.6445 };
float a_vector_ch1 [A_SIZE] = { 1., -0.94809 };
float ins_vector_ch1 [B_SIZE] = { 0.0 };
float outs_vector_ch1 [A_SIZE] = { 0.0 };
// ch2 plant
float b_vector_ch2 [B_SIZE] = { 0.62806727 };
float a_vector_ch2 [A_SIZE] = { 1., -0.94942247 };
float ins_vector_ch2 [B_SIZE] = { 0.0 };
float outs_vector_ch2 [A_SIZE] = { 0.0 };
// ch3 plant
float b_vector_ch3 [B_SIZE] = { 0.62806727 };
float a_vector_ch3 [A_SIZE] = { 1., -0.94942247 };
float ins_vector_ch3 [B_SIZE] = { 0.0 };
float outs_vector_ch3 [A_SIZE] = { 0.0 };
// ch4 plant
float b_vector_ch4 [B_SIZE] = { 0.62806727 };
float a_vector_ch4 [A_SIZE] = { 1., -0.94942247 };
float ins_vector_ch4 [B_SIZE] = { 0.0 };
float outs_vector_ch4 [A_SIZE] = { 0.0 };
void Plant_Out_Recursive_Reset (int which_channel, recursive_filter_t * f)
{
    switch (which_channel)
    {
    case CH1:
        f->b_params = b_vector_ch1;
        f->a_params = a_vector_ch1;
        f->b_size = B_SIZE;
        f->a_size = A_SIZE;
        f->last_inputs = ins_vector_ch1;
        f->last_outputs = outs_vector_ch1;    
        break;

    case CH2:
        f->b_params = b_vector_ch2;
        f->a_params = a_vector_ch2;
        f->b_size = B_SIZE;
        f->a_size = A_SIZE;
        f->last_inputs = ins_vector_ch2;
        f->last_outputs = outs_vector_ch2;    
        break;

    case CH3:
        f->b_params = b_vector_ch3;
        f->a_params = a_vector_ch3;
        f->b_size = B_SIZE;
        f->a_size = A_SIZE;
        f->last_inputs = ins_vector_ch3;
        f->last_outputs = outs_vector_ch3;    
        break;

    case CH4:
        f->b_params = b_vector_ch4;
        f->a_params = a_vector_ch4;
        f->b_size = B_SIZE;
        f->a_size = A_SIZE;
        f->last_inputs = ins_vector_ch4;
        f->last_outputs = outs_vector_ch4;    
        break;
    }
    
    Recursive_Filter_Float_Reset(f);
}


float Plant_Out_Recursive (recursive_filter_t * f, short duty)
{
    return Recursive_Filter_Float(f, (float) duty / 1000.0);
}


extern const unsigned short * p_table_inphase;
extern const unsigned short * p_table_outphase;
extern pi_data_obj_t pi_ch1;
extern pi_data_obj_t pi_ch2;
extern pi_data_obj_t pi_ch3;
extern pi_data_obj_t pi_ch4;
extern unsigned short signal_index;
extern unsigned short phase_accum;
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
        v_adc_ch1[i] = pi_ch1.sample;

        // save ch2 data
        v_duty_ch2[i] = pi_ch2.last_d;
        v_sp_ch2[i] = pi_ch2.setpoint;
        v_error_ch2[i] = pi_ch2.setpoint - pi_ch2.sample;
        v_adc_ch2[i] = pi_ch2.sample;

        // save ch3 data
        v_duty_ch3[i] = pi_ch3.last_d;
        v_sp_ch3[i] = pi_ch3.setpoint;
        v_error_ch3[i] = pi_ch3.setpoint - pi_ch3.sample;
        v_adc_ch3[i] = pi_ch3.sample;

        // save ch4 data
        v_duty_ch4[i] = pi_ch4.last_d;
        v_sp_ch4[i] = pi_ch4.setpoint;
        v_error_ch4[i] = pi_ch4.setpoint - pi_ch4.sample;
        v_adc_ch4[i] = pi_ch4.sample;
        
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
    Vector_Short_To_File (file, "adc1", v_adc_ch1, SIZEOF_SIGNALS);
    // Vector_Short_To_File (file, "error1", v_error_ch1, SIZEOF_SIGNALS);    
    Vector_Short_To_File (file, "setpoint1", v_sp_ch1, SIZEOF_SIGNALS);

    // Vector_Short_To_File (file, "duty2", v_duty_ch2, SIZEOF_SIGNALS);
    Vector_Short_To_File (file, "adc2", v_adc_ch2, SIZEOF_SIGNALS);
    // Vector_Short_To_File (file, "error2", v_error_ch2, SIZEOF_SIGNALS);    
    // Vector_Short_To_File (file, "setpoint2", v_sp_ch2, SIZEOF_SIGNALS);
    
    // Vector_Short_To_File (file, "duty3", v_duty_ch3, SIZEOF_SIGNALS);
    Vector_Short_To_File (file, "adc3", v_adc_ch3, SIZEOF_SIGNALS);
    // Vector_Short_To_File (file, "error3", v_error_ch3, SIZEOF_SIGNALS);    
    // Vector_Short_To_File (file, "setpoint3", v_sp_ch3, SIZEOF_SIGNALS);

    // Vector_Short_To_File (file, "duty4", v_duty_ch4, SIZEOF_SIGNALS);
    Vector_Short_To_File (file, "adc4", v_adc_ch4, SIZEOF_SIGNALS);
    // Vector_Short_To_File (file, "error4", v_error_ch4, SIZEOF_SIGNALS);    
    // Vector_Short_To_File (file, "setpoint4", v_sp_ch4, SIZEOF_SIGNALS);
    
    printf("\nRun by hand python3 simul_outputs.py\n");
    
}


extern unsigned char treat_in_ch1;
extern signals_struct_t global_signals;
// = {
//     // general all channels things
//     // .signal = SINUSOIDAL_SIGNAL,
//     .signal = SQUARE_SIGNAL,
//     // .signal = TRIANGULAR_SIGNAL,

//     .freq_int = 23,
//     .freq_dec = 10,

//     .power = 100,

//     // by channel things    
//     .kp_ch1 = 50,
//     .ki_ch1 = 3,
//     .kp_ch2 = 10,
//     .ki_ch2 = 10,
//     // .kp_ch2 = 0,
//     // .ki_ch2 = 0,
//     .kp_ch3 = 5,
//     .ki_ch3 = 2,
//     .kp_ch4 = 1,
//     .ki_ch4 = 30,
    
// };


void Test_All_Signals_All_Antennas (void)
{
    int more_antennas = 0;
    int current_ok = 1;

    for (int j = 0; j < 3; j++)
    {
        switch (j)
        {
        case 0:
            printf("\nemitting with SINUSOIDAL_SIGNAL\n");
            global_signals.signal = SINUSOIDAL_SIGNAL;
            global_signals.kp_ch1 = 2;
            global_signals.ki_ch1 = 30;
            break;
        case 1:
            printf("\nemitting with TRIANGULAR_SIGNAL\n");
            global_signals.signal = TRIANGULAR_SIGNAL;
            global_signals.kp_ch1 = 10;
            global_signals.ki_ch1 = 10;
            break;
        case 2:
            printf("\nemitting with SQUARE_SIGNAL\n");
            global_signals.signal = SQUARE_SIGNAL;
            global_signals.kp_ch1 = 50;
            global_signals.ki_ch1 = 3;
            break;
        }

        TSP_Get_Know_Antennas_Reset ();
        
        do {        
            more_antennas = Plant_Out_Recursive_From_Know_Antennas_Reset (&plant_ch1);
            if (more_antennas)
            {
                // empty signals buffers
                for (int i = 0; i < (SIZEOF_SIGNALS - 1); i++)
                {
                    v_duty_ch1[i] = 0;
                    v_sp_ch1[i] = 0;
                    v_error_ch1[i] = 0;
                    v_adc_ch1[i] = 0;
                }
            
                // current sensed 1A -> 887 pts
                global_signals.max_c_ch1 = (unsigned short) (max_antenna_current * 887);
                printf("max_c_ch1: %d current: %f\n", global_signals.max_c_ch1, max_antenna_current);
                Signals_Setup_All_Channels();
    
                for (int i = 0; i < (SIZEOF_SIGNALS - 1); i++)
                {
                    timer1_seq_ready = 1;
                    Signals_Generate_All_Channels ();

                    // save ch1 data
                    v_duty_ch1[i] = pi_ch1.last_d;
                    v_sp_ch1[i] = pi_ch1.setpoint;
                    v_error_ch1[i] = pi_ch1.setpoint - pi_ch1.sample;
                    v_adc_ch1[i] = pi_ch1.sample;

                    if (treat_in_ch1 == 2)    //some error on channel!!!
                    {
                        printf("treat stopped on channel 1 in sequence: %d\n", i);
                        current_ok = 0;
                        break;
                    }
                }
            }
        
        } while ((more_antennas) && (current_ok));

        printf(" antennas shuffle ended: ");
        if (current_ok)
            PrintOK();
        else
        {
            PrintERR();
            break;
        }
    }

    printf(" signals shuffle ended: ");
    if (current_ok)
        PrintOK();
    else
        PrintERR();

        

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
    Vector_Short_To_File (file, "adc1", v_adc_ch1, SIZEOF_SIGNALS);
    // Vector_Short_To_File (file, "error1", v_error_ch1, SIZEOF_SIGNALS);    
    Vector_Short_To_File (file, "setpoint1", v_sp_ch1, SIZEOF_SIGNALS);

    printf("\nRun by hand python3 simul_outputs.py\n");
    
}


float Vin = 192.0;
float Rsense = 0.055;
float Ao = 13.0;
float La = 0.142;
float Ra = 11.0;
int Plant_Out_Recursive_From_Know_Antennas_Reset (recursive_filter_t * f)
{
    int more_antennas = 0;
    antenna_st my_antenna;
    
    f->b_params = b_vector_ch1;
    f->a_params = a_vector_ch1;
    f->b_size = B_SIZE;
    f->a_size = A_SIZE;
    f->last_inputs = ins_vector_ch1;
    f->last_outputs = outs_vector_ch1;

    more_antennas = TSP_Get_Know_Antennas (&my_antenna);
    if (more_antennas)
    {
        La = my_antenna.inductance_int + my_antenna.inductance_dec / 100.0;
        La = La / 1000.0;    // convert mHy to Hy
        Ra = my_antenna.resistance_int + my_antenna.resistance_dec / 100.0;
        max_antenna_current = my_antenna.current_limit_int + my_antenna.current_limit_dec / 100.0;
    
        float b0 = Vin * Rsense * Ao;
        b0 = b0 / (La * fsampling);

        float a0 = 1.0;
        float a1 = -1.0 + (Ra + Rsense)/(La * fsampling);

        b_vector_ch1[0] = b0;
        a_vector_ch1[0] = a0;
        a_vector_ch1[1] = a1;        

        for (int i = 0; i < B_SIZE; i++)
            ins_vector_ch1[i] = 0.0;

        for (int i = 0; i < A_SIZE; i++)
            outs_vector_ch1[i] = 0.0;
    }

    printf("b params: %f\n", f->b_params[0]);
    printf("a params: %f %f\n", f->a_params[0], f->a_params[1]);

    // reset pi controller
    Plant_Out_PI_Flush (CH1);
    Plant_Out_PI_Flush (CH2);
    Plant_Out_PI_Flush (CH3);
    Plant_Out_PI_Flush (CH4);    

    // reset pwm and adc
    // reset pwm and adc
    TIM8_Update_CH3(0);
    TIM8_Update_CH4(0);

    TIM8_Update_CH2 (0);
    TIM8_Update_CH1 (0);
    
    TIM4_Update_CH2 (0);
    TIM4_Update_CH3 (0);
    
    TIM5_Update_CH1 (0);
    TIM5_Update_CH2 (0);
    
    IS_CH1 = 0;
    IS_CH2 = 0;
    IS_CH3 = 0;
    IS_CH4 = 0;    
    
    return more_antennas;
}


void Plant_Out_Recursive_From_Single_Antenna_Reset (recursive_filter_t * f, unsigned char antenna_index)
{
    antenna_st my_antenna;
    
    f->b_params = b_vector_ch1;
    f->a_params = a_vector_ch1;
    f->b_size = B_SIZE;
    f->a_size = A_SIZE;
    f->last_inputs = ins_vector_ch1;
    f->last_outputs = outs_vector_ch1;

    TSP_Get_Know_Single_Antenna (&my_antenna, antenna_index);

    La = my_antenna.inductance_int + my_antenna.inductance_dec / 100.0;
    La = La / 1000.0;    // convert mHy to Hy
    Ra = my_antenna.resistance_int + my_antenna.resistance_dec / 100.0;
    max_antenna_current = my_antenna.current_limit_int + my_antenna.current_limit_dec / 100.0;
    
    float b0 = Vin * Rsense * Ao;
    b0 = b0 / (La * fsampling);

    float a0 = 1.0;
    float a1 = -1.0 + (Ra + Rsense)/(La * fsampling);

    b_vector_ch1[0] = b0;
    a_vector_ch1[0] = a0;
    a_vector_ch1[1] = a1;        

    for (int i = 0; i < B_SIZE; i++)
        ins_vector_ch1[i] = 0.0;

    for (int i = 0; i < A_SIZE; i++)
        outs_vector_ch1[i] = 0.0;

    float Tau = La * Ra;
    float DCgain = Vin / Ra;        
    printf("antenna La: %f Ra: %f Tau: %f DC gain: %f\n",
           La,
           Ra,
           Tau,
           DCgain);
    
    printf("b params: %f\n", f->b_params[0]);
    printf("a params: %f %f\n", f->a_params[0], f->a_params[1]);

    // reset pi controller
    Plant_Out_PI_Flush (CH1);
    Plant_Out_PI_Flush (CH2);
    Plant_Out_PI_Flush (CH3);
    Plant_Out_PI_Flush (CH4);    

    // reset pwm and adc
    TIM8_Update_CH3(0);
    TIM8_Update_CH4(0);

    TIM8_Update_CH2 (0);
    TIM8_Update_CH1 (0);
    
    TIM4_Update_CH2 (0);
    TIM4_Update_CH3 (0);
    
    TIM5_Update_CH1 (0);
    TIM5_Update_CH2 (0);
    
    IS_CH1 = 0;
    IS_CH2 = 0;
    IS_CH3 = 0;
    IS_CH4 = 0;    
    
}


void Test_Single_Signal_Single_Antenna (void)
{
    unsigned char antenna_index = 0;    //check tests_know_antennas.c

    unsigned char antenna_signal = SINUSOIDAL_SIGNAL;
    // unsigned char antenna_signal = TRIANGULAR_SIGNAL;
    // unsigned char antenna_signal = SQUARE_SIGNAL;

    int current_ok = 1;

    switch (antenna_signal)
    {
    case SINUSOIDAL_SIGNAL:
        printf("\nemitting with SINUSOIDAL_SIGNAL\n");
        global_signals.signal = SINUSOIDAL_SIGNAL;
        global_signals.kp_ch1 = 40;
        global_signals.ki_ch1 = 12;
        break;
    case TRIANGULAR_SIGNAL:
        printf("\nemitting with TRIANGULAR_SIGNAL\n");
        global_signals.signal = TRIANGULAR_SIGNAL;
        global_signals.kp_ch1 = 10;
        global_signals.ki_ch1 = 10;
        break;
    case SQUARE_SIGNAL:
        printf("\nemitting with SQUARE_SIGNAL\n");
        global_signals.signal = SQUARE_SIGNAL;
        global_signals.kp_ch1 = 30;
        global_signals.ki_ch1 = 8;
        break;
    }

    Plant_Out_Recursive_From_Single_Antenna_Reset (&plant_ch1, antenna_index);
    phase_accum = 256;

    // empty signals buffers
    for (int i = 0; i < (SIZEOF_SIGNALS - 1); i++)
    {
        v_duty_ch1[i] = 0;
        v_sp_ch1[i] = 0;
        v_error_ch1[i] = 0;
        v_adc_ch1[i] = 0;
    }
            
    // current sensed 1A -> 887 pts
    global_signals.max_c_ch1 = (unsigned short) (max_antenna_current * 887);
    printf("max_c_ch1: %d current: %f\n", global_signals.max_c_ch1, max_antenna_current);
    Signals_Setup_All_Channels();
    
    for (int i = 0; i < (SIZEOF_SIGNALS - 1); i++)
    {
        timer1_seq_ready = 1;
        Signals_Generate_All_Channels ();

        // save ch1 data
        v_duty_ch1[i] = pi_ch1.last_d;
        v_sp_ch1[i] = pi_ch1.setpoint;
        v_error_ch1[i] = pi_ch1.setpoint - pi_ch1.sample;
        v_adc_ch1[i] = pi_ch1.sample;

        if (treat_in_ch1 == 2)    //some error on channel!!!
        {
            printf("treat stopped on channel 1 in sequence: %d\n", i);
            current_ok = 0;
            break;
        }
    }

    printf(" antenna emission ended: ");
    if (current_ok)
        PrintOK();
    else
        PrintERR();
        

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
    Vector_Short_To_File (file, "adc1", v_adc_ch1, SIZEOF_SIGNALS);
    // Vector_Short_To_File (file, "error1", v_error_ch1, SIZEOF_SIGNALS);    
    Vector_Short_To_File (file, "setpoint1", v_sp_ch1, SIZEOF_SIGNALS);

    printf("\nRun by hand python3 simul_outputs.py\n");
    
}


void Test_Single_Signal_Single_Antenna_Single_Freq (void)
{
    unsigned char antenna_index = 0;    //check tests_know_antennas.c

    unsigned char antenna_signal = TRIANGULAR_SIGNAL;
    // unsigned char antenna_signal = SQUARE_SIGNAL;
    // unsigned char antenna_signal = SINUSOIDAL_SIGNAL;

    // float my_freq = 0.9;
    // float my_freq = 5.0;    
    // float my_freq = 11.5;
    // float my_freq = 23.5;
    // float my_freq = 44.0;
    // float my_freq = 67.0;
    // float my_freq = 60.9;
    // float my_freq = 80.9;
    float my_freq = 100.0;

    int current_ok = 1;

    switch (antenna_signal)
    {
    case SINUSOIDAL_SIGNAL:
        printf("\nemitting with SINUSOIDAL_SIGNAL\n");
        global_signals.signal = SINUSOIDAL_SIGNAL;
        global_signals.kp_ch1 = 80;
        global_signals.ki_ch1 = 20;
        // global_signals.kp_ch1 = 40;
        // global_signals.ki_ch1 = 12;
        break;
    case TRIANGULAR_SIGNAL:
        printf("\nemitting with TRIANGULAR_SIGNAL\n");
        global_signals.signal = TRIANGULAR_SIGNAL;
        global_signals.kp_ch1 = 10;
        global_signals.ki_ch1 = 10;
        break;
    case SQUARE_SIGNAL:
        printf("\nemitting with SQUARE_SIGNAL\n");
        global_signals.signal = SQUARE_SIGNAL;
        global_signals.kp_ch1 = 60;
        global_signals.ki_ch1 = 3;
        break;
    }

    // resolve this with fsampling and 256 signal points
    float phase = my_freq * 256 * 256 / 7000.0;
    phase_accum = (unsigned short) phase;
    printf("freq ask: %f phase calc: %f phase_accum: %d\n",
           my_freq,
           phase,
           phase_accum);        
    
    Plant_Out_Recursive_From_Single_Antenna_Reset (&plant_ch1, antenna_index);

    // check the Tau for parameters correction
    float Tau = La * Ra;
    float DCgain = 192.0 / Ra;
    printf("antenna La: %f Ra: %f Tau: %f DC gain: %f\n",
           La,
           Ra,
           Tau,
           DCgain);

    // override pi parameters
    if ((Tau > 3.0) && (DCgain < 9.0) && (antenna_signal == SINUSOIDAL_SIGNAL))
    {
        printf("override pi params!\n");
        global_signals.kp_ch1 = 1;
        global_signals.ki_ch1 = 100;
    }
    
    // empty signals buffers
    for (int i = 0; i < (SIZEOF_SIGNALS - 1); i++)
    {
        v_duty_ch1[i] = 0;
        v_sp_ch1[i] = 0;
        v_error_ch1[i] = 0;
        v_adc_ch1[i] = 0;
    }
            
    // current sensed 1A -> 887 pts
    global_signals.max_c_ch1 = (unsigned short) (max_antenna_current * 887);
    printf("max_c_ch1: %d current: %f\n", global_signals.max_c_ch1, max_antenna_current);
    Signals_Setup_All_Channels();
    
    for (int i = 0; i < (SIZEOF_SIGNALS - 1); i++)
    {
        timer1_seq_ready = 1;
        Signals_Generate_All_Channels ();

        // save ch1 data
        v_duty_ch1[i] = pi_ch1.last_d;
        v_sp_ch1[i] = pi_ch1.setpoint;
        v_error_ch1[i] = pi_ch1.setpoint - pi_ch1.sample;
        v_adc_ch1[i] = pi_ch1.sample;

        if (treat_in_ch1 == 2)    //some error on channel!!!
        {
            printf("treat stopped on channel 1 in sequence: %d\n", i);
            current_ok = 0;
            break;
        }
    }

    printf(" antenna emission ended: ");
    if (current_ok)
        PrintOK();
    else
        PrintERR();
        

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
    Vector_Short_To_File (file, "adc1", v_adc_ch1, SIZEOF_SIGNALS);
    // Vector_Short_To_File (file, "error1", v_error_ch1, SIZEOF_SIGNALS);    
    Vector_Short_To_File (file, "setpoint1", v_sp_ch1, SIZEOF_SIGNALS);

    printf("\nRun by hand python3 simul_outputs.py\n");
    
}


extern short p_inphase_ch1 [];
extern const unsigned short * p_table_inphase;
void Test_Single_Signal_Single_Antenna_Single_Freq_OpenLoop (void)
{
    unsigned char antenna_index = 0;    //check tests_know_antennas.c

    unsigned char antenna_signal = TRIANGULAR_SIGNAL;
    // unsigned char antenna_signal = SQUARE_SIGNAL;
    // unsigned char antenna_signal = SINUSOIDAL_SIGNAL;

    // float my_freq = 0.9;
    // float my_freq = 5.0;    
    float my_freq = 11.5;
    // float my_freq = 23.5;
    // float my_freq = 44.0;
    // float my_freq = 67.0;
    // float my_freq = 60.9;
    // float my_freq = 80.9;
    // float my_freq = 100.0;

    int current_ok = 1;

    switch (antenna_signal)
    {
    case SINUSOIDAL_SIGNAL:
        printf("\nemitting with SINUSOIDAL_SIGNAL\n");
        global_signals.signal = SINUSOIDAL_SIGNAL;
        break;
    case TRIANGULAR_SIGNAL:
        printf("\nemitting with TRIANGULAR_SIGNAL\n");
        global_signals.signal = TRIANGULAR_SIGNAL;
        break;
    case SQUARE_SIGNAL:
        printf("\nemitting with SQUARE_SIGNAL\n");
        global_signals.signal = SQUARE_SIGNAL;
        break;
    }

    // resolve this with fsampling and 256 signal points
    float phase = my_freq * 256 * 256 / 7000.0;
    phase_accum = (unsigned short) phase;
    printf("freq ask: %f phase calc: %f phase_accum: %d\n",
           my_freq,
           phase,
           phase_accum);        
    
    Plant_Out_Recursive_From_Single_Antenna_Reset (&plant_ch1, antenna_index);

    // check the Tau for parameters correction
    float Tau = La * Ra;
    float DCgain = 192.0 / Ra;
    printf("antenna La: %f Ra: %f Tau: %f DC gain: %f\n",
           La,
           Ra,
           Tau,
           DCgain);
    
    // empty signals buffers
    for (int i = 0; i < (SIZEOF_SIGNALS - 1); i++)
    {
        v_ref_ch1[i] = 0;
        v_signal_ch1[i] = 0;
        v_adc_ch1[i] = 0;
    }
            
    // current sensed 1A -> 887 pts
    global_signals.max_c_ch1 = (unsigned short) (max_antenna_current * 887);
    printf("max_c_ch1: %d current: %f\n", global_signals.max_c_ch1, max_antenna_current);
    Signals_Setup_All_Channels();

    // printf("b params: %f\n", f->b_params[0]);
    // printf("a params: %f %f\n", f->a_params[0], f->a_params[1]);
    // setup de reference signal
    // k = 0
    float a1_pos = -a_vector_ch1[1];
    // float calc = 16. * a1_pos / b_vector_ch1[0];
    float calc = 0.4 * a1_pos / b_vector_ch1[0];    
    
    *p_inphase_ch1 = (short) calc;

    // k = 1..n
    for (int i = 1; i < 127; i++)
    {
        // calc = 8. / b_vector_ch1[0];
        calc = 0.2 / b_vector_ch1[0];        
        calc = calc * (i + a1_pos * (3*i + 1));
        *(p_inphase_ch1 + i) = (short) calc;
    }
    
    for (int i = 0; i < (SIZEOF_SIGNALS - 1); i++)
    {
        timer1_seq_ready = 1;
        Signals_Generate_Single_Channel_OpenLoop ();

        // current setpoint
        unsigned int csp = *(p_table_inphase + i) * global_signals.max_c_ch1;
        csp >>= 10;    //compensate for max current 1024
        
        // save ch1 data
        v_ref_ch1[i] = (short) csp;
        v_signal_ch1[i] = *(p_inphase_ch1 + i);
        v_adc_ch1[i] = IS_CH1;

        if (treat_in_ch1 == 2)    //some error on channel!!!
        {
            printf("treat stopped on channel 1 in sequence: %d\n", i);
            current_ok = 0;
            break;
        }
    }

    printf(" antenna emission ended: ");
    if (current_ok)
        PrintOK();
    else
        PrintERR();
        

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
    Vector_Short_To_File (file, "adc1", v_adc_ch1, SIZEOF_SIGNALS);
    Vector_Short_To_File (file, "signal_x", v_signal_ch1, SIZEOF_SIGNALS);    
    Vector_Short_To_File (file, "setpoint1", v_ref_ch1, SIZEOF_SIGNALS);

    printf("\nRun by hand python3 simul_outputs.py\n");
    
}


void Test_All_Signals_All_Antennas_All_Freq (void)
{
    // unsigned char antenna_index = 10;    //check tests_know_antennas.c
    // float my_freq = -4.1;
    int antenna_signal = SQUARE_SIGNAL;

    unsigned char antenna_index = 8;    //check tests_know_antennas.c
    float my_freq = 0.9;
    
    int shuffle_not_ended = 1;
    int current_ok = 1;

    do {
        // change antennas
        if (antenna_index < 9)
            antenna_index++;
        else
        {
            antenna_index = 0;

            // change frequency
            if ((int) my_freq < 100)
                my_freq += 20.0;
            else
            {
                my_freq = 0.9;

                // change signal
                if (antenna_signal < SINUSOIDAL_SIGNAL)
                    antenna_signal++;
                else
                {
                    shuffle_not_ended = 0;
                }
            }
        }

        switch (antenna_signal)
        {
        case SINUSOIDAL_SIGNAL:
            printf("\nemitting with SINUSOIDAL_SIGNAL\n");
            global_signals.signal = SINUSOIDAL_SIGNAL;
            break;
        case TRIANGULAR_SIGNAL:
            printf("\nemitting with TRIANGULAR_SIGNAL\n");
            global_signals.signal = TRIANGULAR_SIGNAL;
            break;
        case SQUARE_SIGNAL:
            printf("\nemitting with SQUARE_SIGNAL\n");
            global_signals.signal = SQUARE_SIGNAL;
            break;
        }

        // resolve this with fsampling and 256 signal points
        float phase = my_freq * 256 * 256 / 7000.0;
        phase_accum = (unsigned short) phase;
        printf("freq ask: %f phase calc: %f phase_accum: %d\n",
               my_freq,
               phase,
               phase_accum);        
    
        // Plant_Out_Recursive_From_Single_Antenna_Reset (&plant_ch1, antenna_index);
        Set_Recursive_From_Single_Antenna (&plant_ch1, antenna_index);

        // empty signals buffers
        for (int i = 0; i < (SIZEOF_SIGNALS - 1); i++)
        {
            v_duty_ch1[i] = 0;
            v_sp_ch1[i] = 0;
            v_error_ch1[i] = 0;
            v_adc_ch1[i] = 0;
        }
            
        // current sensed 1A -> 887 pts
        global_signals.max_c_ch1 = (unsigned short) (max_antenna_current * 887);
        printf("max_c_ch1: %d current: %f\n", global_signals.max_c_ch1, max_antenna_current);
        Signals_Setup_All_Channels();
    
        for (int i = 0; i < (SIZEOF_SIGNALS - 1); i++)
        {
            timer1_seq_ready = 1;
            Signals_Generate_All_Channels ();

            // save ch1 data
            v_duty_ch1[i] = pi_ch1.last_d;
            v_sp_ch1[i] = pi_ch1.setpoint;
            v_error_ch1[i] = pi_ch1.setpoint - pi_ch1.sample;
            v_adc_ch1[i] = pi_ch1.sample;

            if (treat_in_ch1 == 2)    //some error on channel!!!
            {
                printf("treat stopped on channel 1 in sequence: %d\n", i);
                current_ok = 0;
                break;
            }
        }

        printf(" antenna emission ended: ");
        if (current_ok)
            PrintOK();
        else
        {
            PrintERR();
            break;
        }

    } while (shuffle_not_ended);

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
    Vector_Short_To_File (file, "adc1", v_adc_ch1, SIZEOF_SIGNALS);
    // Vector_Short_To_File (file, "error1", v_error_ch1, SIZEOF_SIGNALS);    
    Vector_Short_To_File (file, "setpoint1", v_sp_ch1, SIZEOF_SIGNALS);

    printf("\nRun by hand python3 simul_outputs.py\n");
    
}


float v_dummy [SIZEOF_SIGNALS] = { 0 };
void Test_Plant_Step_Response (void)
{
    int length = 200;

    printf("testing only for ch1!\n");
    for (int i = 0; i < length; i++)
    {
        v_duty_ch1[i] = 950;
        v_dummy[i] = Plant_Out_Recursive (&plant_ch1, v_duty_ch1[i]);
        v_error_ch1[i] = Adc12BitsConvertion(v_dummy[i]);        
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

    Vector_Short_To_File (file, "duty", v_duty_ch1, length);
    Vector_Short_To_File (file, "adc_sample", v_error_ch1, length);
    Vector_Float_To_File (file, "plant_out", v_dummy, length);
    printf("\nRun by hand python3 simul_outputs.py\n");
    
}


void Set_Recursive_From_Single_Antenna (recursive_filter_t * f, unsigned char antenna_index)
{
    antenna_st my_antenna;
    
    f->b_params = b_vector_ch1;
    f->a_params = a_vector_ch1;
    f->b_size = B_SIZE;
    f->a_size = A_SIZE;
    f->last_inputs = ins_vector_ch1;
    f->last_outputs = outs_vector_ch1;

    TSP_Get_Know_Single_Antenna (&my_antenna, antenna_index);

    La = my_antenna.inductance_int + my_antenna.inductance_dec / 100.0;
    La = La / 1000.0;    // convert mHy to Hy
    Ra = my_antenna.resistance_int + my_antenna.resistance_dec / 100.0;
    max_antenna_current = my_antenna.current_limit_int + my_antenna.current_limit_dec / 100.0;
    
    float b0 = Rsense * Ao;
    b0 = b0 / (La * fsampling);

    float a0 = 1.0;
    float a1 = -1.0 + (Ra + Rsense)/(La * fsampling);

    b_vector_ch1[0] = b0;
    a_vector_ch1[0] = a0;
    a_vector_ch1[1] = a1;        

    for (int i = 0; i < B_SIZE; i++)
        ins_vector_ch1[i] = 0.0;

    for (int i = 0; i < A_SIZE; i++)
        outs_vector_ch1[i] = 0.0;

    printf("b params: %f\n", f->b_params[0]);
    printf("a params: %f %f\n", f->a_params[0], f->a_params[1]);

    // check the Tau for parameters correction
    float Tau = La / Ra;
    float Tau_s = Tau * fsampling;
    int Tau_samples = (int) Tau_s;
    printf("antenna La: %f Ra: %f Tau: %f Tau_samples: %d\n",
           La,
           Ra,
           Tau,
           Tau_samples);

    printf("low pass L-R wc: %fr/s fc: %fHz\n",
           Ra/La,
           Ra/(La * 6.28));

    // pre calculation for pid
    printf(" setting pid controller\n");
    // float zero_to_pole = 8;
    // float b1_pre = 1. - (1. - a1_pos) * zero_to_pole;
    float a1_pos = -a1;
    float gain = b0 /(1. - a1_pos);
    float gain_five_perc = 19. /(gain * Vin);
    printf(" filter gain: %f prop gain: %f gain 5 perc: %f\n", gain, gain * Vin, gain_five_perc);

    // calc ki from kp and zero eq to pole freq
    float zero_eq_pole = Ra/(La * 6.28);
    float ki_gain = zero_eq_pole * gain_five_perc * 6.28 * 128. * 10. / fsampling;
    
    global_signals.ki_ch1 = (short) ki_gain;    //ki = kp * 2 * np.pi * 20 / fs    
    global_signals.kp_ch1 = (short) (gain_five_perc * 128);

    printf(" pid values ki: %d kp: %d\n",
           global_signals.ki_ch1,
           global_signals.kp_ch1);

    printf(" zero from ki_eff: %f to kp: %fHz\n",
           (float) global_signals.ki_ch1 * fsampling / 128.,
           (float) global_signals.ki_ch1 * fsampling / (128. * 6.28 * (float) global_signals.kp_ch1));
    
}


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
        output = Plant_Out_Recursive(&plant_ch1, a);
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
        Plant_Out_Recursive_Reset (CH1, &plant_ch1);
        Plant_Out_PI_Flush (CH1);
    }
    else if (a < 950)    // regulation on negative
    {
        float output = 0.0;
        output = Plant_Out_Recursive(&plant_ch1, -a);
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
    if (a)
    {
        float output = 0.0;
        output = Plant_Out_Recursive(&plant_ch2, a);
        IS_CH2 = Adc12BitsConvertion (output);
    }
    // printf("HL CH2: %d\n", a);
}

void TIM8_Update_CH1 (unsigned short a)
{
    if (a == 0)    // no emition no adc data
    {
        IS_CH2 = 0;
        Plant_Out_Recursive_Reset (CH2, &plant_ch2);
        Plant_Out_PI_Flush (CH2);
    }
    else if (a < 950)    // regulation on negative
    {
        float output = 0.0;
        // output = Plant_Out_Recursive(-a);
        output = Plant_Out_Recursive(&plant_ch2, -a << 5);    // simulate a fast discharge     
        IS_CH2 = Adc12BitsConvertion (output);
    }
    else
    {
        // regulation by positive, do nothing in here
    }
    // printf("LR CH2: %d\n", a);
}

void TIM4_Update_CH2 (unsigned short a)
{
    if (a)
    {
        float output = 0.0;
        output = Plant_Out_Recursive(&plant_ch3, a);
        IS_CH3 = Adc12BitsConvertion (output);
    }
    // printf("HL CH3: %d\n", a);
}

void TIM4_Update_CH3 (unsigned short a)
{
    if (a == 0)    // no emition no adc data
    {
        IS_CH3 = 0;
        Plant_Out_Recursive_Reset (CH3, &plant_ch3);
        Plant_Out_PI_Flush (CH3);        
    }
    else if (a < 950)    // regulation on negative
    {
        float output = 0.0;
        // output = Plant_Out_Recursive(-a);
        output = Plant_Out_Recursive(&plant_ch3, -a << 5);    // simulate a fast discharge     
        IS_CH3 = Adc12BitsConvertion (output);
    }
    else
    {
        // regulation by positive, do nothing in here
    }
    // printf("LR CH3: %d\n", a);
}

void TIM5_Update_CH1 (unsigned short a)
{
    if (a)
    {
        float output = 0.0;
        output = Plant_Out_Recursive(&plant_ch4, a);
        IS_CH4 = Adc12BitsConvertion (output);
    }
    // printf("HL CH4: %d\n", a);
}

void TIM5_Update_CH2 (unsigned short a)
{
    if (a == 0)    // no emition no adc data
    {
        IS_CH4 = 0;
        Plant_Out_Recursive_Reset (CH4, &plant_ch4);
        Plant_Out_PI_Flush (CH4);
    }
    else if (a < 950)    // regulation on negative
    {
        float output = 0.0;
        // output = Plant_Out_Recursive(-a);
        output = Plant_Out_Recursive(&plant_ch4, -a << 5);    // simulate a fast discharge     
        IS_CH4 = Adc12BitsConvertion (output);
    }
    else
    {
        // regulation by positive, do nothing in here
    }    
    // printf("LR CH4: %d\n", a);
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

void ErrorSetStatus (unsigned char error, unsigned char channel)
{
    error <<= 4;
    error += channel + 1;
    printf("error: 0x%02x\n", error);
}
//--- end of file ---//


