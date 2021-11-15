/**
 * @file bsp_gpio.h
 * @author BlackSheep (blacksheep.208h@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2021-11-10
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef BSP_GPIO_H__
#define BSP_GPIO_H__

#include "stm32f4xx_hal.h"

#define LED1_PORT GPIOA
#define LED1_PIN GPIO_PIN_6
#define LED2_PORT GPIOA
#define LED2_PIN GPIO_PIN_7

typedef struct gpio_out
{
    GPIO_TypeDef *gpio;
    uint16_t pin;
} gpio_out_t;

#define LED_NUM 2
#define LED_INDEX_START 0
#define LED_INDEX_END (LED_NUM - 1)

void set_pin_hig(GPIO_TypeDef *gpio, uint16_t pin);
void set_pin_low(GPIO_TypeDef *gpio, uint16_t pin);
void toggle_pin(GPIO_TypeDef *gpio, uint16_t pin);
uint8_t get_pin_level(GPIO_TypeDef *gpiox, uint16_t pin);

#endif
