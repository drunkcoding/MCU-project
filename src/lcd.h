#include "common.h"

void LCD_init();
void LCD_reset();
void LCD_graylevel();
void LCD_putchar(uint8_t chr);
void LCD_putcmd(uint8_t chr);
void LCD_goto(uint8_t addr);
void disp_256x160(int x, int y, char *dp);
void disp_modify(int x, int y, char *dp);
void LCD_address(int x, int y, int x_total, int y_total);
void clear_screen(int x, int y);
