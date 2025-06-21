#define PI_INPUT 1
#define PI_OUTPUT 0
#define RISING_EDGE 2
#define FALLING_EDGE 3

#define IRQ_RXDONE 0x40

int gpioSetMode(unsigned gpio, unsigned mode);
int gpioWrite(unsigned gpio, unsigned level);
int gpioInitialise(void);

typedef void (*gpioISRFuncEx_t)(int gpio, int level, unsigned int tick, void *userdata);
typedef void (*gpioISRFunc_t)(int gpio,
                              int level,
                              unsigned int tick);

int gpioSetISRFunc(
    unsigned gpio, unsigned edge, int timeout, gpioISRFunc_t f);

int gpioSetISRFuncEx(
    unsigned gpio,
    unsigned edge,
    int timeout,
    gpioISRFuncEx_t f,
    void *userdata);