/*
** SDMMC.h SD card interface 
** 
** 07/20/07 v1.4 LDJ
** 11/22/07 v1.5 LDJ
**
*/

#ifndef SDMMC_H
#define SDMMC_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include "common.h"

//File address and sector definition
#define START_ADDRESS       35264        //Use physical block address
                                         //Use Winhex to find address
                                         //Start block address 16640 for 272kbps
                                         //Start block address 31844 for 128kbps
                                         
                                         
#define N_SECTORS            870        // number of sectors 
                                         // 272kbps file has 15360 sectors
                                         // 128kps file has 3465
#define SECTOR_SIZE          512         // logic block(sector) size SDHC

// Init ERROR code definitions
#define E_COMMAND_ACK       0x80
#define E_INIT_TIMEOUT      0x81

//SD card IO definitions
#define SD_SDO1 PORTDbits.RD0 //SDO1
#define SD_SCK1 PORTDbits.RD10 //SCK1
#define SD_SS1 PORTDbits.RD9 //SS1
#define SD_SDI1 PORTCbits.RC4 //SDI1


//SD reading using SPI2 module
/*#define SD_SCK2 PORTGbits.RG6
#define SD_SDI2 PORTGbits.RG7
#define SD_SDO2 PORTGbits.RG8
#define SD_SS2 PORTGbits.RG9
 */

//Timeout periods 
#define INIT_TIMEOUT       10000  
#define RX_TIMEOUT       6250
#define TX_TIMEOUT       62500

// SD card commands and responses
#define RESET           0x00 // a.k.a. GO_IDLE (CMD0)
#define SEND_IF_COND    0X08 // a.k.a  SEND_IF_COND (CMD8)
#define READ_SINGLE     0x11 // a.k.a.  (CMD17)
#define READ_MULTI      0x12 // a.k.a.    (CMD18)
#define WRITE_SINGLE    0x18 // a.k.a.  (CMD24)
#define SEND_CSD        9
#define SEND_CID        10
#define SET_BLEN        16
#define APP_CMD         0x37 //a.k.a    (CMD55) 
#define READ_OCR        0x3A //a.k.a    (CMD58)
#define SD_SEND_OP_COND 0x29 //a.k.a    (AMCD41)
#define CRC_ON_OFF      0x3B //a.k.a    (CMD59)

#define DATA_START      0xFE
#define DATA_ACCEPT     0x05

void initSD( void);     // initializes I/O pins and SPI
int initMedia( void);   // initializes the SD/MMC memory device
int readSECTOR ( unsigned int a , char * p);  // reads a block of data 
int writeSECTOR( unsigned int a, char * p);  // writes a block of data
unsigned char writeSPI( unsigned char b); //Send and receive one byte of data
unsigned char sendSDCmd( unsigned char c, unsigned int a,unsigned char *response); //send a command to SD
int SD_Rx(unsigned int addr,int numSector,char * SD_Buffer);

//int getCD();            // chech card presence
//int getWP();            // check write protection tab

#ifdef	__cplusplus
}
#endif

#endif //SDMMC_H