#ifndef _led_H
#define _led_H

#include "system.h"

/*  LED时钟端口、引脚定义 */


#define LED2_PORT 			GPIOE   
#define LED2_PIN 			GPIO_Pin_5
#define LED2_PORT_RCC		RCC_APB2Periph_GPIOE

#define PWM2_PORT 			GPIOE   
#define PWM2_PIN 			GPIO_Pin_0
#define PWM2_PORT_RCC		RCC_APB2Periph_GPIOE

#define PWM3_PORT 			GPIOE   
#define PWM3_PIN 			GPIO_Pin_2
#define PWM3_PORT_RCC		RCC_APB2Periph_GPIOE

#define LED2 PEout(5)  	

#define LED1_ON GPIO_ResetBits(GPIOB, GPIO_Pin_5);
#define LED1_OFF GPIO_SetBits(GPIOB, GPIO_Pin_5);
#define LED2_ON GPIO_ResetBits(GPIOE, GPIO_Pin_5);
#define LED2_OFF GPIO_SetBits(GPIOE, GPIO_Pin_5);
#define PWM2_ON GPIO_SetBits(GPIOE, GPIO_Pin_0);
#define PWM2_OFF GPIO_ResetBits(GPIOE, GPIO_Pin_0);
#define PWM3_ON GPIO_SetBits(GPIOE, GPIO_Pin_2);
#define PWM3_OFF GPIO_ResetBits(GPIOE, GPIO_Pin_2);

void LED_Init(void);


#endif
