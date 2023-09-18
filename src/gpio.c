//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### GPIO.C ################################
//---------------------------------------------

// Includes --------------------------------------------------------------------
#include "stm32f10x.h"
#include "gpio.h"
#include "hard.h"


// Module Private Types Constants and Macros -----------------------------------
#define USE_EXTI_LINES


#define RCC_GPIOA_CLK    (RCC->APB2ENR & 0x00000004)
#define RCC_GPIOA_CLKEN    (RCC->APB2ENR |= 0x00000004)
#define RCC_GPIOA_CLKDIS    (RCC->APB2ENR &= ~0x00000004)

#define RCC_GPIOB_CLK    (RCC->APB2ENR & 0x00000008)
#define RCC_GPIOB_CLKEN    (RCC->APB2ENR |= 0x00000008)
#define RCC_GPIOB_CLKDIS    (RCC->APB2ENR &= ~0x00000008)

#define RCC_GPIOC_CLK    (RCC->APB2ENR & 0x00000010)
#define RCC_GPIOC_CLKEN    (RCC->APB2ENR |= 0x00000010)
#define RCC_GPIOC_CLKDIS    (RCC->APB2ENR &= ~0x00000010)

#define RCC_GPIOD_CLK    (RCC->APB2ENR & 0x00000020)
#define RCC_GPIOD_CLKEN    (RCC->APB2ENR |= 0x00000020)
#define RCC_GPIOD_CLKDIS    (RCC->APB2ENR &= ~0x00000020)

#define RCC_AFIO_CLK    (RCC->APB2ENR & 0x00000001)
#define RCC_AFIO_CLKEN    (RCC->APB2ENR |= 0x00000001)
#define RCC_AFIO_CLKDIS    (RCC->APB2ENR &= ~0x00000001)


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Functions ------------------------------------------------------------


//--- Tamper config ---//
void Tamper_Config(void)
{
	unsigned long temp;

	//--- GPIOB ---//
	//--- Clock ---//
	if (!(RCC->APB2ENR & 0x00000008))
		RCC->APB2ENR |= 0x00000008;

	//--- Config pines ---//
	temp = GPIOB->CRH;
	temp &= 0xFF0FFFFF;
	temp |= 0x00800000;
	GPIOB->CRH = temp;
	GPIOB->BSRR = 0x00002000;
}





//------- GPIO REGISTERS ----------//
//
//	GPIOx->CRL	pin 7 - 0
//	3  2  1  0
//	CNF0  MODE0
//	GPIOx->CRH	pin 15 - 8
//
//      En hexa me queda cada nibble es igual a la configuracion de ese pin
//      nibble0 configura completo pin0
//
//	MODEx 00 Input (reset)
//	MODEx 01 Output 10MHz
//	MODEx 10 Output 2MHz
//	MODEx 11 Output 50MHz
//
//	Input Mode
//      las funciones alternativas de los pines estan directamente conectadad al
//      periferico en el modo input
//      CNFx 00 Analog
//      CNFx 01 Input Floating (reset)
//      CNFx 10 Input (pull up / dwn)
//      CNFx 11 Reserved
//
//	Output Mode
//      CNFx 00 Push Pull
//      CNFx 01 Open Drain
//      CNFx 10 Alternate func Push Pull
//      CNFx 11 Alternate func Open Drain
//
//      Pull-Up Pull-Dwn si el pin es Input Pup Pdwn el registro ODR coloca pull-up (1) o pull-dwn (0)
//      GPIOx->ODR 0xNNNN, 1 bit por pin
//

//-- GPIO Configuration --------------------
void GpioInit (void)
{
    unsigned long temp;

    //--- GPIO Clocks ---//
    if (!RCC_GPIOA_CLK)
        RCC_GPIOA_CLKEN;

    if (!RCC_GPIOB_CLK)
        RCC_GPIOB_CLKEN;

    if (!RCC_GPIOC_CLK)
        RCC_GPIOC_CLKEN;

    if (!RCC_GPIOD_CLK)
        RCC_GPIOD_CLKEN;

    //--- GPIOA Low Side ------------------//
    // PA0 Analog Channel 0 (SENSE_POWER)
    // PA1 Analog Channel 1 (SENSE_MEAS)
    // PA2 Analog Channel 2 (V_SENSE_28V)
    // PA3 Analog Channel 3 (V_SENSE_25V)
    // PA4 Analog DAC Output (REF_POWER)
    // PA5 Analog DAC Output (REF_MEAS)
    // PA6 Analog Channel 6 (V_SENSE_11V)
    // PA7 Analog Channel 7 (V_SENSE_8V)
    temp = GPIOA->CRL;
    temp &= 0x00000000;
    temp |= 0x00000000;
    GPIOA->CRL = temp;

    //--- GPIOA High Side ------------------//
    //PA8 NC
    //PA9 alternative Tx Usart1
    //PA10 alternative Rx Usart1
    //PA11 NC
    //PA12 NC
    //PA13 NC
    //PA14 NC
    //PA15 NC
    temp = GPIOA->CRH;
    temp &= 0xFFFFF00F;
    temp |= 0x000008A0;
    GPIOA->CRH = temp;

    //--- GPIOA Pull-Up Pull-Dwn ------------------//
    // temp = GPIOA->ODR;    //PA3 pull-up PA4 pull-down
    // temp &= 0xFFE7;
    // temp |= 0x0008;
    // GPIOA->ODR = temp;

    //--- GPIOB Low Side -------------------//
    //PB0 NC
    //PB1 NC
    //PB2 PROT_CH3 input
    //PB3 NC
    //PB4 NC
    //PB5 SW_RX_TX    RS485
    //PB6 
    //PB7 alternative TIM4_CH2
    temp = GPIOB->CRL;
    temp &= 0x0F0FF0FF;
    temp |= 0xA0200400;
    GPIOB->CRL = temp;

    //--- GPIOB High Side -------------------//
    //PB8 alternative TIM4_CH3
    //PB9 NC
    //PB10 alternative Tx Usart3
    //PB11 alternative Rx Usart3
    //PB12 NC
    //PB13 PROT_CH2 input
    //PB14 check tamper_funcs module
    //PB15 PROT_CH1 input
    temp = GPIOB->CRH;
    temp &= 0x0F0F00F0;
    temp |= 0x40408B0A;
    GPIOB->CRH = temp;    
    
    //--- GPIOC Low Side -------------------//
    //PC0 LED1
    //PC1 LED2
    //PC2 NC
    //PC3 NC
    //PC4 Sense 200V ADC Channel 14
    //PC5 Sense 15V ADC Channel 15
    //PC6 alternative TIM8_CH1
    //PC7 alternative TIM8_CH2
    temp = GPIOC->CRL;
    temp &= 0x0000FF00;
    temp |= 0xAA000022;
    GPIOC->CRL = temp;

    //--- GPIOC High Side -------------------//
    //PC8 alternative TIM8_CH3
    //PC9 alternative TIM8_CH4
    //PC10 alternative Tx Uart4
    //PC11 alternativo Rx Uart4
    //PC12 alternativo Tx Uart5
    //PC13 NC
    //PC14 NC    oscillator
    //PC15 NC    oscillator
    temp = GPIOC->CRH;   
    temp &= 0xFFF00000;
    temp |= 0x000B8BAA;
    GPIOC->CRH = temp;

    //--- GPIOD Low Side -------------------//
    //PD0 NC
    //PD1 NC
    //PD2 alternative Rx Uart5
    //PD3 No implemented
    //PD4 No implemented
    //PD5 No implemented
    //PD6 No implemented
    //PD7 No implemented    
    temp = GPIOD->CRL;   
    temp &= 0xFFFFF0FF;    
    temp |= 0x00000800;
    GPIOD->CRL = temp;

#ifdef USE_EXTI_LINES
    //Interrupts on:
    // PA4 PROT_CH4
    // PB2 PROT_CH3
    // PB13 PROT_CH2
    // PB15 PROT_CH1
    if (!RCC_AFIO_CLK)
        RCC_AFIO_CLKEN;

    // EXTI2 Select Port B & Pin2 for external interrupt
    temp = AFIO->EXTICR[0];
    temp &= ~AFIO_EXTICR1_EXTI2;
    temp |= AFIO_EXTICR1_EXTI2_PB;
    AFIO->EXTICR[0] = (unsigned short) temp;

    // EXTI4 Select Port A & Pin4 for external interrupt
    temp = AFIO->EXTICR[1];
    temp &= ~AFIO_EXTICR2_EXTI4;
    temp |= AFIO_EXTICR2_EXTI4_PA;
    AFIO->EXTICR[1] = (unsigned short) temp;

    // EXTI13 & EXTI15 Select PortB-Pin13 and PortB-Pin15 for external interrupt
    temp = AFIO->EXTICR[3];
    temp &= ~(AFIO_EXTICR4_EXTI13 | AFIO_EXTICR4_EXTI15);
    temp |= (AFIO_EXTICR4_EXTI13_PB | AFIO_EXTICR4_EXTI15_PB);
    AFIO->EXTICR[3] = (unsigned short) temp;

    // EXTI->IMR |= 0x00000001;    //Corresponding mask bit for interrupts EXTI2 EXTI4 EXTI13 EXTI15
    // EXTI->EMR |= 0x00000000;    //Corresponding mask bit for events
    //Interrupt line on rising edge
    EXTI->RTSR |= EXTI_RTSR_TR2 | EXTI_RTSR_TR4 | EXTI_RTSR_TR13 | EXTI_RTSR_TR15;
    EXTI->FTSR |= 0x00000000;    //Interrupt line on falling edge

    // Enable NVIC for EXTIs
    NVIC_EnableIRQ(EXTI2_IRQn);
    NVIC_SetPriority(EXTI2_IRQn, 2);

    NVIC_EnableIRQ(EXTI4_IRQn);
    NVIC_SetPriority(EXTI4_IRQn, 2);
    
    NVIC_EnableIRQ(EXTI15_10_IRQn);
    NVIC_SetPriority(EXTI15_10_IRQn, 2);
    
#endif    // USE_EXTI_LINES
}


#ifdef USE_EXTI_LINES
void EXTIOff (void)
{
    EXTI->IMR &= ~(EXTI_IMR_MR2 | EXTI_IMR_MR4 | EXTI_IMR_MR13 | EXTI_IMR_MR15);
}

void EXTIOn (void)
{
    EXTI->IMR |= (EXTI_IMR_MR2 | EXTI_IMR_MR4 | EXTI_IMR_MR13 | EXTI_IMR_MR15);
}
#endif


//--- end of file ---//
