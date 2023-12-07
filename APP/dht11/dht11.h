#ifndef _dht11_H
#define _dht11_H

#include "system.h"
#include "SysTick.h"

#include "stm32f10x.h"
/************************** DHT11 数据类型定义********************************/
typedef struct
{
	uint8_t  humi_int;		//湿度的整数部分
	uint8_t  humi_deci;	 	//湿度的小数部分
	uint8_t  temp_int;	 	//温度的整数部分
	uint8_t  temp_deci;	 	//温度的小数部分
	uint8_t  check_sum;	 	//校验和
} DHT11_Data_TypeDef;

/************************** DHT11 连接引脚定义********************************/
#define      macDHT11_Dout_SCK_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define      macDHT11_Dout_GPIO_CLK                       RCC_APB2Periph_GPIOG

#define      macDHT11_Dout_GPIO_PORT                      GPIOG
#define      macDHT11_Dout_GPIO_PIN                       GPIO_Pin_11

/************************** DHT11 函数宏定义********************************/
#define      macDHT11_Dout_0	                            GPIO_ResetBits ( macDHT11_Dout_GPIO_PORT, macDHT11_Dout_GPIO_PIN ) 
#define      macDHT11_Dout_1	                            GPIO_SetBits ( macDHT11_Dout_GPIO_PORT, macDHT11_Dout_GPIO_PIN ) 

#define      macDHT11_Dout_IN()	                          GPIO_ReadInputDataBit ( macDHT11_Dout_GPIO_PORT, macDHT11_Dout_GPIO_PIN ) 

/************************** DHT11 函数声明 ********************************/
uint8_t                  DHT11_Read_TempAndHumidity      ( DHT11_Data_TypeDef * DHT11_Data );


#define DHT11 (GPIO_Pin_11) //PG11
#define GPIO_DHT11 GPIOG

#define DHT11_DQ_IN PGin(11)	  //输入
#define DHT11_DQ_OUT PGout(11)  //输出


void DHT11_IO_OUT(void);
void DHT11_IO_IN(void);
u8 DHT11_Init(void);
void DHT11_Rst(void);
u8 DHT11_Check(void);
u8 DHT11_Read_Bit(void);
u8 DHT11_Read_Byte(void);
u8 DHT11_Read_Data(u8 *temp,u8 *humi);


#endif
