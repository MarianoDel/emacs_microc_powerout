//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "meas.h"

// helper modules
#include "tests_ok.h"

#include <stdio.h>
#include <string.h>


// Types Constants and Macros --------------------------------------------------


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
volatile unsigned short adc_ch[2];


// Module Functions to Test ----------------------------------------------------
void Test_Meas (void);


// Module Auxiliary Functions --------------------------------------------------


// Tests Module Auxiliary or General Functions ---------------------------------


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{

    Test_Meas ();

    return 0;
}


extern unsigned char Meas_Calc_Display_Value_V3 (unsigned char gain, unsigned short meas);
void Test_Meas (void)
{
    unsigned char display = 0;
    unsigned char display1 = 0;
    unsigned char gain = 0;
    
    for (int i = 0; i < 461; i++)
    {
	display = Meas_Calc_Display_Value_V3 (0, i);
	display1 = Meas_Calc_Display_Value_V3 (gain, i);	
	printf("adc: %d display: %d gain: %d display1: %d\r\n", i,
	       display,
	       gain,
	       display1);
    }
}





// Module Mocked Functions -----------------------------------------------------
void MA16_U16Circular_Reset (void * filter)
{
}


unsigned short MA16_U16Circular (void * filter, unsigned short sample)
{
}


void Led_On (void)
{
}


void Led_Off (void)
{
}


unsigned char Led_Is_On (void)
{
    return 1;
}


void DAC_Output2 (unsigned short a)
{
}



//--- end of file ---//


