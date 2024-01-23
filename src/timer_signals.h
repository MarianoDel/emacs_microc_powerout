//------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TIMER_SIGNALS.H ##########################
//------------------------------------------------
#ifndef _TIMER_SIGNALS_H_
#define _TIMER_SIGNALS_H_


// Module Exported Types Constants and Macros ----------------------------------


// Exported Module Functions ---------------------------------------------------
void Timer_Square_Set_Registers (unsigned short new_psc, unsigned short new_arr);
unsigned char Timer_Square_Signal_Ended (void);
void Timer_Square_Signal_Reset (void);
// void Timer_Square_Signal_Set (void);
void Timer_Square_Signal_Stop (void);

void Timer_Sine_Set_Registers (unsigned short new_psc, unsigned short new_arr);
unsigned char Timer_Sine_Signal_Ended (void);
void Timer_Sine_Signal_Reset (void);
// void Timer_Sine_Signal_Set (void);
void Timer_Sine_Signal_Stop (void);


#endif    /* _TIMER_SIGNALS_H_ */

//--- end of file ---//

