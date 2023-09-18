//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### HARD.C #################################
//---------------------------------------------
#include "hard.h"
#include "stm32f10x.h"


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
// for the LEDs
led_state_t led_state = START_BLINKING;
unsigned char blink = 0;
unsigned char how_many_blinks = 0;
volatile unsigned short timer_led = 0;



// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
// called from int timer on 1ms ticks
void HARD_Timeouts (void)
{
    if (timer_led)
        timer_led--;

}

//cambia configuracion de bips del LED
void ChangeLed (unsigned char how_many)
{
    how_many_blinks = how_many;
    led_state = START_BLINKING;
}

//mueve el LED segun el estado del Programa
void UpdateLed (void)
{
    switch (led_state)
    {
        case START_BLINKING:
            blink = how_many_blinks;
            
            if (blink)
            {
                LED_ON;
                timer_led = 200;
                led_state++;
                blink--;
            }
            break;

        case WAIT_TO_OFF:
            if (!timer_led)
            {
                LED_OFF;
                timer_led = 200;
                led_state++;
            }
            break;

        case WAIT_TO_ON:
            if (!timer_led)
            {
                if (blink)
                {
                    blink--;
                    timer_led = 200;
                    led_state = WAIT_TO_OFF;
                    LED_ON;
                }
                else
                {
                    led_state = WAIT_NEW_CYCLE;
                    timer_led = 2000;
                }
            }
            break;

        case WAIT_NEW_CYCLE:
            if (!timer_led)
                led_state = START_BLINKING;

            break;

        default:
            led_state = START_BLINKING;
            break;
    }
}


// void HARD_L1_ON (void)
// {
//     LED1_ON;
// }


// void HARD_L1_OFF (void)
// {
//     LED1_OFF;
// }

// unsigned char Led1_Is_On (void)
// {
//     return LED1;
// }


// void Led1_On (void)
// {
//     LED1_ON;
// }


// void Led1_Off (void)
// {
//     LED1_OFF;
// }


// unsigned char Tamper_Pin (void)
// {
//     return TAMPER_PIN;
// }


void Pb14_To_Output (void)
{
    unsigned long temp;

    temp = GPIOB->CRH;
    temp &= 0xF0FFFFFF;
    temp |= 0x02000000;
    GPIOB->CRH = temp;
    
}


void Pb14_To_Input (void)
{
    unsigned long temp;

    temp = GPIOB->CRH;
    temp &= 0xF0FFFFFF;
    temp |= 0x04000000;
    GPIOB->CRH = temp;
    
}


// void Pb14_Off (void)
// {
//     PB14_OFF;
// }


// void Pb14_On (void)
// {
//     PB14_ON;
// }


//--- end of file ---//
