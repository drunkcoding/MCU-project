#include "lcd.h"
#include "gravity.h"
#include "common.h"

int main()
{
    static short int Ax, Ay, Az;
    MCU_init();
    LCD_init();
    SPI_Init();
    ADXL_Init(I2C1);
    clear_screen(1, 1);
    LCD_disp(1, 1, bmp1);
    LCD_OTP_Write();
    while (1)
    {
        Accel_Read(_DATAX0, &Ax, &Ay, &Az);
    }
}

#pragma interrupt INT1Handler ipl7 vector 7
void INT1Handler(void)
{

    mINTClearIFS0(7);
}
