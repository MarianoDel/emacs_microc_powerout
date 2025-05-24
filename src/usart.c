//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### USART.C ################################
//---------------------------------------------

// Includes --------------------------------------------------------------------
#include "usart.h"
#include "stm32f10x.h"
#include "hard.h"

#include <stdio.h>
#include <string.h>


// Private Types Constants and Macros ------------------------------------------
//---- Configurations Defines --------------------
#define SIZEOF_TXDATA        128
#define SIZEOF_RXDATA        128

//---- Common Defines --------------------
// 0xMMMF    Mantissa MMM Fraction F/16
//
#if (defined HSE_CRYSTAL_OSC)
#define USART_PCKL2_9600        0x1D4C    // with xtal HSE 12MHz
#define USART_PCKL1_9600        0x0EA6    // with xtal HSE 12MHz
#pragma message "HSE used as clk input on usart.c"

#elif (defined HSI_INTERNAL_RC)
#if defined SYSCLK_FREQ_64MHz
#define USART_PCKL2_9600    0x1A0A    // with RC internal
#define USART_PCKL1_9600    0x0D05    // with RC internal
#elif defined SYSCLK_FREQ_8MHz
#define USART_PCKL2_9600    0x0341    // with RC internal
#define USART_PCKL1_9600    0x01A0    // with RC internal
#else
#error "wrong sysclk for baudrate on usart.c"
#endif
#pragma message "HSI used as clk input on usart.c"

#else
#error "No clock defined! needed by usart.c"
#endif

#define USART1_9600        USART_PCKL2_9600
#define USART2_9600        USART_PCKL1_9600
#define USART3_9600        USART_PCKL1_9600
#define UART4_9600        USART_PCKL1_9600
#define UART5_9600        USART_PCKL1_9600


#define RCC_USART1_CLK    (RCC->APB2ENR & 0x00004000)
#define RCC_USART1_CLKEN    (RCC->APB2ENR |= 0x00004000)
#define RCC_USART1_CLKDIS    (RCC->APB2ENR &= ~0x00004000)

#define RCC_USART2_CLK    (RCC->APB1ENR & 0x00020000)
#define RCC_USART2_CLKEN    (RCC->APB1ENR |= 0x00020000)
#define RCC_USART2_CLKDIS    (RCC->APB1ENR &= ~0x00020000)

#define RCC_USART3_CLK    (RCC->APB1ENR & 0x00040000)
#define RCC_USART3_CLKEN    (RCC->APB1ENR |= 0x00040000)
#define RCC_USART3_CLKDIS    (RCC->APB1ENR &= ~0x00040000)

#ifdef STM32F10X_HD
#define RCC_UART4_CLK    (RCC->APB1ENR & 0x00080000)
#define RCC_UART4_CLKEN    (RCC->APB1ENR |= 0x00080000)
#define RCC_UART4_CLKDIS    (RCC->APB1ENR &= ~0x00080000)

#define RCC_UART5_CLK    (RCC->APB1ENR & 0x00100000)
#define RCC_UART5_CLKEN    (RCC->APB1ENR |= 0x00100000)
#define RCC_UART5_CLKDIS    (RCC->APB1ENR &= ~0x00100000)
#endif

// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
//--- USART1 ---//
volatile unsigned char * ptx1;
volatile unsigned char * ptx1_pckt_index;
volatile unsigned char * prx1;
volatile unsigned char tx1buff[SIZEOF_TXDATA];
volatile unsigned char rx1buff[SIZEOF_RXDATA];
volatile unsigned char usart1_have_data = 0;

//--- USART2 ---//
volatile unsigned char * ptx2;
volatile unsigned char * ptx2_pckt_index;
volatile unsigned char * prx2;
volatile unsigned char tx2buff[SIZEOF_TXDATA];
volatile unsigned char rx2buff[SIZEOF_RXDATA];
volatile unsigned char usart2_have_data = 0;

//--- USART3 ---//
volatile unsigned char * ptx3;
volatile unsigned char * ptx3_pckt_index;
volatile unsigned char * prx3;
volatile unsigned char tx3buff[SIZEOF_TXDATA];
volatile unsigned char rx3buff[SIZEOF_RXDATA];
volatile unsigned char usart3_have_data = 0;

#ifdef STM32F10X_HD
//--- UART4 ---//
volatile unsigned char * ptx4;
volatile unsigned char * ptx4_pckt_index;
volatile unsigned char * prx4;
volatile unsigned char tx4buff[SIZEOF_TXDATA];
volatile unsigned char rx4buff[SIZEOF_RXDATA];
volatile unsigned char uart4_have_data = 0;

//--- UART5 ---//
volatile unsigned char * ptx5;
volatile unsigned char * ptx5_pckt_index;
volatile unsigned char * prx5;
volatile unsigned char tx5buff[SIZEOF_TXDATA];
volatile unsigned char rx5buff[SIZEOF_RXDATA];
volatile unsigned char uart5_have_data = 0;
#endif

// Module Functions ------------------------------------------------------------
//---- USART1 Functions ----
void Usart1Config(void)
{
    //---- Clk USART1 ----
    if (!RCC_USART1_CLK)
        RCC_USART1_CLKEN;

    //---- Acomodo punteros ----
    ptx1 = tx1buff;
    ptx1_pckt_index = tx1buff;
    prx1 = rx1buff;

    //---- Configuro velocidad y opciones del puerto
    USART1->BRR = USART1_9600;
    // USART1->CR2 |= USART_CR2_STOP_1;	//2 bits stop
    // USART1->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
    // USART1->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_UE;	//SIN TX
    USART1->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;	//para pruebas TX

    //---- Configuro salidas alternativas ----
    // temp = GPIOA->AFR[1];
    // temp &= 0xFFFFF00F;
    // temp |= 0x00000110;	//PA10 -> AF1 PA9 -> AF1
    // GPIOA->AFR[1] = temp;

    //---- Habilito Int y prioridad ----
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_SetPriority(USART1_IRQn, 7);
}


void Usart1Send (char * send)
{
    unsigned char i;

    i = strlen(send);
    Usart1SendUnsigned((unsigned char *) send, i);
}


void Usart1SendUnsigned (unsigned char * send, unsigned char size)
{
    if ((ptx1_pckt_index + size) < &tx1buff[SIZEOF_TXDATA])
    {
        memcpy((unsigned char *)ptx1_pckt_index, send, size);
        ptx1_pckt_index += size;
        USART1->CR1 |= USART_CR1_TXEIE;
    }
}


unsigned char Usart1ReadBuffer (char * bout, unsigned short max_len)
{
    unsigned int len;

    len = prx1 - rx1buff;

    if (len < max_len)
    {
        //el prx1 siempre llega adelantado desde la int, lo corto con un 0
        *prx1 = '\0';
        prx1++;
        len += 1;
        memcpy(bout, (unsigned char *) rx1buff, len);
    }
    else
    {
        memcpy(bout, (unsigned char *) rx1buff, len);
        len = max_len;
    }

    //ajusto punteros de rx luego de la copia
    prx1 = rx1buff;

    return (unsigned char) len;
}


unsigned char Usart1HaveData (void)
{
    return usart1_have_data;
}


void Usart1HaveDataReset (void)
{
    usart1_have_data = 0;
}


void USART1_IRQHandler (void)
{
    unsigned char dummy;

    /* USART in Receive mode --------------------------------------------------*/
    if (USART1->SR & USART_SR_RXNE)
    {
        dummy = USART1->DR & 0x0FF;

        if (prx1 < &rx1buff[SIZEOF_RXDATA - 1])
        {
            //al /r no le doy bola
            if (dummy == '\r')
            {
            }            
            else if ((dummy == '\n') || (dummy == 26))		//26 es CTRL-Z
            {
                *prx1 = '\0';
                usart1_have_data = 1;
                // if (LED)
                // 	LED_OFF;
                // else
                // 	LED_ON;

            }
            else
            {
                *prx1 = dummy;
                prx1++;
            }
        }
        else
            prx1 = rx1buff;    //soluciona problema bloqueo con garbage
    }

    /* USART in Transmit mode -------------------------------------------------*/
    if (USART1->CR1 & USART_CR1_TXEIE)
    {
        if (USART1->SR & USART_SR_TXE)
        {
            if ((ptx1 < &tx1buff[SIZEOF_TXDATA]) && (ptx1 < ptx1_pckt_index))
            {
                USART1->DR = *ptx1;
                ptx1++;
            }
            else
            {
                ptx1 = tx1buff;
                ptx1_pckt_index = tx1buff;
                USART1->CR1 &= ~USART_CR1_TXEIE;
            }
        }
    }

    if ((USART1->SR & USART_SR_ORE) || (USART1->SR & USART_SR_NE) || (USART1->SR & USART_SR_FE))
    {
        dummy = USART1->SR;
        dummy = USART1->DR;
    }
}

//---- USART2 Functions ----
void Usart2Config(void)
{
    //---- Clk USART2 ----
    if (!RCC_USART2_CLK)
        RCC_USART2_CLKEN;

    //---- Acomodo punteros ----
    ptx2 = tx2buff;
    ptx2_pckt_index = tx2buff;
    prx2 = rx2buff;

    //---- Configuro velocidad y opciones del puerto
    USART2->BRR = USART2_9600;
    // USART2->CR2 |= USART_CR2_STOP_1;	//2 bits stop
    // USART2->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
    // USART2->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_UE;	//SIN TX
    USART2->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;	//para pruebas TX

    //---- Configuro salidas alternativas ----
    // temp = GPIOA->AFR[1];
    // temp &= 0xFFFFF00F;
    // temp |= 0x00000110;	//PA10 -> AF1 PA9 -> AF1
    // GPIOA->AFR[1] = temp;

    //---- Habilito Int y prioridad ----
    NVIC_EnableIRQ(USART2_IRQn);
    NVIC_SetPriority(USART2_IRQn, 7);
}


void Usart2Send (char * send)
{
    unsigned char i;

    i = strlen(send);
    Usart2SendUnsigned((unsigned char *) send, i);
}


void Usart2SendUnsigned (unsigned char * send, unsigned char size)
{
    if ((ptx2_pckt_index + size) < &tx2buff[SIZEOF_TXDATA])
    {
        memcpy((unsigned char *)ptx2_pckt_index, send, size);
        ptx2_pckt_index += size;
        USART2->CR1 |= USART_CR1_TXEIE;
    }
}


unsigned char Usart2ReadBuffer (char * bout, unsigned short max_len)
{
    unsigned int len;

    len = prx2 - rx2buff;

    if (len < max_len)
    {
        *prx2 = '\0';    //buffer from int isnt ended with '\0' do it now
        len += 1;    //space for '\0'
    }
    else
    {
        *(bout + max_len - 1) = '\0';
        len = max_len - 1;
    }
    
    memcpy(bout, (unsigned char *) rx2buff, len);    
    prx2 = rx2buff;

    return (unsigned char) len;
}


// void ReadUsart2BufferFlush (void)
// {
//     if (usart2_have_data)
//         usart2_have_data = 0;
    
//     //ajusto punteros de rx
//     prx2 = rx2buff;
// }


unsigned char Usart2HaveData (void)
{
    return usart2_have_data;
}


void Usart2HaveDataReset (void)
{
    usart2_have_data = 0;
}


void USART2_IRQHandler (void)
{
    unsigned char dummy;

    /* USART in Receive mode --------------------------------------------------*/
    if (USART2->SR & USART_SR_RXNE)
    {
        dummy = USART2->DR & 0x0FF;

        if (prx2 < &rx2buff[SIZEOF_RXDATA - 1])
        {
            if (dummy == '\r')    //no le doy bola
            {
            }
            else if ((dummy == '\n') || (dummy == 26))		//26 es CTRL-Z
            {
                *prx2 = '\0';
                usart2_have_data = 1;
                // if (LED)
                // 	LED_OFF;
                // else
                // 	LED_ON;

            }
            else
            {
                *prx2 = dummy;
                prx2++;
            }
        }
        else
            prx2 = rx2buff;    //soluciona problema bloqueo con garbage
    }

    /* USART in Transmit mode -------------------------------------------------*/
    if (USART2->CR1 & USART_CR1_TXEIE)
    {
        if (USART2->SR & USART_SR_TXE)
        {
            if ((ptx2 < &tx2buff[SIZEOF_TXDATA]) && (ptx2 < ptx2_pckt_index))
            {
                USART2->DR = *ptx2;
                ptx2++;
            }
            else
            {
                ptx2 = tx2buff;
                ptx2_pckt_index = tx2buff;
                USART2->CR1 &= ~USART_CR1_TXEIE;
            }
        }
    }

    if ((USART2->SR & USART_SR_ORE) || (USART2->SR & USART_SR_NE) || (USART2->SR & USART_SR_FE))
    {
        dummy = USART2->SR;
        dummy = USART2->DR;
    }
}


//---- USART3 Functions ----
void Usart3Config(void)
{
    //---- Clk USART3 ----
    if (!RCC_USART3_CLK)
        RCC_USART3_CLKEN;

    //---- Acomodo punteros ----
    ptx3 = tx3buff;
    ptx3_pckt_index = tx3buff;
    prx3 = rx3buff;

    //---- Configuro velocidad y opciones del puerto
    USART3->BRR = USART3_9600;
    // USART3->CR2 |= USART_CR2_STOP_1;	//2 bits stop
    // USART3->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
    // USART3->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_UE;	//SIN TX
    USART3->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;	//para pruebas TX

    //---- Configuro salidas alternativas ----
    // temp = GPIOA->AFR[1];
    // temp &= 0xFFFFF00F;
    // temp |= 0x00000110;	//PA10 -> AF1 PA9 -> AF1
    // GPIOA->AFR[1] = temp;

    //---- Habilito Int y prioridad ----
    NVIC_EnableIRQ(USART3_IRQn);
    NVIC_SetPriority(USART3_IRQn, 7);
}


void Usart3Send (char * send)
{
    unsigned char i;

    i = strlen(send);
    Usart3SendUnsigned((unsigned char *) send, i);
}


void Usart3SendUnsigned (unsigned char * send, unsigned char size)
{
    if ((ptx3_pckt_index + size) < &tx3buff[SIZEOF_TXDATA])
    {
        memcpy((unsigned char *)ptx3_pckt_index, send, size);
        ptx3_pckt_index += size;
        USART3->CR1 |= USART_CR1_TXEIE;
    }
}


unsigned char Usart3ReadBuffer (char * bout, unsigned short max_len)
{
    unsigned int len;

    len = prx3 - rx3buff;

    if (len < max_len)
    {
        //el prx3 siempre llega adelantado desde la int, lo corto con un 0
        *prx3 = '\0';
        prx3++;
        len += 1;
        memcpy(bout, (unsigned char *) rx3buff, len);
    }
    else
    {
        memcpy(bout, (unsigned char *) rx3buff, len);
        len = max_len;
    }

    //ajusto punteros de rx luego de la copia
    prx3 = rx3buff;

    return (unsigned char) len;
}


unsigned char Usart3HaveData (void)
{
    return usart3_have_data;
}


void Usart3HaveDataReset (void)
{
    usart3_have_data = 0;
}


void USART3_IRQHandler (void)
{
    unsigned char dummy;

    /* USART in Receive mode --------------------------------------------------*/
    if (USART3->SR & USART_SR_RXNE)
    {
        dummy = USART3->DR & 0x0FF;

        if (prx3 < &rx3buff[SIZEOF_RXDATA - 1])
        {
            //al /r no le doy bola
            if (dummy == '\r')
            {
            }            
            else if ((dummy == '\n') || (dummy == 26))		//26 es CTRL-Z
            {
                *prx1 = '\0';
                usart3_have_data = 1;
            }
            else
            {
                *prx3 = dummy;
                prx3++;
            }
        }
        else
            prx3 = rx3buff;    //soluciona problema bloqueo con garbage
    }

    /* USART in Transmit mode -------------------------------------------------*/
    if (USART3->CR1 & USART_CR1_TXEIE)
    {
        if (USART3->SR & USART_SR_TXE)
        {
            if ((ptx3 < &tx3buff[SIZEOF_TXDATA]) && (ptx3 < ptx3_pckt_index))
            {
                USART3->DR = *ptx3;
                ptx3++;
            }
            else
            {
                ptx3 = tx3buff;
                ptx3_pckt_index = tx3buff;
                USART3->CR1 &= ~USART_CR1_TXEIE;
            }
        }
    }

    if ((USART3->SR & USART_SR_ORE) || (USART3->SR & USART_SR_NE) || (USART3->SR & USART_SR_FE))
    {
        dummy = USART3->SR;
        dummy = USART3->DR;
    }
}


#ifdef STM32F10X_HD
//---- UART4 Functions ----
void Uart4Config(void)
{
    //---- Clk UART4 ----
    if (!RCC_UART4_CLK)
        RCC_UART4_CLKEN;

    //---- Acomodo punteros ----
    ptx4 = tx4buff;
    ptx4_pckt_index = tx4buff;
    prx4 = rx4buff;

    //---- Configuro velocidad y opciones del puerto
    UART4->BRR = UART4_9600;
    // UART4->CR2 |= USART_CR2_STOP_1;	//2 bits stop
    // UART4->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
    // UART4->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_UE;	//SIN TX
    UART4->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;	//para pruebas TX

    //---- Configuro salidas alternativas ----
    // temp = GPIOA->AFR[1];
    // temp &= 0xFFFFF00F;
    // temp |= 0x00000110;	//PA10 -> AF1 PA9 -> AF1
    // GPIOA->AFR[1] = temp;

    //---- Habilito Int y prioridad ----
    NVIC_EnableIRQ(UART4_IRQn);
    NVIC_SetPriority(UART4_IRQn, 7);
}

void Uart4Send (char * send)
{
    unsigned char i;

    i = strlen(send);
    Uart4SendUnsigned((unsigned char *) send, i);
}


void Uart4SendUnsigned (unsigned char * send, unsigned char size)
{
    if ((ptx4_pckt_index + size) < &tx4buff[SIZEOF_TXDATA])
    {
        memcpy((unsigned char *)ptx4_pckt_index, send, size);
        ptx4_pckt_index += size;
        UART4->CR1 |= USART_CR1_TXEIE;
    }
}


unsigned char Uart4ReadBuffer (char * bout, unsigned short max_len)
{
    unsigned int len;

    len = prx4 - rx4buff;

    if (len < max_len)
    {
        //el prx4 siempre llega adelantado desde la int, lo corto con un 0
        *prx4 = '\0';
        prx4++;
        len += 1;
        memcpy(bout, (unsigned char *) rx4buff, len);
    }
    else
    {
        memcpy(bout, (unsigned char *) rx4buff, len);
        len = max_len;
    }

    //ajusto punteros de rx luego de la copia
    prx4 = rx4buff;

    return (unsigned char) len;
}


unsigned char Uart4HaveData (void)
{
    return uart4_have_data;
}


void Uart4HaveDataReset (void)
{
    uart4_have_data = 0;
}


void UART4_IRQHandler (void)
{
    unsigned char dummy;

    /* USART in Receive mode --------------------------------------------------*/
    if (UART4->SR & USART_SR_RXNE)
    {
        dummy = UART4->DR & 0x0FF;

        if (prx4 < &rx4buff[SIZEOF_RXDATA - 1])
        {
            //al /r no le doy bola
            if (dummy == '\r')
            {
            }            
            else if ((dummy == '\n') || (dummy == 26))		//26 es CTRL-Z
            {
                *prx4 = '\0';
                uart4_have_data = 1;
            }
            else
            {
                *prx4 = dummy;
                prx4++;
            }
        }
        else
            prx4 = rx4buff;    //soluciona problema bloqueo con garbage
    }

    /* USART in Transmit mode -------------------------------------------------*/
    if (UART4->CR1 & USART_CR1_TXEIE)
    {
        if (UART4->SR & USART_SR_TXE)
        {
            if ((ptx4 < &tx4buff[SIZEOF_TXDATA]) && (ptx4 < ptx4_pckt_index))
            {
                UART4->DR = *ptx4;
                ptx4++;
            }
            else
            {
                ptx4 = tx4buff;
                ptx4_pckt_index = tx4buff;
                UART4->CR1 &= ~USART_CR1_TXEIE;
            }
        }
    }

    if ((UART4->SR & USART_SR_ORE) || (UART4->SR & USART_SR_NE) || (UART4->SR & USART_SR_FE))
    {
        dummy = UART4->SR;
        dummy = UART4->DR;
    }
}


//---- UART5 Functions ----
void Uart5Config(void)
{
    //---- Clk UART5 ----
    if (!RCC_UART5_CLK)
        RCC_UART5_CLKEN;

    //---- Acomodo punteros ----
    ptx5 = tx5buff;
    ptx5_pckt_index = tx5buff;
    prx5 = rx5buff;

    //---- Configuro velocidad y opciones del puerto
    UART5->BRR = UART5_9600;
    // UART5->CR2 |= USART_CR2_STOP_1;	//2 bits stop
    // UART5->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
    // UART5->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_UE;	//SIN TX
    UART5->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;	//para pruebas TX

    //---- Configuro salidas alternativas ----
    // temp = GPIOA->AFR[1];
    // temp &= 0xFFFFF00F;
    // temp |= 0x00000110;	//PA10 -> AF1 PA9 -> AF1
    // GPIOA->AFR[1] = temp;

    //---- Habilito Int y prioridad ----
    NVIC_EnableIRQ(UART5_IRQn);
    NVIC_SetPriority(UART5_IRQn, 7);
}


void Uart5Send (char * send)
{
    unsigned char i;

    i = strlen(send);
    Uart5SendUnsigned((unsigned char *) send, i);
}


void Uart5SendUnsigned (unsigned char * send, unsigned char size)
{
    if ((ptx5_pckt_index + size) < &tx5buff[SIZEOF_TXDATA])
    {
        memcpy((unsigned char *)ptx5_pckt_index, send, size);
        ptx5_pckt_index += size;
        UART5->CR1 |= USART_CR1_TXEIE;
    }
}


unsigned char Uart5ReadBuffer (char * bout, unsigned short max_len)
{
    unsigned int len;

    len = prx5 - rx5buff;

    if (len < max_len)
    {
        //el prx5 siempre llega adelantado desde la int, lo corto con un 0
        *prx5 = '\0';
        prx5++;
        len += 1;
        memcpy(bout, (unsigned char *) rx5buff, len);
    }
    else
    {
        memcpy(bout, (unsigned char *) rx5buff, len);
        len = max_len;
    }

    //ajusto punteros de rx luego de la copia
    prx5 = rx5buff;

    return (unsigned char) len;
}


unsigned char Uart5HaveData (void)
{
    return uart5_have_data;
}


void Uart5HaveDataReset (void)
{
    uart5_have_data = 0;
}


void UART5_IRQHandler (void)
{
    unsigned char dummy;

    /* USART in Receive mode --------------------------------------------------*/
    if (UART5->SR & USART_SR_RXNE)
    {
        dummy = UART5->DR & 0x0FF;

        if (prx5 < &rx5buff[SIZEOF_RXDATA - 1])
        {
            //al /r no le doy bola
            if (dummy == '\r')
            {
            }            
            else if ((dummy == '\n') || (dummy == 26))		//26 es CTRL-Z
            {
                *prx5 = '\0';
                uart5_have_data = 1;
            }
            else
            {
                *prx5 = dummy;
                prx5++;
            }
        }
        else
            prx5 = rx5buff;    //soluciona problema bloqueo con garbage
    }

    /* USART in Transmit mode -------------------------------------------------*/
    if (UART5->CR1 & USART_CR1_TXEIE)
    {
        if (UART5->SR & USART_SR_TXE)
        {
            if ((ptx5 < &tx5buff[SIZEOF_TXDATA]) && (ptx5 < ptx5_pckt_index))
            {
                UART5->DR = *ptx5;
                ptx5++;
            }
            else
            {
                ptx5 = tx5buff;
                ptx5_pckt_index = tx5buff;
                UART5->CR1 &= ~USART_CR1_TXEIE;
            }
        }
    }

    if ((UART5->SR & USART_SR_ORE) || (UART5->SR & USART_SR_NE) || (UART5->SR & USART_SR_FE))
    {
        dummy = UART5->SR;
        dummy = UART5->DR;
    }
}

#endif
//---- End of File ----//
