#ifndef __BSP_TIMER3_H
#define __BSP_TIMER3_H


#include "stm32f10x.h"


#define ENCODER_TIM_PERIOD (u16)(65535)   // number of pulses per revolution
void Timer3_Init(u32 arr,int psc);
void MOTO_PWM_Init(u32 arr, int psc);
void MOTO_PWM_Out(u16 moto_r, u16 moto_l);
void Timer1_Init(void);
void Encoder_Init_TIM2(void);
void Encoder_Init_TIM4(void);
int Read_Encoder(u8 TIMX);


#endif

