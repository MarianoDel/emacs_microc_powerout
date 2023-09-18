//---------------------------------------------------------
// #### PROYECTO STRETCHER MAGNETO F103 - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### TREATMENT.H ############################################
//---------------------------------------------------------

// Define to prevent recursive inclusion ---------------------------------------
#ifndef __TREATMENT_H_
#define __TREATMENT_H_
// #include "hard.h"    //para respuestas
#include "signals_defs.h"
#include "answers_defs.h"


// Module Exported Types Constants and Macros ----------------------------------
// typedef enum {
// 	CH1_ENABLE = 1,
// 	CH2_ENABLE,
// 	CH3_ENABLE,
// 	CH4_ENABLE,        
// 	CH1_DISABLE,
// 	CH2_DISABLE,
// 	CH3_DISABLE,
// 	CH4_DISABLE

// } ch_in_treatment_e;


// typedef struct {
// 	signal_type_e signal;
// 	unsigned char freq_int;
// 	unsigned char freq_dec;    
// 	// unsigned char freq_table_inc;
// 	unsigned char power;
// 	// unsigned char synchro_needed;

// 	//internals
// 	// unsigned short kprop;
// 	// unsigned short kinteg;
// 	// unsigned short kderv;

// } signals_struct_t;

// typedef enum {
//     UPDWN_AUTO = 0,
//     UPDWN_MANUAL

// } updwn_e;

typedef struct {
    signals_struct_t treatment_signal;
    unsigned short treatment_time;
    unsigned char channels_in_treatment;
    // unsigned short timer_synchro;
    
} treatment_conf_t;


// typedef enum {
// 	ERROR_OK = 0,
// 	ERROR_OVERCURRENT,
// 	ERROR_NO_CURRENT,
// 	ERROR_SOFT_OVERCURRENT,
// 	ERROR_OVERTEMP

// } error_t;


// //TIPO de descarga y estado de signal
// typedef enum
// {
// 	INIT_DISCHARGE = 0,
// 	NORMAL_DISCHARGE,
// 	TAU_DISCHARGE,
// 	FAST_DISCHARGE,
// 	STOPPED_BY_INT

// } discharge_state_t;


//--- Exported constants ---//
#define ENABLE_CH1_FLAG    0x41
#define ENABLE_CH2_FLAG    0x42
#define ENABLE_CH3_FLAG    0x44

#define DISABLE_CH1_FLAG    0x81
#define DISABLE_CH2_FLAG    0x82
#define DISABLE_CH3_FLAG    0x84

//timer synchro cuenta cada 100us
#define TIMER_SYNCHRO_MIN     100    //minimo en 100Hz
#define TIMER_SYNCHRO_MAX     2000    //maximo en 5Hz
#define FREQ_MIN_ALLOWED    5
#define FREQ_MAX_ALLOWED    100


//--- Exported macro ---//
//esto es 100 mas el ajuste
#define K_SYNCHRO_ADJUST    108

#define ERROR_OVERCURRENT_MASK			0x01
#define ERROR_NO_CURRENT_MASK				0x02
#define ERROR_OVERTEMP_MASK				0x04
#define ERROR_SOFT_OVERCURRENT_MASK		0x08
#define ERROR_FLUSH_MASK					0xff

#define SIZEOF_OVERCURRENT_BUFF			8



// Module Exported Functions ---------------------------------------------------
void Treatment_SetChannelsFlag (unsigned char);
unsigned char Treatment_GetChannelsFlag (void);

void Treatment_Manager (void);
resp_e Treatment_SetSignalType (signal_type_e);
signal_type_e Treatment_GetSignalType (void);
resp_e Treatment_SetFrequency (unsigned char, unsigned char);
void Treatment_GetFrequency (unsigned char *, unsigned char *);
resp_e Treatment_SetPower (unsigned char);
unsigned char Treatment_GetPower (void);
resp_e Treatment_SetTime (unsigned char, unsigned char, unsigned char);
resp_e Treatment_SetTimeinMinutes (unsigned short);
unsigned short Treatment_GetTime (void);
void Treatment_GetAllConf (char * tosend);
resp_e Treatment_AssertParams (void);
// unsigned char Treatment_GetState (void);
void Treatment_Timeouts (void);


#endif

//--- end of file ---//
