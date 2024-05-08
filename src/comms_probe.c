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

typedef enum {
    INIT_SEARCH,
    NO_CONN,
    IN_STANDBY
    
} probe_status_e;
    


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
char buff [SIZEOF_RXDATA] = { 0 };
char s_ok [] = {"OK\r\n"};
char s_nok [] = {"ERROR\r\n"};
volatile unsigned short keep_alive_timer = 0;
unsigned char probe_keep_cnt = 0;
probe_connection_e probe_connection = CONN_RESET;
probe_status_e probe_status = INIT_SEARCH;


// Module Private Functions ----------------------------------------------------
void ParseCommsWithProbe (char * str);
void Probe_Set_Connection (probe_connection_e new_conn_status);



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

    // antenna name
    
    // // new model name Probe1.\r\n
    // if (!strncmp(str, "name ", (sizeof("name ") - 1)))
    // {
    //     char sname [21] = { 0 };        
    //     char * ps = str + 5;
    //     int finded = 0;
        
    //     for (int i = 0; i < 20; i++)
    //     {
    //         if (*(ps + i) == '.')
    //         {
    //             if (i > 1)    // at least two chars
    //             {
    //                 finded = 1;
    //                 break;
    //             }
    //         }
    //         else
    //             sname[i] = *(ps + i);

    //     }

    //     if (finded)
    //     {
    //         sprintf (dummy_str, "new probe %s\r\n", sname);
    //         Usart1Send (dummy_str);
    //         Probe_Set_Connection (CONN_NEW);
    //     }
    // }
    
    // name Probe1\r\n
    if (!strncmp(str, "name ", (sizeof("name ") - 1)))
    {
        sprintf (dummy_str, "new probe %s\r\n", str + 5);
        Usart1Send (dummy_str);
        Probe_Set_Connection (CONN_NEW);
        // Wait_ms(2);
        // Usart3Send ("keepalive\r\n");
    }

    // keepalive answer
    else if (!strncmp(str, "ok", (sizeof("ok") - 1)))
    {
        if (probe_keep_cnt < KEEP_ALIVE_CNTR)
            probe_keep_cnt++;
    }

    else if (!strncmp(str, "start", (sizeof("start") - 1)))
    {
        Usart1Send ("probe start\r\n");
        // Treatment_Start ();
        if (probe_keep_cnt < KEEP_ALIVE_CNTR)
            probe_keep_cnt++;
    }    
}


unsigned char Probe_Get_Status (void)
{
    return probe_connection;
}


void Probe_Set_Connection (probe_connection_e new_conn_status)
{
    probe_connection = new_conn_status;
}


void Probe_Comms_Update (void)
{
    switch (probe_status)
    {
    case INIT_SEARCH:
        // get ready for a new antenna search
        Probe_Set_Connection (CONN_RESET);
        probe_status++;
        break;

    case NO_CONN:
        // wait for probe name/connect
        if (Probe_Get_Status () == CONN_NEW)
        {
            probe_keep_cnt = KEEP_ALIVE_CNTR + 1;    // +1 for first keepalive
            probe_status++;

            //from here if we get name again, its a reconnect            
            Probe_Set_Connection (CONN_STABLISH);
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
                probe_status = INIT_SEARCH;                
            }
        }

        // reconnection? or new antenna
        if (Probe_Get_Status () == CONN_NEW)
            probe_status = INIT_SEARCH;
        
        break;

    default:
        probe_status = INIT_SEARCH;
        break;
    }

    Comms_Probe ();
}

//---- end of file ----//
