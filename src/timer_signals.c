//------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TIMER_SIGNALS.C ##########################
//------------------------------------------------

// Includes --------------------------------------------------------------------
#include "timer_signals.h"
#include "tim.h"


// #include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
////////////////////////////////
// Functions for Square Timer //
////////////////////////////////
void Timer_Square_Set_Registers (unsigned short new_psc, unsigned short new_arr)
{
    TIM6_Change (new_psc, new_arr);
}


unsigned char Timer_Square_Signal_Ended (void)
{
    return TIM6_UIF_Get_Flag();
}

    
void Timer_Square_Signal_Reset (void)
{
    TIM6_UIF_Reset();
}


void Timer_Square_Signal_Stop (void)
{
    TIM6_Stop();
}

// void Timer_Square_Signal_Set (void)
// {
//     TIM6_UIF_Set();
// }


//////////////////////////////
// Functions for Sine Timer //
//////////////////////////////
// frequency of sinusoidal its fixed for all configurations
void Timer_Sine_Set_Registers (unsigned short new_psc, unsigned short new_arr)
{
    TIM7_Change (new_psc, new_arr);
}


unsigned char Timer_Sine_Signal_Ended (void)
{
    return TIM7_UIF_Get_Flag();
}

    
void Timer_Sine_Signal_Reset (void)
{
    TIM7_UIF_Reset();
}


void Timer_Sine_Signal_Stop (void)
{
    TIM7_Stop();
}


// void Timer_Sine_Signal_Set (void)
// {
//     TIM7_UIF_Set();
// }


//--- end of file ---//
