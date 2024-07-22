/*
********************************************************************************************************
檔名：bsp_exti.c
功  能：外部中斷 PB2
備  註：MPU6050的INT介面鏈接PB2，由於PB2是boot1引腳，如果除錯不進中斷，可以先拔除 核心板上的boot1跳線冒
淘  寶：https://shop60670850.taobao.com
作  者：lilianhe
日  期: 2017-2-6
*********************************************************************************************************
*/

#include "bsp_exti.h"

void Exti_PB2_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	
  // 開啟時鐘
  // 外部中斷，需要使能AFIO時鐘
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;          // PB2配置為上拉輸入
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruct);	
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource2);  // 外部中斷和GPIO對映起來
	
	EXTI_InitStruct.EXTI_Line=EXTI_Line2;
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;    // 中斷觸發
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;// MPU6050發生中斷時會有一個下降沿
	EXTI_Init(&EXTI_InitStruct);
}
