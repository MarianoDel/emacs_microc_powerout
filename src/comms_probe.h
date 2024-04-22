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
    CONN_RESET,
    CONN_NEW,
    CONN_STABLISH

} probe_connection_e;

// Module Exported Functions ---------------------------------------------------
void Probe_Comms_Update (void);
probe_connection_e Probe_Get_Status (void);
void Comms_Probe_Timeout (void);


#endif    /* _COMMS_PROBE_H_ */

//--- end of file ---//

