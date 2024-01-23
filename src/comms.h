//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### COMMS.H ###############################
//---------------------------------------------

#ifndef _COMMS_H_
#define _COMMS_H_

//---- Includes to help the Defines ----------



//---- Configurations Defines --------------------
#define RpiSend(X)    Usart1Send(X)
#define RpiHaveData()    Usart1HaveData()
#define RpiHaveDataReset()    Usart1HaveDataReset()
#define RpiReadBuffer(X,Y)    Usart1ReadBuffer(X,Y)


// Module Exported Functions ---------------------------------------------------
void Comms_Timeouts (void);
void Comms_Update (void);
// static void Comms_Messages (char * msg_str);
unsigned char Comms_Rpi_Answering (void);

#endif    /* _COMMS_H_ */
