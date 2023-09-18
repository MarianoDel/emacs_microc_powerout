//---------------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### ERRORS.C ##########################################
//---------------------------------------------------------
#include "errors.h"

#include <stdio.h>


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
unsigned char signals_errors_flags[4] = { 0 };


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
void Error_SetString (char * p_errstr, unsigned short err_num)
{
    sprintf(p_errstr, "ERROR(0x%03X)\r\n", err_num);
}


void Error_SetStatus(unsigned char error, unsigned char ch)
{
    if (ch < 4)
    {
        signals_errors_flags[ch] &= 0xF0;
        signals_errors_flags[ch] |= error & 0x0F;
    }
}


unsigned char Error_GetStatus(unsigned char ch)
{
    unsigned char error = 0;
    
    if (ch < 4)
        error = signals_errors_flags[ch];

    return error;
}


void Error_SetStatus_For_Checks (unsigned char error, unsigned char ch)
{
    if (ch < 4)
        signals_errors_flags[ch] = error & 0xF0;
    
}
// void Error_SetString_FromStatus (char * p_errstr, unsigned char status_error)
// {
//     if (!status_error)
//         return;

    
//     sprintf(p_errstr, "ERROR(0x%03X)\r\n", err_num);
// }

//--- end of file ---//

