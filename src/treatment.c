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
#include "dac.h"

#include "signals.h"
#include "utils.h"
#include "comms_probe.h"
#include "meas.h"
#include "timer_signals.h"


#include <string.h>
#include <stdlib.h>
#include <stdio.h>


// Private Types Constants and Macros ------------------------------------------
typedef enum {
    TREATMENT_INIT = 0,
    TREATMENT_STANDBY_NO_COMMS,
    TREATMENT_STANDBY_SQUARE_WITH_COMMS,
    TREATMENT_STANDBY_SINE_WITH_COMMS,    
    TREATMENT_SQUARE_RUNNING,
    TREATMENT_SINE_RUNNING,
    TREATMENT_STOPPING

} treatment_e;


typedef enum {
    PROBE_NO_COMMS = 0,
    PROBE_PLUGGED_WITH_COMMS

} probe_comms_e;



// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
treatment_conf_t treatment_conf;
treatment_e treat_state = 0;
probe_comms_e probe_comm_state = 0;


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
void Treatment_Manager (void)
{
    unsigned char start_flag = 0;
    unsigned char meas_to_report = 0;
    unsigned short cond_to_report = 0;
    unsigned int res_int = 0;
    
    switch (treat_state)
    {
    case TREATMENT_INIT:
        Signals_Stop ();
        ChangeLed_With_Timer (LED_TREATMENT_STANDBY, 4000);
        treat_state++;
        break;
        
    case TREATMENT_STANDBY_NO_COMMS:
        if (Comms_Rpi_Answering())
        {
            ChangeLed_With_Timer (LED_TREATMENT_STANDBY, 0);

            // for square negative meas
            if (treatment_conf.mode == MODE_SQUARE)
            {
                Timer_Polarity (POLARITY_NEG);
                Meas_Square_Reset();
                treat_state = TREATMENT_STANDBY_SQUARE_WITH_COMMS;
            }
            else
            {
                Timer_Polarity (POLARITY_POS);
                DAC_Output2(0);
                treat_state = TREATMENT_STANDBY_SINE_WITH_COMMS;
            }
        }
        break;
        
    case TREATMENT_STANDBY_SQUARE_WITH_COMMS:    // AND MEASUREMENTS
        
        if (Treatment_Start_Flag ())
        {
            Treatment_Start_Flag_Reset ();

            if (Probe_Get_Status () == CONN_STABLISH)
                start_flag = 1;
        }
        
        if (start_flag)
        {

            if (treatment_conf.mode == MODE_SQUARE)
            {
                Usart1Send("starting square\r\n");
                Signals_Square_Reset ();
                ChangeLed(LED_TREATMENT_SQUARE_RUNNING);
                treat_state = TREATMENT_SQUARE_RUNNING;
                Meas_Online_Reset();
            }
            else if (treatment_conf.mode == MODE_SINE)
            {
                Usart1Send("starting sinusoidal\r\n");
                Signals_Sinusoidal_Reset ();
                ChangeLed(LED_TREATMENT_SINE_RUNNING);                
                treat_state = TREATMENT_SINE_RUNNING;
                Meas_Sine_Reset();
            }            
        }

        // get meas and report every 400ms
        if (treatment_conf.mode == MODE_SQUARE)
        {
            // if (Meas_Square (&meas_to_report))
            // {
            //     // new meas filtered value, report it
            //     char buff [40];
            //     sprintf(buff, "display %d\r\n", meas_to_report);
            //     Usart1Send(buff);
            // }
            if (Meas_Square_V2(&meas_to_report))
            {
                char buff [40];
                sprintf(buff, "display %d\r\n", meas_to_report);
                Usart1Send(buff);                
            }
        }

        // check for change in mode
        if (treatment_conf.mode == MODE_SINE)
        {
            Timer_Polarity (POLARITY_POS);
            DAC_Output2(0);
            treat_state = TREATMENT_STANDBY_SINE_WITH_COMMS;
        }
        break;

    case TREATMENT_STANDBY_SINE_WITH_COMMS:
        
        if (Treatment_Start_Flag ())
        {
            Treatment_Start_Flag_Reset ();

            if (Probe_Get_Status () == CONN_STABLISH)
                start_flag = 1;
        }
        
        if (start_flag)
        {

            if (treatment_conf.mode == MODE_SQUARE)
            {
                Usart1Send("starting square\r\n");
                Signals_Square_Reset ();
                ChangeLed(LED_TREATMENT_SQUARE_RUNNING);
                treat_state = TREATMENT_SQUARE_RUNNING;
            }
            else if (treatment_conf.mode == MODE_SINE)
            {
                Usart1Send("starting sinusoidal\r\n");
                Signals_Sinusoidal_Reset ();
                ChangeLed(LED_TREATMENT_SINE_RUNNING);                
                treat_state = TREATMENT_SINE_RUNNING;
            }            
        }

        // check for change in mode
        if (treatment_conf.mode == MODE_SQUARE)
        {
            Timer_Polarity (POLARITY_NEG);
            Meas_Square_Reset();
            treat_state = TREATMENT_STANDBY_SQUARE_WITH_COMMS;
        }
        break;
        
    case TREATMENT_SQUARE_RUNNING:
        Signals_Square (&treatment_conf);

        if (Treatment_Stop_Flag ())
        {
            Treatment_Stop_Flag_Reset ();
            treat_state = TREATMENT_STOPPING;
        }

        if (Meas_Online_Update (&res_int))
        {
            // new meas filtered value, report it
            char buff [40];
            sprintf(buff, "resistance %d\r\n", res_int);
            Usart1Send(buff);
        }
        
        // go out if not comms?
        break;
        
    case TREATMENT_SINE_RUNNING:
        Signals_Sinusoidal (&treatment_conf);

        if (Treatment_Stop_Flag ())
        {
            Treatment_Stop_Flag_Reset ();            
            treat_state = TREATMENT_STOPPING;
        }

        if (Meas_Sine_Update (&cond_to_report))
        {
            // new meas filtered value, report it
            char buff [40];
            sprintf(buff, "resistance %d\r\n", cond_to_report);
            Usart1Send(buff);
        }
        
        // go out if not comms?        
        break;
                
    case TREATMENT_STOPPING:
        Signals_Stop ();

        Usart1Send("stopped\r\n");

        // for square negative meas
        if (treatment_conf.mode == MODE_SQUARE)
        {
            Timer_Polarity (POLARITY_NEG);
            Meas_Square_Reset();
            treat_state = TREATMENT_STANDBY_SQUARE_WITH_COMMS;
        }
        else
        {
            Timer_Polarity (POLARITY_POS);
            DAC_Output2(0);
            treat_state = TREATMENT_STANDBY_SINE_WITH_COMMS;
        }
        ChangeLed(LED_TREATMENT_STANDBY);
        break;

    default:
        treat_state = TREATMENT_INIT;
        break;
    }

    // check probe communications
    switch (probe_comm_state)
    {
    case PROBE_NO_COMMS:
        break;

    case PROBE_PLUGGED_WITH_COMMS:
        break;

    default:
        probe_comm_state = PROBE_NO_COMMS;
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

        Signals_Set_Frequency_Intensity_Change_Flag ();
        
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

        Signals_Set_Frequency_Intensity_Change_Flag ();
        
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

    if (resp == resp_ok)
        Signals_Set_Frequency_Intensity_Change_Flag ();
    
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
    else if (!strncmp(str, "sine", sizeof("sine") - 1))
    {
        treatment_conf.mode = MODE_SINE;
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
        Meas_Square_Set_Dac_Gain(gain);
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
