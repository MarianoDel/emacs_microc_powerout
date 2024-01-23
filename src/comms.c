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
    
    // check if its own
    if ((strncmp (msg_str, "ch1", sizeof("ch1") - 1) != 0) &&
        (strncmp (msg_str, "chf", sizeof("chf") - 1) != 0))
        return;

    char * msg = (msg_str + 4);
    
    // -- config messages for signals --
    if (!strncmp(msg, "frequency", sizeof("frequency") - 1))
    {
        resp = Treatment_SetFrequency_Str (msg + sizeof("frequency"));
        if (resp == resp_ok)
            Usart1Send (s_ans_ok);
        else
            Usart1Send (s_ans_nok);
    }

    else if (!strncmp(msg, "intensity", sizeof("intensity") - 1))
    {
        resp = Treatment_SetIntensity_Str (msg + sizeof("intensity"));
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
    else if (!strncmp(msg, "mode", sizeof("mode") - 1))
    {
        resp = Treatment_SetMode_Str (msg + sizeof("mode"));
        if (resp == resp_ok)
            Usart1Send (s_ans_ok);
        else
            Usart1Send (s_ans_nok);
    }

    else if (!strncmp(msg, "threshold", sizeof("threshold") - 1))
    {
        resp = Treatment_SetThreshold_Str (msg + sizeof("threshold"));
        if (resp == resp_ok)
            Usart1Send (s_ans_ok);
        else
            Usart1Send (s_ans_nok);
    }

    // -- operation messages --
    else if (!strncmp(msg, "start", sizeof("start") - 1))
    {
        Treatment_Start ();
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
    
    
    
    
    
    //mensajes nuevos
//     if (!strncmp(msg, (const char *)"signal triangular", (sizeof("signal triangular") - 1)))
//     {
//         Treatment_SetSignalType(TRIANGULAR_SIGNAL);
//         RpiSend(s_ans_ok);
//     }

//     else if (!strncmp(msg, (const char *)"signal square", (sizeof("signal square") - 1)))
//     {
//         Treatment_SetSignalType(SQUARE_SIGNAL);
//         RpiSend(s_ans_ok);
//     }

//     else if (!strncmp(msg, (const char *)"signal sinusoidal", (sizeof("signal sinusoidal") - 1)))
//     {
//         Treatment_SetSignalType(SINUSOIDAL_SIGNAL);
//         RpiSend(s_ans_ok);
//     }

//     else if (!strncmp(msg, (const char *)"power", (sizeof("power") - 1)))
//     {
//         char num [3];
//         unsigned short power;

//         resp = resp_error;

//         if (strlen(msg) >= 9)
//         {
//             strncpy(num, (msg + 6), 3);
//             power = atoi(num);

//             if (power <= 100)
//             {
//                 if (Treatment_SetPower ((unsigned char) power) == resp_ok)
//                 {
//                     resp = resp_ok;
//                     comms_messages_rpi |= COMM_CONF_CHANGE;
//                 }
//             }
//         }
            
//         if (resp == resp_ok)
//             RpiSend(s_ans_ok);
//         else
//             RpiSend(s_ans_nok);
//     }

//     //-- Frequency Setting
//     else if (!strncmp(msg, s_frequency, sizeof(s_frequency) - 1))
//     {
//         // char to_send [64];
        
//         msg += sizeof(s_frequency);    //normalizo al payload, hay un espacio

//         // what we get is E.DD | EE.DD | EEE.DD, always two decimal positions
//         figures = StringIsANumber(msg, &new_freq_int);

//         if ((figures) && (figures <= 3) && (new_freq_int < 210))
//         {
//             msg += figures + 1;    //normalizo con el punto
//             figures = StringIsANumber(msg, &new_freq_dec);

//             if ((figures > 1) && (figures < 3))
//             {
//                 resp = Treatment_SetFrequency ((unsigned char) new_freq_int, (unsigned char) new_freq_dec);
//                 if (resp == resp_ok)
//                     comms_messages_rpi |= COMM_CONF_CHANGE;                
//             }
//         }

//         if (resp == resp_ok)
//             RpiSend(s_ans_ok);
//         else
//             RpiSend(s_ans_nok);
//     }
    
//     else if (!strncmp(msg, "goto bridge mode", sizeof("goto bridge mode") - 1))
//     {
//         comms_messages_rpi |= COMM_GOTO_BRIDGE;
//         RpiSend((char *)"Going to Bridge Mode...\r\n");
//     }

//     else if (!strncmp(msg, "voltage", sizeof("voltage") - 1))
//     {
//         char to_send [64];
//         float fcalc = 1.0;
//         short volt_int, volt_dec;

//         fcalc = Sense_200V;
//         fcalc = fcalc * K_200V;
//         volt_int = (short) fcalc;
//         fcalc = fcalc - volt_int;
//         fcalc = fcalc * 10;
//         volt_dec = (short) fcalc;
//         sprintf(to_send, "High Supply: %3d.%01dV\r\n", volt_int, volt_dec);
//         RpiSend(to_send);

//         fcalc = Sense_15V;
//         fcalc = fcalc * K_15V;
//         volt_int = (short) fcalc;
//         fcalc = fcalc - volt_int;
//         fcalc = fcalc * 10;
//         volt_dec = (short) fcalc;        
//         sprintf(to_send, "Low Supply: %3d.%01dV\r\n", volt_int, volt_dec);
//         RpiSend(to_send);
        
//     }
    
//     else if (!strncmp(msg, "hard_soft", sizeof("hard_soft") - 1))
//     {
//         char to_send [80];
//         sprintf(to_send, "%s\r\n%s\r\n", HARD, SOFT);
//         RpiSend(to_send);
//     }


//     //-- Buzzer Actions
//     else if (strncmp(msg, s_buzzer_short, sizeof(s_buzzer_short) - 1) == 0)
//     {
//         unsigned short bips_qtty = 0;
        
//         msg += sizeof(s_buzzer_short);		//normalizo al payload, hay un espacio

//         //lo que viene es un byte de 1 a 9
//         figures = StringIsANumber(msg, &bips_qtty);
//         if (figures == 1)
//         {
//             BuzzerCommands(BUZZER_SHORT_CMD, (unsigned char) bips_qtty);
//             RpiSend(s_ans_ok);
//         }
//         else
//             resp = resp_error;
//     }

//     else if (strncmp(msg, s_buzzer_half, sizeof(s_buzzer_half) - 1) == 0)
//     {
//         unsigned short bips_qtty = 0;
        
//         msg += sizeof(s_buzzer_half);		//normalizo al payload, hay un espacio

//         //lo que viene es un byte de 1 a 9
//         figures = StringIsANumber(msg, &bips_qtty);
//         if (figures == 1)
//         {
//             BuzzerCommands(BUZZER_HALF_CMD, (unsigned char) bips_qtty);
//             RpiSend(s_ans_ok);
//         }
//         else
//             RpiSend(s_ans_nok);

//     }

//     else if (strncmp(msg, s_buzzer_long, sizeof(s_buzzer_long) - 1) == 0)
//     {
//         unsigned short bips_qtty = 0;
        
//         msg += sizeof(s_buzzer_long);		//normalizo al payload, hay un espacio

//         //lo que viene es un byte de 1 a 9
//         figures = StringIsANumber(msg, &bips_qtty);
//         if (figures == 1)
//             BuzzerCommands(BUZZER_LONG_CMD, (unsigned char) bips_qtty);
//         else
//             resp = resp_error;
//     }

//     // old is keepalive, new is keepalive alone
//     else if (!strncmp(msg, (const char *)"keepalive", (sizeof("keepalive") - 1)))
//     {
//         RpiSend(s_ans_ok);
//     }

//     else if (!strncmp(msg, (const char *)"duration,", (sizeof("duration,") - 1)))
//     {
//         unsigned short new_time = 0;
        
//         msg += sizeof("duration,") - 1;		//normalizo al payload

//         // check old or new conf string
//         //example.	duration,00,10,00,1
//         if ((*(msg + 2) == ',') &&
//             (*(msg + 5) == ',') &&
//             (*(msg + 8) == ','))
//         {
//             // old conf type
//             unsigned char hours = (*(msg + 0) - '0') * 10 + *(msg + 1) - '0';
//             unsigned char minutes = (*(msg + 3) - '0') * 10 + *(msg + 4) - '0';

//             figures = 3;
//             new_time = hours * 60 + minutes;
//         }
//         else
//         {
//             // new conf type
//             //lo que viene son tres bytes con el tiempo de 1 a 120 se supone
//             figures = StringIsANumber(msg, &new_time);
//         }
        
//         if (figures == 3)
//         {
//             if (Treatment_SetTimeinMinutes(new_time) == resp_ok)
//             {
//                 RpiSend(s_ans_ok);
//                 comms_messages_rpi |= COMM_CONF_CHANGE;
//             }
//             else
//                 RpiSend(s_ans_nok);
//         }
//         else
//             RpiSend(s_ans_nok);
//     }

//     else if (!strncmp((const char *)msg, (const char *)"serial num", (sizeof("serial num") - 1)))
//     {
//         char to_send[40] = { 0 };
        
// #ifdef USE_DEVICE_ID_4BYTES
//         unsigned int device_id = *((unsigned short*)0x1FFFF7E8);
//         device_id <<= 16;
//         device_id |= *((unsigned short*)(0x1FFFF7E8 + 2));
//         device_id ^= *((unsigned int*)(0x1FFFF7E8 + 4));
//         device_id ^= *((unsigned int*)(0x1FFFF7E8 + 8));
//         sprintf(to_send, "Device Id: 0x%08x\r\n", device_id);
            
//         RpiSend(to_send);
// #endif
// #ifdef USE_DEVICE_ID_12BYTES
//         sprintf(to_send, "Device Id: 0x%04x%04x%08x%08x\r\n",
//                 *((unsigned short*)0x1FFFF7E8),
//                 *((unsigned short*)(0x1FFFF7E8 + 2)),
//                 *((unsigned int*)(0x1FFFF7E8 + 4)),
//                 *((unsigned int*)(0x1FFFF7E8 + 8)));
        
//         RpiSend(to_send);
// #endif
//     }

    else
    {
        // RpiSend(s_ans_nok);
    }

}


//---- End of File ----//
