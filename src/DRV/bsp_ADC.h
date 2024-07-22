#ifndef _BSP_ADC_H
#define _BSP_ADC_H

//DRV標頭檔案
#include "stm32f10x.h"
#include "delay.h"

void ADC_Power_Init(void);
u16 Get_Adc(u8 ch) ;
u16 Get_Adc_Average(u8 ch,u8 times);
u16 Get_BAT_ADC(void);
u16 Get_SYS_ADC(void);

#endif

