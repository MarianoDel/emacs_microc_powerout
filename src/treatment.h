//-----------------------------------------------------------
// #### MICROCURRENT CHANNEL PROJECT F103 - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TREATMENT.H #########################################
//-----------------------------------------------------------

// Define to prevent recursive inclusion ---------------------------------------
#ifndef __TREATMENT_H_
#define __TREATMENT_H_

#include "answers_defs.h"


// Module Exported Types Constants and Macros ----------------------------------
typedef enum {
    POLARITY_POS,
    POLARITY_NEG,
    POLARITY_ALT
    
} polarity_e;


typedef enum {
    MODE_SQUARE,
    MODE_SINE
    
} mode_e;


typedef struct {
    unsigned short freq_int;
    unsigned short freq_dec;
    polarity_e polarity;
    mode_e mode;
    unsigned short intensity;
    unsigned short itov_high;
    unsigned short itov_low;
    unsigned char threshold;
    unsigned char gain;    
    
} treatment_conf_t;





// Module Exported Functions ---------------------------------------------------
void Treatment_Manager (void);
resp_e Treatment_SetFrequency_Str (char * str);
resp_e Treatment_SetFrequency (unsigned short fint, unsigned short fdec);
resp_e Treatment_SetIntensity_Str (char * str);
resp_e Treatment_SetIntensity (unsigned short intensity);
resp_e Treatment_SetPolarity_Str (char * str);
resp_e Treatment_SetMode_Str (char * str);
resp_e Treatment_SetThreshold_Str (char * str);
resp_e Treatment_SetThreshold (unsigned short threshold);
void Treatment_Start (void);
void Treatment_Stop (void);
resp_e Treatment_SetGain_Str (char * str);
resp_e Treatment_SetGain (unsigned short gain);
unsigned char Treatment_GetGain (void);

unsigned char Treatment_Start_Flag (void);
void Treatment_Start_Flag_Set (void);
void Treatment_Start_Flag_Reset (void);

unsigned char Treatment_Stop_Flag (void);
void Treatment_Stop_Flag_Set (void);
void Treatment_Stop_Flag_Reset (void);


#endif

//--- end of file ---//
