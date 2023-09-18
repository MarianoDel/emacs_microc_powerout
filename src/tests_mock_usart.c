//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS_MOCK_USART.C ####################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "tests_mock_usart.h"
#include "tests_ok.h"

#include <stdio.h>
#include <string.h>


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
// Mock the Usart1
unsigned char usart1_have_data = 0;
char usart1_msg_received [200] = { 0 };
char usart1_msg_sended [200] = { 0 };

void (*pU1Cb) (char *);

// Mock the Usart2
unsigned char usart2_have_data = 0;
unsigned char usart2_have_activation_buffer = 0;
char usart2_msg_received [200] = { 0 };
char usart2_msg_sended [200] = { 0 };

void (*pU2Cb) (char *);

// Mock the Usart3
unsigned char usart3_have_data = 0;
char usart3_msg_received [200] = { 0 };
char usart3_msg_sended [200] = { 0 };

void (*pU3Cb) (char *);

// Mock the Uart4
unsigned char uart4_have_data = 0;
char uart4_msg_received [200] = { 0 };
char uart4_msg_sended [200] = { 0 };

void (*pU4Cb) (char *);

// Mock the Uart5
unsigned char uart5_have_data = 0;
char uart5_msg_received [200] = { 0 };
char uart5_msg_sended [200] = { 0 };

void (*pU5Cb) (char *);

// Module Functions ------------------------------------------------------------
///////////////////////////
// Usart1 Mock Functions //
///////////////////////////
void Usart1Send (char * msg)
{
    strcpy(usart1_msg_sended, msg);
    PrintYellow("usart1 tx -> ");
    printf("%s\n", msg);
    if (pU1Cb != NULL)
        pU1Cb(msg);
    
}


void Usart1Callback (void (* pCb) (char *))
{
    pU1Cb = pCb;
}


unsigned char Usart1ReadBuffer (unsigned char * bout, unsigned short max_len)
{
    unsigned char len = 0;
    len = strlen(usart1_msg_received);
    if (max_len > len)
        strcpy(bout, usart1_msg_received);
    else
        printf("error on Usart1ReadBuffer max_len\n");

    return len;
}


void Usart1FillRxBuffer (char * msg)
{
    strcpy(usart1_msg_received, msg);
    PrintYellow("usart1 rx <- ");
    printf("%s\n", msg);
    usart1_have_data = 1;
}


unsigned char Usart1HaveData (void)
{
    return usart1_have_data;
}


void Usart1HaveDataReset (void)
{
    usart1_have_data = 0;
}


///////////////////////////
// Usart2 Mock Functions //
///////////////////////////
void Usart2Send (char * msg)
{
    strcpy(usart2_msg_sended, msg);
    // PrintBoldWhite("usart2 tx -> ");
    PrintYellow("usart2 tx -> ");    
    printf("%s\n", msg);
    if (pU2Cb != NULL)
        pU2Cb(msg);
}


void Usart2Callback (void (* pCb) (char *))
{
    pU2Cb = pCb;
}


unsigned char Usart2ReadBuffer (unsigned char * bout, unsigned short max_len)
{
    unsigned char len = 0;
    len = strlen(usart2_msg_received);
    if (max_len > len)
        strcpy(bout, usart2_msg_received);
    else
        printf("error on Usart2ReadBuffer max_len\n");

    return len;
}


void Usart2FillRxBuffer (char * msg)
{
    strcpy(usart2_msg_received, msg);
    PrintYellow("usart2 rx <- ");
    printf("%s\n", msg);
    usart2_have_data = 1;
}


unsigned char Usart2HaveData (void)
{
    return usart2_have_data;
}


void Usart2HaveDataReset (void)
{
    usart2_have_data = 0;
}


unsigned char Usart2HaveActivationBuffer (void)
{
    return usart2_have_activation_buffer;
}


void Usart2HaveActivationBufferReset (void)
{
    usart2_have_activation_buffer = 0;
}

///////////////////////////
// Usart3 Mock Functions //
///////////////////////////
void Usart3Send (char * msg)
{
    strcpy(usart3_msg_sended, msg);
    PrintYellow("usart3 tx -> ");
    printf("%s\n", msg);
    if (pU3Cb != NULL)
        pU3Cb(msg);
}


void Usart3Callback (void (* pCb) (char *))
{
    pU3Cb = pCb;
}


unsigned char Usart3HaveData (void)
{
    return usart3_have_data;
}


void Usart3HaveDataReset (void)
{
    usart3_have_data = 0;
}

unsigned char Usart3ReadBuffer (unsigned char * bout, unsigned short max_len)
{
    unsigned char len = 0;
    len = strlen(usart3_msg_received);
    if (max_len > len)
        strcpy(bout, usart3_msg_received);
    else
        printf("error on Usart3ReadBuffer max_len\n");

    return len;
}


void Usart3FillRxBuffer (char * msg)
{
    strcpy(usart3_msg_received, msg);
    PrintYellow("usart3 rx <- ");
    printf("%s\n", msg);
    usart3_have_data = 1;
}


//////////////////////////
// Uart4 Mock Functions //
//////////////////////////
void Uart4Send (char * msg)
{
    strcpy(uart4_msg_sended, msg);
    PrintYellow("uart4 tx -> ");
    printf("%s\n", msg);
    if (pU4Cb != NULL)
        pU4Cb(msg);

}


void Uart4Callback (void (* pCb) (char *))
{
    pU4Cb = pCb;
}


unsigned char Uart4HaveData (void)
{
    return uart4_have_data;
}


void Uart4HaveDataReset (void)
{
    uart4_have_data = 0;
}

unsigned char Uart4ReadBuffer (unsigned char * bout, unsigned short max_len)
{
    unsigned char len = 0;
    len = strlen(uart4_msg_received);
    if (max_len > len)
        strcpy(bout, uart4_msg_received);
    else
        printf("error on Uart4ReadBuffer max_len\n");

    return len;
}


void Uart4FillRxBuffer (char * msg)
{
    strcpy(uart4_msg_received, msg);
    PrintYellow("uart4 rx <- ");
    printf("%s\n", msg);
    uart4_have_data = 1;
}


//////////////////////////
// Uart5 Mock Functions //
//////////////////////////
void Uart5Send (char * msg)
{
    strcpy(uart5_msg_sended, msg);
    PrintYellow("uart5 tx -> ");
    printf("%s\n", msg);
    if (pU5Cb != NULL)
        pU5Cb(msg);
}


void Uart5Callback (void (* pCb) (char *))
{
    pU5Cb = pCb;
}


unsigned char Uart5HaveData (void)
{
    return uart5_have_data;
}


void Uart5HaveDataReset (void)
{
    uart5_have_data = 0;
}

unsigned char Uart5ReadBuffer (unsigned char * bout, unsigned short max_len)
{
    unsigned char len = 0;
    len = strlen(uart5_msg_received);
    if (max_len > len)
        strcpy(bout, uart5_msg_received);
    else
        printf("error on Uart5ReadBuffer max_len\n");

    return len;
}


void Uart5FillRxBuffer (char * msg)
{
    strcpy(uart5_msg_received, msg);
    PrintYellow("uart5 rx <- ");
    printf("%s\n", msg);
    uart5_have_data = 1;
}


//--- end of file ---//


