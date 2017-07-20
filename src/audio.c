#include "audio.h"

// Configuration Bit settings
// SYSCLK = 80 MHz (8MHz Crystal/ FPLLIDIV * FPLLMUL / FPLLODIV)
// PBCLK = 40 MHz
// Primary Osc w/PLL (XT+,HS+,EC+PLL)
// WDT OFF
// Other options are don't care
//
#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_2

#define SYS_FREQ (80000000L)

/*********************************************************************
 * Function:        int main(void)
 *
 * PreCondition:    None
 *
 * Input:			None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:		Examples for the usage of the DMA Peripheral Lib
 *
 * Note:            None.
 ********************************************************************/
int main(void)
{
    int res;

    // Configure the device for maximum performance but do not change the PBDIV
    // Given the options, this function will change the flash wait states, RAM
    // wait state and enable prefetch cache but will not change the PBDIV.
    // The PBDIV value is already set via the pragma FPBDIV option above..
    SYSTEMConfig(SYS_FREQ, SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);

    srand(ReadCoreTimer()); // init the random generator

    // send some data from memory to an SPI channel
    res = DmaDoM2Spi();

    // receive some data from an SPI channel and store it in the memory
    res &= DmaDoSpi2M();

    return res;
}

/*********************************************************************
 * Function:        void DmaDoM2Spi(void)
 *
 * PreCondition:    None
 *
 * Input:			None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:		Example for sending a memory buffer to an SPI channel using the DMA and SPI Peripheral Lib.
 * 					The DMA interrupts will  be enabled and will signal when the transfer is completed.
 *
 * Note:            None.
 ********************************************************************/
int DmaDoM2Spi(void)
{
    int ix;

    static unsigned char txferTxBuff[256]; // the buffer containing the data to be transmitted
                                           // less than DmaGetMaxTxferSize() bytes per transfer
    DmaChannel dmaTxChn = DMA_CHANNEL1;    // DMA channel to use for our example
                                           // NOTE: the DMA ISR setting has to match the channel number
    SpiChannel spiTxChn = SPI_CHANNEL1;    // the transmitting SPI channel to use in our example

    // fill the transmit buffer with some random data
    for (ix = 0; ix < sizeof(txferTxBuff) / sizeof(*txferTxBuff); ix++)
    {
        txferTxBuff[ix] = rand();
    }

    // open and configure the SPI channel to use: master, no frame mode, 8 bit mode.
    // won't use SS for communicating with the slave
    // we'll be using 40MHz/4=10MHz SPI clock
    SpiChnOpen(spiTxChn, SPI_OPEN_MSTEN | SPI_OPEN_SMP_END | SPI_OPEN_MODE8, 4);

    // open and configure the DMA channel.
    DmaChnOpen(dmaTxChn, DMA_CHN_PRI2, DMA_OPEN_DEFAULT);

    // set the events: we want the SPI transmit buffer empty interrupt to start our transfer
    DmaChnSetEventControl(dmaTxChn, DMA_EV_START_IRQ_EN | DMA_EV_START_IRQ(_SPI1_TX_IRQ));

    // set the transfer:
    // source is our buffer, dest is the SPI transmit buffer
    // source size is the whole buffer, destination size is one byte
    // cell size is one byte: we want one byte to be sent per each SPI TXBE event
    DmaChnSetTxfer(dmaTxChn, txferTxBuff, (void *)&SPI1BUF, sizeof(txferTxBuff), 1, 1);

    DmaChnSetEvEnableFlags(dmaTxChn, DMA_EV_BLOCK_DONE); // enable the transfer done interrupt, when all buffer transferred

    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
    INTEnableInterrupts();

    INTSetVectorPriority(INT_VECTOR_DMA(dmaTxChn), INT_PRIORITY_LEVEL_5);        // set INT controller priority
    INTSetVectorSubPriority(INT_VECTOR_DMA(dmaTxChn), INT_SUB_PRIORITY_LEVEL_3); // set INT controller sub-priority

    INTEnable(INT_SOURCE_DMA(dmaTxChn), INT_ENABLED); // enable the chn interrupt in the INT controller

    DmaTxIntFlag = 0; // clear the interrupt flag we're  waiting on

    DmaChnStartTxfer(dmaTxChn, DMA_WAIT_NOT, 0); // force the DMA transfer: the SPI TBE flag it's already been active

    // wait for the transfer to complete
    // In a real application you can do some other stuff while the DMA transfer is taking place
    while (!DmaTxIntFlag)
        ;

    // ok, we've sent the data in the buffer
    return 1;
}

// handler for the DMA channel 1 interrupt
void __ISR(_DMA1_VECTOR, IPL5SOFT) DmaHandler1(void)
{
    int evFlags; // event flags when getting the interrupt

    INTClearFlag(INT_SOURCE_DMA(DMA_CHANNEL1)); // acknowledge the INT controller, we're servicing int

    evFlags = DmaChnGetEvFlags(DMA_CHANNEL1); // get the event flags

    if (evFlags & DMA_EV_BLOCK_DONE)
    { // just a sanity check. we enabled just the DMA_EV_BLOCK_DONE transfer done interrupt
        DmaTxIntFlag = 1;
        DmaChnClrEvFlags(DMA_CHANNEL1, DMA_EV_BLOCK_DONE);
    }
}

/*********************************************************************
 * Function:        int DmaDoSpi2M(void)
 *
 * PreCondition:    None
 *
 * Input:			None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:		Example for receiving a memory buffer from an SPI channel using the DMA and SPI Peripheral Lib.
 * 					The DMA interrupts will  be enabled and will signal when the transfer is completed.
 *
 * Note:            None.
 ********************************************************************/
int DmaDoSpi2M(void)
{
    static unsigned char txferRxBuff[256]; // the buffer to store the data to be received
                                           // less than DmaGetMaxTxferSize(), the maximum buffer size per transfer
    DmaChannel dmaRxChn = DMA_CHANNEL2;    // DMA channel to use for our example
                                           // NOTE: the DMA ISR setting has to match the channel number
    SpiChannel spiRxChn = SPI_CHANNEL1;    // the receiving SPI channel to use in our example

    // open and configure the SPI channel to use: slave, no frame mode, 8 bit mode.
    // won't use SS for communicating with the master
    // SPI clock is irrelevant in slve mode
    SpiChnOpen(spiRxChn, SPI_OPEN_SLVEN | SPI_OPEN_MODE8, 4);

    // open and configure the DMA channel.
    DmaChnOpen(dmaRxChn, DMA_CHN_PRI3, DMA_OPEN_DEFAULT);

    // set the events: we want the SPI receive buffer full interrupt to start our transfer
    DmaChnSetEventControl(dmaRxChn, DMA_EV_START_IRQ_EN | DMA_EV_START_IRQ(_SPI1_RX_IRQ));

    // set the transfer:
    // source is the SPI buffer, dest is our memory buffer
    // source size is one byte, destination size is the whole buffer
    // cell size is one byte: we want one byte to be sent per each SPI RXBF event
    DmaChnSetTxfer(dmaRxChn, (void *)&SPI1BUF, txferRxBuff, 1, sizeof(txferRxBuff), 1);

    DmaChnSetEvEnableFlags(dmaRxChn, DMA_EV_BLOCK_DONE); // enable the transfer done interrupt, when all buffer transferred

    INTEnableSystemMultiVectoredInt(); // enable system wide multi vectored interrupts

    INTSetPriorityAndSubPriority(INT_SOURCE_DMA(dmaRxChn), INT_PRIORITY_LEVEL_5, INT_SUB_PRIORITY_LEVEL_3); // set INT controller priorities

    INTEnable(INT_SOURCE_DMA(dmaRxChn), INT_ENABLED); // enable the chn interrupt in the INT controller

    DmaRxIntFlag = 0; // clear the interrupt flag we're  waiting on

    DmaChnEnable(dmaRxChn); // enable the DMA channel

    // wait for the transfer to complete
    // In a real application you can do some other stuff while the DMA transfer is taking place
    while (!DmaRxIntFlag)
        ;

    // ok, we've received the data in the buffer
    return 1;
}

// handler for the DMA channel 2 interrupt
void __ISR(_DMA2_VECTOR, IPL5SOFT) DmaHandler2(void)
{
    int evFlags; // event flags when getting the interrupt

    INTClearFlag(INT_SOURCE_DMA(DMA_CHANNEL2)); // acknowledge the INT controller, we're servicing int

    evFlags = DmaChnGetEvFlags(DMA_CHANNEL2); // get the event flags

    if (evFlags & DMA_EV_BLOCK_DONE)
    { // just a sanity check. we enabled just the DMA_EV_BLOCK_DONE transfer done interrupt
        DmaRxIntFlag = 1;
        DmaChnClrEvFlags(DMA_CHANNEL2, DMA_EV_BLOCK_DONE);
    }
}
