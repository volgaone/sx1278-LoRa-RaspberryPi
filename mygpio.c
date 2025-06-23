#include "mygpio.h"
#include <stdio.h>
#include <gpiod.h>
#include <pthread.h>
#include <inttypes.h>
#include "stdint.h"
#include <sys/time.h>

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
    struct gpiod_line *line;
    int ret;
    struct timespec ts = {10, 0}; // 10 seconds timeout
    printf("gpioSetISRFunc called with gpio: %u, edge: %u, timeout: %d\n", gpio, edge, timeout);
    line = gpiod_chip_get_line(chip, gpio);
    if (!line)
    {
        printf("Get line failed\n");
        ret = -1;
    }

    ret = gpiod_line_request_rising_edge_events(line, CONSUMER);
    if (ret < 0)
    {
        printf("Request event notification failed\n");
        ret = -1;
    }

    ret = gpiod_line_event_wait(line, &ts);
    if (ret < 0)
    {
        printf("Wait event notification failed\n");
        ret = -1;
    }
    else if (ret == 0)
    {
        printf("Wait event notification on line #%u timeout\n", gpio);
    }
    else if (ret == 0)
    {
        struct gpiod_line_event event;
        ret = gpiod_line_event_read(line, &event);
        printf("Get event notification on line #%u\n", gpio);
        if (ret < 0)
        {
            printf("Read last event notification failed\n");
            ret = -1;
        }
    }

    return 0; // Assuming success for now
}

typedef struct line_wait_args
{
    struct gpiod_line *line;
    struct timespec *ts;
    int edge;
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
    else
    {
        struct gpiod_line_event event;
        ret = gpiod_line_event_read(args->line, &event);
        printf("Get event notification on line #%u\n", gpiod_line_offset(args->line));
        if (ret < 0)
        {
            printf("Read last event notification failed\n");
            ret = -1;
        }
    }
    args->ret = ret;
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
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("Current local time and date: %s\n", asctime(timeinfo));
    line_wait_args *args = (line_wait_args *)arg;
    printf("Manually waiting for event on line #%u for %u seconds\n", gpiod_line_offset(args->line), args->ts->tv_sec);
    int ret;
    if (args->edge == RISING_EDGE)
    {
        while (gpiod_line_get_value(args->line) == 0)
        {
            nanosleep(&ts_wait_step, 0); // Sleep for 1000 microseconds
            gettimeofday(&rawtime2, NULL);

            unsigned long int delta_time_us = rawtime2.tv_usec - rawtime1.tv_usec;
            unsigned long int delta_time_sec = rawtime2.tv_sec - rawtime1.tv_sec;
            if (delta_time_sec != last_seconds_value)
            {
                last_seconds_value = delta_time_sec;
                printf("Current delta time: %ld seconds\n", delta_time_sec);
            }
            // printf("Current time delta: %u %u\n", delta_time_us, delta_time_sec);
            if (args->ts->tv_sec == 0)
            {
                if (delta_time_us > args->ts->tv_nsec / 1000)
                {
                    printf("Timeout delta is %u us\n", delta_time_us);
                    timeout = true;
                    break;
                }
            }
            else if (args->ts->tv_sec > 0)
            {
                if (delta_time_sec >= args->ts->tv_sec)
                {
                    printf("Timeout delta is %u seconds\n", delta_time_sec);
                    timeout = true;
                    break;
                }
            }
        }
        if (!timeout)
            printf("Rising edge detected on line #%u\n", gpiod_line_offset(args->line));
        else
            printf("Timeout waiting for rising edge on line #%u\n", gpiod_line_offset(args->line));
    }
    else if (args->edge == FALLING_EDGE)
    {
        while (gpiod_line_get_value(args->line) == 1)
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
            printf("Falling edge detected on line #%u\n", gpiod_line_offset(args->line));
        else
            printf("Timeout waiting for falling edge on line #%u\n", gpiod_line_offset(args->line));
    }
    else
    {
        printf("Invalid edge %u for GPIO %u\n", args->edge, gpiod_line_offset(args->line));
        args->ret = -1;
        return NULL;
    }
    args->ret = ret;
    printf("THREAD FINISHED\n");
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
    struct gpiod_line *line;
    int ret;
    printf("gpioSetISRFuncEx called with gpio: %u, edge: %u, timeout: %d, userdata: %p\n", gpio, edge, timeout, userdata);
    line = gpiod_chip_get_line(chip, gpio);
    if (!line)
    {
        printf("Get line failed\n");
        return -1;
    }

    // if (edge == RISING_EDGE)
    // {
    //     ret = gpiod_line_request_rising_edge_events(line, CONSUMER);
    //     printf("Request rising edge events on line #%u\n", gpiod_line_offset(line));
    // }
    // else if (edge == FALLING_EDGE)
    // {
    //     ret = gpiod_line_request_falling_edge_events(line, CONSUMER);
    //     printf("Request falling edge events on line #%u\n", gpiod_line_offset(line));
    // }
    // else
    // {
    //     printf("Invalid edge %u for GPIO %u\n", edge, gpio);
    //     gpiod_line_release(line);
    //     return -1;
    // }
    // if (ret < 0)
    // {
    //     printf("Request event notification failed\n");
    //     gpiod_line_release(line);
    //     return -1;
    // }
    // we can't be allocating this on the stack because it will be used in a thread
    // and the thread will be using it after this function returns
    line_wait_args *args = malloc(sizeof(line_wait_args));
    if (!args)
    {
        printf("Failed to allocate memory for line_wait_args\n");
        return -1;
    }
    args->line = line;
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
    //pthread_join(thread, NULL);

    return 0; // Assuming success for now
}