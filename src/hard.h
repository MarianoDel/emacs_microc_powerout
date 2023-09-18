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
#define HARDWARE_VERSION_1_0    // first prototype


//--- Software ------------------//
#define FIRMWARE_VERSION_1_0    // init version


//-------- Type of Program (depending on software version) ----------------


//--- Serial Number / Device Id Bytes length ----------
#define USE_DEVICE_ID_4BYTES
// #define USE_DEVICE_ID_12BYTES



//-------- Type of Program and Features ----------------
// #define USE_NO_TREATMENT_DETECT    //cuando esta en tratamiento revisa si las potencias tambien
// #define USE_BUZZER_ON_START


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
#ifdef HARDWARE_VERSION_1_0
#define HARD "Hardware Version: 1.0"
#endif
#ifdef FIRMWARE_VERSION_1_0
#define SOFT "Firmware Version: 1.0"
#endif
//--- End of Hardware & Software Messages ------------------//



// Exported Types --------------------------------------------------------------
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



//--- ESTADOS DEL LED -----------
typedef enum
{    
    START_BLINKING = 0,
    WAIT_TO_OFF,
    WAIT_TO_ON,
    WAIT_NEW_CYCLE
} led_state_t;

//Estados Externos de LED BLINKING
#define LED_NO_BLINKING    0     
#define LED_TREATMENT_STANDBY    1
#define LED_TREATMENT_GENERATING    2
#define LED_TREATMENT_PAUSED    3
#define LED_TREATMENT_BRIDGE_MODE    5



//--- Exported Module Functions ----
void ChangeLed (unsigned char);
void UpdateLed (void);
void HARD_Timeouts (void);
void HARD_L1_ON (void);
void HARD_L1_OFF (void);

unsigned char Led1_Is_On (void);
void Led1_On (void);
void Led1_Off (void);

void Pb14_To_Output (void);
void Pb14_Off (void);
void Pb14_On (void);
void Pb14_To_Input (void);
unsigned char Tamper_Pin (void);

#endif
