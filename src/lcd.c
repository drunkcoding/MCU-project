#include "lcd.h"
#include "common.h"

void LCD_init()
{
    LCD_reset();
    LCD_putcmd(LCD_EXT_CMD1);
    LCD_putcmd(LCD_STOP_IDLE);
    LCD_putcmd(LCD_EXT_CMD2);
    LCD_putcmd(LCD_AUTOREAD_SET); //Autoread control
    LCD_putchar(0X9F);            //Autoread disable
    LCD_putcmd(LCD_ANA_SET);      //Analog SET
    LCD_putchar(0x00);            //OSC Frequency adjustment
    LCD_putchar(0x01);            //Frequency on booster capacitors->6KHz
    LCD_putchar(0x00);            //Bias=1/14 //Modified
    //LCD_graylevel();
    LCD_putcmd(LCD_EXT_CMD1);      //EXT=0
    LCD_putcmd(LCD_PAGE_ADDR_SET); //Page Address setting
    LCD_putchar(0X00);             // YS=0
    LCD_putchar(0X14);             // YE=28h //Modified
    LCD_putcmd(LCD_COLM_ADDR_SET); //Column Address setting
    LCD_putchar(0X00);             // XS=0
    LCD_putchar(0Xff);             // XE=256 ff
    LCD_putcmd(LCD_SCAN_SET);      //Data scan direction
    LCD_putchar(0x05);             //MV Page direction //Modified
                                   //MX.MY=Normal
                                   //LCD_putchar(0xA6) //Removed
    LCD_putcmd(LCD_LOW_TOP);  //0x0C LSB on top of each column
    LCD_putcmd(LCD_DISP_CON);  //Display Control
    LCD_putchar(0X00);         //
    LCD_putchar(0X9F);         //Duty=160
    LCD_putchar(0X00);         //default setting, frame inversion //Added
                               //LCD_putchar(0X20);  //Nline=off //Removed
    LCD_putcmd(LCD_DISP_MODE); //Display Mode
    LCD_putchar(0X10);         //10=Monochrome Mode,11=4Gray
    LCD_putcmd(LCD_EV_CON);    //EV control //Modified now at 16V
    LCD_putchar(0x28);         //VPR[5-0] 18V VPR[5-0] 0x28 VPR[8-6] 0x05
    LCD_putchar(0x05);         //VPR[8-6] 16V VPR[5-0] 0x3D, VPR[8-6] 0x04
    LCD_putcmd(LCD_POW_CON);   //Power control
    LCD_putchar(0x0B);         //D0=regulator ; D1=follower ; D3=boost, on:1 off:0
    delay_us(100);
    LCD_putcmd(LCD_DISP_ON); //Display on
}

void LCD_reset()
{
    PORTWrite(IOPORT_D, 0b0); // RESET = 0
    delay_ms(5);
    PORTWrite(IOPORT_D, 0b0110);// RS=0,RESET=1,CS=1,E=0
    delay_ms(5);
}

void LCD_graylevel()
{
    LCD_putcmd(LCD_GRAY_SET); // Gray Level
    LCD_putchar(0x00);
    LCD_putchar(0x00);
    LCD_putchar(0x00);
    LCD_putchar(0x0c);
    LCD_putchar(0x0c);
    LCD_putchar(0x0c);
    LCD_putchar(0x00);
    LCD_putchar(0x00);
    LCD_putchar(0x12);
    LCD_putchar(0x00);
    LCD_putchar(0x00);
    LCD_putchar(0x12);
    LCD_putchar(0x12);
    LCD_putchar(0x12);
    LCD_putchar(0x00);
    LCD_putchar(0x00);
}

void LCD_putcmd(uint8_t chr)
{
    PORTClearBits(IOPORT_D,BIT_3 | BIT_1); // RS=0,CS=0
    PORTSetBits(IOPORT_D,BIT_0);//E=1
    delay_us(1);
    PORTWrite(IOPORT_E, chr);
    delay_us(1);
    PORTClearBits(IOPORT_D,BIT_0); //E=0;
    delay_us(1);
    PORTSetBits(IOPORT_D,BIT_1); //CS=1
    delay_us(1);
}

void LCD_putchar(uint8_t chr)
{
    PORTSetBits(IOPORT_D,BIT_3 | BIT_0); //RS=1,E=1
    PORTClearBits(IOPORT_D,BIT_1); //CS=0
    delay_us(1);
    PORTWrite(IOPORT_E, chr);
    delay_us(1);
    PORTClearBits(IOPORT_D,BIT_0); //E=0
    delay_us(1);
    PORTSetBits(IOPORT_D,BIT_1); //CS=1
    delay_us(1);
}

void LCD_address(int x, int y, int x_total, int y_total)
{
    LCD_putcmd(LCD_COLM_ADDR_SET); //Set Column Address
    LCD_putchar(x);
    LCD_putchar(x + x_total - 1);
    LCD_putcmd(LCD_PAGE_ADDR_SET); //Set Page Address
    LCD_putchar(y);
    LCD_putchar(y + y_total-1);
    LCD_putcmd(LCD_EXT_CMD1);
    LCD_putcmd(LCD_WRITE_EN);
}

void clear_screen(int x, int y)
{
    int i, j;
    LCD_address(x, y, 256, 20);
    for (i = 0; i < 20; i++)
    {
        for (j = 0; j < 256; j++)
        {
            LCD_putchar(0x00);
        }
    }
}

void disp_256x160(int x, int y, char *dp)
{
    int i, j;
    LCD_address(x, y, 256, 20);
    for (i = 0; i < 20; i++)
    {
        for (j = 0; j < 256; j++)
        {
            LCD_putchar(*dp);
            dp++;
        }
    }
}

void disp_modify(int x, int y)
{
    int i,j;
    LCD_address(x,y,10,20);
    for (i=0;i<2;i++)
    {
        for (j=0;j<30;j++)
        {
            LCD_putchar(0x00);
        }
    }
}