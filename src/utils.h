//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### UTILS.H ###############################
//---------------------------------------------
#ifndef _UTILS_H_
#define _UTILS_H_


//--- Funciones del modulo utils.c -------------------------------------//
unsigned char StringIsANumber (char * pn, unsigned short * number);
unsigned short GetValue (unsigned char * pn, char delimiter);
unsigned char StringCheckNumbers (char * str, unsigned char len);

#endif    /* _UTILS_H_ */

//--- end of file ---//

