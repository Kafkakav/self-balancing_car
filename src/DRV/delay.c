/*
********************************************************************************************************
檔名：delay.c
功  能：延時函式us ms
備  註：
淘  寶：https://shop60670850.taobao.com
作  者：lilianhe
日  期: 2017-2-6
*********************************************************************************************************
*/

/*
********************************************************************************************************
更新日誌：
1.2017-2-6，修正了中斷中呼叫出現死循環的錯誤，防止延時不準確,採用do while結構!

*********************************************************************************************************
*/

#include "delay.h"

static u8  fac_us=0;//us延時倍乘數
static u16 fac_ms=0;//ms延時倍乘數

/*
********************************************************************************************************
函式名稱：Delay_Init(u8 SYSCLK)
函式功能：初始化延遲函式
硬體連線：SYSTICK的時鐘固定為HCLK時鐘的1/8，SYSCLK:系統時鐘
備    註：
日    期:  2017-2-6
*********************************************************************************************************
*/
void Delay_Init(u8 SYSCLK)
{
//	SysTick->CTRL&=0xfffffffb;//bit2清空,選擇外部時鐘  HCLK/8
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//選擇外部時鐘  HCLK/8
	fac_us=SYSCLK/8;
	fac_ms=(u16)fac_us*1000;
}								    

/*
********************************************************************************************************
函式名稱：Delay_ms(u16 nms)
函式功能：延時nms
硬體連線：//SysTick->LOAD為24位暫存器,所以,最大延時為:
					//nms<=0xffffff*8*1000/SYSCLK
					//SYSCLK單位為Hz,nms單位為ms
					//對72M條件下,nms<=1864 
備    註：
日    期: 2017-2-6
*********************************************************************************************************
*/
void Delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;//時間載入(SysTick->LOAD為24bit)
	SysTick->VAL =0x00;           //清空計數器
	SysTick->CTRL=0x01 ;          //開始倒數  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待時間到達   
	SysTick->CTRL=0x00;       //關閉計數器
	SysTick->VAL =0X00;       //清空計數器	  	    
}   

/*
********************************************************************************************************
函式名稱：void Delay_us(u32 nus)
函式功能：延時nus
硬體連線：
備    注:
日    期: 2017-2-6
*********************************************************************************************************
*/   								   
void Delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; //時間載入	  		 
	SysTick->VAL=0x00;        //清空計數器
	SysTick->CTRL=0x01 ;      //開始倒數 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待時間到達   
	SysTick->CTRL=0x00;       //關閉計數器
	SysTick->VAL =0X00;       //清空計數器	 
}
//------------------End of File----------------------------




