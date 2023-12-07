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
/*��ʼ��ESP8266�õ���GPIO����*/
void ESP8266_Gpio_Init(void){
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/* ���� CH_PD ���� PA4, ʹ�����ţ��ߵ�ƽ��Ч*/
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE ); 					   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init ( GPIOA, & GPIO_InitStructure );	 
	
	/* ���� RST ���� PA15���͵�ƽ��λ */
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_GPIOA, ENABLE ); 			   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	
	GPIO_Init ( GPIOA, & GPIO_InitStructure );

}

//����WF-ESP8266ģ��
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
 * ��������ESP8266_Cmd
 * ����  ����ESP8266ģ�鷢��ATָ��
 * ����  ��cmd�������͵�ָ��
 *         reply1��reply2���ڴ�����Ӧ��ΪNULL������Ӧ������Ϊ���߼���ϵ
 *         waittime���ȴ���Ӧ��ʱ��
 * ����  : 1��ָ��ͳɹ�
 *         0��ָ���ʧ��
 */
bool ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime )
{    
	strEsp8266_Fram_Record .InfBit .FramLength = 0;               //���¿�ʼ�����µ����ݰ�

	USART3_Send_Cmd (cmd);

	if ( ( reply1 == 0 ) && ( reply2 == 0 ) )                      //����Ҫ��������
		return true;
	
	delay_ms ( waittime );                 //��ʱ
	
	strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ]  = '\0';

	printf( "---------ATָ����Ӧ��%s \r\n", strEsp8266_Fram_Record .Data_RX_BUF );
  //strstr(str1,str2) ���������ж��ַ���str2�Ƿ���str1���Ӵ�
	if ( ( reply1 != 0 ) && ( reply2 != 0 ) )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) || 
						 ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) ); 
 	
	else if ( reply1 != 0 )
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply1 ) );
	
	else
		return ( ( bool ) strstr ( strEsp8266_Fram_Record .Data_RX_BUF, reply2 ) );

}

/*
 * ��������ESP8266_Net_Mode_Choose
 * ����  ��ѡ��ESP8266ģ��Ĺ���ģʽ
 * ����  ��enumMode������ģʽ
 * ����  : 1��ѡ��ɹ�
 *         0��ѡ��ʧ��
 * ����  �����ⲿ����
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
 * ��������ESP8266_JoinAP
 * ����  ��WF-ESP8266ģ�������ⲿWiFi
 * ����  ��pSSID��WiFi�����ַ���
 *       ��pPassWord��WiFi�����ַ���
 * ����  : 1�����ӳɹ�
 *         0������ʧ��
 * ����  �����ⲿ����
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
	//����Ƿ����ж�
	if ( USART_GetITStatus ( USART3, USART_IT_RXNE ) != RESET )
	{
		ucCh  = USART_ReceiveData( USART3 );
		
		if ( strEsp8266_Fram_Record .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) )                       //Ԥ��1���ֽ�д������
			strEsp8266_Fram_Record .Data_RX_BUF [ strEsp8266_Fram_Record .InfBit .FramLength ++ ]  = ucCh;
	}
	 	 
	if ( USART_GetITStatus( USART3, USART_IT_IDLE ) == SET )                                         //����֡�������FramFinishFlag = 1����Ϊ1
	{
    strEsp8266_Fram_Record .InfBit .FramFinishFlag = 1;
		ucCh = USART3->SR;
		ucCh = USART3->DR;
		//ucCh = USART_ReceiveData( USART3 );                                                              //�������������жϱ�־λ(�ȶ�USART_SR��Ȼ���USART_DR)	
		
  }	
}
