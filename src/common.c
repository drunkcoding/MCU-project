#include "common.h"

void delay_ms(int i)
{
    OpenTimer1(T1_ON | T1_PS_1_64, i * MS_SCALE / 64 - 1);
    INTClearFlag(INT_T1);
    IFS0bits.T1IF = 0b0;
    while (!IFS0bits.T1IF)
        ;
    CloseTimer1();
}
void delay_us(int i)
{
    OpenTimer1(T1_ON, i * US_SCALE - 1);
    INTClearFlag(INT_T1);
    while (!IFS0bits.T1IF)
        ;
    CloseTimer1();
}

void MCU_init()
{
    INTEnableSystemMultiVectoredInt();
    /* setup I/O ports to connect to the LCD module */
    PORTSetPinsDigitalOut(IOPORT_D, BIT_0 | BIT_1 | BIT_2 | BIT_3);                                 // BIT_0 = E, BIT_1 = CS, BIT_2 = RESET, BIT_3 = RS
    PORTSetPinsDigitalOut(IOPORT_E, BIT_0 | BIT_1 | BIT_2 | BIT_3 | BIT_4 | BIT_5 | BIT_6 | BIT_7); // BIT_0-BIT_7 = D0-D7
    PORTClearBits(IOPORT_D, BIT_0 | BIT_1 | BIT_2 | BIT_3);
    PORTClearBits(IOPORT_E, BIT_0 | BIT_1 | BIT_2 | BIT_3 | BIT_4 | BIT_5 | BIT_6 | BIT_7);
}
