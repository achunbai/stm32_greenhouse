#include "Beep.h"

void BEEP_Init(void)	  //�˿ڳ�ʼ��
{
	GPIO_InitTypeDef GPIO_InitStructure;	//����һ���ṹ�������������ʼ��GPIO

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);   /* ����GPIOʱ�� */

	/*  ����GPIO��ģʽ��IO�� */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;		//ѡ����Ҫ���õ�IO��
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;		  //�����������ģʽ
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //���ô�������
	GPIO_Init(GPIOB,&GPIO_InitStructure); 	 /* ��ʼ��GPIO */
	
	GPIO_ResetBits(GPIOB,GPIO_Pin_8);//���͵�ƽ
}
