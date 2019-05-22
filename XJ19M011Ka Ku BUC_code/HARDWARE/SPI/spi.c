#include "spi.h"
#include "stm32f10x_spi.h"
#include "usart.h"
#include "led.h"

SPI_InitTypeDef  SPI_InitStructure;

void SPI1_Init(void)
{
	RCC_APB2PeriphClockCmd(	spi_gbio_clk|spi_rcc_clk, ENABLE );	
	
	spi_gpio_config();
	spi_nvic_config();
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//ѡ���˴���ʱ�ӵ���̬:ʱ�����ո�
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//���ݲ����ڵڶ���ʱ����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI1, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
	
	SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_RXNE,ENABLE);
	SPI_Cmd(SPI1, ENABLE); //ʹ��SPI����
//	SPI_I2S_ClearFlag(SPI1,RESET);
//	SPI_I2S_ClearITPendingBit(SPI1,SPI_I2S_IT_RXNE);
//		
//	SPI1_ReadWriteByte(0XFF);
}  

void spi_gpio_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin =  SCK | MOSI;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(spi_gpio, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = MISO;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(spi_gpio, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =CS;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(spi_gpio, &GPIO_InitStructure);
	GPIO_SetBits(spi_gpio,CS);
}

void spi_nvic_config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
 
}
//SPI �ٶ����ú���
//SpeedSet:
//SPI_BaudRatePrescaler_2   2��Ƶ   (SPI 36M@sys 72M)
//SPI_BaudRatePrescaler_8   8��Ƶ   (SPI 9M@sys 72M)
//SPI_BaudRatePrescaler_16  16��Ƶ  (SPI 4.5M@sys 72M)
//SPI_BaudRatePrescaler_256 256��Ƶ (SPI 281.25K@sys 72M) 
void SPI1_SetSpeed(u8 SpeedSet)
{
	SPI_InitStructure.SPI_BaudRatePrescaler = SpeedSet ;
  SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1,ENABLE);
} 

//SPIx ��дһ���ֽ�
//TxData:Ҫд����ֽ�
//����ֵ:��ȡ�����ֽ�
u8 SPI1_ReadWriteByte(u8 TxData)
{		
		u8 retry=0;				 
	while((SPI1->SR&1<<1)==0)//�ȴ���������	
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI1->DR=TxData;	 	  //����һ��byte 
	retry=0;
	while((SPI1->SR&1<<0)==0) //�ȴ�������һ��byte  
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI1->DR;          //�����յ�������				    						    
}

u8 flag=0;
void spi_send(void)
{
//	u8 crc = 0x00;
//	int i;
//	crc = (buff[1]+buff[2]) & 0x0f;
//	buff[3] = crc;

//	for(i=0;i<5;i++)
//	{
		 while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_TXE)==RESET); 
		 SPI_I2S_SendData(SPI1, 0x55);
	//	flag = SPI1_ReadWriteByte( buff[i]);		
//	}
}

u8 SPI_Res[200]={0};
unsigned char SPI_num=0;
extern unsigned char spi_over;
unsigned char spi_data_num=0;
extern u8 spi_data;

void SPI1_IRQHandler(void)
{
	//OSIntEnter();  
//	if(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_IT_RXNE)==SET)
//	{
		spi_data = SPI_I2S_ReceiveData(SPI1);
		GPIO_SetBits(spi_gpio,CS);
	  
		LED2 =!LED2;
//		SPI_Res[SPI_num] = SPI_I2S_ReceiveData(SPI1);
//		if(SPI_Res[SPI_num]==0x55&&(SPI_Res[SPI_num-4]==0xAA))
//		{
//			spi_over = 1;
//			spi_data_num = SPI_num;
//		}
//		SPI_num++;
//		if(SPI_num>199) SPI_num = 0;
//	}
	//OSIntExit();  
}
