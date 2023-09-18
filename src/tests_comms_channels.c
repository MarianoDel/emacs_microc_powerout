//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "comms_channels.h"
#include "antennas_defs.h"

// helper modules
#include "tests_ok.h"
#include "tests_mock_usart.h"

#include <stdio.h>
#include <string.h>


// Types Constants and Macros --------------------------------------------------


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
antenna_st global_ant;
int cb_usart_value = 0;
int antenna_answered = 0;



// Module Functions to Test ----------------------------------------------------
void Test_Comms_From_Channel1 (void);
void Test_Comms_From_Channel2 (void);
void Test_Comms_From_Channel3 (void);
void Test_Comms_From_Channel4 (void);


// Module Auxiliary Functions --------------------------------------------------
void CB_Usart (char * s);


// Tests Module Auxiliary or General Functions ---------------------------------


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{

    Test_Comms_From_Channel1 ();
    // Test_Comms_From_Channel2 ();
    // Test_Comms_From_Channel3 ();
    // Test_Comms_From_Channel4 ();

    return 0;
}


void Test_Comms_From_Channel1 (void)
{
    // set callback on usart1 RpiSend()
    Usart1Callback(CB_Usart);


    cb_usart_value = 0;
    Usart2FillRxBuffer("loco\r\n");    
    Comms_Channel1 ();

    printf("Test error string on channel 1: ");
    if (cb_usart_value == 0)    //no answer
        PrintOK();
    else
        PrintERR();


    // cb_usart_value = 0;    
    // Usart2FillRxBuffer("name:Tunel\r\n");
    // Comms_Channel1 ();
    
    // printf("Test antenna name channel 1: ");
    // if (cb_usart_value == 2)
    //     PrintOK();
    // else
    //     PrintERR();


    char my_ant_str [] = { "ant3,003.50,019.00,003.50,065.00\r\n" };
    cb_usart_value = 0;    
    Usart2FillRxBuffer(my_ant_str);
    Comms_Channel1 ();
    global_ant.resistance_int = 0;
    global_ant.resistance_dec = 0;
    global_ant.inductance_int = 0;
    global_ant.inductance_dec = 0;

    
    printf("Test old antenna & parse: ");
    if ((cb_usart_value == 4) &&
        (global_ant.resistance_int == 0) &&
        (global_ant.resistance_dec == 0) &&
        (global_ant.inductance_int == 0) &&
        (global_ant.inductance_dec == 0))
    {
        PrintOK();
    }
    else
        PrintERR();

    cb_usart_value = 0;    
    Usart2FillRxBuffer("temp,055.55\r\n");
    Comms_Channel1 ();
    
    printf("Test antenna current temp: ");
    if (cb_usart_value == 2)
        PrintOK();
    else
        PrintERR();

    cb_usart_value = 0;
    antenna_answered = 0;    
    Usart2FillRxBuffer("ok\r\n");
    Comms_Channel1 ();
    
    printf("Test antenna keepalive answer: ");
    if ((cb_usart_value == 0) && (antenna_answered))
        PrintOK();
    else
        PrintERR();

    char my_ant_str2 [] = { "ant3,0aa.50,019.00,003.50,065.50\r\n" };    
    cb_usart_value = 0;    
    Usart2FillRxBuffer(my_ant_str2);
    Comms_Channel1 ();
    
    global_ant.resistance_int = 0;
    global_ant.resistance_dec = 0;
    global_ant.inductance_int = 0;
    global_ant.inductance_dec = 0;
    
    printf("Test new antenna with errors on params: ");
    if ((cb_usart_value == 3) &&
        (global_ant.resistance_int == 0) &&
        (global_ant.resistance_dec == 0) &&
        (global_ant.inductance_int == 0) &&
        (global_ant.inductance_dec == 0))
        PrintOK();
    else
        PrintERR();

    cb_usart_value = 0;    
    Usart2FillRxBuffer("temp,0aa.55\r\n");
    Comms_Channel1 ();
    
    printf("Test antenna error on temp integer: ");
    if (cb_usart_value == 0)
        PrintOK();
    else
        PrintERR();

    cb_usart_value = 0;    
    Usart2FillRxBuffer("temp,055.aa\r\n");
    Comms_Channel1 ();
    
    printf("Test antenna error on temp decimal: ");
    if (cb_usart_value == 0)
        PrintOK();
    else
        PrintERR();

    global_ant.resistance_int = 0;
    global_ant.resistance_dec = 0;
    global_ant.inductance_int = 0;
    global_ant.inductance_dec = 0;
        
    strcpy (my_ant_str2,"ant3,003.50,019.00,003.50,065.50\r\n");
    cb_usart_value = 0;    
    Usart2FillRxBuffer(my_ant_str2);
    Comms_Channel1 ();
    
    printf("Test new antenna with good params: ");
    if ((cb_usart_value == 3) &&
        (global_ant.resistance_int == 3) &&
        (global_ant.resistance_dec == 50) &&
        (global_ant.inductance_int == 19) &&
        (global_ant.inductance_dec == 0) &&
        (global_ant.current_limit_int == 3) &&
        (global_ant.current_limit_dec == 50) &&
        (global_ant.temp_max_int == 65) &&
        (global_ant.temp_max_dec == 50))
        PrintOK();
    else
        PrintERR();
    
}

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


void Test_Comms_From_Channel2 (void)
{
    // set callback on usart3
    Usart3Callback(CB_Usart);

    cb_usart_value = 0;    
    Usart3FillRxBuffer("loco\r\n");    
    Comms_Channel2 ();

    printf("Test error string on channel 2: ");
    if (cb_usart_value == 1)
        PrintOK();
    else
        PrintERR();

    cb_usart_value = 0;    
    Usart3FillRxBuffer("name:Tunel\r\n");
    Comms_Channel2 ();
    
    printf("Test correct string on channel 2: ");
    if (cb_usart_value == 2)
        PrintOK();
    else
        PrintERR();
}


void Test_Comms_From_Channel3 (void)
{
    // set callback on uart4
    Uart4Callback(CB_Usart);

    cb_usart_value = 0;    
    Uart4FillRxBuffer("loco\r\n");    
    Comms_Channel3 ();

    printf("Test error string on channel 3: ");
    if (cb_usart_value == 1)
        PrintOK();
    else
        PrintERR();

    cb_usart_value = 0;    
    Uart4FillRxBuffer("name:Tunel\r\n");
    Comms_Channel3 ();
    
    printf("Test correct string on channel 3: ");
    if (cb_usart_value == 2)
        PrintOK();
    else
        PrintERR();
}


void Test_Comms_From_Channel4 (void)
{
    // set callback on uart5
    Uart5Callback(CB_Usart);

    cb_usart_value = 0;    
    Uart5FillRxBuffer("loco\r\n");    
    Comms_Channel4 ();

    printf("Test error string on channel 4: ");
    if (cb_usart_value == 1)
        PrintOK();
    else
        PrintERR();

    cb_usart_value = 0;    
    Uart5FillRxBuffer("name:Tunel\r\n");
    Comms_Channel4 ();
    
    printf("Test correct string on channel 4: ");
    if (cb_usart_value == 2)
        PrintOK();
    else
        PrintERR();
}

// Module Mocked Functions -----------------------------------------------------
void AntennaSetCurrentTemp (unsigned char ch, unsigned char t_int, unsigned char t_dec)
{
    printf("set current temp on ch%d %d.%dC\n", ch + 1, t_int, t_dec);
}

void AntennaIsAnswering (unsigned char ch)
{
    printf("antenna is answering on ch%d\n", ch + 1);
    antenna_answered = 1;
}

void AntennaSetName (unsigned char ch, char * pname)
{
    printf("antenna on ch%d set name to: %s\n", ch + 1, pname);
}



void AntennaSetParamsStruct (unsigned char ch, antenna_st *ant)
{
    printf("antenna on ch%d set params to:\n", ch + 1);
    printf("  R: %d.%d L: %d.%d I: %d.%d T: %d.%d\n",
           ant->resistance_int,
           ant->resistance_dec,
           ant->inductance_int,
           ant->inductance_dec,
           ant->current_limit_int,
           ant->current_limit_dec,
           ant->temp_max_int,
           ant->temp_max_dec);

    global_ant.resistance_int = ant->resistance_int; 
    global_ant.resistance_dec = ant->resistance_dec;
    global_ant.inductance_int = ant->inductance_int;
    global_ant.inductance_dec = ant->inductance_dec;
    global_ant.current_limit_int = ant->current_limit_int;
    global_ant.current_limit_dec = ant->current_limit_dec;
    global_ant.temp_max_int = ant->temp_max_int;
    global_ant.temp_max_dec = ant->temp_max_dec;

}
    
//--- end of file ---//


