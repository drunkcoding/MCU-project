#include "common.h"

#ifndef __ADC_VALUE__
#define __ADC_VALUE__
#define G_STEP 248.0
#define IDEAL_MID 511.5
#define ZERO_VALUE_Y 511.5
//#define ZERO_VALUE_X (ZERO_VALUE_Y - 74.4)
#define ZERO_VALUE_X 511.5
#define ZERO_VALUE_Z (ZERO_VALUE_Y - 74.4)
static int count = 0;
static double voltage_equi[3];
static double ZERO_VALUE[3] = {IDEAL_MID, IDEAL_MID, IDEAL_MID - 74.4};
#endif

//void ADC_init();
