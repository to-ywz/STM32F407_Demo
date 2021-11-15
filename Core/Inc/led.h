#ifndef DEVICE_LED_H_
#define DEVICE_LED_H_

#include "bsp_gpio.h"

#define MACROSTR(k) #k

#define LED_NUMBERS \
    X(None)         \
    X(Oneshot)      \
    X(Heartbeat)    \
    X(Timer)        \
    X(ON)           \
    X(OFF)

//单片机内部Flash操作状态
typedef enum
{
#define X(Enum) Enum,
    LED_NUMBERS
#undef X
} Led_STATE_t;


#define MACROSTR(k) #k

#define LED_ID_NUMBERS \
    X(LED1)            \
    X(LED0)

//单片机内部Flash操作状态
typedef enum
{
#define X(Enum) Enum,
    LED_ID_NUMBERS
#undef X
} Led_ID_t;

typedef struct ledObject
{
    gpio_out_t *ports;
    Led_STATE_t *state;
    uint8_t * tick;
    
    void (*setupLedMode)(Led_ID_t id, Led_STATE_t state);
    Led_STATE_t (*getLedMode)(Led_ID_t id);
} LedObject_t;

extern LedObject_t g_LedList;

void Led_Init(void);
void Led_CheckMode(void);

#endif
