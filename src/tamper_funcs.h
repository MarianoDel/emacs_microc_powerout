//-------------------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TAMPER_FUNCS.H ##################################
//-------------------------------------------------------
#ifndef _TAMPER_FUNCS_H_
#define _TAMPER_FUNCS_H_

#include "parameters.h"

// Exported Types Constants and Macros -----------------------------------------
typedef enum {
    TAMPER_DISABLE,
    TAMPER_ENABLE,    
    TAMPER_ENABLE_TAMPERED,
    TAMPER_ENABLE_NO_TAMPER,
    TAMPER_ENABLE_LOW_BAT,
    TAMPER_ERROR
    
} tamper_state_e;


// Module Exported Functions ---------------------------------------------------
tamper_state_e Tamper_GetStatus (parameters_typedef * conf);
tamper_state_e Tamper_SetStatus (parameters_typedef * conf, tamper_state_e new_state);
char * Tamper_CodeToString (tamper_state_e code);


#endif    /* _TAMPER_FUNCS_H_ */

//--- end of file ---//

