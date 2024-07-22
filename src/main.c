/*
********************************************************************************************************
檔名：main.c
功  能：
備  註：
淘  寶：https://shop60670850.taobao.com
作  者：lilianhe
日  期: 2017-2-6
*********************************************************************************************************
*/

/*
********************************************************************************************************
更新日誌：
1.2017-2-8，增加EEPROM測試函式
2.2017-2-8，增加uart測試函式
3.2017-2-8，增加oled測試函式
*********************************************************************************************************
*/
#include "bsp_sys.h"
#include "stdio.h"


extern u8 OLED_ADC_Flag;
extern u8 OLED_JIAODU_Flag;

extern u16 ADC_BAT_Val,ADC_BAT_temp;
extern u16 ADC_SYS_Val,ADC_SYS_temp;
extern int Encoder_A,Encoder_B;

extern float Car_Angle_Balance,Angle_Balance,Gyro_Balance,Gyro_Turn; //平衡傾角 平衡陀螺儀 轉向陀螺儀
u8 Flag_Qian,Flag_Hou,Flag_Left,Flag_Right,Flag_sudu=2; //藍芽遙控相關的變數
unsigned char RX_Buf[32];

/**
  * @brief  主函式
  * @param  無  
  * @retval 無
  */
int main(void)
{	
//***************************設定區域性變數***************************//

	
//*****************************系統初始*****************************//
	
	STM32_System_Init();//所有系統配置在這個函式里完成
	
	while (1)
	{

		if(OLED_ADC_Flag==1)//電壓1S顯示一次
		{
			OLED_ADC_Flag=0;
      //OLED display move to car.c - EXTI2_IRQHandler
		}
			
	}
	
}	

/*********************************************END OF FILE**********************/
