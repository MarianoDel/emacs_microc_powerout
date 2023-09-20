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

// helper modules
#include "tests_ok.h"
#include "tests_mock_usart.h"

#include <stdio.h>
#include <string.h>

// Types Constants and Macros --------------------------------------------------


// Externals -------------------------------------------------------------------
extern treatment_conf_t treatment_conf;


// Globals ---------------------------------------------------------------------



// Module Functions to Test ----------------------------------------------------
void Test_Treatment_Settings (void);
void Test_Treatment_Manager (void);


// Module Auxiliary Functions --------------------------------------------------


// Tests Module Auxiliary or General Functions ---------------------------------


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{

    Test_Treatment_Settings();

    return 0;
}


void Test_Treatment_Settings (void)
{
    int some_err = 0;    
    resp_e resp = resp_ok;
    char test_str [200];
    
    printf("\n-- Testing Treatment Module Settings --\n");

    if (!some_err)
    {
        strcpy(test_str, "100.00Hz\r\n");
        resp = Treatment_SetFrequency_Str (test_str);
        if ((resp != resp_ok) ||
            (treatment_conf.freq_int != 100) ||
            (treatment_conf.freq_dec != 0))
        {
            printf("\nfrequency error: %s ", test_str);
            some_err = 1;
        }
    }

    if (!some_err)
    {
        for (int i = 0; i < 1000; i++)
        {
            int fint = i;
            int fdec = 50;

            sprintf(test_str, "%d.%02dHz\r\n", fint, fdec);
            resp = Treatment_SetFrequency_Str (test_str);
            if ((resp != resp_ok) ||
                (treatment_conf.freq_int != fint) ||
                (treatment_conf.freq_dec != fdec))
            {
                printf("\nfrequency error: %s ", test_str);
                some_err = 1;
                break;
            }
        }
    }

    // signal = TRIANGULAR_SIGNAL;
    // resp = Treatment_SetSignalType (signal);
    // signal = Treatment_GetSignalType();
    // if ((resp != resp_ok) || (signal != TRIANGULAR_SIGNAL))
    // {
    //     printf("\nTreatSignal error with %d ", signal);
    //     some_err = 1;
    // }

    // signal = SINUSOIDAL_SIGNAL;
    // resp = Treatment_SetSignalType (signal);
    // signal = Treatment_GetSignalType();
    // if ((resp != resp_ok) || (signal != SINUSOIDAL_SIGNAL))
    // {
    //     printf("\nTreatSignal error with %d ", signal);
    //     some_err = 1;
    // }

    // signal = 10;
    // resp = Treatment_SetSignalType (signal);
    // if (resp != resp_error)
    // {
    //     printf("\nTreatSignal error with %d ", signal);
    //     some_err = 1;
    // }

    // unsigned char f_int = 10;
    // unsigned char f_dec = 10;
    // resp = Treatment_SetFrequency (f_int, f_dec);
    // Treatment_GetFrequency (&f_int, &f_dec);
    // if ((resp != resp_ok) || (f_int != 10) || (f_dec != 10))
    // {
    //     printf("\nTreatFreq error with %d.%d ", f_int, f_dec);
    //     some_err = 1;
    // }

    // f_int = 100;
    // f_dec = 10;
    // resp = Treatment_SetFrequency (f_int, f_dec);
    // if (resp != resp_error)
    // {
    //     printf("\nTreatFreq error with %d.%d ", f_int, f_dec);
    //     some_err = 1;
    // }

    // unsigned char power = 100;
    // resp = Treatment_SetPower (power);
    // power = Treatment_GetPower ();

    // if ((resp != resp_ok) || (power != 100))
    // {
    //     printf("\nTreatPower error with %d ", power);
    //      some_err = 1;
    // }

    // power = 1;
    // resp = Treatment_SetPower (power);
    // power = Treatment_GetPower ();
    // if ((resp != resp_ok) || (power != 10))
    // {
    //     printf("\nTreatPower error with %d ", power);
    //     some_err = 1;
    // }

    // power = 120;
    // resp = Treatment_SetPower (power);
    // power = Treatment_GetPower ();
    // if ((resp != resp_ok) || (power != 100))
    // {
    //     printf("\nTreatPower error with %d ", power);
    //     some_err = 1;
    // }

    // unsigned char minutes = 100;
    // unsigned short secs = 0;
    // resp = Treatment_SetTimeinMinutes (minutes);
    // secs = Treatment_GetTime ();
    // if ((resp != resp_ok) || (secs != (minutes * 60)))
    // {
    //     printf("\nTreatTime error with %d ", minutes);
    //     some_err = 1;
    // }

    // minutes = 121;
    // resp = Treatment_SetTimeinMinutes (minutes);
    // if (resp != resp_error)
    // {
    //     printf("\nTreatTime error with %d ", minutes);
    //     some_err = 1;
    // }

    // unsigned char channels_a = 0;
    // unsigned char channels_b = 0;    
    // channels_a |= ENABLE_CH1_FLAG | ENABLE_CH2_FLAG | ENABLE_CH3_FLAG;
    // Treatment_SetChannelsFlag (channels_a);
    // channels_b = Treatment_GetChannelsFlag ();
    // if ((channels_a & 0x0f) != channels_b)
    // {
    //     printf("\nTreatChannels error with setted: %d getted: %d ", channels_a, channels_b);
    //     some_err = 1;
    // }

    // channels_a = DISABLE_CH1_FLAG;
    // Treatment_SetChannelsFlag (channels_a);
    // channels_b = Treatment_GetChannelsFlag ();
    // if (channels_b != ((ENABLE_CH2_FLAG | ENABLE_CH3_FLAG) & 0x0f))
    // {
    //     printf("\nTreatChannels error with setted: %d getted: %d ", channels_a, channels_b);
    //     some_err = 1;
    // }
    

    // char all_conf [250];
    // Treatment_GetAllConf(all_conf);
    // printf("-- get all conf\n");
    // printf("%s", all_conf);
    
    printf("Testing Treatment Module Settings: ");
    if (some_err)
        PrintERR();
    else
        PrintOK();
    
}


// void Test_Treatment_Manager (void)
// {
//     int some_err = 0;    
//     resp_e resp = resp_ok;

//     antenna_in_this_treatment = 0x0f;
//     printf("\n-- Testing Manager All Antennas --\n");
//     printf("-- treat in standby\n");
//     for (int i = 0; i < 20; i++)
//         Treatment_Manager();

//     if (treat_state != TREATMENT_STANDBY)
//         some_err = 1;

//     if (!some_err)
//     {
//         printf("-- treat to running\n");        
//         comms_messages_rpi |= COMM_START_TREAT;
        
//         for (int i = 0; i < 20; i++)
//         {
//             // printf("running loop: %d treat_state: %d\n", i, treat_state);
//             Treatment_Manager();
//         }

//         if (treat_state != TREATMENT_RUNNING)
//             some_err = 1;
        
//     }

//     if (!some_err)        
//     {
//         printf("-- treat to pause\n");
//         comms_messages_rpi |= COMM_PAUSE_TREAT;
        
//         for (int i = 0; i < 20; i++)
//         {
//             Treatment_Manager();
//             if (i == 5)
//                 comms_messages_rpi |= COMM_START_TREAT;
//         }

//         if (treat_state != TREATMENT_PAUSED)
//             some_err = 1;
        
//     }

//     if (!some_err)
//     {
//         printf("-- treat to run again\n");    
//         comms_messages_rpi |= COMM_PAUSE_TREAT;
        
//         for (int i = 0; i < 20; i++)
//             Treatment_Manager();

//         if (treat_state != TREATMENT_RUNNING)
//             some_err = 1;
        
//     }

//     if (!some_err)
//     {
//         printf("-- treat to stop\n");
//         comms_messages_rpi |= COMM_STOP_TREAT;
        
//         for (int i = 0; i < 20; i++)
//             Treatment_Manager();

//         if (treat_state != TREATMENT_STANDBY)
//             some_err = 1;
        
//     }

//     if (!some_err)
//     {
//         printf("-- treat to running double START sended\n");    
//         comms_messages_rpi |= COMM_START_TREAT;
        
//         for (int i = 0; i < 20; i++)
//         {
//             Treatment_Manager();
//             if (i == 5)
//                 comms_messages_rpi |= COMM_START_TREAT;
//         }

//         if (treat_state != TREATMENT_RUNNING)
//             some_err = 1;
        
//     }

//     if (!some_err)
//     {
//         printf("-- treat to pause and stop\n");
//         comms_messages_rpi |= COMM_PAUSE_TREAT;
        
//         for (int i = 0; i < 20; i++)
//         {
//             Treatment_Manager();
//             if (i == 5)
//                 comms_messages_rpi |= COMM_STOP_TREAT;
//         }

//         if (treat_state != TREATMENT_STANDBY)
//             some_err = 1;
        
//     }
    
//     printf("Testing Treatment Manager: ");
//     if (some_err)
//         PrintERR();
//     else
//         PrintOK();
    
// }


// Module Mocked Functions -----------------------------------------------------
void RPI_Send (char * a)
{
    Usart1Send(a);
}


void ChangeLed (unsigned char number)
{
    printf("change led to %d bips\n", number);
}


void Wait_ms (unsigned short number)
{
    printf("wait for %d millis\n", number);
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


// char * led_blinking [4] = {"LED_NO_BLINKING",
//                            "LED_TREATMENT_STANDBY",
//                            "LED_TREATMENT_SQUARE_RUNNING",
//                            "LED_TREATMENT_SINUS_RUNNING"};

// void ChangeLed(unsigned char function)
// {
//     printf("led blinking: %d %s\n", function, led_blinking[function]);
// }


void Signals_Sinus (treatment_conf_t * treat)
{
    printf("generating sinus signal: %d\n", treat->mode);
}

void Signals_Square (treatment_conf_t * treat)
{
    printf("generating square signal: %d\n", treat->mode);
}


void Signals_Stop (void)
{
    printf("stopping all signals\n");
}

//--- end of file ---//


