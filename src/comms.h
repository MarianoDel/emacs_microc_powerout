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

//---- Common Defines --------------------
// #define COMM_NONE_TREAT     0x0000
// #define COMM_START_TREAT    0x0001
// #define COMM_STOP_TREAT     0x0002
// #define COMM_PAUSE_TREAT    0x0004
// #define COMM_CONF_CHANGE    0x0008
// #define COMM_NO_COMM_CH1    0x0100
// #define COMM_NO_COMM_CH2    0x0200
// #define COMM_NO_COMM_CH3    0x0400

// //canales
// #define CH0		0
// #define CH1		1
// #define CH2		2
// #define CH3		3
// #define CH4		4




//--- Exported functions ---//
void Comms_Update (void);
static void Comms_Messages (char * msg_str);
unsigned char Comms_Rpi_Answering (void);

#endif    /* _COMMS_H_ */
