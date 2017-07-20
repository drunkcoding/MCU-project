#include "common.h"
#include "gravity.h"
// http://www.electro-tech-online.com/threads/adxl345-accelerometer-and-18f4550-in-hi-tech-c.140017/
int RecieveByteArray(I2C_MODULE I2C_bus, int DataSz, BYTE i2cData[])
{
    I2C_RESULT result;
    BOOL Success = TRUE;
    int count = 0;
    while (Success && (count < DataSz))
    {
        result = I2CReceiverEnable(I2C_bus, TRUE);
        if (result == I2C_RECEIVE_OVERFLOW)
        {
            DBPRINTF("Error: I2C Receive Overflow\n");
            Success = FALSE;
            return (0);
        }
        else
        { // Wait for receive to complete.
            while (Success != I2CReceivedDataIsAvailable(I2C_bus))
                ;
            // Respond with NAK when reading last byte.
            if (count == DataSz - 1)
                I2CAcknowledgeByte(I2C_bus, FALSE);
            else
                I2CAcknowledgeByte(I2C_bus, TRUE);
            i2cData[count] = I2CGetByte(I2C_bus);
            count += 1;
            Success = FALSE;
            do // Wait until acknowledge has been sent.
            {
                Success = I2CAcknowledgeHasCompleted(I2C_bus);
            } while (!Success);
        }
    }
    return count;
}

int ADXL_Write(BYTE command, BYTE data1)
{
    StartTransfer(FALSE);
    TransmitOneByte(WriteAddress.byte);
    TransmitOneByte(0x0);
    TransmitOneByte(command);
    TransmitOneByte(data1);
    StopTransfer();
    return 1;
}

BYTE ADXL_Read(BYTE command)
{
    BYTE tmp, num;
    StartTransfer(FALSE);
    TransmitOneByte(WriteAddress.byte);
    TransmitOneByte(0x0);
    TransmitOneByte(command);
    StartTransfer(TRUE);
    TransmitOneByte(ReadAddress.byte);
    num = RecieveByteArray(I2C1, 1, &tmp); // Read with NAK after last byte.
    StopTransfer();
    return tmp;
}

int Accel_Read(BYTE command, short int *Ax, short int *Ay, short int *Az)
{
    BOOL Success = TRUE;
    UINT8 i2cData[10];
    int count;

    // Check that bus is not occupied.
    while (Success != I2CBusIsIdle(I2C1))
        ;

    StartTransfer(FALSE);
    TransmitOneByte(WriteAddress.byte);
    TransmitOneByte(0x0);
    TransmitOneByte(command);
    StartTransfer(TRUE);
    TransmitOneByte(ReadAddress.byte);
    count = RecieveByteArray(I2C1, 6, i2cData);
    StopTransfer();

    if (count)
    {
        *Ax = i2cData[1] << 8;
        *Ax = *Ax | i2cData[0]; // low_byte;
        *Ay = i2cData[3] << 8;
        *Ay = *Ay | i2cData[2]; // low_byte;
        *Az = i2cData[5] << 8;
        *Az = *Az | i2cData[4]; // low_byte;
    }
    return count;
}

void ADXL_Init(I2C_MODULE id)
{
    static int dataSz;

    I2CSetFrequency(id, PB_CLK, I2C_CLOCK_FREQ); // Set the I2C baudrate

    I2CEnable(id, TRUE);                                             // Enable the I2C bus
    I2C_FORMAT_7_BIT_ADDRESS(WriteAddress, ADXL_ADDRESS, I2C_WRITE); // Initialize the Write Address
    I2C_FORMAT_7_BIT_ADDRESS(ReadAddress, ADXL_ADDRESS, I2C_READ);   // Initialize Address with the READ bit set

    // Accelerometer setup.
    dataSz = ADXL_Write(_DATA_FORMAT, 0x08); // Full resolution, +/-2g, 4mg/LSB, right justified
                                             //    dataSz = ADXL_Write(_BW_RATE, 0x0A); 	// Set 100 Hz data rate
                                             //    dataSz = ADXL_Write(_BW_RATE, 0x07); 	// Set 12 Hz data rate
    dataSz = ADXL_Write(_BW_RATE, 0x17);     // Set 12 Hz data rate, Low power mode
    dataSz = ADXL_Write(_FIFO_CTL, 0x80);    // stream mode
    dataSz = ADXL_Write(_POWER_CTL, 0x08);   // POWER_CTL reg: measurement mode

    if (dataSz == 0)
    {
        WriteAddress.byte = 0;
        ReadAddress.byte = 0;
    }
    return;
}

/******************************************************************************
 * Filter subroutine for Analog Devices ADXL345 Accelerometer device
 *
 * Overview:
 * This function keep a 16 point floating average.
 * When the Accelerometer have several measurements available, 
 * all available values are Read before the function return.
 * If the bus is running at 100 kHz, this may take up to 32 milliseconds.
 * 
 * Arguments:
 *		Ax, Ay, Az - Pointers to memory variables to recieve output.
 *					 If the accelerometer has no measurements available,
 *					 the values are not altered.
 *****************************************************************************/
int ADXL_Filter(short int *Ax, short int *Ay, short int *Az)
{
    unsigned char Entries = 0;
    int DataSz = 0, count;
    static int I = 0;
    //	static short int	Ax, Ay, Az, Tp;
    static short int Cx[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    static short int Cy[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    static short int Cz[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    static long int Sx = 0, Sy = 0, Sz = 0;
    //	int					x1, y1, x2, y2, height;
    char text[40];

    Entries = 0;
    Entries = ADXL_Read(_FIFO_STATUS);
    count = Entries & 0x003F;
    while (count > 1)
    {
        DataSz = Accel_Read(_DATAX0, Ax, Ay, Az);
        count -= 1;
        if (DataSz == 0)
        {
            snprintf(text, 40, "No Data, Entries: %i DataSz: %i", Entries, DataSz);
            //DebugMsg(text, -1); // y1 += height;
        }
        else // Filter 16 point flating average.
        {
            Sx = Sx - Cx[I];
            Sx = Sx + *Ax;
            Cx[I] = *Ax;
            Sy = Sy - Cy[I];
            Sy = Sy + *Ay;
            Cy[I] = *Ay;
            Sz = Sz - Cz[I];
            Sz = Sz + *Az;
            Cz[I] = *Az;
            I++;
            if (I == 16)
                I = 0;
        }
    }
    if ((Entries & 0x003F) > 0)
    {
        *Ax = Sx >> 4;
        *Ay = Sy >> 4;
        *Az = Sz >> 4;
    }

    return Entries;
}

BOOL StartTransfer(BOOL restart)
{
    I2C_STATUS status;

    // Send the Start (or Restart) signal
    if (restart)
    {
        I2CRepeatStart(EEPROM_I2C_BUS);
    }
    else
    {
        // Wait for the bus to be idle, then start the transfer
        while (!I2CBusIsIdle(EEPROM_I2C_BUS))
            ;

        if (I2CStart(EEPROM_I2C_BUS) != I2C_SUCCESS)
        {
            DBPRINTF("Error: Bus collision during transfer Start\n");
            return FALSE;
        }
    }

    // Wait for the signal to complete
    do
    {
        status = I2CGetStatus(EEPROM_I2C_BUS);

    } while (!(status & I2C_START));

    return TRUE;
}

BOOL TransmitOneByte(UINT8 data)
{
    // Wait for the transmitter to be ready
    while (!I2CTransmitterIsReady(EEPROM_I2C_BUS))
        ;

    // Transmit the byte
    if (I2CSendByte(EEPROM_I2C_BUS, data) == I2C_MASTER_BUS_COLLISION)
    {
        DBPRINTF("Error: I2C Master Bus Collision\n");
        return FALSE;
    }

    // Wait for the transmission to finish
    while (!I2CTransmissionHasCompleted(EEPROM_I2C_BUS))
        ;
    while (!I2CByteWasAcknowledged(EEPROM_I2C_BUS))
        ;
    return TRUE;
}

void StopTransfer(void)
{
    I2C_STATUS status;

    // Send the Stop signal
    I2CStop(EEPROM_I2C_BUS);

    // Wait for the signal to complete
    do
    {
        status = I2CGetStatus(EEPROM_I2C_BUS);

    } while (!(status & I2C_STOP));
}
