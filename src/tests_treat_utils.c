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
#include "utils.h"


#include <stdio.h>
// #include <math.h>

// Types Constants and Macros --------------------------------------------------


// Externals -------------------------------------------------------------------
volatile unsigned short adc_ch [2];
volatile unsigned char usart1_have_data = 0;
unsigned short comms_messages_rpi = 0;

// Globals ---------------------------------------------------------------------


// Module Functions to Test ----------------------------------------------------
void Test_Utils_Module (void);
void Test_Treatment_Module (void);
void Test_Functions (void);

// Module Auxiliary Functions --------------------------------------------------


// Tests Module Auxiliary or General Functions ---------------------------------
void PrintOK (void);
void PrintERR (void);


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    // Test_Functions ();
    Test_Utils_Module();
    Test_Treatment_Module();

    return 0;
}


void Test_Utils_Module (void)
{
    int some_err = 0;
    unsigned short new_number;
    unsigned char cnt;

    char num0 [] = "000";
    char num1 [] = "999";

    printf("Testing Utils Module: ");
    
    new_number = 0;
    cnt = 0;
    cnt = StringIsANumber(num1, &new_number);    
    if ((cnt != 3) || (new_number != 999))
    {
        printf("\nStringIs error with number %s ", num1);
        some_err = 1;
    }

    new_number = 0;
    cnt = 0;
    cnt = StringIsANumber(num0, &new_number);    
    if ((cnt != 3) || (new_number != 0))
    {
        printf("\nStringIs error with number %s ", num0);
        some_err = 1;
    }


    char num2 [] = "1111,99";
    new_number = GetValue(num2, ',');    
    if (new_number != 1111)
    {
        printf("\nGetValue error with number %s ", num2);
        some_err = 1;
    }

    if (some_err)
        PrintERR();
    else
        PrintOK();
    
}


void Test_Treatment_Module (void)
{
    int some_err = 0;    
    resp_t resp = resp_ok;
    
    printf("Testing Treatment Module: ");

    signal_type_t signal = SQUARE_SIGNAL;
    resp = TreatmentSetSignalType (signal);
    signal = TreatmentGetSignalType();
    if ((resp != resp_ok) || (signal != SQUARE_SIGNAL))
    {
        printf("\nTreatSignal error with %d ", signal);
        some_err = 1;
    }

    signal = TRIANGULAR_SIGNAL;
    resp = TreatmentSetSignalType (signal);
    signal = TreatmentGetSignalType();
    if ((resp != resp_ok) || (signal != TRIANGULAR_SIGNAL))
    {
        printf("\nTreatSignal error with %d ", signal);
        some_err = 1;
    }

    signal = SINUSOIDAL_SIGNAL;
    resp = TreatmentSetSignalType (signal);
    signal = TreatmentGetSignalType();
    if ((resp != resp_ok) || (signal != SINUSOIDAL_SIGNAL))
    {
        printf("\nTreatSignal error with %d ", signal);
        some_err = 1;
    }

    signal = 10;
    resp = TreatmentSetSignalType (signal);
    if (resp != resp_error)
    {
        printf("\nTreatSignal error with %d ", signal);
        some_err = 1;
    }

    unsigned char f_int = 10;
    unsigned char f_dec = 10;
    resp = TreatmentSetFrequency (f_int, f_dec);
    TreatmentGetFrequency (&f_int, &f_dec);
    if ((resp != resp_ok) || (f_int != 10) || (f_dec != 10))
    {
        printf("\nTreatFreq error with %d.%d ", f_int, f_dec);
        some_err = 1;
    }

    f_int = 100;
    f_dec = 10;
    resp = TreatmentSetFrequency (f_int, f_dec);
    if (resp != resp_error)
    {
        printf("\nTreatFreq error with %d.%d ", f_int, f_dec);
        some_err = 1;
    }

    unsigned char power = 100;
    resp = TreatmentSetPower (power);
    power = TreatmentGetPower ();

    if ((resp != resp_ok) || (power != 100))
    {
        printf("\nTreatPower error with %d ", power);
         some_err = 1;
    }

    power = 1;
    resp = TreatmentSetPower (power);
    power = TreatmentGetPower ();
    if ((resp != resp_ok) || (power != 10))
    {
        printf("\nTreatPower error with %d ", power);
        some_err = 1;
    }

    power = 120;
    resp = TreatmentSetPower (power);
    power = TreatmentGetPower ();
    if ((resp != resp_ok) || (power != 100))
    {
        printf("\nTreatPower error with %d ", power);
        some_err = 1;
    }

    unsigned char minutes = 100;
    unsigned short secs = 0;
    resp = TreatmentSetTimeinMinutes (minutes);
    secs = TreatmentGetTime ();
    if ((resp != resp_ok) || (secs != (minutes * 60)))
    {
        printf("\nTreatTime error with %d ", minutes);
        some_err = 1;
    }

    minutes = 121;
    resp = TreatmentSetTimeinMinutes (minutes);
    if (resp != resp_error)
    {
        printf("\nTreatTime error with %d ", minutes);
        some_err = 1;
    }

    updwn_e updwn = UPDWN_AUTO;
    TreatmentSetUpDwn (updwn);
    updwn = TreatmentGetUpDwn ();
    if (updwn != UPDWN_AUTO)
    {
        printf("\nTreatUpDwn error with %d ", updwn);
        some_err = 1;
    }

    updwn = UPDWN_MANUAL;
    TreatmentSetUpDwn (updwn);
    updwn = TreatmentGetUpDwn ();
    if (updwn != UPDWN_MANUAL)
    {
        printf("\nTreatUpDwn error with %d ", updwn);
        some_err = 1;
    }

    unsigned char channels_a = 0;
    unsigned char channels_b = 0;    
    channels_a |= ENABLE_CH1_FLAG | ENABLE_CH2_FLAG | ENABLE_CH3_FLAG;
    TreatmentSetChannelsFlag (channels_a);
    channels_b = TreatmentGetChannelsFlag ();
    if ((channels_a & 0x0f) != channels_b)
    {
        printf("\nTreatChannels error with setted: %d getted: %d ", channels_a, channels_b);
        some_err = 1;
    }

    channels_a = DISABLE_CH1_FLAG;
    TreatmentSetChannelsFlag (channels_a);
    channels_b = TreatmentGetChannelsFlag ();
    if (channels_b != ((ENABLE_CH2_FLAG | ENABLE_CH3_FLAG) & 0x0f))
    {
        printf("\nTreatChannels error with setted: %d getted: %d ", channels_a, channels_b);
        some_err = 1;
    }
    

    if (some_err)
        PrintERR();
    else
        PrintOK();
    
}



void Test_Functions (void)
{
    printf("tested ok!\n");
    PrintOK();
}


void PrintOK (void)
{
    printf("\033[0;32m");    //green
    printf("OK\n");
    printf("\033[0m");    //reset
}


void PrintERR (void)
{
    printf("\033[0;31m");    //red
    printf("ERR\n");
    printf("\033[0m");    //reset
}
//--- end of file ---//


