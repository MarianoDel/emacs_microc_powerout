//---------------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### ANTENNAS.C ###########################################
//---------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "antennas.h"
#include "channels_defs.h"
#include "answers_defs.h"
#include "usart.h"

#include <stdio.h>
#include <string.h>


// Module Private Types Constants and Macros -----------------------------------
// -- for module config ---------
#define RPI_Send(X)    Usart1Send(X)
#define UART_CH1_Send(X)    Usart2Send(X)
#define UART_CH2_Send(X)    Usart3Send(X)
#define UART_CH3_Send(X)    Uart4Send(X)
#define UART_CH4_Send(X)    Uart5Send(X)


#define SIZEOF_ANTENNA_NAME    21

typedef struct {
    unsigned short ant_resistance_int;
    unsigned char ant_resistance_dec;
    unsigned short ant_inductance_int;
    unsigned char ant_inductance_dec;
    unsigned char ant_current_limit_int;
    unsigned char ant_current_limit_dec;
    unsigned char ant_temp_max_int;
    unsigned char ant_temp_max_dec;

    char connected_ant_name [SIZEOF_ANTENNA_NAME];    //20 chars and 0 termination
    
} antenna_complete_st;




//--- Used for Antenna Sync - Time base ---//
#define SESSION_CHANNEL_ASK_TEMP 2500 //2.5 segundos
#define SESSION_CHANNEL_ANSWER_TEMP 10000 //10segundos.

// #define SESSION_CHANNEL_1_VERIFY_ANTENNA_TIME 5000			//ms
// #define SESSION_CHANNEL_2_VERIFY_ANTENNA_TIME 5000			//ms
// #define SESSION_CHANNEL_3_VERIFY_ANTENNA_TIME 5000			//ms
// #define SESSION_CHANNEL_4_VERIFY_ANTENNA_TIME 5000			//ms

#define ANT_LOCKED    0x01
#define ANT_CONNECTED    0x02
#define ANT_HAVE_PARAMS    0x04
#define ANT_NEW_PARAMS     0x08
#define ANT_IN_TREATMENT    0x10
#define ANT_HAVE_NAME    0x20
#define ANT_HAVE_STABLE_PARAMS    0x40

#define AntennaSetStableParamsCh1    (ch1_ant_conn_status |= ANT_HAVE_STABLE_PARAMS)
#define AntennaSetStableParamsCh2    (ch2_ant_conn_status |= ANT_HAVE_STABLE_PARAMS)
#define AntennaSetStableParamsCh3    (ch3_ant_conn_status |= ANT_HAVE_STABLE_PARAMS)
#define AntennaSetStableParamsCh4    (ch4_ant_conn_status |= ANT_HAVE_STABLE_PARAMS)

#define AntennaResetStableParamsCh1    (ch1_ant_conn_status &= ~ANT_HAVE_STABLE_PARAMS)
#define AntennaResetStableParamsCh2    (ch2_ant_conn_status &= ~ANT_HAVE_STABLE_PARAMS)
#define AntennaResetStableParamsCh3    (ch3_ant_conn_status &= ~ANT_HAVE_STABLE_PARAMS)
#define AntennaResetStableParamsCh4    (ch4_ant_conn_status &= ~ANT_HAVE_STABLE_PARAMS)

#define AntennaCheckStableParamsCh1    (ch1_ant_conn_status & ANT_HAVE_STABLE_PARAMS)
#define AntennaCheckStableParamsCh2    (ch2_ant_conn_status & ANT_HAVE_STABLE_PARAMS)
#define AntennaCheckStableParamsCh3    (ch3_ant_conn_status & ANT_HAVE_STABLE_PARAMS)
#define AntennaCheckStableParamsCh4    (ch4_ant_conn_status & ANT_HAVE_STABLE_PARAMS)

#define AntennaSetNewParamsCh1    (ch1_ant_conn_status |= ANT_NEW_PARAMS)
#define AntennaSetNewParamsCh2    (ch2_ant_conn_status |= ANT_NEW_PARAMS)
#define AntennaSetNewParamsCh3    (ch3_ant_conn_status |= ANT_NEW_PARAMS)
#define AntennaSetNewParamsCh4    (ch4_ant_conn_status |= ANT_NEW_PARAMS)

#define AntennaResetNewParamsCh1    (ch1_ant_conn_status &= ~ANT_NEW_PARAMS)
#define AntennaResetNewParamsCh2    (ch2_ant_conn_status &= ~ANT_NEW_PARAMS)
#define AntennaResetNewParamsCh3    (ch3_ant_conn_status &= ~ANT_NEW_PARAMS)
#define AntennaResetNewParamsCh4    (ch4_ant_conn_status &= ~ANT_NEW_PARAMS)

#define AntennaCheckNewParamsCh1    (ch1_ant_conn_status & ANT_NEW_PARAMS)
#define AntennaCheckNewParamsCh2    (ch2_ant_conn_status & ANT_NEW_PARAMS)
#define AntennaCheckNewParamsCh3    (ch3_ant_conn_status & ANT_NEW_PARAMS)
#define AntennaCheckNewParamsCh4    (ch4_ant_conn_status & ANT_NEW_PARAMS)

#define AntennaParamsCh1    (ch1_ant_conn_status |= ANT_HAVE_PARAMS)
#define AntennaParamsCh2    (ch2_ant_conn_status |= ANT_HAVE_PARAMS)
#define AntennaParamsCh3    (ch3_ant_conn_status |= ANT_HAVE_PARAMS)
#define AntennaParamsCh4    (ch4_ant_conn_status |= ANT_HAVE_PARAMS)

#define AntennaResetParamsCh1    (ch1_ant_conn_status &= ~ANT_HAVE_PARAMS)
#define AntennaResetParamsCh2    (ch2_ant_conn_status &= ~ANT_HAVE_PARAMS)
#define AntennaResetParamsCh3    (ch3_ant_conn_status &= ~ANT_HAVE_PARAMS)
#define AntennaResetParamsCh4    (ch4_ant_conn_status &= ~ANT_HAVE_PARAMS)

#define AntennaCheckParamsCh1    (ch1_ant_conn_status & ANT_HAVE_PARAMS)
#define AntennaCheckParamsCh2    (ch2_ant_conn_status & ANT_HAVE_PARAMS)
#define AntennaCheckParamsCh3    (ch3_ant_conn_status & ANT_HAVE_PARAMS)
#define AntennaCheckParamsCh4    (ch4_ant_conn_status & ANT_HAVE_PARAMS)

#define AntennaConnectionCh1    (ch1_ant_conn_status |= ANT_CONNECTED)
#define AntennaConnectionCh2    (ch2_ant_conn_status |= ANT_CONNECTED)
#define AntennaConnectionCh3    (ch3_ant_conn_status |= ANT_CONNECTED)
#define AntennaConnectionCh4    (ch4_ant_conn_status |= ANT_CONNECTED)

#define AntennaResetConnectionCh1    (ch1_ant_conn_status &= ~ANT_CONNECTED)
#define AntennaResetConnectionCh2    (ch2_ant_conn_status &= ~ANT_CONNECTED)
#define AntennaResetConnectionCh3    (ch3_ant_conn_status &= ~ANT_CONNECTED)
#define AntennaResetConnectionCh4    (ch4_ant_conn_status &= ~ANT_CONNECTED)

#define AntennaCheckConnectionCh1    (ch1_ant_conn_status & ANT_CONNECTED)
#define AntennaCheckConnectionCh2    (ch2_ant_conn_status & ANT_CONNECTED)
#define AntennaCheckConnectionCh3    (ch3_ant_conn_status & ANT_CONNECTED)
#define AntennaCheckConnectionCh4    (ch4_ant_conn_status & ANT_CONNECTED)

#define AntennaLockCh1    (ch1_ant_conn_status |= ANT_LOCKED)
#define AntennaLockCh2    (ch2_ant_conn_status |= ANT_LOCKED)
#define AntennaLockCh3    (ch3_ant_conn_status |= ANT_LOCKED)
#define AntennaLockCh4    (ch4_ant_conn_status |= ANT_LOCKED)

#define AntennaUnlockCh1    (ch1_ant_conn_status &= ~ANT_LOCKED)
#define AntennaUnlockCh2    (ch2_ant_conn_status &= ~ANT_LOCKED)
#define AntennaUnlockCh3    (ch3_ant_conn_status &= ~ANT_LOCKED)
#define AntennaUnlockCh4    (ch4_ant_conn_status &= ~ANT_LOCKED)

#define AntennaCheckLockCh1    (ch1_ant_conn_status & ANT_LOCKED)
#define AntennaCheckLockCh2    (ch2_ant_conn_status & ANT_LOCKED)
#define AntennaCheckLockCh3    (ch3_ant_conn_status & ANT_LOCKED)
#define AntennaCheckLockCh4    (ch4_ant_conn_status & ANT_LOCKED)

#define AntennaInTreatmentCh1    (ch1_ant_conn_status |= ANT_IN_TREATMENT)
#define AntennaInTreatmentCh2    (ch2_ant_conn_status |= ANT_IN_TREATMENT)
#define AntennaInTreatmentCh3    (ch3_ant_conn_status |= ANT_IN_TREATMENT)
#define AntennaInTreatmentCh4    (ch4_ant_conn_status |= ANT_IN_TREATMENT)

#define AntennaResetInTreatmentCh1    (ch1_ant_conn_status &= ~ANT_IN_TREATMENT)
#define AntennaResetInTreatmentCh2    (ch2_ant_conn_status &= ~ANT_IN_TREATMENT)
#define AntennaResetInTreatmentCh3    (ch3_ant_conn_status &= ~ANT_IN_TREATMENT)
#define AntennaResetInTreatmentCh4    (ch4_ant_conn_status &= ~ANT_IN_TREATMENT)

#define AntennaCheckInTreatmentCh1    (ch1_ant_conn_status & ANT_IN_TREATMENT)
#define AntennaCheckInTreatmentCh2    (ch2_ant_conn_status & ANT_IN_TREATMENT)
#define AntennaCheckInTreatmentCh3    (ch3_ant_conn_status & ANT_IN_TREATMENT)
#define AntennaCheckInTreatmentCh4    (ch4_ant_conn_status & ANT_IN_TREATMENT)

#define AntennaNameCh1    (ch1_ant_conn_status |= ANT_HAVE_NAME)
#define AntennaNameCh2    (ch2_ant_conn_status |= ANT_HAVE_NAME)
#define AntennaNameCh3    (ch3_ant_conn_status |= ANT_HAVE_NAME)
#define AntennaNameCh4    (ch4_ant_conn_status |= ANT_HAVE_NAME)

#define AntennaResetNameCh1    (ch1_ant_conn_status &= ~ANT_HAVE_NAME)
#define AntennaResetNameCh2    (ch2_ant_conn_status &= ~ANT_HAVE_NAME)
#define AntennaResetNameCh3    (ch3_ant_conn_status &= ~ANT_HAVE_NAME)
#define AntennaResetNameCh4    (ch4_ant_conn_status &= ~ANT_HAVE_NAME)

#define AntennaCheckNameCh1    (ch1_ant_conn_status & ANT_HAVE_NAME)
#define AntennaCheckNameCh2    (ch2_ant_conn_status & ANT_HAVE_NAME)
#define AntennaCheckNameCh3    (ch3_ant_conn_status & ANT_HAVE_NAME)
#define AntennaCheckNameCh4    (ch4_ant_conn_status & ANT_HAVE_NAME)

// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
volatile unsigned short antenna_timer = 0;
volatile unsigned short antenna_info_timer = 0;

antenna_complete_st antenna_conn_ch1;
antenna_complete_st antenna_conn_ch2;
antenna_complete_st antenna_conn_ch3;
antenna_complete_st antenna_conn_ch4;


antenna_state_e antenna_state;

unsigned char ch1_ant_current_temp_int = 0;
unsigned char ch1_ant_current_temp_dec = 0;
unsigned char ch2_ant_current_temp_int = 0;
unsigned char ch2_ant_current_temp_dec = 0;
unsigned char ch3_ant_current_temp_int = 0;
unsigned char ch3_ant_current_temp_dec = 0;
unsigned char ch4_ant_current_temp_int = 0;
unsigned char ch4_ant_current_temp_dec = 0;

unsigned char ch1_ant_conn_status = 0;
unsigned char ch2_ant_conn_status = 0;
unsigned char ch3_ant_conn_status = 0;
unsigned char ch4_ant_conn_status = 0;

unsigned char keepalive_ch1 = 0;
unsigned char keepalive_ch2 = 0;
unsigned char keepalive_ch3 = 0;
unsigned char keepalive_ch4 = 0;

unsigned char antenna_info_sended = 0;
unsigned char antenna_send_info_with_timer = 0;


// Module Private Functions ----------------------------------------------------
// void AntennaBackupParams (antenna_st *, antenna_complete_st *);
void AntennaSendKnowParams (void);

// void AntennaFlushParamsInLock (antenna_st *);
// void AntennaFlushParams (unsigned char);

static void AntennaUpdateInStandByCh1_SM (void);
static void AntennaUpdateInStandByCh2_SM (void);
static void AntennaUpdateInStandByCh3_SM (void);
static void AntennaUpdateInStandByCh4_SM (void);


// Module Exported Functions ---------------------------------------------------
typedef enum {
    INIT_SEARCH,
    NO_CONN,
    GET_NAME,
    IN_STANDBY
    
} standby_state_e;

standby_state_e standby_ch1 = INIT_SEARCH;
standby_state_e standby_ch2 = INIT_SEARCH;
standby_state_e standby_ch3 = INIT_SEARCH;
standby_state_e standby_ch4 = INIT_SEARCH;

#define KEEP_ALIVE_NAME_CNTR    3
#define KEEP_ALIVE_CNTR    3
static void AntennaUpdateInStandByCh1_SM (void)
{
    switch (standby_ch1)
    {
    case INIT_SEARCH:
        // get ready for a new antenna search
        AntennaResetNewParamsCh1;
        AntennaResetParamsCh1;        
        AntennaResetNameCh1;
        AntennaResetConnectionCh1;
        AntennaSetStableParamsCh1;    //stable in no connect
        standby_ch1++;
        break;

    case NO_CONN:
        // ask for params
        if ((AntennaCheckParamsCh1) &&
            (AntennaCheckNewParamsCh1))
        {
            keepalive_ch1 = KEEP_ALIVE_NAME_CNTR;
            standby_ch1++;
            AntennaResetNewParamsCh1;    //from here if we get params again, its a reconnect
            
            UART_CH1_Send("get_name\r\n");
        }
        else
            UART_CH1_Send("get_params\r\n");

        break;
        
    case GET_NAME:
        if (keepalive_ch1)
        {
            keepalive_ch1--;
            if (AntennaCheckNameCh1)
            {
                AntennaResetConnectionCh1;
                AntennaSetStableParamsCh1;    //stable in conn with name                
                keepalive_ch1 = KEEP_ALIVE_CNTR;
                standby_ch1++;
                UART_CH1_Send("keepalive\r\n");                
            }
            else
                UART_CH1_Send("get_name\r\n");

        }
        else
        {
            //no name sended
            AntennaResetConnectionCh1;
            AntennaSetStableParamsCh1;    //stable in conn without name
            keepalive_ch1 = KEEP_ALIVE_CNTR;
            standby_ch1++;
            UART_CH1_Send("keepalive\r\n");                
        }

        if (AntennaCheckNewParamsCh1)    //new antenna connected
            standby_ch1 = INIT_SEARCH;
        
        break;

    case IN_STANDBY:
        if (keepalive_ch1)
        {
            if (AntennaCheckConnectionCh1)
            {
                AntennaResetConnectionCh1;
                keepalive_ch1 = KEEP_ALIVE_CNTR;
            }

            keepalive_ch1--;
            UART_CH1_Send("keepalive\r\n");
        }
        else
        {
            // connection lost
            standby_ch1 = INIT_SEARCH;
        }

        if (AntennaCheckNewParamsCh1)    //new antenna connected
            standby_ch1 = INIT_SEARCH;
        
        break;

    }
}


static void AntennaUpdateInStandByCh2_SM (void)
{
    switch (standby_ch2)
    {
    case INIT_SEARCH:
        // get ready for a new antenna search
        AntennaResetNewParamsCh2;
        AntennaResetParamsCh2;        
        AntennaResetNameCh2;
        AntennaResetConnectionCh2;
        AntennaSetStableParamsCh2;    //stable in no connect
        standby_ch2++;
        break;

    case NO_CONN:
        // ask for params
        if ((AntennaCheckParamsCh2) &&
            (AntennaCheckNewParamsCh2))
        {
            keepalive_ch2 = KEEP_ALIVE_NAME_CNTR;
            standby_ch2++;
            AntennaResetNewParamsCh2;    //from here if we get params again, its a reconnect
            
            UART_CH2_Send("get_name\r\n");
        }
        else
            UART_CH2_Send("get_params\r\n");

        break;
        
    case GET_NAME:
        if (keepalive_ch2)
        {
            keepalive_ch2--;
            if (AntennaCheckNameCh2)
            {
                AntennaResetConnectionCh2;
                AntennaSetStableParamsCh2;    //stable in conn with name                
                keepalive_ch2 = KEEP_ALIVE_CNTR;
                standby_ch2++;
                UART_CH2_Send("keepalive\r\n");                
            }
            else
                UART_CH2_Send("get_name\r\n");

        }
        else
        {
            //no name sended
            AntennaResetConnectionCh2;
            AntennaSetStableParamsCh2;    //stable in conn without name
            keepalive_ch2 = KEEP_ALIVE_CNTR;
            standby_ch2++;
            UART_CH2_Send("keepalive\r\n");                
        }

        if (AntennaCheckNewParamsCh2)    //new antenna connected
            standby_ch2 = INIT_SEARCH;
        
        break;

    case IN_STANDBY:
        if (keepalive_ch2)
        {
            if (AntennaCheckConnectionCh2)
            {
                AntennaResetConnectionCh2;
                keepalive_ch2 = KEEP_ALIVE_CNTR;
            }

            keepalive_ch2--;
            UART_CH2_Send("keepalive\r\n");
        }
        else
        {
            // connection lost
            standby_ch2 = INIT_SEARCH;
        }

        if (AntennaCheckNewParamsCh2)    //new antenna connected
            standby_ch2 = INIT_SEARCH;
        
        break;

    }
}


static void AntennaUpdateInStandByCh3_SM (void)
{
    switch (standby_ch3)
    {
    case INIT_SEARCH:
        // get ready for a new antenna search
        AntennaResetNewParamsCh3;
        AntennaResetParamsCh3;        
        AntennaResetNameCh3;
        AntennaResetConnectionCh3;
        AntennaSetStableParamsCh3;    //stable in no connect
        standby_ch3++;
        break;

    case NO_CONN:
        // ask for params
        if ((AntennaCheckParamsCh3) &&
            (AntennaCheckNewParamsCh3))
        {
            keepalive_ch3 = KEEP_ALIVE_NAME_CNTR;
            standby_ch3++;
            AntennaResetNewParamsCh3;    //from here if we get params again, its a reconnect
            
            UART_CH3_Send("get_name\r\n");
        }
        else
            UART_CH3_Send("get_params\r\n");

        break;
        
    case GET_NAME:
        if (keepalive_ch3)
        {
            keepalive_ch3--;
            if (AntennaCheckNameCh3)
            {
                AntennaResetConnectionCh3;
                AntennaSetStableParamsCh3;    //stable in conn with name                
                keepalive_ch3 = KEEP_ALIVE_CNTR;
                standby_ch3++;
                UART_CH3_Send("keepalive\r\n");                
            }
            else
                UART_CH3_Send("get_name\r\n");

        }
        else
        {
            //no name sended
            AntennaResetConnectionCh3;
            AntennaSetStableParamsCh3;    //stable in conn without name
            keepalive_ch3 = KEEP_ALIVE_CNTR;
            standby_ch3++;
            UART_CH3_Send("keepalive\r\n");                
        }

        if (AntennaCheckNewParamsCh3)    //new antenna connected
            standby_ch3 = INIT_SEARCH;
        
        break;

    case IN_STANDBY:
        if (keepalive_ch3)
        {
            if (AntennaCheckConnectionCh3)
            {
                AntennaResetConnectionCh3;
                keepalive_ch3 = KEEP_ALIVE_CNTR;
            }

            keepalive_ch3--;
            UART_CH3_Send("keepalive\r\n");
        }
        else
        {
            // connection lost
            standby_ch3 = INIT_SEARCH;
        }

        if (AntennaCheckNewParamsCh3)    //new antenna connected
            standby_ch3 = INIT_SEARCH;
        
        break;

    }
}


static void AntennaUpdateInStandByCh4_SM (void)
{
    switch (standby_ch4)
    {
    case INIT_SEARCH:
        // get ready for a new antenna search
        AntennaResetNewParamsCh4;
        AntennaResetParamsCh4;        
        AntennaResetNameCh4;
        AntennaResetConnectionCh4;
        AntennaSetStableParamsCh4;    //stable in no connect
        standby_ch4++;
        break;

    case NO_CONN:
        // ask for params
        if ((AntennaCheckParamsCh4) &&
            (AntennaCheckNewParamsCh4))
        {
            keepalive_ch4 = KEEP_ALIVE_NAME_CNTR;
            standby_ch4++;
            AntennaResetNewParamsCh4;    //from here if we get params again, its a reconnect
            
            UART_CH4_Send("get_name\r\n");
        }
        else
            UART_CH4_Send("get_params\r\n");

        break;
        
    case GET_NAME:
        if (keepalive_ch4)
        {
            keepalive_ch4--;
            if (AntennaCheckNameCh4)
            {
                AntennaResetConnectionCh4;
                AntennaSetStableParamsCh4;    //stable in conn with name                
                keepalive_ch4 = KEEP_ALIVE_CNTR;
                standby_ch4++;
                UART_CH4_Send("keepalive\r\n");                
            }
            else
                UART_CH4_Send("get_name\r\n");

        }
        else
        {
            //no name sended
            AntennaResetConnectionCh4;
            AntennaSetStableParamsCh4;    //stable in conn without name
            keepalive_ch4 = KEEP_ALIVE_CNTR;
            standby_ch4++;
            UART_CH4_Send("keepalive\r\n");                
        }

        if (AntennaCheckNewParamsCh4)    //new antenna connected
            standby_ch4 = INIT_SEARCH;
        
        break;

    case IN_STANDBY:
        if (keepalive_ch4)
        {
            if (AntennaCheckConnectionCh4)
            {
                AntennaResetConnectionCh4;
                keepalive_ch4 = KEEP_ALIVE_CNTR;
            }

            keepalive_ch4--;
            UART_CH4_Send("keepalive\r\n");
        }
        else
        {
            // connection lost
            standby_ch4 = INIT_SEARCH;
        }

        if (AntennaCheckNewParamsCh4)    //new antenna connected
            standby_ch4 = INIT_SEARCH;
        
        break;

    }
}


void AntennaUpdateStates (void)
{
    switch (antenna_state)
    {
    case ANTENNA_INIT:
        // when the device is power on, force the first info update
        antenna_info_timer = ANTENNA_INFO_TIMER_FIRST_START;
        antenna_info_sended = 0;
        antenna_state++;
        break;

    case ANTENNA_IN_STANDBY:
        if (!antenna_timer)
        {
            AntennaUpdateInStandByCh1_SM();
            AntennaUpdateInStandByCh2_SM();
            AntennaUpdateInStandByCh3_SM();
            AntennaUpdateInStandByCh4_SM();            
            
            antenna_timer = ANTENNA_KEEP_ALIVE_TIMER;
        }

        //check if we have all channels locked for treatment
        //save the effective channels in the treatment
        if (AntennaCheckLockCh1 && AntennaCheckLockCh2 && AntennaCheckLockCh3 && AntennaCheckLockCh4)
        {
            if (AntennaGetConnection(CH1))
                AntennaInTreatmentCh1;

            if (AntennaGetConnection(CH2))
                AntennaInTreatmentCh2;
 
            if (AntennaGetConnection(CH3))
                AntennaInTreatmentCh3;

            if (AntennaGetConnection(CH4))
                AntennaInTreatmentCh4;
                        
            antenna_state++;
        }

        // if some antenna has stable params (conection or disconnection), send it with timer
        if ((AntennaCheckStableParamsCh1) ||
            (AntennaCheckStableParamsCh2) ||
            (AntennaCheckStableParamsCh3) ||
            (AntennaCheckStableParamsCh4))
        {
            if (AntennaCheckStableParamsCh1)
                AntennaResetStableParamsCh1;

            if (AntennaCheckStableParamsCh2)
                AntennaResetStableParamsCh2;

            if (AntennaCheckStableParamsCh3)
                AntennaResetStableParamsCh3;

            if (AntennaCheckStableParamsCh4)
                AntennaResetStableParamsCh4;
            
            if (!antenna_info_timer)
                antenna_info_timer = ANTENNA_INFO_TIMER_UPDATE;
            
            antenna_info_sended = 0;
        }

        //timer exhausted, send knowed antennas info
        if (!antenna_info_timer)
        {
            if (!antenna_info_sended)
            {
                AntennaSendKnowParams();
                antenna_info_sended = 1;
            }
        }

        // if the comms (PC) ask for knowed antennas
        if (antenna_send_info_with_timer)
        {
            if (!antenna_info_timer)
                antenna_info_timer = ANTENNA_INFO_TIMER_UPDATE;
            
            antenna_info_sended = 0;
            antenna_send_info_with_timer = 0;
        }            
        break;

    case ANTENNA_IN_TREATMENT:
        if (!antenna_timer)
        {
            if (AntennaCheckInTreatmentCh1)    //si es una de las que esta en tratamiento
            {
                if (AntennaCheckParamsCh1)
                {
                    if (AntennaCheckConnectionCh1)    //tengo parametros y contesta
                    {
                        UART_CH1_Send("get_temp\r\n");
                        keepalive_ch1 = KEEP_ALIVE_COUNTER;                    
                        AntennaResetConnectionCh1;
                    }
                    else 
                    {
                        if (keepalive_ch1)
                        {
                            keepalive_ch1--;
                            UART_CH1_Send("get_temp\r\n");
                        }
                        else    //estaba conectada pero se perdio
                        {
                            AntennaResetParamsCh1;
                            antenna_send_info_with_timer = 1;
                        }
                    }
                }
            }

            if (AntennaCheckInTreatmentCh2)    //si es una de las que esta en tratamiento
            {                            
                if (AntennaCheckParamsCh2)
                {
                    if (AntennaCheckConnectionCh2)    //tengo parametros y contesta
                    {
                        UART_CH2_Send("get_temp\r\n");
                        keepalive_ch2 = KEEP_ALIVE_COUNTER;                    
                        AntennaResetConnectionCh2;
                    }
                    else 
                    {
                        if (keepalive_ch2)
                        {
                            keepalive_ch2--;
                            UART_CH2_Send("get_temp\r\n");
                        }
                        else    //estaba conectada pero se perdio
                        {
                            AntennaResetParamsCh2;
                            antenna_send_info_with_timer = 1;
                        }
                    }
                }
            }

            if (AntennaCheckInTreatmentCh3)    //si es una de las que esta en tratamiento
            {                                            
                if (AntennaCheckParamsCh3)
                {
                    if (AntennaCheckConnectionCh3)    //tengo parametros y contesta
                    {
                        UART_CH3_Send("get_temp\r\n");
                        keepalive_ch3 = KEEP_ALIVE_COUNTER;                    
                        AntennaResetConnectionCh3;
                    }
                    else 
                    {
                        if (keepalive_ch3)
                        {
                            keepalive_ch3--;
                            UART_CH3_Send("get_temp\r\n");
                        }
                        else    //estaba conectada pero se perdio
                        {                            
                            AntennaResetParamsCh3;
                            antenna_send_info_with_timer = 1;
                        }
                    }
                }
            }

            if (AntennaCheckInTreatmentCh4)    //si es una de las que esta en tratamiento
            {                                                        
                if (AntennaCheckParamsCh4)
                {
                    if (AntennaCheckConnectionCh4)    //tengo parametros y contesta
                    {
                        UART_CH4_Send("get_temp\r\n");
                        keepalive_ch4 = KEEP_ALIVE_COUNTER;                    
                        AntennaResetConnectionCh4;
                    }
                    else 
                    {
                        if (keepalive_ch4)
                        {
                            keepalive_ch4--;
                            UART_CH4_Send("get_temp\r\n");
                        }
                        else    //estaba conectada pero se perdio
                        {
                            AntennaResetParamsCh4;
                            antenna_send_info_with_timer = 1;
                        }
                    }
                }
            }

            antenna_timer = ANTENNA_TEMP_TIMER;
        }

        //si tengo todos los canales en unlock salgo del tratamiento
        if ((!AntennaCheckLockCh1) && (!AntennaCheckLockCh2) && (!AntennaCheckLockCh3) && (!AntennaCheckLockCh4))
        {
            AntennaResetInTreatmentCh1;
            AntennaResetInTreatmentCh2;
            AntennaResetInTreatmentCh3;
            AntennaResetInTreatmentCh4;
            
            antenna_state = ANTENNA_IN_STANDBY;
        }

        //si alguna tuvo update, se desenchufo y enchufo
        if (AntennaCheckNewParamsCh1 && AntennaCheckInTreatmentCh1)
        {
            AntennaResetNewParamsCh1;
            AntennaResetInTreatmentCh1;
            AntennaResetParamsCh1;
            antenna_send_info_with_timer = 1;
        }

        if (AntennaCheckNewParamsCh2 && AntennaCheckInTreatmentCh2)
        {
            AntennaResetNewParamsCh2;
            AntennaResetInTreatmentCh2;
            AntennaResetParamsCh2;
            antenna_send_info_with_timer = 1;
        }

        if (AntennaCheckNewParamsCh3 && AntennaCheckInTreatmentCh3)
        {
            AntennaResetNewParamsCh3;
            AntennaResetInTreatmentCh3;
            AntennaResetParamsCh3;
            antenna_send_info_with_timer = 1;
        }

        if (AntennaCheckNewParamsCh4 && AntennaCheckInTreatmentCh4)
        {
            AntennaResetNewParamsCh4;
            AntennaResetInTreatmentCh4;
            AntennaResetParamsCh4;
            antenna_send_info_with_timer = 1;
        }

        //si estoy en tratamiento puedo enviar info sin esperar
        if (antenna_send_info_with_timer)
        {
            AntennaSendKnowParams();            
            antenna_send_info_with_timer = 0;
        }
        break;

    case ANTENNA_IN_PAUSE:
        break;

    default:
        antenna_state = ANTENNA_INIT;
        break;
    }
}

//la llaman desde el comms para setear los valores actuales de temperatura en la antenas
void AntennaSetCurrentTemp (unsigned char ch, unsigned char t_int, unsigned char t_dec)
{
    switch (ch)
    {
    case CH1:
        ch1_ant_current_temp_int = t_int;
        ch1_ant_current_temp_dec = t_dec;
        AntennaConnectionCh1;        
        break;

    case CH2:
        ch2_ant_current_temp_int = t_int;
        ch2_ant_current_temp_dec = t_dec;
        AntennaConnectionCh2;                
        break;

    case CH3:
        ch3_ant_current_temp_int = t_int;
        ch3_ant_current_temp_dec = t_dec;
        AntennaConnectionCh3;                
        break;

    case CH4:
        ch4_ant_current_temp_int = t_int;
        ch4_ant_current_temp_dec = t_dec;
        AntennaConnectionCh4;                
        break;
    }    
}

//la llaman desde el comms para respuestas del keepalive de antenna
void AntennaIsAnswering (unsigned char ch)
{
    if (ch == CH1)
        AntennaConnectionCh1;

    if (ch == CH2)
        AntennaConnectionCh2;

    if (ch == CH3)
        AntennaConnectionCh3;

    if (ch == CH4)
        AntennaConnectionCh4;
    
}

//info que mandan del modulo comms cuando se conecta una antenna o responde a get_params
void AntennaSetParamsStruct (unsigned char ch, antenna_st *ant)
{
    switch (ch)
    {
    case CH1:
        antenna_conn_ch1.ant_resistance_int = ant->resistance_int;
        antenna_conn_ch1.ant_resistance_dec = ant->resistance_dec;
        antenna_conn_ch1.ant_inductance_int = ant->inductance_int;
        antenna_conn_ch1.ant_inductance_dec = ant->inductance_dec;
        antenna_conn_ch1.ant_current_limit_int = ant->current_limit_int;
        antenna_conn_ch1.ant_current_limit_dec = ant->current_limit_dec;
        antenna_conn_ch1.ant_temp_max_int = ant->temp_max_int;
        antenna_conn_ch1.ant_temp_max_dec = ant->temp_max_dec;
        break;

    case CH2:
        antenna_conn_ch2.ant_resistance_int = ant->resistance_int;
        antenna_conn_ch2.ant_resistance_dec = ant->resistance_dec;
        antenna_conn_ch2.ant_inductance_int = ant->inductance_int;
        antenna_conn_ch2.ant_inductance_dec = ant->inductance_dec;
        antenna_conn_ch2.ant_current_limit_int = ant->current_limit_int;
        antenna_conn_ch2.ant_current_limit_dec = ant->current_limit_dec;
        antenna_conn_ch2.ant_temp_max_int = ant->temp_max_int;
        antenna_conn_ch2.ant_temp_max_dec = ant->temp_max_dec;
        break;

    case CH3:
        antenna_conn_ch3.ant_resistance_int = ant->resistance_int;
        antenna_conn_ch3.ant_resistance_dec = ant->resistance_dec;
        antenna_conn_ch3.ant_inductance_int = ant->inductance_int;
        antenna_conn_ch3.ant_inductance_dec = ant->inductance_dec;
        antenna_conn_ch3.ant_current_limit_int = ant->current_limit_int;
        antenna_conn_ch3.ant_current_limit_dec = ant->current_limit_dec;
        antenna_conn_ch3.ant_temp_max_int = ant->temp_max_int;
        antenna_conn_ch3.ant_temp_max_dec = ant->temp_max_dec;
        break;

    case CH4:
        antenna_conn_ch4.ant_resistance_int = ant->resistance_int;
        antenna_conn_ch4.ant_resistance_dec = ant->resistance_dec;
        antenna_conn_ch4.ant_inductance_int = ant->inductance_int;
        antenna_conn_ch4.ant_inductance_dec = ant->inductance_dec;
        antenna_conn_ch4.ant_current_limit_int = ant->current_limit_int;
        antenna_conn_ch4.ant_current_limit_dec = ant->current_limit_dec;
        antenna_conn_ch4.ant_temp_max_int = ant->temp_max_int;
        antenna_conn_ch4.ant_temp_max_dec = ant->temp_max_dec;
        break;
    }

    //reviso un poco los parametros
    if (((ant->resistance_int) || (ant->resistance_dec)) &&
        ((ant->inductance_int) || (ant->inductance_dec)) &&
        ((ant->current_limit_int) || (ant->current_limit_dec)) &&
        ((ant->temp_max_int) || (ant->temp_max_dec)))
    {
        if (ch == CH1)
        {
            AntennaParamsCh1;
            AntennaSetNewParamsCh1;
            AntennaConnectionCh1;
        }
        
        if (ch == CH2)
        {
            AntennaParamsCh2;
            AntennaSetNewParamsCh2;
            AntennaConnectionCh2;
        }
        
        if (ch == CH3)
        {
            AntennaParamsCh3;
            AntennaSetNewParamsCh3;
            AntennaConnectionCh3;
        }
        
        if (ch == CH4)
        {
            AntennaParamsCh4;
            AntennaSetNewParamsCh4;
            AntennaConnectionCh4;
        }
    }    
}

//me llaman desde comms y GTK_Signal para conocer los parametros de antena
void AntennaGetParamsStruct (unsigned char ch, antenna_st *ant)
{
    switch (ch)
    {
    case CH1:
        ant->resistance_int = antenna_conn_ch1.ant_resistance_int;
        ant->resistance_dec = antenna_conn_ch1.ant_resistance_dec;
        ant->inductance_int = antenna_conn_ch1.ant_inductance_int;
        ant->inductance_dec = antenna_conn_ch1.ant_inductance_dec;
        ant->current_limit_int = antenna_conn_ch1.ant_current_limit_int;
        ant->current_limit_dec = antenna_conn_ch1.ant_current_limit_dec;
        ant->temp_max_int = antenna_conn_ch1.ant_temp_max_int;
        ant->temp_max_dec = antenna_conn_ch1.ant_temp_max_dec;        
        break;

    case CH2:
        ant->resistance_int = antenna_conn_ch2.ant_resistance_int;
        ant->resistance_dec = antenna_conn_ch2.ant_resistance_dec;
        ant->inductance_int = antenna_conn_ch2.ant_inductance_int;
        ant->inductance_dec = antenna_conn_ch2.ant_inductance_dec;
        ant->current_limit_int = antenna_conn_ch2.ant_current_limit_int;
        ant->current_limit_dec = antenna_conn_ch2.ant_current_limit_dec;
        ant->temp_max_int = antenna_conn_ch2.ant_temp_max_int;
        ant->temp_max_dec = antenna_conn_ch2.ant_temp_max_dec;                
        break;

    case CH3:
        ant->resistance_int = antenna_conn_ch3.ant_resistance_int;
        ant->resistance_dec = antenna_conn_ch3.ant_resistance_dec;
        ant->inductance_int = antenna_conn_ch3.ant_inductance_int;
        ant->inductance_dec = antenna_conn_ch3.ant_inductance_dec;
        ant->current_limit_int = antenna_conn_ch3.ant_current_limit_int;
        ant->current_limit_dec = antenna_conn_ch3.ant_current_limit_dec;
        ant->temp_max_int = antenna_conn_ch3.ant_temp_max_int;
        ant->temp_max_dec = antenna_conn_ch3.ant_temp_max_dec;                
        break;

    case CH4:
        ant->resistance_int = antenna_conn_ch4.ant_resistance_int;
        ant->resistance_dec = antenna_conn_ch4.ant_resistance_dec;
        ant->inductance_int = antenna_conn_ch4.ant_inductance_int;
        ant->inductance_dec = antenna_conn_ch4.ant_inductance_dec;
        ant->current_limit_int = antenna_conn_ch4.ant_current_limit_int;
        ant->current_limit_dec = antenna_conn_ch4.ant_current_limit_dec;
        ant->temp_max_int = antenna_conn_ch4.ant_temp_max_int;
        ant->temp_max_dec = antenna_conn_ch4.ant_temp_max_dec;                
        break;
    }        
}


//contesta a la sesion si la antena esta presente/contestando y tiene parametros
unsigned char AntennaGetConnection (unsigned char ch)
{
    unsigned char conn = 0;

    if (ch == CH1)
        if (AntennaCheckParamsCh1 && keepalive_ch1)
            conn = 1;

    if (ch == CH2)
        if (AntennaCheckParamsCh2 && keepalive_ch2)
            conn = 1;

    if (ch == CH3)
        if (AntennaCheckParamsCh3 && keepalive_ch3)
            conn = 1;

    if (ch == CH4)
        if (AntennaCheckParamsCh4 && keepalive_ch4)
            conn = 1;

    return conn;
    
}

//contesta a la sesion la ultima temperatura conocida de la antena
unsigned char AntennaGetCurrentTemp (unsigned char ch)
{
    unsigned char temp = 0;

    if (ch == CH1)
        temp = ch1_ant_current_temp_int;

    if (ch == CH2)
        temp = ch2_ant_current_temp_int;

    if (ch == CH3)
        temp = ch3_ant_current_temp_int;

    if (ch == CH4)
        temp = ch4_ant_current_temp_int;

    return temp;
    
}


// answer 1 in error
// 0 all ok
unsigned char AntennaGetTempStatus (unsigned char channel)
{
    unsigned char ctemp = 0;
    unsigned char stemp = 0;    

    ctemp = AntennaGetCurrentTemp(channel);

    if (channel == CH1)
        stemp = antenna_conn_ch1.ant_temp_max_int;

    if (channel == CH2)
        stemp = antenna_conn_ch2.ant_temp_max_int;

    if (channel == CH3)
        stemp = antenna_conn_ch3.ant_temp_max_int;

    if (channel == CH4)
        stemp = antenna_conn_ch4.ant_temp_max_int;    

    if (ctemp > stemp)
        return 1;

    return 0;
}


//llaman desde la sesion para saber si tiene antena presente y avisar del comienzo
//de un nuevo tratamiento
unsigned char AntennaVerifyForTreatment (unsigned char ch)
{
    resp_e resp = resp_error;

    switch (ch)
    {
    case CH1:
        if (AntennaCheckParamsCh1)
        {
            keepalive_ch1 = KEEP_ALIVE_COUNTER;
            resp = resp_ok;
        }

        AntennaLockCh1;
        break;

    case CH2:
        if (AntennaCheckParamsCh2)
        {
            keepalive_ch2 = KEEP_ALIVE_COUNTER;            
            resp = resp_ok;
        }

        AntennaLockCh2;
        break;

    case CH3:
        if (AntennaCheckParamsCh3)
        {
            keepalive_ch3 = KEEP_ALIVE_COUNTER;            
            resp = resp_ok;
        }

        AntennaLockCh3;
        break;

    case CH4:
        if (AntennaCheckParamsCh4)
        {
            keepalive_ch4 = KEEP_ALIVE_COUNTER;
            resp = resp_ok;
        }

        AntennaLockCh4;
        break;

    default:
        break;
    }

    return resp;
}

void AntennaEndTreatment (unsigned char ch)
{
    if (ch == CH1)
        AntennaUnlockCh1;

    if (ch == CH2)
        AntennaUnlockCh2;

    if (ch == CH3)
        AntennaUnlockCh3;

    if (ch == CH4)
        AntennaUnlockCh4;
}

// void AntennaBackupParams (antenna_st *plock_in, antenna_complete_st *psession)
// {
//     plock_in->resistance_int = psession->ant_resistance_int;
//     plock_in->resistance_dec = psession->ant_resistance_dec;
//     plock_in->inductance_int = psession->ant_inductance_int;
//     plock_in->inductance_dec = psession->ant_inductance_dec;
//     plock_in->current_limit_int = psession->ant_current_limit_int;
//     plock_in->current_limit_dec = psession->ant_current_limit_dec;
//     plock_in->temp_max_int = psession->ant_temp_max_int;
//     plock_in->temp_max_dec = psession->ant_temp_max_dec;    
// }

// void AntennaFlushParamsInLock (antenna_st * plock_in)
// {
//     plock_in->resistance_int = 0;
//     plock_in->resistance_dec = 0;
//     plock_in->inductance_int = 0;
//     plock_in->inductance_dec = 0;
//     plock_in->current_limit_int = 0;
//     plock_in->current_limit_dec = 0;
//     plock_in->temp_max_int = 0;
//     plock_in->temp_max_dec = 0;    
// }

// void AntennaFlushParams (unsigned char ch)
// {
//     switch (ch)
//     {
//     case CH1:
//         antenna_conn_ch1.ant_resistance_int = 0;
//         antenna_conn_ch1.ant_resistance_dec = 0;
//         antenna_conn_ch1.ant_inductance_int = 0;
//         antenna_conn_ch1.ant_inductance_dec = 0;
//         antenna_conn_ch1.ant_current_limit_int = 0;
//         antenna_conn_ch1.ant_current_limit_dec = 0;
//         antenna_conn_ch1.ant_temp_max_int = 0;
//         antenna_conn_ch1.ant_temp_max_dec = 0;
//         break;

//     case CH2:
//         antenna_conn_ch2.ant_resistance_int = 0;
//         antenna_conn_ch2.ant_resistance_dec = 0;
//         antenna_conn_ch2.ant_inductance_int = 0;
//         antenna_conn_ch2.ant_inductance_dec = 0;
//         antenna_conn_ch2.ant_current_limit_int = 0;
//         antenna_conn_ch2.ant_current_limit_dec = 0;
//         antenna_conn_ch2.ant_temp_max_int = 0;
//         antenna_conn_ch2.ant_temp_max_dec = 0;
//         break;

//     case CH3:
//         antenna_conn_ch3.ant_resistance_int = 0;
//         antenna_conn_ch3.ant_resistance_dec = 0;
//         antenna_conn_ch3.ant_inductance_int = 0;
//         antenna_conn_ch3.ant_inductance_dec = 0;
//         antenna_conn_ch3.ant_current_limit_int = 0;
//         antenna_conn_ch3.ant_current_limit_dec = 0;
//         antenna_conn_ch3.ant_temp_max_int = 0;
//         antenna_conn_ch3.ant_temp_max_dec = 0;
//         break;

//     case CH4:
//         antenna_conn_ch4.ant_resistance_int = 0;
//         antenna_conn_ch4.ant_resistance_dec = 0;
//         antenna_conn_ch4.ant_inductance_int = 0;
//         antenna_conn_ch4.ant_inductance_dec = 0;
//         antenna_conn_ch4.ant_current_limit_int = 0;
//         antenna_conn_ch4.ant_current_limit_dec = 0;
//         antenna_conn_ch4.ant_temp_max_int = 0;
//         antenna_conn_ch4.ant_temp_max_dec = 0;
//         break;
//     }        
// }

void AntennaSendKnowParams (void)
{
    unsigned char some_sended = 0;
    unsigned char antbuff [128];

    if (AntennaGetConnection(CH1))
    {
        if (AntennaCheckNameCh1)
        {
            sprintf((char *)antbuff, "%s,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,1\r\n",
                    antenna_conn_ch1.connected_ant_name,
                    antenna_conn_ch1.ant_resistance_int,
                    antenna_conn_ch1.ant_resistance_dec,
                    antenna_conn_ch1.ant_inductance_int,
                    antenna_conn_ch1.ant_inductance_dec,
                    antenna_conn_ch1.ant_current_limit_int,
                    antenna_conn_ch1.ant_current_limit_dec,
                    antenna_conn_ch1.ant_temp_max_int,
                    antenna_conn_ch1.ant_temp_max_dec);            
        }
        else
        {
            sprintf((char *)antbuff, "ch1,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,1\r\n",
                    antenna_conn_ch1.ant_resistance_int,
                    antenna_conn_ch1.ant_resistance_dec,
                    antenna_conn_ch1.ant_inductance_int,
                    antenna_conn_ch1.ant_inductance_dec,
                    antenna_conn_ch1.ant_current_limit_int,
                    antenna_conn_ch1.ant_current_limit_dec,
                    antenna_conn_ch1.ant_temp_max_int,
                    antenna_conn_ch1.ant_temp_max_dec);
        }

        some_sended = 1;
        RPI_Send((char *) antbuff);
    }

    if (AntennaGetConnection(CH2))
    {
        if (AntennaCheckNameCh2)
        {
            sprintf((char *)antbuff, "%s,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,2\r\n",
                    antenna_conn_ch2.connected_ant_name,
                    antenna_conn_ch2.ant_resistance_int,
                    antenna_conn_ch2.ant_resistance_dec,
                    antenna_conn_ch2.ant_inductance_int,
                    antenna_conn_ch2.ant_inductance_dec,
                    antenna_conn_ch2.ant_current_limit_int,
                    antenna_conn_ch2.ant_current_limit_dec,
                    antenna_conn_ch2.ant_temp_max_int,
                    antenna_conn_ch2.ant_temp_max_dec);
            
        }
        else
        {
            sprintf((char *)antbuff, "ch2,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,2\r\n",
                    antenna_conn_ch2.ant_resistance_int,
                    antenna_conn_ch2.ant_resistance_dec,
                    antenna_conn_ch2.ant_inductance_int,
                    antenna_conn_ch2.ant_inductance_dec,
                    antenna_conn_ch2.ant_current_limit_int,
                    antenna_conn_ch2.ant_current_limit_dec,
                    antenna_conn_ch2.ant_temp_max_int,
                    antenna_conn_ch2.ant_temp_max_dec);
        }

        some_sended = 1;
        RPI_Send((char *) antbuff);
    }

    if (AntennaGetConnection(CH3))
    {
        if (AntennaCheckNameCh3)            
        {
            sprintf((char *)antbuff, "%s,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,3\r\n",
                    antenna_conn_ch3.connected_ant_name,
                    antenna_conn_ch3.ant_resistance_int,
                    antenna_conn_ch3.ant_resistance_dec,
                    antenna_conn_ch3.ant_inductance_int,
                    antenna_conn_ch3.ant_inductance_dec,
                    antenna_conn_ch3.ant_current_limit_int,
                    antenna_conn_ch3.ant_current_limit_dec,
                    antenna_conn_ch3.ant_temp_max_int,
                    antenna_conn_ch3.ant_temp_max_dec);            
        }
        else
        {
            sprintf((char *)antbuff, "ch3,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,3\r\n",
                    antenna_conn_ch3.ant_resistance_int,
                    antenna_conn_ch3.ant_resistance_dec,
                    antenna_conn_ch3.ant_inductance_int,
                    antenna_conn_ch3.ant_inductance_dec,
                    antenna_conn_ch3.ant_current_limit_int,
                    antenna_conn_ch3.ant_current_limit_dec,
                    antenna_conn_ch3.ant_temp_max_int,
                    antenna_conn_ch3.ant_temp_max_dec);
        }

        some_sended = 1;
        RPI_Send((char *) antbuff);
    }

    if (AntennaGetConnection(CH4))
    {
        if (AntennaCheckNameCh4)
        {
            sprintf((char *)antbuff, "%s,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,4\r\n",
                    antenna_conn_ch4.connected_ant_name,
                    antenna_conn_ch4.ant_resistance_int,
                    antenna_conn_ch4.ant_resistance_dec,
                    antenna_conn_ch4.ant_inductance_int,
                    antenna_conn_ch4.ant_inductance_dec,
                    antenna_conn_ch4.ant_current_limit_int,
                    antenna_conn_ch4.ant_current_limit_dec,
                    antenna_conn_ch4.ant_temp_max_int,
                    antenna_conn_ch4.ant_temp_max_dec);            
        }
        else
        {
            sprintf((char *)antbuff, "ch4,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,4\r\n",
                    antenna_conn_ch4.ant_resistance_int,
                    antenna_conn_ch4.ant_resistance_dec,
                    antenna_conn_ch4.ant_inductance_int,
                    antenna_conn_ch4.ant_inductance_dec,
                    antenna_conn_ch4.ant_current_limit_int,
                    antenna_conn_ch4.ant_current_limit_dec,
                    antenna_conn_ch4.ant_temp_max_int,
                    antenna_conn_ch4.ant_temp_max_dec);
        }

        some_sended = 1;
        RPI_Send((char *) antbuff);
    }

    if (!some_sended)
        RPI_Send("antenna none\r\n");

}

//me llaman desde comms para conocer las antenas conectadas
void AntennaSendKnowInfoWithTimer (void)
{
    antenna_send_info_with_timer = 1;
}

//me llaman desde comms para setear el nombre de la antena
void AntennaSetName (unsigned char ch, char * pname)
{
    unsigned char len;
    
    len = strlen(pname);
    //si tiene \r final lo quito y ajusto
    if (*(pname + len - 1) == '\r')
    {
        *(pname + len - 1) = '\0';
        len--;
    }

    if (len < (SIZEOF_ANTENNA_NAME - 1))
    {
        if (ch == CH1)
        {
            AntennaNameCh1;            
            strcpy(antenna_conn_ch1.connected_ant_name, pname);
            AntennaConnectionCh1;
        }

        if (ch == CH2)
        {
            AntennaNameCh2;            
            strcpy(antenna_conn_ch2.connected_ant_name, pname);
            AntennaConnectionCh2;
        }

        if (ch == CH3)
        {
            AntennaNameCh3;            
            strcpy(antenna_conn_ch3.connected_ant_name, pname);
            AntennaConnectionCh3;
        }

        if (ch == CH4)
        {
            AntennaNameCh4;
            strcpy(antenna_conn_ch4.connected_ant_name, pname);
            AntennaConnectionCh4;
        }
    }    
}


void AntennaTimeouts (void)
{

    if (antenna_timer)
        antenna_timer--;

    if (antenna_info_timer)
        antenna_info_timer--;

}


//---- end of file ----//
