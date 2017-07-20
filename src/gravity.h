#include "common.h"

#ifndef __ADXL345_REG__
#define __ADXL345_REG__
// ADXL345 Register Definition
#define _DEVID 0x00
#define _POWER_CTL 0x2D
#define _DATA_FORMAT 0x31
#define _BW_RATE 0x2C
#define _DATAX0 0x32
#define _DATAX1 0x33
#define _DATAY0 0x34
#define _DATAY1 0x35
#define _DATAZ0 0x36
#define _DATAZ1 0x37
#define _FIFO_CTL 0x38
#define _FIFO_STATUS 0x39
#define _SPEED 0x0F // Buffer Speed - 3200Hz
#define _ACCEL_ERROR 0x02
#endif

#ifndef __I2C_ADDR__
#define __I2C_ADDR__
static I2C_7_BIT_ADDRESS WriteAddress;
static I2C_7_BIT_ADDRESS ReadAddress;
#define I2C_CLOCK_FREQ 1e5
#define ADXL_ADDRESS 0x53
#define EEPROM_I2C_BUS I2C1 // 0x1	//
//#define EEPROM_ADDRESS 0x50 // 0b1010000 Serial EEPROM address
#endif

int ADXL_Write(BYTE command, BYTE data1);
BYTE ADXL_Read(BYTE command);
void ADXL_Init(I2C_MODULE id);
int ADXL_Filter(short int *Ax, short int *Ay, short int *Az);
int RecieveByteArray(I2C_MODULE I2C_bus, int DataSz, UINT8 i2cData[]);
//void I2C_Device_Config(BYTE command);
BOOL StartTransfer(BOOL restart);
BOOL TransmitOneByte(UINT8 data);
void StopTransfer(void);
