/*
********************************************************************************************************
檔名：bsp_sys.c
功  能：初始化所有外設
備  註：
淘  寶：https://shop60670850.taobao.com
作  者：lilianhe
日  期: 2017-2-6
*********************************************************************************************************
*/

#include "bsp_sys.h"
#include "bsp_usart.h"

void STM32_System_Init(void)
{	
	//STM32內部初始化
	Delay_Init(72);        //滴答時鐘初始化
	NVIC_Config();         //中斷配置配置初始化
	LED_GPIO_Init();       //LED初始化
	USART1_Init(115200);   //串列埠初始化
	printf("=>NVIC/I2C/GPIO\n");
	IIC_Init();            //IIC模擬初始化
	ADC_Power_Init();	   //ADC電壓採樣

#if 1
    printf("=>OLED\n");
	OLED_Init();
	OLED_Clear();
	OLED_ShowString(0,0,"B:    V , ",12);
	OLED_ShowString(80,0,"S:   V",12);
	OLED_ShowString(0,1,"A:",12);
	OLED_ShowString(0,2,"B:",12);
	OLED_ShowString(0,3,"Angle:",12);
	OLED_ShowString(0,4,"MCW:",12);
#endif

	printf("=>Encoder\n");
	Encoder_Init_TIM2();
	Encoder_Init_TIM3();
	printf("=>MPU\n");
	MPU_Init();
	printf("=>DMP\n");
	mpu_dmp_init();
	printf("=>BT\n");
	Bluetooth_Init(9600);
	printf("=>Exti\n");
	//Timer1_Init();//最後啟動定時器1
	Exti_PB2_Init();       //MPU6050 INT
	printf("=>TB6612\n");
	TB6612_Init();//電機驅動

	printf("SYS Started\n");
	Delay_ms(100);          //等待初始化完成
}


