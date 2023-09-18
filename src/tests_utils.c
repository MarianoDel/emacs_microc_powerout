//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS_UTILS.C #########################
//---------------------------------------------

#include "tests_utils.h"

#include <stdio.h>


// Module Functions ------------------------------------------------------------
int Utils_Distance_Floats (float a, float b, int percentage)
{
    if ((percentage < 1) ||
        (percentage > 100))
    {
        printf("percentage must be between 1 and 100%%\n");
        return -1;
    }

    float perc = 0.0;
    if (a > b)
    {
        perc = a / 100;
        if (a > (b + perc))
            return 0;
        else
            return 1;
    }
    else
    {
        perc = b / 100;
        if (b > (a + perc))
            return 0;
        else
            return 1;
    }
}

//--- end of file ---//


