//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "treatment.h"
#include "comms.h"
#include "antennas_defs.h"
#include "antennas.h"
#include "channels_defs.h"


// helper modules
#include "tests_ok.h"
#include "tests_mock_usart.h"
#include "tests_know_antennas.h"
#include "tests_vector_utils.h"
#include "tests_recursive_utils.h"


#include <stdio.h>
#include <string.h>
// #include <math.h>

// Types Constants and Macros --------------------------------------------------
// #define SIZEOF_SIGNALS    256    // one signal or one complete table
#define SIZEOF_SIGNALS    (256 * 2)
// #define SIZEOF_SIGNALS    (506)
// #define SIZEOF_SIGNALS    (1020)
// #define SIZEOF_SIGNALS    (132)
// #define SIZEOF_SIGNALS    (20000)

//ESTADOS DEL BUZZER copied from hard.h
typedef enum
{    
    BUZZER_INIT = 0,
    BUZZER_TO_STOP,

    BUZZER_MULTIPLE_LONG,
    BUZZER_MULTIPLE_LONGA,
    BUZZER_MULTIPLE_LONGB,

    BUZZER_MULTIPLE_HALF,
    BUZZER_MULTIPLE_HALFA,
    BUZZER_MULTIPLE_HALFB,

    BUZZER_MULTIPLE_SHORT,
    BUZZER_MULTIPLE_SHORTA,
    BUZZER_MULTIPLE_SHORTB
    
} buzzer_state_t;

//COMANDOS DEL BUZZER	(tienen que ser los del estado de arriba)
// copied from hard.h
#define BUZZER_STOP_CMD		BUZZER_TO_STOP
#define BUZZER_LONG_CMD		BUZZER_MULTIPLE_LONG
#define BUZZER_HALF_CMD		BUZZER_MULTIPLE_HALF
#define BUZZER_SHORT_CMD	BUZZER_MULTIPLE_SHORT

#define IS_CH4    adc_ch[0]
#define IS_CH3    adc_ch[1]
#define IS_CH2    adc_ch[2]
#define IS_CH1    adc_ch[3]


// Externals -------------------------------------------------------------------
volatile unsigned short adc_ch [4];
volatile unsigned char timer1_seq_ready = 0;

unsigned short comms_messages_rpi = 0;
extern volatile unsigned short secs_in_treatment;
extern unsigned short secs_end_treatment;
extern unsigned short secs_elapsed_up_to_now;


// Globals ---------------------------------------------------------------------
int answer_params = 0;
int answer_keepalive = 0;
int answer_name = 0;
int answer_temp = 0;
int overcurrent_samples = 0;


#define B_SIZE    1
#define A_SIZE    2
// ch1 plant
// float b_vector_ch1 [B_SIZE] = { 0.62806727 };
// float a_vector_ch1 [A_SIZE] = { 1., -0.94942247 };
float b_vector_ch1 [B_SIZE] = { 0.6445 };
float a_vector_ch1 [A_SIZE] = { 1., -0.94809 };
float ins_vector_ch1 [B_SIZE] = { 0.0 };
float outs_vector_ch1 [A_SIZE] = { 0.0 };
recursive_filter_t plant_ch1;

short v_duty_ch1 [SIZEOF_SIGNALS] = { 0 };
short v_error_ch1 [SIZEOF_SIGNALS] = { 0 };
short v_sp_ch1 [SIZEOF_SIGNALS] = { 0 };
short v_adc_ch1 [SIZEOF_SIGNALS] = { 0 };
short v_ref_ch1 [SIZEOF_SIGNALS] = { 0 };
short v_signal_ch1 [SIZEOF_SIGNALS] = { 0 };

float b_vector_ch3 [B_SIZE] = { 0.6445 };
float a_vector_ch3 [A_SIZE] = { 1., -0.94809 };
float ins_vector_ch3 [B_SIZE] = { 0.0 };
float outs_vector_ch3 [A_SIZE] = { 0.0 };
recursive_filter_t plant_ch3;

short v_duty_ch3 [SIZEOF_SIGNALS] = { 0 };
short v_adc_ch3 [SIZEOF_SIGNALS] = { 0 };

// Module Functions to Test ----------------------------------------------------
void Test_Treatment_All_Chain (void);
void Test_Treatment_All_Chain_No_Vectors (void);
void Test_Treatment_All_Chain_Soft_Overcurrent_Channel1_Continue (void);
void Test_Treatment_All_Chain_Soft_Overcurrent_Channel1_Stop (void);


// Module Auxiliary Functions --------------------------------------------------
void Usart2CB (char * msg);
void Usart3CB (char * msg);
void Uart4CB (char * msg);
void Uart5CB (char * msg);
void Set_Recursive_From_Single_Antenna (recursive_filter_t * f,
                                        unsigned char antenna_index,
                                        unsigned char which_channel);
float Plant_Out_Recursive (recursive_filter_t * f, short duty);
unsigned short Adc12BitsConvertion (float sample);
unsigned short Adc10BitsConvertion (float sample);


// Tests Module Auxiliary or General Functions ---------------------------------


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{

    Test_Treatment_All_Chain();
    // Test_Treatment_All_Chain_No_Vectors ();    //no saved vectors 

    // Test_Treatment_All_Chain_Soft_Overcurrent_Channel1_Stop ();
    // Test_Treatment_All_Chain_Soft_Overcurrent_Channel1_Continue ();

    return 0;
}


typedef enum {
    TREATMENT_STANDBY = 0,
    TREATMENT_CHECK_ANTENNAS_CONNECTED,
    TREATMENT_STARTING,    
    TREATMENT_RUNNING,
    TREATMENT_PAUSED,
    TREATMENT_WITH_ERRORS,
    TREATMENT_STOPPING

} treatment_t;


extern unsigned char treat_state;
extern treatment_conf_t treatment_conf;
void Test_Treatment_All_Chain (void)
{
    int some_err = 0;    
    resp_e resp = resp_ok;

    printf("\n-- Testing Treatment Manager All Chain --\n");    
    printf("-- treat in standby\n");
    for (int i = 0; i < 20; i++)
        Treatment_Manager();

    if (treat_state != TREATMENT_STANDBY)
        some_err = 1;

    printf("-- settings treatment params...\n");
    for (int i = 0; i < 20; i++)
    {
        Treatment_Manager();
        if (i == 0)
        {
            printf("-- set signal\n");
            Usart1FillRxBuffer("signal square\r\n");
            // Usart1FillRxBuffer("signal triangular\r\n");
            // Usart1FillRxBuffer("signal sinusoidal\r\n");            
        }

        if (i == 5)
        {
            printf("-- set power\n");
            Usart1FillRxBuffer("power 100\r\n");
            // Usart1FillRxBuffer("power 050\r\n");
            // Usart1FillRxBuffer("power 025\r\n");            
            // Usart1FillRxBuffer("power 010\r\n");            
        }

        if (i == 10)
        {
            printf("-- set frequency\n");
            // Usart1FillRxBuffer("frequency 200.00\r\n");            
            // Usart1FillRxBuffer("frequency 90.99\r\n");
            Usart1FillRxBuffer("frequency 86.22\r\n");            
            // Usart1FillRxBuffer("frequency 56.00\r\n");            
            // Usart1FillRxBuffer("frequency 28.00\r\n");
            // Usart1FillRxBuffer("frequency 27.34\r\n");
            // Usart1FillRxBuffer("frequency 20.50\r\n");            
            // Usart1FillRxBuffer("frequency 15.00\r\n");
            // Usart1FillRxBuffer("frequency 14.00\r\n");
            // Usart1FillRxBuffer("frequency 10.50\r\n");
            // Usart1FillRxBuffer("frequency 07.00\r\n");
            // Usart1FillRxBuffer("frequency 01.00\r\n");            
        }

        if (i == 15)
        {
            printf("-- set time\n");
            Usart1FillRxBuffer("duration,120\r\n");
        }
    }
        
    if (treat_state != TREATMENT_STANDBY)
        some_err = 1;
    
    
    if (!some_err)
    {
        printf("-- setting antennas connection\n");

        // activate usarts callbacks
        Usart2Callback(Usart2CB);
        // Usart3Callback(Usart3CB);
        Uart4Callback(Uart4CB);
        // Uart5Callback(Uart5CB);

        // activate antennas answers
        answer_params = 1;
        answer_keepalive = 1;
        answer_name = 1;
        
    
        for (int i = 0; i < 20000; i++)
        {
            AntennaUpdateStates ();
            AntennaTimeouts ();
        }
        
        if (treat_state != TREATMENT_STANDBY)
            some_err = 1;
        
    }
    
    
    if (!some_err)
    {
        printf("-- treat to running\n");        
        
        for (int i = 0; i < SIZEOF_SIGNALS; i++)
        {
            timer1_seq_ready = 1;
            Treatment_Manager();
            if (i == 0)
            {
                printf("-- set to run\n");
                Usart1FillRxBuffer("start,\r\n");
            }
        }

        if (treat_state != TREATMENT_RUNNING)
            some_err = 1;
        
    }
    
    printf("Testing Treatment All Chain: ");
    if (some_err)
        PrintERR();
    else
        PrintOK();

    // backup data to file
        ///////////////////////////
    // Backup Data to a file //
    ///////////////////////////
    FILE * file = fopen("data.txt", "w");

    if (file == NULL)
    {
        printf("data file not created!\n");
        return;
    }

    Vector_Short_To_File (file, "duty1", v_duty_ch1, SIZEOF_SIGNALS);
    Vector_Short_To_File (file, "adc1", v_adc_ch1, SIZEOF_SIGNALS);
    Vector_Short_To_File (file, "duty3", v_duty_ch3, SIZEOF_SIGNALS);
    Vector_Short_To_File (file, "adc3", v_adc_ch3, SIZEOF_SIGNALS);
    printf("\nRun by hand python3 simul_outputs.py\n");
}


void Test_Treatment_All_Chain_No_Vectors (void)
{
    int some_err = 0;    
    resp_e resp = resp_ok;

    printf("\n-- Testing Treatment Manager All Chain --\n");    
    printf("-- treat in standby\n");
    for (int i = 0; i < 20; i++)
        Treatment_Manager();

    if (treat_state != TREATMENT_STANDBY)
        some_err = 1;

    printf("-- settings treatment params...\n");
    for (int i = 0; i < 20; i++)
    {
        Treatment_Manager();
        if (i == 0)
        {
            printf("-- set signal\n");
            Usart1FillRxBuffer("signal square\r\n");
            // Usart1FillRxBuffer("signal triangular\r\n");
            // Usart1FillRxBuffer("signal sinusoidal\r\n");            
        }

        if (i == 5)
        {
            printf("-- set power\n");
            Usart1FillRxBuffer("power 100\r\n");
            // Usart1FillRxBuffer("power 050\r\n");
            // Usart1FillRxBuffer("power 010\r\n");            
        }

        if (i == 10)
        {
            printf("-- set frequency\n");
            // Usart1FillRxBuffer("frequency 90.99\r\n");
            // Usart1FillRxBuffer("frequency 86.22\r\n");            
            // Usart1FillRxBuffer("frequency 56.00\r\n");
            // Usart1FillRxBuffer("frequency 28.00\r\n");            
            // Usart1FillRxBuffer("frequency 20.50\r\n");            
            // Usart1FillRxBuffer("frequency 15.00\r\n");
            // Usart1FillRxBuffer("frequency 14.00\r\n");
            // Usart1FillRxBuffer("frequency 10.50\r\n");
            Usart1FillRxBuffer("frequency 07.83\r\n");
        }

        if (i == 15)
        {
            printf("-- set time\n");
            Usart1FillRxBuffer("duration,120\r\n");
        }
    }
        
    if (treat_state != TREATMENT_STANDBY)
        some_err = 1;
    
    
    if (!some_err)
    {
        printf("-- setting antennas connection\n");

        // activate usarts callbacks
        Usart2Callback(Usart2CB);
        // Usart3Callback(Usart3CB);
        // Uart4Callback(Uart4CB);
        // Uart5Callback(Uart5CB);

        // activate antennas answers
        answer_params = 1;
        answer_keepalive = 1;
        answer_name = 1;
        answer_temp = 1;
        
    
        for (int i = 0; i < 20000; i++)
        {
            AntennaUpdateStates ();
            AntennaTimeouts ();
        }
        
        if (treat_state != TREATMENT_STANDBY)
            some_err = 1;
        
    }
    
    
    if (!some_err)
    {
        printf("-- treat to running\n");        
        
        for (int i = 0; i < SIZEOF_SIGNALS; i++)
        {
            timer1_seq_ready = 1;
            Treatment_Manager();
            AntennaUpdateStates ();
            AntennaTimeouts ();            
            if (i == 0)
            {
                printf("-- set to run\n");
                Usart1FillRxBuffer("start,\r\n");
            }
        }

        if (treat_state != TREATMENT_RUNNING)
            some_err = 1;
        
    }
    
    printf("Testing Treatment All Chain No vectors: ");
    if (some_err)
        PrintERR();
    else
        PrintOK();

}


void Test_Treatment_All_Chain_Soft_Overcurrent_Channel1_Continue (void)
{
    int some_err = 0;    
    resp_e resp = resp_ok;

    printf("\n-- Testing Treatment Manager All Chain --\n");    
    printf("-- treat in standby\n");
    for (int i = 0; i < 20; i++)
        Treatment_Manager();

    if (treat_state != TREATMENT_STANDBY)
        some_err = 1;

    printf("-- settings treatment params...\n");
    for (int i = 0; i < 20; i++)
    {
        Treatment_Manager();
        if (i == 0)
        {
            printf("-- set signal\n");
            Usart1FillRxBuffer("signal square\r\n");
        }

        if (i == 5)
        {
            printf("-- set power\n");
            Usart1FillRxBuffer("power 050\r\n");
        }

        if (i == 10)
        {
            printf("-- set frequency\n");
            Usart1FillRxBuffer("frequency 20.50\r\n");            
        }

        if (i == 15)
        {
            printf("-- set time\n");
            Usart1FillRxBuffer("duration,120\r\n");
        }
    }
        
    if (treat_state != TREATMENT_STANDBY)
        some_err = 1;
    
    
    if (!some_err)
    {
        printf("-- setting antennas connection\n");

        // activate usarts callbacks
        Usart2Callback(Usart2CB);
        Usart3Callback(Usart3CB);
        Uart4Callback(Uart4CB);
        Uart5Callback(Uart5CB);

        // activate antennas answers
        answer_params = 1;
        answer_keepalive = 1;
        answer_name = 1;
        
    
        for (int i = 0; i < 20000; i++)
        {
            AntennaUpdateStates ();
            AntennaTimeouts ();
        }
        
        if (treat_state != TREATMENT_STANDBY)
            some_err = 1;
        
    }
    
    
    if (!some_err)
    {
        printf("-- treat to running\n");

        // activate overcurrent samples
        overcurrent_samples = 2000;
        
        for (int i = 0; i < SIZEOF_SIGNALS; i++)
        {
            timer1_seq_ready = 1;
            Treatment_Manager();
            if (i == 0)
            {
                printf("-- set to run\n");
                Usart1FillRxBuffer("start,\r\n");
            }
        }

        if (treat_state != TREATMENT_RUNNING)
            some_err = 1;
        
    }
    
    printf("Testing Treatment All Chain with soft overcurrent: ");
    if (some_err)
        PrintERR();
    else
        PrintOK();

    // backup data to file
        ///////////////////////////
    // Backup Data to a file //
    ///////////////////////////
    FILE * file = fopen("data.txt", "w");

    if (file == NULL)
    {
        printf("data file not created!\n");
        return;
    }

    Vector_Short_To_File (file, "duty1", v_duty_ch1, SIZEOF_SIGNALS);
    Vector_Short_To_File (file, "adc1", v_adc_ch1, SIZEOF_SIGNALS);
    printf("\nRun by hand python3 simul_outputs.py\n");
}


void Test_Treatment_All_Chain_Soft_Overcurrent_Channel1_Stop (void)
{
    int some_err = 0;    
    resp_e resp = resp_ok;

    printf("\n-- Testing Treatment Manager All Chain --\n");    
    printf("-- treat in standby\n");
    for (int i = 0; i < 20; i++)
        Treatment_Manager();

    if (treat_state != TREATMENT_STANDBY)
        some_err = 1;

    printf("-- settings treatment params...\n");
    for (int i = 0; i < 20; i++)
    {
        Treatment_Manager();
        if (i == 0)
        {
            printf("-- set signal\n");
            Usart1FillRxBuffer("signal square\r\n");
        }

        if (i == 5)
        {
            printf("-- set power\n");
            Usart1FillRxBuffer("power 100\r\n");
        }

        if (i == 10)
        {
            printf("-- set frequency\n");
            Usart1FillRxBuffer("frequency 20.50\r\n");            
        }

        if (i == 15)
        {
            printf("-- set time\n");
            Usart1FillRxBuffer("duration,120\r\n");
        }
    }
        
    if (treat_state != TREATMENT_STANDBY)
        some_err = 1;
    
    
    if (!some_err)
    {
        printf("-- setting antennas connection\n");

        // activate usarts callbacks
        Usart2Callback(Usart2CB);
        // Usart3Callback(Usart3CB);
        // Uart4Callback(Uart4CB);
        // Uart5Callback(Uart5CB);

        // activate antennas answers
        answer_params = 1;
        answer_keepalive = 1;
        answer_name = 1;
    
        for (int i = 0; i < 20000; i++)
        {
            AntennaUpdateStates ();
            AntennaTimeouts ();
        }
        
        if (treat_state != TREATMENT_STANDBY)
            some_err = 1;
        
    }
    
    
    if (!some_err)
    {
        printf("-- treat to running\n");

        // activate overcurrent samples
        overcurrent_samples = 2000;
        
        for (int i = 0; i < SIZEOF_SIGNALS; i++)
        {
            timer1_seq_ready = 1;
            Treatment_Manager();
            if (i == 0)
            {
                printf("-- set to run\n");
                Usart1FillRxBuffer("start,\r\n");
            }
        }

        if (treat_state != TREATMENT_STANDBY)
            some_err = 1;
        
    }
    
    printf("Testing Treatment All Chain with soft overcurrent and STOP: ");
    if (some_err)
        PrintERR();
    else
        PrintOK();

    // backup data to file
        ///////////////////////////
    // Backup Data to a file //
    ///////////////////////////
    FILE * file = fopen("data.txt", "w");

    if (file == NULL)
    {
        printf("data file not created!\n");
        return;
    }

    Vector_Short_To_File (file, "duty1", v_duty_ch1, SIZEOF_SIGNALS);
    Vector_Short_To_File (file, "adc1", v_adc_ch1, SIZEOF_SIGNALS);
    printf("\nRun by hand python3 simul_outputs.py\n");
}


float Vin = 192.0;
float Rsense = 0.055;
float Ao = 13.0;
float La = 0.142;
float Ra = 11.0;
float fsampling = 7000.0;
void Set_Recursive_From_Single_Antenna (recursive_filter_t * f,
                                        unsigned char antenna_index,
                                        unsigned char which_channel)
{
    antenna_st my_antenna;

    if (which_channel == CH1)
    {
        f->b_params = b_vector_ch1;
        f->a_params = a_vector_ch1;
        f->b_size = B_SIZE;
        f->a_size = A_SIZE;
        f->last_inputs = ins_vector_ch1;
        f->last_outputs = outs_vector_ch1;
    }

    if (which_channel == CH3)
    {
        f->b_params = b_vector_ch3;
        f->a_params = a_vector_ch3;
        f->b_size = B_SIZE;
        f->a_size = A_SIZE;
        f->last_inputs = ins_vector_ch3;
        f->last_outputs = outs_vector_ch3;        
    }

    TSP_Get_Know_Single_Antenna (&my_antenna, antenna_index);

    La = my_antenna.inductance_int + my_antenna.inductance_dec / 100.0;
    La = La / 1000.0;    // convert mHy to Hy
    Ra = my_antenna.resistance_int + my_antenna.resistance_dec / 100.0;
    float max_antenna_current = my_antenna.current_limit_int + my_antenna.current_limit_dec / 100.0;
    
    float b0 = Rsense * Ao;
    b0 = b0 / (La * fsampling);

    float a0 = 1.0;
    float a1 = -1.0 + (Ra + Rsense)/(La * fsampling);

    if (which_channel == CH1)
    {
        b_vector_ch1[0] = b0;
        a_vector_ch1[0] = a0;
        a_vector_ch1[1] = a1;

        for (int i = 0; i < B_SIZE; i++)
            ins_vector_ch1[i] = 0.0;

        for (int i = 0; i < A_SIZE; i++)
            outs_vector_ch1[i] = 0.0;
    }

    if (which_channel == CH3)
    {
        b_vector_ch3[0] = b0;
        a_vector_ch3[0] = a0;
        a_vector_ch3[1] = a1;

        for (int i = 0; i < B_SIZE; i++)
            ins_vector_ch3[i] = 0.0;

        for (int i = 0; i < A_SIZE; i++)
            outs_vector_ch3[i] = 0.0;
    }
    
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


float Plant_Out_Recursive (recursive_filter_t * f, short duty)
{
    return Recursive_Filter_Float(f, (float) duty * Vin / 1000.0);
}


void Plant_Out_Recursive_Reset (int which_channel, recursive_filter_t * f)
{
    Recursive_Filter_Float_Reset(f);
}


void Plant_Out_PI_Flush (unsigned char which_channel)
{
}

// Module Mocked Functions -----------------------------------------------------
void RPI_Send (char * a)
{
    Usart1Send(a);
}


void HARD_L1_ON (void)
{
    printf("Led1 -> ON\n");
}


void HARD_L1_OFF (void)
{
    printf("Led1 -> OFF\n");
}

unsigned char Led1_Is_On (void)
{
    // printf("Led1 -> ON\n");
    return 1;
}


void Led1_On (void)
{
    // printf("Led1 -> ON\n");
}


void Led1_Off (void)
{
    // printf("Led1 -> OFF\n");    
}

void ChangeLed (unsigned char number)
{
    printf("change led to %d bips\n", number);
}


void Wait_ms (unsigned short number)
{
    printf("wait for %d millis\n", number);
}


// void AntennaSendKnowInfoWithTimer (void)
// {
// }

void Usart2CB (char * msg)
{
    if ((answer_params) &&
        (!strncmp(msg, "get_params", sizeof("get_params") - 1)))
    {
        // const char s_antena [] = { "ant0,012.27,087.90,001.80,065.00\r\n" };
        // Usart2FillRxBuffer((char *) s_antena);

        // mock process string
        int my_ant_index = 0;
        // int my_ant_index = 1;
        // int my_ant_index = 2;
        // int my_ant_index = 3;
        // int my_ant_index = 4;
        // int my_ant_index = 5;
        // int my_ant_index = 6;
        // int my_ant_index = 7;
        // int my_ant_index = 8;
        // int my_ant_index = 9;        
        antenna_st my_ant;
        TSP_Get_Know_Single_Antenna(&my_ant, my_ant_index);
        AntennaSetParamsStruct (CH1, &my_ant);
        Set_Recursive_From_Single_Antenna(&plant_ch1, my_ant_index, CH1);
    }

    if ((answer_keepalive) &&
        (!strncmp(msg, "keepalive", sizeof("keepalive") - 1)))
    {
        AntennaIsAnswering(CH1);        
    }

    if ((answer_name) &&
        (!strncmp(msg, "get_name", sizeof("get_name") - 1)))
    {
        AntennaSetName(CH1, "tunnel");
    }

    if ((answer_temp) &&
        (!strncmp(msg, "get_temp", sizeof("get_temp") - 1)))
    {
        AntennaSetCurrentTemp (CH1, 40, 0);
    }
    
}


void Usart3CB (char * msg)
{
    // Usart3Send(a);

    if ((answer_params) &&
        (!strncmp(msg, "get_params", sizeof("get_params") - 1)))
    {
        // const char s_antena [] = { "ant1,023.85,141.60,001.30,065.00\r\n" };
        // Usart3FillRxBuffer(s_antena);

        // mock process string
        antenna_st my_ant;
        TSP_Get_Know_Single_Antenna(&my_ant, 1);
        
        AntennaSetParamsStruct (CH2, &my_ant);
    }
    
    if ((answer_keepalive) &&
        (!strncmp(msg, "keepalive", sizeof("keepalive") - 1)))
    {
        AntennaIsAnswering(CH2);
    }

    if ((answer_name) &&
        (!strncmp(msg, "get_name", sizeof("get_name") - 1)))
    {
        AntennaSetName(CH2, "tunnel");
    }

    if ((answer_temp) &&
        (!strncmp(msg, "get_temp", sizeof("get_temp") - 1)))
    {
        AntennaSetCurrentTemp (CH2, 40, 0);
    }
    
}


void Uart4CB (char * msg)
{
    // Uart4Send(a);

    if ((answer_params) &&
        (!strncmp(msg, "get_params", sizeof("get_params") - 1)))
    {
        // const char s_antena [] = { "ant1,017.00,120.00,001.30,065.00\r\n" };
        // Uart4FillRxBuffer(s_antena);

        // mock process string
        // int my_ant_index = 0;
        // int my_ant_index = 1;
        // int my_ant_index = 2;
        // int my_ant_index = 3;
        // int my_ant_index = 4;
        // int my_ant_index = 5;
        // int my_ant_index = 6;
        // int my_ant_index = 7;
        int my_ant_index = 8;
        // int my_ant_index = 9;        
        antenna_st my_ant;
        TSP_Get_Know_Single_Antenna(&my_ant, my_ant_index);
        AntennaSetParamsStruct (CH3, &my_ant);
        Set_Recursive_From_Single_Antenna(&plant_ch3, my_ant_index, CH3);
    }

    if ((answer_keepalive) &&
        (!strncmp(msg, "keepalive", sizeof("keepalive") - 1)))
    {
        AntennaIsAnswering(CH3);
    }

    if ((answer_name) &&
        (!strncmp(msg, "get_name", sizeof("get_name") - 1)))
    {
        AntennaSetName(CH3, "tunnel");
    }

    if ((answer_temp) &&
        (!strncmp(msg, "get_temp", sizeof("get_temp") - 1)))
    {
        AntennaSetCurrentTemp (CH3, 40, 0);
    }        
}


void Uart5CB (char * msg)
{
    // Uart5Send(a);

    if ((answer_params) &&
        (!strncmp(msg, "get_params", sizeof("get_params") - 1)))
    {
        // const char s_antena [] = { "ant2,005.70,011.10,002.80,065.00\r\n" };
        // Uart5FillRxBuffer(s_antena);

        // mock process string
        antenna_st my_ant;
        TSP_Get_Know_Single_Antenna(&my_ant, 3);
        
        AntennaSetParamsStruct (CH4, &my_ant);
    }

    if ((answer_keepalive) &&
        (!strncmp(msg, "keepalive", sizeof("keepalive") - 1)))
    {
        AntennaIsAnswering(CH4);
    }

    if ((answer_name) &&
        (!strncmp(msg, "get_name", sizeof("get_name") - 1)))
    {
        AntennaSetName(CH4, "tunnel");
    }

    if ((answer_temp) &&
        (!strncmp(msg, "get_temp", sizeof("get_temp") - 1)))
    {
        AntennaSetCurrentTemp (CH4, 40, 0);
    }    
}

void BuzzerCommands (unsigned char cmd, unsigned char number)
{
    switch (cmd)
    {
    case BUZZER_SHORT_CMD:
        printf("buzzer %d bips short\n", number);
        break;

    case BUZZER_HALF_CMD:
        printf("buzzer %d bips half\n", number);        
        break;

    case BUZZER_LONG_CMD:
        printf("buzzer %d bips long\n", number);        
        break;

    default:
        printf("buzzer %d bips UNKNOW\n", number);        
        break;
           
    }
}


void UpdateLed (void)
{
}

void UpdateBuzzer (void)
{
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

// Mocked Functions for Signals Module -----------------------------------------
int pwm_cnt = 0;
void TIM8_Update_CH3 (unsigned short a)
{
    // printf("HL CH1: %d\n", a);
    if (a)
    {
        float output = 0.0;
        output = Plant_Out_Recursive(&plant_ch1, a);

        IS_CH1 = Adc12BitsConvertion (output);

        if (overcurrent_samples)
        {
            v_adc_ch1[pwm_cnt] = overcurrent_samples;
            IS_CH1 = overcurrent_samples;
        }
        else
            v_adc_ch1[pwm_cnt] = IS_CH1;
        
        v_duty_ch1[pwm_cnt] = a;
    }
}

void TIM8_Update_CH4 (unsigned short a)
{
    // printf("LR CH1: %d\n", a);
    if (a == 0)    // no emition no adc data
    {
        IS_CH1 = 0;
        Plant_Out_Recursive_Reset (CH1, &plant_ch1);
        Plant_Out_PI_Flush (CH1);
        v_adc_ch1[pwm_cnt] = IS_CH1;
        v_duty_ch1[pwm_cnt] = a;        
    }
    else if (a < 950)    // regulation on negative
    {
        float output = 0.0;
        // output = Plant_Out_Recursive(&plant_ch1, - a);
        output = Plant_Out_Recursive(&plant_ch1, -(950 - a));        
        // output = Plant_Out_Recursive(&plant_ch1, -a << 5);    // simulate a fast discharge     
        IS_CH1 = Adc12BitsConvertion (output);
        v_adc_ch1[pwm_cnt] = IS_CH1;
        // v_duty_ch1[pwm_cnt] = -a;
        v_duty_ch1[pwm_cnt] = -(950 - a);                                
    }
    else
    {
        // regulation by positive, do nothing in here
    }
    pwm_cnt++;
}

void TIM8_Update_CH2 (unsigned short a)
{
    // printf("HL CH2: %d\n", a);
}

void TIM8_Update_CH1 (unsigned short a)
{
    // printf("LR CH2: %d\n", a);
}

void TIM4_Update_CH2 (unsigned short a)
{
    // printf("HL CH3: %d\n", a);
    if (a)
    {
        float output = 0.0;
        output = Plant_Out_Recursive(&plant_ch3, a);

        IS_CH3 = Adc12BitsConvertion (output);

        if (overcurrent_samples)
        {
            v_adc_ch3[pwm_cnt] = overcurrent_samples;
            IS_CH3 = overcurrent_samples;
        }
        else
            v_adc_ch3[pwm_cnt] = IS_CH3;
        
        v_duty_ch3[pwm_cnt] = a;
    }    
}

void TIM4_Update_CH3 (unsigned short a)
{
    // printf("LR CH3: %d\n", a);
    if (a == 0)    // no emition no adc data
    {
        IS_CH3 = 0;
        Plant_Out_Recursive_Reset (CH3, &plant_ch3);
        Plant_Out_PI_Flush (CH3);
        v_adc_ch3[pwm_cnt] = IS_CH3;
        v_duty_ch3[pwm_cnt] = a;        
    }
    else if (a < 950)    // regulation on negative
    {
        float output = 0.0;
        // output = Plant_Out_Recursive(&plant_ch3, -a);
        output = Plant_Out_Recursive(&plant_ch3, -(950 - a));        
        // output = Plant_Out_Recursive(&plant_ch3, -a << 5);    // simulate a fast discharge     
        IS_CH3 = Adc12BitsConvertion (output);
        v_adc_ch3[pwm_cnt] = IS_CH3;
        // v_duty_ch3[pwm_cnt] = -a;
        v_duty_ch3[pwm_cnt] = -(950 - a);        
    }
    else
    {
        // regulation by positive, do nothing in here
    }    
}

void TIM5_Update_CH1 (unsigned short a)
{
    // printf("HL CH4: %d\n", a);
}

void TIM5_Update_CH2 (unsigned short a)
{
    // printf("LR CH4: %d\n", a);
}

// void Error_SetStatus (unsigned char error, unsigned char channel)
// {
//     error <<= 4;
//     error += channel + 1;
//     printf("error: 0x%02x\n", error);
// }

void EXTIOn (void)
{
}

void EXTIOff (void)
{
}

//--- end of file ---//


