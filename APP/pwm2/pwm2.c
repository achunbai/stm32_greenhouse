#include "pwm2.h"

void pwm2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;//定义结构体变量
	
	RCC_APB2PeriphClockCmd(pwm2_PORT_RCC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=pwm2_PIN;  //选择你要设置的IO口
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //设置推挽输出模式
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //设置传输速率
	GPIO_Init(pwm2_PORT,&GPIO_InitStructure); 	   /* 初始化GPIO */
	GPIO_SetBits(pwm2_PORT,pwm2_PIN);   //将LED端口拉高，熄灭所有LED
}
