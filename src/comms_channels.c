//---------------------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### COMMS_CHANNELS.C ##################################
//---------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "comms_channels.h"
#include "channels_defs.h"
#include "answers_defs.h"
#include "usart.h"
#include "antennas.h"
#include "utils.h"


#include <string.h>
#include <stdio.h>


#define SIZEOF_RXDATA    128
#define RpiSend(X)    Usart1Send(X)

// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
char buff [SIZEOF_RXDATA] = { 0 };
char s_ok [] = {"OK\r\n"};
char s_nok [] = {"ERROR\r\n"};



// Module Private Functions ----------------------------------------------------
resp_e ParseCommsWithChannels (char * str, unsigned char channel);
resp_e ParseCurrentTemp (char * str, unsigned char * t_int, unsigned char * t_dec);
resp_e ParseAntennaParams (char * str, antenna_st * antenna);



// Module Functions ------------------------------------------------------------
//channel 1, connected to usart2
void Comms_Channel1 (void)
{
    if (Usart2HaveData())
    {
        Usart2HaveDataReset();
        
        Usart2ReadBuffer (buff, SIZEOF_RXDATA);

        ParseCommsWithChannels((char *) buff, CH1);            
    }
}


//channel 2, connected to usart3
void Comms_Channel2 (void)
{
    if (Usart3HaveData ())
    {
        Usart3HaveDataReset ();
        
        Usart3ReadBuffer (buff, SIZEOF_RXDATA);

        ParseCommsWithChannels((char *) buff, CH2);
    }
}


//channel 3, connected to uart4
void Comms_Channel3 (void)
{
    if (Uart4HaveData())
    {
        Uart4HaveDataReset ();
        
        Uart4ReadBuffer (buff, SIZEOF_RXDATA);

        ParseCommsWithChannels((char *) buff, CH3);
    }
}


//channel 4, connected to uart5
void Comms_Channel4 (void)
{
    if (Uart5HaveData ())
    {
        Uart5HaveDataReset ();
        
        Uart5ReadBuffer (buff, SIZEOF_RXDATA);

        ParseCommsWithChannels((char *) buff, CH4);
        
        // char buff2 [100] = { 0 };
        // sprintf(buff2, "<- %s\n", buff);
        // Usart1Send(buff2);
    }
}


resp_e ParseCommsWithChannels (char * str, unsigned char channel)
{
    resp_e resp = resp_error;
    char dummy_str [30] = { 0 };
    
    // rcv temp,055.00\r\n
    // snd temp,055.00,1\r\n    1 to 4
    if (!strncmp(str, (const char *)"temp", (sizeof("temp") - 1)))
    {
        if ((*(str + 4) == ',') &&
            (*(str + 8) == '.'))
        // if ((*(str + 4) == ',') &&
        //     (*(str + 8) == '.') &&
        //     (*(str + 11) == '\r'))
        {
            unsigned char temp_i = 0;
            unsigned char temp_d = 0;

            if (ParseCurrentTemp(str, &temp_i, &temp_d) == resp_ok)
            {
                AntennaSetCurrentTemp (channel, temp_i, temp_d);

                sprintf(dummy_str, ",%d\r\n", channel + 1);
                strcpy((str + 11), dummy_str);
                RpiSend(str);
                resp = resp_ok;
            }
        }
    }

    //ant0,012.27,087.90,001.80,065.00\r\n.
    else if (!strncmp(str, (const char *)"ant", (sizeof("ant") - 1)))
    {
        // check if antenna is new and process string, or tell the interface the error
        if (*(str + sizeof("ant0,012.27,087.90,001.80,065.") - 1) != '5')
        {
            sprintf(dummy_str, "old antenna ch%d\r\n", channel + 1);
            RpiSend(dummy_str);
            return resp_error;
        }
        
        sprintf(dummy_str, "new antenna ch%d\r\n", channel + 1);
        RpiSend(dummy_str);

        if ((*(str + 4) == ',') &&
            (*(str + 11) == ',') &&
            (*(str + 18) == ',') &&
            (*(str + 25) == ','))
        {
            antenna_st antenna_aux;
            if (ParseAntennaParams ((char *) buff, &antenna_aux) == resp_ok)
            {
                AntennaSetParamsStruct (channel, &antenna_aux);
                resp = resp_ok;
            }
        }
    }

    //respuesta al keepalive
    else if (!strncmp(str, (const char *)"ok", (sizeof("ok") - 1)))
    {
        AntennaIsAnswering(channel);
        resp = resp_ok;
    }

    else if ((!strncmp(str, (const char *)"name:", sizeof("name:") - 1)))
    {
        AntennaSetName(channel, (str + (sizeof("name:") - 1)));
        resp = resp_ok;
    }

    return resp;
}


resp_e ParseAntennaParams (char * str, antenna_st * antenna)
{
    unsigned short i = 0;

    // check all the strings first, then do the load    
    if ((StringCheckNumbers((str + 5), 3) == 3) &&
        (StringCheckNumbers((str + 9), 2) == 2) &&
        (StringCheckNumbers((str + 12), 3) == 3) && 
        (StringCheckNumbers((str + 16), 2) == 2) &&
        (StringCheckNumbers((str + 19), 3) == 3) &&
        (StringCheckNumbers((str + 23), 2) == 2) &&
        (StringCheckNumbers((str + 26), 3) == 3) &&
        (StringCheckNumbers((str + 30), 2) == 2))
    {
        i = (*(str + 5) - '0') * 100;
        i += (*(str + 6) - '0') * 10;
        i += (*(str + 7) - '0');
        
        antenna->resistance_int = i;

        i = (*(str + 9) - '0') * 10;
        i += (*(str + 10) - '0');

        antenna->resistance_dec = i;

        i = (*(str + 12) - '0') * 100;
        i += (*(str + 13) - '0') * 10;
        i += (*(str + 14) - '0');

        antenna->inductance_int = i;

        i = (*(str + 16) - '0') * 10;
        i += (*(str + 17) - '0');

        antenna->inductance_dec = i;

        i = (*(str + 19) - '0') * 100;
        i += (*(str + 20) - '0') * 10;
        i += (*(str + 21) - '0');

        antenna->current_limit_int = i;

        i = (*(str + 23) - '0') * 10;
        i += (*(str + 24) - '0');

        antenna->current_limit_dec = i;

        i = (*(str + 26) - '0') * 100;
        i += (*(str + 27) - '0') * 10;
        i += (*(str + 28) - '0');

        antenna->temp_max_int = i;

        i = (*(str + 30) - '0') * 10;
        i += (*(str + 31) - '0');

        antenna->temp_max_dec = i;

        return resp_ok;
    }

    return resp_error;
}


resp_e ParseCurrentTemp (char * str, unsigned char * t_int, unsigned char * t_dec)
{
    unsigned short i = 0;
    unsigned short d = 0;

    if (StringCheckNumbers((str + 5), 3) == 3)
    {
        i = (*(str + 5) - '0') * 100;
        i += (*(str + 6) - '0') * 10;
        i += (*(str + 7) - '0');
        *t_int = i;
    }
    else
        return resp_error;

    if (StringCheckNumbers((str + 9), 2) == 2)
    {
        d = (*(str + 9) - '0') * 10;
        d += (*(str + 10) - '0');
        *t_dec = d;
    }
    else
        return resp_error;

    return resp_ok;
}


//---- end of file ----//
