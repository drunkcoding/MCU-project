/*
** SDMMC.c SD card interface 
** 
** 7/20/06 v1.4 LDJ 
** 7/14/07 v2.0 LDJ 
** 
*/

#include "SDMMC.h"

void initSD( void)
{
    //Modified
    SPI1CONCLR=0x8000;
    SD_SS1 = 1;           // initially keep the SD card disabled
    TRISDCLR=0x601;       // Configure SCK1,SDO1,SS1 as output
    PORTDCLR=0x601;
    int dummyData=SPI1BUF; //Clear the buffer
    SPI1STATCLR=0x40;    //Clear any overflow flag
    
    // init the spi module for a slow (safe) clock speed first
    SPI1CON = 0x0120;   // ON,CKE=1. CKP=0, MASTEN=1, sample middle
    SPI1BRG = 15;       // SPI baud rate 250kHz at FPB=8MHz
    //SPI1BRG=255;          // SPI baud rate 156kHz at FPB=80MHz
    SPI1CONSET= 0x8000;    // SPI module on
    //Modified ended
    
}   // initSD


// send one byte of data and receive one back at the same time
unsigned char writeSPI( unsigned char b)
{
    SPI1BUF = b;                  // write to buffer for TX

    while( !SPI1STATbits.SPIRBF); // wait transfer complete
    unsigned char r1=SPI1BUF;
    return r1;               // read the received value
}// writeSPI

#define readSPI()   writeSPI( 0xFF)
#define clockSPI()  writeSPI( 0xFF)
//Modified
#define disableSD() SD_SS1 = 1
#define enableSD()  SD_SS1 = 0
//Modified ended


unsigned char sendSDCmd( unsigned char c, unsigned int a, unsigned char * response)
// c    command code
// a    byte address of data block
//      or command argument excluding 2-byte CRC+end bit
{
    int i;
    unsigned char r;

    // enable SD card
    enableSD();
        
    // Send a comand packet (6 bytes)
    // Send MSB first 
    writeSPI( c | 0x40);    // send command 
    writeSPI( a>>24);       // msb of the address
    writeSPI( a>>16);       
    writeSPI( a>>8);
    writeSPI( a);           // lsb
    
    //Send CRC
    if(c==RESET) writeSPI( 0x95);  
    else if(c==SEND_IF_COND) writeSPI(0x35);
    else if (c==CRC_ON_OFF) writeSPI(0x91);
    else if (c==APP_CMD) writeSPI(0x65);
    else if (c==SD_SEND_OP_COND) writeSPI(0x77);
    else if (c==READ_OCR) writeSPI(0x7A);
    else if (c==READ_SINGLE) writeSPI(0xE3);
    else writeSPI(0x7F);

    // now wait for a response, allow for up to 8 bytes delay
    for( i=0; i<8; i++) 
    {
        r = readSPI();      
        if ( r != 0xFF) 
            break;
    }
    
    //Read extra 4 bytes in response R7 and R3 for CMD8 and CMD58 respectively
    if(c==SEND_IF_COND||c==READ_OCR)
    {
        for (i=3;i>=0;i--)
        {
            *(response+i)=readSPI();
        }
    }
    
    disableSD();clockSPI();// disable SD card
    
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
// returns 1 if successful
// otherwise,
//      E_COMMAND_ACK   failed to acknowledge reset command
//      E_INIT_TIMEOUT  failed to initialize
{
    int i;
    unsigned char r;
    unsigned char response[4];

    //with the card NOT selected     
    disableSD(); clockSPI();

    //send 80 clock cycles start up
    for ( i=0; i<20; i++)
        clockSPI();

    //send a single RESET command
    r = sendSDCmd( RESET, 0, response); 
     if ( r != 1)    // must return Idle
        return 0;   // Reset failed

    //send interface condition command
    //Voltage range 2.7~3.6V
    //Check pattern 0x77
    r = sendSDCmd(SEND_IF_COND,0x00000177,response);
    
    //Check CMD8 response
    if (r==0x09) return 0;
    else if (r==0x01) //Spec 2.0 card
    {
        if(*(response+1)!=1||*(response)!=0x77)
        {
            return 0;
        }
    }
    else if (r==0x04){}//Spec 1.0 card
    else return 0;
   
    //send repeatedly INIT until Idle terminates
    //CMD55+ACMD41
    for (i=0; i<INIT_TIMEOUT; i++) 
    {
        r=sendSDCmd(APP_CMD,0,response);
        if(r==1)
        {
            //Activate SDHC high-capacity card
            r=sendSDCmd(SD_SEND_OP_COND,0x40000000,response); 
            if (!r) 
            break; 
        }
    } 
    if ( i == INIT_TIMEOUT)   
        return 0;  // init timed out 
    
    //read CCS to confirm high-capacity is activated
    //CMD58
    //r=sendSDCmd(READ_OCR,0,response);
    for(i=0;i<INIT_TIMEOUT;i++)
    {
        r=sendSDCmd(READ_OCR,0,response);
            if(r==0)
            {
                if(*(response+3)==0xc0) break;
            }
    }
    if(i==INIT_TIMEOUT)
    {
        return 0;
    }

    //increase speed 
    SPI1CONCLR = 0x8000;        // disable the SPI1 module
    SPI1BRG = 0;                // Baud rate changed to PBLCK/2, maximum
                                // SD possibly accepts a maximum of 10MHz
    SPI1CONSET = 0x8000;           // re-enable the SPI1 module
    
    
    return 1;           
} // init media


int readSECTOR( unsigned int a, char *p)
// a       Physical block address of sector requested
// p        pointer to sector buffer
// returns  TRUE if successful
{
    int r, i;
    unsigned char response[4];
        
    // send READ command
    r = sendSDCmd( READ_SINGLE, a, response);
    if ( r == 0)    // check if command was accepted
    {  
        enableSD();
        //wait for the start block token
        for( i=0; i<RX_TIMEOUT; i++)
        {
            r = readSPI();  
            if ( r == DATA_START) 
                break;
        } 

        //if it did not timeout, read 512 byte of data
        if ( i != RX_TIMEOUT)
        {
            i = 512;
            do{ 
                *p++ = readSPI();
            } while (--i>0);

            // Read two CRC
            readSPI();
            readSPI();

        } // data arrived
    } // command accepted
    else //CRC error 
    {
        if(r==0x01)
        {
            return 0;
        }
    }

    //remember to disable the card
    disableSD();clockSPI();

    return ( r == DATA_START);    // return TRUE if successful
} // readSECTOR


int writeSECTOR( unsigned int a, char *p)
// a        Physical block address of sector requested
// p        pointer to sector buffer
// returns  TRUE if successful
{
    unsigned r, i;
    unsigned char response[4];
   
    
    //NO Write Protect pin available on current module 
    // 0. check Write Protect 
    /*if ( getWP())
        return FAIL;*/
    
    // 1. send WRITE command
    r = sendSDCmd( WRITE_SINGLE, a, response);
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
    disableSD();clockSPI();

    return r;      // return TRUE if successful

} // writeSECTOR

//This function is not useful because PIC32MX795F512L has only 128K data RAM. 
//Therefore, it is impossible to read back the whole song at one time.
int SD_Rx(unsigned int addr,int numSector,char * SD_Buffer)
// addr    Physical block address of sector requested
// returns TRUE if successful
{
    //char  SD_buffer[SECTOR_SIZE];

    int i,r;      

    // Read all sectors of the mp3   
    for( i=0; i<numSector; i++)
    {
        // read back one block at a time
        r=readSECTOR( addr+i, SD_Buffer);
        if (!r)
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
