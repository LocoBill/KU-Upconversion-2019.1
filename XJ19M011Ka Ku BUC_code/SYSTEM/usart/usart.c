#include "sys.h"
#include "usart.h"	
#include "ATT.h"
#include "adc.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_UCOS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��
////////////////////////////////////////////////////////////////////////////////// 	  

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/
 

//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  

//��ʼ��IO ����1 
//bound:������
void uart_init(u32 bound){
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	  USART_InitTypeDef USART_InitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	 
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
 	  USART_DeInit(USART1);  //��λ����1
	 //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure); //��ʼ��PA9
   
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);  //��ʼ��PA10

   //Usart1 NVIC ����

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	  NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	 USART_InitStructure.USART_BaudRate = bound;//һ������Ϊ9600;
	 USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	 USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	 USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	 USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	 USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART1, &USART_InitStructure); //��ʼ������
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ��� 

}
unsigned char tab[56];   //"xx.xxI"(8)���ȱ���
unsigned char re_data[8];
unsigned char num = 0,i=0;
float RF_buf,IF_buf;
void USART1_IRQHandler(void)                	//����1�жϷ������
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
	{
		
		tab[num] =USART_ReceiveData(USART1);//(USART1->DR);	//��ȡ���յ�������
		
		if(tab[num]=='R')
		{
			RF_buf = (float)((tab[num-5]-'0')*10+(tab[num-4]-'0')+(tab[num-2]-'0')*0.1+(tab[num-1]-'0')*0.01);
			Handle_RF();
		}
		else if(tab[num]=='I')
		{
			IF_buf = (float)((tab[num-5]-'0')*10+(tab[num-4]-'0')+(tab[num-2]-'0')*0.1+(tab[num-1]-'0')*0.01);
			Handle_IF();
		}
			else if(tab[num]=='e'&&tab[num-1]=='r'&&tab[num-2]=='1'&&tab[num-3]=='0'&&tab[num-4]=='0'&&tab[num-5]=='0')
			printf("�¶ȣ�%.2lf��  ������%.2lfmA  ��ѹ��%.2lfV  \r\n",temp,curr,vol);
			num ++;
			if(num>=56)
				num = 0;
		  		 
  } 
} 

int RF_data,IF_data;

void Handle_RF(void)
{
		RF_data = (int)(RF_buf*2);
	if(RF_buf>=0.00 && RF_buf<32.00)
	{
		RF_ATT16 	= RF_data/32;
		RF_ATT8  	= (RF_data%32)/16;
		RF_ATT4  	= (RF_data%16)/8;
		RF_ATT2  	= (RF_data%8)/4;
		RF_ATT1  	= (RF_data%4)/2;
		RF_ATT0_5	= RF_data%2;
		printf("RF:%.2lf OK\r\n",RF_buf);
	}

}

void Handle_IF(void)
{
	IF_data = (int)(IF_buf*4);
	if(IF_buf>=0.00 && IF_buf<32.00) 
	{
		IF_ATT16 		= IF_data/64;
		IF_ATT8  		= (IF_data%64)/32;
		IF_ATT4 		= (IF_data%32)/16;
		IF_ATT2 		= (IF_data%16)/8;
		IF_ATT1  		= (IF_data%8)/4;
		IF_ATT0_5		= (IF_data%4)/2;
		IF_ATT0_25	=	IF_data%2;	
		printf("IF:%.2lf OK\r\n",IF_buf);
	}
}
	
