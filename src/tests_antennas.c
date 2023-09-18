//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "antennas.h"
#include "channels_defs.h"
#include "answers_defs.h"
// #include "tim.h"
// #include "dsp.h"

// helper modules
#include "tests_ok.h"
#include "tests_mock_usart.h"
#include "tests_know_antennas.h"


#include <stdio.h>
#include <string.h>


// Types Constants and Macros --------------------------------------------------
// from antennas.c module
typedef enum {
    INIT_SEARCH,
    NO_CONN,
    GET_NAME,
    IN_STANDBY
    
} standby_state_e;


// Externals -------------------------------------------------------------------
extern antenna_state_e antenna_state;
extern unsigned char standby_ch1;
extern unsigned char standby_ch2;
extern unsigned char standby_ch3;
extern unsigned char standby_ch4;


// Globals ---------------------------------------------------------------------
int answer_params = 0;
int answer_keepalive = 0;
int answer_name = 0;


// Module Functions to Test ----------------------------------------------------
void Test_Antennas_Standby (void);
void Test_Antenna_All_Ch_Connection (void);
void Test_Antenna_All_Ch_No_Connection (void);
void Test_Antenna_All_Ch_Name (void);
void Test_Antenna_Comms_Callbacks (void);
void Test_Temp_Comms_Callbacks (void);
void Test_Antenna_Veriry_For_Treatment (void);
void Test_Antenna_All_Ch_In_Treatment (void);


// Module Auxiliary Functions --------------------------------------------------
void Usart2CB (char * msg);
void Usart3CB (char * msg);
void Uart4CB (char * msg);
void Uart5CB (char * msg);

// Tests Module Auxiliary or General Functions ---------------------------------


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    Test_Antennas_Standby ();
    Test_Antenna_All_Ch_No_Connection ();
    Test_Antenna_All_Ch_Connection ();
    Test_Antenna_All_Ch_Name ();
    Test_Antenna_Comms_Callbacks ();
    Test_Temp_Comms_Callbacks ();
    Test_Antenna_Veriry_For_Treatment ();
    Test_Antenna_All_Ch_In_Treatment ();
    

    return 0;
}


void Test_Antennas_Standby (void)
{
    for (int i = 0; i < 5000; i++)
    {
        AntennaUpdateStates ();
        AntennaTimeouts ();
    }

    printf("test antenna stand by: ");
    if (antenna_state == ANTENNA_IN_STANDBY)
        PrintOK();
    else
        PrintERR();    
    
}


void Test_Antenna_All_Ch_No_Connection (void)
{
    // activate usarts callbacks
    Usart2Callback(Usart2CB);
    Usart3Callback(Usart3CB);
    Uart4Callback(Uart4CB);
    Uart5Callback(Uart5CB);
    
    for (int i = 0; i < 20000; i++)
    {
        AntennaUpdateStates ();
        AntennaTimeouts ();
    }

    printf("test antenna all channels no connection: ");
    if ((antenna_state == ANTENNA_IN_STANDBY) &&
        (standby_ch1 == NO_CONN) &&
        (standby_ch2 == NO_CONN) &&
        (standby_ch3 == NO_CONN) &&
        (standby_ch4 == NO_CONN))
        PrintOK();
    else
    {
        PrintERR();
        printf("antenna_state: %d ch1: %d ch2: %d ch3: %d ch4: %d\n",
               antenna_state,
               standby_ch1,
               standby_ch2,
               standby_ch3,
               standby_ch4);
    }
}


void Test_Antenna_All_Ch_Connection (void)
{
    answer_params = 1;
    answer_keepalive = 1;

    // activate usarts callbacks
    Usart2Callback(Usart2CB);
    Usart3Callback(Usart3CB);
    Uart4Callback(Uart4CB);
    Uart5Callback(Uart5CB);

    for (int i = 0; i < 20000; i++)
    // for (int i = 0; i < 2000; i++)        
    {
        AntennaUpdateStates ();
        AntennaTimeouts ();
    }

    printf("test antenna all channels conn: ");
    if ((antenna_state == ANTENNA_IN_STANDBY) &&
        (standby_ch1 == IN_STANDBY) &&
        (standby_ch2 == IN_STANDBY) &&
        (standby_ch3 == IN_STANDBY) &&
        (standby_ch4 == IN_STANDBY))
        PrintOK();
    else
    {
        PrintERR();
        printf("antenna_state: %d ch1: %d ch2: %d ch3: %d ch4: %d\n",
               antenna_state,
               standby_ch1,
               standby_ch2,
               standby_ch3,
               standby_ch4);
    }
    
}


void Test_Antenna_All_Ch_Name (void)
{
    answer_keepalive = 0;
    for (int i = 0; i < 20000; i++)
    {
        AntennaUpdateStates ();
        AntennaTimeouts ();
    }

    printf("test antenna all channels name & no conn: ");
    if ((antenna_state == ANTENNA_IN_STANDBY) &&
        (standby_ch1 == NO_CONN) &&
        (standby_ch2 == NO_CONN) &&
        (standby_ch3 == NO_CONN) &&
        (standby_ch4 == NO_CONN))
        PrintOK();
    else
    {
        PrintERR();
        printf("antenna_state: %d ch1: %d ch2: %d ch3: %d ch4: %d\n",
               antenna_state,
               standby_ch1,
               standby_ch2,
               standby_ch3,
               standby_ch4);
    }

    answer_name = 1;
    answer_keepalive = 1;
    for (int i = 0; i < 5000; i++)
    {
        AntennaUpdateStates ();
        AntennaTimeouts ();
    }

    printf("test antenna all channels name & conn: ");
    if ((antenna_state == ANTENNA_IN_STANDBY) &&
        (standby_ch1 == IN_STANDBY) &&
        (standby_ch2 == IN_STANDBY) &&
        (standby_ch3 == IN_STANDBY) &&
        (standby_ch4 == IN_STANDBY))
        PrintOK();
    else
    {
        PrintERR();
        printf("antenna_state: %d ch1: %d ch2: %d ch3: %d ch4: %d\n",
               antenna_state,
               standby_ch1,
               standby_ch2,
               standby_ch3,
               standby_ch4);
    }
    
}


void Test_Antenna_All_Ch_In_Treatment (void)
{
    int error = 0;

    printf("\ntest antenna force drop down connection\n");
    for (int i = CH1; i <= CH4; i++)
        AntennaEndTreatment (i);

    answer_params = 0;
    answer_keepalive = 0;
    answer_name = 0;
    for (int i = 0; i < 20000; i++)
    {
        AntennaUpdateStates ();
        AntennaTimeouts ();
    }

    printf("  antenna all channels no conn: ");
    if ((antenna_state == ANTENNA_IN_STANDBY) &&
        (standby_ch1 == NO_CONN) &&
        (standby_ch2 == NO_CONN) &&
        (standby_ch3 == NO_CONN) &&
        (standby_ch4 == NO_CONN))
        PrintOK();
    else
    {
        PrintERR();
        printf("antenna_state: %d ch1: %d ch2: %d ch3: %d ch4: %d\n",
               antenna_state,
               standby_ch1,
               standby_ch2,
               standby_ch3,
               standby_ch4);
    }

    answer_params = 1;
    answer_keepalive = 1;
    for (int i = 0; i < 20000; i++)
    {
        AntennaUpdateStates ();
        AntennaTimeouts ();
    }
    
    printf("  antenna all channels conn: ");
    if ((antenna_state == ANTENNA_IN_STANDBY) &&
        (standby_ch1 == IN_STANDBY) &&
        (standby_ch2 == IN_STANDBY) &&
        (standby_ch3 == IN_STANDBY) &&
        (standby_ch4 == IN_STANDBY))
        PrintOK();
    else
    {
        PrintERR();
        printf("antenna_state: %d ch1: %d ch2: %d ch3: %d ch4: %d\n",
               antenna_state,
               standby_ch1,
               standby_ch2,
               standby_ch3,
               standby_ch4);
    }

    printf("  antenna verify for treatment: ");
    for (int i = CH1; i <= CH4; i++)
    {
        if (AntennaVerifyForTreatment (i) == resp_error)
            error = i;
    }

    // //AntennaCheckNameCh1 to Ch4
    // if (!(ch1_ant_conn_status & 0x20))
    //     error = 1;

    // if (!(ch2_ant_conn_status & 0x20))
    //     error = 2;

    // if (!(ch3_ant_conn_status & 0x20))
    //     error = 3;

    // if (!(ch4_ant_conn_status & 0x20))
    //     error = 4;
    
    if (!error)
        PrintOK();
    else
    {
        PrintERR();
        printf("  error on ch%d\n", error);
    }
    
    for (int i = 0; i < 1000; i++)
    {
        AntennaUpdateStates ();
        AntennaTimeouts ();
    }

    printf("  antenna in treatment: ");
    if ((antenna_state == ANTENNA_IN_TREATMENT) &&
        (standby_ch1 == IN_STANDBY) &&
        (standby_ch2 == IN_STANDBY) &&
        (standby_ch3 == IN_STANDBY) &&
        (standby_ch4 == IN_STANDBY))
        PrintOK();
    else
    {
        PrintERR();
        printf("antenna_state: %d ch1: %d ch2: %d ch3: %d ch4: %d\n",
               antenna_state,
               standby_ch1,
               standby_ch2,
               standby_ch3,
               standby_ch4);
    }
    
}


extern unsigned char ch1_ant_conn_status;
extern unsigned char ch2_ant_conn_status;
extern unsigned char ch3_ant_conn_status;
extern unsigned char ch4_ant_conn_status;
void Test_Antenna_Comms_Callbacks (void)
{
    int error = 0;
    antenna_st my_ant = { .resistance_int = 10,
                          .resistance_dec = 10,
                          .inductance_int = 10,
                          .inductance_dec = 10,
                          .current_limit_int = 3,
                          .current_limit_dec = 50,
                          .temp_max_int = 55,
                          .temp_max_dec = 0 };
    
    printf("test antenna set and get params: ");
    for (int i = CH1; i <= CH4; i++)
    {
        AntennaSetParamsStruct (i, &my_ant);
    }

    antenna_st saved_ant;
    for (int i = CH1; i <= CH4; i++)
    {
        AntennaGetParamsStruct (i, &saved_ant);
        if ((saved_ant.resistance_int != my_ant.resistance_int) ||
            (saved_ant.resistance_dec != my_ant.resistance_dec) ||
            (saved_ant.inductance_int != my_ant.inductance_int) ||
            (saved_ant.inductance_dec != my_ant.inductance_dec) ||
            (saved_ant.current_limit_int != my_ant.current_limit_int) ||
            (saved_ant.current_limit_dec != my_ant.current_limit_dec) ||
            (saved_ant.temp_max_int != my_ant.temp_max_int) ||
            (saved_ant.temp_max_dec != my_ant.temp_max_dec))
        {
            error = i;
        }
    }

    if (!error)
        PrintOK();
    else
    {
        PrintERR();
        printf("  error on ch%d\n", error);
    }


    error = 0;
    printf("test antenna set name with return: ");
    char my_name [] = { "Tunnel\r" };
    for (int i = CH1; i <= CH4; i++)
    {
        AntennaSetName (i, my_name);
    }

    //AntennaCheckNameCh1 to Ch4
    if (!(ch1_ant_conn_status & 0x20))
        error = 1;

    if (!(ch2_ant_conn_status & 0x20))
        error = 2;

    if (!(ch3_ant_conn_status & 0x20))
        error = 3;

    if (!(ch4_ant_conn_status & 0x20))
        error = 4;
    
    if (!error)
        PrintOK();
    else
    {
        PrintERR();
        printf("  error on ch%d\n", error);
    }
    
    
}


void Test_Temp_Comms_Callbacks (void)
{
    int error = 0;

    printf("test antenna set and get temp: ");
    unsigned char ti = 50;
    unsigned char td = 55;
    unsigned char tcurrent = 0;
    
    for (int i = CH1; i <= CH4; i++)
    {
        AntennaSetCurrentTemp (i, ti, td);
    }

    for (int i = CH1; i <= CH4; i++)
    {
        tcurrent = AntennaGetCurrentTemp (i);
        if (tcurrent != ti)
            error = i;

    }

    if (!error)
        PrintOK();
    else
    {
        PrintERR();
        printf("  error on ch%d\n", error);
    }
}


void Test_Antenna_Veriry_For_Treatment (void)
{
    int error = 0;
    resp_e resp = resp_error;

    printf("test antenna verify for treatment: ");

    for (int i = CH1; i <= CH4; i++)
    {
        resp = AntennaVerifyForTreatment (i);
        if (resp != resp_ok)
            error = i;
    }

    if (!error)
        PrintOK();
    else
    {
        PrintERR();
        printf("  error on ch%d\n", error);
    }
}


// Module Mocked Functions -----------------------------------------------------
void RPI_Send (char * a)
{
    Usart1Send(a);
}


void Usart2CB (char * msg)
{
    if ((answer_params) &&
        (!strncmp(msg, "get_params", sizeof("get_params") - 1)))
    {
        // const char s_antena [] = { "ant0,012.27,087.90,001.80,065.00\r\n" };
        // Usart2FillRxBuffer((char *) s_antena);

        // mock process string
        antenna_st my_ant;
        TSP_Get_Know_Single_Antenna(&my_ant, 0);
        
        AntennaSetParamsStruct (CH1, &my_ant);
    }

    if ((answer_keepalive) &&
        (!strncmp(msg, "keepalive", sizeof("keepalive") - 1)))
    {
        AntennaIsAnswering(CH1);        
    }

    if ((answer_name) &&
        (!strncmp(msg, "get_name", sizeof("get_name") - 1)))
    {
        AntennaSetName(CH1, "tunnel");
    }
    
}


void Usart3CB (char * msg)
{
    // Usart3Send(a);

    if ((answer_params) &&
        (!strncmp(msg, "get_params", sizeof("get_params") - 1)))
    {
        // const char s_antena [] = { "ant1,023.85,141.60,001.30,065.00\r\n" };
        // Usart3FillRxBuffer(s_antena);

        // mock process string
        antenna_st my_ant;
        TSP_Get_Know_Single_Antenna(&my_ant, 1);
        
        AntennaSetParamsStruct (CH2, &my_ant);
    }
    
    if ((answer_keepalive) &&
        (!strncmp(msg, "keepalive", sizeof("keepalive") - 1)))
    {
        AntennaIsAnswering(CH2);
    }

    if ((answer_name) &&
        (!strncmp(msg, "get_name", sizeof("get_name") - 1)))
    {
        AntennaSetName(CH2, "tunnel");
    }
    
}


void Uart4CB (char * msg)
{
    // Uart4Send(a);

    if ((answer_params) &&
        (!strncmp(msg, "get_params", sizeof("get_params") - 1)))
    {
        // const char s_antena [] = { "ant1,017.00,120.00,001.30,065.00\r\n" };
        // Uart4FillRxBuffer(s_antena);

        // mock process string
        antenna_st my_ant;
        TSP_Get_Know_Single_Antenna(&my_ant, 2);
        
        AntennaSetParamsStruct (CH3, &my_ant);
    }

    if ((answer_keepalive) &&
        (!strncmp(msg, "keepalive", sizeof("keepalive") - 1)))
    {
        AntennaIsAnswering(CH3);
    }

    if ((answer_name) &&
        (!strncmp(msg, "get_name", sizeof("get_name") - 1)))
    {
        AntennaSetName(CH3, "tunnel");
    }
    
}


void Uart5CB (char * msg)
{
    // Uart5Send(a);

    if ((answer_params) &&
        (!strncmp(msg, "get_params", sizeof("get_params") - 1)))
    {
        // const char s_antena [] = { "ant2,005.70,011.10,002.80,065.00\r\n" };
        // Uart5FillRxBuffer(s_antena);

        // mock process string
        antenna_st my_ant;
        TSP_Get_Know_Single_Antenna(&my_ant, 3);
        
        AntennaSetParamsStruct (CH4, &my_ant);
    }

    if ((answer_keepalive) &&
        (!strncmp(msg, "keepalive", sizeof("keepalive") - 1)))
    {
        AntennaIsAnswering(CH4);
    }

    if ((answer_name) &&
        (!strncmp(msg, "get_name", sizeof("get_name") - 1)))
    {
        AntennaSetName(CH4, "tunnel");
    }
    
}

// void UpdateLed (void)
// {
// }

// void UpdateBuzzer (void)
// {
// }

// void UpdateRaspberryMessages (void)
// {
// }

//--- end of file ---//


