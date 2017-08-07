#include "common.h"

//Max 65ms
void delay_ms(int i)
{
    OpenTimer1(T1_ON | T1_PS_1_8, i * MS_SCALE / 8 - 1);
    INTClearFlag(INT_T1);
    IFS0bits.T1IF = 0b0;
    while (!IFS0bits.T1IF)
        ;
    CloseTimer1();
}

//Max 8ms
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
    PORTSetPinsDigitalOut(IOPORT_D, BIT_0 | BIT_1 | BIT_2 | BIT_3 | BIT_4);                         // BIT_0 = E, BIT_1 = CS, BIT_2 = RESET, BIT_3 = RS, BIT_4 = 7Segment
    PORTSetPinsDigitalOut(IOPORT_E, BIT_0 | BIT_1 | BIT_2 | BIT_3 | BIT_4 | BIT_5 | BIT_6 | BIT_7); // BIT_0-BIT_7 = D0-D7
    PORTClearBits(IOPORT_D, BIT_0 | BIT_1 | BIT_2 | BIT_3 | BIT_4);
    PORTClearBits(IOPORT_E, BIT_0 | BIT_1 | BIT_2 | BIT_3 | BIT_4 | BIT_5 | BIT_6 | BIT_7);
    /*setup I/O ports to connect to the 7-segment display*/
    PORTSetPinsDigitalOut(IOPORT_C, BIT_1 | BIT_2 | BIT_3 | BIT_4);                          // D1-D4
    PORTSetPinsDigitalOut(IOPORT_A, BIT_0 | BIT_1 | BIT_4 | BIT_5 | BIT_6 | BIT_7 | BIT_10); //A-G
    PORTClearBits(IOPORT_C, BIT_1 | BIT_2 | BIT_3 | BIT_4);
    PORTClearBits(IOPORT_A, BIT_0 | BIT_1 | BIT_4 | BIT_5 | BIT_6 | BIT_7 | BIT_10);
    //Set PIN AN0/RB0, AN1/RB1, AN2/RB2
    TRISBSET = 0x0007;
    AD1PCFG = 0xfff8;
    TRISASET = 0x0600;
}

void ADC_init()
{

    //ADC module initialization
    //Select AN0 as MUXA positive input and -VREF as negative input
    AD1CHSbits.CH0NA = 0b0;
    AD1CHSbits.CH0SA = 0b0000;
    //Select converted form as unsigned integer 32-bit
    AD1CON1bits.FORM = 0b100;
    //NOT Auto-sample and Auto-convert. See ADCS and SAMC setting
    AD1CON1bits.SSRC = 0b111;
    //Select voltage reference as internal reference
    AD1CON2bits.VCFG = 0b000;
    //Select regular mode in MUX A
    AD1CON2bits.CSCNA = 0b0;
    //Interrupt at completion of every 1 conversions
    AD1CON2bits.SMPI = 0b0010;
    //One 16-word buffer
    AD1CON2bits.BUFM = 0b0;
    //Use MUXA, NO alternation between A and B
    AD1CON2bits.ALTS = 0b0;
    //Select PBCLK as ADC source
    AD1CON3bits.ADRC = 0b0;
    //Use T_AD=500T_PB, and auto-sample period 28*T_AD
    //Sample rate is exactly 500samples/sec.
    AD1CON3bits.ADCS = 0b00111001;
    AD1CON3bits.SAMC = 0b11100;

    AD1CON2bits.CSCNA = 0b1;
    AD1CSSL = 0x07; // Scan Input
    //Turn on ADC module
    AD1CON1bits.ADON = 0b1;
    //Configure ADC interrupt
    //Priority 5, sub-priority 3
    IFS1bits.AD1IF = 0b0;
    IPC6bits.AD1IP = 0b101;
    IPC6bits.AD1IS = 0b11;
    //IEC1bits.AD1IE = 0b1;
    //Initiate sampling
    AD1CON1bits.SAMP = 0b1;
    AD1CON1bits.ASAM = 0b1;
}
