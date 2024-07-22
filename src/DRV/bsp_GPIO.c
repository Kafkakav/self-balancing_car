/*
********************************************************************************************************
檔名：bsp_GPIO.c
功  能：配置需要的普通IO
備  註：最好把所有的IO在這裡配置，方便管理
淘  寶：https://shop60670850.taobao.com
作  者：lilianhe
日  期: 2017-2-6
*********************************************************************************************************
*/

/*
********************************************************************************************************
GPIO更新日誌：
1.2017-2-6，編寫LED初始化程式

*********************************************************************************************************
*/

#include "bsp_GPIO.h"   

/*
********************************************************************************************************
函式名稱：void LED_GPIO_Config(void)
函式功能：初始化LED1的GPIO功能
硬體連線：LED1----PC13
備    註：
日    期: 2017-2-6
*********************************************************************************************************
*/
void LED_GPIO_Init(void)
{		
		/*定義一個GPIO_InitTypeDef型別的結構體*/
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd( LED1_CLK, ENABLE); 	/*開啟GPIO的外設時鐘*/																   
		GPIO_InitStructure.GPIO_Pin = LED1_Pin;	/*選擇要控制的GPIO引腳*/	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; /*設定引腳模式為通用推輓輸出*/   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; /*設定引腳速率為50MHz */ 
		GPIO_Init(LED1_PORT, &GPIO_InitStructure);/*呼叫庫函式，初始化GPIOC13*/	
}

//LED閃爍
void LED_Flash(int time)
{
			 static int temp;
		if(++temp==time)
		{
			GPIO_WriteBit(LED1_PORT, LED1_Pin, (BitAction) (1 - GPIO_ReadInputDataBit(LED1_PORT, LED1_Pin)))	;//LED 藍色色狀態翻轉
			temp=0;
		}
}

//------------------End of File----------------------------
