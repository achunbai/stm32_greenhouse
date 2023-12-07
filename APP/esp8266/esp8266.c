#include "esp8266.h"
#include "usart.h"
#include <stdarg.h>

struct  STRUCT_USARTx_Fram strEsp8266_Fram_Record = { 0 };

void ESP8266_Init(void){
	USART3_Init();
	ESP8266_Gpio_Init();
	GPIO_SetBits ( GPIOA, GPIO_Pin_15 );
	GPIO_ResetBits ( GPIOA, GPIO_Pin_4 );
}
/*初始化ESP8266用到的GPIO引脚*/
void ESP8266_Gpio_Init(void){
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/* 配置 CH_PD 引脚 PA4, 使能引脚，高电平有效*/
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE ); 					   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init ( GPIOA, & GPIO_InitStructure );	 
	
	/* 配置 RST 引脚 PA15，低电平复位 */
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_GPIOA, ENABLE ); 			   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	
	GPIO_Init ( GPIOA, & GPIO_InitStructure );

}

//重启WF-ESP8266模块
void Esp8266_Rst ( void )
{
	 GPIO_ResetBits ( GPIOA, GPIO_Pin_15 );
	 delay_ms ( 500 ); 
	 GPIO_SetBits ( GPIOA, GPIO_Pin_15 );
}

void USART3_Send_Cmd(char* cmd){
	while ( *cmd != '\0' ){
		USART_SendData(USART3,*cmd);
		while( USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET );
		cmd++;
	}
}


/*
 * 函数名：ESP8266_Cmd
 * 描述  ：对ESP8266模块发送AT指令
 * 输入  ：cmd，待发送的指令
 *         reply1，reply2，期待的响应，为NULL表不需响应，两者为或逻辑关系
 *         waittime，等待响应的时间
 * 返回  : 1，指令发送成功
 *         0，指令发送失败
 */
bool ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime )
{    
	strEsp8266_Fram_Record .InfBit .FramLength = 0;               //从新开始接收新的数据包

	USART3_Send_Cmd (cmd);

	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //不需要接收数据
		return true;
	
	delay_ms ( waittime );                 //延时
	
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';

	printf( "---------AT指令响应：%s \r\n", strEsp8266_Fram_Record .Data_RX_BUF );
  //strstr(str1,str2) 函数用于判断字符串str2是否是str1的子串
	if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) || 
						 ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) ); 
 	
	else if ( reply1 != 0 )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) );
	
	else
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) );

}

/*
 * 函数名：ESP8266_Net_Mode_Choose
 * 描述  ：选择ESP8266模块的工作模式
 * 输入  ：enumMode，工作模式
 * 返回  : 1，选择成功
 *         0，选择失败
 * 调用  ：被外部调用
 */
bool ESP8266_Net_Mode_Choose ( ENUM_Net_ModeTypeDef enumMode )
{
	switch ( enumMode )
	{
		case STA:
			return ESP8266_Cmd ( "AT+CWMODE=1\r\n", "OK", "no change", 2500 ); 
	  case AP:
		  return ESP8266_Cmd ( "AT+CWMODE=2\r\n", "OK", "no change", 2500 ); 
		case STA_AP:
		  return ESP8266_Cmd ( "AT+CWMODE=3\r\n", "OK", "no change", 2500 ); 
	  default:
		  return false;
  }
}

/*
 * 函数名：ESP8266_JoinAP
 * 描述  ：WF-ESP8266模块连接外部WiFi
 * 输入  ：pSSID，WiFi名称字符串
 *       ：pPassWord，WiFi密码字符串
 * 返回  : 1，连接成功
 *         0，连接失败
 * 调用  ：被外部调用
 */
bool ESP8266_JoinAP ( char * pSSID, char * pPassWord )
{
	char cCmd [120];

	sprintf ( cCmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", pSSID, pPassWord );
	
	return ESP8266_Cmd ( cCmd, "OK", NULL, 5000 );
}


void USART3_IRQHandler ( void )
{	
	uint8_t ucCh;
//	BEEP_ON;
//	delay_ms(500);
//	BEEP_OFF;
	//检查是否发生中断
	if ( USART_GetITStatus ( USART3, USART_IT_RXNE ) != RESET )
	{
		ucCh  = USART_ReceiveData( USART3 );
		
		if ( strEsp8266_Fram_Record .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) )                       //预留1个字节写结束符
			strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ++ ]  = ucCh;
	}
	 	 
	if ( USART_GetITStatus( USART3, USART_IT_IDLE ) == SET )                                         //数据帧接收完毕FramFinishFlag = 1设置为1
	{
    strEsp8266_Fram_Record .InfBit .FramFinishFlag = 1;
		ucCh = USART3->SR;
		ucCh = USART3->DR;
		//ucCh = USART_ReceiveData( USART3 );                                                              //由软件序列清除中断标志位(先读USART_SR，然后读USART_DR)	
		
  }	
}
