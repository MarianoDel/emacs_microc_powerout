//------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MEAS.H ################################
//------------------------------------------------
#ifndef _MEAS_H_
#define _MEAS_H_



// Module Exported Types Constants and Macros ----------------------------------


// Exported Module Functions ---------------------------------------------------
unsigned char Meas_Square (unsigned char * result);
void Meas_Square_Init (void);
void Meas_Timeout (void);
void Meas_Square_Set_Dac_Gain (unsigned char dac_gain);
unsigned char Meas_Sine_Update (unsigned short * conductivity);
void Meas_Sine (unsigned short dac_value);

#endif    /* _MEAS_H_ */

//--- end of file ---//

