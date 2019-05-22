#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "stm32f10x_spi.h"
#include "ATT.h"
#include "adc.h"
/*
//Project： XJ19M011 Ka Ku BUC
//Company:XJKJ
//Author:LocoBin
//Version:V1.0
//Description:   默认全衰   RF衰减控制io 原理图标注错误 软件修复
*/
extern float RF_buf,IF_buf;
u8 buff[5];
unsigned char spi_over=0;
u8 spi_data =0;

 int main(void)
 {	
	 delay_init();	    	 //延时函数初始化	
	 NVIC_Configuration();	 
	 delay_ms(100);
	 Adc_Init();
	 ATT_init();
	 uart_init(9600);
	 while(1)
	 {
		 Get_adcdata();
		 if(temp<-40||temp>100.0||curr>800.0||vol<23.0||vol>25.0||N_Protect==0)			//保护
			 Protect = 0;
		 else	 Protect = 1;
		 delay_ms(50);
	 }
	
}



