//-------------------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TAMPER_FUNCS.C ##################################
//-------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "tamper_funcs.h"
#include "hard.h"
#include "tim.h"
#include "parameters.h"
#include "flash_program.h"

// #include <string.h>
// #include <stdio.h>



// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
char * tamper_string[6] = {"TAMPER_DISABLE",
                           "TAMPER_ENABLE",
                           "TAMPER_ENABLE_TAMPERED",
                           "TAMPER_ENABLE_NO_TAMPER",
                           "TAMPER_ENABLE_LOW_BAT",
                           "TAMPER_ERROR"};


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
tamper_state_e Tamper_GetStatus (parameters_typedef * conf)
{
    // check mem_conf first
    // -- active by default
    if (conf->tamper_config == TAMPER_DISABLE)
        return TAMPER_DISABLE;

    // check vbat second
    // -- vbat is good by default -- no vbat sensor on 103RC
    // if ((AdcGetVBat() < 2048) &&
    //     (Tamper_Pin()))
    //     return TAMPER_ENABLE_LOW_BAT;
    
    // check tamper_pin
    if (Tamper_Pin())
        return TAMPER_ENABLE_TAMPERED;
    else
        return TAMPER_ENABLE_NO_TAMPER;
    
}


tamper_state_e Tamper_SetStatus (parameters_typedef * conf, tamper_state_e new_state)
{
    tamper_state_e t = TAMPER_ERROR;
    
    // check validity
    if ((new_state != TAMPER_ENABLE) &&
        (new_state != TAMPER_DISABLE))
        return TAMPER_ERROR;
    
    // check the new state on conf
    if (conf->tamper_config != new_state)
    {
        // save new config on mem_conf
        conf->tamper_config = new_state;
        // Flash_WriteConfigurations ();

        if (Flash_WriteConfigurations () != 0)
            return TAMPER_ERROR;
        
    }
    
    if (new_state == TAMPER_DISABLE)
        t = TAMPER_DISABLE;

    if (new_state == TAMPER_ENABLE)
    {
        Pb14_To_Output();
        Pb14_Off();
        Wait_ms(1);
        Pb14_On();
        Pb14_To_Input();

        if (!Tamper_Pin())
            t = TAMPER_ENABLE_NO_TAMPER;
        else
            t = TAMPER_ENABLE_TAMPERED;
    }

    return t;
}


char * Tamper_CodeToString (tamper_state_e code)
{
    return tamper_string[code];
}

//---- end of file ----//
