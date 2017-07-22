/*
** SDMMC.h SD card interface 
** 
** 07/20/07 v1.4 LDJ
** 11/22/07 v1.5 LDJ
**
*/

#include <plib.h>

//File address and sector definition
#define START_ADDRESS       8448         // start address offset 0x000420000
                                         // Logic block address(??) 8448 
                                         // address offset= LBA<<9
                                         // End address offset 0x000B7DE00
                                         // Logic block address(??) 23535
#define N_SECTORS            15088       // number of sectors
#define SECTOR_SIZE          512         // logic block(sector) size as checked

// Init ERROR code definitions
#define E_COMMAND_ACK       0x80
#define E_INIT_TIMEOUT      0x81

//SD card IO definitions
//SD reading using SPI2 module
#define SD_SCK2 PORTGbits.RG6
#define SD_SDI2 PORTGbits.RG7
#define SD_SDO2 PORTGbits.RG8
#define SD_SS2 PORTGbits.RG9

//Timeout periods
#define INIT_TIMEOUT       10000   
#define RX_TIMEOUT       6250
#define TX_TIMEOUT       62500

// SD card commands and responses
#define RESET           0 // a.k.a. GO_IDLE (CMD0)
#define INIT            1 // a.k.a. SEND_OP_COND (CMD1)
#define READ_SINGLE     17
#define WRITE_SINGLE    24
#define SEND_CSD        9
#define SEND_CID        10
#define SET_BLEN        16
#define APP_CMD         55
#define SEND_APP_OP     41

#define DATA_START      0xFE
#define DATA_ACCEPT     0x05

void initSD( void);     // initializes I/O pins and SPI
int initMedia( void);   // initializes the SD/MMC memory device
int readSECTOR ( unsigned int a , char * p);  // reads a block of data 
int writeSECTOR( unsigned int a, char * p);  // writes a block of data

int SD_Rx(unsigned int addr,int numSector);

//int getCD();            // chech card presence
//int getWP();            // check write protection tab