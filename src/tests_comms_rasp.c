//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "comms_from_rasp.h"
#include "treatment.h"
#include "comms.h"
#include "signals_defs.h"
#include "tests_mock_usart.h"
#include "tests_ok.h"



#include <stdio.h>
#include <string.h>
// #include <math.h>


// Types Constants and Macros --------------------------------------------------


// Externals -- Globals on Tested Module ---------------------------------------


// Globals -- Externals on Tested Module ---------------------------------------
volatile unsigned short adc_ch [7];
unsigned short comms_messages_rpi = 0;


// Globals ---------------------------------------------------------------------
char s_test [100] = { 0 };
int cb_usart_value = 0;
int cb_gpio = 0;



// Module Auxialiary Functions -------------------------------------------------
void CB_Usart (char * s);


// Module Mocked Functions -----------------------------------------------------
void HARD_L1_ON (void);
void HARD_L1_OFF (void);
void BuzzerCommands(unsigned char command, unsigned char multiple);


// Module Functions for Tests --------------------------------------------------
void Test_Comm_From_Rasp_Module (void);
void Test_Functions (void);


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    // Test_Functions ();
    Test_Comm_From_Rasp_Module();

    return 0;
}


void Test_Comm_From_Rasp_Module (void)
{
    int some_err = 0;

    // set callback on usart1
    Usart1Callback(CB_Usart);    

    //////////////////////
    // Test Signal Type //
    //////////////////////
    signal_type_e signal;

    if (!some_err)
    {
        strcpy(s_test, "signal triangular\r\n");
        Usart1FillRxBuffer(s_test);
        UpdateRaspberryMessages();

        signal = Treatment_GetSignalType();
        if (signal != TRIANGULAR_SIGNAL)
        {
            printf("\nSignal type error: %d ", signal);
            some_err = 1;
        }
    }

    if (!some_err)
    {    
        strcpy(s_test, "signal square\r\n");
        Usart1FillRxBuffer(s_test);
        UpdateRaspberryMessages();

        signal = Treatment_GetSignalType();
        if (signal != SQUARE_SIGNAL)
        {
            printf("\nSignal type error: %d ", signal);
            some_err = 1;
        }
    }

    if (!some_err)
    {    
        strcpy(s_test, "signal sinusoidal\r\n");
        Usart1FillRxBuffer(s_test);
        UpdateRaspberryMessages();

        signal = Treatment_GetSignalType();
        if (signal != SINUSOIDAL_SIGNAL)
        {
            printf("\nSignal type error: %d ", signal);
            some_err = 1;
        }
    }

    ////////////////
    // Test Power //
    ////////////////
    unsigned char power = 0;

    if (!some_err)
    {
        strcpy(s_test, "power 010\r\n");
        Usart1FillRxBuffer(s_test);    
        UpdateRaspberryMessages();

        power = Treatment_GetPower();
        if (power != 10)
        {
            printf("\nPower error: %d ", power);
            some_err = 1;
        }
    }

    ////////////////////
    // Test Frequency //
    ////////////////////
    unsigned char f_int = 0;
    unsigned char f_dec = 0;

    if (!some_err)
    {
        f_int = 10;
        f_dec = 20;
        sprintf(s_test, "frequency %02d.%02d\r\n", f_int, f_dec);
        Usart1FillRxBuffer(s_test);
        UpdateRaspberryMessages();

        Treatment_GetFrequency(&f_int, &f_dec);
        if ((f_int != 10) || (f_dec != 20))
        {
            printf("\nFrequency error: %d.%d ", f_int, f_dec);
            some_err = 1;
        }
    }

    if (!some_err)
    {    
        f_int = 86;
        f_dec = 22;
        sprintf(s_test, "frequency %03d.%02d\r\n", f_int, f_dec);
        Usart1FillRxBuffer(s_test);
        UpdateRaspberryMessages();

        Treatment_GetFrequency(&f_int, &f_dec);
        if ((f_int != 86) || (f_dec != 22))
        {
            printf("\nFrequency error: %d.%d ", f_int, f_dec);
            some_err = 1;
        }
    }

    if (!some_err)
    {    
        f_int = 200;
        f_dec = 0;
        sprintf(s_test, "frequency %03d.%02d\r\n", f_int, f_dec);
        Usart1FillRxBuffer(s_test);
        UpdateRaspberryMessages();

        Treatment_GetFrequency(&f_int, &f_dec);
        if ((f_int != 200) || (f_dec != 0))
        {
            printf("\nFrequency error: %d.%d ", f_int, f_dec);
            some_err = 1;
        }
    }

    if (!some_err)
    {    
        f_int = 199;
        f_dec = 99;
        sprintf(s_test, "frequency %03d.%02d\r\n", f_int, f_dec);
        Usart1FillRxBuffer(s_test);
        UpdateRaspberryMessages();

        Treatment_GetFrequency(&f_int, &f_dec);
        if ((f_int != 199) || (f_dec != 99))
        {
            printf("\nFrequency error: %d.%d ", f_int, f_dec);
            some_err = 1;
        }
    }
    
    //////////////////////
    // Test Bridge Mode //
    //////////////////////
    if (!some_err)
    {    
        strcpy(s_test, "goto bridge mode\r\n");
        comms_messages_rpi = 0;
        Usart1FillRxBuffer(s_test);
        UpdateRaspberryMessages();

        if (comms_messages_rpi != COMM_GOTO_BRIDGE)
        {
            printf("\nUpDwn error getted: %d ", comms_messages_rpi);
            some_err = 1;
        }
    }

    //////////////////
    // Test Voltage //
    //////////////////
    if (!some_err)
    {    
        adc_ch[5] = 3000;
        adc_ch[6] = 2500;

        strcpy(s_test, "voltage\r\n");
        Usart1FillRxBuffer(s_test);
        cb_usart_value = 0;
        UpdateRaspberryMessages();

        if (cb_usart_value != 1)
        {
            printf("\nVoltage with some error ");
            some_err = 1;        
        }
    }
    
    //////////////////////
    // Test Hard & Soft //
    //////////////////////
    if (!some_err)
    {    
        strcpy(s_test, "hard_soft\r\n");
        Usart1FillRxBuffer(s_test);
        cb_usart_value = 0;    
        UpdateRaspberryMessages();
    
        if (cb_usart_value != 2)
        {
            printf("\nHard or Soft with some error ");
            some_err = 1;
        }
    }

    /////////////////
    // Test Buzzer //
    /////////////////
    if (!some_err)
    {    
        strcpy(s_test, "buzzer short 3\r\n");
        Usart1FillRxBuffer(s_test);
        cb_gpio = 0;
        UpdateRaspberryMessages();
    
        if (cb_gpio != 1)
        {
            printf("\nBuzzer with errors ");
            some_err = 1;
        }
    }

    if (!some_err)
    {    
        strcpy(s_test, "buzzer half 2\r\n");
        Usart1FillRxBuffer(s_test);
        cb_gpio = 0;
        UpdateRaspberryMessages();
    
        if (cb_gpio != 2)
        {
            printf("\nBuzzer with errors ");
            some_err = 1;
        }
    }

    if (!some_err)
    {    
        strcpy(s_test, "buzzer long 1\r\n");
        Usart1FillRxBuffer(s_test);
        cb_gpio = 0;
        UpdateRaspberryMessages();
    
        if (cb_gpio != 3)
        {
            printf("\nBuzzer with errors ");
            some_err = 1;
        }
    }

    ////////////////////
    // Test keepalive //
    ////////////////////
    if (!some_err)
    {    
        strcpy(s_test, "keepalive,\r\n");
        Usart1FillRxBuffer(s_test);
        cb_usart_value = 0;
        UpdateRaspberryMessages();
    
        if (cb_usart_value != 3)
        {
            printf("\nNo keepalive ");
            some_err = 1;
        }
    }

    ///////////////////
    // Test Duration //
    ///////////////////
    if (!some_err)
    {    
        strcpy(s_test, "duration,100\r\n");
        Usart1FillRxBuffer(s_test);
        UpdateRaspberryMessages();

        unsigned short time = Treatment_GetTime ();
        if (time != (100*60))
        {
            printf("\nTime error getted: %d ", time);
            some_err = 1;
        }
    }


    ///////////////////////////
    // Test Stop Pause Start //
    ///////////////////////////
    if (!some_err)
    {    
        strcpy(s_test, "stop,\r\n");
        comms_messages_rpi = 0;
        Usart1FillRxBuffer(s_test);
        UpdateRaspberryMessages();

        if (comms_messages_rpi != COMM_STOP_TREAT)
        {
            printf("\nStop error getted: %d ", comms_messages_rpi);
            some_err = 1;
        }
    }

    if (!some_err)
    {    
        strcpy(s_test, "start,\r\n");
        comms_messages_rpi = 0;
        Usart1FillRxBuffer(s_test);
        UpdateRaspberryMessages();

        if (comms_messages_rpi != COMM_START_TREAT)
        {
            printf("\nStart error getted: %d ", comms_messages_rpi);
            some_err = 1;
        }
    }

    if (!some_err)
    {    
        strcpy(s_test, "pause,\r\n");
        comms_messages_rpi = 0;
        Usart1FillRxBuffer(s_test);    
        UpdateRaspberryMessages();

        if (comms_messages_rpi != COMM_PAUSE_TREAT)
        {
            printf("\nPause error getted: %d ", comms_messages_rpi);
            some_err = 1;
        }
    }

    printf("\n All Tests done results: ");
    if (some_err)
        PrintERR();
    else
        PrintOK();
    
}


// Module Auxiliary Functions --------------------------------------------------
void CB_Usart (char * s)
{
    if (strncmp(s, "High Supply:", sizeof("High Supply:") - 1) == 0)
        cb_usart_value = 1;

    if (strncmp(s, "Hardware Version:", sizeof("Hardware Version:") - 1) == 0)
        cb_usart_value = 2;

    if (strncmp(s, "OK", sizeof("OK") - 1) == 0)
        cb_usart_value = 3;
    
    // if (strncmp(s, "ok", sizeof("ok") - 1) == 0)
    //     cb_usart_value = 4;

    // if (strncmp(s, "Hrd 2.0 Soft 1.1", sizeof("Hrd 2.0 Soft 1.1") - 1) == 0)
    //     cb_usart_value = 5;
}


// Module Mocked Functions -----------------------------------------------------
void HARD_L1_ON (void)
{
    printf("Led1 -> ON\n");
}

void HARD_L1_OFF (void)
{
    printf("Led1 -> OFF\n");
}

void BuzzerCommands(unsigned char command, unsigned char multiple)
{
    printf("buzzer cmd: %d multiple: %d\n", command, multiple);

    if ((command == 8) && (multiple == 3))
        cb_gpio = 1;

    if ((command == 5) && (multiple == 2))
        cb_gpio = 2;

    if ((command == 2) && (multiple == 1))
        cb_gpio = 3;
}


void ChangeLed (unsigned char how_many)
{
    printf("toggle led %d times\n", how_many);
}


void Wait_ms (unsigned short millis)
{
    printf("wait %d ms\n", millis);
}

// Mocked Signals Module
void Signals_Setup_Treatment_Data (void)
{
}

void Signals_Set_Reset_Channel_For_Treatment (void)
{
}

void Signals_Set_Channel_Table_Open_Loop (void)
{
}

void Signals_Setup_All_Channels_Open_Loop (void)
{
}

void Signals_Generate_All_Channels_Open_Loop (void)
{
}

void Signals_Stop_All_Channels (void)
{
}

void Signals_Stop_Single_Channel (void)
{
}
// Mocked Antenna Module
void AntennaSendKnowInfoWithTimer (void)
{
}

void AntennaVerifyForTreatment (void)
{
}

void AntennaGetParamsStruct (void)
{
}

void AntennaEndTreatment (void)
{
}

void AntennaGetConnection (void)
{
}

void AntennaGetTempStatus (void)
{
}
// Mocked Error Module
void Error_SetStatus_For_Checks (unsigned char error, unsigned char ch)
{
}

void Error_SetStatus (unsigned char error, unsigned char ch)
{
}

void Error_GetStatus (void)
{
}

void Error_SetString (void)
{
}
// Mocked General
void EXTIOn (void)
{
}

void EXTIOff (void)
{
}

void UpdateLed (void)
{
}

void UpdateBuzzer (void)
{
}
//--- end of file ---//


