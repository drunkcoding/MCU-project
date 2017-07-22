#pragma config FNOSC = FRC           // Oscillator Selection Bits (Fast RC Osc)
#pragma config FPBDIV = DIV_1          // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/1)

#include "lcd.h"
#include "common.h"
#include "SDMMC.h"

int main()
{
    
    MCU_init();
    LCD_init();
    clear_screen(1, 1);
    disp_256x160(1, 1, bmp1);
    //LCD_putcmd(LCD_EXT_CMD1);
    //LCD_putcmd(LCD_ALL_ON);
    
    /*int code=0;
    initSD();
    code=initMedia();
    if (!code)
    {
        printf("Initialization failure");   
        return 0;
    }
    else
    {
        code=SD_Rx(START_ADDRESS,N_SECTORS);
        if(!code)
        {
            printf("Read failure");
            return 0;
        }
    }*/
    while (1) {}
    return 1;
}
