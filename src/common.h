#ifndef __HEADERS__
#define __HEADERS__
#include <plib.h>
#endif

#ifndef __TYPES__
#define __TYPES__
//typedef unsigned char uint8_t;
//typedef unsigned long uint32_t;
#endif

#ifndef __SYS_CONSTANTS__
#define __SYS_CONSTANTS__
#define SYS_FREQ (8000000)
#define PB_CLK (SYS_FREQ / 1)
#define MS_SCALE (SYS_FREQ / 1000)
#define US_SCALE (SYS_FREQ / 1000000)
#endif

#ifndef __LCD_CONSTANTS__
#define __LCD_CONSTANTS__
#define LCD_EXT_CMD1 0x30
#define LCD_EXT_CMD2 0x31
#define LCD_EXT_CMD3 0x38
#define LCD_EXT_CMD4 0x39
#define LCD_WRITE_EN 0x5C
#define LCD_STOP_IDLE 0x94
#define LCD_AUTOREAD_SET 0xD7
#define LCD_LOW_FIST 0x0C
#define LCD_HIGH_FIST 0x08
#define LCD_ANA_SET 0x32
#define LCD_GRAY_SET 0x20
#define LCD_PAGE_ADDR_SET 0x75
#define LCD_COLM_ADDR_SET 0x15
#define LCD_DISP_ON 0xAF
#define LCD_DISP_OFF 0xAE
#define LCD_SCAN_SET 0xBC
#define LCD_DISP_CON 0xCA
#define LCD_DISP_MODE 0xF0
#define LCD_EV_CON 0x81
#define LCD_POW_CON 0x20
#define LCD_PIXEL_ON 0x23
#define LCD_ICON_ON 0x77
#define LCD_DISP_WITH 0xFF
#define LCD_DISP_HEIGHT 0x28
#endif

#ifndef __SYS_FUNC__
#define __SYS_FUNC__
void delay_ms(int i);
void delay_us(int i);
void MCU_init();
void Timer1_init();
#endif

