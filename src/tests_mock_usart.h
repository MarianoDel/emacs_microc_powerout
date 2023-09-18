//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS_MOCK_USART.H ####################
//---------------------------------------------
#ifndef _TESTS_MOCK_USART_H_
#define _TESTS_MOCK_USART_H_


// Module Exported Types Constants and Macros ----------------------------------



// Module Exported Functions ---------------------------------------------------
void Usart1Send (char * msg);
unsigned char Usart1ReadBuffer (unsigned char * bout, unsigned short max_len);
void Usart1FillRxBuffer (char * msg);
unsigned char Usart1HaveData (void);
void Usart1HaveDataReset (void);
void Usart1Callback (void (* pCb) (char *));

void Usart2Send (char * msg);
unsigned char Usart2ReadBuffer (unsigned char * bout, unsigned short max_len);
void Usart2FillRxBuffer (char * msg);
unsigned char Usart2HaveData (void);
void Usart2HaveDataReset (void);
void Usart2Callback (void (* pCb) (char *));

void Usart3Send (char * msg);
void Usart3Callback (void (* pCb) (char *));
unsigned char Usart3HaveData (void);
void Usart3HaveDataReset (void);
void Usart3FillRxBuffer (char * msg);

void Uart4Send (char * msg);
void Uart4Callback (void (* pCb) (char *));
unsigned char Uart4HaveData (void);
void Uart4HaveDataReset (void);
void Uart4FillRxBuffer (char * msg);

void Uart5Send (char * msg);
void Uart5Callback (void (* pCb) (char *));
unsigned char Uart5HaveData (void);
void Uart5HaveDataReset (void);
void Uart5FillRxBuffer (char * msg);

#endif
//--- end of file ---//


