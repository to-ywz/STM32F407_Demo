#include "bsp.h"
#include "community.h"

#define XTAL_VAL 8000000UL
#define MAX_FREQ_VAL 480000000UL

/**
 * @brief SystemClock Initialization
 * @note 32 直接使用 CubexMX 生成
 */
static void system_clock_init(uint32_t freq)
{
}

/**
 * @brief Borad Initialization
 * 
 */
void bsp_init(void)
{
    // system_clock_init(200000000UL);
    timer_init();
    Led_Init();
}

static uint16_t per10msCounter = 0;
static uint16_t per1msCounter = 0;
/**
 * @brief SysTick Interrupt 10ms polling
 * 
 */
void bsp_per10ms_run(void)
{
    per10msCounter++;
    if (per10msCounter % 5 == 0)
    { // 50ms
        comm_pipe.bit.Led_TODO = 1;
    }
}

/**
 * @brief SysTick Interrupt 1ms polling
 * 
 */
void bsp_per1ms_run(void)
{

}

/**
 * @brief rtos idle 接口
 * @note 预留
 */
void bsp_idle(void)
{
}
