#include "mygpio.h"
#include <stdio.h>

int gpioSetMode(unsigned gpio, unsigned mode)
{
    printf("gpioSetMode called with gpio: %u, mode: %u\n", gpio, mode);
    // Here you would typically set the GPIO mode using a library function
    // For now, we just return success
    return 0; // Assuming success for now

}
int gpioWrite(unsigned gpio, unsigned level)
{
    printf("gpioWrite called with gpio: %u, level: %u\n", gpio, level);
    // Here you would typically write to the GPIO pin using a library function
    // For now, we just return success
    return 0; // Assuming success for now

}
int gpioInitialise(void)
{
    printf("gpioInitialise called\n");
    // Here you would typically initialize the GPIO library
    // For now, we just return success
    return 0; // Assuming success for now

}

int gpioSetISRFunc(
    unsigned gpio, unsigned edge, int timeout, gpioISRFunc_t f)
{
    printf("gpioSetISRFunc called with gpio: %u, edge: %u, timeout: %d\n", gpio, edge, timeout);
    // Here you would typically set the ISR function for the GPIO pin
    // For now, we just return success
    return 0; // Assuming success for now

}

int gpioSetISRFuncEx(
    unsigned gpio,
    unsigned edge,
    int timeout,
    gpioISRFuncEx_t f,
    void *userdata)
{
    printf("gpioSetISRFuncEx called with gpio: %u, edge: %u, timeout: %d, userdata: %p\n", gpio, edge, timeout, userdata);
    // Here you would typically set the ISR function for the GPIO pin with additional user data
    // For now, we just return success
    return 0; // Assuming success for now

}