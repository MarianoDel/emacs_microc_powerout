//-----------------------------------------------------------
// #### MICROCURRENT CHANNEL PROJECT F103 - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TREATMENT.C #########################################
//-----------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "treatment.h"
#include "comms.h"
#include "hard.h"
#include "usart.h"
#include "tim.h"

#include "utils.h"


#include <string.h>
#include <stdlib.h>
#include <stdio.h>


// Private Types Constants and Macros ------------------------------------------
typedef enum {
    TREATMENT_STANDBY = 0,
    TREATMENT_SQUARE_RUNNING,
    TREATMENT_SINUS_RUNNING,
    TREATMENT_STOPPING

} treatment_e;



// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
treatment_conf_t treatment_conf;
treatment_e treat_state = 0;


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
void Treatment_Manager (void)
{
    switch (treat_state)
    {
    case TREATMENT_STANDBY:    // AND MEASUREMENTS
        if (Treatment_Start_Flag ())
        {
            Treatment_Start_Flag_Reset ();
            if (treatment_conf.mode == MODE_SQUARE)
            {
                Usart1Send("starting square\r\n");
                ChangeLed(LED_TREATMENT_SQUARE_RUNNING);
                treat_state = TREATMENT_SQUARE_RUNNING;
            }
            else if (treatment_conf.mode == MODE_SINUS)
            {
                Usart1Send("starting sinus\r\n");                
                ChangeLed(LED_TREATMENT_SINUS_RUNNING);                
                treat_state = TREATMENT_SINUS_RUNNING;
            }            
        }
        break;

    case TREATMENT_SQUARE_RUNNING:
        Signals_Square (&treatment_conf);

        if (Treatment_Stop_Flag ())
        {
            Treatment_Stop_Flag_Reset ();
            treat_state = TREATMENT_STOPPING;
        }
        break;
        
    case TREATMENT_SINUS_RUNNING:
        Signals_Sinus (&treatment_conf);

        if (Treatment_Stop_Flag ())
        {
            Treatment_Stop_Flag_Reset ();            
            treat_state = TREATMENT_STOPPING;
        }
        break;
                
    case TREATMENT_STOPPING:
        Signals_Stop ();

        Usart1Send("stopped\r\n");

        ChangeLed(LED_TREATMENT_STANDBY);
        treat_state = TREATMENT_STANDBY;
        break;

    default:
        treat_state = TREATMENT_STANDBY;
        break;
    }            
}


resp_e Treatment_SetFrequency_Str (char * str)
{
    resp_e resp = resp_error;
    int figures = 0;
    unsigned short new_freq_int = 0;
    unsigned short new_freq_dec = 0;    
    
    // what we get is E.DD | EE.DD | EEE.DD, always two decimal positions
    figures = StringIsANumber(str, &new_freq_int);

    if ((figures) && (figures <= 3) && (new_freq_int <= 999))
    {
        str += figures + 1;    // point normalization
        figures = StringIsANumber(str, &new_freq_dec);

        if ((figures > 1) && (figures < 3) && (new_freq_dec <= 99))
        {
            resp = Treatment_SetFrequency (new_freq_int, new_freq_dec);
        }
    }

    return resp;
}


resp_e Treatment_SetFrequency (unsigned short fint, unsigned short fdec)
{
    resp_e resp = resp_error;
    
    if ((fint <= 999) && (fdec <= 99))
    {
        treatment_conf.freq_int = fint;
        treatment_conf.freq_dec = fdec;
        resp = resp_ok;
    }

    return resp;
}


resp_e Treatment_SetIntensity_Str (char * str)
{
    resp_e resp = resp_error;
    int figures = 0;
    unsigned short new_intensity = 0;
    
    // what we get is E | EE | EEE, no decimal positions
    figures = StringIsANumber(str, &new_intensity);

    if ((figures) && (figures <= 3) && (new_intensity <= 999))
    {
        resp = Treatment_SetIntensity (new_intensity);
    }

    return resp;
}


resp_e Treatment_SetIntensity (unsigned short intensity)
{
    resp_e resp = resp_error;
    
    if (intensity <= 999)
    {
        treatment_conf.intensity = intensity;
        resp = resp_ok;
    }

    return resp;
}


resp_e Treatment_SetPolarity_Str (char * str)
{
    resp_e resp = resp_error;
    
    if (!strncmp(str, "pos", sizeof("pos") - 1))
    {
        treatment_conf.polarity = POLARITY_POS;
        resp = resp_ok;
    }
    else if (!strncmp(str, "neg", sizeof("neg") - 1))
    {
        treatment_conf.polarity = POLARITY_NEG;
        resp = resp_ok;        
    }
    else if (!strncmp(str, "alt", sizeof("alt") - 1))
    {
        treatment_conf.polarity = POLARITY_ALT;
        resp = resp_ok;        
    }

    return resp;
}


resp_e Treatment_SetMode_Str (char * str)
{
    resp_e resp = resp_error;
    
    if (!strncmp(str, "square", sizeof("square") - 1))
    {
        treatment_conf.mode = MODE_SQUARE;
        resp = resp_ok;        
    }
    else if (!strncmp(str, "sinus", sizeof("sinus") - 1))
    {
        treatment_conf.mode = MODE_SINUS;
        resp = resp_ok;        
    }

    return resp;
}


resp_e Treatment_SetThreshold_Str (char * str)
{
    resp_e resp = resp_error;
    int figures = 0;
    unsigned short new_threshold = 0;
    
    // what we get is E | EE | EEE, no decimal positions
    figures = StringIsANumber(str, &new_threshold);

    if ((figures) && (figures <= 3) && (new_threshold <= 999))
    {
        resp = Treatment_SetThreshold (new_threshold);
    }

    return resp;
}


resp_e Treatment_SetThreshold (unsigned short threshold)
{
    resp_e resp = resp_error;
    
    if (threshold <= 100)
    {
        treatment_conf.threshold = threshold;
        resp = resp_ok;
    }

    return resp;
}


void Treatment_Start (void)
{
    Treatment_Start_Flag_Set ();
}


void Treatment_Stop (void)
{
    Treatment_Stop_Flag_Set ();
}


resp_e Treatment_SetGain_Str (char * str)
{
    resp_e resp = resp_error;
    int figures = 0;
    unsigned short new_gain = 0;
    
    // what we get is E | EE | EEE, no decimal positions
    figures = StringIsANumber(str, &new_gain);

    if ((figures) && (figures <= 3) && (new_gain <= 999))
    {
        resp = Treatment_SetGain (new_gain);
    }

    return resp;
}


resp_e Treatment_SetGain (unsigned short gain)
{
    resp_e resp = resp_error;
    
    if (gain <= 100)
    {
        treatment_conf.gain = gain;
        resp = resp_ok;
    }

    return resp;
}


unsigned char Treatment_GetGain (void)
{
    return treatment_conf.gain;
}


void Treatment_GetAllConf (char * tosend)
{
    // char buf[30];

    // // Selected signal
    // sprintf(buf, "signal: %d\n", treatment_conf.treatment_signal.signal);
    // strcpy(tosend, buf);

    // // Selected Freq
    // sprintf(buf, "freq: %d.%02dHz\n",
    //         treatment_conf.treatment_signal.freq_int,
    //         treatment_conf.treatment_signal.freq_dec);
    // strcat(tosend, buf);

    // // Selected power
    // sprintf(buf, "power: %d\n", treatment_conf.treatment_signal.power);
    // strcat(tosend, buf);

    // // Treatment duration
    // sprintf(buf, "time in s: %d\n", treatment_conf.treatment_time);
    // strcat(tosend, buf);

    // // Active channels
    // // unsigned char c1 = 0;
    // // unsigned char c2 = 0;
    // // unsigned char c3 = 0;

    // // if (treatment_conf.channels_in_treatment & ENABLE_CH1_FLAG)
    // //     c1 = 1;

    // // if (treatment_conf.channels_in_treatment & ENABLE_CH2_FLAG)
    // //     c2 = 1;

    // // if (treatment_conf.channels_in_treatment & ENABLE_CH3_FLAG)
    // //     c3 = 1;
    
    // // sprintf(buf, "in treat ch1:%d ch2:%d ch3:%d\n", c1, c2, c3);
    // // strcat(tosend, buf);

    // // //Timer for sync
    // // unsigned short tsync = 0;
    // // // tsync = treatment_conf.timer_synchro / 10;
    // // sprintf(buf, "synchro: %dms\n", tsync);
    // // strcat(tosend, buf);
}


#define START_SET    0x01
#define START_SET_MASK    0xFE
#define STOP_SET    0x02
#define STOP_SET_MASK    0xFD
unsigned char start_stop_flag = 0;
unsigned char Treatment_Start_Flag (void)
{
    if (start_stop_flag & START_SET)
        return 1;

    return 0;
}


void Treatment_Start_Flag_Set (void)
{
    start_stop_flag |= START_SET;
}


void Treatment_Start_Flag_Reset (void)
{
    start_stop_flag &= START_SET_MASK;
}


unsigned char Treatment_Stop_Flag (void)
{
    if (start_stop_flag & STOP_SET)
        return 1;

    return 0;
}


void Treatment_Stop_Flag_Set (void)
{
    start_stop_flag |= STOP_SET;
}


void Treatment_Stop_Flag_Reset (void)
{
    start_stop_flag &= STOP_SET_MASK;
}

//--- end of file ---//
