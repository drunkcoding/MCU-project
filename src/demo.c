
#pragma config FNOSC = FRC           // Oscillator Selection Bits (Fast RC Osc)
#pragma config FPBDIV = DIV_1          // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/1)

#include "lcd.h"
#include "common.h"
#include "SDMMC.h"
#include "maze.h"

int main()
{
    int i=0;
    MCU_init();
    
    //Enable to show maze 1
    /*LCD_init();
    clear_screen(0, 1);
    disp_256x160(0, 1, maze1);
    */
    
    
    int code=0;
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
    }
    
    while (1){}
    return 1;
}