//----------------------------------------------------------
// #### MAGNET PROJECT - Custom Board ####
// ## Internal Test Functions Module
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TEST_FUNCTIONS.C ###################################
//----------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "test_functions.h"
#include "hard.h"
#include "stm32f10x.h"
#include "gpio.h"
#include "adc.h"
#include "usart.h"
#include "dma.h"
#include "tim.h"
#include "dac.h"

#include "treatment.h"

#include <stdio.h>
#include <string.h>


// Externals -------------------------------------------------------------------
extern volatile unsigned short adc_ch [];
extern volatile unsigned short wait_ms_var;
extern volatile unsigned short timer_standby;
// extern volatile unsigned char usart3_have_data;


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------
void TF_Led (void);
void TF_Led_Timers (void);
void TF_Led_Dac (void);

// void TF_Usart1_Tx (void);
void TF_Usart1_Tx_String (void);

void TF_Adc_Usart1_Tx (void);
void TF_Adc_Usart1_Voltages (void);


// Module Functions ------------------------------------------------------------
void TF_Hardware_Tests (void)
{
    // TF_Led ();
    TF_Led_Timers ();
    // TF_Led_Dac ();

    // TF_Usart1_Tx_String ();
    // TF_Adc_Usart1_Tx ();
    // TF_Adc_Usart1_Voltages ();

}


void TF_Led (void)
{
    while (1)
    {
        if (LED)
            LED_OFF;
        else
            LED_ON;
        
        Wait_ms(200);
    }
}


void TF_Led_Timers (void)
{
    int state = 0;

    while (1)
    {
        if (state == 0)
        {
            if (!timer_standby)
            {
                ChangeLed_With_Timer (LED_TREATMENT_STANDBY, 4000);
                timer_standby = 20000;
                state++;
            }
        }

        if (state == 1)
        {
            if (!timer_standby)
            {
                ChangeLed_With_Timer (LED_TREATMENT_STANDBY, 0);
                timer_standby = 20000;
                state++;
            }
        }

        if (state == 2)
        {
            if (!timer_standby)
            {
                ChangeLed (LED_TREATMENT_SINE_RUNNING);
                timer_standby = 20000;
                state++;
            }
        }

        if (state == 3)
        {
            if (!timer_standby)
            {
                ChangeLed (LED_TREATMENT_SQUARE_RUNNING);
                timer_standby = 20000;
                state = 0;
            }
        }

        UpdateLed();
    }
}


void TF_Led_Dac (void)
{
    DAC_Config ();
    
    while (1)
    {
        if (LED)
        {
            LED_OFF;
            DAC_Output1 (2047);
            DAC_Output2 (4095);
        }
        else
        {
            LED_ON;
            DAC_Output1 (4095);
            DAC_Output2 (2047);
        }
        
        Wait_ms(200);
    }
}





void TF_Usart1_Tx (void)
{
    Usart1Config();
    
    while (1)
    {
        unsigned char snd = 'M';
        Usart1SendUnsigned(&snd, 1);
        Wait_ms(100);
    }
}


void TF_Usart1_Tx_String (void)
{
    Usart1Config();
    
    while (1)
    {
        LED_ON;
        Usart1Send("Mariano\n");
        Wait_ms(10);
        LED_OFF;
        Wait_ms(1990);
    }
}


void TF_Adc_Usart1_Tx (void)
{
    char buff [100] = { 0 };

    //-- Test ADC Multiple conversion Scanning (starts each sequence) and DMA 
    //-- DMA configuration.
    DMAConfig();
    DMA_ENABLE;
    
    //Uso ADC con DMA
    AdcConfig();
    ADC_START;

    Usart1Config ();
    
    while (1)
    {
        for (int i = 0; i < ADC_CHANNEL_QUANTITY; i++)
            adc_ch[i] = 0;
        
        Wait_ms(1000);
        Usart1Send("starting conversion with channels in:\n");
        sprintf(buff, "%d %d %d %d %d %d\n",
                SENSE_POWER,
                SENSE_MEAS,
                V_SENSE_28V,
                V_SENSE_25V,
                V_SENSE_11V,
                V_SENSE_8V);
        
        Usart1Send (buff);

        LED_ON;
        ADC_START;
        Wait_ms(100);
        LED_OFF;
        
        Wait_ms(900);
        Usart1Send("conversion ended:\n");
        sprintf(buff, "%d %d %d %d %d %d\n",
                SENSE_POWER,
                SENSE_MEAS,
                V_SENSE_28V,
                V_SENSE_25V,
                V_SENSE_11V,
                V_SENSE_8V);
        
        Usart1Send (buff);
        
    }
    //--- End Test ADC Multiple conversion Scanning Continuous Mode and DMA ----------------//        
}


void TF_Adc_Usart1_Voltages (void)
{
    char buff [100] = { 0 };

    //-- Test ADC Multiple conversion Scanning (starts each sequence) and DMA 
    //-- DMA configuration.
    DMAConfig();
    DMA_ENABLE;
    
    //Uso ADC con DMA
    AdcConfig();
    // ADC_START;

    Usart1Config ();

    int calc_int, calc_dec;
    while (1)
    {
        for (int i = 0; i < ADC_CHANNEL_QUANTITY; i++)
            adc_ch[i] = 0;
        
        Wait_ms(1000);
        Usart1Send("starting conversion with channels in:\n");
        sprintf(buff, "%d %d %d %d %d %d\n",
                SENSE_POWER,
                SENSE_MEAS,
                V_SENSE_28V,
                V_SENSE_25V,
                V_SENSE_11V,
                V_SENSE_8V);
        
        Usart1Send (buff);

        LED_ON;
        ADC_START;
        Wait_ms(100);
        LED_OFF;
        
        Wait_ms(900);
        Usart1Send("conversion ended:\n");
        sprintf(buff, "%d %d %d %d %d %d\n",
                SENSE_POWER,
                SENSE_MEAS,
                V_SENSE_28V,
                V_SENSE_25V,
                V_SENSE_11V,
                V_SENSE_8V);

        // SENSE_POWER resistor multiplier 11
        calc_int = SENSE_POWER * 330 * 11;
        calc_int >>= 12;
        calc_dec = calc_int;
        calc_int = calc_int / 100;
        calc_dec = calc_dec - calc_int * 100;
        sprintf(buff, "Power: %d.%02d, ", calc_int, calc_dec);
        Usart1Send (buff);

        // SENSE_MEAS resistor multiplier 2
        calc_int = SENSE_MEAS * 330 * 2;
        calc_int >>= 12;
        calc_dec = calc_int;
        calc_int = calc_int / 100;
        calc_dec = calc_dec - calc_int * 100;
        sprintf(buff, "Meas: %d.%02d, ", calc_int, calc_dec);
        Usart1Send (buff);

        // V_SENSE_28V resistor multiplier 11
        calc_int = V_SENSE_28V * 330 * 11;
        calc_int >>= 12;
        calc_dec = calc_int;
        calc_int = calc_int / 100;
        calc_dec = calc_dec - calc_int * 100;
        sprintf(buff, "V28V: %d.%02d, ", calc_int, calc_dec);
        Usart1Send (buff);

        // V_SENSE_25V resistor multiplier 11
        calc_int = V_SENSE_25V * 330 * 11;
        calc_int >>= 12;
        calc_dec = calc_int;
        calc_int = calc_int / 100;
        calc_dec = calc_dec - calc_int * 100;
        sprintf(buff, "V25V: %d.%02d, ", calc_int, calc_dec);
        Usart1Send (buff);

        // V_SENSE_11V resistor multiplier 11
        calc_int = V_SENSE_11V * 330 * 11;
        calc_int >>= 12;
        calc_dec = calc_int;
        calc_int = calc_int / 100;
        calc_dec = calc_dec - calc_int * 100;
        sprintf(buff, "V11V: %d.%02d, ", calc_int, calc_dec);
        Usart1Send (buff);

        // V_SENSE_8V resistor multiplier 11
        calc_int = V_SENSE_8V * 330 * 11;
        calc_int >>= 12;
        calc_dec = calc_int;
        calc_int = calc_int / 100;
        calc_dec = calc_dec - calc_int * 100;
        sprintf(buff, "V8V: %d.%02d\n", calc_int, calc_dec);
        Usart1Send (buff);
                
    }
    //--- End Test ADC Multiple conversion Scanning Continuous Mode and DMA ----------------//        
}


//--- end of file ---//
