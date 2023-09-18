//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### USART.H ################################
//---------------------------------------------

#ifndef _USART_H_
#define _USART_H_


//--- Exported Macros ------------------------------

//--- Exported functions ---//
void Usart1Config (void);
void Usart1Send (char *);
void Usart1SendUnsigned (unsigned char *, unsigned char);
unsigned char Usart1ReadBuffer (char *, unsigned short);
unsigned char Usart1HaveData (void);
void Usart1HaveDataReset (void);
void USART1_IRQHandler (void);

void Usart2Config (void);
void Usart2Send (char *);
void Usart2SendUnsigned (unsigned char *, unsigned char);
unsigned char Usart2ReadBuffer (char *, unsigned short);
unsigned char Usart2HaveData (void);
void Usart2HaveDataReset (void);
void USART2_IRQHandler (void);
// void ReadUsart2BufferFlush (void);

void Usart3Config (void);
void Usart3Send (char *);
void Usart3SendUnsigned (unsigned char *, unsigned char);
unsigned char Usart3ReadBuffer (char *, unsigned short);
unsigned char Usart3HaveData (void);
void Usart3HaveDataReset (void);
void USART3_IRQHandler (void);

#ifdef STM32F10X_HD
void Uart4Config (void);
void Uart4Send (char *);
void Uart4SendUnsigned (unsigned char *, unsigned char);
unsigned char Uart4ReadBuffer (char *, unsigned short);
unsigned char Uart4HaveData (void);
void Uart4HaveDataReset (void);
void UART4_IRQHandler (void);

void Uart5Config (void);
void Uart5Send (char *);
void Uart5SendUnsigned (unsigned char *, unsigned char);
unsigned char Uart5ReadBuffer (char *, unsigned short);
unsigned char Uart5HaveData (void);
void Uart5HaveDataReset (void);
void UART5_IRQHandler (void);
#endif

#endif
