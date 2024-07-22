#ifndef __BSP_GPIO_H
#define	__BSP_GPIO_H

#include "stm32f10x.h"

/** the macro definition to trigger the led on or off 
  * 1 - off
  *0 - on
  */
#define ON  0
#define OFF 1
//配置LED1
#define LED1_CLK  RCC_APB2Periph_GPIOC
#define LED1_PORT GPIOC
#define LED1_Pin  GPIO_Pin_13


/* 帶參宏，可以像行內函數一樣使用 */
#define LED1(a)	if (a)	\
					GPIO_SetBits(LED1_PORT,LED1_Pin);\
					else		\
					GPIO_ResetBits(LED1_PORT,LED1_Pin)




///* 直接操作暫存器的方法控制IO */
//#define	digitalHi(p,i)			{p->BSRR=i;}			//設定為高電平		
//#define digitalLo(p,i)			{p->BRR=i;}				//輸出低電平
//#define digitalToggle(p,i)		{p->ODR ^=i;}			//輸出反轉狀態
//
//
///* 定義控制IO的宏 */
//#define LED1_TOGGLE		digitalToggle(GPIOB,GPIO_Pin_0)
//#define LED1_OFF		digitalHi(GPIOB,GPIO_Pin_0)
//#define LED1_ON			digitalLo(GPIOB,GPIO_Pin_0)
//
//#define LED2_TOGGLE		digitalToggle(GPIOF,GPIO_Pin_7)
//#define LED2_OFF		digitalHi(GPIOF,GPIO_Pin_7)
//#define LED2_ON			digitalLo(GPIOF,GPIO_Pin_7)
//
//#define LED3_TOGGLE		digitalToggle(GPIOF,GPIO_Pin_8)
//#define LED3_OFF		digitalHi(GPIOF,GPIO_Pin_8)
//#define LED3_ON			digitalLo(GPIOF,GPIO_Pin_8)

void LED_GPIO_Init(void);
void LED_Flash(int time);

#endif /* __LED_H */
