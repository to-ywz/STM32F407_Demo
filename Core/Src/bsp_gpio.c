/**
 * @file bsp_gpio.c
 * @author BlackSheep (blacksheep.208h@gmail.com)
 * @brief
 * @version 0.1
 * @date 2021-11-10
 *
 * @copyright Copyright (c) 2021
 *
 */
#include "bsp_gpio.h"

/**
 * @brief GPIO ��ʼ�� (�󲿷�GPIOͨ��������    ��, �����Զ������ʵ�ֳ�ʼ��)
 *
 */
gpio_out_t g_gpio_out_list[] = {
    {LED1_PORT, LED1_PIN},
    {LED2_PORT, LED2_PIN}};

#define GPIO_OUT_NUM (sizeof(gpio_out_t) / sizeof(g_gpio_out_list))

/**
 * @brief GPIO �����ʼ��
 * @note Ԥ��
 */
void in_gpio_init(void)
{
    // GPIO_InitTypeDef gpio_init;

    // gpio_init.Mode = GPIO_MODE_INPUT;
    // gpio_init.Pull = GPIO_PULLUP;

    // for (uint8_t i = 0; i < GPIO_IN_NUM; i++){

    // }
}

/**
 * @brief GPIO �����ʼ��
 *
 */
void out_gpio_init(void)
{
    GPIO_InitTypeDef gpio_init;

    gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init.Pull = GPIO_PULLUP;
    gpio_init.Speed = GPIO_SPEED_FREQ_MEDIUM;

    for (uint8_t i = 0; i < GPIO_OUT_NUM; i++)
    {
        gpio_init.Pin = g_gpio_out_list[i].pin;
        HAL_GPIO_Init(g_gpio_out_list[i].gpio, &gpio_init);
    }
}

/**
 * @brief 			GPIO ��ƽ����
 *
 * @param gpiox 	GPIO �˿�
 * @param gpio_pin 	GPIO ����
 */
void set_pin_hig(GPIO_TypeDef *gpiox, uint16_t gpio_pin)
{
    HAL_GPIO_WritePin(gpiox, gpio_pin, GPIO_PIN_SET);
}

/**
 * @brief 			GPIO ��ƽ����
 *
 * @param gpiox 	GPIO �˿�
 * @param gpio_pin 	GPIO ����
 */
void set_pin_low(GPIO_TypeDef *gpiox, uint16_t gpio_pin)
{
    HAL_GPIO_WritePin(gpiox, gpio_pin, GPIO_PIN_RESET);
}

/**
 * @brief 			GPIO ��ƽ��ת
 *
 * @param gpiox 	GPIO �˿�
 * @param gpio_pin 	GPIO ����
 */
void toggle_pin(GPIO_TypeDef *gpiox, uint16_t gpio_pin)
{
    HAL_GPIO_TogglePin(gpiox, gpio_pin);
}

/**
 * @brief           ��ȡ GPIO ��ƽ
 *
 * @param gpiox     GPIO �˿�
 * @param pin       GPIO ����
 * @retval          ��ƽ״̬
 */
uint8_t get_pin_level(GPIO_TypeDef *gpiox, uint16_t pin)
{
    return (uint8_t)HAL_GPIO_ReadPin(gpiox, pin);
}