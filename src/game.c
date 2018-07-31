#include "game.h"

#pragma interrupt Timer45Handler ipl6 vector 20
void Timer45Handler(void)
{
    DispCnt++;
    IFS0bits.T5IF = 0b0;
}

#pragma interrupt Timer3Handler ipl6 vector 12
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

void AccBall(double AccResult[], double AccOrigin[])
{
    double ang_x, ang_y, ang_z, Accx, Accy, Accz;
    double Ax = AccOrigin[0], Ay = AccOrigin[1], Az = AccOrigin[2];
    /*Calculate angle*/
    ang_x = atan(Ax / sqrt(Ay * Ay + Az * Az));
    ang_y = atan(Ay / sqrt(Ax * Ax + Az * Az));
    ang_z = atan(sqrt(Ax * Ax + Ay * Ay) / Az);
    Accx = (fabs(sin(ang_x)) - MU * cos(ang_x)) * GRAVITY;
    Accy = (fabs(sin(ang_y)) - MU * cos(ang_y)) * GRAVITY;
    if (Accx < 0)
        Accx = 0;
    if (Accy < 0)
        Accy = 0;
    //Accz = Az / 1.0;
    AccResult[0] = Accx;
    AccResult[1] = Accy;
    if (ang_x < 0)
        AccResult[0] = -Accx;
    if (ang_y > 0)
        AccResult[1] = -Accy;
    //AccResult[2] = Accz;
}

void InitDispTimer()
{
    /*Timer45 count 1 second*/
    T4CON = 0x0;
    T5CON = 0x0;
    T4CONbits.T32 = 0b1;
    TMR4 = TMR5 = 0x0;
    PR4 = SYS_FREQ - 1;
    IFS0bits.T5IF = 0b0;
    IPC5bits.T5IP = 0b110;
    IPC5bits.T5IS = 0b11;
    IEC0bits.T5IE = 0b1;
    T4CONbits.ON = 0b1;

    /*Timer3 count 1 milisecond to switch display*/
    T3CON = 0x0;
    TMR3 = 0x0;
    PR3 = MS_SCALE - 1;
    IFS0bits.T3IF = 0b0;
    IPC3bits.T3IP = 0b110;
    IPC3bits.T3IS = 0b01;
    IEC0bits.T3IE = 0b1;
    T3CONbits.ON = 0b1;
}
