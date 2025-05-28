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
void Meas_Module_Init (void);
void Meas_Module_Update (void);

void Meas_Square_Set_Dac_Gain (unsigned char dac_gain);
void Meas_Square_V3_Set_Ref (void);
void Meas_Square_V3 (unsigned char * result, unsigned short * meas_raw, unsigned short * power_raw);

void Meas_Online_Calc_Resistance_V2 (unsigned short current_itov);
unsigned char Meas_Online_Flag_Get (void);
void Meas_Online_Flag_Reset (void);
unsigned int Meas_Online_Get_Value (unsigned short * current, unsigned short * meas);


#endif    /* _MEAS_H_ */

//--- end of file ---//

