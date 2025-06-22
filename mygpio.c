#include "mygpio.h"
#include <stdio.h>
#include <gpiod.h>
#include <pthread.h>
#include <inttypes.h>
#include "stdint.h"

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
    gpiod_line_release(line);
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

typedef struct line_wait_args
{
    struct gpiod_line *line;
    struct timespec *ts;
    int ret;
} line_wait_args;

static void *run(void *arg)
{
    line_wait_args *args = (line_wait_args *)arg;
    printf("Waiting for event on line #%u\n", gpiod_line_offset(args->line));
    int ret = gpiod_line_event_wait(args->line, args->ts);
    if (ret < 0)
    {
        printf("Wait event notification failed\n");
        ret = -1;
    }
    else if (ret == 0)
    {
        printf("Wait event notification on line #%u timeout\n", gpiod_line_offset(args->line));
    }
    args->ret = ret;
    return NULL;
}

int gpioSetISRFuncEx(
    unsigned gpio,
    unsigned edge,
    int timeout,
    gpioISRFuncEx_t f,
    void *userdata)
{
    struct gpiod_line *line;
    int ret;
    printf("gpioSetISRFuncEx called with gpio: %u, edge: %u, timeout: %d, userdata: %p\n", gpio, edge, timeout, userdata);
    line = gpiod_chip_get_line(chip, gpio);
    if (!line)
    {
        printf("Get line failed\n");
        return -1;
    }

    if (edge == RISING_EDGE)
    {
        ret = gpiod_line_request_rising_edge_events(line, CONSUMER);
        printf("Request rising edge events on line #%u\n", gpiod_line_offset(line));
    }
    else if (edge == FALLING_EDGE)
    {
        ret = gpiod_line_request_falling_edge_events(line, CONSUMER);
        printf("Request falling edge events on line #%u\n", gpiod_line_offset(line));
    }
    else
    {
        printf("Invalid edge %u for GPIO %u\n", edge, gpio);
        gpiod_line_release(line);
        return -1;
    }
    if (ret < 0)
    {
        printf("Request event notification failed\n");
        gpiod_line_release(line);
        return -1;
    }
    //we can't be allocating this on the stack because it will be used in a thread
    //and the thread will be using it after this function returns
    line_wait_args *args = malloc(sizeof(line_wait_args));
    if (!args) {
        printf("Failed to allocate memory for line_wait_args\n");
        return -1;
    }
    args->line = line;
    struct timespec *ts = malloc(sizeof(struct timespec));
    if (!ts) {
        printf("Failed to allocate memory for timespec\n");
        free(args);
        return -1;
    }
    *ts = (struct timespec){10, 0}; // 10 seconds timeout
    args->ts = ts;
    pthread_t thread;
    pthread_create(&thread, NULL, run, (void*)args);
    pthread_join(thread, NULL);

    return 0; // Assuming success for now
}