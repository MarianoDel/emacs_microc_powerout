//---------------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### COMMS.C ###########################################
//---------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "comms.h"
#include "answers_defs.h"
#include "hard.h"
// #include "adc.h"

// #include "channels_defs.h"
#include "usart.h"
#include "treatment.h"
// #include "utils.h"
// #include "antennas.h"

// #include "parameters.h"
// #include "tamper_funcs.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


// Module Private Types Constants and Macros -----------------------------------
char s_ans_ok [] = {"ok\n"};
char s_ans_nok [] = {"nok\n"};
#define SIZEOF_LOCAL_BUFF    128
#define COMMS_TT_RELOAD    3000


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
char local_buff [SIZEOF_LOCAL_BUFF];
volatile unsigned short comms_timeout = 0;
    

// Module Private Functions ----------------------------------------------------
static void Comms_Messages (char * msg_str);


// Module Functions ------------------------------------------------------------
void Comms_Timeouts (void)
{
    if (comms_timeout)
        comms_timeout--;
}


unsigned char Comms_Rpi_Answering (void)
{
    if (comms_timeout)
        return 1;
    
    return 0;
}


void Comms_Update (void)
{
    if (Usart1HaveData())
    {
        Usart1HaveDataReset();
        Led_On();
        Usart1ReadBuffer(local_buff, SIZEOF_LOCAL_BUFF);
        Comms_Messages(local_buff);
        comms_timeout = COMMS_TT_RELOAD;
        Led_Off();
    }
}


static void Comms_Messages (char * msg_str)
{
    resp_e resp;
    
    char * msg = msg_str;
    
    // -- config messages for signals --
    if (!strncmp(msg, "sine frequency", sizeof("sine frequency") - 1))
    {
        resp = Treatment_SetFrequency_Str (MODE_SINE, msg + sizeof("sine frequency"));
        if (resp == resp_ok)
            Usart1Send (s_ans_ok);
        else
            Usart1Send (s_ans_nok);
    }

    else if (!strncmp(msg, "square frequency", sizeof("square frequency") - 1))
    {
        resp = Treatment_SetFrequency_Str (MODE_SQUARE, msg + sizeof("square frequency"));
        if (resp == resp_ok)
            Usart1Send (s_ans_ok);
        else
            Usart1Send (s_ans_nok);
    }
    
    else if (!strncmp(msg, "sine intensity", sizeof("sine intensity") - 1))
    {
        resp = Treatment_SetIntensity_Str (MODE_SINE, msg + sizeof("sine intensity"));
        if (resp == resp_ok)
            Usart1Send (s_ans_ok);
        else
            Usart1Send (s_ans_nok);
    }

    else if (!strncmp(msg, "square intensity", sizeof("square intensity") - 1))
    {
        resp = Treatment_SetIntensity_Str (MODE_SQUARE, msg + sizeof("square intensity"));
        if (resp == resp_ok)
            Usart1Send (s_ans_ok);
        else
            Usart1Send (s_ans_nok);
    }
    
    else if (!strncmp(msg, "polarity", sizeof("polarity") - 1))
    {
        resp = Treatment_SetPolarity_Str (msg + sizeof("polarity"));
        if (resp == resp_ok)
            Usart1Send (s_ans_ok);
        else
            Usart1Send (s_ans_nok);
    }

    // config messages for channel setup
    // else if (!strncmp(msg, "mode", sizeof("mode") - 1))
    // {
    //     resp = Treatment_SetMode_Str (msg + sizeof("mode"));
    //     if (resp == resp_ok)
    //         Usart1Send (s_ans_ok);
    //     else
    //         Usart1Send (s_ans_nok);
    // }

    else if (!strncmp(msg, "threshold", sizeof("threshold") - 1))
    {
        resp = Treatment_SetThreshold_Str (msg + sizeof("threshold"));
        if (resp == resp_ok)
            Usart1Send (s_ans_ok);
        else
            Usart1Send (s_ans_nok);
    }

    // -- operation messages --
    else if (!strncmp(msg, "square start", sizeof("square start") - 1))
    {
        Treatment_Start (MODE_SQUARE);
        Usart1Send (s_ans_ok);
    }

    else if (!strncmp(msg, "sine start", sizeof("sine start") - 1))
    {
        Treatment_Start (MODE_SINE);
        Usart1Send (s_ans_ok);
    }

    else if (!strncmp(msg, "stop", sizeof("stop") - 1))
    {
        Treatment_Stop ();
        Usart1Send (s_ans_ok);
    }

    // -- measures messages --
    if (!strncmp(msg, "set_gain", sizeof("set_gain") - 1))
    {
        resp = Treatment_SetGain_Str (msg + sizeof("set_gain"));
        if (resp == resp_ok)
            Usart1Send (s_ans_ok);
        else
            Usart1Send (s_ans_nok);
    }

    if (!strncmp(msg, "get_gain", sizeof("get_gain") - 1))
    {
        char buff [50];
        unsigned char gain;
        gain = Treatment_GetGain ();
        sprintf(buff, "gain: %d\n", gain);
        Usart1Send(buff);
    }

    // -- board messages --
    if (!strncmp(msg, "voltages", sizeof("voltages") - 1))
    {
        // char buff [50];
        // Hard_GetVoltages (buff);
        // Usart1Send(buff);

        Hard_GetVoltages_Complete ();
    }

    if (!strncmp(msg, "hard_soft", sizeof("hard_soft") - 1))
    {
        char buff [50];
        Hard_GetHardSoft (buff);
        Usart1Send(buff);
    }

}


//---- End of File ----//
