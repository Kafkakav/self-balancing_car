/*
********************************************************************************************************
檔名：bsp_NVIC.c
功  能：中斷配置
備  註：
淘  寶：https://shop60670850.taobao.com
作  者：lilianhe
日  期: 2017-2-6
*********************************************************************************************************
*/

/*
********************************************************************************************************
更新日誌：
1.2017-2-6,編寫uart和time中斷


*********************************************************************************************************
*/

#include "bsp_NVIC.h"

/*
********************************************************************************************************
函式名稱：void NVIC_Config(void)
函式功能：配置各個中斷的優先順序
硬體連線：
備    註：
日    期: 2017-2-6
*********************************************************************************************************
*/
void NVIC_Config(void)
{
	NVIC_TIM1_Config();
	NVIC_Exti_PB2_Config();
}

//配置timer1的中斷配置
void NVIC_TIM1_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel=TIM1_UP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void NVIC_Exti_PB2_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//
	
	//外部中斷
	NVIC_InitStruct.NVIC_IRQChannel=EXTI2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	NVIC_Init(&NVIC_InitStruct);

}

//------------------End of File----------------------------
