//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "tamper_funcs.h"

// helper modules
#include "tests_ok.h"
#include "tests_mock_usart.h"

#include <stdio.h>
// #include <math.h>

// Types Constants and Macros --------------------------------------------------


// Externals - Globals on module -----------------------------------------------


// Globals - Externals on module -----------------------------------------------


// Globals - for this test module ----------------------------------------------
int flash_save_ok = 1;
int tamper_pin_value = 0;
int vbat_value = 0;
parameters_typedef mem_conf;


// Module Functions to Test ----------------------------------------------------
void Test_CodeToString (void);
void Test_GetStatus (void);
void Test_SetStatus (void);


// Module Auxiliary Functions --------------------------------------------------


// Tests Module Auxiliary or General Functions ---------------------------------


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{

    Test_CodeToString ();
    
    Test_GetStatus ();

    Test_SetStatus ();    

    return 0;
}


void Test_CodeToString (void)
{
    printf("\n-- Testing Tamper CodeToString --\n");

    for (int i = 0; i < 6; i++)
    {
        char * str;
        str = Tamper_CodeToString (i);
        printf("code: %d string: %s\n", i, str);
    }

    printf("Testing CodeToString: ");
    PrintOK();
    
}


void Test_GetStatus (void)
{
    printf("\n-- Testing Tamper GetStatus --\n");

    int some_err = 0;
    tamper_state_e t = TAMPER_DISABLE;
    
    if (!some_err)
    {
        mem_conf.tamper_config = TAMPER_DISABLE;
        t = Tamper_GetStatus(&mem_conf);
        if (t != TAMPER_DISABLE)
            some_err = 1;        
    }

    if (!some_err)
    {
        mem_conf.tamper_config = TAMPER_ENABLE;
        tamper_pin_value = 0;
        vbat_value = 3000;        
        t = Tamper_GetStatus(&mem_conf);
        if (t != TAMPER_ENABLE_NO_TAMPER)
            some_err = 1;        
        
    }

    if (!some_err)
    {
        mem_conf.tamper_config = TAMPER_ENABLE;
        tamper_pin_value = 1;
        vbat_value = 3000;        
        t = Tamper_GetStatus(&mem_conf);
        if (t != TAMPER_ENABLE_TAMPERED)
            some_err = 1;        
        
    }

    if (!some_err)
    {
        mem_conf.tamper_config = TAMPER_ENABLE;
        tamper_pin_value = 1;
        vbat_value = 2047;
        t = Tamper_GetStatus(&mem_conf);
        if (t != TAMPER_ENABLE_LOW_BAT)
            some_err = 1;        
        
    }
    
    printf("Testing GetStatus: ");
    if (!some_err)
    {
        PrintOK();    
    }
    else
    {
        PrintERR();
        char * str;
        str = Tamper_CodeToString (t);
        printf("code: %d string: %s\n", t, str);
    
    }
    
}


void Test_SetStatus (void)
{
    printf("\n-- Testing Tamper SetStatus --\n");

    int some_err = 0;
    tamper_state_e t = TAMPER_DISABLE;
    
    if (!some_err)
    {
        mem_conf.tamper_config = TAMPER_DISABLE;
        t = Tamper_SetStatus(&mem_conf, TAMPER_ENABLE_LOW_BAT);
        if (t != TAMPER_ERROR)
        {
            some_err = 1;
            printf("  tamper set error\n");
        }
    }

    if (!some_err)
    {
        mem_conf.tamper_config = TAMPER_ENABLE;
        flash_save_ok = 0;
        tamper_pin_value = 0;
        vbat_value = 3000;        
        t = Tamper_SetStatus(&mem_conf, TAMPER_DISABLE);
        if ((t != TAMPER_ERROR) ||
            (mem_conf.tamper_config != TAMPER_DISABLE))
        {
            some_err = 1;
            printf("  tamper set disable mem error\n");
        }
    }

    if (!some_err)
    {
        mem_conf.tamper_config = TAMPER_DISABLE;
        flash_save_ok = 1;        
        tamper_pin_value = 0;
        vbat_value = 3000;        
        t = Tamper_SetStatus(&mem_conf, TAMPER_DISABLE);
        if ((t != TAMPER_DISABLE) ||
            (mem_conf.tamper_config != TAMPER_DISABLE))
        {
            some_err = 1;
            printf("  tamper set disable mem ok\n");
        }
    }
    
    if (!some_err)
    {
        mem_conf.tamper_config = TAMPER_DISABLE;
        tamper_pin_value = 0;
        vbat_value = 3000;        
        t = Tamper_SetStatus(&mem_conf, TAMPER_ENABLE);
        if ((t != TAMPER_ENABLE_NO_TAMPER) ||
            (mem_conf.tamper_config != TAMPER_ENABLE))
        {
            some_err = 1;
            printf("  tamper set enable no tamper\n");
        }
    }

    if (!some_err)
    {
        mem_conf.tamper_config = TAMPER_DISABLE;
        tamper_pin_value = 1;
        vbat_value = 3000;        
        t = Tamper_SetStatus(&mem_conf, TAMPER_ENABLE);
        if ((t != TAMPER_ENABLE_TAMPERED) ||
            (mem_conf.tamper_config != TAMPER_ENABLE))
        {
            some_err = 1;
            printf("  tamper set enable tampered\n");
        }
    }
    
    printf("Testing SetStatus: ");
    if (!some_err)
    {
        PrintOK();    
    }
    else
    {
        PrintERR();
        char * str;
        str = Tamper_CodeToString (t);
        printf("code: %d string: %s\n", t, str);
    
    }
    
}




// Module Mocked Functions -----------------------------------------------------
unsigned char Flash_WriteConfigurations (void)
{
    if (flash_save_ok)
    {
        printf("-- write conf ok!\n");        
        return 1;
    }
    else
    {
        printf("-- write conf with errors!\n");
        return 0;
    }
}


unsigned char Tamper_Pin (void)
{
    return tamper_pin_value;
}


unsigned short AdcGetVBat (void)
{
    return vbat_value;
}

void Pb14_To_Output(void)
{
    printf("PB14 to output\n");
}


void Pb14_Off(void)
{
    printf("PB14 to off\n");
}


void Pb14_On(void)
{
    printf("PB14 to on\n");
}


void Pb14_To_Input(void)
{
    printf("PB14 to input\n");
}


void Wait_ms (unsigned short number)
{
    printf("wait for %d millis\n", number);
}


//--- end of file ---//


