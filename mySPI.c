#include "mySPI.h"
#include <stdio.h>
#include <wiringPiSPI.h>
#include <string.h>

int ch = -1;

int spiOpen(unsigned spiChan, unsigned baud, unsigned spiFlags)
{
    ch = spiChan;
    return wiringPiSPISetup(spiChan, baud);
}

int spiClose(unsigned handle)
{
    return wiringPiSPIClose(ch);
}

int spiXfer(unsigned handle, char *txBuf, char *rxBuf, unsigned count)
{
    int ret = wiringPiSPIDataRW(ch, txBuf, count);
    memcpy(rxBuf, txBuf, count); // Copy txBuf to rxBuf for consistency
    return ret;
}