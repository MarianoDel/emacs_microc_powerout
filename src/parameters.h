//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### PARAMETERS.H ##########################
//---------------------------------------------

#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_


//-- Memory Struct to Save --------------------
typedef struct parameters {

    //-- Tamper config backup ----
    unsigned char tamper_config;    // 1

    unsigned char vdummy [31];

    //-- End of Struct check alignment ---- //32

} parameters_typedef;

//-- End of Memory Struct to Save --------------------

#endif    /* _PARAMETERS_H_ */

//--- end of file ---//

