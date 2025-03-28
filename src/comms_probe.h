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
    PROBE_CONN_STABLISH_MODE_SQUARE,
    PROBE_CONN_STABLISH_MODE_SINE

} probe_connection_e;


// Module Exported Functions ---------------------------------------------------
void Probe_Comms_Update (void);
probe_connection_e Probe_Get_Status (void);
void Comms_Probe_Timeout (void);


#endif    /* _COMMS_PROBE_H_ */

//--- end of file ---//

