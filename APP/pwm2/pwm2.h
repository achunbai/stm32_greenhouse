#ifndef _pwm2_H
#define _pwm2_H

#include "system.h"

/*  LED时钟端口、引脚定义 */
#define pwm2_PORT 			GPIOD   
#define pwm2_PIN 			GPIO_Pin_4
#define pwm2_PORT_RCC		RCC_APB2Periph_GPIOD



#define pwm2 PBout(5)  	  	

#define pwm2_ON GPIO_ResetBits(GPIOD, GPIO_Pin_4);
#define pwm2_OFF GPIO_SetBits(GPIOD, GPIO_Pin_4);

void pwm2_Init(void);


#endif
