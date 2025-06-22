#include "mySPI.h"
#include <stdio.h>
#include <wiringPiSPI.h>

int ch = -1;

int spiOpen(unsigned spiChan, unsigned baud, unsigned spiFlags)
{
    printf("spiOpen called with spiChan: %u, baud: %u, spiFlags: %u\n", spiChan, baud, spiFlags);
    ch = spiChan;
    return wiringPiSPISetup(spiChan, baud);
}

int spiClose(unsigned handle)
{
    printf("spiClose called with handle: %u\n", handle);
    return wiringPiSPIClose(ch);
}

/* We ignore the rxBuf parameter because the received data will be over-written into the txBuf*/
int spiXfer(unsigned handle, char *txBuf, char *rxBuf, unsigned count)
{
    printf("spiXfer called with handle: %u, count: %u\n", handle, count);
    
    int ret = wiringPiSPIDataRW(ch, txBuf, count);
    memcpy(rxBuf, txBuf, count); // Copy txBuf to rxBuf for consistency
    return ret;
}