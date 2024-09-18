//超聲波例程
//藍芽模組只需要接四個介面，TXD，RXD，5V，GND
//使用HC05藍芽模組，串列埠透傳功能，和使用串列埠差不多，使用stm32 的uart2
//PA2----藍芽RXD  
//PA3----藍芽TXD

#include "bluetooth.h"
#include <stdio.h>

volatile unsigned char *rx2_address;
volatile unsigned int rx2_count;
volatile unsigned int rx2_length;
extern u8 Flag_Qian,Flag_Hou,Flag_Left,Flag_Right,Flag_sudu; //藍芽遙控相關的變數
/*
********************************************************************************************************
函式名稱：void USART2_Init(u32 bound)
函式功能：串列埠1初始化函式
硬體連線：PA2----RXD，PA3----TXD
備    註：bound為設定波特率
日    期:  2017-2-8
*********************************************************************************************************
*/
void Bluetooth_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	/* config USART2 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	/* USART2 GPIO config */
	/* Configure USART2 Tx (PA.02) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
	/* Configure USART2 Rx (PA.03) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* USART2 mode config */
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
	
	
	/* Configure the NVIC Preemption Priority Bits */  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* 使能串列埠1接收中斷 */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(USART2, ENABLE);
}


//藍芽發送到手機
void USART2_Send(unsigned char *tx_buf, int len)
{
	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_ClearITPendingBit(USART2, USART_FLAG_TXE);
	while(len--)
	{
		USART_SendData(USART2, *tx_buf);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TC) != 1);
		USART_ClearFlag(USART2, USART_FLAG_TC);
		USART_ClearITPendingBit(USART2, USART_FLAG_TXE);
		tx_buf++;
	}
	
}

//藍芽接受，從手機接收
void USART2_Receive(unsigned char *rx_buf, int len)
{
	rx2_count = 0;
	rx2_length = len;
	rx2_address = rx_buf;
}


//串列埠2中斷函式
#define UART_BUFMAX 64
u8 uart_rxbuf[UART_BUFMAX];
u8 uart_rxlen= 0;
void USART2_IRQHandler(void)
{
	u8 uart_receive=0;//藍芽接收相關變數
	//u8 Res;
    
	//printf("IRQ:%u\n", USART_GetITStatus(USART2, USART_IT_RXNE));
	// when enable USART_IT_RXNE,  USART_FLAG_ORE will be enable
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET || 
		USART_GetFlagStatus(USART2, USART_FLAG_ORE) != RESET)  //接收中斷(接收到的數據必須是0x0d 0x0a結尾)
	{
		uart_receive = USART_ReceiveData(USART2);
		//printf("COM:0x%x [%c]\n", uart_receive, (char)uart_receive);
		if(uart_receive == 'Z') {
			printf("JOY:Z\n");
			Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=0;
			uart_rxlen = 0;
		}
		else if(uart_receive == '\r' || uart_receive == '\n') {
			uart_rxbuf[uart_rxlen] = 0;
			//printf("JOY:[%s]\n", (char *)uart_rxbuf);
			if(uart_rxlen > 0) {
				printf("JOY:[%s]\n", (char *)uart_rxbuf);
				//前後左右
				Flag_Qian=0,Flag_Hou=0,Flag_Left=0,Flag_Right=0; 
				#if 0
				if(uart_rxbuf[0] == 'Z') //剎車 Z
					Flag_Qian = 0;
				else if(uart_rxbuf[0] == 'W') //前 W
					Flag_Qian = 1; 
				else if(uart_rxbuf[0] == 'S') //後 S
					Flag_Hou = 1;
				else if(uart_rxbuf[0] == 'A') //左 A
					Flag_Left = 1;
				else if(uart_rxbuf[0] == 'D') //右 D
					Flag_Right = 1;
				#else // 設定左右反向
				if(uart_rxbuf[0] == 'Z') //剎車 Z
					Flag_Qian = 0;
				else if(uart_rxbuf[0] == 'W') //前 W
					Flag_Qian = 1;
				else if(uart_rxbuf[0] == 'S') //後 S
					Flag_Hou = 1;
				else if(uart_rxbuf[0] == 'A') //左 A
					Flag_Right = 1;
				else if(uart_rxbuf[0] == 'D') //右 D
					Flag_Left = 1;
				#endif
			}
			uart_rxlen = 0;
		}
        else {
			uart_rxbuf[uart_rxlen++] = uart_receive;
			if(uart_rxlen >= UART_BUFMAX) //buffer ovwerflow, reset
				uart_rxlen = 0;
		}
//		if(rx2_length > rx2_count)
//		{
//			*rx2_address = Res;
//			rx2_address++;
//			rx2_count++;	
//		}
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
	else {
		//注意！不能使用if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)来判断
      	if (USART_GetFlagStatus(USART2, USART_FLAG_ORE) != RESET) {
 			uart_receive = USART_ReceiveData(USART2);
			printf("ORE:0x%x\n",uart_receive);
		}

	}

}
