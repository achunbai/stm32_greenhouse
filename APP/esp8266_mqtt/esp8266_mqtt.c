#include "esp8266_mqtt.h"
#include "font_show.h"
#include "tftlcd.h"
#include "SysTick.h"
#include "led.h"
#include "cJSON.h"
#include "ws2812.h"

char cStr [ 100 ] = { 0 }, cCh;
char * pCh, * pCh1;

void ESP8266_Mqtt_Connect(void){
	while(font_init()) 		        //检查字库
	{  
		LCD_ShowString(30,5,tftlcd_data.width,tftlcd_data.height,16,"Font Error!   ");
		delay_ms(500);
	}
	FRONT_COLOR=RED;
	LCD_ShowFontString(10,10,tftlcd_data.width,tftlcd_data.height,"钛合金大棚计划V3.3初始化中，请稍后...",16,0);
	FRONT_COLOR=BLACK;
	LCD_ShowFontString(10,30,tftlcd_data.width,tftlcd_data.height,"*****正在配置 ESP8266 ...*****",16,0);
	LCD_ShowFontString(10,50,tftlcd_data.width,tftlcd_data.height,"--使能 ESP8266 ...",16,0);
	printf( "\r\n*****正在配置 ESP8266 ...*****\r\n" );
	printf( "\r\n--使能 ESP8266 ...\r\n" );
	GPIO_SetBits ( GPIOA, GPIO_Pin_4 ); 
	LCD_ShowFontString(10,70,tftlcd_data.width,tftlcd_data.height,"--选择 ESP8266 的工作模式为STA...",16,0);
	printf("\r\n--选择 ESP8266 的工作模式为STA...\r\n");
	while( ! ESP8266_Net_Mode_Choose ( STA ) );
	LCD_ShowFontString(10,90,tftlcd_data.width,tftlcd_data.height,"-- ESP8266 连接至WiFi无线网络...",16,0);
	printf("\r\n-- ESP8266 连接至WiFi无线网络...\r\n");
	while( ! ESP8266_JoinAP ( macUser_ESP8266_ApSsid, macUser_ESP8266_ApPwd ) );
	LCD_ShowFontString(10,110,tftlcd_data.width,tftlcd_data.height,"--配置MQTT用户属性...",16,0);
	printf("\r\n--配置MQTT用户属性...\r\n");
	while(! ESP8266_Cmd ( "AT+MQTTUSERCFG=0,1,\"taihejindapeng\",\"\",\"\",0,0,\"\"\r\n", "OK", NULL, 500 ));
	LCD_ShowFontString(10,130,tftlcd_data.width,tftlcd_data.height,"--连接MQTT服务器...",16,0);
	printf("\r\n--连接MQTT服务器...\r\n");
	while(! ESP8266_Cmd ( "AT+MQTTCONN=0,\"YOUR_SERVER\",1883,1\r\n", "OK", NULL, 500 ));
	strEsp8266_Fram_Record .InfBit .FramLength = 0;
	strEsp8266_Fram_Record .InfBit .FramFinishFlag = 0;
	
	// 订阅 "/greenhouse/control" 话题
	while(! ESP8266_Cmd ( "AT+MQTTSUB=0,\"greenhouse/control\",1\r\n", "OK", NULL, 500 ));
	// 发布消息到 "/greenhouse/data" 话题
	while(! ESP8266_Cmd ( "AT+MQTTPUB=0,\"greenhouse/data\",\"hello world\",1,0\r\n", "OK", NULL, 500 ));	
	LCD_ShowFontString(10,150,tftlcd_data.width,tftlcd_data.height,"Broker服务器地址: **你自己写吧**",16,0);
	LCD_ShowFontString(10,170,tftlcd_data.width,tftlcd_data.height,"控制话题为: /greenhouse/control",16,0);
	LCD_ShowFontString(10,190,tftlcd_data.width,tftlcd_data.height,"数据话题为: ",16,0);
	LCD_ShowFontString(10,210,tftlcd_data.width,tftlcd_data.height,"/greenhouse/data/air",16,0);
	LCD_ShowFontString(10,230,tftlcd_data.width,tftlcd_data.height,"/greenhouse/data/soil",16,0);
	LCD_ShowFontString(10,250,tftlcd_data.width,tftlcd_data.height,"/greenhouse/data/utils",16,0);
	LCD_ShowFontString(10,270,tftlcd_data.width,tftlcd_data.height,"/greenhouse/data/mode",16,0);
	
	LCD_ShowFontString(10,290,tftlcd_data.width,tftlcd_data.height,"*****配置 ESP8266 完毕*****",16,0);
	printf( "\r\n*****配置 ESP8266 完毕*****\r\n" );
	delay_ms(200);
	LCD_Clear(WHITE);
}

void mqtt_light_control(u8 light_level)
{
	u8 light_level_buf[2];
	light_level_buf[0] = light_level + 0x30;
	light_level_buf[1] = '\0';
	printf("MQTT控制信息：\r\n补光强度： %d \r\n", light_level);
	RGB_ShowCol(light_level,RGB_COLOR_WHITE);
	LCD_ShowFontString(278,220,tftlcd_data.width,tftlcd_data.height,light_level_buf,16,0);
}

void mqtt_fan_control(u8 fan_status)
{
	printf("MQTT控制信息：\r\n风扇状态： %d \r\n", fan_status);
	if(fan_status)
	{
		FRONT_COLOR=GREEN;
		LCD_ShowFontString(85,240,tftlcd_data.width,tftlcd_data.height,"开启",16,0);
		PWM3_ON;	//开风扇
	}
	else
	{
		FRONT_COLOR=RED;
		LCD_ShowFontString(85,240,tftlcd_data.width,tftlcd_data.height,"关闭",16,0);
		PWM3_OFF;	//关风扇
	}
}

void mqtt_pump_control(u8 pump_status)
{
	printf("MQTT控制信息：\r\n水泵状态： %d \r\n", pump_status);
	if(pump_status)
	{
		FRONT_COLOR=GREEN;
		LCD_ShowFontString(248,240,tftlcd_data.width,tftlcd_data.height,"开启",16,0);
		PWM2_ON;	//开闸放水
	}
	else
	{
		FRONT_COLOR=RED;
		LCD_ShowFontString(248,240,tftlcd_data.width,tftlcd_data.height,"关闭",16,0);
		PWM2_OFF;	//关闸停水
	}
}

void Esp8266_SendData_ReceiveMsg(u8 *control_mode, u8 *temp, u8 *humi, u8 *temp_2, u8 *humi_2, u8 *lsens_value, u8 *light_level, u8 *fan_status, u8 *pump_status, u16 *co2_level)
{
	char cmd[256], buf[256], hex_buf[512];
	u8 i;
	cJSON *json, *device, *mode, *fan, *pump, *light;
    memset(buf, '\0', sizeof(buf));
	memset(cmd, '\0', sizeof(cmd));

	sprintf(buf, "AT+MQTTPUB=0,\"greenhouse/data/air\",\"{\\\"air_temperature\\\": %d}\",1,0\r\n", *temp);
  ESP8266_Cmd ( buf, "OK", NULL, 500 );
	sprintf(buf, "AT+MQTTPUB=0,\"greenhouse/data/air\",\"{\\\"air_humidity\\\": %d}\",1,0\r\n", *humi);
  ESP8266_Cmd ( buf, "OK", NULL, 500 );
	sprintf(buf, "AT+MQTTPUB=0,\"greenhouse/data/air\",\"{\\\"CO2_level\\\": %d}\",1,0\r\n", *co2_level);
  ESP8266_Cmd ( buf, "OK", NULL, 500 );
	sprintf(buf, "AT+MQTTPUB=0,\"greenhouse/data/soil\",\"{\\\"soil_temperature\\\": %d}\",1,0\r\n", *temp_2);
  ESP8266_Cmd ( buf, "OK", NULL, 500 );
	sprintf(buf, "AT+MQTTPUB=0,\"greenhouse/data/soil\",\"{\\\"soil_humidity\\\": %d}\",1,0\r\n", *humi_2);
  ESP8266_Cmd ( buf, "OK", NULL, 500 );
	sprintf(buf, "AT+MQTTPUB=0,\"greenhouse/data/mode\",\"{\\\"control_mode\\\": %d}\",1,0\r\n", *control_mode);
  ESP8266_Cmd ( buf, "OK", NULL, 500 );
	sprintf(buf, "AT+MQTTPUB=0,\"greenhouse/data/util\",\"{\\\"light_level\\\": %d}\",1,0\r\n", *light_level);
  ESP8266_Cmd ( buf, "OK", NULL, 500 );
	sprintf(buf, "AT+MQTTPUB=0,\"greenhouse/data/util\",\"{\\\"fan_status\\\": %d}\",1,0\r\n", *fan_status);
  ESP8266_Cmd ( buf, "OK", NULL, 500 );
	sprintf(buf, "AT+MQTTPUB=0,\"greenhouse/data/util\",\"{\\\"pump_status\\\": %d}\",1,0\r\n", *pump_status);
  ESP8266_Cmd ( buf, "OK", NULL, 500 );
	
	// 在接收到消息时
	if (strEsp8266_Fram_Record.InfBit.FramFinishFlag)
	{
    USART_ITConfig (USART3, USART_IT_RXNE, DISABLE); // 关闭串口接收中断

    strEsp8266_Fram_Record.Data_RX_BUF[strEsp8266_Fram_Record.InfBit.FramLength] = '\0';
    printf("Received =%s\n", strEsp8266_Fram_Record.Data_RX_BUF);
		
		if ( ( pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "MODE_CONTROL_CMD" ) ) != 0 )
    {
      cCh = * ( pCh + 17 );
      printf("%c\n",cCh);
			*control_mode = cCh - '0'; // 将字符转换为对应的整数
			sprintf(buf, "AT+MQTTPUB=0,\"greenhouse/data/mode\",\"{\\\"control_mode\\\": %d}\",1,0\r\n", *control_mode);
			ESP8266_Cmd ( buf, "OK", NULL, 500 );
		}
		else if ( ( pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "LIGHT_CONTROL_CMD" ) ) != 0 )
		{
			cCh = * ( pCh + 18 );
      printf("%c\n",cCh);
			*light_level = cCh - '0'; // 将字符转换为对应的整数
			mqtt_light_control(*light_level);
			sprintf(buf, "AT+MQTTPUB=0,\"greenhouse/data/util\",\"{\\\"light_level\\\": %d}\",1,0\r\n", *light_level);
			ESP8266_Cmd ( buf, "OK", NULL, 500 );
		}
		else if ( ( pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "FAN_CONTROL_CMD" ) ) != 0 )
		{
			cCh = * ( pCh + 16 );
      printf("%c\n",cCh);
			*fan_status = cCh - '0'; // 将字符转换为对应的整数
			mqtt_light_control(*fan_status);
			sprintf(buf, "AT+MQTTPUB=0,\"greenhouse/data/util\",\"{\\\"fan_status\\\": %d}\",1,0\r\n", *fan_status);
			ESP8266_Cmd ( buf, "OK", NULL, 500 );
		}
		else if ( ( pCh = strstr ( strEsp8266_Fram_Record .Data_RX_BUF, "PUMP_CONTROL_CMD" ) ) != 0 )
		{
			cCh = * ( pCh + 17 );
      printf("%c\n",cCh);
			*pump_status = cCh - '0'; // 将字符转换为对应的整数
			mqtt_light_control(*pump_status);
			sprintf(buf, "AT+MQTTPUB=0,\"greenhouse/data/util\",\"{\\\"pump_status\\\": %d}\",1,0\r\n", *pump_status);
			ESP8266_Cmd ( buf, "OK", NULL, 500 );
		}

		strEsp8266_Fram_Record.InfBit.FramLength = 0;
    strEsp8266_Fram_Record.InfBit.FramFinishFlag = 0;
    USART_ITConfig (USART3, USART_IT_RXNE, ENABLE); // 重新开启串口接收中断
	}
}
