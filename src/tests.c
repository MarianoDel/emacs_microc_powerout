//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "tests_ok.h"


#include <stdio.h>
// #include <math.h>

// Types Constants and Macros --------------------------------------------------


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
unsigned char sync_verify = 0;
unsigned short timer1_seq_cnt = 0;
unsigned short sync_cnt = 0;
unsigned short sync_capt = 0;
unsigned short sync_last_capt = 0;


// Module Functions to Test ----------------------------------------------------
void Test_Functions (void);
void Test_Sync_Freq (void);

unsigned char TIM1_SyncVerify (unsigned char * freq_int, unsigned char * freq_dec);


// Module Auxiliary Functions --------------------------------------------------


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    // Test_Functions ();

    Test_Sync_Freq ();

    return 0;
}


void Test_Functions (void)
{
    printf("tested ok!\n");
    PrintOK();
}


void Test_Sync_Freq (void)
{
    printf("Test for no sync: ");
    sync_verify = 0;

    unsigned char f_int = 0;
    unsigned char f_dec = 0;
    
    if (TIM1_SyncVerify(&f_int, &f_dec) == 0)
        PrintOK();
    else
        PrintERR();

    printf("Test for no sync int: ");
    sync_verify = 1;
    timer1_seq_cnt = 65000;
    if (TIM1_SyncVerify(&f_int, &f_dec) == 0)
        PrintOK();
    else
        PrintERR();


    sync_verify = 1;
    timer1_seq_cnt = 7200 / 29;
    sync_cnt = timer1_seq_cnt;
    sync_capt = 200;
    sync_last_capt = 900;
    
    TIM1_SyncVerify(&f_int, &f_dec);
    printf("freq: %d.%02dHz\n", f_int, f_dec);
    printf("Test for 30Hz sync: ");
    if (TIM1_SyncVerify(&f_int, &f_dec) == 1)        
        PrintOK();
    else
        PrintERR();


    sync_verify = 1;
    timer1_seq_cnt = 64999;
    sync_cnt = timer1_seq_cnt;
    sync_capt = 200;
    sync_last_capt = 900;
    
    TIM1_SyncVerify(&f_int, &f_dec);
    printf("freq: %d.%02dHz\n", f_int, f_dec);
    printf("Test for 0.3Hz sync: ");
    if (TIM1_SyncVerify(&f_int, &f_dec) == 1)        
        PrintOK();
    else
        PrintERR();
    

    sync_verify = 1;
    timer1_seq_cnt = 7200 / 200;
    sync_cnt = timer1_seq_cnt;
    sync_capt = 200;
    sync_last_capt = 100;
    
    TIM1_SyncVerify(&f_int, &f_dec);
    printf("freq: %d.%02dHz\n", f_int, f_dec);
    printf("Test for 200Hz sync: ");
    if (TIM1_SyncVerify(&f_int, &f_dec) == 1)        
        PrintOK();
    else
        PrintERR();
    

}
// Other module functions to test ----------------------------------------------
unsigned char TIM1_SyncVerify (unsigned char * freq_int, unsigned char * freq_dec)
{
    if ((!sync_verify) ||
        (timer1_seq_cnt == 65000))
        return 0;

    unsigned int calc_int = 0;
    unsigned int calc_dec = 0;
    unsigned int calc_div = 0;

    calc_div = 1000 * sync_cnt + sync_capt - sync_last_capt;

    if (calc_div == 0)
        return 0;
    
    calc_dec = 7200 * 1000 * 100;
    calc_dec = calc_dec / calc_div;
    
    calc_int = 7200 * 1000;
    calc_int = calc_int / calc_div;

    calc_dec = calc_dec - calc_int * 100;

    *freq_int = (unsigned char) calc_int;
    *freq_dec = (unsigned char) calc_dec;

    return 1;
}


//--- end of file ---//


