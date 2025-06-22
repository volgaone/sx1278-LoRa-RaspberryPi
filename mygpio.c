#include "mygpio.h"
#include <stdio.h>
#include <gpiod.h>

#ifndef CONSUMER
#define CONSUMER "Consumer"
#endif

struct gpiod_chip *chip;

int gpioSetMode(unsigned gpio, unsigned mode)
{
    printf("gpioSetMode called with gpio: %u, mode: %u\n", gpio, mode);
    struct gpiod_line *line;
    line = gpiod_chip_get_line(chip, gpio);
    if (!line)
    {
        printf("Get line %u failed \n", gpio);
        return -1;
    }
    if (mode == PI_INPUT)
    {
        int ret = gpiod_line_request_input(line, CONSUMER);
        if (ret < 0)
        {
            printf("Request line %u as input failed\n", gpio);
            gpiod_line_release(line);
            return -1;
        }
    }
    else if (mode == PI_OUTPUT)
    {
        int ret = gpiod_line_request_output(line, CONSUMER, 0);
        if (ret < 0)
        {
            printf("Request line %u as output failed\n", gpio);
            gpiod_line_release(line);
            return -1;
        }
    }
    else
    {
        printf("Invalid mode %u for GPIO %u\n", mode, gpio);
        gpiod_line_release(line);
        return -1;
    }

    return 0; // Assuming success for now
}
int gpioWrite(unsigned gpio, unsigned level)
{
    struct gpiod_line *line;
    line = gpiod_chip_get_line(chip, gpio);
    if (gpiod_line_set_value(line, level) < 0)
    {
        printf("Could not set value %u on line #%u\n", level, gpio);
        gpiod_line_release(line);
    }
    printf("Output %u on line #%u\n", level, gpio);
    return 0; // Assuming success for now
}
int gpioInitialise(void)
{
    char *chipname = "gpiochip0";
    chip = gpiod_chip_open_by_name(chipname);
    if (!chip)
    {
        printf("Open chip %s failed\n", chipname);
        return -1;
    }
    printf("gpioInitialise called successfully\n");
    return 0; 
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