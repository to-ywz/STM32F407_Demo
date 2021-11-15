#include "led.h"
#include "bsp_systick.h"

// 文件变量
extern gpio_out_t g_gpio_out_list[];

// 全局变量
LedObject_t g_LedList;

// 静态全局
static Led_STATE_t stateArray[LED_NUM];
static uint8_t tickArray[LED_NUM];

// 本地函数
// LED 基本操作
static void Led_turnOn(uint8_t id);
static void Led_turnOff(uint8_t id);
static void Led_Toggle(uint8_t id);

// LED 工作模式
static void Led_NoneMode(uint8_t id);
static void Led_OneshotMode(uint8_t id);
static void Led_TimerMode(uint8_t id);
static void Led_HeartbeatMode(uint8_t id);

// LED 工作状态
static void setupLedMode(Led_ID_t id, Led_STATE_t state);
static Led_STATE_t getLedMode(Led_ID_t id);

/**
 * @brief   LED 初始化函数
 *
 */
void Led_Init(void)
{
    g_LedList.ports = g_gpio_out_list;
    g_LedList.state = stateArray;
    g_LedList.tick = tickArray;
    g_LedList.getLedMode = getLedMode;
    g_LedList.setupLedMode = setupLedMode;

    for (uint8_t i = 0; i < LED_NUM; i++)
    {
        g_LedList.state[i] = None;
        g_LedList.tick[i] = 0;
    }
    // 如果需要使某个LED处于其它模式, 可在这修改
    // g_LedList.getLedMode = ON;
    Led_CheckMode();
}

/**
 * @brief           设置 LED 工作模式
 * 
 * @param id        LED 编号
 * @param state     工作模式
 */
static void setupLedMode(Led_ID_t id, Led_STATE_t state)
{
    g_LedList.state[id] = state;
}

/**
 * @brief       读取 LED 工作模式
 * 
 * @param id    LED 编号
 * @retval      LED 工作模式
 */
static Led_STATE_t getLedMode(Led_ID_t id)
{
    return g_LedList.state[id];
}

/**
 * @brief     点亮 LED
 *
 * @param id  LED 编号
 */
static void Led_turnOn(uint8_t id)
{
    set_pin_low(g_LedList.ports[id].gpio, g_LedList.ports[id].pin);
}

/**
 * @brief     开启 LED
 *
 * @param id  LED 编号
 */
static void Led_turnOff(uint8_t id)
{
    set_pin_hig(g_LedList.ports[id].gpio, g_LedList.ports[id].pin);
}

/**
 * @brief     LED 电平反转
 *
 * @param id  LED 编号
 */
static void Led_Toggle(uint8_t id)
{
    toggle_pin(g_LedList.ports[id].gpio, g_LedList.ports[id].pin);
}

/**
 * @brief     LED None 模式
 *
 * @param id  LED 编号
 */
static void Led_NoneMode(uint8_t id)
{
    Led_turnOff(id);
}

/**
 * @brief     LED 定时闪烁模式
 *
 * @param id  LED 编号
 */
static void Led_TimerMode(uint8_t id)
{
    g_LedList.tick[id]++;
    if (g_LedList.tick[id] < 10)
    {
        return;
    }

    g_LedList.tick[id] = 0;
    Led_Toggle(id);
}

/**
 * @brief     LED 单次闪烁模式
 *
 * @param id  LED_ID
 */
static void Led_OneshotMode(uint8_t id)
{
    static uint8_t finished = 0;
    g_LedList.tick[id]++;
    if ((!finished) && GPIO_PIN_RESET == get_pin_level(g_LedList.ports[id].gpio, g_LedList.ports[id].pin))
    { // 先熄灭 LED
        Led_turnOff(id);
        return;
    }
    if ((!finished) && g_LedList.tick[id] == 4)
    {                   // 200ms 内 不点亮 LED
        Led_turnOn(id); // 点亮 LED
    }

    if ((!finished) && g_LedList.tick[id] == 14)
    { // 500ms 后LED 熄灭, 不再进行闪烁操作
        g_LedList.tick[id] = 0;
        Led_turnOff(id);
        finished = 1;
    }
}

/**
 * @brief     LED 心跳模式
 *
 * @param id  LED_ID
 */
static void Led_HeartbeatMode(uint8_t id)
{
    if (!g_LedList.tick[id])
    { // 先关闭 LED, 保证 4次取反之后为关闭状态
        Led_turnOff(id);
    }

    g_LedList.tick[id]++;
    if (0 == g_LedList.tick[id] % 2 && // 100ms 翻转一次
        g_LedList.tick[id] <= 8)       // 4次反转(400ms)后长灭
    {// 无需特判 0, 进入该分支判断时tick最小值为 1
        Led_Toggle(id);
        return;
    }

    if (22 == g_LedList.tick[id])
    { //
        g_LedList.tick[id] = 0;
    }
}

/**
 * @brief 50ms 运行一次
 *
 */
void Led_CheckMode(void)
{
    for (uint8_t i = 0; i < LED_NUM; i++)
    {
        switch (g_LedList.getLedMode(i))
        {
        case None:
            Led_NoneMode(i);
            break;
        case Oneshot:
            Led_OneshotMode(i);
            break;
        case Heartbeat:
            Led_HeartbeatMode(i);
            break;
        case Timer:
            Led_TimerMode(i);
            break;
        case ON:
            Led_turnOn(i);
            break;
        case OFF:
            Led_turnOff(i);
            break;

        default:
            break;
        }
    }
}
