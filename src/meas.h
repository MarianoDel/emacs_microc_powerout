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
void Meas_Square_Reset (void);
void Meas_Square_Init (void);
void Meas_Timeout (void);
void Meas_Square_Set_Dac_Gain (unsigned char dac_gain);

unsigned char Meas_Square_V2 (unsigned char * result);

// for sinusoidal online meas
void Meas_Sine_Reset (void);
void Meas_Sine (unsigned short dac_value);
unsigned char Meas_Sine_Update (unsigned short * conductivity);

// for square online meas
void Meas_Online_Reset (void);
void Meas_Online (unsigned short dac_value);
unsigned char Meas_Online_Update (unsigned int * resistance);


void Meas_Square_V3_Set_Ref (void);
void Meas_Square_V3 (unsigned char * result, unsigned short * meas_raw, unsigned short * power_raw);
// void Meas_Square_V3 (unsigned char * result, unsigned short * meas_raw);
void Meas_Module_Init (void);
void Meas_Module_Update (void);

void Meas_Online_Calc_Resistance_V2 (unsigned short current_itov);
unsigned char Meas_Online_Flag_Get (void);
void Meas_Online_Flag_Reset (void);
unsigned int Meas_Online_Get_Value (void);


#endif    /* _MEAS_H_ */

//--- end of file ---//

