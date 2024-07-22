#ifndef __ENCODER_H
#define __ENCODER_H

#include "stm32f10x.h"
#include "bsp_sys.h"

void Encoder_Init_TIM2(void);
void Encoder_Init_TIM3(void);
int Read_Encoder(u8 TIMX);

#endif
