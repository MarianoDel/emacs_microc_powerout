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
#include "hard.h"
#include "stm32f10x.h"


// #include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------
void Timer_Polarity_Gpios_To_Push_Pull (void);
void Timer_Polarity_Gpios_To_Alternative (void);


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


//////////////////////////////////
// Functions for Polarity Timer //
//////////////////////////////////
void Timer_Polarity (polarity_e new_polarity)
{
    if (new_polarity == POLARITY_ALT)
    {
        Timer_Polarity_Gpios_To_Alternative ();
        // set timer and start
        // freq = 0.3Hz psc: 6399 arr: 33333 arr_4: 8333
        TIM8_Init();
        // TIM8_Change(6399, 16667);    //NOT NEEDED all in init
    }
    else if (new_polarity == POLARITY_NEG)
    {
        Timer_Polarity_Always_Right ();
    }
    else    // must be POLARITY_POS
    {
        Timer_Polarity_Always_Left ();
    }
}


void Timer_Polarity_Always_Right (void)
{
    TIM8_Stop ();

    // revert gpios
    Timer_Polarity_Gpios_To_Push_Pull ();
    LEFT_OFF;
    RIGHT_ON;
}


void Timer_Polarity_Always_Left (void)
{
    TIM8_Stop ();

    // revert gpios
    Timer_Polarity_Gpios_To_Push_Pull ();
    LEFT_ON;
    RIGHT_OFF;    
}


void Timer_Polarity_Gpios_To_Push_Pull (void)
{
    unsigned long temp = 0;
    
    // PC7 to push pull output
    temp = GPIOC->CRL;
    temp &= 0x0FFFFFFF;
    temp |= 0x20000000;
    GPIOC->CRL = temp;

    //PB0 to push pull output
    temp = GPIOB->CRL;
    temp &= 0xFFFFFFF0;
    temp |= 0x00000002;
    GPIOB->CRL = temp;    
}


void Timer_Polarity_Gpios_To_Alternative (void)
{
    unsigned long temp = 0;
    
    // PC7 alternative TIM8_CH2
    temp = GPIOC->CRL;
    temp &= 0x0FFFFFFF;
    temp |= 0xA0000000;
    GPIOC->CRL = temp;

    // PB0 alternative TIM8_CH2N
    temp = GPIOB->CRL;
    temp &= 0xFFFFFFF0;
    temp |= 0x0000000A;
    GPIOB->CRL = temp;    
}
//--- end of file ---//
