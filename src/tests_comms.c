//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "comms.h"

// helper modules
#include "tests_ok.h"
#include "tests_mock_usart.h"

#include <stdio.h>
#include <string.h>


// Types Constants and Macros --------------------------------------------------


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
int cb_usart_value = 0;
int treat_func = 0;



// Module Functions to Test ----------------------------------------------------
void Test_Comms (void);


// Module Auxiliary Functions --------------------------------------------------
void CB_Usart (char * s);


// Tests Module Auxiliary or General Functions ---------------------------------


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{

    Test_Comms ();

    return 0;
}


void Test_Comms (void)
{
    // set callback on usart1 
    Usart1Callback (CB_Usart);

    char test_str [200];

    // -- config messages for signals --
    strcpy (test_str, "ch2 frequency 320Hz\r\n");
    Usart1FillRxBuffer (test_str);
    treat_func = 0;
    
    Comms_Update ();

    printf(" tests not own frequency set: ");
    if (treat_func == 0)
        PrintOK();
    else
        PrintERR();

    
    strcpy (test_str, "ch1 frequency 320Hz\r\n");
    Usart1FillRxBuffer (test_str);
    treat_func = 0;
    
    Comms_Update ();

    printf(" tests on frequency set: ");
    if (treat_func == 1)
        PrintOK();
    else
        PrintERR();

    strcpy (test_str, "ch1 intensity 200uA\r\n");
    Usart1FillRxBuffer (test_str);
    treat_func = 0;
    
    Comms_Update ();

    printf(" tests on intensity set: ");
    if (treat_func == 2)
        PrintOK();
    else
        PrintERR();

    strcpy (test_str, "ch1 polarity neg\r\n");
    Usart1FillRxBuffer (test_str);
    treat_func = 0;
    
    Comms_Update ();

    printf(" tests on polarity set: ");
    if (treat_func == 3)
        PrintOK();
    else
        PrintERR();

    strcpy (test_str, "ch1 mode square\r\n");
    Usart1FillRxBuffer (test_str);
    treat_func = 0;
    
    Comms_Update ();

    printf(" tests on mode set: ");
    if (treat_func == 4)
        PrintOK();
    else
        PrintERR();

    strcpy (test_str, "ch1 threshold 100\r\n");
    Usart1FillRxBuffer (test_str);
    treat_func = 0;
    
    Comms_Update ();

    printf(" tests on threshold set: ");
    if (treat_func == 5)
        PrintOK();
    else
        PrintERR();


    // -- operation messages --    
    strcpy (test_str, "ch1 start\r\n");
    Usart1FillRxBuffer (test_str);
    treat_func = 0;
    
    Comms_Update ();

    printf(" tests on start set: ");
    if (treat_func == 6)
        PrintOK();
    else
        PrintERR();


    strcpy (test_str, "ch1 stop\r\n");
    Usart1FillRxBuffer (test_str);
    treat_func = 0;
    
    Comms_Update ();

    printf(" tests on stop set: ");
    if (treat_func == 7)
        PrintOK();
    else
        PrintERR();


    // -- measures messages --    
    strcpy (test_str, "ch1 set_gain 10\r\n");
    Usart1FillRxBuffer (test_str);
    treat_func = 0;
    
    Comms_Update ();

    printf(" tests on set_gain set: ");
    if (treat_func == 8)
        PrintOK();
    else
        PrintERR();

    strcpy (test_str, "ch1 get_gain\r\n");
    Usart1FillRxBuffer (test_str);
    treat_func = 0;
    
    Comms_Update ();

    printf(" tests on get_gain set: ");
    if (treat_func == 9)
        PrintOK();
    else
        PrintERR();

    
    // -- board messages --    
    strcpy (test_str, "ch1 voltages\r\n");
    Usart1FillRxBuffer (test_str);
    treat_func = 0;
    
    Comms_Update ();

    printf(" tests on voltages set: ");
    if (treat_func == 10)
        PrintOK();
    else
        PrintERR();


    strcpy (test_str, "ch1 hard_soft\r\n");
    Usart1FillRxBuffer (test_str);
    treat_func = 0;
    
    Comms_Update ();

    printf(" tests on hard_soft set: ");
    if (treat_func == 11)
        PrintOK();
    else
        PrintERR();

    strcpy (test_str, "chf hard_soft\r\n");
    Usart1FillRxBuffer (test_str);
    treat_func = 0;
    
    Comms_Update ();

    printf(" tests on chf hard_soft set: ");
    if (treat_func == 11)
        PrintOK();
    else
        PrintERR();
    
    
}


// void Test_Comms_With_Usart (void)
// {
//     // set callback on usart1 RpiSend()
//     Usart1Callback(CB_Usart);


//     cb_usart_value = 0;
//     Usart2FillRxBuffer("loco\r\n");    
//     Comms_Channel1 ();

//     printf("Test error string on channel 1: ");
//     if (cb_usart_value == 0)    //no answer
//         PrintOK();
//     else
//         PrintERR();


//     // cb_usart_value = 0;    
//     // Usart2FillRxBuffer("name:Tunel\r\n");
//     // Comms_Channel1 ();
    
//     // printf("Test antenna name channel 1: ");
//     // if (cb_usart_value == 2)
//     //     PrintOK();
//     // else
//     //     PrintERR();


//     char my_ant_str [] = { "ant3,003.50,019.00,003.50,065.00\r\n" };
//     cb_usart_value = 0;    
//     Usart2FillRxBuffer(my_ant_str);
//     Comms_Channel1 ();
//     global_ant.resistance_int = 0;
//     global_ant.resistance_dec = 0;
//     global_ant.inductance_int = 0;
//     global_ant.inductance_dec = 0;

    
//     printf("Test old antenna & parse: ");
//     if ((cb_usart_value == 4) &&
//         (global_ant.resistance_int == 0) &&
//         (global_ant.resistance_dec == 0) &&
//         (global_ant.inductance_int == 0) &&
//         (global_ant.inductance_dec == 0))
//     {
//         PrintOK();
//     }
//     else
//         PrintERR();

//     cb_usart_value = 0;    
//     Usart2FillRxBuffer("temp,055.55\r\n");
//     Comms_Channel1 ();
    
//     printf("Test antenna current temp: ");
//     if (cb_usart_value == 2)
//         PrintOK();
//     else
//         PrintERR();

//     cb_usart_value = 0;
//     antenna_answered = 0;    
//     Usart2FillRxBuffer("ok\r\n");
//     Comms_Channel1 ();
    
//     printf("Test antenna keepalive answer: ");
//     if ((cb_usart_value == 0) && (antenna_answered))
//         PrintOK();
//     else
//         PrintERR();

//     char my_ant_str2 [] = { "ant3,0aa.50,019.00,003.50,065.50\r\n" };    
//     cb_usart_value = 0;    
//     Usart2FillRxBuffer(my_ant_str2);
//     Comms_Channel1 ();
    
//     global_ant.resistance_int = 0;
//     global_ant.resistance_dec = 0;
//     global_ant.inductance_int = 0;
//     global_ant.inductance_dec = 0;
    
//     printf("Test new antenna with errors on params: ");
//     if ((cb_usart_value == 3) &&
//         (global_ant.resistance_int == 0) &&
//         (global_ant.resistance_dec == 0) &&
//         (global_ant.inductance_int == 0) &&
//         (global_ant.inductance_dec == 0))
//         PrintOK();
//     else
//         PrintERR();

//     cb_usart_value = 0;    
//     Usart2FillRxBuffer("temp,0aa.55\r\n");
//     Comms_Channel1 ();
    
//     printf("Test antenna error on temp integer: ");
//     if (cb_usart_value == 0)
//         PrintOK();
//     else
//         PrintERR();

//     cb_usart_value = 0;    
//     Usart2FillRxBuffer("temp,055.aa\r\n");
//     Comms_Channel1 ();
    
//     printf("Test antenna error on temp decimal: ");
//     if (cb_usart_value == 0)
//         PrintOK();
//     else
//         PrintERR();

//     global_ant.resistance_int = 0;
//     global_ant.resistance_dec = 0;
//     global_ant.inductance_int = 0;
//     global_ant.inductance_dec = 0;
        
//     strcpy (my_ant_str2,"ant3,003.50,019.00,003.50,065.50\r\n");
//     cb_usart_value = 0;    
//     Usart2FillRxBuffer(my_ant_str2);
//     Comms_Channel1 ();
    
//     printf("Test new antenna with good params: ");
//     if ((cb_usart_value == 3) &&
//         (global_ant.resistance_int == 3) &&
//         (global_ant.resistance_dec == 50) &&
//         (global_ant.inductance_int == 19) &&
//         (global_ant.inductance_dec == 0) &&
//         (global_ant.current_limit_int == 3) &&
//         (global_ant.current_limit_dec == 50) &&
//         (global_ant.temp_max_int == 65) &&
//         (global_ant.temp_max_dec == 50))
//         PrintOK();
//     else
//         PrintERR();
    
// }

// 1 on called or others on answers
// 2 - 5
void CB_Usart (char * s)
{
    cb_usart_value = 1;

    if (strncmp(s, "temp,055.55,x\r\n", 12) == 0)
    {
        cb_usart_value = 2;        
    }

    if (strncmp(s, "new antenna chx\r\n", 14) == 0)
    {
        cb_usart_value = 3;
    }

    if (strncmp(s, "old antenna chx\r\n", 14) == 0)
    {
        cb_usart_value = 4;
    }
}



// Module Mocked Functions -----------------------------------------------------
void Treatment_SetFrequency_Str (char * str)
{
    treat_func = 1;
    printf("new frequency str: %s\n", str);
}


void Treatment_SetIntensity_Str (char * str)
{
    treat_func = 2;
    printf("new intensity str: %s\n", str);
}


void Treatment_SetPolarity_Str (char * str)
{
    treat_func = 3;    
    printf("new polarity str: %s\n", str);
}


void Treatment_SetMode_Str (char * str)
{
    treat_func = 4;
    printf("new mode str: %s\n", str);
}


void Treatment_SetThreshold_Str (char * str)
{
    treat_func = 5;
    printf("new threshold str: %s\n", str);
}


void Treatment_Start (void)
{
    treat_func = 6;
}


void Treatment_Stop (void)
{
    treat_func = 7;
}


void Treatment_SetGain_Str (char * str)
{
    treat_func = 8;
    printf("new setgain str: %s\n", str);
}


void Treatment_GetGain (void)
{
    treat_func = 9;
}


void Hard_GetVoltages (void)
{
    treat_func = 10;
}


void Hard_GetHardSoft (void)
{
    treat_func = 11;
}


void Led_On (void)
{
    printf("led to on!\n");
}


void Led_Off (void)
{
    printf("led to off!\n");
}


//--- end of file ---//


