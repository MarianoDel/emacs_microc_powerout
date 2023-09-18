//---------------------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### ANTENNAS_DEFS.H ###################################
//---------------------------------------------------------
#ifndef _ANTENNAS_DEFS_H_
#define _ANTENNAS_DEFS_H_

typedef struct {

    unsigned short resistance_int;
    unsigned char resistance_dec;
    unsigned short inductance_int;
    unsigned char inductance_dec;
    unsigned char current_limit_int;
    unsigned char current_limit_dec;
    unsigned char temp_max_int;
    unsigned char temp_max_dec;

} antenna_st;


#endif    /* _ANTENNAS_DEFS_H_ */

//--- end of file ---//

