#ifndef __SPI_H
#define __SPI_H
#include "sys.h" 

#define SPI_SPEED_2   0
#define SPI_SPEED_8   1
#define SPI_SPEED_16  2
#define SPI_SPEED_256 3

#define spi_gpio					 		GPIOA 
#define spi_rcc_clk						RCC_APB2Periph_SPI1
#define spi_gbio_clk					RCC_APB2Periph_GPIOA
#define SCK 									GPIO_Pin_5
#define MISO									GPIO_Pin_6
#define MOSI									GPIO_Pin_7
#define CS										GPIO_Pin_4



extern u8 buff[5];
void SPI1_Init(void);			 //初始化SPI口
void spi_gpio_config(void);
void spi_nvic_config(void);
void SPI1_SetSpeed(u8 SpeedSet); //设置SPI速度   
u8 SPI1_ReadWriteByte(u8 TxData);//SPI总线读写一个字节
void spi_send(void);


#endif
