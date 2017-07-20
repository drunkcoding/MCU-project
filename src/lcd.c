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
    LCD_putchar(0x03);            //Bias=1/11
    LCD_graylevel();
    LCD_putcmd(LCD_EXT_CMD1);      //EXT=0
    LCD_putcmd(LCD_PAGE_ADDR_SET); //Page Address setting
    LCD_putchar(0X00);             // XS=0
    LCD_putchar(0X28);             // XE=159
    LCD_putcmd(LCD_COLM_ADDR_SET); //Clumn Address setting
    LCD_putchar(0X00);             // XS=0
    LCD_putchar(0Xff);             // XE=256 ff
    LCD_putcmd(LCD_SCAN_SET);      //Data scan direction
    LCD_putchar(0x00);             //MX.MY=Normal
    //LCD_putchar(0xA6);
    LCD_putcmd(LCD_HIGH_FIST); //0x0C low first D0-D7
    LCD_putcmd(LCD_DISP_CON);  //Display Control
    LCD_putchar(0X00);         //
    LCD_putchar(0X9F);         //Duty=160
    LCD_putchar(0X20);         //Nline=off
    LCD_putcmd(LCD_DISP_MODE); //Display Mode
    LCD_putchar(0X10);         //10=Monochrome Mode,11=4Gray
    LCD_putcmd(LCD_EV_CON);    //EV control
    LCD_putchar(0x08);         //VPR[5-0]
    LCD_putchar(0x04);         //VPR[8-6]
    LCD_putcmd(LCD_POW_CON);   //Power control
    LCD_putchar(0x0B);         //D0=regulator ; D1=follower ; D3=booste, on:1 off:0
    delay_us(100);
    //LCD_putcmd(LCD_PIXEL_ON); // All pixel on
    LCD_putcmd(LCD_ICON_ON); // Enable ICON RAM
    LCD_putcmd(LCD_DISP_ON); //Display on
}

void LCD_reset()
{
    PORTWrite(IOPORT_D, 0x0); // RESET = 0
    delay_ms(200);
    PORTWrite(IOPORT_D, 0xF); // RESET = 1
    delay_ms(200);
}

void LCD_graylevel()
{
    LCD_putcmd(LCD_GRAY_SET); // Gray Level
    LCD_putchar(0x01);
    LCD_putchar(0x03);
    LCD_putchar(0x05);
    LCD_putchar(0x07);
    LCD_putchar(0x09);
    LCD_putchar(0x0b);
    LCD_putchar(0x0d);
    LCD_putchar(0x10);
    LCD_putchar(0x11);
    LCD_putchar(0x13);
    LCD_putchar(0x15);
    LCD_putchar(0x17);
    LCD_putchar(0x19);
    LCD_putchar(0x1b);
    LCD_putchar(0x1d);
    LCD_putchar(0x1f);
}

void LCD_OTP_Write()
{
    LCD_putcmd(0x30);
    // Extension Command 1
    LCD_putcmd(0xAE);
    // Display OFF
    delay_ms(50);
    // Delay 50ms
    LCD_putcmd(0x39);
    //Enable OTP
    LCD_putcmd(0XD6);
    LCD_putchar(0x10);
    LCD_putcmd(0x31);
    // Extension Command 2
    LCD_putcmd(0xF0);
    // Frame Rate = 73Hz
    LCD_putchar(0x0D);
    LCD_putchar(0x0D);
    LCD_putchar(0x0D);
    LCD_putchar(0x0D);
    LCD_putcmd(0xE4);
    // OTP Selection Control
    LCD_putchar(0x99);
    LCD_putcmd(0xE5);
    // OTP Programming Setting
    LCD_putchar(0x0F);
    LCD_putcmd(0xE0);
    // OTP WR/RD Control
    LCD_putchar(0x20);
    delay_ms(100);
    // Delay 100ms
    LCD_putcmd(0xE2);
    // OTP Write
    delay_ms(100);
    // Delay 100ms
    LCD_putcmd(0xE1);
    // OTP Control Out
    LCD_putcmd(0x30);
    // Extension Command 1
}

void LCD_putcmd(UINT8 chr)
{
    PORTWrite(IOPORT_D, 0b0101); // BIT_0 = E, BIT_1 = CS, BIT_2 = RESET, BIT_3 = RS
    PORTWrite(IOPORT_E, chr);
    PORTWrite(IOPORT_D, 0b0100);
    delay_us(100);
    PORTWrite(IOPORT_D, 0b0110);
}

void LCD_putchar(UINT8 chr)
{
    PORTWrite(IOPORT_D, 0b1101); // BIT_0 = E, BIT_1 = CS, BIT_2 = RESET, BIT_3 = RS
    PORTWrite(IOPORT_E, chr);
    PORTWrite(IOPORT_D, 0b1100);
    delay_us(100);
    PORTWrite(IOPORT_D, 0b1110);
}

void LCD_disp(int x, int y, char *dp)
{
    int i, j;
    //LCD_address(x, y, LCD_DISP_WITH, LCD_DISP_HEIGHT);
    LCD_putcmd(LCD_EXT_CMD1);
    LCD_putcmd(LCD_DISP_ON); //Display on
    for (i = 0; i < LCD_DISP_HEIGHT; i++)
    {
        for (j = 0; j < LCD_DISP_WITH; j++)
        {
            LCD_putchar(*dp);
            dp++;
        }
    }
    LCD_putcmd(LCD_EXT_CMD1);
    LCD_putcmd(LCD_DISP_ON); //Display on
    //LCD_putcmd(0xD6);
    //LCD_putcmd(0xD7);
}

void LCD_address(int x, int y, int x_total, int y_total)
{
    x = x + 83;
    y = y - 1;
    LCD_putcmd(LCD_COLM_ADDR_SET); //Set Column Address
    LCD_putchar(x);
    LCD_putchar(x + x_total - 1);
    LCD_putcmd(LCD_PAGE_ADDR_SET); //Set Page Address
    LCD_putchar(y);
    LCD_putchar(y + y_total - 1);
    LCD_putcmd(LCD_EXT_CMD1);
    LCD_putcmd(LCD_WRITE_EN);
}

void clear_screen(int x, int y)
{
    int i, j;
    //LCD_address(x, y, LCD_DISP_WITH, LCD_DISP_HEIGHT);
    LCD_putcmd(LCD_EXT_CMD1);
    LCD_putcmd(LCD_DISP_ON); //Display on
    for (i = 0; i < LCD_DISP_HEIGHT; i++)
    {
        for (j = 0; j < LCD_DISP_WITH; j++)
        {
            LCD_putchar(0x00);
        }
    }
}
