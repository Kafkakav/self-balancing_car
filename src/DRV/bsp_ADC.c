/*
********************************************************************************************************
檔名：bsp_adc.c
功  能：adc電源電壓採樣
	硬體連線		STM32       TB6612
						   PB0  ----   BAT
							 PB1  ----   SYS
							 
備  註：
淘  寶：https://shop60670850.taobao.com
作  者：lilianhe
日  期: 2017-2-6
*********************************************************************************************************
*/

//PB0接電機驅動的BAT介面，採集電池電壓。分壓電阻10：1
//PB1接電機驅動的SYS介面，採集系統電壓。分壓電阻1：1

#include "bsp_adc.h"
void ADC_Power_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;	
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE );// 打開 ADC IO埠時鐘
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;// 配置 ADC IO 引腳模式// 必須為模擬輸入
	GPIO_Init(GPIOB, &GPIO_InitStructure);		
	
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1, ENABLE );	// 打開ADC時鐘
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;// ADC 模式配置	// 只使用一個ADC，屬於獨立模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ; // 多通道
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;// 連續轉換模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;// 不用外部觸發轉換，軟體開啟即可
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;// 轉換結果右對齊
	ADC_InitStructure.ADC_NbrOfChannel = 1;	// 轉換通道1個
	ADC_Init(ADC1, &ADC_InitStructure);// 初始化ADC
	
//	RCC_ADCCLKConfig(RCC_PCLK2_Div8); // 配置ADC時鐘為PCLK2的8分頻，即9MHz
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_8 | ADC_Channel_9, 1, 
//	                         ADC_SampleTime_55Cycles5);// 配置 ADC 通道轉換順序和採樣時間
//	//ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);// ADC 轉換結束產生中斷，在中斷服務程式中讀取轉換值
	ADC_Cmd(ADC1, ENABLE);// 開啟ADC ，並開始轉換 
	ADC_ResetCalibration(ADC1);// 初始化ADC 校準暫存器 
	while(ADC_GetResetCalibrationStatus(ADC1));// 等待校準暫存器初始化完成
	ADC_StartCalibration(ADC1);// ADC開始校準	
	while(ADC_GetCalibrationStatus(ADC1));// 等待校準完成 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);// 由於沒有采用外部觸發，所以使用軟體觸發ADC轉換

//  NVIC_InitTypeDef NVIC_InitStructure;
//	// 優先順序分組
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

//  // 配置中斷優先順序
//  NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQ;
//  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_Init(&NVIC_InitStructure);
}


//void ADC_IRQHandler(void)
//{	
//	if (ADC_GetITStatus(ADC1,ADC_IT_EOC)==SET) 
//	{
//		// 讀取ADC的轉換值
//		ADC_ConvertedValue = ADC_GetConversionValue(ADC1);
//	}
//	ADC_ClearITPendingBit(ADC1,ADC_IT_EOC);
//}

//獲得ADC值
//ch:通道值 0~3
u16 Get_Adc(u8 ch)   
{
  	//設定指定ADC的規則組通道，一個序列，採樣時間
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,採樣時間為239.5週期	  			    
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的軟體轉換啟動功能	
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待轉換結束
	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1規則組的轉換結果
}

u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
	}
	return temp_val/times;
}

u16 Get_BAT_ADC(void)
{
	return Get_Adc(ADC_Channel_8);
}

u16 Get_SYS_ADC(void)
{
	return Get_Adc(ADC_Channel_9);
}

