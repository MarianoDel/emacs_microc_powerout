//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS_KNOW_ANTENNAS.C #################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "tests_know_antennas.h"

#include <stdio.h>
// #include <string.h>


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Functions ------------------------------------------------------------
int know_antenna = 0;
int TSP_Get_Know_Antennas (antenna_st * p)
{
    int more_antennas = 1;
    
    // set know antennas (from emacs_antenna_magnet/src/main.c)
    switch (know_antenna)
    {
    case 0:
        // const char s_antena [] = { "ant0,012.27,087.90,001.80,065.00\r\n" };
        // const char s_name [] = { "name:Plannar 5 inches\r\n" };
        printf("\nname:Plannar 5 inches in index: %d\n", know_antenna);
        p->resistance_int = 12;
        p->resistance_dec = 27;
        p->inductance_int = 87;
        p->inductance_dec = 90;
        p->current_limit_int = 1;
        p->current_limit_dec = 80;
        p->temp_max_int = 65;
        p->temp_max_dec = 0;
        know_antenna++;
        break;

    case 1:
        // const char s_antena [] = { "ant1,023.85,141.60,001.30,065.00\r\n" };
        // const char s_name [] = { "name:Plannar 5 inches\r\n" };
        printf("\nname:Plannar 5 inches in index: %d\n", know_antenna);
        p->resistance_int = 23;
        p->resistance_dec = 85;
        p->inductance_int = 141;
        p->inductance_dec = 60;
        p->current_limit_int = 1;
        p->current_limit_dec = 30;
        p->temp_max_int = 65;
        p->temp_max_dec = 0;
        know_antenna++;
        break;

    case 2:
        // const char s_antena [] = { "ant1,017.00,120.00,001.30,065.00\r\n" };
        // const char s_name [] = { "name:Plannar 5 inches\r\n" };
        printf("\nname:Plannar 5 inches in index: %d\n", know_antenna);
        p->resistance_int = 17;
        p->resistance_dec = 00;
        p->inductance_int = 120;
        p->inductance_dec = 00;
        p->current_limit_int = 1;
        p->current_limit_dec = 30;
        p->temp_max_int = 65;
        p->temp_max_dec = 0;
        know_antenna++;
        break;

    case 3:
        // const char s_antena [] = { "ant2,005.70,011.10,002.80,065.00\r\n" };
        // const char s_name [] = { "name:Wrist Tunnel\r\n" };
        printf("\nname:Wrist Tunnel in index: %d\n", know_antenna);
        p->resistance_int = 5;
        p->resistance_dec = 70;
        p->inductance_int = 11;
        p->inductance_dec = 10;
        p->current_limit_int = 2;
        p->current_limit_dec = 80;
        p->temp_max_int = 65;
        p->temp_max_dec = 0;
        know_antenna++;
        break;

    case 4:
        // const char s_antena [] = { "ant3,003.50,019.00,003.50,065.00\r\n" };
        // const char s_name [] = { "name:Tunnel 6 inches\r\n" };
        printf("\nname:Tunnel 6 inches in index: %d\n", know_antenna);
        p->resistance_int = 3;
        p->resistance_dec = 50;
        p->inductance_int = 19;
        p->inductance_dec = 00;
        p->current_limit_int = 3;
        p->current_limit_dec = 50;
        p->temp_max_int = 65;
        p->temp_max_dec = 0;
        know_antenna++;
        break;

    case 5:
        // const char s_antena [] = { "ant4,004.00,022.60,003.50,065.00\r\n" };
        // const char s_name [] = { "name:Tunnel 8 inches\r\n" };
        printf("\nname:Tunnel 8 inches in index: %d\n", know_antenna);
        p->resistance_int = 4;
        p->resistance_dec = 00;
        p->inductance_int = 22;
        p->inductance_dec = 60;
        p->current_limit_int = 3;
        p->current_limit_dec = 50;
        p->temp_max_int = 65;
        p->temp_max_dec = 0;
        know_antenna++;
        break;

    case 6:
        // const char s_antena [] = { "ant4,002.50,021.00,003.50,065.00\r\n" };
        // const char s_name [] = { "name:Tunnel 10 inches\r\n" };
        printf("\nname:Tunnel 10 inches in index: %d\n", know_antenna);
        p->resistance_int = 2;
        p->resistance_dec = 50;
        p->inductance_int = 21;
        p->inductance_dec = 00;
        p->current_limit_int = 3;
        p->current_limit_dec = 50;
        p->temp_max_int = 65;
        p->temp_max_dec = 0;
        know_antenna++;
        break;

    case 7:
        // const char s_antena [] = { "ant4,006.60,056.50,003.50,065.00\r\n" };
        // const char s_name [] = { "name:Tunnel 10 inches\r\n" };
        printf("\nname:Tunnel 10 inches in index: %d\n", know_antenna);
        p->resistance_int = 6;
        p->resistance_dec = 60;
        p->inductance_int = 56;
        p->inductance_dec = 50;
        p->current_limit_int = 3;
        p->current_limit_dec = 50;
        p->temp_max_int = 65;
        p->temp_max_dec = 0;
        know_antenna++;
        break;

    case 8:
        // const char s_antena [] = { "ant4,010.10,128.10,003.50,065.00\r\n" };
        // const char s_name [] = { "name:Tunnel 10 inches\r\n" };
        printf("\nname:Tunnel 10 inches in index: %d\n", know_antenna);
        p->resistance_int = 10;
        p->resistance_dec = 10;
        p->inductance_int = 128;
        p->inductance_dec = 10;
        p->current_limit_int = 3;
        p->current_limit_dec = 50;
        p->temp_max_int = 65;
        p->temp_max_dec = 0;
        know_antenna++;
        break;

    case 9:
        // const char s_antena [] = { "anta,061.00,063.00,000.64,055.00\r\n" };
        // const char s_name [] = { "name:GT Googles\r\n" };
        printf("\nname:GT Googles in index: %d\n", know_antenna);
        p->resistance_int = 61;
        p->resistance_dec = 0;
        p->inductance_int = 63;
        p->inductance_dec = 0;
        p->current_limit_int = 0;
        p->current_limit_dec = 64;
        p->temp_max_int = 55;
        p->temp_max_dec = 0;
        know_antenna++;
        break;
                
    default:
        more_antennas = 0;
        break;
    }

    return more_antennas;
}


void TSP_Get_Know_Antennas_Reset (void)
{
    know_antenna = 0;
}


void TSP_Get_Know_Single_Antenna (antenna_st * p, unsigned char index)
{
    know_antenna = index;
    
    if (TSP_Get_Know_Antennas (p) == 0)
    {
        //return a default antenna
        printf("\nname:Plannar default\n");
        p->resistance_int = 11;
        p->resistance_dec = 00;
        p->inductance_int = 142;
        p->inductance_dec = 00;
        p->current_limit_int = 1;
        p->current_limit_dec = 0;
        p->temp_max_int = 65;
        p->temp_max_dec = 0;        
    }
}

//--- end of file ---//


