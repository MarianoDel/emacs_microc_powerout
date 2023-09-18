//---------------------------------------------------------
// #### Part of Startup Files for STM32F103RC
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### STARTUP_CLOCKS.H ############################################
//---------------------------------------------------------

#ifndef STARTUP_CLOCKS_H_
#define STARTUP_CLOCKS_H_

#include "hard.h"        //for config pourpose

//--- Defines for conf clocks -------
#ifdef HSI_INTERNAL_RC
// sanity checks
#if (!defined SYSCLK_FREQ_64MHz) && (!defined SYSCLK_FREQ_48MHz) && (!defined SYSCLK_FREQ_8MHz)
#error "define sysclk frequency on hard.h"
#endif
#if (defined SYSCLK_FREQ_72MHz) && (defined SYSCLK_FREQ_48MHz) && (defined SYSCLK_FREQ_8MHz)
#error "select one or the sysclk frequency on hard.h"
#endif
#endif

#ifdef HSE_CRYSTAL_OSC
// sanity checks
#if (!defined CRYSTAL_8MHZ) && (!defined CRYSTAL_12MHZ)
#error "define crystal frequency on hard.h"
#endif
#if (defined CRYSTAL_8MHZ) && (defined CRYSTAL_12MHZ)
#error "select one or the other crystal frequency on hard.h"
#endif
#endif

//--- Exported Module Functions ----
void SystemInit (void);


#endif
