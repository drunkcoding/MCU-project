// DEVCFG3
// USERID = No Setting
#pragma config FSRSSEL = PRIORITY_7 // SRS Select (SRS Priority 7)
#pragma config FMIIEN = ON          // Ethernet RMII/MII Enable (MII Enabled)
#pragma config FETHIO = ON          // Ethernet I/O Pin Select (Default Ethernet I/O)
#pragma config FCANIO = ON          // CAN I/O Pin Select (Default CAN I/O)
#pragma config FUSBIDIO = ON        // USB USID Selection (Controlled by the USB Module)
#pragma config FVBUSONIO = ON       // USB VBUS ON Selection (Controlled by USB Module)

// DEVCFG2
#pragma config FPLLIDIV = DIV_2  // PLL Input Divider (2x Divider)
#pragma config FPLLMUL = MUL_20  // PLL Multiplier (20x Multiplier)
#pragma config UPLLIDIV = DIV_12 // USB PLL Input Divider (12x Divider)
#pragma config UPLLEN = OFF      // USB PLL Enable (Disabled and Bypassed)
#pragma config FPLLODIV = DIV_1  // System PLL Output Clock Divider (PLL Divide by 1)

// DEVCFG1
#pragma config FNOSC = FRC       // Oscillator Selection Bits (Fast RC Osc with PLL)
#pragma config FSOSCEN = ON      // Secondary Oscillator Enable (Enabled)
#pragma config IESO = ON         // Internal/External Switch Over (Enabled)
#pragma config POSCMOD = OFF     // Primary Oscillator Configuration (Primary osc disabled)
#pragma config OSCIOFNC = OFF    // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FPBDIV = DIV_1    // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/8)
#pragma config FCKSM = CSDCMD    // Clock Switching and Monitor Selection (Clock Switch Disable, FSCM Disabled)
#pragma config WDTPS = PS1048576 // Watchdog Timer Postscaler (1:1048576)
#pragma config FWDTEN = ON       // Watchdog Timer Enable (WDT Enabled)

// DEVCFG0
#pragma config DEBUG = OFF       // Background Debugger Enable (Debugger is disabled)
#pragma config ICESEL = ICS_PGx2 // ICE/ICD Comm Channel Select (ICE EMUC2/EMUD2 pins shared with PGC2/PGD2)
#pragma config PWP = OFF         // Program Flash Write Protect (Disable)
#pragma config BWP = OFF         // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF          // Code Protect (Protection Disabled)

#include "lcd.h"
#include "common.h"
//#include "SDMMC.h"
#include "game.h"
#include "adc.h"
#include "maze.h"

void __ISR(_ADC_VECTOR, IPL5SOFT) ADCHandler(void)
{
    unsigned int voltage = ADC1BUF0;
    //double voltage_equix = 0, voltage_equiy = 0, voltage_equiz = 0;
    //unsigned char str2[5];

    voltage_equi[count] = (voltage - ZERO_VALUE[count]) * 1.0 / G_STEP;

    //count = (count + 1) % 3;
    if (count == 0)
        AD1CHSbits.CH0SA = 0b0001;
    else if (count == 1)
        AD1CHSbits.CH0SA = 0b0010;
    else if (count == 2)
        AD1CHSbits.CH0SA = 0b0000;
    count++;
    IFS1bits.AD1IF = 0;
}

int main()
{
    int i = 0;
    int column = (255 - x_origin), page = y_origin, column_old, page_old;
    double AccResult[3];
    unsigned char BallDisp[48], Recover[48];
    unsigned char bmp2[5120];
    for (i = 0; i < 5120; i++)
        if (i % 2 == 0)
            bmp2[i] = 0xFF;
        else
            bmp2[i] = 0x00;
    MCU_init();
    ADC_init();
    //Enable to show maze 1
    LCD_init();
    clear_screen(0, 1);
    disp_256x160(0, 1, bmp1);
    for (i = 0; i < 48; i++)
        BallDisp[i] = ball_thin[i] | bmp1[column * 20 + page - 1 + (i % 3)];
    disp_modify(column, page, BallDisp);
    while (1)
    {
        IEC1bits.AD1IE = 0b1; // Open ADC
        while (count != 3)
            ;
        count = 0;
        IEC1bits.AD1IE = 0b0; // Close ADC
        AccBall(AccResult, voltage_equi);
        column_old = column;
        page_old = page;
        column += (int)(AccResult[1] / GRAVITY * 20.0);
        page += (int)(AccResult[0] / GRAVITY * 20.0 / 8.0);
        if (column > 255 - 16 - 2)
            column = 255 - 16 - 2;
        if (column < 3)
            column = 3;
        if (page > 20 - 2)
            page = 20 - 2;
        if (page < 1)
            page = 1;
        for (i = 0; i < 48; i++)
        {
            BallDisp[i] = (ball_thin[i]) | (bmp1[column * 20 + page - 1 + (i % 3)]);
            Recover[i] = bmp1[column_old * 20 + page_old - 1 + (i % 3)];
            //Recover[i] = 0x00;
        }
        disp_modify(column_old, page_old, Recover);
        //delay_ms(10);
        disp_modify(column, page, BallDisp);
        delay_ms(40);
    }
    return 1;
}