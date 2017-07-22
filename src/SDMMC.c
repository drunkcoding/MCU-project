/*
** SDMMC.c SD card interface 
** 
** 7/20/06 v1.4 LDJ 
** 7/14/07 v2.0 LDJ 
** 
*/

#include "sdmmc.h"

void initSD( void)
{
    //Modified
    SD_SS2 = 1;           // initially keep the SD card disabled
    TRISGCLR=0x340;       // Configure SCK2,SDO2,SS2 as output
    PORTGCLR=0x340;
    
    // init the spi module for a slow (safe) clock speed first
    SPI2CON = 0x0120;   // ON, CKE=1; CKP=0, MASTEN=1, sample middle
    SPI2BRG = 15;       // SPI baud rate 250kHz
    SPI2CONSET= 0x8000;    // SPI module on
    //Modified ended
    
}   // initSD


// send one byte of data and receive one back at the same time
unsigned char writeSPI( unsigned char b)
{
    SPI2BUF = b;                  // write to buffer for TX
    while( !SPI2STATbits.SPIRBF); // wait transfer complete
    return SPI2BUF;               // read the received value
}// writeSPI

#define readSPI()   writeSPI( 0xFF)
#define clockSPI()  writeSPI( 0xFF)
//Modified
#define disableSD() SD_SS2 = 1; clockSPI()
#define enableSD()  SD_SS2 = 0
//Modified ended


int sendSDCmd( unsigned char c, unsigned a)
// c    command code
// a    byte address of data block
{
    int i, r;

    // enable SD card
    enableSD();

    // Send a comand packet (6 bytes)
    // Send MSB first 
    writeSPI( c | 0x40);    // send command 
    writeSPI( a>>24);       // msb of the address
    writeSPI( a>>16);       
    writeSPI( a>>8);
    writeSPI( a);           // lsb
    
    writeSPI( 0x95);        // send CMD0 CRC 

    // now wait for a response, allow for up to 8 bytes delay
    for( i=0; i<50; i++) 
    {
        r = readSPI();      
        if ( r != 0xFF) 
            break;
    }
    return r;         

/* return response
    FF - timeout 
    00 - command accepted
    01 - command received, card in idle state after RESET

other codes:
    bit 0 = Idle state
    bit 1 = Erase Reset
    bit 2 = Illegal command
    bit 3 = Communication CRC error
    bit 4 = Erase sequence error
    bit 5 = Address error
    bit 6 = Parameter error
    bit 7 = Always 0
*/
    // NOTE CSCD is still low!
} // sendSDCmd


int initMedia( void)
// returns 0 if successful
//          E_COMMAND_ACK   failed to acknowledge reset command
//          E_INIT_TIMEOUT  failed to initialize
{
    int i, r;

    // 1. with the card NOT selected     
    disableSD(); 

    // 2. send 80 clock cycles start up
    for ( i=0; i<10; i++)
        clockSPI();

    // 3. now select the card
    enableSD();

    // 4. send a single RESET command
    r = sendSDCmd( RESET, 0); disableSD();
    if ( r != 1)                // must return Idle
        return E_COMMAND_ACK;   // comand rejected

    // 5. send repeatedly INIT until Idle terminates
    for (i=0; i<INIT_TIMEOUT; i++) 
    {
        r = sendSDCmd(INIT, 0); disableSD();
        if ( !r) 
            break; 
    } 
    if ( i == INIT_TIMEOUT)   
        return E_INIT_TIMEOUT;  // init timed out 

    // 6. increase speed 
    SPI2CONCLR = 0x8000;        // disable the SPI2 module
    SPI2BRG = 0;                // Baud rate changed to 4MHz, max with FPB=8MHz
    SPI2CONSET = 0x8000;           // re-enable the SPI2 module
    
    return 0;           
} // init media


int readSECTOR( unsigned int a, char *p)
// a        Logic block address of sector requested
// p        pointer to sector buffer
// returns  TRUE if successful
{
    int r, i;
        
    // 1. send READ command
    r = sendSDCmd( READ_SINGLE, ( a << 9));
    if ( r == 0)    // check if command was accepted
    {  
        // 2. wait for a response
        for( i=0; i<RX_TIMEOUT; i++)
        {
            r = readSPI();     
            if ( r == DATA_START) 
                break;
        } 

        // 3. if it did not timeout, read 512 byte of data
        if ( i != RX_TIMEOUT)
        {
            i = 512;
            do{ 
                *p++ = readSPI();
            } while (--i>0);

            // 4. ignore CRC
            readSPI();
            readSPI();

        } // data arrived

    } // command accepted

    // 5. remember to disable the card
    disableSD();

    return ( r == DATA_START);    // return TRUE if successful
} // readSECTOR


int writeSECTOR( unsigned int a, char *p)
// a        Logic block address of sector requested
// p        pointer to sector buffer
// returns  TRUE if successful
{
    unsigned r, i;

    //NO Write Protect pin available on current module 
    // 0. check Write Protect 
    /*if ( getWP())
        return FAIL;*/
    
    // 1. send WRITE command
    r = sendSDCmd( WRITE_SINGLE, ( a << 9));
    if ( r == 0)    // check if command was accepted
    {  

        // 2. send data
        writeSPI( DATA_START);

        // send 512 bytes of data
        for( i=0; i<512; i++)
            writeSPI( *p++);

        // 3. send dummy CRC
        clockSPI();

        // 4. check if data accepted
        r = readSPI(); 

        if ( (r & 0xf) == DATA_ACCEPT)
        {

            // 5. wait for write completion
            for( i=0; i<TX_TIMEOUT; i++)
            { 
                r = readSPI();
                if ( r != 0 )
                    break;
            } 

        } //Write successful
        else{
            r = 0;
		}// Write Failed

    } //Write command accepted

    // 6. disable the card
    disableSD();

    return r;      // return TRUE if successful

} // writeSECTOR

int SD_Rx(unsigned int addr,int numSector)
// addr        Logic block address of sector requested
// returns TRUE if successful
{
    char  SD_buffer[SECTOR_SIZE];

    int i;      

    // Read all sectors of the mp3   
    for( i=0; i<numSector; i++)
    {
        // read back one block at a time
        if (!readSECTOR( addr+i, SD_buffer))
        {   // reading failed
            printf("Read failure");
            return 0;
        }
    } // i

    return 1;
}// SD_Rx

// SD card connector presence detection switch
/*int getCD( void) 
// returns  TRUE card present
//          FALSE card not present
{
    return !SDCD;
}*/


// card Write Protect tab detection switch
/*int getWP( void)
// returns  TRUE write protect tab on LOCK
//          FALSE write protection tab OPEN
{
    return SDWP;
}*/
