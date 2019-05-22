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
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//选择了串行时钟的稳态:时钟悬空高
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//数据捕获于第二个时钟沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	
	SPI_I2S_ITConfig(SPI1,SPI_I2S_IT_RXNE,ENABLE);
	SPI_Cmd(SPI1, ENABLE); //使能SPI外设
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
 
}
//SPI 速度设置函数
//SpeedSet:
//SPI_BaudRatePrescaler_2   2分频   (SPI 36M@sys 72M)
//SPI_BaudRatePrescaler_8   8分频   (SPI 9M@sys 72M)
//SPI_BaudRatePrescaler_16  16分频  (SPI 4.5M@sys 72M)
//SPI_BaudRatePrescaler_256 256分频 (SPI 281.25K@sys 72M) 
void SPI1_SetSpeed(u8 SpeedSet)
{
	SPI_InitStructure.SPI_BaudRatePrescaler = SpeedSet ;
  SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1,ENABLE);
} 

//SPIx 读写一个字节
//TxData:要写入的字节
//返回值:读取到的字节
u8 SPI1_ReadWriteByte(u8 TxData)
{		
		u8 retry=0;				 
	while((SPI1->SR&1<<1)==0)//等待发送区空	
	{
		retry++;
		if(retry>200)return 0;
	}			  
	SPI1->DR=TxData;	 	  //发送一个byte 
	retry=0;
	while((SPI1->SR&1<<0)==0) //等待接收完一个byte  
	{
		retry++;
		if(retry>200)return 0;
	}	  						    
	return SPI1->DR;          //返回收到的数据				    						    
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
