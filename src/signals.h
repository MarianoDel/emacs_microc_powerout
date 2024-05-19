//------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### SIGNALS.H ################################
//------------------------------------------------
#ifndef _SIGNALS_H_
#define _SIGNALS_H_

#include "answers_defs.h"
#include "treatment.h"


// Module Exported Types Constants and Macros ----------------------------------
typedef struct {
    unsigned int arr;
    unsigned int arr_div_4;
    unsigned short psc;
    unsigned short freq_int;
    unsigned char freq_dec;
    
} timers_data_st;


// Exported Module Functions ---------------------------------------------------
void Signals_Timeouts (void);

resp_e Signals_Square (treatment_conf_t * pconf);
void Signals_Square_Reset (void);

resp_e Signals_Sinusoidal (treatment_conf_t * pconf);
void Signals_Sinusoidal_Reset (void);

resp_e Signals_Timers_Calculation (timers_data_st * td);

void Signals_Sync_Enable (void);
void Signals_Sync_Disable (void);

void Signals_Stop (void);

void Signals_Set_Frequency_Intensity_Change_Flag (void);

#endif    /* _SIGNALS_H_ */

//--- end of file ---//

