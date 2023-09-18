//---------------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TREATMENT.C ###############################
//---------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "treatment.h"
#include "comms.h"
#include "hard.h"
#include "usart.h"
#include "tim.h"

#include "antennas_defs.h"
#include "antennas.h"
#include "signals.h"
#include "channels_defs.h"
#include "errors.h"

#include "comms_from_rasp.h"
#include "gpio.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


// Private Types Constants and Macros ------------------------------------------
#define RPI_Send(X)    Usart1Send(X)

typedef enum {
    TREATMENT_STANDBY = 0,
    TREATMENT_CHECK_ANTENNAS_CONNECTED,
    TREATMENT_STARTING,    
    TREATMENT_RUNNING,
    TREATMENT_PAUSED,
    TREATMENT_WITH_ERRORS,
    TREATMENT_STOPPING

} treatment_t;

#define ENABLE_CHX_MASK    0x40
#define DISABLE_CHX_MASK    0x80
#define CHX_MASK        0x0F


// #define TREATMENT_USE_SIGNALS_WITH_FEEDBACK
#define TREATMENT_USE_SIGNALS_OPEN_LOOP


// Externals -------------------------------------------------------------------
extern unsigned short comms_messages_rpi;


// Globals ---------------------------------------------------------------------
treatment_conf_t treatment_conf;
unsigned char global_error = 0;
treatment_t treat_state = 0;


volatile unsigned short secs_in_treatment = 0;
volatile unsigned short millis = 0;
unsigned short secs_end_treatment = 0;
unsigned short secs_elapsed_up_to_now = 0;

#define RPI_Flush_Comms (comms_messages_rpi &= ~COMM_RPI_ALL_MSG_MASK)


// Module Private Functions ----------------------------------------------------
void Treatment_Check_Channels_Loop (void);
resp_e Treatment_Check_All_Errors (void);


// Module Functions ------------------------------------------------------------
void Treatment_Manager (void)
{
    char buff [100] = { 0 };
    resp_e resp = resp_error;
    unsigned char some_channel = 0;
    
    switch (treat_state)
    {
    case TREATMENT_STANDBY:
        if (comms_messages_rpi & COMM_START_TREAT)
        {
            // rpi asks to start treatment
            comms_messages_rpi &= ~COMM_START_TREAT;
            if (Treatment_AssertParams() == resp_error)
            {
                RPI_Send("ERROR on params\r\n");
            }
            else
            {
                signals_struct_t new_treat_data;
                unsigned char fi = 0;
                unsigned char fd = 0;
                Treatment_GetFrequency(&fi, &fd);
                new_treat_data.freq_int = fi;
                new_treat_data.freq_dec = fd;
                new_treat_data.signal = Treatment_GetSignalType();
                new_treat_data.power = Treatment_GetPower();                
                Signals_Setup_Treatment_Data(&new_treat_data);
#ifdef TESTING_SHOW_INFO
                printf("\n treatment -> signal freq: %02d.%02dHz sig: %d power: %d\n",
                       new_treat_data.freq_int,
                       new_treat_data.freq_dec,
                       new_treat_data.signal,
                       new_treat_data.power);
#endif
                
                RPI_Send("OK\r\n");                
                treat_state = TREATMENT_CHECK_ANTENNAS_CONNECTED;
            }
        }
        RPI_Flush_Comms;
        break;

    case TREATMENT_CHECK_ANTENNAS_CONNECTED:
        strcpy(buff, "treat start, ");
        
        for (int i = 0; i < 4; i++)
        {
            resp = AntennaVerifyForTreatment(i);

            if (resp == resp_ok)
            {
                char buff_ch [10];
                antenna_st ch_ant;
                
                some_channel++;
                Signals_Set_Reset_Channel_For_Treatment(i, 1);
                AntennaGetParamsStruct(i, &ch_ant);
#if (defined TREATMENT_USE_SIGNALS_WITH_FEEDBACK)
                Signals_Set_Channel_PI_Parameters (i, &ch_ant);
#elif (defined TREATMENT_USE_SIGNALS_OPEN_LOOP)
                Signals_Set_Channel_Table_Open_Loop (i, &ch_ant);
#else
#error "Select type of generation signal on treatment.c"
#endif

                Error_SetStatus_For_Checks(0x80, i);    // checks on this channel
                sprintf(buff_ch, "ch%d ", i + 1);                
                strcat(buff, buff_ch);
            }
            else
            {
                Error_SetStatus_For_Checks(0, i);    // no checks on this channel
                Signals_Set_Reset_Channel_For_Treatment(i, 0);
            }
            
        }

        if (some_channel)
        {
            treat_state = TREATMENT_STARTING;

            // resets all errors status
            for (int i = 0; i < 4; i++)
                Error_SetStatus(0, i);

            // show channels in treatment
            strcat (buff, "\r\n");
            RPI_Send(buff);

            // check for sync
            unsigned char f_int = 0;
            unsigned char f_dec = 0;
            if (TIM1_SyncVerify(&f_int, &f_dec) != 0)
            {
                sprintf(buff,"sync on freq: %d.%02dHz\r\n", f_int, f_dec);
                RpiSend(buff);
                Signals_Sync_Enable();
            }
            else
            {
                RpiSend("no sync needed\r\n");
                Signals_Sync_Disable();
            }
        }
        else
        {
            RPI_Send("ERROR no antenna connected\r\n");
            treat_state = TREATMENT_STANDBY;
        }
        break;
        
    case TREATMENT_STARTING:
        secs_end_treatment = Treatment_GetTime();
        secs_in_treatment = 1;    // a 1 here starts the timer
        secs_elapsed_up_to_now = 0;

        // enable the ints protecction
        EXTIOn ();
            
        treat_state = TREATMENT_RUNNING;
        ChangeLed(LED_TREATMENT_GENERATING);
#ifdef USE_BUZZER_ON_START
        BuzzerCommands(BUZZER_HALF_CMD, 1);
#endif

#if (defined TREATMENT_USE_SIGNALS_WITH_FEEDBACK)
        Signals_Setup_All_Channels ();    // consecutive starts and stops will always use this info
#elif (defined TREATMENT_USE_SIGNALS_OPEN_LOOP)
        Signals_Setup_All_Channels_Open_Loop ();
#else
#error "Select type of generation signal on treatment.c"
#endif
        break;

    case TREATMENT_RUNNING:
#if (defined TREATMENT_USE_SIGNALS_WITH_FEEDBACK)
        Signals_Generate_All_Channels ();
#elif (defined TREATMENT_USE_SIGNALS_OPEN_LOOP)
        Signals_Generate_All_Channels_Open_Loop ();
#else
#error "Select type of generation signal on treatment.c"
#endif

        
        if (comms_messages_rpi & COMM_PAUSE_TREAT)
        {
            comms_messages_rpi &= ~COMM_PAUSE_TREAT;
            RPI_Send("OK\r\n");
            ChangeLed(LED_TREATMENT_PAUSED);
            secs_elapsed_up_to_now = secs_in_treatment;
            Signals_Stop_All_Channels ();
            treat_state = TREATMENT_PAUSED;            
        }

        if (comms_messages_rpi & COMM_STOP_TREAT)
        {
            comms_messages_rpi &= ~COMM_STOP_TREAT;

            // treatment finish by other timer or stop, finish_ok,
            RPI_Send("OK\r\n");
            Signals_Stop_All_Channels ();            
            treat_state = TREATMENT_STOPPING;
        }

        // someone sends me another start???
        if (comms_messages_rpi & COMM_START_TREAT)
        {
            comms_messages_rpi &= ~COMM_START_TREAT;
            RPI_Send("ERROR\r\n");
        }

        if (secs_in_treatment >= secs_end_treatment)
        {
            // treatment finish by own timer
            RPI_Send("ended ok\r\n");
#ifdef USE_BUZZER_ON_END
            BuzzerCommands(BUZZER_SHORT_CMD, 3);
#endif
            Signals_Stop_All_Channels ();
            treat_state = TREATMENT_STOPPING;
        }

        // checks channels for errors
        Treatment_Check_Channels_Loop ();
        resp = Treatment_Check_All_Errors ();

        if (resp == resp_error)
            treat_state = TREATMENT_WITH_ERRORS;
        
        RPI_Flush_Comms;
        break;

    case TREATMENT_PAUSED:
        // un segundo pause, me hace arrancar nuevamente
        if (comms_messages_rpi & COMM_PAUSE_TREAT)
        {
            comms_messages_rpi &= ~COMM_PAUSE_TREAT;
            secs_in_treatment = secs_elapsed_up_to_now;
            RPI_Send("OK\r\n");
            treat_state = TREATMENT_RUNNING;
            ChangeLed(LED_TREATMENT_GENERATING);
        }

        if (comms_messages_rpi & COMM_STOP_TREAT)
        {
            //estaba en pausa y me mandaron stop
            comms_messages_rpi &= ~COMM_STOP_TREAT;
            RPI_Send("OK\r\n");
            // PowerSendStop();
            treat_state = TREATMENT_STOPPING;
        }
        RPI_Flush_Comms;
        break;
                
    case TREATMENT_STOPPING:
        secs_in_treatment = 0;    //con 0 freno el timer
        // sprintf (buff, "treat end, ch1: 0x%04x, ch2: 0x%04x, ch3: 0x%04x\r\n",
        //          comms_messages_1,
        //          comms_messages_2,
        //          comms_messages_3);                    
        // RPI_Send(buff);

        RPI_Send("OK\r\n");
        for (int i = 0; i < 4; i++)
            AntennaEndTreatment(i);

        // disable the ints protecction
        EXTIOff ();
        
        treat_state = TREATMENT_STANDBY;
        ChangeLed(LED_TREATMENT_STANDBY);
        break;

    case TREATMENT_WITH_ERRORS:
        Wait_ms(1000);

        for (int i = 0; i < 4; i++)
            AntennaEndTreatment(i);
        
        RPI_Send("STOP\r\n");
        Wait_ms(1000);
        RPI_Send("STOP\r\n");
        Wait_ms(1000);
        RPI_Send("Flushing errors\r\n");
        Wait_ms(1000);
        
        treat_state = TREATMENT_STANDBY;
        ChangeLed(LED_TREATMENT_STANDBY);
        break;

    default:
        treat_state = TREATMENT_STANDBY;
        break;
    }            


    // check rasp comms at all time
    UpdateRaspberryMessages();

    UpdateLed();
    
    UpdateBuzzer();
}


int loop_ch = 0;
void Treatment_Check_Channels_Loop (void)
{
    unsigned char error_ch = 0;    
    char buff_err [30] = { 0 };
    
    if (loop_ch < CH4)
        loop_ch++;
    else
        loop_ch = CH1;

    error_ch = Error_GetStatus(loop_ch);

    // channel need a report or not
    if (!(error_ch & 0x80))
        return;

    // report already sended
    if ((error_ch & 0x40))
        return;

    // else report the error
    if (error_ch & 0x0F)
    {
        Error_SetString(buff_err, ((0x0F & error_ch) << 4) | (loop_ch + 1));
        RPI_Send(buff_err);
        Error_SetStatus_For_Checks((0x80 | 0x40), loop_ch);
    }

    // reports on antennas
    // antenna disconnected
    if (!AntennaGetConnection (loop_ch))
    {
        // set the error
        Error_SetStatus(ERROR_ANTENNA_LOST, loop_ch);

        // report the error
        Error_SetString(buff_err, (ERROR_ANTENNA_LOST << 4) | (loop_ch + 1));
        RPI_Send(buff_err);
        Error_SetStatus_For_Checks((0x80 | 0x40), loop_ch);

        // disable the channel
        Signals_Stop_Single_Channel(loop_ch);
        Signals_Set_Reset_Channel_For_Treatment(loop_ch, 0);        
    }

    // antenna temp
    if (AntennaGetTempStatus (loop_ch))
    {
        // set the error
        Error_SetStatus(ERROR_OVERTEMP, loop_ch);

        // report the error
        Error_SetString(buff_err, (ERROR_OVERTEMP << 4) | (loop_ch + 1));
        RPI_Send(buff_err);
        Error_SetStatus_For_Checks((0x80 | 0x40), loop_ch);

        // disable the channel
        Signals_Stop_Single_Channel(loop_ch);
        Signals_Set_Reset_Channel_For_Treatment(loop_ch, 0);                
    }
}


resp_e Treatment_Check_All_Errors (void)
{
    unsigned char error_ch1 = 0;
    unsigned char error_ch2 = 0;
    unsigned char error_ch3 = 0;
    unsigned char error_ch4 = 0;    

    error_ch1 = Error_GetStatus(CH1);
    error_ch2 = Error_GetStatus(CH2);
    error_ch3 = Error_GetStatus(CH3);
    error_ch4 = Error_GetStatus(CH4);

    // needs a report and was sended?
    if (error_ch1 & 0x80)
    {
        if (!(error_ch1 & 0x40))
            return resp_ok;
    }

    if (error_ch2 & 0x80)
    {
        if (!(error_ch2 & 0x40))
            return resp_ok;
    }

    if (error_ch3 & 0x80)
    {
        if (!(error_ch3 & 0x40))
            return resp_ok;
    }

    if (error_ch4 & 0x80)
    {
        if (!(error_ch4 & 0x40))
            return resp_ok;
    }

    // printf(" error in all channels: %02x %02x %02x %02x\n", error_ch1, error_ch2, error_ch3, error_ch4);
    return resp_error;
}


resp_e Treatment_SetSignalType (signal_type_e a)
{
    if ((a == SQUARE_SIGNAL) ||
        (a == TRIANGULAR_SIGNAL) ||
        (a == SINUSOIDAL_SIGNAL))

        treatment_conf.treatment_signal.signal = a;
    else
        return resp_error;

    return resp_ok;
}


signal_type_e Treatment_GetSignalType (void)
{
    return treatment_conf.treatment_signal.signal;
}


resp_e Treatment_SetFrequency (unsigned char freq_int, unsigned char freq_dec)
{
    treatment_conf.treatment_signal.freq_int = freq_int;
    treatment_conf.treatment_signal.freq_dec = freq_dec;
    
    return resp_ok;

    // resp_e resp = resp_error;
    // unsigned int calc = 1000000;
    // unsigned int freq = 0;

    // //el synchro es un timer con tick cada 100us
    // //la cuenta para 2 decimales da 1M/(freq*100)
    // freq = freq_int * 100;
    // freq += freq_dec;
    // freq = freq * K_SYNCHRO_ADJUST;
    // freq = freq / 100;

    // calc = calc / freq;
    // if ((calc < TIMER_SYNCHRO_MAX) && (calc > TIMER_SYNCHRO_MIN))
    // {
    //     treatment_conf.treatment_signal.freq_int = freq_int;
    //     treatment_conf.treatment_signal.freq_dec = freq_dec;
    //     // treatment_conf.timer_synchro = (unsigned short) calc;
    //     resp = resp_ok;
    // }
    
    // return resp;
}


void Treatment_SetChannelsFlag (unsigned char  a)
{
    if (a & ENABLE_CHX_MASK)
        treatment_conf.channels_in_treatment |= (a & CHX_MASK);

    if (a & DISABLE_CHX_MASK)
        treatment_conf.channels_in_treatment &= (~(a & CHX_MASK));
}


unsigned char Treatment_GetChannelsFlag (void)
{
    return (treatment_conf.channels_in_treatment & CHX_MASK);
}

// resp_e TreatmentChannelFlags (ch_in_treatment_t a)
// {
//     if (a == CH1_ENABLE)
//         treatment_conf.ch1_enable = 1;

//     if (a == CH2_ENABLE)
//         treatment_conf.ch2_enable = 1;

//     if (a == CH3_ENABLE)
//         treatment_conf.ch3_enable = 1;

//     if (a == CH1_DISABLE)
//         treatment_conf.ch1_enable = 0;

//     if (a == CH2_DISABLE)
//         treatment_conf.ch2_enable = 0;

//     if (a == CH3_DISABLE)
//         treatment_conf.ch3_enable = 0;
    
//     return resp_ok;
// }

void Treatment_GetFrequency (unsigned char * f_int, unsigned char * f_dec)
{
    *f_int = treatment_conf.treatment_signal.freq_int;
    *f_dec = treatment_conf.treatment_signal.freq_dec;    
}


resp_e Treatment_SetPower (unsigned char a)
{
    if (a > 100)
        treatment_conf.treatment_signal.power = 100;
    else if (a < 10)
        treatment_conf.treatment_signal.power = 10;
    else
        treatment_conf.treatment_signal.power = a;

    return resp_ok;
}


unsigned char Treatment_GetPower (void)
{
    return treatment_conf.treatment_signal.power;
}


resp_e Treatment_SetTime (unsigned char h, unsigned char m, unsigned char s)
{
    if ((h > 1) || (m > 60) || (s > 60))
        return resp_error;

    treatment_conf.treatment_time = h * 3600;
    treatment_conf.treatment_time += m * 60;
    treatment_conf.treatment_time += s;
    
    return resp_ok;
}


resp_e Treatment_SetTimeinMinutes (unsigned short m)
{
    if (m > 120)
        return resp_error;

    treatment_conf.treatment_time = m * 60;
    
    return resp_ok;
}


unsigned short Treatment_GetTime (void)
{
    return treatment_conf.treatment_time;
}


void Treatment_GetAllConf (char * tosend)
{
    char buf[30];

    // Selected signal
    sprintf(buf, "signal: %d\n", treatment_conf.treatment_signal.signal);
    strcpy(tosend, buf);

    // Selected Freq
    sprintf(buf, "freq: %d.%02dHz\n",
            treatment_conf.treatment_signal.freq_int,
            treatment_conf.treatment_signal.freq_dec);
    strcat(tosend, buf);

    // Selected power
    sprintf(buf, "power: %d\n", treatment_conf.treatment_signal.power);
    strcat(tosend, buf);

    // Treatment duration
    sprintf(buf, "time in s: %d\n", treatment_conf.treatment_time);
    strcat(tosend, buf);

    // Active channels
    // unsigned char c1 = 0;
    // unsigned char c2 = 0;
    // unsigned char c3 = 0;

    // if (treatment_conf.channels_in_treatment & ENABLE_CH1_FLAG)
    //     c1 = 1;

    // if (treatment_conf.channels_in_treatment & ENABLE_CH2_FLAG)
    //     c2 = 1;

    // if (treatment_conf.channels_in_treatment & ENABLE_CH3_FLAG)
    //     c3 = 1;
    
    // sprintf(buf, "in treat ch1:%d ch2:%d ch3:%d\n", c1, c2, c3);
    // strcat(tosend, buf);

    // //Timer for sync
    // unsigned short tsync = 0;
    // // tsync = treatment_conf.timer_synchro / 10;
    // sprintf(buf, "synchro: %dms\n", tsync);
    // strcat(tosend, buf);
}


//verifica que se cumplan con todos los parametros para poder enviar una senial coherente
resp_e Treatment_AssertParams (void)
{
    resp_e resp = resp_error;

    if ((treatment_conf.treatment_signal.power > 100) || (treatment_conf.treatment_signal.power < 10))
        return resp;

    // freq extended
    float calc = 0.0;
    calc = treatment_conf.treatment_signal.freq_int + treatment_conf.treatment_signal.freq_dec / 100.;
    if ((calc < 0.5) && (calc > 99.99))
        return resp;

    if ((treatment_conf.treatment_signal.signal != SQUARE_SIGNAL) &&
        (treatment_conf.treatment_signal.signal != TRIANGULAR_SIGNAL) &&
        (treatment_conf.treatment_signal.signal != SINUSOIDAL_SIGNAL))
        return resp;

    return resp_ok;
}


// treatment_t Treatment_GetState (void)
// {
//     return treat_state;
// }


void Treatment_Timeouts (void)
{
    if (secs_in_treatment)
    {
        if (millis < (1000 - 1)) 
            millis++;
        else
        {
            secs_in_treatment++;
            millis = 0;
        }
    }    
}


//--- end of file ---//
