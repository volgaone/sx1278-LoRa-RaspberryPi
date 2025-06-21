#include "mySPI.h"
#include <stdio.h>

int spiOpen(unsigned spiChan, unsigned baud, unsigned spiFlags)
{
    printf("spiOpen called with spiChan: %u, baud: %u, spiFlags: %u\n", spiChan, baud, spiFlags);
    return 0;
}

int spiClose(unsigned handle)
{
    printf("spiClose called with handle: %u\n", handle);
    return 0; // Assuming success for now
}

int spiXfer(unsigned handle, char *txBuf, char *rxBuf, unsigned count)
{
    printf("spiXfer called with handle: %u, count: %u\n", handle, count);
    return 0; // Error if buffers are NULL
}