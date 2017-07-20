#include "common.h"

#ifndef __DMA_FLAG__
#define __DMA_FLAG__
// some local data
volatile int DmaTxIntFlag; // flag used in interrupts, signal that DMA transfer ended
volatile int DmaRxIntFlag; // flag used in interrupts, signal that DMA transfer ended
#endif

// prototypes
int DmaDoM2Spi(void);
int DmaDoSpi2M(void);
