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
// #include "pwm.h"

#include "dac.h"

#include "comms_from_rasp.h"

#include "comms_channels.h"
#include "antennas.h"

#include "treatment.h"

#include <stdio.h>
#include <string.h>


// #define HIGH_LEFT_PWM_CH1(X)    TIM8_Update_CH3(X)
// #define LOW_RIGHT_PWM_CH1(X)    TIM8_Update_CH4(X)

// #define HIGH_LEFT_PWM_CH2(X)    TIM8_Update_CH2(X)
// #define LOW_RIGHT_PWM_CH2(X)    TIM8_Update_CH1(X)

// #define HIGH_LEFT_PWM_CH3(X)    TIM4_Update_CH2(X)
// #define LOW_RIGHT_PWM_CH3(X)    TIM4_Update_CH3(X)

// #define HIGH_LEFT_PWM_CH4(X)    TIM5_Update_CH1(X)
// #define LOW_RIGHT_PWM_CH4(X)    TIM5_Update_CH2(X)

// Externals -------------------------------------------------------------------
extern volatile unsigned short adc_ch [];
extern volatile unsigned short wait_ms_var;
extern volatile unsigned short timer_standby;
// extern volatile unsigned char usart3_have_data;


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------
void TF_Led (void);
void TF_Led_Dac (void);

// void TF_Usart1_Tx (void);
void TF_Usart1_Tx_String (void);

void TF_Adc_Usart1_Tx (void);
void TF_Adc_Usart1_Voltages (void);


// Module Functions ------------------------------------------------------------
void TF_Hardware_Tests (void)
{
    // TF_Led ();
    // TF_Led_Dac ();

    // TF_Usart1_Tx_String ();
    // TF_Adc_Usart1_Tx ();
    TF_Adc_Usart1_Voltages ();

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
    // ADC_START;

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


// void TF_Adc_Usart1_Voltages (void)
// {
//     char buff [100] = { 0 };
//     unsigned int seq_cnt = 0;

//     //-- Test ADC Multiple conversion Scanning Continuous Mode and DMA 
//     //-- DMA configuration.
//     DMAConfig();
//     DMA_ENABLE;
    
//     //Uso ADC con DMA
//     AdcConfig();
//     ADC_START;

//     Usart1Config ();
    
//     while (1)
//     {
//         if (!wait_ms_var)
//         {
//             int calc_int, calc_dec;

//             calc_int = Sense_200V * 3300;
//             calc_int = calc_int / 4054;
//             calc_dec = calc_int;
//             calc_int = calc_int / 10;
//             calc_dec = calc_dec - calc_int * 10;
            
//             sprintf(buff, "High supply: %d.%01d, ", calc_int, calc_dec);
//             Usart1Send (buff);

//             calc_int = Sense_15V * 3300;
//             // calc_int = calc_int / 5339;
//             calc_int = calc_int / 5310;    // ajusto decimales
//             calc_dec = calc_int;
//             calc_int = calc_int / 100;
//             calc_dec = calc_dec - calc_int * 100;
            
//             sprintf(buff, "Low supply: %d.%02d, ", calc_int, calc_dec);
//             Usart1Send (buff);

//             calc_int = Sense_12V * 3300;
//             calc_int = calc_int / 5339;
//             calc_dec = calc_int;
//             calc_int = calc_int / 100;
//             calc_dec = calc_dec - calc_int * 100;
//             sprintf(buff, "Twelve supply: %d.%02d, ", calc_int, calc_dec);
//             Usart1Send (buff);

//             sprintf(buff, "seq: %d\n", seq_cnt);
//             Usart1Send (buff);
            
//             wait_ms_var = 1000;
//             seq_cnt = 0;
//         }

//         if (sequence_ready)
//         {
//             seq_cnt++;
//             sequence_ready_reset;
//         }
//     }
//     //--- End Test ADC Multiple conversion Scanning Continuous Mode and DMA ----------------//        
// }


// void TF_Adc_Usart1_IS (void)
// {
//     char buff [100] = { 0 };
//     unsigned int seq_cnt = 0;

//     //-- Test ADC Multiple conversion Scanning Continuous Mode and DMA 
//     //-- DMA configuration.
//     DMAConfig();
//     DMA_ENABLE;
    
//     //Uso ADC con DMA
//     AdcConfig();
//     ADC_START;

//     Usart1Config ();
    
//     while (1)
//     {
//         if (!wait_ms_var)
//         {
//             sprintf(buff, "IS_CH1: %d IS_CH2: %d IS_CH3: %d IS_CH4: %d, seq: %d\n",
//                     IS_CH1,
//                     IS_CH2,
//                     IS_CH3,
//                     IS_CH4,                    
//                     seq_cnt);
            
//             Usart1Send (buff);
//             wait_ms_var = 1000;
//             seq_cnt = 0;
//         }

//         if (sequence_ready)
//         {
//             seq_cnt++;
//             sequence_ready_reset;
//         }
//     }
//     //--- End Test ADC Multiple conversion Scanning Continuous Mode and DMA ----------------//        
// }



//--- end of file ---//
