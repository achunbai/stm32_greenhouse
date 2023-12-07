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



void dht11_data_pros(u8 *temp, u8 *humi, u8 *temp_2, u8 *humi_2)	//数据处理函数
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
	
	printf("显示信息：\r\n空气温度=%d°C  空气相对湿度=%d%%\r\n土壤温度=%d°C  土壤相对湿度=%d%%\r\n", *temp, *humi, *temp_2, *humi_2);
}

void co2_data_pros(u16 *co2_level)	//数据处理函数
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
	
	printf("CO2浓度=%d%%\r\n",*co2_level);
}

void light_data_pros(u8 *lsens_value)	//数据处理函数
{
	u8 light_buf[4];
	
	FRONT_COLOR=RED;
	
	*lsens_value=Lsens_Get_Val();
	light_buf[0]=*lsens_value/100+0x30;
	light_buf[1]=*lsens_value%100/10+0x30;
	light_buf[2]=*lsens_value%100%10+0x30;
	light_buf[3]='\0';
	LCD_ShowString(248,160,tftlcd_data.width,tftlcd_data.height,16,light_buf);
	
	printf("光照强度=%d%%\r\n",*lsens_value);
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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //中断优先级分组 分2组
	LED_Init();
	USART1_Init(115200);
	TFTLCD_Init();			//LCD初始化	
	ADCx_Init();				//初始化ADC
	//EN25QXX_Init();				//初始化EN25Q128	
	//BEEP_Init();	
	ESP8266_Init();
	ESP8266_Mqtt_Connect();
	Hwjs_Init();
	

	while(font_init()) 		        //检查字库
	{  
		LCD_ShowString(30,5,tftlcd_data.width,tftlcd_data.height,16,"Font Error!   ");
		delay_ms(500);
	} 
	
	FRONT_COLOR=RED;
	LCD_ShowFontString(10,10,tftlcd_data.width,tftlcd_data.height,"钛合金大棚计划V3.4",16,0);
	FRONT_COLOR=BLACK;
	LCD_ShowFontString(10,30,tftlcd_data.width,tftlcd_data.height,"本计划由：                倾情赞助",16,0);
	FRONT_COLOR=GREEN;
	LCD_ShowFontString(90,30,tftlcd_data.width,tftlcd_data.height,"卢浩然的理想L9 ",16,0);
	FRONT_COLOR=BLACK;
	LCD_ShowFontString(10,60,tftlcd_data.width,tftlcd_data.height,"DHT11_1 测试结果：",16,0);
	LCD_ShowFontString(10,80,tftlcd_data.width,tftlcd_data.height,"DHT11_2 测试结果：",16,0);
	LCD_ShowFontString(10,100,tftlcd_data.width,tftlcd_data.height,"---------------大棚信息---------------",16,0);
	LCD_ShowFontString(10,120,tftlcd_data.width,tftlcd_data.height,"空气温度:   ℃",16,0);
	LCD_ShowFontString(170,120,tftlcd_data.width,tftlcd_data.height,"土壤温度:   ℃",16,0);
	LCD_ShowFontString(10,140,tftlcd_data.width,tftlcd_data.height,"空气相对湿度:   %",16,0);
	LCD_ShowFontString(170,140,tftlcd_data.width,tftlcd_data.height,"土壤相对湿度:   %",16,0);
	LCD_ShowFontString(10,160,tftlcd_data.width,tftlcd_data.height,"CO2浓度:    %",16,0);
	LCD_ShowFontString(170,160,tftlcd_data.width,tftlcd_data.height,"光照强度:    %",16,0);
	LCD_ShowFontString(10,180,tftlcd_data.width,tftlcd_data.height,"---------------控制信息---------------",16,0);
	LCD_ShowFontString(10,200,tftlcd_data.width,tftlcd_data.height,"控制模式：",16,0);
	LCD_ShowFontString(10,220,tftlcd_data.width,tftlcd_data.height,"补光状态：",16,0);
	LCD_ShowFontString(170,220,tftlcd_data.width,tftlcd_data.height,"补光亮度等级：",16,0);
	LCD_ShowFontString(10,240,tftlcd_data.width,tftlcd_data.height,"风扇状态：",16,0);
	LCD_ShowFontString(170,240,tftlcd_data.width,tftlcd_data.height,"水泵状态：",16,0);
	
	FRONT_COLOR=RED;
	while(DHT11_Init())	//检测DHT11_1是否存在
	{
		LCD_ShowFontString(150,60,tftlcd_data.width,tftlcd_data.height,"错误！",16,0);
		printf("DHT11_1 Check Error!\r\n");
		delay_ms(500);		
	}
	LCD_ShowFontString(150,60,tftlcd_data.width,tftlcd_data.height,"成功！",16,0);
	printf("DHT11_1 检测成功!\r\n");
	
	while(DHT11_2_Init())	//检测DHT11_1是否存在
	{
		LCD_ShowFontString(150,80,tftlcd_data.width,tftlcd_data.height,"错误！",16,0);
		printf("DHT11_2 Check Error!\r\n");
		delay_ms(500);		
	}
	LCD_ShowFontString(150,80,tftlcd_data.width,tftlcd_data.height,"成功！",16,0);
	printf("DHT11_2 检测成功!\r\n");
	
	while(1)
	{
		/*
		RGB_ShowCol(col%6,RGB_COLOR_WHITE);
		col表示亮多少列灯，范围是0-6,0表示全灭
		RGB颜色参数：
		RGB_COLOR_RED,RGB_COLOR_GREEN,RGB_COLOR_BLUE,RGB_COLOR_WHITE,RGB_COLOR_YELLOW
		+红外接收码 00FFA857
		-红外接收码 00FFE01F
		0红外接收码 00FF6897
		1红外接收码 00FF30CF
		2红外接收码 00FF18E7
		3红外接收码 00FF7A85
		4红外接收码 00FF10EF
		5红外接收码 00FF38C7
		6红外接收码 00FF5AA5
		7红外接收码 00FF42BD
		8红外接收码 00FF4AB5
		9红外接收码 00FF52AD
		*/
	  DHT11_Read_Data(&temp, &humi);		//温度湿度在主函数中定义，数据处理读写主函数内的温湿度
		DHT11_2_Read_Data(&temp_2, &humi_2);		//温度湿度在主函数中定义，数据处理读写主函数内的温湿度
		
		co2_level=Get_ADC_Value(ADC_Channel_1,10);
		lsens_value=Lsens_Get_Val();
		dht11_data_pros(&temp, &humi, &temp_2, &humi_2); 	
		co2_data_pros(&co2_level); 
		light_data_pros(&lsens_value);
		printf("控制信息：\r\n空气温度=%d°C  空气相对湿度=%d%%\r\n土壤温度=%d°C  土壤相对湿度=%d%%\r\n", temp, humi, temp_2, humi_2);
		
		Esp8266_SendData_ReceiveMsg(&control_mode, &temp, &humi, &temp_2, &humi_2, &lsens_value, &light_level, &fan_status, &pump_status, &co2_level);
		
		if(hw_jsbz==1)	//如果红外接收到
		{
			hw_jsbz=0;	   //清零
			printf("红外接收码：\r\n%0.8X\r\n",hw_jsm);
			switch(hw_jsm)
			{
				case 0x00FFA857:
					control_mode = 1;
					printf("红外控制信息：\r\n手动模式 \r\n");
					break;
				case 0x00FFE01F:
					control_mode = 0;
					printf("红外控制信息：\r\n自动模式 \r\n");
					break;
				case 0x00FF6897:
					RGB_ShowCol(0,RGB_COLOR_WHITE);
					light_level=0;
					LCD_ShowFontString(278,220,tftlcd_data.width,tftlcd_data.height,"0",16,0);
					printf("红外控制信息：\r\n补光强度 0 \r\n");
					break;
				case 0x00FF30CF:
					RGB_ShowCol(1,RGB_COLOR_WHITE);
					light_level=1;
					LCD_ShowFontString(278,220,tftlcd_data.width,tftlcd_data.height,"1",16,0);
					printf("红外控制信息：\r\n补光强度 1 \r\n");
					break;
				case 0x00FF18E7:
					RGB_ShowCol(2,RGB_COLOR_WHITE);
					light_level=2;
					LCD_ShowFontString(278,220,tftlcd_data.width,tftlcd_data.height,"2",16,0);
					printf("红外控制信息：\r\n补光强度 2 \r\n");
					break;
				case 0x00FF7A85:
					RGB_ShowCol(3,RGB_COLOR_WHITE);
					light_level=3;
					LCD_ShowFontString(278,220,tftlcd_data.width,tftlcd_data.height,"3",16,0);
					printf("红外控制信息：\r\n补光强度 3 \r\n");
					break;
				case 0x00FF10EF:
					RGB_ShowCol(4,RGB_COLOR_WHITE);
					light_level=4;
					LCD_ShowFontString(278,220,tftlcd_data.width,tftlcd_data.height,"4",16,0);
					printf("红外控制信息：\r\n补光强度 4 \r\n");
					break;
				case 0x00FF38C7:
					RGB_ShowCol(5,RGB_COLOR_WHITE);
					light_level=5;
					LCD_ShowFontString(278,220,tftlcd_data.width,tftlcd_data.height,"5",16,0);
					printf("红外控制信息：\r\n补光强度 5 \r\n");
					break;
				case 0x00FF5AA5:
					FRONT_COLOR=GREEN;
					LCD_ShowFontString(85,240,tftlcd_data.width,tftlcd_data.height,"开启",16,0);
					printf("红外控制信息：\r\n开启风扇 \r\n");
					PWM3_ON;	//开风扇
					fan_status=1;
					break;
				case 0x00FF42BD:
					FRONT_COLOR=RED;
					LCD_ShowFontString(85,240,tftlcd_data.width,tftlcd_data.height,"关闭",16,0);
					printf("红外控制信息：\r\n关闭风扇 \r\n");
					PWM3_OFF;	//关风扇
					fan_status=0;
					break;
				case 0x00FF4AB5:
					FRONT_COLOR=GREEN;
					LCD_ShowFontString(248,240,tftlcd_data.width,tftlcd_data.height,"开启",16,0);
					printf("红外控制信息：\r\n开启水泵 \r\n");
					PWM2_ON;	//开闸放水
					pump_status=1;
					break;
				case 0x00FF52AD:
					FRONT_COLOR=RED;
					LCD_ShowFontString(248,240,tftlcd_data.width,tftlcd_data.height,"关闭",16,0);
					printf("红外控制信息：\r\n关闭水泵 \r\n");
					PWM2_OFF;	//关闸停水
					pump_status=0;
					break;
			}
			hw_jsm=0;					//接收码清零
		}			
		
		
		if(control_mode == 0)
		{
			FRONT_COLOR=GREEN;
			LCD_ShowFontString(85,200,tftlcd_data.width,tftlcd_data.height,"自动模式",16,0);
			if(lsens_value<60)	//感知光照强度开启补光
			{
				FRONT_COLOR=GREEN;
				LCD_ShowFontString(85,220,tftlcd_data.width,tftlcd_data.height,"开启",16,0);
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
					LCD_ShowFontString(85,220,tftlcd_data.width,tftlcd_data.height,"关闭",16,0);
					LCD_ShowFontString(278,220,tftlcd_data.width,tftlcd_data.height,"0",16,0);
					delay_ms(100);
				}
		
		if(temp > 29 || co2_level > 40)
			{
				FRONT_COLOR=GREEN;
				LCD_ShowFontString(85,240,tftlcd_data.width,tftlcd_data.height,"开启",16,0);
				PWM3_ON;	//开风扇
				fan_status = 1;
			}
		else
			{
				FRONT_COLOR=RED;
				LCD_ShowFontString(85,240,tftlcd_data.width,tftlcd_data.height,"关闭",16,0);
				PWM3_OFF;	//关风扇
				fan_status = 0;
			}
		
		
		if(humi_2 < 35)
			{
				FRONT_COLOR=GREEN;
				LCD_ShowFontString(248,240,tftlcd_data.width,tftlcd_data.height,"开启",16,0);
				PWM2_ON;	//开闸放水
				pump_status = 1;
			}
		else
			{
				FRONT_COLOR=RED;
				LCD_ShowFontString(248,240,tftlcd_data.width,tftlcd_data.height,"关闭",16,0);
				PWM2_OFF;	//关闸停水
				pump_status = 0;
			}
		}		
		else
		{
			FRONT_COLOR=RED;
			LCD_ShowFontString(85,200,tftlcd_data.width,tftlcd_data.height,"手动模式",16,0);
			FRONT_COLOR=YELLOW;
			LCD_ShowFontString(85,220,tftlcd_data.width,tftlcd_data.height,"手动控制",16,0);
		}
		
		//delay_ms(500);		
	}
}

