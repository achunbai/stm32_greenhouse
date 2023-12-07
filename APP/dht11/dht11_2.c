#include "dht11_2.h"
#include "SysTick.h"

static void                           DHT11_2_GPIO_Config                       ( void );
static void                           DHT11_2_Mode_IPU                          ( void );
static void                           DHT11_2_Mode_Out_PP                       ( void );
static uint8_t                        DHT11_2_ReadByte                          ( void );

//DHT11��ʼ�� 
//����0����ʼ���ɹ���1��ʧ��
u8 DHT11_2_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOG,ENABLE);

	GPIO_InitStructure.GPIO_Pin=DHT11_2;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIO_DHT11_2,&GPIO_InitStructure);
	GPIO_SetBits(GPIO_DHT11_2,DHT11_2);	   //����

	DHT11_2_Rst();	  
	return DHT11_2_Check();	
}

//����DHT11�õ���I/O��
static void DHT11_2_GPIO_Config ( void )
{		
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*����macDHT11_2_Dout_GPIO_PORT������ʱ��*/
  macDHT11_2_Dout_SCK_APBxClock_FUN ( macDHT11_2_Dout_GPIO_CLK, ENABLE );	

	/*ѡ��Ҫ���Ƶ�macDHT11_2_Dout_GPIO_PORT����*/															   
  	GPIO_InitStructure.GPIO_Pin = macDHT11_2_Dout_GPIO_PIN;	

	/*��������ģʽΪͨ���������*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	/*������������Ϊ50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*���ÿ⺯������ʼ��macDHT11_2_Dout_GPIO_PORT*/
  	GPIO_Init ( macDHT11_2_Dout_GPIO_PORT, &GPIO_InitStructure );		  
	
}


//ʹDHT11-DATA���ű�Ϊ��������ģʽ
static void DHT11_2_Mode_IPU(void)
{
 	  GPIO_InitTypeDef GPIO_InitStructure;

	  	/*ѡ��Ҫ���Ƶ�macDHT11_2_Dout_GPIO_PORT����*/	
	  GPIO_InitStructure.GPIO_Pin = macDHT11_2_Dout_GPIO_PIN;

	   /*��������ģʽΪ��������ģʽ*/ 
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ; 

	  /*���ÿ⺯������ʼ��macDHT11_2_Dout_GPIO_PORT*/
	  GPIO_Init(macDHT11_2_Dout_GPIO_PORT, &GPIO_InitStructure);	 
	
}


//ʹDHT11-DATA���ű�Ϊ�������ģʽ
static void DHT11_2_Mode_Out_PP(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;

	 	/*ѡ��Ҫ���Ƶ�macDHT11_2_Dout_GPIO_PORT����*/															   
  	GPIO_InitStructure.GPIO_Pin = macDHT11_2_Dout_GPIO_PIN;	

	/*��������ģʽΪͨ���������*/
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	/*������������Ϊ50MHz */   
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	/*���ÿ⺯������ʼ��macDHT11_2_Dout_GPIO_PORT*/
  	GPIO_Init(macDHT11_2_Dout_GPIO_PORT, &GPIO_InitStructure);	 	 
	
}


//��DHT11��ȡһ���ֽڣ�MSB����
static uint8_t DHT11_2_ReadByte ( void )
{
	uint8_t i, temp=0;
	

	for(i=0;i<8;i++)    
	{	 
		/*ÿbit��50us�͵�ƽ���ÿ�ʼ����ѯֱ���ӻ����� ��50us �͵�ƽ ����*/  
		while(macDHT11_2_Dout_IN()==Bit_RESET);

		/*DHT11 ��26~28us�ĸߵ�ƽ��ʾ��0������70us�ߵ�ƽ��ʾ��1����
		 *ͨ����� x us��ĵ�ƽ��������������״ ��x ���������ʱ 
		 */
		delay_us(40); //��ʱx us �����ʱ��Ҫ��������0������ʱ�伴��	   	  

		if(macDHT11_2_Dout_IN()==Bit_SET)/* x us����Ϊ�ߵ�ƽ��ʾ���ݡ�1�� */
		{
			/* �ȴ�����1�ĸߵ�ƽ���� */
			while(macDHT11_2_Dout_IN()==Bit_SET);

			temp|=(uint8_t)(0x01<<(7-i));  //�ѵ�7-iλ��1��MSB���� 
		}
		else	 // x us��Ϊ�͵�ƽ��ʾ���ݡ�0��
		{			   
			temp&=(uint8_t)~(0x01<<(7-i)); //�ѵ�7-iλ��0��MSB����
		}
	}
	
	return temp;
	
}


/*
 * һ�����������ݴ���Ϊ40bit����λ�ȳ�
 * 8bit ʪ������ + 8bit ʪ��С�� + 8bit �¶����� + 8bit �¶�С�� + 8bit У��� 
 */
uint8_t DHT11_2_Read_TempAndHumidity(DHT11_2_Data_TypeDef *DHT11_2_Data)
{  
	/*���ģʽ*/
	DHT11_2_Mode_Out_PP();
	/*��������*/
	macDHT11_2_Dout_0;
	/*��ʱ18ms*/
	delay_ms(18);

	/*�������� ������ʱ30us*/
	macDHT11_2_Dout_1; 

	delay_us(30);   //��ʱ30us

	/*������Ϊ���� �жϴӻ���Ӧ�ź�*/ 
	DHT11_2_Mode_IPU();

	/*�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������*/   
	if(macDHT11_2_Dout_IN()==Bit_RESET)     
	{
		/*��ѯֱ���ӻ����� ��80us �͵�ƽ ��Ӧ�źŽ���*/  
		while(macDHT11_2_Dout_IN()==Bit_RESET);

		/*��ѯֱ���ӻ������� 80us �ߵ�ƽ �����źŽ���*/
		while(macDHT11_2_Dout_IN()==Bit_SET);

		/*��ʼ��������*/   
		DHT11_2_Data->humi_int= DHT11_2_ReadByte();

		DHT11_2_Data->humi_deci= DHT11_2_ReadByte();

		DHT11_2_Data->temp_int= DHT11_2_ReadByte();

		DHT11_2_Data->temp_deci= DHT11_2_ReadByte();

		DHT11_2_Data->check_sum= DHT11_2_ReadByte();


		/*��ȡ���������Ÿ�Ϊ���ģʽ*/
		DHT11_2_Mode_Out_PP();
		/*��������*/
		macDHT11_2_Dout_1;

		/*����ȡ�������Ƿ���ȷ*/
		if(DHT11_2_Data->check_sum == DHT11_2_Data->humi_int + DHT11_2_Data->humi_deci + DHT11_2_Data->temp_int+ DHT11_2_Data->temp_deci)
			return SUCCESS;
		else 
			return ERROR;
	}
	
	else
		return ERROR;
}

//��λDHT11
void DHT11_2_Rst()	   
{                 
	DHT11_2_IO_OUT(); 	//SET OUTPUT
    DHT11_2_DQ_OUT=0; 	//����DQ
    delay_ms(20);    	//��������18ms
    DHT11_2_DQ_OUT=1; 	//DQ=1 
	delay_us(30);     	//��������20~40us
}

//�ȴ�DHT11�Ļ�Ӧ
//����1:δ��⵽DHT11�Ĵ���
//����0:����
u8 DHT11_2_Check() 	   
{   
	u8 retry=0;
	DHT11_2_IO_IN();//SET INPUT	 
    while (DHT11_2_DQ_IN&&retry<100)//DHT11������40~50us
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=100)return 1;
	else retry=0;
    while (!DHT11_2_DQ_IN&&retry<100)//DHT11���ͺ���ٴ�����40~50us
	{
		retry++;
		delay_us(1);
	};
	if(retry>=100)return 1;	    
	return 0;
}

//��DHT11��ȡһ��λ
//����ֵ��1/0
u8 DHT11_2_Read_Bit(void) 			 
{
 	u8 retry=0;
	while(DHT11_2_DQ_IN&&retry<100)//�ȴ���Ϊ�͵�ƽ 12-14us ��ʼ
	{
		retry++;
		delay_us(1);
	}
	retry=0;
	while(!DHT11_2_DQ_IN&&retry<100)//�ȴ���ߵ�ƽ	 26-28us��ʾ0,116-118us��ʾ1
	{
		retry++;
		delay_us(1);
	}
	delay_us(40);//�ȴ�40us
	if(DHT11_2_DQ_IN)return 1;
	else return 0;		   
}

//��DHT11��ȡһ���ֽ�
//����ֵ������������
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

//��DHT11��ȡһ������
//temp:�¶�ֵ(��Χ:0~50��)
//humi:ʪ��ֵ(��Χ:20%~90%)
//����ֵ��0,����;1,��ȡʧ��
u8 DHT11_2_Read_Data(u8 *temp,u8 *humi)    
{        
 	u8 buf[5];
	u8 i;
	DHT11_2_Rst();
	if(DHT11_2_Check()==0)
	{
		for(i=0;i<5;i++)//��ȡ40λ����
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

//DHT11���ģʽ����
void DHT11_2_IO_OUT()	
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=DHT11_2;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	 //�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIO_DHT11_2,&GPIO_InitStructure);	
}

//DHT11����ģʽ����
void DHT11_2_IO_IN()	
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin=DHT11_2;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;	 //��������ģʽ
	GPIO_Init(GPIO_DHT11_2,&GPIO_InitStructure);	
}





