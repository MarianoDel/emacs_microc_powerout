//---------------------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### ANTENNAS.H ########################################
//---------------------------------------------------------
#ifndef _ANTENNAS_H_
#define _ANTENNAS_H_

#include "antennas_defs.h"


//---- Configurations Defines --------------------
#define ANTENNA_KEEP_ALIVE_TIMER        1250    //1.25 segundos
#define ANTENNA_TEMP_TIMER        1000    //1 segundo
#define KEEP_ALIVE_COUNTER    3
#define KEEP_ALIVE_ANTENNA_NAME_COUNTER    3
#define ANTENNA_INFO_TIMER_FIRST_START        10000
#define ANTENNA_INFO_TIMER_UPDATE        (KEEP_ALIVE_COUNTER * ANTENNA_KEEP_ALIVE_TIMER)

//---- Common Defines --------------------

//---- Exported Types --------------------
typedef enum {
    ANTENNA_INIT = 0,
    ANTENNA_IN_STANDBY,
    ANTENNA_IN_TREATMENT,
    ANTENNA_IN_PAUSE

} antenna_state_e;

enum Session_Channel_1_Verify_Antenna_states
{
	SESSION_CHANNEL_1_VERIFY_ANTENNA_INIT = 0,
	SESSION_CHANNEL_1_VERIFY_ANTENNA_WAIT_PARAMS,
	SESSION_CHANNEL_1_VERIFY_ANTENNA_FIN_OK,
	SESSION_CHANNEL_1_VERIFY_ANTENNA_FIN_ERROR
};

enum Session_Channel_2_Verify_Antenna_states
{
	SESSION_CHANNEL_2_VERIFY_ANTENNA_INIT = 0,
	SESSION_CHANNEL_2_VERIFY_ANTENNA_WAIT_PARAMS,
	SESSION_CHANNEL_2_VERIFY_ANTENNA_FIN_OK,
	SESSION_CHANNEL_2_VERIFY_ANTENNA_FIN_ERROR
};

enum Session_Channel_3_Verify_Antenna_states
{
	SESSION_CHANNEL_3_VERIFY_ANTENNA_INIT = 0,
	SESSION_CHANNEL_3_VERIFY_ANTENNA_WAIT_PARAMS,
	SESSION_CHANNEL_3_VERIFY_ANTENNA_FIN_OK,
	SESSION_CHANNEL_3_VERIFY_ANTENNA_FIN_ERROR
};

enum Session_Channel_4_Verify_Antenna_states
{
	SESSION_CHANNEL_4_VERIFY_ANTENNA_INIT = 0,
	SESSION_CHANNEL_4_VERIFY_ANTENNA_WAIT_PARAMS,
	SESSION_CHANNEL_4_VERIFY_ANTENNA_FIN_OK,
	SESSION_CHANNEL_4_VERIFY_ANTENNA_FIN_ERROR
};


//--- Exported functions ---//
void AntennaUpdateStates (void);

void AntennaSetCurrentTemp (unsigned char channel, unsigned char tint, unsigned char tdec);
unsigned char AntennaGetCurrentTemp (unsigned char channel);
unsigned char AntennaGetTempStatus (unsigned char channel);

void AntennaSetParamsStruct (unsigned char channel, antenna_st * pant);
void AntennaGetParamsStruct (unsigned char channel, antenna_st * pant);

unsigned char AntennaVerifyForTreatment (unsigned char);
unsigned char AntennaGetConnection (unsigned char);

void AntennaIsAnswering (unsigned char channel);
void AntennaEndTreatment (unsigned char channel);
void AntennaSendKnowInfoWithTimer (void);
void AntennaSetName (unsigned char channel, char * pname);

void AntennaTimeouts (void);
    
#endif    /* _ANTENNAS_H_ */

//--- end of file ---//

