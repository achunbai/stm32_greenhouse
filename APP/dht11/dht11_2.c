#include "dht11_2.h"
#include "SysTick.h"

static void                           DHT11_2_GPIO_Config                       ( void );
static void                           DHT11_2_Mode_IPU                          ( void );
static void                           DHT11_2_Mode_Out_PP                       ( void );
static uint8_t                        DHT11_2_ReadByte                          ( void );

//DHT11初始化 
//返回0：初始化成功，1：失败
u8 DHT11_2_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);

	GPIO_InitStructure.GPIO_Pin=DHT11_2;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIO_DHT11_2,&GPIO_InitStructure);
	GPIO_SetBits(GPIO_DHT11_2,DHT11_2);	   //拉高

	DHT11_2_Rst();	  
	return DHT11_2_Check();	
}

//配置DHT11用到的I/O口
static void DHT11_2_GPIO_Config ( void )
{		
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*开启macDHT11_2_Dout_GPIO_PORT的外设时钟*/
  macDHT11_2_Dout_SCK_APBxClock_FUN ( macDHT11_2_Dout_GPIO_CLK, ENABLE );	

	/*选择要控制的macDHT11_2_Dout_GPIO_PORT引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = macDHT11_2_Dout_GPIO_PIN;	

	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*调用库函数，初始化macDHT11_2_Dout_GPIO_PORT*/
  	GPIO_Init ( macDHT11_2_Dout_GPIO_PORT, &GPIO_InitStructure );		  
	
}


//使DHT11-DATA引脚变为上拉输入模式
static void DHT11_2_Mode_IPU(void)
{
 	  GPIO_InitTypeDef GPIO_InitStructure;

	  	/*选择要控制的macDHT11_2_Dout_GPIO_PORT引脚*/	
	  GPIO_InitStructure.GPIO_Pin = macDHT11_2_Dout_GPIO_PIN;

	   /*设置引脚模式为浮空输入模式*/ 
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 

	  /*调用库函数，初始化macDHT11_2_Dout_GPIO_PORT*/
	  GPIO_Init(macDHT11_2_Dout_GPIO_PORT, &GPIO_InitStructure);	 
	
}


//使DHT11-DATA引脚变为推挽输出模式
static void DHT11_2_Mode_Out_PP(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;

	 	/*选择要控制的macDHT11_2_Dout_GPIO_PORT引脚*/															   
  	GPIO_InitStructure.GPIO_Pin = macDHT11_2_Dout_GPIO_PIN;	

	/*设置引脚模式为通用推挽输出*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	/*设置引脚速率为50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	/*调用库函数，初始化macDHT11_2_Dout_GPIO_PORT*/
  	GPIO_Init(macDHT11_2_Dout_GPIO_PORT, &GPIO_InitStructure);	 	 
	
}


//从DHT11读取一个字节，MSB先行
static uint8_t DHT11_2_ReadByte ( void )
{
	uint8_t i, temp=0;
	

	for(i=0;i<8;i++)    
	{	 
		/*每bit以50us低电平标置开始，轮询直到从机发出 的50us 低电平 结束*/  
		while(macDHT11_2_Dout_IN()==Bit_RESET);

		/*DHT11 以26~28us的高电平表示“0”，以70us高电平表示“1”，
		 *通过检测 x us后的电平即可区别这两个状 ，x 即下面的延时 
		 */
		delay_us(40); //延时x us 这个延时需要大于数据0持续的时间即可	   	  

		if(macDHT11_2_Dout_IN()==Bit_SET)/* x us后仍为高电平表示数据“1” */
		{
			/* 等待数据1的高电平结束 */
			while(macDHT11_2_Dout_IN()==Bit_SET);

			temp|=(uint8_t)(0x01<<(7-i));  //把第7-i位置1，MSB先行 
		}
		else	 // x us后为低电平表示数据“0”
		{			   
			temp&=(uint8_t)~(0x01<<(7-i)); //把第7-i位置0，MSB先行
		}
	}
	
	return temp;
	
}


/*
 * 一次完整的数据传输为40bit，高位先出
 * 8bit 湿度整数 + 8bit 湿度小数 + 8bit 温度整数 + 8bit 温度小数 + 8bit 校验和 
 */
uint8_t DHT11_2_Read_TempAndHumidity(DHT11_2_Data_TypeDef *DHT11_2_Data)
{  
	/*输出模式*/
	DHT11_2_Mode_Out_PP();
	/*主机拉低*/
	macDHT11_2_Dout_0;
	/*延时18ms*/
	delay_ms(18);

	/*总线拉高 主机延时30us*/
	macDHT11_2_Dout_1; 

	delay_us(30);   //延时30us

	/*主机设为输入 判断从机响应信号*/ 
	DHT11_2_Mode_IPU();

	/*判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行*/   
	if(macDHT11_2_Dout_IN()==Bit_RESET)     
	{
		/*轮询直到从机发出 的80us 低电平 响应信号结束*/  
		while(macDHT11_2_Dout_IN()==Bit_RESET);

		/*轮询直到从机发出的 80us 高电平 标置信号结束*/
		while(macDHT11_2_Dout_IN()==Bit_SET);

		/*开始接收数据*/   
		DHT11_2_Data->humi_int= DHT11_2_ReadByte();

		DHT11_2_Data->humi_deci= DHT11_2_ReadByte();

		DHT11_2_Data->temp_int= DHT11_2_ReadByte();

		DHT11_2_Data->temp_deci= DHT11_2_ReadByte();

		DHT11_2_Data->check_sum= DHT11_2_ReadByte();


		/*读取结束，引脚改为输出模式*/
		DHT11_2_Mode_Out_PP();
		/*主机拉高*/
		macDHT11_2_Dout_1;

		/*检查读取的数据是否正确*/
		if(DHT11_2_Data->check_sum == DHT11_2_Data->humi_int + DHT11_2_Data->humi_deci + DHT11_2_Data->temp_int+ DHT11_2_Data->temp_deci)
			return SUCCESS;
		else 
			return ERROR;
	}
	
	else
		return ERROR;
}

//复位DHT11
void DHT11_2_Rst()	   
{                 
	DHT11_2_IO_OUT(); 	//SET OUTPUT
    DHT11_2_DQ_OUT=0; 	//拉低DQ
    delay_ms(20);    	//拉低至少18ms
    DHT11_2_DQ_OUT=1; 	//DQ=1 
	delay_us(30);     	//主机拉高20~40us
}

//等待DHT11的回应
//返回1:未检测到DHT11的存在
//返回0:存在
u8 DHT11_2_Check() 	   
{   
	u8 retry=0;
	DHT11_2_IO_IN();//SET INPUT	 
    while (DHT11_2_DQ_IN&&retry<100)//DHT11会拉低40~50us
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=100)return 1;
	else retry=0;
    while (!DHT11_2_DQ_IN&&retry<100)//DHT11拉低后会再次拉高40~50us
	{
		retry++;
		delay_us(1);
	};
	if(retry>=100)return 1;	    
	return 0;
}

//从DHT11读取一个位
//返回值：1/0
u8 DHT11_2_Read_Bit(void) 			 
{
 	u8 retry=0;
	while(DHT11_2_DQ_IN&&retry<100)//等待变为低电平 12-14us 开始
	{
		retry++;
		delay_us(1);
	}
	retry=0;
	while(!DHT11_2_DQ_IN&&retry<100)//等待变高电平	 26-28us表示0,116-118us表示1
	{
		retry++;
		delay_us(1);
	}
	delay_us(40);//等待40us
	if(DHT11_2_DQ_IN)return 1;
	else return 0;		   
}

//从DHT11读取一个字节
//返回值：读到的数据
u8 DHT11_2_Read_Byte(void)    
{        
    u8 i,dat;
    dat=0;
	for (i=0;i<8;i++) 
	{
   		dat<<=1; 
	    dat|=DHT11_2_Read_Bit();
    }						    
    return dat;
}

//从DHT11读取一次数据
//temp:温度值(范围:0~50°)
//humi:湿度值(范围:20%~90%)
//返回值：0,正常;1,读取失败
u8 DHT11_2_Read_Data(u8 *temp,u8 *humi)    
{        
 	u8 buf[5];
	u8 i;
	DHT11_2_Rst();
	if(DHT11_2_Check()==0)
	{
		for(i=0;i<5;i++)//读取40位数据
		{
			buf[i]=DHT11_2_Read_Byte();
		}
		if((buf[0]+buf[1]+buf[2]+buf[3])==buf[4])
		{
			*humi=buf[0];
			*temp=buf[2];
		}
		
	}else return 1;
	return 0;	    
}

//DHT11输出模式配置
void DHT11_2_IO_OUT()	
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=DHT11_2;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIO_DHT11_2,&GPIO_InitStructure);	
}

//DHT11输入模式配置
void DHT11_2_IO_IN()	
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=DHT11_2;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;	 //上拉输入模式
	GPIO_Init(GPIO_DHT11_2,&GPIO_InitStructure);	
}





