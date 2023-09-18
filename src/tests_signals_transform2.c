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

typedef struct {
    short setpoint;
    short sample;
    short last_d;
    short error_z1;
    short error_z2;
    short ki_accumulator;    
    unsigned short kp;
    unsigned short ki;
    unsigned short kd;
} pid_st;


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


short v_input [SIZEOF_SIGNALS] = { 0 };
short v_input_applied [SIZEOF_SIGNALS] = { 0 };
short v_duty [SIZEOF_SIGNALS] = { 0 };
short v_error [SIZEOF_SIGNALS] = { 0 };
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
void Plant_From_Input_And_Feedback (void);


short PID (pid_st * p);
void PID_Clear (pid_st * p);

unsigned short Adc12BitsConvertion (float sample);
unsigned short Adc10BitsConvertion (float sample);


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    printf("Start of Analog simulations...\n");
    // Plant_Step_Response ();
    Plant_From_Input_And_Feedback ();
    
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


void Plant_From_Input_And_Feedback (void)
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
        v_input[i] = 1000;
    }
    for (int i = SIZEOF_SIGNALS / 2; i < SIZEOF_SIGNALS; i++)
    {
        v_input[i] = 0;
    }
    // end of step function
    
    // triangular
    // for (int i = 0; i < SIZEOF_SIGNALS / 2; i++)
    // {
    //     v_input[i] =  2 * i * 1000 / SIZEOF_SIGNALS ;
    // }
    // for (int i = SIZEOF_SIGNALS / 2; i < SIZEOF_SIGNALS; i++)
    // {
    //     v_input[i] = 0;
    // }
    // end of triangular

    // sinusoidal
    // for (int i = 0; i < SIZEOF_SIGNALS / 2; i++)
    // {
    //     v_input[i] = 1000 * sin(6.28 * (float) i / (float)SIZEOF_SIGNALS);
    // }
    // for (int i = SIZEOF_SIGNALS / 2; i < SIZEOF_SIGNALS; i++)
    // {
    //     v_input[i] = 0;
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
        v_input[i] = v_input[i] * multi;
    }

    // pre calculation for pid
    // float zero_to_pole = 8;
    // float b1_pre = 1. - (1. - a1_pos) * zero_to_pole;
    float b0 = b_vector_ch1[0];
    float gain = b0 /(1. - a1_pos);
    float gain_five_perc = 19. /(gain * Vin);
    printf("  filter gain: %f prop gain: %f gain 5 perc: %f\n", gain, gain * Vin, gain_five_perc);
    gain_five_perc = gain_five_perc * 128;
    printf(" setting pid controller\n");
    pid_st pid_control;
    pid_control.ki = 2;
    pid_control.kp = (short) gain_five_perc;
    // pid_control.kp = 40;    
    pid_control.kd = 0;

    float ki_dig = (float) pid_control.ki / fsampling;
    float kp_dig = (float) pid_control.kp - ki_dig / 2.;
    float kd_dig = (float) pid_control.kd * fsampling;

    printf(" pid values ki: %f kp: %f kd: %f\n", ki_dig / 128., kp_dig / 128., kd_dig / 128.);
    PID_Clear (&pid_control);

    float freq_selected = 200.0;
    float points = fsampling / freq_selected;    
    unsigned short points_int = (unsigned short) points;
    unsigned short points_offset = 0;
    if (points_int < SIZEOF_SIGNALS)
    {
        points = SIZEOF_SIGNALS / points;
        points_offset = (unsigned short) points;
    }
    else
        points_offset = 1;
    
    printf(" apply signal, freq: %fHz, points: %d offset: %d\n",
           freq_selected,
           points_int,
           points_offset);

    int i = 0;
    int loops = 0;    
    do {
        pid_control.setpoint = v_input[i];
        v_input_applied[loops] = v_input[i];
        if (loops)
            pid_control.sample = v_output[loops - 1];
        else
            pid_control.sample = 0;
        
        duty = PID (&pid_control);
        v_duty[loops] = duty;
        v_error[loops] = pid_control.error_z1;
        signal_out = Recursive_Filter_Float (&plant, Vin * (float) duty / 1000.0);
        v_output[loops] = Adc12BitsConvertion(signal_out);
        i += points_offset;
        loops++;
        
    } while (i < SIZEOF_SIGNALS);
    printf(" loops: %d\n", loops);

    ///////////////////////////
    // Backup Data to a file //
    ///////////////////////////
    FILE * file = fopen("data.txt", "w");

    if (file == NULL)
    {
        printf("data file not created!\n");
        return;
    }

    // Vector_Short_To_File (file, "input", v_input, loops);
    Vector_Short_To_File (file, "input", v_input_applied, loops);    
    Vector_Short_To_File (file, "error", v_error, loops);
    Vector_Short_To_File (file, "duty", v_duty, loops);
    Vector_Short_To_File (file, "output", v_output, loops);    

    // Vector_Short_To_File (file, "input", v_input, SIZEOF_SIGNALS);
    // Vector_Short_To_File (file, "error", v_error, SIZEOF_SIGNALS);
    // Vector_Short_To_File (file, "duty", v_duty, SIZEOF_SIGNALS);
    // Vector_Short_To_File (file, "output", v_output, SIZEOF_SIGNALS);    
    
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


#define PID_CONSTANT_DIVIDER    7    //todos se dividen por 128
short PID (pid_st * p)
{
    int acc = 0;
    short error = 0;
    short d = 0;

    unsigned short k1 = 0;
    unsigned short k2 = 0;
    unsigned short k3 = 0;
    
    short val_k1 = 0;
    short val_k2 = 0;
    short val_k3 = 0;

    k1 = p->kp + p->ki + p->kd;
    k2 = p->kp + p->kd + p->kd;
    k3 = p->kd;
    
    error = p->setpoint - p->sample;

    //K1
    acc = k1 * error;
    val_k1 = acc / 128;

    //K2
    acc = k2 * p->error_z1;
    val_k2 = acc / 128;

    //K3
    acc = k3 * p->error_z2;
    val_k3 = acc / 128;

    d = p->last_d + val_k1 - val_k2 + val_k3;

    //Update PID variables
    p->error_z2 = p->error_z1;
    p->error_z1 = error;
    p->last_d = d;

    return d;    
}


void PID_Clear (pid_st * p)
{
    p->error_z2 = 0;
    p->error_z1 = 0;
    p->last_d = 0;    
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


//--- end of file ---//


