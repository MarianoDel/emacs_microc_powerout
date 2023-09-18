//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS_SIGNALS.C #######################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "tests_know_antennas.h"

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


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
///////////////////////////////
// Plant with Antenna Params //
///////////////////////////////
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
recursive_filter_t plant;


////////////////////////////////////////////////
// Pre filter for Antenna Compensation Params //
////////////////////////////////////////////////
#define B_SIZE_PRE    1
#define A_SIZE_PRE    2

float b_vector_pre [B_SIZE_PRE] = { 0.6445 };
float a_vector_pre [A_SIZE_PRE] = { 1., -0.94809 };
float ins_vector_pre [B_SIZE_PRE] = { 0.0 };
float outs_vector_pre [A_SIZE_PRE] = { 0.0 };
recursive_filter_t pre_filter;


short v_signal [SIZEOF_SIGNALS] = { 0 };
short v_pre [SIZEOF_SIGNALS] = { 0 };
short v_pre_f [SIZEOF_SIGNALS] = { 0 };
short v_input [SIZEOF_SIGNALS] = { 0 };
short v_output [SIZEOF_SIGNALS] = { 0 };

float max_antenna_current = 0.0;
float fsampling = 7000.0;

#define CURRENT_TO_VOLTAGE_PTS    887    // current sensed 1A -> 887 pts

float Vin = 192.0;
float Rsense = 0.055;
float Ao = 13.0;
float La = 0.142;
float Ra = 11.0;

// Module Functions to Test ----------------------------------------------------
// -- set recursive
void Set_Recursive_From_Single_Antenna (recursive_filter_t * f, unsigned char antenna_index);


// -- tests functions
void Plant_Step_Response (void);
void Plant_From_Input_Response (void);
// void Test_All_Signals_All_Antennas (void);
// void Test_Single_Signal_Single_Antenna (void);
// void Test_Single_Signal_Single_Antenna_Single_Freq (void);



unsigned short Adc12BitsConvertion (float sample);
unsigned short Adc10BitsConvertion (float sample);


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    printf("Start of Analog simulations...\n");
    // Plant_Step_Response ();
    Plant_From_Input_Response ();
    
    printf("all simulations done!\n");
    
    return 0;
}


void Plant_Step_Response (void)
{
    float signal_out = 0.0;
    short duty = 0;
    unsigned char antenna_index = 0;
    
    printf("step response\n");    

    printf(" setting plant from know antenna index: %d...\n", antenna_index);
    Set_Recursive_From_Single_Antenna (&plant, antenna_index);

    printf(" plant reset\n");
    Recursive_Filter_Float_Reset(&plant);

    printf(" setting input signal\n");
    for (int i = 0; i < SIZEOF_SIGNALS; i++)
    {
        //12.27V on 12.27 ohms = 1A
        //duty@12.27V => 12.27/192 * 1000 
        v_input[i] = 64;    // 1A on 12.27 ohms -> 887 pts ADC 12bits
    }

    printf(" apply signal\n");
    for (int i = 0; i < SIZEOF_SIGNALS; i++)
    {
        duty = v_input[i];
        signal_out = Recursive_Filter_Float (&plant, Vin * (float) duty / 1000.0);
        v_output[i] = Adc12BitsConvertion(signal_out);        
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

    Vector_Short_To_File (file, "input", v_input, SIZEOF_SIGNALS);
    Vector_Short_To_File (file, "output", v_output, SIZEOF_SIGNALS);    

    printf("\nRun by hand python3 simul_outputs.py\n");
    
}


void Plant_From_Input_Response (void)
{
    float signal_out = 0.0;
    short duty = 0;
    unsigned char antenna_index = 0;
    // unsigned char antenna_index = 6;
    // unsigned char antenna_index = 8;    
    
    printf("from input response\n");    

    printf(" setting plant from know antenna index: %d...\n", antenna_index);
    Set_Recursive_From_Single_Antenna (&plant, antenna_index);
    float a1_pos = -a_vector_ch1[1];
    printf(" a1_pos: %f\n", a1_pos);

    printf(" plant reset\n");
    Recursive_Filter_Float_Reset(&plant);

    printf(" setting input signal\n");
    // all signals to 1000pts
    // step
    for (int i = 0; i < SIZEOF_SIGNALS / 2; i++)
    {
        v_signal[i] = 1000;
    }
    for (int i = SIZEOF_SIGNALS / 2; i < SIZEOF_SIGNALS; i++)
    {
        v_signal[i] = 0;
    }
    // v_signal[0] = 200;
    // v_signal[1] = 400;
    // v_signal[2] = 600;
    // v_signal[3] = 800;    
    v_signal[127] = 500;
    // end of step function
    
    // triangular
    // for (int i = 0; i < SIZEOF_SIGNALS / 2; i++)
    // {
    //     v_signal[i] =  2 * i * 1000 / SIZEOF_SIGNALS ;
    // }
    // for (int i = SIZEOF_SIGNALS / 2; i < SIZEOF_SIGNALS; i++)
    // {
    //     v_signal[i] = 0;
    // }
    // end of triangular

    // sinusoidal
    // for (int i = 0; i < SIZEOF_SIGNALS / 2; i++)
    // {
    //     v_signal[i] = 1000 * sin(6.28 * (float) i / (float)SIZEOF_SIGNALS);
    // }
    // for (int i = SIZEOF_SIGNALS / 2; i < SIZEOF_SIGNALS; i++)
    // {
    //     v_signal[i] = 0;
    // }
    // end of sinusoidal

    // adjust imputs values to current selected
    float multi = max_antenna_current * 887. / 1000.;
    // ten percent
    // float multi = max_antenna_current / 10 * 887. / 1000.;    
    // float multi = 887. / 1000.;    
    printf("  selected current: %f Ra: %f multiplier: %f\n", max_antenna_current, Ra, multi);
    for (int i = 0; i < SIZEOF_SIGNALS; i++)
    {
        v_input[i] = v_signal[i] * multi;
    }

    // pre filter for square
    float zero_to_pole = 8;
    float b1_pre = 1. - (1. - a1_pos) * zero_to_pole;    
    float gain_pre = (1. - b1_pre) / (1. - a1_pos);
    printf("  pre filter gain: %f zero: %f pole: %f\n", gain_pre, a1_pos, b1_pre);
    float v_pre_ff [SIZEOF_SIGNALS] = { 0 };
    for (int i = 0; i < SIZEOF_SIGNALS; i++)
    {
        if (i)
        {
            // v_pre_ff[i] = gain_pre * v_input[i] - gain_pre * 0.98 * v_input[i - 1];
            v_pre_ff[i] = b1_pre * v_pre_ff[i - 1] + gain_pre * v_input[i] - gain_pre * a1_pos * v_input[i - 1];
            // printf("v_pre_ff: %f v_input: %d v_input_1: %d\n", v_pre_ff[i], v_input[i], v_input[i-1]);
        }
        else
        {
            v_pre_ff[i] = gain_pre * v_input[i];
            // printf("v_pre_ff: %f v_input: %d\n", v_pre_ff[i], v_input[i]);
        }
    }
    // end of pre filter square
    
    // pre filter for triangular and sinusoidal
    // float gain_pre = 1./(1. - a1_pos);    
    // printf("  pre filter gain: %f\n", gain_pre);
    // float v_pre_ff [SIZEOF_SIGNALS] = { 0 };
    // for (int i = 0; i < SIZEOF_SIGNALS; i++)
    // {
    //     if (i)
    //     {
    //         // v_pre_ff[i] = gain_pre * v_input[i] - gain_pre * 0.98 * v_input[i - 1];
    //         v_pre_ff[i] = gain_pre * v_input[i] - gain_pre * a1_pos * v_input[i - 1];            
    //         // printf("v_pre_ff: %f v_input: %d v_input_1: %d\n", v_pre_ff[i], v_input[i], v_input[i-1]);
    //     }
    //     else
    //     {
    //         v_pre_ff[i] = gain_pre * v_input[i];
    //         // printf("v_pre_ff: %f v_input: %d\n", v_pre_ff[i], v_input[i]);
    //     }
    // }
    // end of pre filter for triangular and sinusoidal

    // direct from input without filter
    // multi = (Ra * 1000.) / (Vin * 887.);
    // printf("  duty for 1 amps: %f\n", multi);
    // for (int i = 0; i < SIZEOF_SIGNALS; i++)
    //     v_pre[i] = v_input[i] * multi;

    // adjust prefilter output for duty
    int accum = 0;
    multi = (Ra * 1000.) / (Vin * 887.);
    printf("  duty for 1 amps: %f\n", multi);
    for (int i = 0; i < SIZEOF_SIGNALS; i++)
    {
        v_pre[i] = v_pre_ff[i] * multi;

        // pre filter with roof and floor
        // if (v_pre[i] >= 0)
        // {
        //     int pre = v_pre[i];
        //     int a = pre + accum;
        //     if (a > 950)
        //     {
        //         if (pre > 950)    // still adding to accum
        //         {
        //             accum += pre - 950;
        //             v_pre[i] = 950;
        //         }
        //         else    // start to drop the accum
        //         {
        //             int diff = 950 - pre;
        //             if (accum > diff)
        //             {
        //                 v_pre[i] = 950;
        //                 accum -= diff;
        //             }
        //             else
        //             {
        //                 v_pre[i] = pre + accum;
        //                 accum = 0;
        //             }
        //         }
        //     }
        //     else
        //     {
        //         if (accum > 0)
        //         {
        //         }
        //         else
        //         {
        //             v_pre[i] = v_pre[i] + accum;
        //             accum = 0;
        //         }
        //     }
        // }
        // else if (v_pre[i] < -950)
        // {
        //     accum -= 950 - v_pre[i];
        //     v_pre[i] = -950;
        // }
        // else
        //     accum = 0;        
    }


    
    // float pre filter
    // float v_pre_f [SIZEOF_SIGNALS];
    // for (int i = 0; i < SIZEOF_SIGNALS; i++)
    // {
    //     if (i)            
    //         v_pre_f[i] = gain_pre * v_input[i] - gain_pre * 0.98 * v_input[i - 1];
    //     else
    //         v_pre_f[i] = gain_pre * v_input[i];
        
    // }    
    // for (int i = 0; i < SIZEOF_SIGNALS; i++)
    //     v_pre_from_f[i] = (short) v_pre_f[i];
    
    printf(" apply signal\n");
    for (int i = 0; i < SIZEOF_SIGNALS; i++)
    {
        duty = v_pre[i];
        signal_out = Recursive_Filter_Float (&plant, Vin * (float) duty / 1000.0);
        v_output[i] = Adc12BitsConvertion(signal_out);        
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

    // Vector_Short_To_File (file, "signal", v_signal, SIZEOF_SIGNALS);    
    Vector_Short_To_File (file, "input", v_input, SIZEOF_SIGNALS);
    Vector_Short_To_File (file, "pre_scale", v_pre_f, SIZEOF_SIGNALS);
    Vector_Short_To_File (file, "prefilter", v_pre, SIZEOF_SIGNALS);
    Vector_Short_To_File (file, "output", v_output, SIZEOF_SIGNALS);    

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
    
}


// void Test_Single_Signal_Single_Antenna (void)
// {
//     unsigned char antenna_index = 0;    //check tests_know_antennas.c

//     unsigned char antenna_signal = SINUSOIDAL_SIGNAL;
//     // unsigned char antenna_signal = TRIANGULAR_SIGNAL;
//     // unsigned char antenna_signal = SQUARE_SIGNAL;

//     int current_ok = 1;

//     switch (antenna_signal)
//     {
//     case SINUSOIDAL_SIGNAL:
//         printf("\nemitting with SINUSOIDAL_SIGNAL\n");
//         global_signals.signal = SINUSOIDAL_SIGNAL;
//         global_signals.kp_ch1 = 40;
//         global_signals.ki_ch1 = 12;
//         break;
//     case TRIANGULAR_SIGNAL:
//         printf("\nemitting with TRIANGULAR_SIGNAL\n");
//         global_signals.signal = TRIANGULAR_SIGNAL;
//         global_signals.kp_ch1 = 10;
//         global_signals.ki_ch1 = 10;
//         break;
//     case SQUARE_SIGNAL:
//         printf("\nemitting with SQUARE_SIGNAL\n");
//         global_signals.signal = SQUARE_SIGNAL;
//         global_signals.kp_ch1 = 30;
//         global_signals.ki_ch1 = 8;
//         break;
//     }

//     Plant_Out_Recursive_From_Single_Antenna_Reset (&plant_ch1, antenna_index);
//     phase_accum = 256;

//     // empty signals buffers
//     for (int i = 0; i < (SIZEOF_SIGNALS - 1); i++)
//     {
//         v_duty_ch1[i] = 0;
//         v_sp_ch1[i] = 0;
//         v_error_ch1[i] = 0;
//         v_adc_ch1[i] = 0;
//     }
            
//     // current sensed 1A -> 887 pts
//     global_signals.max_c_ch1 = (unsigned short) (max_antenna_current * 887);
//     printf("max_c_ch1: %d current: %f\n", global_signals.max_c_ch1, max_antenna_current);
//     Signals_Setup_All_Channels();
    
//     for (int i = 0; i < (SIZEOF_SIGNALS - 1); i++)
//     {
//         timer1_seq_ready = 1;
//         Signals_Generate_All_Channels ();

//         // save ch1 data
//         v_duty_ch1[i] = pi_ch1.last_d;
//         v_sp_ch1[i] = pi_ch1.setpoint;
//         v_error_ch1[i] = pi_ch1.setpoint - pi_ch1.sample;
//         v_adc_ch1[i] = pi_ch1.sample;

//         if (treat_in_ch1 == 2)    //some error on channel!!!
//         {
//             printf("treat stopped on channel 1 in sequence: %d\n", i);
//             current_ok = 0;
//             break;
//         }
//     }

//     printf(" antenna emission ended: ");
//     if (current_ok)
//         PrintOK();
//     else
//         PrintERR();
        

//     ///////////////////////////
//     // Backup Data to a file //
//     ///////////////////////////
//     FILE * file = fopen("data.txt", "w");

//     if (file == NULL)
//     {
//         printf("data file not created!\n");
//         return;
//     }

//     // Vector_Short_To_File (file, "duty1", v_duty_ch1, SIZEOF_SIGNALS);
//     Vector_Short_To_File (file, "adc1", v_adc_ch1, SIZEOF_SIGNALS);
//     // Vector_Short_To_File (file, "error1", v_error_ch1, SIZEOF_SIGNALS);    
//     Vector_Short_To_File (file, "setpoint1", v_sp_ch1, SIZEOF_SIGNALS);

//     printf("\nRun by hand python3 simul_outputs.py\n");
    
// }


// void Test_Single_Signal_Single_Antenna_Single_Freq (void)
// {
//     unsigned char antenna_index = 0;    //check tests_know_antennas.c

//     unsigned char antenna_signal = TRIANGULAR_SIGNAL;
//     // unsigned char antenna_signal = SQUARE_SIGNAL;
//     // unsigned char antenna_signal = SINUSOIDAL_SIGNAL;

//     // float my_freq = 0.9;
//     // float my_freq = 5.0;    
//     // float my_freq = 11.5;
//     // float my_freq = 23.5;
//     // float my_freq = 44.0;
//     // float my_freq = 67.0;
//     // float my_freq = 60.9;
//     // float my_freq = 80.9;
//     float my_freq = 100.0;

//     int current_ok = 1;

//     switch (antenna_signal)
//     {
//     case SINUSOIDAL_SIGNAL:
//         printf("\nemitting with SINUSOIDAL_SIGNAL\n");
//         global_signals.signal = SINUSOIDAL_SIGNAL;
//         global_signals.kp_ch1 = 80;
//         global_signals.ki_ch1 = 20;
//         // global_signals.kp_ch1 = 40;
//         // global_signals.ki_ch1 = 12;
//         break;
//     case TRIANGULAR_SIGNAL:
//         printf("\nemitting with TRIANGULAR_SIGNAL\n");
//         global_signals.signal = TRIANGULAR_SIGNAL;
//         global_signals.kp_ch1 = 10;
//         global_signals.ki_ch1 = 10;
//         break;
//     case SQUARE_SIGNAL:
//         printf("\nemitting with SQUARE_SIGNAL\n");
//         global_signals.signal = SQUARE_SIGNAL;
//         global_signals.kp_ch1 = 60;
//         global_signals.ki_ch1 = 3;
//         break;
//     }

//     // resolve this with fsampling and 256 signal points
//     float phase = my_freq * 256 * 256 / 7000.0;
//     phase_accum = (unsigned short) phase;
//     printf("freq ask: %f phase calc: %f phase_accum: %d\n",
//            my_freq,
//            phase,
//            phase_accum);        
    
//     Plant_Out_Recursive_From_Single_Antenna_Reset (&plant_ch1, antenna_index);

//     // check the Tau for parameters correction
//     float Tau = La * Ra;
//     float DCgain = 192.0 / Ra;
//     printf("antenna La: %f Ra: %f Tau: %f DC gain: %f\n",
//            La,
//            Ra,
//            Tau,
//            DCgain);

//     // override pi parameters
//     if ((Tau > 3.0) && (DCgain < 9.0) && (antenna_signal == SINUSOIDAL_SIGNAL))
//     {
//         printf("override pi params!\n");
//         global_signals.kp_ch1 = 1;
//         global_signals.ki_ch1 = 100;
//     }
    
//     // empty signals buffers
//     for (int i = 0; i < (SIZEOF_SIGNALS - 1); i++)
//     {
//         v_duty_ch1[i] = 0;
//         v_sp_ch1[i] = 0;
//         v_error_ch1[i] = 0;
//         v_adc_ch1[i] = 0;
//     }
            
//     // current sensed 1A -> 887 pts
//     global_signals.max_c_ch1 = (unsigned short) (max_antenna_current * 887);
//     printf("max_c_ch1: %d current: %f\n", global_signals.max_c_ch1, max_antenna_current);
//     Signals_Setup_All_Channels();
    
//     for (int i = 0; i < (SIZEOF_SIGNALS - 1); i++)
//     {
//         timer1_seq_ready = 1;
//         Signals_Generate_All_Channels ();

//         // save ch1 data
//         v_duty_ch1[i] = pi_ch1.last_d;
//         v_sp_ch1[i] = pi_ch1.setpoint;
//         v_error_ch1[i] = pi_ch1.setpoint - pi_ch1.sample;
//         v_adc_ch1[i] = pi_ch1.sample;

//         if (treat_in_ch1 == 2)    //some error on channel!!!
//         {
//             printf("treat stopped on channel 1 in sequence: %d\n", i);
//             current_ok = 0;
//             break;
//         }
//     }

//     printf(" antenna emission ended: ");
//     if (current_ok)
//         PrintOK();
//     else
//         PrintERR();
        

//     ///////////////////////////
//     // Backup Data to a file //
//     ///////////////////////////
//     FILE * file = fopen("data.txt", "w");

//     if (file == NULL)
//     {
//         printf("data file not created!\n");
//         return;
//     }

//     // Vector_Short_To_File (file, "duty1", v_duty_ch1, SIZEOF_SIGNALS);
//     Vector_Short_To_File (file, "adc1", v_adc_ch1, SIZEOF_SIGNALS);
//     // Vector_Short_To_File (file, "error1", v_error_ch1, SIZEOF_SIGNALS);    
//     Vector_Short_To_File (file, "setpoint1", v_sp_ch1, SIZEOF_SIGNALS);

//     printf("\nRun by hand python3 simul_outputs.py\n");
    
// }


// extern short p_inphase_ch1 [];
// extern const unsigned short * p_table_inphase;
// void Test_Single_Signal_Single_Antenna_Single_Freq_OpenLoop (void)
// {
//     unsigned char antenna_index = 0;    //check tests_know_antennas.c

//     unsigned char antenna_signal = TRIANGULAR_SIGNAL;
//     // unsigned char antenna_signal = SQUARE_SIGNAL;
//     // unsigned char antenna_signal = SINUSOIDAL_SIGNAL;

//     // float my_freq = 0.9;
//     // float my_freq = 5.0;    
//     float my_freq = 11.5;
//     // float my_freq = 23.5;
//     // float my_freq = 44.0;
//     // float my_freq = 67.0;
//     // float my_freq = 60.9;
//     // float my_freq = 80.9;
//     // float my_freq = 100.0;

//     int current_ok = 1;

//     switch (antenna_signal)
//     {
//     case SINUSOIDAL_SIGNAL:
//         printf("\nemitting with SINUSOIDAL_SIGNAL\n");
//         global_signals.signal = SINUSOIDAL_SIGNAL;
//         break;
//     case TRIANGULAR_SIGNAL:
//         printf("\nemitting with TRIANGULAR_SIGNAL\n");
//         global_signals.signal = TRIANGULAR_SIGNAL;
//         break;
//     case SQUARE_SIGNAL:
//         printf("\nemitting with SQUARE_SIGNAL\n");
//         global_signals.signal = SQUARE_SIGNAL;
//         break;
//     }

//     // resolve this with fsampling and 256 signal points
//     float phase = my_freq * 256 * 256 / 7000.0;
//     phase_accum = (unsigned short) phase;
//     printf("freq ask: %f phase calc: %f phase_accum: %d\n",
//            my_freq,
//            phase,
//            phase_accum);        
    
//     Plant_Out_Recursive_From_Single_Antenna_Reset (&plant_ch1, antenna_index);

//     // check the Tau for parameters correction
//     float Tau = La * Ra;
//     float DCgain = 192.0 / Ra;
//     printf("antenna La: %f Ra: %f Tau: %f DC gain: %f\n",
//            La,
//            Ra,
//            Tau,
//            DCgain);
    
//     // empty signals buffers
//     for (int i = 0; i < (SIZEOF_SIGNALS - 1); i++)
//     {
//         v_ref_ch1[i] = 0;
//         v_signal_ch1[i] = 0;
//         v_adc_ch1[i] = 0;
//     }
            
//     // current sensed 1A -> 887 pts
//     global_signals.max_c_ch1 = (unsigned short) (max_antenna_current * 887);
//     printf("max_c_ch1: %d current: %f\n", global_signals.max_c_ch1, max_antenna_current);
//     Signals_Setup_All_Channels();

//     // printf("b params: %f\n", f->b_params[0]);
//     // printf("a params: %f %f\n", f->a_params[0], f->a_params[1]);
//     // setup de reference signal
//     // k = 0
//     float a1_pos = -a_vector_ch1[1];
//     // float calc = 16. * a1_pos / b_vector_ch1[0];
//     float calc = 0.4 * a1_pos / b_vector_ch1[0];    
    
//     *p_inphase_ch1 = (short) calc;

//     // k = 1..n
//     for (int i = 1; i < 127; i++)
//     {
//         // calc = 8. / b_vector_ch1[0];
//         calc = 0.2 / b_vector_ch1[0];        
//         calc = calc * (i + a1_pos * (3*i + 1));
//         *(p_inphase_ch1 + i) = (short) calc;
//     }
    
//     for (int i = 0; i < (SIZEOF_SIGNALS - 1); i++)
//     {
//         timer1_seq_ready = 1;
//         Signals_Generate_Single_Channel_OpenLoop ();

//         // current setpoint
//         unsigned int csp = *(p_table_inphase + i) * global_signals.max_c_ch1;
//         csp >>= 10;    //compensate for max current 1024
        
//         // save ch1 data
//         v_ref_ch1[i] = (short) csp;
//         v_signal_ch1[i] = *(p_inphase_ch1 + i);
//         v_adc_ch1[i] = IS_CH1;

//         if (treat_in_ch1 == 2)    //some error on channel!!!
//         {
//             printf("treat stopped on channel 1 in sequence: %d\n", i);
//             current_ok = 0;
//             break;
//         }
//     }

//     printf(" antenna emission ended: ");
//     if (current_ok)
//         PrintOK();
//     else
//         PrintERR();
        

//     ///////////////////////////
//     // Backup Data to a file //
//     ///////////////////////////
//     FILE * file = fopen("data.txt", "w");

//     if (file == NULL)
//     {
//         printf("data file not created!\n");
//         return;
//     }

//     // Vector_Short_To_File (file, "duty1", v_duty_ch1, SIZEOF_SIGNALS);
//     Vector_Short_To_File (file, "adc1", v_adc_ch1, SIZEOF_SIGNALS);
//     Vector_Short_To_File (file, "signal_x", v_signal_ch1, SIZEOF_SIGNALS);    
//     Vector_Short_To_File (file, "setpoint1", v_ref_ch1, SIZEOF_SIGNALS);

//     printf("\nRun by hand python3 simul_outputs.py\n");
    
// }




// float v_dummy [SIZEOF_SIGNALS] = { 0 };
// void Test_Plant_Step_Response (void)
// {
//     int length = 200;

//     printf("testing only for ch1!\n");
//     for (int i = 0; i < length; i++)
//     {
//         v_duty_ch1[i] = 950;
//         v_dummy[i] = Plant_Out_Recursive (&plant_ch1, v_duty_ch1[i]);

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


//--- end of file ---//


