#include "pwm2.h"

void pwm2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;//����ṹ�����
	
	RCC_APB2PeriphClockCmd(pwm2_PORT_RCC,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=pwm2_PIN;  //ѡ����Ҫ���õ�IO��
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //�����������ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //���ô�������
	GPIO_Init(pwm2_PORT,&GPIO_InitStructure); 	   /* ��ʼ��GPIO */
	GPIO_SetBits(pwm2_PORT,pwm2_PIN);   //��LED�˿����ߣ�Ϩ������LED
}
