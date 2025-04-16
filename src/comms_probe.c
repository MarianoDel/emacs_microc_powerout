//------------------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### COMMS_PROBE.C ##################################
//------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "comms_probe.h"
#include "usart.h"
#include "treatment.h"


#include <string.h>
#include <stdio.h>


// Module Private Types Constants and Macros -----------------------------------
#define SIZEOF_RXDATA    128
#define KEEP_ALIVE_CNTR    3
#define KEEP_ALIVE_TT    1000    


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
char buff [SIZEOF_RXDATA] = { 0 };
char s_ok [] = {"OK\r\n"};
char s_nok [] = {"ERROR\r\n"};
volatile unsigned short keep_alive_timer = 0;
unsigned char probe_keep_cnt = 0;
probe_connection_e probe_connection = PROBE_CONN_RESET;



// Module Private Functions ----------------------------------------------------
void ParseCommsWithProbe (char * str);
void Probe_Set_Connection (probe_connection_e new_conn_status);
void Probe_Set_Mode (probe_mode_e new_mode);
void Probe_Set_Status (probe_status_e new_mode);



// Module Functions ------------------------------------------------------------
void Comms_Probe_Timeout (void)
{
    if (keep_alive_timer)
        keep_alive_timer--;
}


// probes connected to usart3
void Comms_Probe (void)
{
    if (Usart3HaveData ())
    {
        Usart3HaveDataReset ();
        Usart3ReadBuffer (buff, SIZEOF_RXDATA);
        ParseCommsWithProbe(buff);
    }
}


void ParseCommsWithProbe (char * str)
{
    char dummy_str [40] = { 0 };
    
    // name Probe1\r\n
    if (!strncmp(str, "name NervSync", (sizeof("name NervSync") - 1)))
    {
	// clean interface
	Usart1Send ("\r\n");
        strcpy (dummy_str, "new probe NervSync\r\n");
        Usart1Send (dummy_str);
        Probe_Set_Connection (PROBE_CONN_NEW);
	Probe_Set_Mode (PROBE_MODE_SQUARE);
    }

    if (!strncmp(str, "name CellSync", (sizeof("name CellSync") - 1)))
    {
	// clean interface
	Usart1Send ("\r\n");
        strcpy (dummy_str, "new probe CellSync\r\n");
        Usart1Send (dummy_str);
        Probe_Set_Connection (PROBE_CONN_NEW);
	Probe_Set_Mode (PROBE_MODE_SINE);
    }
    
    // keepalive answer
    else if (!strncmp(str, "ok", (sizeof("ok") - 1)))
    {
        if (probe_keep_cnt < KEEP_ALIVE_CNTR)
            probe_keep_cnt++;
    }

    else if (!strncmp(str, "start", (sizeof("start") - 1)))
    {
	if (Probe_Get_Mode() == PROBE_MODE_SQUARE)
	    Usart1Send ("probe start square\r\n");
	else if (Probe_Get_Mode() == PROBE_MODE_SINE)
	    Usart1Send ("probe start sine\r\n");
	
        if (probe_keep_cnt < KEEP_ALIVE_CNTR)
            probe_keep_cnt++;
    }    
}



probe_mode_e probe_mode = PROBE_MODE_NONE;
probe_mode_e Probe_Get_Mode (void)
{
    return probe_mode;
}


void Probe_Set_Mode (probe_mode_e new_mode)
{
    probe_mode = new_mode;
}


void Probe_Set_Connection (probe_connection_e new_conn_status)
{
    probe_connection = new_conn_status;
}


probe_connection_e Probe_Get_Connection (void)
{
    return probe_connection;
}


void Probe_Comms_Update (void)
{
    switch (Probe_Get_Status())
    {
    case INIT_SEARCH:
        // get ready for a new antenna search
        Probe_Set_Connection (PROBE_CONN_RESET);
	Probe_Set_Mode (PROBE_MODE_NONE);
        Probe_Set_Status(NO_CONN);
        break;

    case NO_CONN:
        // wait for probe name/connect
        if (Probe_Get_Connection () == PROBE_CONN_NEW)
        {
            probe_keep_cnt = KEEP_ALIVE_CNTR + 1;    // +1 for first keepalive
            Probe_Set_Status(IN_STANDBY);

            //from here if we get name again, its a reconnect
	    Probe_Set_Status (IN_STANDBY);
	    Probe_Set_Connection (PROBE_CONN_STABLISH);
            keep_alive_timer = 10;
        }
        break;
        
    case IN_STANDBY:
        if (!keep_alive_timer)
        {
            if (probe_keep_cnt)
            {
                probe_keep_cnt--;
                Usart3Send ("keepalive\r\n");
                keep_alive_timer = KEEP_ALIVE_TT;
            }
            else
            {
                // connection lost
                Probe_Set_Status (INIT_SEARCH);
                Usart1Send("none probe\r\n");
            }
        }

        // reconnection? or new antenna
        if (Probe_Get_Connection () == PROBE_CONN_NEW)
            Probe_Set_Status(INIT_SEARCH);
        
        break;

    default:
        Probe_Set_Status(INIT_SEARCH);
        break;
    }

    Comms_Probe ();
}


probe_status_e probe_status = INIT_SEARCH;
probe_status_e Probe_Get_Status (void)
{
    return probe_status;
}


void Probe_Set_Status (probe_status_e new_status)
{
    probe_status = new_status;
}
//---- end of file ----//
