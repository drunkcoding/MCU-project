# MPLAB IDE generated this makefile for use with GNU make.
# Project: disp.mcp
# Date: Thu Aug 03 09:24:15 2017

AS = pic32-as.exe
CC = pic32-gcc.exe
LD = pic32-ld.exe
AR = pic32-ar.exe
HX = pic32-bin2hex.exe
RM = rm

disp.hex : disp.elf
	$(HX) "disp.elf"

disp.elf : common.o demo.o game.o lcd.o
	$(CC) -mprocessor=32MX795F512L "common.o" "demo.o" "game.o" "lcd.o" -o"disp.elf" -Wl,-L"C:\Program Files (x86)\Microchip\mplabc32\v2.02\pic32mx\lib",--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__MPLAB_DEBUGGER_PIC32MXSK=1,-Map="disp.map"

common.o : common.c common.h
	$(CC) -mprocessor=32MX795F512L -x c -c "common.c" -o"common.o" -MMD -MF"common.d" -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -g

demo.o : demo.c lcd.h common.h game.h adc.h maze.h
	$(CC) -mprocessor=32MX795F512L -x c -c "demo.c" -o"demo.o" -MMD -MF"demo.d" -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -g

game.o : game.c game.h common.h
	$(CC) -mprocessor=32MX795F512L -x c -c "game.c" -o"game.o" -MMD -MF"game.d" -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -g

lcd.o : lcd.c lcd.h common.h
	$(CC) -mprocessor=32MX795F512L -x c -c "lcd.c" -o"lcd.o" -MMD -MF"lcd.d" -D__DEBUG -D__MPLAB_DEBUGGER_PIC32MXSK=1 -g

clean : 
	$(RM) "common.o" "demo.o" "game.o" "lcd.o" "disp.elf" "disp.hex"

