#include "c8051f330.h"
#define SYSCLK     24000000        // SYSCLK frequency in Hz

sbit ADF4002_LE = P1^2;
sbit ADF4002_DATA = P1^1;
sbit ADF4002_CLK = P1^0 ;
sbit ADF4002_LD = P1^3 ;

void SYSCLK_Init (void)
{
   OSCICN = 0x83;
  // RSTSRC = 0x04;          
}

void delay_us(int t)
{
	unsigned int x,y;
	for(x=t;x>0;x--)
		for(y=110;y>0;y--);
}


void write_ADF4002_24_resister(unsigned long addr_data)
 {
	unsigned char i;
	unsigned long reg_data_value;
	ADF4002_LE = 1;
	delay_us(5);
	ADF4002_LE = 0;
	delay_us(5);
	reg_data_value = addr_data;
	for(i = 0; i < 24; i++)
	{	
		if((0x800000 & reg_data_value) == 0x800000)
		{
			ADF4002_DATA = 1;				
		}
		else
		{
			ADF4002_DATA = 0;
		}
		delay_us(1);
		ADF4002_CLK = 1;
		delay_us(1);
		ADF4002_CLK = 0;
		delay_us(1);
		reg_data_value <<= 1;
	}
	delay_us(5);
	ADF4002_LE = 1;
	delay_us(5);
 }
void ADF4002()
 {

	write_ADF4002_24_resister(0xD8092);		//10 Function Latch 输出参数重要 R分频输出	 
	write_ADF4002_24_resister(0xD8093); //11 Initialization Latch
 	write_ADF4002_24_resister(0x4); 	 //00 R COUNTER  
	write_ADF4002_24_resister(0xA01);   //01 N=10 COUNTER B =6  A =4
	
 }

void PORT_Init_4002(void)
{
   XBR0     = 0x00;    //将SPI的I/O连接到端口引脚
   XBR1     = 0x40;    //交叉开关使能
   P1MDOUT |= 0xE0;        //或运算，确保低四位为1，高四位维持原来的 
  // P1MDOUT |= 0x01;   //P1^3,4,5 
}

void main()
{
	PCA0MD &= ~0x40;   //关看门狗
	SYSCLK_Init();
	PORT_Init_4002();
	delay_us(50000);
	delay_us(50000);

	ADF4002();
	ADF4002();
	while(1)
	{	 
//		if(ADF4002_LD == 0)
//		{
//			delay_us(10);
//		   	if(ADF4002_LD == 0)
//			{
//
//				write_ADF4002_24_resister(0xD809a);		//10 Function Latch 输出参数重要 R分频输出	 
//				write_ADF4002_24_resister(0xD809a); //11 Initialization Latch
//			 	write_ADF4002_24_resister(0x100004); 	 //00 R COUNTER  
//				write_ADF4002_24_resister(0xA01);   //01 N COUNTER B =6  A =4				
//							
//				delay_us(50000);
//				
//				write_ADF4002_24_resister(0xD8092);		//10 Function Latch 输出参数重要 R分频输出	 
//				write_ADF4002_24_resister(0xD8093); //11 Initialization Latch
//			 	write_ADF4002_24_resister(0x100004); 	 //00 R COUNTER  
//				write_ADF4002_24_resister(0xA01);   //01 N COUNTER B =6  A =4	
//				
//				delay_us(50000);			
															
//			}
//
//		}
	//	OSCICN = 0x00;
	}
}



	
	

