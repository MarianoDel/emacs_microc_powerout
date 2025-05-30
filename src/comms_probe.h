//---------------------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### COMMS_PROBE.H #####################################
//---------------------------------------------------------
#ifndef _COMMS_PROBE_H_
#define _COMMS_PROBE_H_


// Exported Types Constants and Macros -----------------------------------------
typedef enum {
    PROBE_CONN_RESET,
    PROBE_CONN_NEW,
    PROBE_CONN_STABLISH

} probe_connection_e;


typedef enum {
    PROBE_MODE_NONE,
    PROBE_MODE_SQUARE,
    PROBE_MODE_SINE

} probe_mode_e;


typedef enum {
    INIT_SEARCH,
    NO_CONN,
    IN_STANDBY
    
} probe_status_e;

// Module Exported Functions ---------------------------------------------------
void Probe_Comms_Update (void);
void Comms_Probe_Timeout (void);

probe_mode_e Probe_Get_Mode (void);
probe_status_e Probe_Get_Status (void);

#endif    /* _COMMS_PROBE_H_ */

//--- end of file ---//

