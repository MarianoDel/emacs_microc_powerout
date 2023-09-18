//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS_KNOW_ANTENNAS.H #################
//---------------------------------------------
#ifndef _TESTS_KNOW_ANTENNAS_H_
#define _TESTS_KNOW_ANTENNAS_H_


// Module Exported Types Constants and Macros ----------------------------------
// typedef struct {

//     unsigned short resistance_int;
//     unsigned char resistance_dec;
//     unsigned short inductance_int;
//     unsigned char inductance_dec;
//     unsigned char current_limit_int;
//     unsigned char current_limit_dec;
//     unsigned char temp_max_int;
//     unsigned char temp_max_dec;

// } antenna_st;
#include "antennas_defs.h"


// Module Exported Functions ---------------------------------------------------
int TSP_Get_Know_Antennas (antenna_st * p);
void TSP_Get_Know_Antennas_Reset (void);
void TSP_Get_Know_Single_Antenna (antenna_st * p, unsigned char index);
    

#endif    /* _TESTS_KNOW_ANTENNAS_H_ */

//--- end of file ---//


