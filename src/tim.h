//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### TIM.H #################################
//---------------------------------------------

#ifndef _TIMER_H_
#define _TIMER_H_

// Module Exported Types Constants and Macros ----------------------------------
#define DUTY_10_PERCENT		100
#define DUTY_50_PERCENT		500
#define DUTY_95_PERCENT        950
#define DUTY_100_PERCENT        1000
#define DUTY_FOR_DMAX           450

#define DUTY_ALWAYS    (DUTY_100_PERCENT)    //use (duty_100 - 1) in TIM->ARR
#define DUTY_NONE    0

#define EnableTimer1    (TIM1->CR1 |= TIM_CR1_CEN)
#define DisableTimer1    (TIM1->CR1 &= ~TIM_CR1_CEN)

#define EnableTimer3 (TIM3->CR1 |= TIM_CR1_CEN)
#define DisableTimer3    (TIM3->CR1 &= ~TIM_CR1_CEN)

#ifdef STM32F10X_HD
#define EnableTimer6    (TIM6->CR1 |= TIM_CR1_CEN)
#define DisableTimer6    (TIM6->CR1 &= ~TIM_CR1_CEN)

#define EnableTimer7    (TIM7->CR1 |= TIM_CR1_CEN)
#define DisableTimer7    (TIM7->CR1 &= ~TIM_CR1_CEN)
#endif


//--- Exported Functions ------------------------------
void Wait_ms (unsigned short);

void TIM1_Init (void);
unsigned char TIM1_SyncGet (void);
void TIM1_SyncReset (void);
unsigned char TIM1_SyncVerify (unsigned char * freq_int, unsigned char * freq_dec);

void TIM3_Init (void);

void TIM4_Init (void);
void TIM4_Update_CH2 (unsigned short a);
void TIM4_Update_CH3 (unsigned short a);

#ifdef STM32F10X_HD
// Timer 5 Functions
void TIM5_Init (void);
void TIM5_Update_CH1 (unsigned short a);
void TIM5_Update_CH2 (unsigned short a);

void TIM6_Init(void);
void TIM6_IRQHandler (void);

void TIM7_Init(void);
void TIM7_IRQHandler (void);

// Timer 8 Functions
void TIM8_Init (void);
void TIM8_Update_CH1 (unsigned short a);
void TIM8_Update_CH2 (unsigned short a);
void TIM8_Update_CH3 (unsigned short a);
void TIM8_Update_CH4 (unsigned short a);
#endif

#endif /* _TIMER_H_ */
