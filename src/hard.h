//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### HARD.H #################################
//---------------------------------------------

#ifndef HARD_H_
#define HARD_H_


//----------- Defines For Configuration -------------

//----- Board Configuration -------------------//
//--- Hardware ------------------//
#define HARDWARE_VERSION_2_0    // ch1 included in main brd ver 2.0
// #define HARDWARE_VERSION_1_0    // first prototype


//--- Software ------------------//
#define FIRMWARE_VERSION_1_0    // init version


//-------- Type of Program (depending on software version) ----------------


//--- Serial Number / Device Id Bytes length ----------
#define USE_DEVICE_ID_4BYTES
// #define USE_DEVICE_ID_12BYTES



//-------- Type of Program and Features ----------------


//-------- Kind of Reports Sended ----------------

//-------- Others Configurations depending on the formers ------------
// #define K_200V    0.0957    //con diodo z prot de 3.1V da error
// #define K_200V    0.0806    //con diodo z 5.1V
// #define K_15V    0.00804    //con z 3.1V
// #define K_15V    0.00619    //con z 5.1V


//-------- Oscillator and Crystal selection (Freq in startup_clocks.h) ---
#define HSI_INTERNAL_RC
// #define HSE_CRYSTAL_OSC

#ifdef HSE_CRYSTAL_OSC
// #define CRYSTAL_8MHZ
#define CRYSTAL_12MHZ
#endif

#ifdef HSE_CRYSTAL_OSC
// #define SYSCLK_FREQ_72MHz
#define SYSCLK_FREQ_8MHz
#endif

#ifdef HSI_INTERNAL_RC
#define SYSCLK_FREQ_64MHz
// #define SYSCLK_FREQ_8MHz
#endif

//-------- End Of Defines For Configuration ------




//--- Hardware & Software Messages ------------------//
#ifdef HARDWARE_VERSION_2_0
#define HARD "Hardware Version: 2.0"
#endif
#ifdef HARDWARE_VERSION_1_0
#define HARD "Hardware Version: 1.0"
#endif
#ifdef FIRMWARE_VERSION_1_0
#define SOFT "Firmware Version: 1.0"
#endif
//--- End of Hardware & Software Messages ------------------//



// Exported Types --------------------------------------------------------------
#ifdef HARDWARE_VERSION_2_0

// PA defines ----
// PA0 Analog Channel 0 (SENSE_POWER)
// PA1 Analog Channel 1 (SENSE_MEAS)
// PA2 NC
// PA3 NC

// PA4 Analog DAC Output (REF_POWER)
// PA5 Analog DAC Output (REF_MEAS)

// PA6 NC
// PA7 NC

// PA8 NC

// PA9 PA10 Alternative Usart1 Tx Rx

// PA11 PA12 PA13 PA14 PA15 NC

// PB defines ----
// PB0 Out or Alternative TIM8_CH2N
#define RIGHT    ((GPIOB->ODR & 0x0001) != 0)
#define RIGHT_ON    (GPIOB->BSRR = 0x00000001)
#define RIGHT_OFF    (GPIOB->BSRR = 0x00010000)
#define RIGHT_CH1    RIGHT
#define RIGHT_CH1_ON    RIGHT_ON
#define RIGHT_CH1_OFF    RIGHT_OFF

// PB1 Out or Alternative TIM8_CH3N
#define RIGHT_PLATE    ((GPIOB->ODR & 0x0002) != 0)
#define RIGHT_PLATE_ON    (GPIOB->BSRR = 0x00000002)
#define RIGHT_PLATE_OFF    (GPIOB->BSRR = 0x00020000)

// PB2 PB3 PB4 PB5 PB6 PB7 NC

// PB9 PB10 NC

// PB10 PB11 Alternative Usart3 Tx Rx

// PB12 NC

// PB13 Input (SYNC_IN)
#define SYNC_IN    ((GPIOB->IDR & 0x2000) != 0)

// PB14 PB15 NC

// PC defines ----
// PC0 
#define LED    ((GPIOC->ODR & 0x0001) != 0)
#define LED_ON    (GPIOC->BSRR = 0x00000001)
#define LED_OFF    (GPIOC->BSRR = 0x00010000)
#define LED_CH1    LED
#define LED_CH1_ON    LED_ON
#define LED_CH1_OFF    LED_OFF

// PC1 PC2 PC3 NC

// PC4 Analog Channel 14 (NTC_10K)

// PC6 NC

// PC7 Out or Alternative TIM8_CH2
#define LEFT    ((GPIOC->ODR & 0x0080) != 0)
#define LEFT_ON    (GPIOC->BSRR = 0x00000080)
#define LEFT_OFF    (GPIOC->BSRR = 0x00800000)
#define LEFT_CH1    LEFT
#define LEFT_CH1_ON    LEFT_ON
#define LEFT_CH1_OFF    LEFT_OFF

// PC8 Out or Alternative TIM8_CH3
#define LEFT_PLATE    ((GPIOC->ODR & 0x0100) != 0)
#define LEFT_PLATE_ON    (GPIOC->BSRR = 0x00000100)
#define LEFT_PLATE_OFF    (GPIOC->BSRR = 0x01000000)

// PC9 PC10 PC11 PC12 NC

// PC13 PC14 PC15 NC

// PD defines ----
// PD0 PD1 PD2 NC

#endif //HARDWARE_VERSION_2_0


#ifdef HARDWARE_VERSION_1_0

// PA defines ----
// PA0 Analog Channel 0 (SENSE_POWER)
// PA1 Analog Channel 1 (SENSE_MEAS)
// PA2 Analog Channel 2 (V_SENSE_28V)
// PA3 Analog Channel 3 (V_SENSE_25V)

// PA4 Analog DAC Output (REF_POWER)
// PA5 Analog DAC Output (REF_MEAS)

// PA6 Analog Channel 6 (V_SENSE_11V)
// PA7 Analog Channel 7 (V_SENSE_8V)

// PA8 NC

// PA9 PA10 Alternative Usart1 Tx Rx

// PA11 PA12 PA13 PA14 PA15 NC

// PB defines ----
// PB0 Out or Alternative TIM8_CH2N
#define RIGHT    ((GPIOB->ODR & 0x0001) != 0)
#define RIGHT_ON    (GPIOB->BSRR = 0x00000001)
#define RIGHT_OFF    (GPIOB->BSRR = 0x00010000)

// PB1 PB2 NC

// PB3 PB4 PB5 PB6 PB7 NC

// PB9 PB10 NC

// PB10 PB11 Alternative Usart3 Tx Rx

// PB12 NC

// PB13 Input (SYNC_IN)
#define SYNC_IN    ((GPIOB->IDR & 0x2000) != 0)

// PB14 PB15 NC

// PC defines ----
// PC0 
#define LED    ((GPIOC->ODR & 0x0001) != 0)
#define LED_ON    (GPIOC->BSRR = 0x00000001)
#define LED_OFF    (GPIOC->BSRR = 0x00010000)

// PC1 PC2 PC3 NC

// PC4 Analog Channel 14 (NTC_10K)

// PC6 NC

// PC7 Out or Alternative TIM8_CH2
#define LEFT    ((GPIOC->ODR & 0x0080) != 0)
#define LEFT_ON    (GPIOC->BSRR = 0x00000080)
#define LEFT_OFF    (GPIOC->BSRR = 0x00800000)

// PC8 PC9 NC

// PC10 PC11 PC12 NC

// PC13 PC14 PC15 NC

// PD defines ----
// PD0 PD1 PD2 NC

#endif //HARDWARE_VERSION_1_0



// LED states (how many blinks)
#define LED_NO_BLINKING    0     
#define LED_TREATMENT_STANDBY    1
#define LED_TREATMENT_SQUARE_RUNNING    2
#define LED_TREATMENT_SINE_RUNNING    3



//--- Exported Module Functions ----
void ChangeLed (unsigned char how_many);
void ChangeLed_With_Timer (unsigned char how_many, unsigned short led_timer_off);
void UpdateLed (void);
void HARD_Timeouts (void);

unsigned char Sync_Input_Is_On (void);

unsigned char Led_Is_On (void);
void Led_On (void);
void Led_Off (void);

void Hard_GetVoltages (char * buff);
void Hard_GetHardSoft (char * buff);
void Hard_GetVoltages_Complete (void);

#endif
