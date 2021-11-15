#include "bsp_systick.h"

static SOFT_TMR s_tmr[TMR_CNT];
volatile int32_t G_getruntime = 0;
static volatile uint32_t S_delaycnt = 0;
static volatile uint8_t S_timeoutflag = 0;
static volatile uint8_t S_enablesystickisr = 0; /* 等待变量初始化 */

/**
 * @brief 定时器初始化
 * 
 */
void timer_init(void)
{
    uint8_t i;
    for (i = 0; i < TMR_CNT; i++)
    {
        s_tmr[i].count = 0;
        s_tmr[i].preload = 0;
        s_tmr[i].flag = 0;
        s_tmr[i].mode = TMR_ONCE_MODE;
    }
    /*
		配置systic中断周期为1ms，并启动systick中断。
    	SystemCoreClock 是固件中定义的系统内核时钟
    	SysTick_Config() 函数的形参表示内核时钟多少个周期后触发一次Systick定时中断.
	    	-- SystemCoreClock / 1000  表示定时频率为 1000Hz， 也就是定时周期为  1ms
	    	-- SystemCoreClock / 500   表示定时频率为 500Hz，  也就是定时周期为  2ms
	    	-- SystemCoreClock / 2000  表示定时频率为 2000Hz， 也就是定时周期为  500us

    	对于常规的应用，我们一般取定时周期1ms。对于低速CPU或者低功耗应用，可以设置定时周期为 10ms
    */
    SysTick_Config(SystemCoreClock / 1000);
    S_enablesystickisr = 1; /* 1表示执行systick中断 */
}

/**
 * @brief           初始化定时器
 * 
 * @param tmrid     定时器ID
 * @param period    定时周期
 */
void start_timer(uint8_t tmrid, uint32_t period)
{
    if (tmrid >= TMR_CNT)
        return;
    DISABLE_INT();
    s_tmr[tmrid].count = period;
    s_tmr[tmrid].preload = period;
    s_tmr[tmrid].flag = 0;
    s_tmr[tmrid].mode = TMR_ONCE_MODE;
    ENABLE_INT();
}

/**
 * @brief           初始化自动重装定时器
 * 
 * @param tmrid     定时器 ID
 * @param period    定时器周期
 */
void start_autotimer(uint8_t tmrid, uint32_t period)
{
    if (tmrid > TMR_CNT)
        return;
    DISABLE_INT();
    s_tmr[tmrid].count = period;
    s_tmr[tmrid].preload = period;
    s_tmr[tmrid].flag = 0;
    s_tmr[tmrid].mode = TMR_AUTO_MODE;
    ENABLE_INT();
}

/**
 * @brief       停止定时器
 * 
 * @param tmrid 定时器ID
 */
void stop_timer(uint8_t tmrid)
{
    if (tmrid >= TMR_CNT)
        return;
    DISABLE_INT();
    s_tmr[tmrid].count = 0;
    s_tmr[tmrid].flag = 0;
    s_tmr[tmrid].mode = TMR_ONCE_MODE;
    ENABLE_INT();
}

/**
 * @brief           检查软件定时器是否到时间
 * 
 * @param   tmrid   定时器ID
 * @return  uint8_t 是否到时间
 */
uint8_t bsp_checktimer(uint8_t tmrid)
{
    if (tmrid >= TMR_CNT)
    {
        return 0;
    }

    if (s_tmr[tmrid].flag == 1)
    {
        s_tmr[tmrid].flag = 0;
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief           获取运行时间
 * 
 * @return int32_t  运行时间
 */
int32_t bsp_getruntime(void)
{
    int32_t runtime;

    DISABLE_INT();          /* 关中断 */
    runtime = G_getruntime; /* 这个变量在Systick中断中被改写，因此需要关中断进行保护 */
    ENABLE_INT();           /* 开中断 */

    return runtime;
}

/**
 * @brief               检测运行时间
 * 
 * @param _lastTime     
 * @return int32_t 
 */
int32_t checkruntime(int32_t _lastTime)
{
    int32_t now_time;
    int32_t time_diff;

    DISABLE_INT(); /* 关中断 */

    now_time = G_getruntime; /* 这个变量在Systick中断中被改写，因此需要关中断进行保护 */

    ENABLE_INT(); /* 开中断 */

    if (now_time >= _lastTime)
    {
        time_diff = now_time - _lastTime;
    }
    else
    {
        time_diff = 0x7FFFFFFF - _lastTime + now_time;
    }

    return time_diff;
}

/**
 * @brief       定时递减函数
 * 
 * @param tmr   需要递减的定时器
 */
static void softtimerdec(SOFT_TMR *tmr)
{
    if (tmr->count > 0)
    {
        if (--tmr->count == 0)
        {
            tmr->flag = 1;
            if (tmr->mode == TMR_AUTO_MODE)
            {
                tmr->count = tmr->preload;
            }
        }
    }
}

/**
 * @brief       systick ms 延时
 * 
 * @param n     延时时间
 */
void delayms(uint32_t n)
{
    if (n == 0)
    {
        return;
    }
    else if (n == 1)
    {
        n = 2;
    }

    DISABLE_INT(); /* 关中断 */

    S_delaycnt = n;
    S_timeoutflag = 0;

    ENABLE_INT(); /* 开中断 */

    while (1)
    {
        /*
			等待延迟时间到
			注意：编译器认为 S_timeoutflag = 0，所以可能优化错误，因此 S_timeoutflag 变量必须申明为 volatile
		*/
        if (S_timeoutflag == 1)
        {
            break;
        }
    }
}

/**
 * @brief       systick us 延时
 * 
 * @param n     延时时间
 */
void delayus(uint32_t n)
{
    uint32_t ticks;
    uint32_t told;
    uint32_t tnow;
    uint32_t tcnt = 0;
    uint32_t reload;

    reload = SysTick->LOAD;
    ticks = n * (SystemCoreClock / 1000000); /* 需要的节拍数 */

    tcnt = 0;
    told = SysTick->VAL; /* 刚进入时的计数器值 */

    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            /* SYSTICK是一个递减的计数器 */
            if (tnow < told)
            {
                tcnt += told - tnow;
            }
            /* 重新装载递减 */
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;

            /* 时间超过/等于要延迟的时间,则退出 */
            if (tcnt >= ticks)
            {
                break;
            }
        }
    }
}



static uint8_t s_count = 0;
extern void bsp_per10ms_run(void);
extern void bsp_per1ms_run(void);
void systick_isr(void)
{
    uint8_t i;

    /* 每隔1ms进来1次 （仅用于 bsp_DelayMS） */
    if (S_delaycnt > 0)
    {
        if (--S_delaycnt == 0)
        {
            S_timeoutflag = 1;
        }
    }

    /* 每隔1ms，对软件定时器的计数器进行减一操作 */
    for (i = 0; i < TMR_CNT; i++)
    {
        softtimerdec(&s_tmr[i]);
    }

    /* 全局运行时间每1ms增1 */
    G_getruntime++;
    if (G_getruntime == 0x7FFFFFFF) /* 这个变量是 int32_t 类型，最大数为 0x7FFFFFFF */
    {
        G_getruntime = 0;
    }

    bsp_per1ms_run(); /* 每隔1ms调用一次此函数，此函数在 bsp.c */
    if (++s_count > 10)
    {
        s_count = 0;
        bsp_per10ms_run();
    }
}
/*
*********************************************************************************************************
*	函 数 名: SysTick_Handler
*	功能说明: 系统嘀嗒定时器中断服务程序。启动文件中引用了该函数。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void SysTick_Handler(void)
{
    systick_isr(); /* 安富莱bsp库的滴答定时中断服务程序 */
    HAL_IncTick();
}
