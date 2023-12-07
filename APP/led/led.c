#include "led.h"

/*******************************************************************************
* �� �� ��         : LED_Init
* ��������		   : LED��ʼ������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;//����ṹ�����
	
	RCC_APB2PeriphClockCmd(LED2_PORT_RCC,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //�����������ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //���ô�������

	GPIO_InitStructure.GPIO_Pin=LED2_PIN|PWM2_PIN|PWM3_PIN;  //ѡ����Ҫ���õ�IO��
	GPIO_Init(LED2_PORT,&GPIO_InitStructure); 	   /* ��ʼ��GPIO */
	GPIO_ResetBits(LED2_PORT,LED2_PIN|PWM2_PIN|PWM3_PIN);   //��LED�˿����ߣ�Ϩ������LED
	
}



