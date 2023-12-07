#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "tftlcd.h"
#include "dht11.h"
#include "dht11_2.h"
#include "pwm.h"
#include "lsens.h"
#include "ws2812.h"
#include "flash.h"
#include "font_show.h"
#include "adc_temp.h"
#include "adc.h"
#include "esp8266.h"
#include "esp8266_mqtt.h"
#include "hwjs.h"



void dht11_data_pros(u8 *temp, u8 *humi, u8 *temp_2, u8 *humi_2)	//���ݴ�����
{
	u8 temp_buf[3],humi_buf[3],temp_2_buf[3],humi_2_buf[3];
	while(DHT11_Read_Data(temp,humi))
	{
		delay_ms(10);
	}
	while(DHT11_2_Read_Data(temp_2,humi_2))
	{
		delay_ms(10);
	}
	
	FRONT_COLOR=RED;
	
	temp_buf[0]=*temp/10+0x30;	
	temp_buf[1]=*temp%10+0x30;
	temp_buf[2]='\0';
	LCD_ShowString(88,120,tftlcd_data.width,tftlcd_data.height,16,temp_buf);
		
	humi_buf[0]=*humi/10+0x30;	
	humi_buf[1]=*humi%10+0x30;
	humi_buf[2]='\0';
	LCD_ShowString(118,140,tftlcd_data.width,tftlcd_data.height,16,humi_buf);
	
	temp_2_buf[0]=*temp_2/10+0x30;	
	temp_2_buf[1]=*temp_2%10+0x30;
	temp_2_buf[2]='\0';
	LCD_ShowString(248,120,tftlcd_data.width,tftlcd_data.height,16,temp_2_buf);
		
	humi_2_buf[0]=*humi_2/10+0x30;	
	humi_2_buf[1]=*humi_2%10+0x30;
	humi_2_buf[2]='\0';
	LCD_ShowString(278,140,tftlcd_data.width,tftlcd_data.height,16,humi_2_buf);
	
	printf("��ʾ��Ϣ��\r\n�����¶�=%d��C  �������ʪ��=%d%%\r\n�����¶�=%d��C  �������ʪ��=%d%%\r\n", *temp, *humi, *temp_2, *humi_2);
}

void co2_data_pros(u16 *co2_level)	//���ݴ�����
{
	u8 co2_buf[4];
	
	FRONT_COLOR=RED;
	
	*co2_level=Get_ADC_Value(ADC_Channel_1,10);
	*co2_level=*co2_level*100/4095;
	co2_buf[0]=(u8)*co2_level/100+0x30;
	co2_buf[1]=(u8)*co2_level%100/10+0x30;
	co2_buf[2]=(u8)*co2_level%100%10+0x30;
	co2_buf[3]='\0';
	LCD_ShowString(80,160,tftlcd_data.width,tftlcd_data.height,16,co2_buf);
	
	printf("CO2Ũ��=%d%%\r\n",*co2_level);
}

void light_data_pros(u8 *lsens_value)	//���ݴ�����
{
	u8 light_buf[4];
	
	FRONT_COLOR=RED;
	
	*lsens_value=Lsens_Get_Val();
	light_buf[0]=*lsens_value/100+0x30;
	light_buf[1]=*lsens_value%100/10+0x30;
	light_buf[2]=*lsens_value%100%10+0x30;
	light_buf[3]='\0';
	LCD_ShowString(248,160,tftlcd_data.width,tftlcd_data.height,16,light_buf);
	
	printf("����ǿ��=%d%%\r\n",*lsens_value);
}

int main()
{
	u8 lsens_value=0;
	u8 temp;  	    
	u8 humi;
	u8 temp_2;  	    
	u8 humi_2;
	u8 control_mode=0;
	u8 light_level=0;
	u8 fan_status=0;
	u8 pump_status=0;
	u16 co2_level;
	
	SysTick_Init(72);
	TIM3_CH2_PWM_Init(500,72-1);
	//TIM_SetCompare2(TIM3,0);
	Lsens_Init();
	RGB_LED_Init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����� ��2��
	LED_Init();
	USART1_Init(115200);
	TFTLCD_Init();			//LCD��ʼ��	
	ADCx_Init();				//��ʼ��ADC
	//EN25QXX_Init();				//��ʼ��EN25Q128	
	//BEEP_Init();	
	ESP8266_Init();
	ESP8266_Mqtt_Connect();
	Hwjs_Init();
	

	while(font_init()) 		        //����ֿ�
	{  
		LCD_ShowString(30,5,tftlcd_data.width,tftlcd_data.height,16,"Font Error!   ");
		delay_ms(500);
	} 
	
	FRONT_COLOR=RED;
	LCD_ShowFontString(10,10,tftlcd_data.width,tftlcd_data.height,"�ѺϽ����ƻ�V3.4",16,0);
	FRONT_COLOR=BLACK;
	LCD_ShowFontString(10,30,tftlcd_data.width,tftlcd_data.height,"���ƻ��ɣ�                ��������",16,0);
	FRONT_COLOR=GREEN;
	LCD_ShowFontString(90,30,tftlcd_data.width,tftlcd_data.height,"¬��Ȼ������L9 ",16,0);
	FRONT_COLOR=BLACK;
	LCD_ShowFontString(10,60,tftlcd_data.width,tftlcd_data.height,"DHT11_1 ���Խ����",16,0);
	LCD_ShowFontString(10,80,tftlcd_data.width,tftlcd_data.height,"DHT11_2 ���Խ����",16,0);
	LCD_ShowFontString(10,100,tftlcd_data.width,tftlcd_data.height,"---------------������Ϣ---------------",16,0);
	LCD_ShowFontString(10,120,tftlcd_data.width,tftlcd_data.height,"�����¶�:   ��",16,0);
	LCD_ShowFontString(170,120,tftlcd_data.width,tftlcd_data.height,"�����¶�:   ��",16,0);
	LCD_ShowFontString(10,140,tftlcd_data.width,tftlcd_data.height,"�������ʪ��:   %",16,0);
	LCD_ShowFontString(170,140,tftlcd_data.width,tftlcd_data.height,"�������ʪ��:   %",16,0);
	LCD_ShowFontString(10,160,tftlcd_data.width,tftlcd_data.height,"CO2Ũ��:    %",16,0);
	LCD_ShowFontString(170,160,tftlcd_data.width,tftlcd_data.height,"����ǿ��:    %",16,0);
	LCD_ShowFontString(10,180,tftlcd_data.width,tftlcd_data.height,"---------------������Ϣ---------------",16,0);
	LCD_ShowFontString(10,200,tftlcd_data.width,tftlcd_data.height,"����ģʽ��",16,0);
	LCD_ShowFontString(10,220,tftlcd_data.width,tftlcd_data.height,"����״̬��",16,0);
	LCD_ShowFontString(170,220,tftlcd_data.width,tftlcd_data.height,"�������ȵȼ���",16,0);
	LCD_ShowFontString(10,240,tftlcd_data.width,tftlcd_data.height,"����״̬��",16,0);
	LCD_ShowFontString(170,240,tftlcd_data.width,tftlcd_data.height,"ˮ��״̬��",16,0);
	
	FRONT_COLOR=RED;
	while(DHT11_Init())	//���DHT11_1�Ƿ����
	{
		LCD_ShowFontString(150,60,tftlcd_data.width,tftlcd_data.height,"����",16,0);
		printf("DHT11_1 Check Error!\r\n");
		delay_ms(500);		
	}
	LCD_ShowFontString(150,60,tftlcd_data.width,tftlcd_data.height,"�ɹ���",16,0);
	printf("DHT11_1 ���ɹ�!\r\n");
	
	while(DHT11_2_Init())	//���DHT11_1�Ƿ����
	{
		LCD_ShowFontString(150,80,tftlcd_data.width,tftlcd_data.height,"����",16,0);
		printf("DHT11_2 Check Error!\r\n");
		delay_ms(500);		
	}
	LCD_ShowFontString(150,80,tftlcd_data.width,tftlcd_data.height,"�ɹ���",16,0);
	printf("DHT11_2 ���ɹ�!\r\n");
	
	while(1)
	{
		/*
		RGB_ShowCol(col%6,RGB_COLOR_WHITE);
		col��ʾ�������еƣ���Χ��0-6,0��ʾȫ��
		RGB��ɫ������
		RGB_COLOR_RED,RGB_COLOR_GREEN,RGB_COLOR_BLUE,RGB_COLOR_WHITE,RGB_COLOR_YELLOW
		+��������� 00FFA857
		-��������� 00FFE01F
		0��������� 00FF6897
		1��������� 00FF30CF
		2��������� 00FF18E7
		3��������� 00FF7A85
		4��������� 00FF10EF
		5��������� 00FF38C7
		6��������� 00FF5AA5
		7��������� 00FF42BD
		8��������� 00FF4AB5
		9��������� 00FF52AD
		*/
	  DHT11_Read_Data(&temp, &humi);		//�¶�ʪ�����������ж��壬���ݴ����д�������ڵ���ʪ��
		DHT11_2_Read_Data(&temp_2, &humi_2);		//�¶�ʪ�����������ж��壬���ݴ����д�������ڵ���ʪ��
		
		co2_level=Get_ADC_Value(ADC_Channel_1,10);
		lsens_value=Lsens_Get_Val();
		dht11_data_pros(&temp, &humi, &temp_2, &humi_2); 	
		co2_data_pros(&co2_level); 
		light_data_pros(&lsens_value);
		printf("������Ϣ��\r\n�����¶�=%d��C  �������ʪ��=%d%%\r\n�����¶�=%d��C  �������ʪ��=%d%%\r\n", temp, humi, temp_2, humi_2);
		
		Esp8266_SendData_ReceiveMsg(&control_mode, &temp, &humi, &temp_2, &humi_2, &lsens_value, &light_level, &fan_status, &pump_status, &co2_level);
		
		if(hw_jsbz==1)	//���������յ�
		{
			hw_jsbz=0;	   //����
			printf("��������룺\r\n%0.8X\r\n",hw_jsm);
			switch(hw_jsm)
			{
				case 0x00FFA857:
					control_mode = 1;
					printf("���������Ϣ��\r\n�ֶ�ģʽ \r\n");
					break;
				case 0x00FFE01F:
					control_mode = 0;
					printf("���������Ϣ��\r\n�Զ�ģʽ \r\n");
					break;
				case 0x00FF6897:
					RGB_ShowCol(0,RGB_COLOR_WHITE);
					light_level=0;
					LCD_ShowFontString(278,220,tftlcd_data.width,tftlcd_data.height,"0",16,0);
					printf("���������Ϣ��\r\n����ǿ�� 0 \r\n");
					break;
				case 0x00FF30CF:
					RGB_ShowCol(1,RGB_COLOR_WHITE);
					light_level=1;
					LCD_ShowFontString(278,220,tftlcd_data.width,tftlcd_data.height,"1",16,0);
					printf("���������Ϣ��\r\n����ǿ�� 1 \r\n");
					break;
				case 0x00FF18E7:
					RGB_ShowCol(2,RGB_COLOR_WHITE);
					light_level=2;
					LCD_ShowFontString(278,220,tftlcd_data.width,tftlcd_data.height,"2",16,0);
					printf("���������Ϣ��\r\n����ǿ�� 2 \r\n");
					break;
				case 0x00FF7A85:
					RGB_ShowCol(3,RGB_COLOR_WHITE);
					light_level=3;
					LCD_ShowFontString(278,220,tftlcd_data.width,tftlcd_data.height,"3",16,0);
					printf("���������Ϣ��\r\n����ǿ�� 3 \r\n");
					break;
				case 0x00FF10EF:
					RGB_ShowCol(4,RGB_COLOR_WHITE);
					light_level=4;
					LCD_ShowFontString(278,220,tftlcd_data.width,tftlcd_data.height,"4",16,0);
					printf("���������Ϣ��\r\n����ǿ�� 4 \r\n");
					break;
				case 0x00FF38C7:
					RGB_ShowCol(5,RGB_COLOR_WHITE);
					light_level=5;
					LCD_ShowFontString(278,220,tftlcd_data.width,tftlcd_data.height,"5",16,0);
					printf("���������Ϣ��\r\n����ǿ�� 5 \r\n");
					break;
				case 0x00FF5AA5:
					FRONT_COLOR=GREEN;
					LCD_ShowFontString(85,240,tftlcd_data.width,tftlcd_data.height,"����",16,0);
					printf("���������Ϣ��\r\n�������� \r\n");
					PWM3_ON;	//������
					fan_status=1;
					break;
				case 0x00FF42BD:
					FRONT_COLOR=RED;
					LCD_ShowFontString(85,240,tftlcd_data.width,tftlcd_data.height,"�ر�",16,0);
					printf("���������Ϣ��\r\n�رշ��� \r\n");
					PWM3_OFF;	//�ط���
					fan_status=0;
					break;
				case 0x00FF4AB5:
					FRONT_COLOR=GREEN;
					LCD_ShowFontString(248,240,tftlcd_data.width,tftlcd_data.height,"����",16,0);
					printf("���������Ϣ��\r\n����ˮ�� \r\n");
					PWM2_ON;	//��բ��ˮ
					pump_status=1;
					break;
				case 0x00FF52AD:
					FRONT_COLOR=RED;
					LCD_ShowFontString(248,240,tftlcd_data.width,tftlcd_data.height,"�ر�",16,0);
					printf("���������Ϣ��\r\n�ر�ˮ�� \r\n");
					PWM2_OFF;	//��բͣˮ
					pump_status=0;
					break;
			}
			hw_jsm=0;					//����������
		}			
		
		
		if(control_mode == 0)
		{
			FRONT_COLOR=GREEN;
			LCD_ShowFontString(85,200,tftlcd_data.width,tftlcd_data.height,"�Զ�ģʽ",16,0);
			if(lsens_value<60)	//��֪����ǿ�ȿ�������
			{
				FRONT_COLOR=GREEN;
				LCD_ShowFontString(85,220,tftlcd_data.width,tftlcd_data.height,"����",16,0);
				light_level = 5 - lsens_value / 10;
				switch(light_level)
				{
					case 0:
						RGB_ShowCol(5,RGB_COLOR_WHITE);
						LCD_ShowFontString(278,220,tftlcd_data.width,tftlcd_data.height,"5",16,0);
						break;
					case 1:
						RGB_ShowCol(5,RGB_COLOR_WHITE);
						LCD_ShowFontString(278,220,tftlcd_data.width,tftlcd_data.height,"5",16,0);
						break;
					case 2:
						RGB_ShowCol(4,RGB_COLOR_WHITE);
						LCD_ShowFontString(278,220,tftlcd_data.width,tftlcd_data.height,"4",16,0);
						break;
					case 3:
						RGB_ShowCol(3,RGB_COLOR_WHITE);
						LCD_ShowFontString(278,220,tftlcd_data.width,tftlcd_data.height,"3",16,0);
						break;
					case 4:
						RGB_ShowCol(2,RGB_COLOR_WHITE);
						LCD_ShowFontString(278,220,tftlcd_data.width,tftlcd_data.height,"2",16,0);
						break;
					case 5:
						RGB_ShowCol(1,RGB_COLOR_WHITE);
						LCD_ShowFontString(278,220,tftlcd_data.width,tftlcd_data.height,"1",16,0);
						break;
					
				}
				delay_ms(100);
			}
			else
				{
					RGB_LED_Clear();
					FRONT_COLOR=RED;
					light_level = 0;
					LCD_ShowFontString(85,220,tftlcd_data.width,tftlcd_data.height,"�ر�",16,0);
					LCD_ShowFontString(278,220,tftlcd_data.width,tftlcd_data.height,"0",16,0);
					delay_ms(100);
				}
		
		if(temp > 29 || co2_level > 40)
			{
				FRONT_COLOR=GREEN;
				LCD_ShowFontString(85,240,tftlcd_data.width,tftlcd_data.height,"����",16,0);
				PWM3_ON;	//������
				fan_status = 1;
			}
		else
			{
				FRONT_COLOR=RED;
				LCD_ShowFontString(85,240,tftlcd_data.width,tftlcd_data.height,"�ر�",16,0);
				PWM3_OFF;	//�ط���
				fan_status = 0;
			}
		
		
		if(humi_2 < 35)
			{
				FRONT_COLOR=GREEN;
				LCD_ShowFontString(248,240,tftlcd_data.width,tftlcd_data.height,"����",16,0);
				PWM2_ON;	//��բ��ˮ
				pump_status = 1;
			}
		else
			{
				FRONT_COLOR=RED;
				LCD_ShowFontString(248,240,tftlcd_data.width,tftlcd_data.height,"�ر�",16,0);
				PWM2_OFF;	//��բͣˮ
				pump_status = 0;
			}
		}		
		else
		{
			FRONT_COLOR=RED;
			LCD_ShowFontString(85,200,tftlcd_data.width,tftlcd_data.height,"�ֶ�ģʽ",16,0);
			FRONT_COLOR=YELLOW;
			LCD_ShowFontString(85,220,tftlcd_data.width,tftlcd_data.height,"�ֶ�����",16,0);
		}
		
		//delay_ms(500);		
	}
}

