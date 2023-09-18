//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DAC.H #################################
//---------------------------------------------
#ifndef _DAC_H_
#define _DAC_H_



// Module Exported Functions ---------------------------------------------------
void DAC_Config (void);
void DAC_Output1 (unsigned short new_val);
void DAC_Output2 (unsigned short new_val);
unsigned short DAC_Buffer1 (void);
unsigned short DAC_Buffer2 (void);


#endif /* _DAC_H_ */
