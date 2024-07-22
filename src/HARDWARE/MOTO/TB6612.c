/*
********************************************************************************************************
檔名：TB6612.c
功  能：配置電機驅動
備  註：
	硬體連線		STM32         TB6612
				
			TIM4-CH3-PB9  ----  PWMA(A電機)
			        PB15  ----  AIN2
			        PB14  ----  AIN1
					PB13  ----  BIN1
					PB12  ----  BIN2
			TIM4-CH4-PB8  ----  PWMB（B電機）
			
淘  寶：https://shop60670850.taobao.com
作  者：lilianhe
日  期: 2017-2-6
*********************************************************************************************************
*/



#include "tb6612.h"


//***************************TB6612配置***************************//
void TB6612_Init(void)
{
	TB6612_PWM_Init(7200, 1);//PWM頻率初始化10KHz
	TB6612_GPIO_Config();    //電機驅動IO配置
}
	

//***************************PWM頻率及占空比初始化***************************//
//=====初始化PWM 10KHZ 高頻可以防止電機低頻時的尖叫聲
// ARR= 3599 時頻率為20Khz
//STBY直接拉高
//arr：自動重灌暫存器，psc分頻係數
//PWM的頻率 = 72MHz/ARR/PCS 例如  20K = 72M/7200/1 =  10K
void TB6612_PWM_Init(u32 arr, int psc)
{
	TIM_OCInitTypeDef TIM_OCInitSructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	GPIO_InitTypeDef    GPIO_InitStructure;
	
	//配置pwm輸出埠
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8| GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		         // 複用推輓輸出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	//初始化定時器
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	TIM_TimeBaseStructure.TIM_Period = arr-1;                   //自動重新裝載暫存器週期的值澹ㄥ計數值澹)
	TIM_TimeBaseStructure.TIM_Prescaler = psc-1;                  //時鐘分頻係數
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //對外部時鐘進行採樣的時鐘分頻
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上計數
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);              //參數初始化

	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	//設定通道3 pwm參數
	TIM_OCInitSructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitSructure.TIM_OutputState= TIM_OutputState_Enable;
	TIM_OCInitSructure.TIM_Pulse = 0;                            //占空比=
	TIM_OCInitSructure.TIM_OCPolarity = TIM_OCPolarity_High;     //當定時器計數值小於CCR1_Val時為高電平
	TIM_OC3Init(TIM4, &TIM_OCInitSructure);                      //參數初始化
	TIM_OC3PolarityConfig(TIM4, TIM_OCPreload_Enable);           //開始輸出pwm

	//設定通道4 pwm參數
	TIM_OCInitSructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitSructure.TIM_OutputState= TIM_OutputState_Enable;
	TIM_OCInitSructure.TIM_Pulse = 0;                            //占空比= 
	TIM_OCInitSructure.TIM_OCPolarity = TIM_OCPolarity_High;     //當定時器計數值小於CCR1_Val時為高電平
	TIM_OC4Init(TIM4, &TIM_OCInitSructure);                      //參數初始化
	TIM_OC4PolarityConfig(TIM4, TIM_OCPreload_Enable);           //開始輸出pwm

	TIM_ARRPreloadConfig(TIM4, ENABLE);                          //啟動自動重灌
	TIM_Cmd(TIM4, ENABLE);                                       //啟動定時	
}

//***************************占空比調節***************************//
//占空比 = TIMx_CCRx / TIMx_ARR
//moto_r：右輪電機，moto_l：左輪電機.   數值 0-3600
void TB6612_PWM_Out(u16 moto_l, u16 moto_r)
{
	
	TIM_OCInitTypeDef TIM_OCInitSructure;
	
	TIM_OCInitSructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitSructure.TIM_OutputState= TIM_OutputState_Enable;
	//CH3 左電機
	TIM_OCInitSructure.TIM_Pulse = moto_l;               //占空比= ccr/3600
	TIM_OC3Init(TIM4, &TIM_OCInitSructure);              //參數初始化
	TIM_OC3PolarityConfig(TIM4, TIM_OCPreload_Enable);   //開始輸出pwm
	
	//CH4 右電機
	TIM_OCInitSructure.TIM_Pulse = moto_r;               //占空比= ccr /3600
	TIM_OC4Init(TIM4, &TIM_OCInitSructure);              //參數初始化
	TIM_OC4PolarityConfig(TIM4, TIM_OCPreload_Enable);   //開始輸出pwm
	
	TIM_ARRPreloadConfig(TIM4, ENABLE);                  //啟動自動重灌
	 
}	


void TB6612_GPIO_Config(void)
{		
		/*定義一個GPIO_InitTypeDef型別的結構體*/
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);                                	  /*開啟GPIO的外設時鐘*/																   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;	    /*選擇要控制的GPIO引腳*/	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                                          /*設定引腳模式為通用推輓輸出*/   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                                         /*設定引腳速率為50MHz */ 
		GPIO_Init(GPIOB, &GPIO_InitStructure);                                                 /*呼叫庫函式，初始化GPIO*/	
}

//以下單獨定義每個輪子運動狀態，如果不對可以修改裡面的參數
//比如左輪前進，AIN1（0），AIN2（1）.如果往後轉了，就改成AIN1（1），AIN2（0）

void Motor_A(u8 state)
{
	if(state == GO)      //左電機前進
	{
		AIN1(0);
		AIN2(1);
	}
	if(state == BACK)   //左電機後退
	{
		AIN1(1);
		AIN2(0);
	}
	if(state == STOP)  //停轉
	{
		AIN1(1);
		AIN2(1);
	}
}

void Motor_B(u8 state)
{
	if(state == GO)      //右電機前進
	{
		BIN1(0);
		BIN2(1);
	}
	if(state == BACK)   //右電機後退
	{
		BIN1(1);
		BIN2(0);
	}
	if(state == STOP)  //停轉
	{
		BIN1(1);
		BIN2(1);
	}
}

//以下定義了小車整體的運動方向。轉彎靠左右電機差速實現。

void Car_Go(void)
{
	//小車前進
	//左電機前進      右電機前進
	Motor_A(GO);   Motor_B(GO);
}

void Car_Back(void)
{
	//小車後退
	//左電機後退     右電機後退
	Motor_A(BACK);   Motor_B(BACK);
	
}

void Car_Right(void)
{
	//小車右轉圈
	//左電機前進      右電機後退
	Motor_A(GO);   Motor_B(BACK);
}

void Car_Left(void)
{
	//小車左轉圈
	//左電機後退      右電機前進
	Motor_A(BACK);   Motor_B(GO);
}

void Car_Stop(void)
{
	//小車停車
	//左電機停止      右電機停止
	Motor_A(STOP);   Motor_B(STOP);
}





