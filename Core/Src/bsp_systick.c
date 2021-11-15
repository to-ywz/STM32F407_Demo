#include "bsp_systick.h"

static SOFT_TMR s_tmr[TMR_CNT];
volatile int32_t G_getruntime = 0;
static volatile uint32_t S_delaycnt = 0;
static volatile uint8_t S_timeoutflag = 0;
static volatile uint8_t S_enablesystickisr = 0; /* �ȴ�������ʼ�� */

/**
 * @brief ��ʱ����ʼ��
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
		����systic�ж�����Ϊ1ms��������systick�жϡ�
    	SystemCoreClock �ǹ̼��ж����ϵͳ�ں�ʱ��
    	SysTick_Config() �������βα�ʾ�ں�ʱ�Ӷ��ٸ����ں󴥷�һ��Systick��ʱ�ж�.
	    	-- SystemCoreClock / 1000  ��ʾ��ʱƵ��Ϊ 1000Hz�� Ҳ���Ƕ�ʱ����Ϊ  1ms
	    	-- SystemCoreClock / 500   ��ʾ��ʱƵ��Ϊ 500Hz��  Ҳ���Ƕ�ʱ����Ϊ  2ms
	    	-- SystemCoreClock / 2000  ��ʾ��ʱƵ��Ϊ 2000Hz�� Ҳ���Ƕ�ʱ����Ϊ  500us

    	���ڳ����Ӧ�ã�����һ��ȡ��ʱ����1ms�����ڵ���CPU���ߵ͹���Ӧ�ã��������ö�ʱ����Ϊ 10ms
    */
    SysTick_Config(SystemCoreClock / 1000);
    S_enablesystickisr = 1; /* 1��ʾִ��systick�ж� */
}

/**
 * @brief           ��ʼ����ʱ��
 * 
 * @param tmrid     ��ʱ��ID
 * @param period    ��ʱ����
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
 * @brief           ��ʼ���Զ���װ��ʱ��
 * 
 * @param tmrid     ��ʱ�� ID
 * @param period    ��ʱ������
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
 * @brief       ֹͣ��ʱ��
 * 
 * @param tmrid ��ʱ��ID
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
 * @brief           ��������ʱ���Ƿ�ʱ��
 * 
 * @param   tmrid   ��ʱ��ID
 * @return  uint8_t �Ƿ�ʱ��
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
 * @brief           ��ȡ����ʱ��
 * 
 * @return int32_t  ����ʱ��
 */
int32_t bsp_getruntime(void)
{
    int32_t runtime;

    DISABLE_INT();          /* ���ж� */
    runtime = G_getruntime; /* ���������Systick�ж��б���д�������Ҫ���жϽ��б��� */
    ENABLE_INT();           /* ���ж� */

    return runtime;
}

/**
 * @brief               �������ʱ��
 * 
 * @param _lastTime     
 * @return int32_t 
 */
int32_t checkruntime(int32_t _lastTime)
{
    int32_t now_time;
    int32_t time_diff;

    DISABLE_INT(); /* ���ж� */

    now_time = G_getruntime; /* ���������Systick�ж��б���д�������Ҫ���жϽ��б��� */

    ENABLE_INT(); /* ���ж� */

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
 * @brief       ��ʱ�ݼ�����
 * 
 * @param tmr   ��Ҫ�ݼ��Ķ�ʱ��
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
 * @brief       systick ms ��ʱ
 * 
 * @param n     ��ʱʱ��
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

    DISABLE_INT(); /* ���ж� */

    S_delaycnt = n;
    S_timeoutflag = 0;

    ENABLE_INT(); /* ���ж� */

    while (1)
    {
        /*
			�ȴ��ӳ�ʱ�䵽
			ע�⣺��������Ϊ S_timeoutflag = 0�����Կ����Ż�������� S_timeoutflag ������������Ϊ volatile
		*/
        if (S_timeoutflag == 1)
        {
            break;
        }
    }
}

/**
 * @brief       systick us ��ʱ
 * 
 * @param n     ��ʱʱ��
 */
void delayus(uint32_t n)
{
    uint32_t ticks;
    uint32_t told;
    uint32_t tnow;
    uint32_t tcnt = 0;
    uint32_t reload;

    reload = SysTick->LOAD;
    ticks = n * (SystemCoreClock / 1000000); /* ��Ҫ�Ľ����� */

    tcnt = 0;
    told = SysTick->VAL; /* �ս���ʱ�ļ�����ֵ */

    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            /* SYSTICK��һ���ݼ��ļ����� */
            if (tnow < told)
            {
                tcnt += told - tnow;
            }
            /* ����װ�صݼ� */
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;

            /* ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳� */
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

    /* ÿ��1ms����1�� �������� bsp_DelayMS�� */
    if (S_delaycnt > 0)
    {
        if (--S_delaycnt == 0)
        {
            S_timeoutflag = 1;
        }
    }

    /* ÿ��1ms���������ʱ���ļ��������м�һ���� */
    for (i = 0; i < TMR_CNT; i++)
    {
        softtimerdec(&s_tmr[i]);
    }

    /* ȫ������ʱ��ÿ1ms��1 */
    G_getruntime++;
    if (G_getruntime == 0x7FFFFFFF) /* ��������� int32_t ���ͣ������Ϊ 0x7FFFFFFF */
    {
        G_getruntime = 0;
    }

    bsp_per1ms_run(); /* ÿ��1ms����һ�δ˺������˺����� bsp.c */
    if (++s_count > 10)
    {
        s_count = 0;
        bsp_per10ms_run();
    }
}
/*
*********************************************************************************************************
*	�� �� ��: SysTick_Handler
*	����˵��: ϵͳ��શ�ʱ���жϷ�����������ļ��������˸ú�����
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void SysTick_Handler(void)
{
    systick_isr(); /* ������bsp��ĵδ�ʱ�жϷ������ */
    HAL_IncTick();
}
