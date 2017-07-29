#include "common.h"
#include <math.h>

#define MIN_MEASURE 4 / 1000.0
#define MU 0.6
#define GRAVITY (9.8)

static double Speedx = 0, Speedy = 0, Speedz = 0;
static int DispCnt = 0;

//void ConvertAngle(double ang_x, double ang_y, const short int acc_x, const short int acc_y);
void AccBall(double *Accx, double *Accy, double *Accz, const short int Ax, const short int Ay, const short int Az);
void SSD(int n);
void InitDispTimer();
