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
typedef enum {
    TREATMENT_STANDBY = 0,
    TREATMENT_SQUARE_RUNNING,
    TREATMENT_SINUS_RUNNING,
    TREATMENT_STOPPING

} treatment_e;



// Externals -------------------------------------------------------------------
extern treatment_conf_t treatment_conf;
extern treatment_e treat_state;


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

    Test_Treatment_Manager ();
        
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

    if (!some_err)
    {
        for (int i = 0; i < 1000; i++)
        {
            int intensity = i;

            sprintf(test_str, "%duA\r\n", intensity);
            resp = Treatment_SetIntensity_Str (test_str);
            if ((resp != resp_ok) ||
                (treatment_conf.intensity != intensity))
            {
                printf("\nintensity error: %s ", test_str);
                some_err = 1;
                break;
            }
        }
    }

    if (!some_err)
    {
        strcpy(test_str, "pos\r\n");
        resp = Treatment_SetPolarity_Str (test_str);
        if ((resp != resp_ok) ||
            (treatment_conf.polarity != 0))
        {
            printf("\npolarity error: %s ", test_str);
            some_err = 1;
        }
    }

    if (!some_err)
    {
        strcpy(test_str, "neg\r\n");
        resp = Treatment_SetPolarity_Str (test_str);
        if ((resp != resp_ok) ||
            (treatment_conf.polarity != 1))
        {
            printf("\npolarity error: %s ", test_str);
            some_err = 1;
        }
    }

    if (!some_err)
    {
        strcpy(test_str, "alt\r\n");
        resp = Treatment_SetPolarity_Str (test_str);
        if ((resp != resp_ok) ||
            (treatment_conf.polarity != 2))
        {
            printf("\npolarity error: %s ", test_str);
            some_err = 1;
        }
    }
    
    if (!some_err)
    {
        strcpy(test_str, "square\r\n");
        resp = Treatment_SetMode_Str (test_str);
        if ((resp != resp_ok) ||
            (treatment_conf.mode != 0))
        {
            printf("\nmode error: %s ", test_str);
            some_err = 1;
        }
    }

    if (!some_err)
    {
        strcpy(test_str, "sinus\r\n");
        resp = Treatment_SetMode_Str (test_str);
        if ((resp != resp_ok) ||
            (treatment_conf.mode != 1))
        {
            printf("\nmode error: %s ", test_str);
            some_err = 1;
        }
    }

    if (!some_err)
    {
        for (int i = 0; i <= 100; i++)
        {
            sprintf(test_str, "%d\r\n", i);
            resp = Treatment_SetThreshold_Str (test_str);
            if ((resp != resp_ok) ||
                (treatment_conf.threshold != i))
            {
                printf("\nthreshold error: %s ", test_str);
                some_err = 1;
                break;
            }
        }
    }

    if (!some_err)
    {
        Treatment_Start ();        
        if (Treatment_Start_Flag() != 1)
        {
            printf("\nstart flag error!");
            some_err = 1;            
        }

        Treatment_Start_Flag_Reset();
        if (Treatment_Start_Flag() != 0)
        {
            printf("\nstart flag error!");
            some_err = 1;            
        }        
    }

    if (!some_err)
    {
        Treatment_Stop ();
        if (Treatment_Stop_Flag() != 1)
        {
            printf("\nstop flag error!");
            some_err = 1;            
        }

        Treatment_Stop_Flag_Reset();
        if (Treatment_Stop_Flag() != 0)
        {
            printf("\nstop flag error!");
            some_err = 1;            
        }
    }
    
    if (!some_err)
    {
        for (int i = 0; i <= 100; i++)
        {
            sprintf(test_str, "%d\r\n", i);
            resp = Treatment_SetGain_Str (test_str);
            if ((resp != resp_ok) ||
                (treatment_conf.gain != i))
            {
                printf("\ngain error: %s ", test_str);
                some_err = 1;
                break;
            }
        }

        if (Treatment_GetGain() != 100)
        {
            printf("\nget gain error");
            some_err = 1;
        }
    }
    
    printf("Testing Treatment Module Settings: ");
    if (some_err)
        PrintERR();
    else
        PrintOK();
    
}


void Test_Treatment_Manager (void)
{
    int some_err = 0;    
    resp_e resp = resp_ok;

    printf("\n-- Testing Treatment Manager --\n");
    printf("-- treat in standby\n");
    for (int i = 0; i < 20; i++)
        Treatment_Manager();

    if (treat_state != TREATMENT_STANDBY)
        some_err = 1;

    if (!some_err)
    {
        printf("-- treat to running on square\n");
        Treatment_SetMode_Str("square");
        Treatment_Start();
        
        for (int i = 0; i < 20; i++)
            Treatment_Manager();

        if (treat_state != TREATMENT_SQUARE_RUNNING)
            some_err = 1;
        
    }

    if (!some_err)        
    {
        printf("-- treat to stop from square\n");
        Treatment_Stop();
        
        Treatment_Manager();
        if (treat_state != TREATMENT_STOPPING)
            some_err = 1;

        if (!some_err)
        {
            for (int i = 0; i < 20; i++)
                Treatment_Manager();            
            
            if (treat_state != TREATMENT_STANDBY)
                some_err = 1;
        }        
    }

    if (!some_err)
    {
        printf("-- treat to running on sinus\n");
        Treatment_SetMode_Str("sinus");
        Treatment_Start();
        
        for (int i = 0; i < 20; i++)
            Treatment_Manager();

        if (treat_state != TREATMENT_SINUS_RUNNING)
            some_err = 1;
        
    }

    if (!some_err)        
    {
        printf("-- treat to stop from sinus\n");
        Treatment_Stop();
        
        Treatment_Manager();
        if (treat_state != TREATMENT_STOPPING)
            some_err = 1;

        if (!some_err)
        {
            for (int i = 0; i < 20; i++)
                Treatment_Manager();            
            
            if (treat_state != TREATMENT_STANDBY)
                some_err = 1;
        }        
    }
    
    printf("Testing Treatment Manager: ");
    if (some_err)
        PrintERR();
    else
        PrintOK();
    
}


// Module Mocked Functions -----------------------------------------------------
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


