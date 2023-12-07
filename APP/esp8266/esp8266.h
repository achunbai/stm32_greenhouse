#ifndef _EXP8266_H
#define _EXP8266_H
#include <stdbool.h>
#include <string.h>
#include "stm32f10x.h"
#include "usart.h"
#include "SysTick.h"

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

#define RX_BUF_MAX_LEN     1024    //最大接收缓存字节数
//串口数据帧的处理结构体
extern struct  STRUCT_USARTx_Fram                                  
{
	char  Data_RX_BUF [ RX_BUF_MAX_LEN ];
	
  union {
     u16 InfAll;
    struct {
		   u16 FramLength       :15;                               // 14:0 
		   u16 FramFinishFlag   :1;                                // 15 
	  } InfBit;
  };
	
} strEsp8266_Fram_Record;
//枚举，ESP8266的工作模式的种类
typedef enum{
	STA,
  AP,
  STA_AP  
} ENUM_Net_ModeTypeDef;

void ESP8266_Init(void);
void ESP8266_Gpio_Init(void);
void USART3_Send_Cmd(char* cmd);
bool ESP8266_Cmd ( char * cmd, char * reply1, char * reply2, u32 waittime );
bool ESP8266_Net_Mode_Choose ( ENUM_Net_ModeTypeDef enumMode );
bool ESP8266_JoinAP ( char * pSSID, char * pPassWord );
#endif
