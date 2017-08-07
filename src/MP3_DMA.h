/* 
 * File:   MP3_DMA.h
 * Author: Robert
 *
 * Created on July 31, 2017, 7:42 PM
 */

#ifndef MP3_DMA_H
#define	MP3_DMA_H

#ifdef	__cplusplus
extern "C" {
#endif

    
#include "common.h"
#include "SDMMC.h"
    
#define MP3_SDO4 PORTFbits.RF5
#define MP3_SDI4 PORTFbits.RF4
#define MP3_SCK4 PORTFbits.RF13
#define MP3_SS4 PORTFbits.RF12
#define MP3_DREQ PORTDbits.RD12
#define MP3_RESET PORTAbits.RA3
#define MP3_XCS PORTAbits.RA2
    
    void MP3_DMA_Init(void);
    static int sector_cnt=0;
    static char dummySPI4RX[SECTOR_SIZE];
    static char  SD_buffer[SECTOR_SIZE];



#ifdef	__cplusplus
}
#endif

#endif	/* MP3_DMA_H */

