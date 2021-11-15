#ifndef __BSP_SYSTICK_H
#define __BSP_SYSTICK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

#define ENABLE_INT()	__set_PRIMASK(0)	/* ʹ���ж� */
#define DISABLE_INT()	__set_PRIMASK(1)	/* ��ֹ�ж� */
#define TMR_CNT 4 		/* �����ʱ������ */

// ��ʱ������ģʽ
typedef enum TMR_MODE_E
{
	TMR_ONCE_MODE = 0,		/* ����ģʽ*/
	TMR_AUTO_MODE = 1		/* �Զ���װ */
}TMR_MODE_E;

// ��ʱ���ṹ��
typedef struct
{
	__IO uint8_t mode;		/* ��ʱ������ģʽ */
	__IO uint8_t flag;		/* ��ʱ��־  */
	__IO uint32_t count;	/* ��ǰʱ�� */
	__IO uint32_t preload;	/* Ԥװ��ֵ */
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
