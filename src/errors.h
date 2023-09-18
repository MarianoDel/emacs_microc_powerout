//---------------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### ERRORS.H ##########################################
//---------------------------------------------------------
#ifndef _ERRORS_H_
#define _ERRORS_H_


// Exported Types Constants and Macros -----------------------------------------
typedef enum {
	ERROR_OK = 0,
	ERROR_OVERCURRENT,
	ERROR_NO_CURRENT,
	ERROR_SOFT_OVERCURRENT,
	ERROR_OVERTEMP,
        ERROR_ANTENNA_LOST

} error_e;


// //Antenna conection errors.
// // The equipment can't find antenna. Antenna doesn't response, (probably never connected)
// #define ERR_CHANNEL_ANTENNA_DISCONNECTED(err_channel)    (0x10 + err_channel)
// // Antenna disconnected.
// #define ERR_CHANNEL_ANTENNA_LOST(err_channel)    (0x20 + err_channel)
// // Antenna is not emitting
// #define ERR_CHANNEL_ANTENNA_NOT_EMITTING(err_channel)    (0x30 + err_channel)
// // Antenna temperature errors,  was major than upper limit.
// #define ERR_CHANNEL_ANTENNA_TMP_OUT_OF_RANGE(err_channel)    (0x40 + err_channel)
// // The current is bigger than limit.
// #define ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(err_channel)    (0x50 + err_channel)


// Module Exported Functions ---------------------------------------------------
void Error_SetString (char * p_errstr, unsigned short err_num);
void Error_SetStatus(unsigned char error, unsigned char ch);
unsigned char Error_GetStatus(unsigned char ch);
void Error_SetStatus_For_Checks (unsigned char error, unsigned char ch);


#endif    /* _ERRORS_H_ */

//--- end of file ---//

