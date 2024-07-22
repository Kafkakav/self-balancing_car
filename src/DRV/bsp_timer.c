/*
********************************************************************************************************
檔名：bsp_timer.c
功  能：
備  註：
			  timer1 用於系統100ms定時功能
				timer2 用於左電機編碼器計數
				timer4 用於右電機編碼器計數

*********************************************************************************************************
*/


#include "bsp_timer.h"



//***************************定時器1初始化 系統每10ms處理一次中斷，更新數據，更新pwm等***************************//
void Timer1_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = 999;//自動重新裝載暫存器週期的值澹ㄥ計數值澹)
	TIM_TimeBaseStructure.TIM_Prescaler = 719;//時鐘分頻係數
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//對外部時鐘進行採樣的時鐘分頻
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上計數
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0; //高級定時器1是用定時器功能配置這個才可以是正常的計數頻率一開始的72mhz 值得注意的地方
	TIM_TimeBaseInit(TIM1,&TIM_TimeBaseStructure);//參數初始化

	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);

	TIM_Cmd(TIM1, ENABLE);//啟動定時器 
}


