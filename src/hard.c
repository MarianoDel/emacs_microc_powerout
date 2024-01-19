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


// Module Private Types Constants and Macros -----------------------------------
typedef enum
{    
    START_BLINKING = 0,
    WAIT_TO_OFF,
    WAIT_TO_ON,
    WAIT_NEW_CYCLE
} led_state_t;


#define LED_DEFAULT_TIMER_OFF    2000
// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
// for the LEDs
led_state_t led_state = START_BLINKING;
unsigned char blink = 0;
unsigned char how_many_blinks = 0;
unsigned short led_space_in_off = 0;
volatile unsigned short timer_led = 0;



// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
// called from int timer on 1ms ticks
void HARD_Timeouts (void)
{
    if (timer_led)
        timer_led--;

}


// change bips LED config and timer in off
void ChangeLed_With_Timer (unsigned char how_many, unsigned short led_timer_off)
{
    how_many_blinks = how_many;
    led_state = START_BLINKING;

    if (led_timer_off)
        led_space_in_off = led_timer_off;
    else
        led_space_in_off = LED_DEFAULT_TIMER_OFF;

}


// change bips LED 
void ChangeLed (unsigned char how_many)
{
    how_many_blinks = how_many;
    led_state = START_BLINKING;
}


// update the LED routine
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
                    timer_led = led_space_in_off;
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


unsigned char Led_Is_On (void)
{
    return LED;
}


void Led_On (void)
{
    LED_ON;
}


void Led_Off (void)
{
    LED_OFF;
}

//--- end of file ---//
