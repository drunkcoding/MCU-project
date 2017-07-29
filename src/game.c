#include "game.h"

#pragma interrupt Timer45Handler ipl7 vector 20
void Timer45Handler(void)
{
    DispCnt++;
    IFS0bits.T5IF = 0b0;
}

#pragma interrupt Timer3Handler ipl7 vector 12
void Timer3Handler(void)
{
    static cnt = 0;
    switch (cnt)
    {
    case 0:
        PORTA = 0b0001;
        SSD(DispCnt % 10);
        break;
    case 1:
        PORTA = 0b0010;
        SSD((DispCnt / 10) % 10);
        break;
    case 2:
        PORTA = 0b0100;
        SSD((DispCnt / 100) % 10);
        break;
    case 3:
        PORTA = 0b1000;
        SSD((DispCnt / 1000) % 10);
        break;
    default:
        PORTA = 0b0001;
        break;
    }
    cnt = (cnt + 1) % 4;
    IFS0bits.T3IF = 0b0;
}

void SSD(int n) //seven-segment diaplay 1 is on, 0 is off for each segment
{
    PORTClearBits(IOPORT_B, BIT_0 | BIT_1 | BIT_2 | BIT_3 | BIT_4 | BIT_5 | BIT_6);
    if (n == 0)
        PORTSetBits(IOPORT_B, BIT_0 | BIT_1 | BIT_2 | BIT_3 | BIT_4 | BIT_5);
    if (n == 1)
        PORTSetBits(IOPORT_B, BIT_0 | BIT_1);
    if (n == 2)
        PORTSetBits(IOPORT_B, BIT_0 | BIT_1 | BIT_3 | BIT_4 | BIT_6);
    if (n == 3)
        PORTSetBits(IOPORT_B, BIT_0 | BIT_1 | BIT_2 | BIT_3 | BIT_6);
    if (n == 4)
        PORTSetBits(IOPORT_B, BIT_1 | BIT_2 | BIT_5 | BIT_6);
    if (n == 5)
        PORTSetBits(IOPORT_B, BIT_0 | BIT_2 | BIT_3 | BIT_5 | BIT_6);
    if (n == 6)
        PORTSetBits(IOPORT_B, BIT_0 | BIT_2 | BIT_3 | BIT_4 | BIT_5 | BIT_6);
    if (n == 7)
        PORTSetBits(IOPORT_B, BIT_0 | BIT_1 | BIT_2);
    if (n == 8)
        PORTSetBits(IOPORT_B, BIT_0 | BIT_1 | BIT_2 | BIT_3 | BIT_4 | BIT_5 | BIT_6);
    if (n == 9)
        PORTSetBits(IOPORT_B, BIT_0 | BIT_1 | BIT_2 | BIT_3 | BIT_5 | BIT_6);
}

void AccBall(double *Accx, double *Accy, double *Accz, const short int Ax, const short int Ay, const short int Az)
{
    double ang_x, ang_y, ang_z;
    /*Calculate angle*/
    ang_x = arctan(Ax / sqrt(Ay * Ay + Az * Az));
    ang_y = arctan(Ay / sqrt(Ax * Ax + Az * Az));
    ang_z = arctan(Az / sqrt(Ax * Ax + Ay * Ay));
    *Accx = (cos(ang_x) - MU * sin(ang_x)) * GRAVITY * Az * MIN_MEASURE;
    *Accy = (cos(ang_y) - MU * sin(ang_y)) * GRAVITY * Az * MIN_MEASURE;
    if (*Accx < 0)
        *Accx = 0;
    if (*Accy < 0)
        *Accy = 0;
    *Accz = Az / 1.0;
}

void InitDispTimer()
{
    /*Timer45 count 1 second*/
    T4CON = 0x0;
    T5CON = 0x0;
    T4CONbits.T32 = 0b1;
    TMR4 = TMR5 = 0x0;
    PR4 = PB_CLK - 1;
    IFS0bits.T5IF = 0b0;
    IPC5bits.T5IP = 0b111;
    IPC5bits.T5IS = 0b11;
    IEC0bits.T5IE = 0b1;
    T4CONbits.ON = 0b1;

    /*Timer3 count 1 milisecond to switch display*/
    T3CON = 0x0;
    TMR3 = 0x0;
    PR3 = MS_SCALE - 1;
    IFS0bits.T3IF = 0b0;
    IPC5bits.T3IP = 0b111;
    IPC5bits.T3IS = 0b01;
    IEC0bits.T3IE = 0b1;
    T3CONbits.ON = 0b1;
}
