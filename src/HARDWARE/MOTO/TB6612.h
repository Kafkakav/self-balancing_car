#ifndef __TB6612_H
#define __TB6612_H

#include "stm32f10x.h"
#include "bsp_sys.h"

//硬體鏈接說明
//電機控制pwm,用到了timer3的ch3和ch4，即 PB0-PB1連線電機驅動的使能端PWMA（左電機）和PWMB（右電機） 
//電機正反控制：INA1和INA2控制 一路（A電機）
//	  				 INB1和INB2控制 一路（B電機）
//             PWMA ---- PB0
//             PWMB ---- PB1
//             INA2 ---- PB12
//             INA1 ---- PB13
//             INB1 ---- PB14
//             INB2 ---- PB15


#define GO    0//定義電機狀態 前進
#define BACK  1//後退
#define STOP  2//停車

/* 帶參宏，可以像行內函數一樣使用 */
#define AIN2(a)	if (a)	\
					GPIO_SetBits(GPIOB,GPIO_Pin_12);\
					else		\
					GPIO_ResetBits(GPIOB,GPIO_Pin_12)

#define AIN1(a)	if (a)	\
					GPIO_SetBits(GPIOB,GPIO_Pin_13);\
					else		\
					GPIO_ResetBits(GPIOB,GPIO_Pin_13)
					
#define BIN1(a)	if (a)	\
					GPIO_SetBits(GPIOB,GPIO_Pin_14);\
					else		\
					GPIO_ResetBits(GPIOB,GPIO_Pin_14)
					
#define BIN2(a)	if (a)	\
					GPIO_SetBits(GPIOB,GPIO_Pin_15);\
					else		\
					GPIO_ResetBits(GPIOB,GPIO_Pin_15)

void TB6612_Init(void);
void TB6612_PWM_Init(u32 arr, int psc);
void TB6612_PWM_Out(u16 moto_l, u16 moto_r);
void TB6612_GPIO_Config(void);
void Motor_A(u8 state);
void Motor_B(u8 state);
void Car_Go(void);					
void Car_Back(void);						
void Car_Left(void);
void Car_Right(void);
void Car_Stop(void);
					
#endif
