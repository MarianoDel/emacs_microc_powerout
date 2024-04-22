//---------------------------------------------------------------
// #### PROJECT MICRO-CURRENTS POWER-OUT F103 - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### MAIN.C ##################################################
//---------------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "stm32f10x.h"
#include "hard.h"

#include "adc.h"
#include "dma.h"
#include "dac.h"
#include "tim.h"
#include "gpio.h"
#include "usart.h"

#include "comms.h"
#include "test_functions.h"
#include "treatment.h"
#include "comms_probe.h"
// #include "parameters.h"


#include <stdio.h>
#include <string.h>


// Private Types Constants and Macros ------------------------------------------


// Externals -------------------------------------------------------------------
//--- Externals from timers
volatile unsigned short timer_standby = 0;
volatile unsigned short wait_ms_var = 0;

//--- Externals from adc
volatile unsigned short adc_ch [ADC_CHANNEL_QUANTITY];


// Globals ---------------------------------------------------------------------
// volatile unsigned short timer_sync_xxx_ms = 0;
// volatile unsigned short timer_out4 = 0;

// parameters_typedef * pmem = (parameters_typedef *) (unsigned int *) FLASH_PAGE_FOR_BKP;	//en flash
// parameters_typedef mem_conf;


// Module Private Functions ----------------------------------------------------
void TimingDelay_Decrement(void);
void SysTickError (void);

// #define RPI_Flush_Comms (comms_messages_rpi &= ~COMM_RPI_ALL_MSG_MASK)


// Module Functions ------------------------------------------------------------
int main (void)
{
    // Gpio Configuration.
    GpioInit();
    
    // Systick Timer Activation
    if (SysTick_Config(64000))
        SysTickError();

    // Hardware Tests
    // TF_Hardware_Tests ();

    // --- main program inits. ---
    //-- DMA configuration.
    DMAConfig();
    DMA_ENABLE;
    
    //-- ADC with DMA
    AdcConfig();
    // ADC_START;

    //-- DAC init for signal generation
    DAC_Config ();
    DAC_Output1(1400);
    
    //-- Comms with rasp
    Usart1Config ();

    //-- Comms with probes
    Usart3Config ();

    //-- TIM1 for signals module sequence ready
    TIM6_Init();
    TIM7_Init();
    

    //-- Main Loop --------------------------
    while (1)
    {
        // // update the antennas module states
        // AntennaUpdateStates ();

        // update the channels comms
        Comms_Update ();

        // update the probe comms
        Probe_Comms_Update ();

        // update treatment state
        Treatment_Manager();

        // the update of led and buzzer on Treatment_Manager()
        UpdateLed();

    }
}

//--- End of Main ---//


// Other Module Functions ------------------------------------------------------
// extern void TF_Prot_Int_Handler (unsigned char ch);
// void EXTI2_IRQHandler (void)
// {
//     if(EXTI->PR & EXTI_PR_PR2)    //Line2
//     {
//         Signals_Overcurrent_Handler (CH3);
//         // TF_Prot_Int_Handler (3);    // PROT_CH3 for tests
//         EXTI->PR |= EXTI_PR_PR2;
//     }
// }


// void EXTI4_IRQHandler (void)
// {
//     if(EXTI->PR & EXTI_PR_PR4)    //Line4
//     {
//         Signals_Overcurrent_Handler (CH4);        
//         // TF_Prot_Int_Handler (4);    // PROT_CH4 for tests
//         EXTI->PR |= EXTI_PR_PR4;
//     }
// }


// void EXTI15_10_IRQHandler (void)
// {
//     if(EXTI->PR & EXTI_PR_PR13)    //Line13
//     {
//         Signals_Overcurrent_Handler (CH2);
//         // TF_Prot_Int_Handler (2);    // PROT_CH2 for tests
//         EXTI->PR |= EXTI_PR_PR13;
//     }
//     else if (EXTI->PR & EXTI_PR_PR15)    //Line15
//     {
//         Signals_Overcurrent_Handler (CH1);        
//         // TF_Prot_Int_Handler (1);    // PROT_CH1 for tests
//         EXTI->PR |= EXTI_PR_PR15;
//     }
// }


void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    if (timer_standby)
        timer_standby--;

    Comms_Probe_Timeout ();

    // Treatment_Timeouts ();
    
    HARD_Timeouts();
    
}


void SysTickError (void)
{
    //Capture systick error...
    while (1)
    {
        if (LED)
            LED_OFF;
        else
            LED_ON;

        for (unsigned char i = 0; i < 255; i++)
        {
            asm ("nop \n\t"
                 "nop \n\t"
                 "nop \n\t" );
        }
    }
}

//--- end of file ---//

