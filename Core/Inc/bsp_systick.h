#ifndef __BSP_SYSTICK_H
#define __BSP_SYSTICK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

#define ENABLE_INT()	__set_PRIMASK(0)	/* 使能中断 */
#define DISABLE_INT()	__set_PRIMASK(1)	/* 禁止中断 */
#define TMR_CNT 4 		/* 软件定时器个数 */

// 定时器工作模式
typedef enum TMR_MODE_E
{
	TMR_ONCE_MODE = 0,		/* 单次模式*/
	TMR_AUTO_MODE = 1		/* 自动重装 */
}TMR_MODE_E;

// 定时器结构体
typedef struct
{
	__IO uint8_t mode;		/* 定时器工作模式 */
	__IO uint8_t flag;		/* 超时标志  */
	__IO uint32_t count;	/* 当前时长 */
	__IO uint32_t preload;	/* 预装载值 */
}SOFT_TMR;

void timer_init(void);
void start_timer(uint8_t tmrid, uint32_t period);
void start_autotimer(uint8_t tmrid, uint32_t period);
uint8_t bsp_checktimer(uint8_t tmrid);
int32_t bsp_getruntime(void);
int32_t checkruntime(int32_t _lastTime);
static void softtimerdec(SOFT_TMR *tmr);
void delayms(uint32_t n);
void delayus(uint32_t n);

#ifdef __cplusplus
}
#endif

#endif
