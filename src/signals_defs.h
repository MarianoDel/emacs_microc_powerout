//------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### SIGNALS_DEFS.H ###########################
//------------------------------------------------
#ifndef _SIGNALS_DEFS_H_
#define _SIGNALS_DEFS_H_


// Exported Types --------------------------------------------------------------
typedef enum {
	SQUARE_SIGNAL = 0,
	TRIANGULAR_SIGNAL,
	SINUSOIDAL_SIGNAL

} signal_type_e;


typedef struct {
    // general all channels things
    signal_type_e signal;

    unsigned char freq_int;
    unsigned char freq_dec;

    unsigned char power;    

    // by channel things    
    unsigned short kp_ch1;
    unsigned short ki_ch1;
    unsigned short kd_ch1;    
    unsigned short kp_ch2;
    unsigned short ki_ch2;
    unsigned short kd_ch2;        
    unsigned short kp_ch3;
    unsigned short ki_ch3;
    unsigned short kd_ch3;
    unsigned short kp_ch4;
    unsigned short ki_ch4;
    unsigned short kd_ch4;

    unsigned short max_c_ch1;
    unsigned short max_c_ch2;
    unsigned short max_c_ch3;
    unsigned short max_c_ch4;

    unsigned char treat_in_ch1;
    unsigned char treat_in_ch2;
    unsigned char treat_in_ch3;
    unsigned char treat_in_ch4;
    
} signals_struct_t;


#endif    /* _SIGNALS_DEFS_H_ */

//--- end of file ---//

