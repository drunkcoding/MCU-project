#include "MP3_DMA.h"

void MP3_DMA_Init(void)
{
    //Port Initializations
    TRISFbits.TRISF12=0b0;
    TRISFbits.TRISF13=0b0;
    TRISFbits.TRISF4=0b1;
    TRISFbits.TRISF5=0b0;
    TRISAbits.TRISA3=0b0; 
    TRISDbits.TRISD6=0b1;
    TRISDbits.TRISD12=0b1;
    TRISAbits.TRISA2=0b0;
    
    //Reset the MP3 module
    //Reset is active-low, Keep it hight afterwards
    MP3_XCS=0b1;
    MP3_SS4=0b1;
    MP3_RESET=0b0;
    delay_ms(1);
    MP3_RESET=0b1;
    delay_ms(5);
    
    //SPI initialization, 4MHz baud rate, master, enhanced buffer, 16-bit
    //CKP=0, CKE=1 according to MP3 module spec
    //Data is sampled on SCK rising edge
    //TX interrupt when buffer is completely empty
    //SS is auto-toggled by SPI module
    //DMA auto-detects SPI4TX interrupt, no need to enable it 
    SPI4CONbits.ON=0b0;
    char dummy=SPI4BUF;
    SPI4BRG=106;
    SPI4STATbits.SPIROV=0b0;
    SPI4CONbits.CKP=0b0;
    SPI4CONbits.CKE=0b1;
    SPI4CONbits.MSSEN=0b1;
    SPI4CONbits.ENHBUF=0b1;
    SPI4CONbits.STXISEL=0b11;
    SPI4CONbits.SRXISEL=0b11;
    SPI4CONbits.MSTEN=0b1;
    
    //DMA initialization, channel 0
    //Start cell transfer upon SPI4 TX Done (IRQ=42)
    //Abort cell transfer upon IC5 input capture interrupt (IRQ=21)
    //No need to enable DMA abort interrupt
    //Interrupt upon block transfer complete
    //Interrupt upon address error
    //Priority 3, sub-priority 2
    //Source is SD_buffer
    //Destination is SPI4BUF
    DMACONbits.ON=0b1;
    DCH1CONbits.CHPRI=0b11;
    DCH1ECONbits.CHSIRQ=42;
    DCH1ECONbits.SIRQEN=0b1;
    DCH1ECONbits.CHAIRQ=21;
    DCH1ECONbits.AIRQEN=0b1;
    DCH1INTbits.CHBCIF=0b0;
    DCH1INTbits.CHBCIE=0b1;
    DCH1INTbits.CHERIF=0b0;
    DCH1INTbits.CHERIE=0b1;
    IFS1bits.DMA1IF=0b0;
    IPC9bits.DMA1IP=0b011;
    IPC9bits.DMA1IS=0b10;
    IEC1bits.DMA1IE=0b1;
    //DCH0SSA=((unsigned int)SD_buffer&0x1FFFFFFF);
    //DCH0DSA=(((unsigned int)&SPI4BUF)&0x1FFFFFFF);
    //DCH0SSA=KVA_TO_PA(SD_buffer);
    DCH1SSA=0x00002124;
    DCH1DSA=KVA_TO_PA(&SPI4BUF);
    DCH1SSIZ=512;
    DCH1DSIZ=1;
    DCH1CSIZ=1;
    
    //DMA initialization, channel 1
    //Start cell transfer upon SPI4 RX Done (IRQ=41)
    //Automatically enabled, even upon block transfer done
    //Source is SPI4BUF
    //Destination is dummySPI4RX;
    DCH0CONbits.CHAEN=0b1;
    DCH0CONbits.CHPRI=0b11;
    DCH0ECONbits.CHSIRQ=41;
    DCH0ECONbits.SIRQEN=0b1;
    DCH0INTbits.CHBCIF=0b0;
    DCH0INTbits.CHERIF=0b0;
    //DCH1SSA=((unsigned int)&SPI4BUF&0x1FFFFFFF);
    //DCH1DSA=(((unsigned int)&dummySPI4RX)&0x1FFFFFFF);
    DCH0SSA=KVA_TO_PA(&SPI4BUF);
    DCH0DSA=KVA_TO_PA(dummySPI4RX);
    DCH0SSIZ=16;
    DCH0DSIZ=512;
    DCH0CSIZ=16;
    DCH0CONbits.CHEN=0b1;
    
    //Input capture initialization, IC5/RD12
    //Capture on first falling edge
    //Interrupt every edge after, priority 3, sub-priority 3
    IC5CONbits.ON=0b0;
    IC5CONbits.ICM=0b110;
    IC5CONbits.FEDGE=0b0;
    IC5CONbits.ICI=0b00;
    IPC5bits.IC5IP=0b011;
    IPC5bits.IC5IS=0b11;
    IFS0bits.IC5IF=0b0;
    //IEC0bits.IC5IE=0b1;
     
    //Change notice interrupt, RD6, on-board push button
    //Priority 3, sub-priority 1
    CNCONbits.ON=0b1;
    CNENbits.CNEN15=0b1;
    CNPUEbits.CNPUE15=0b1;
    IPC6bits.CNIP=0b011;
    IPC6bits.CNIS=0b01;
    IFS1bits.CNIF=0b0;
    IEC1bits.CNIE=0b1;
    
    //MP3 module initialization and tests
    SPI4CONbits.ON=0b1;
    MP3_XCS=0b0;
    SPI4BUF=0x02;//SCI command write
    dummy=SPI4BUF;
    SPI4BUF=0x00;//MODE register at 0x00
    dummy=SPI4BUF;
    SPI4BUF=0x09;
    dummy=SPI4BUF;
    SPI4BUF=0x00;
    dummy=SPI4BUF;
    MP3_XCS=0b1;
    while(!MP3_DREQ);
    MP3_XCS=0b0;
    SPI4BUF=0x02;//SCI command write
    dummy=SPI4BUF;
    SPI4BUF=0x0B;//VOL register at 0x0B
    dummy=SPI4BUF;
    SPI4BUF=0x28; //-80dB attenuation
    dummy=SPI4BUF;
    SPI4BUF=0x28;
    dummy=SPI4BUF;
    MP3_XCS=0b1;
    while(!MP3_DREQ);
    SPI4CONbits.ON=0b0;
    IEC0bits.IC5IE=0b1;
   
    return;
}