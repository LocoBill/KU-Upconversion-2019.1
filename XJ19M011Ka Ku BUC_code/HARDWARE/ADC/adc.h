#ifndef __ADC_H
#define __ADC_H	
#include "sys.h"
#include "stm32f10x_adc.h"

#define Protect			PBout(12)
#define N_Protect		PBin(1)

typedef struct
{
	unsigned int TEMP;
	unsigned int CURRCT;
	unsigned int VOLAGE;
	unsigned int VOLAGE_N;
//	unsigned int POWER2;
}adc_dat;

extern float temp,curr,vol,vol_n;
extern adc_dat ADC_DAT;
void Adc_Init(void);
u16  Get_Adc(u8 ch); 
u16 Get_Adc_Average(u8 ch,u8 times); 
void Get_adcdata(void);
 
#endif 