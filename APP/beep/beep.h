#ifndef _BEEP_H
#define _BEEP_H
#include "stm32f10x.h"
#define BEEP_OFF GPIO_ResetBits(GPIOB,GPIO_Pin_8);//���͵�ƽ
#define BEEP_ON GPIO_SetBits(GPIOB,GPIO_Pin_8);//���ߵ�ƽ

void BEEP_Init(void);
#endif
