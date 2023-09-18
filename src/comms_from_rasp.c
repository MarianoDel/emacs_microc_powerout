//---------------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### COMMS_FROM_RASP.C ############################################
//---------------------------------------------------------

// Includes --------------------------------------------------------------------
// #include "stm32f10x.h"
#include "hard.h"
#include "adc.h"

#include "comms_from_rasp.h"
#include "comms.h"
#include "channels_defs.h"
#include "usart.h"
#include "treatment.h"
#include "utils.h"
#include "antennas.h"

#include "parameters.h"
#include "tamper_funcs.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>



#define SIZEOF_RXDATA    128

// Externals -------------------------------------------------------------------
extern unsigned short comms_messages_rpi;
extern volatile unsigned short adc_ch [];
extern parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------
char local_rasp_buff [SIZEOF_RXDATA];

const char s_getall [] = {"get all conf"};
const char s_buzzer_short [] = {"buzzer short"};
const char s_buzzer_half [] = {"buzzer half"};
const char s_buzzer_long [] = {"buzzer long"};
char s_ans_ok [] = {"OK\r\n"};
char s_ans_nok [] = {"ERROR\r\n"};
    

// Module Private Functions ----------------------------------------------------
static void Raspberry_Messages (char *);
static void SendAllConf (void);
static void SendStatus (void);
resp_e Comms_Signal_Old_Mode (char * m);


// Module Functions ------------------------------------------------------------
void UpdateRaspberryMessages (void)
{
    if (RpiHaveData())
    {
        RpiHaveDataReset();
        HARD_L1_ON();
        RpiReadBuffer(local_rasp_buff, SIZEOF_RXDATA);
        Raspberry_Messages(local_rasp_buff);
        HARD_L1_OFF();
    }
}


static void Raspberry_Messages (char * msg)
{
    resp_e resp = resp_ok;

    const char s_frequency [] = {"frequency"};
    unsigned short new_freq_int = 0;
    unsigned short new_freq_dec = 0;
    unsigned char figures = 0;


    //mensajes nuevos
    if (!strncmp(msg, (const char *)"signal triangular", (sizeof("signal triangular") - 1)))
    {
        Treatment_SetSignalType(TRIANGULAR_SIGNAL);
        RpiSend(s_ans_ok);
    }

    else if (!strncmp(msg, (const char *)"signal square", (sizeof("signal square") - 1)))
    {
        Treatment_SetSignalType(SQUARE_SIGNAL);
        RpiSend(s_ans_ok);
    }

    else if (!strncmp(msg, (const char *)"signal sinusoidal", (sizeof("signal sinusoidal") - 1)))
    {
        Treatment_SetSignalType(SINUSOIDAL_SIGNAL);
        RpiSend(s_ans_ok);
    }

    else if (!strncmp(msg, (const char *)"power", (sizeof("power") - 1)))
    {
        char num [3];
        unsigned short power;

        resp = resp_error;

        if (strlen(msg) >= 9)
        {
            strncpy(num, (msg + 6), 3);
            power = atoi(num);

            if (power <= 100)
            {
                if (Treatment_SetPower ((unsigned char) power) == resp_ok)
                {
                    resp = resp_ok;
                    comms_messages_rpi |= COMM_CONF_CHANGE;
                }
            }
        }
            
        if (resp == resp_ok)
            RpiSend(s_ans_ok);
        else
            RpiSend(s_ans_nok);
    }

    //-- Frequency Setting
    else if (!strncmp(msg, s_frequency, sizeof(s_frequency) - 1))
    {
        // char to_send [64];
        
        msg += sizeof(s_frequency);    //normalizo al payload, hay un espacio

        // what we get is E.DD | EE.DD | EEE.DD, always two decimal positions
        figures = StringIsANumber(msg, &new_freq_int);

        if ((figures) && (figures <= 3) && (new_freq_int < 210))
        {
            msg += figures + 1;    //normalizo con el punto
            figures = StringIsANumber(msg, &new_freq_dec);

            if ((figures > 1) && (figures < 3))
            {
                resp = Treatment_SetFrequency ((unsigned char) new_freq_int, (unsigned char) new_freq_dec);
                if (resp == resp_ok)
                    comms_messages_rpi |= COMM_CONF_CHANGE;                
            }
        }

        if (resp == resp_ok)
            RpiSend(s_ans_ok);
        else
            RpiSend(s_ans_nok);
    }
    
    else if (!strncmp(msg, "goto bridge mode", sizeof("goto bridge mode") - 1))
    {
        comms_messages_rpi |= COMM_GOTO_BRIDGE;
        RpiSend((char *)"Going to Bridge Mode...\r\n");
    }

    else if (!strncmp(msg, "voltage", sizeof("voltage") - 1))
    {
        char to_send [64];
        float fcalc = 1.0;
        short volt_int, volt_dec;

        fcalc = Sense_200V;
        fcalc = fcalc * K_200V;
        volt_int = (short) fcalc;
        fcalc = fcalc - volt_int;
        fcalc = fcalc * 10;
        volt_dec = (short) fcalc;
        sprintf(to_send, "High Supply: %3d.%01dV\r\n", volt_int, volt_dec);
        RpiSend(to_send);

        fcalc = Sense_15V;
        fcalc = fcalc * K_15V;
        volt_int = (short) fcalc;
        fcalc = fcalc - volt_int;
        fcalc = fcalc * 10;
        volt_dec = (short) fcalc;        
        sprintf(to_send, "Low Supply: %3d.%01dV\r\n", volt_int, volt_dec);
        RpiSend(to_send);
        
    }
    
    else if (!strncmp(msg, "hard_soft", sizeof("hard_soft") - 1))
    {
        char to_send [80];
        sprintf(to_send, "%s\r\n%s\r\n", HARD, SOFT);
        RpiSend(to_send);
    }


    //-- Buzzer Actions
    else if (strncmp(msg, s_buzzer_short, sizeof(s_buzzer_short) - 1) == 0)
    {
        unsigned short bips_qtty = 0;
        
        msg += sizeof(s_buzzer_short);		//normalizo al payload, hay un espacio

        //lo que viene es un byte de 1 a 9
        figures = StringIsANumber(msg, &bips_qtty);
        if (figures == 1)
        {
            BuzzerCommands(BUZZER_SHORT_CMD, (unsigned char) bips_qtty);
            RpiSend(s_ans_ok);
        }
        else
            resp = resp_error;
    }

    else if (strncmp(msg, s_buzzer_half, sizeof(s_buzzer_half) - 1) == 0)
    {
        unsigned short bips_qtty = 0;
        
        msg += sizeof(s_buzzer_half);		//normalizo al payload, hay un espacio

        //lo que viene es un byte de 1 a 9
        figures = StringIsANumber(msg, &bips_qtty);
        if (figures == 1)
        {
            BuzzerCommands(BUZZER_HALF_CMD, (unsigned char) bips_qtty);
            RpiSend(s_ans_ok);
        }
        else
            RpiSend(s_ans_nok);

    }

    else if (strncmp(msg, s_buzzer_long, sizeof(s_buzzer_long) - 1) == 0)
    {
        unsigned short bips_qtty = 0;
        
        msg += sizeof(s_buzzer_long);		//normalizo al payload, hay un espacio

        //lo que viene es un byte de 1 a 9
        figures = StringIsANumber(msg, &bips_qtty);
        if (figures == 1)
            BuzzerCommands(BUZZER_LONG_CMD, (unsigned char) bips_qtty);
        else
            resp = resp_error;
    }

    // old is keepalive, new is keepalive alone
    else if (!strncmp(msg, (const char *)"keepalive", (sizeof("keepalive") - 1)))
    {
        RpiSend(s_ans_ok);
    }

    else if (!strncmp(msg, (const char *)"duration,", (sizeof("duration,") - 1)))
    {
        unsigned short new_time = 0;
        
        msg += sizeof("duration,") - 1;		//normalizo al payload

        // check old or new conf string
        //example.	duration,00,10,00,1
        if ((*(msg + 2) == ',') &&
            (*(msg + 5) == ',') &&
            (*(msg + 8) == ','))
        {
            // old conf type
            unsigned char hours = (*(msg + 0) - '0') * 10 + *(msg + 1) - '0';
            unsigned char minutes = (*(msg + 3) - '0') * 10 + *(msg + 4) - '0';

            figures = 3;
            new_time = hours * 60 + minutes;
        }
        else
        {
            // new conf type
            //lo que viene son tres bytes con el tiempo de 1 a 120 se supone
            figures = StringIsANumber(msg, &new_time);
        }
        
        if (figures == 3)
        {
            if (Treatment_SetTimeinMinutes(new_time) == resp_ok)
            {
                RpiSend(s_ans_ok);
                comms_messages_rpi |= COMM_CONF_CHANGE;
            }
            else
                RpiSend(s_ans_nok);
        }
        else
            RpiSend(s_ans_nok);
    }

    else if (!strncmp((const char *)msg, (const char *)"serial num", (sizeof("serial num") - 1)))
    {
        char to_send[40] = { 0 };
        
#ifdef USE_DEVICE_ID_4BYTES
        unsigned int device_id = *((unsigned short*)0x1FFFF7E8);
        device_id <<= 16;
        device_id |= *((unsigned short*)(0x1FFFF7E8 + 2));
        device_id ^= *((unsigned int*)(0x1FFFF7E8 + 4));
        device_id ^= *((unsigned int*)(0x1FFFF7E8 + 8));
        sprintf(to_send, "Device Id: 0x%08x\r\n", device_id);
            
        RpiSend(to_send);
#endif
#ifdef USE_DEVICE_ID_12BYTES
        sprintf(to_send, "Device Id: 0x%04x%04x%08x%08x\r\n",
                *((unsigned short*)0x1FFFF7E8),
                *((unsigned short*)(0x1FFFF7E8 + 2)),
                *((unsigned int*)(0x1FFFF7E8 + 4)),
                *((unsigned int*)(0x1FFFF7E8 + 8)));
        
        RpiSend(to_send);
#endif
    }

    // -- Tamper settings ----
    else if (!strncmp((const char *)msg, "tamper get status", (sizeof("tamper get status") - 1)))
    {
        tamper_state_e t;
        t = Tamper_GetStatus(&mem_conf);
        RpiSend(Tamper_CodeToString(t));
        RpiSend("\r\n");
    }

    else if (!strncmp((const char *)msg, "tamper disable", (sizeof("tamper disable") - 1)))
    {
        tamper_state_e t;
        t = Tamper_SetStatus(&mem_conf, TAMPER_DISABLE);
        RpiSend(Tamper_CodeToString(t));
        RpiSend("\r\n");        
    }

    else if (!strncmp((const char *)msg, "tamper enable", (sizeof("tamper enable") - 1)))
    {
        tamper_state_e t;
        t = Tamper_SetStatus(&mem_conf, TAMPER_ENABLE);
        RpiSend(Tamper_CodeToString(t));
        RpiSend("\r\n");        
    }
    
    //-- end of new messages
    
    //-- old type messages
    else if (!strncmp(msg, (const char *)"get_temp,", (sizeof("get_temp,") - 1)))
    {
        switch (*(msg+9))
        {
        case '1':
            // UART_CH1_Send("get_temp\r\n");
            RpiSend(s_ans_ok);
            break;

        case '2':
            // UART_CH2_Send("get_temp\r\n");
            RpiSend(s_ans_ok);
            break;

        case '3':
            // UART_CH3_Send("get_temp\r\n");
            RpiSend(s_ans_ok);
            break;

        default:
            RpiSend(s_ans_nok);
            break;
        }
    }

    else if (!strncmp(msg, (const char *)"stop,", (sizeof("stop,") - 1)))
    {
        comms_messages_rpi |= COMM_STOP_TREAT;
    }

    else if (!strncmp(msg, (const char *)"pause,", (sizeof("pause,") - 1)))
    {
        comms_messages_rpi |= COMM_PAUSE_TREAT;
    }
    
    else if (!strncmp(msg,(const char *)"start,", (sizeof("start,") - 1)))
    {
        comms_messages_rpi |= COMM_START_TREAT;
    }


    else if (strncmp(msg, s_getall, sizeof(s_getall) - 1) == 0)
        SendAllConf();

    else if (strncmp(msg, (const char *) "get status", sizeof("get status") - 1) == 0)
        SendStatus();
    

    //-- old type for signals and power
    //example. signal,100,100,0000,0003,0003,0003,0006,0000,0000,1
    else if (!strncmp((const char *)&msg[0], (const char *)"signal,", (sizeof("signal,") - 1)))
    {
        if (Comms_Signal_Old_Mode (msg) == resp_error)
            RpiSend(s_ans_nok);
        else
            RpiSend(s_ans_ok);
        
    }

    else if (!strncmp((const char *)&msg[0],
                      (const char *)"state_of_stage,",
                      (sizeof("state_of_stage,") - 1)))
    {
        // do nothing with this info
    }


    else if (!strncmp((const char *)&msg[0],
                      (const char *)"get_antenna,",
                      (sizeof("get_antenna,") - 1)))
    {
        RpiSend(s_ans_ok);
        AntennaSendKnowInfoWithTimer ();            
    }
    
    //-- end of old type messages
    else
    {
        // RpiSend(msg);
        // RpiSend(" ");
        RpiSend(s_ans_nok);
    }

}

//reporta las errores y los limpia
//TODO: o limpiar en otra funcion???
void Raspberry_Report_Errors (unsigned char ch, unsigned short * errors)
{
    //reporta errores como "ERROR(0xNN)\r\n"
    //0x1N antena desconectada
    //0x2N antena perdida
    //0x4N sobre temperatura
    //0x5N sobre corriente
    //N num de canal 1-4

    if (*errors & COMM_ERROR_NO_COMM)
    {
        if (ch == CH1)
            RpiSend("ERROR(0x11)\r\n");

        if (ch == CH2)
            RpiSend("ERROR(0x12)\r\n");

        if (ch == CH3)
            RpiSend("ERROR(0x13)\r\n");

        *errors &= ~COMM_ERROR_NO_COMM;
    }

    if (*errors & COMM_ERROR_NO_CURRENT)
    {
        if (ch == CH1)
            RpiSend("ERROR(0x21)\r\n");

        if (ch == CH2)
            RpiSend("ERROR(0x22)\r\n");

        if (ch == CH3)
            RpiSend("ERROR(0x23)\r\n");

        *errors &= ~COMM_ERROR_NO_CURRENT;
    }

    if (*errors & COMM_ERROR_OVERTEMP)
    {
        if (ch == CH1)
            RpiSend("ERROR(0x41)\r\n");

        if (ch == CH2)
            RpiSend("ERROR(0x42)\r\n");

        if (ch == CH3)
            RpiSend("ERROR(0x43)\r\n");

        *errors &= ~COMM_ERROR_OVERTEMP;
    }
    
    if (*errors & COMM_ERROR_OVERCURRENT)
    {
        if (ch == CH1)
            RpiSend("ERROR(0x51)\r\n");

        if (ch == CH2)
            RpiSend("ERROR(0x52)\r\n");

        if (ch == CH3)
            RpiSend("ERROR(0x53)\r\n");

        *errors &= ~COMM_ERROR_OVERCURRENT;
    }

    if (*errors & COMM_ERROR_SOFT_OVERCURRENT)
    {
        if (ch == CH1)
            RpiSend("ERROR(0x61)\r\n");

        if (ch == CH2)
            RpiSend("ERROR(0x62)\r\n");

        if (ch == CH3)
            RpiSend("ERROR(0x63)\r\n");

        *errors &= ~COMM_ERROR_SOFT_OVERCURRENT;
    }

    if (*errors & COMM_ERROR_NO_TREATMENT)
    {
        if (ch == CH1)
            RpiSend("ERROR(0x71)\r\n");

        if (ch == CH2)
            RpiSend("ERROR(0x72)\r\n");

        if (ch == CH3)
            RpiSend("ERROR(0x73)\r\n");

        *errors &= ~COMM_ERROR_NO_TREATMENT;
    }
}

static void SendAllConf (void)
{
    char b [128];
    Treatment_GetAllConf(b);
    RpiSend(b);
}

static void SendStatus (void)
{
    // char b [128];
    // unsigned char st = 0;
    
    // st = Treatment_GetMainState();
    // sprintf(b, "Main State: %d\r\n", st);
    // RpiSend(b);
}


resp_e Comms_Signal_Old_Mode (char * m)
{
    // signals typedef
    unsigned char initial_power;
    unsigned char final_power;
    // unsigned char sync_on;
    // unsigned short step_number;

    unsigned short rising_time;
    unsigned short maintenance_time;
    unsigned short falling_time;
    unsigned short low_time;

    // unsigned short burst_mode_on;
    // unsigned short burst_mode_off;
    
    //example. signal,100,100,0000,0003,0003,0003,0006,0000,0000,1
    // check the comma separated fields
    if ((*(m + 10) != ',') ||
        (*(m + 14) != ',') ||
        (*(m + 19) != ',') ||
        (*(m + 24) != ',') ||
        (*(m + 29) != ',') ||
        (*(m + 34) != ',') ||
        (*(m + 39) != ',') ||
        (*(m + 44) != ',') ||
        (*(m + 49) != ','))
    {
        return resp_error;
    }
                
    // check the stage (1 to 3 WM_UP MAINT CL_DW)
    if (((*(m + 50) - '0') < 1) ||
        ((*(m + 50) - '0') > 3))
    {
        return resp_error;
    }
        
    initial_power = (m[7] - 48) * 100 + (m[8] - 48) * 10 + (m[9] - 48);
    final_power = (m[11] - 48) * 100 + (m[12] - 48) * 10 + (m[13] - 48);
    // sync_on = (m[18] - 48);

    // get max power
    if (final_power < initial_power)
        final_power = initial_power;
    else
        initial_power = final_power;

    // set the power
    if (Treatment_SetPower (initial_power) != resp_ok)
        return resp_error;

    // check which signal will use
    rising_time = (m[20] - 48) * 1000 +
        (m[21] - 48) * 100 +
        (m[22] - 48) * 10 +
        (m[23] - 48);
            
    maintenance_time = (m[25] - 48) * 1000 +
        (m[26] - 48) * 100 +
        (m[27] - 48) * 10 +
        (m[28] - 48);
            
    falling_time = (m[30] - 48) * 1000 +
        (m[31] - 48) * 100 +
        (m[32] - 48) * 10 +
        (m[33] - 48);
            
    low_time = (m[35] - 48) * 1000 +
        (m[36] - 48) * 100 +
        (m[37] - 48) * 10 +
        (m[38] - 48);

    // square or sinusoidal
    if (rising_time == falling_time)
    {
        if (rising_time == 1)
            Treatment_SetSignalType(SQUARE_SIGNAL);
        else
            Treatment_SetSignalType(SINUSOIDAL_SIGNAL);
    }
    else
    {
        // triangular
        Treatment_SetSignalType(TRIANGULAR_SIGNAL);
    }

    // set frequency
    unsigned short period = rising_time + maintenance_time + falling_time + low_time;

    if (period == 0)
        return resp_error;

    unsigned int freq = 0;
    unsigned int freq_int = 0;
    unsigned int freq_dec = 0;    

    // check freq greater than 1Hz
    freq = 100000 / period;
    freq_int = freq / 100;
    freq_dec = freq  - freq_int * 100;

    if (Treatment_SetFrequency ((unsigned char) freq_int, (unsigned char) freq_dec) != resp_ok)
        return resp_error;

    // signal.burst_mode_on = (m[40] - 48) * 1000 +
    //     (m[41] - 48) * 100 +
    //     (m[42] - 48) * 10 +
    //     (m[43] - 48);
            
    // signal.burst_mode_off = (m[45] - 48) * 1000 +
    //     (m[46] - 48) * 100 +
    //     (m[47] - 48) * 10 +
    //     (m[48] - 48);

    return resp_ok;
}





















//---- End of File ----//
