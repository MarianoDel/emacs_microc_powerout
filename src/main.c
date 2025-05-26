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
#include "meas.h"
#include "signals.h"


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
    // Init ADC with DMA    
    //-- DMA configuration.
    DMAConfig();
    DMA_ENABLE;
    
    //-- ADC with DMA
    AdcConfig();
    AdcStart();

    //-- DAC init for signal generation
    DAC_Config ();
    DAC_Output1(1400);
    DAC_Output2(0);    
    
    //-- Comms with main uc
    Usart1Config ();

    //-- Comms with probes
    Usart3Config ();

    //-- TIM1 for signals module sequence ready
    TIM6_Init();
    TIM7_Init();
    
    //-- Activate meas module
    // Meas_Square_Init ();
    Meas_Module_Init ();

    
    //-- Main Loop --------------------------
    while (1)
    {

        // update comms with main
        Comms_Update ();

        // update the probe comms
        Probe_Comms_Update ();

        // update treatment state
        Treatment_Manager();

        // the update of led and buzzer on Treatment_Manager()
        // UpdateLed();

	// meas always running
	Meas_Module_Update ();

    }
}

//--- End of Main ---//


// Other Module Functions ------------------------------------------------------
void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    if (timer_standby)
        timer_standby--;

    Comms_Probe_Timeout ();

    // Meas_Timeout ();

    Treatment_Timeouts ();
    
    HARD_Timeouts();

    Signals_Timeouts ();
    
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

