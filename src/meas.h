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
unsigned char Meas_Square (unsigned char * result);
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

#endif    /* _MEAS_H_ */

//--- end of file ---//

