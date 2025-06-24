/*
-- Installing: /usr/local/lib/libgpiolib.a
-- Installing: /usr/local/include/gpiolib.h
*/
#include "mygpio.h"
#include <gpiolib.h>
#include <stdio.h>
#include <pthread.h>
#include <inttypes.h>
#include "stdint.h"
#include <sys/time.h>
#include <stdlib.h>
#include <stdbool.h>

static void verbose_callback(const char *msg)
{
    printf("%s", msg);
}

int gpioSetMode(unsigned gpio, unsigned mode)
{
    printf("gpioSetMode called with gpio: %u, mode: %u\n", gpio, mode);
    if (!gpio_num_is_valid(gpio))
    {
        printf("Invalid GPIO number: %u\n", gpio);
        return -1;
    }
    printf("Setting GPIO %u to mode %u\n", gpio, mode);
    if (mode == PI_INPUT)
        gpio_set_fsel(gpio, GPIO_FSEL_INPUT);
    else if (mode == PI_OUTPUT)
    {
        gpio_set_fsel(gpio, GPIO_FSEL_OUTPUT);
        gpio_set_drive(gpio,  DRIVE_HIGH); // Default to low drive
    }
    else
    {
        printf("Invalid mode: %u\n", mode);
        return -1;
    }
    printf("GPIO %u set to mode %u successfully\n", gpio, mode);
    return 0; // Assuming success for now
}
int gpioWrite(unsigned gpio, unsigned level)
{
    printf("gpioWrite called with gpio: %u, level: %u\n", gpio, level);
    if (!gpio_num_is_valid(gpio))
    {
        printf("Invalid GPIO number: %u\n", gpio);
        return -1;
    }
    if (level)
        gpio_set(gpio);
    else
        gpio_clear(gpio);
}
int gpioInitialise(void)
{
    gpiolib_set_verbose(&verbose_callback);
    int ret = gpiolib_init();
    if (ret < 0)
    {
        printf("Failed to initialise gpiolib - %d\n", ret);
        return -1;
    }
    printf("gpiolib initialised successfully with %d\n", ret);
}

int gpioSetISRFunc(
    unsigned gpio, unsigned edge, int timeout, gpioISRFunc_t f)
{
    struct gpiod_line *line;
    int ret;
    struct timespec ts = {10, 0}; // 10 seconds timeout
    printf("gpioSetISRFunc called with gpio: %u, edge: %u, timeout: %d\n", gpio, edge, timeout);

    return 0; // Assuming success for now
}

typedef struct line_wait_args
{
    unsigned gpio;
    struct timespec *ts;
    int edge;
    int ret;
} line_wait_args;

static void *run(void *arg)
{
    line_wait_args *args = (line_wait_args *)arg;
    args->ret = 0;
    return NULL;
}

static void *run_manual_line_read(void *arg)
{
    time_t rawtime;
    struct timeval rawtime1, rawtime2;
    struct tm *timeinfo;
    bool timeout = false;
    struct timespec ts_wait_step = {
        .tv_sec = 0,
        .tv_nsec = 1000};
    unsigned long int last_seconds_value;

    gettimeofday(&rawtime1, NULL);
    line_wait_args *args = (line_wait_args *)arg;
    // printf("Manually waiting for event on line #%u for %u seconds\n", gpiod_line_offset(args->line), args->ts->tv_sec);
    int ret;
    if (args->edge == RISING_EDGE)
    {
        while (gpio_get_level(args->gpio) == 0)
        {
            nanosleep(&ts_wait_step, 0); // Sleep for 1000 microseconds
            gettimeofday(&rawtime2, NULL);

            unsigned long int delta_time_us = rawtime2.tv_usec - rawtime1.tv_usec;
            unsigned long int delta_time_sec = rawtime2.tv_sec - rawtime1.tv_sec;
            if (delta_time_sec != last_seconds_value)
            {
                last_seconds_value = delta_time_sec;
            }
            // printf("Current time delta: %u %u\n", delta_time_us, delta_time_sec);
            if (args->ts->tv_sec == 0)
            {
                if (delta_time_us > args->ts->tv_nsec / 1000)
                {
                    timeout = true;
                    break;
                }
            }
            else if (args->ts->tv_sec > 0)
            {
                if (delta_time_sec >= args->ts->tv_sec)
                {
                    timeout = true;
                    break;
                }
            }
        }
        if (!timeout)
            printf("Rising edge detected on line #%u\n", args->gpio);
        else
            printf("Timeout waiting for rising edge on line #%u\n", args->gpio);
    }
    else if (args->edge == FALLING_EDGE)
    {
        while (gpio_get_level(args->gpio) == 1)
        {
            gettimeofday(&rawtime2, 0);
            uint64_t delta_time = rawtime2.tv_usec - rawtime1.tv_usec;
            if (delta_time > args->ts->tv_sec * 1000000 + args->ts->tv_nsec / 1000)
            {
                timeout = true;
                break;
            }
        }
        if (!timeout)
            printf("Falling edge detected on line #%u\n", args->gpio);
        else
            printf("Timeout waiting for falling edge on line #%u\n", args->gpio);
    }
    else
    {
        printf("Invalid edge %u for GPIO %u\n", args->edge, args->gpio);
        args->ret = -1;
        return NULL;
    }
    args->ret = ret;
    return NULL;
}

int gpioSetISRFuncEx(
    unsigned gpio,
    unsigned edge,
    int timeout,
    gpioISRFuncEx_t f,
    void *userdata,
    pthread_t *thread)
{
    int ret;
    printf("gpioSetISRFuncEx called with gpio: %u, edge: %u, timeout: %d, userdata: %p\n", gpio, edge, timeout, userdata);
    // we can't be allocating this on the stack because it will be used in a thread
    // and the thread will be using it after this function returns
    line_wait_args *args = malloc(sizeof(line_wait_args));
    if (!args)
    {
        printf("Failed to allocate memory for line_wait_args\n");
        return -1;
    }
    args->gpio = gpio;
    struct timespec *ts = malloc(sizeof(struct timespec));
    if (!ts)
    {
        printf("Failed to allocate memory for timespec\n");
        free(args);
        return -1;
    }
    *ts = (struct timespec){5, 0}; // 10 seconds timeout
    args->ts = ts;
    args->edge = edge;

    pthread_create(thread, NULL, run_manual_line_read, (void *)args);

    return 0; // Assuming success for now
}