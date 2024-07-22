/*
********************************************************************************************************
檔名：bsp_usart.c
功  能：串列埠函式
備  註：
淘  寶：https://shop60670850.taobao.com
作  者：lilianhe
日  期: 2017-2-8
*********************************************************************************************************
*/

/*
********************************************************************************************************
更新日誌：
1.2017-2-8，編寫uart1函式

*********************************************************************************************************
*/
#include "bsp_usart.h"
#include <stdio.h>
#include <string.h>

volatile unsigned char *rx_address;
volatile unsigned int rx_count;
volatile unsigned int rx_length;

#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
 
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART1 and Loop until the end of transmission */

#if 0
  	while((DEBUG_USARTx->SR & 0X40) == 0); 
  	DEBUG_USARTx->DR = (u8)ch;    
#else
	/* 發送一個位元組數據到USART1 */
	USART_SendData(USART1, (uint8_t) ch);
	/* 等待發送完畢 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
#endif

	return (ch);

}
__attribute__((weak)) int _write(int file, char *ptr, int len)
{
  int DataIdx;
   for (DataIdx = 0; DataIdx < len; DataIdx++) {
    __io_putchar(*ptr++);
  }
  return len;
}

/*
********************************************************************************************************
函式名稱：void USART1_Init(u32 bound)
函式功能：串列埠1初始化函式
硬體連線：PA9----TXD，PA10----RXD
備    註：bound為設定波特率
日    期:  2017-2-8
*********************************************************************************************************
*/
void USART1_Init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* config USART1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	
	/* USART1 GPIO config */
	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	/* 使能串列埠1接收中斷 */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	USART_Cmd(USART1, ENABLE);
}


/// 重定向c庫函式printf到USART1
int fputc(int ch, FILE *f)
{
		/* 發送一個位元組數據到USART1 */
		USART_SendData(USART1, (uint8_t) ch);
		
		/* 等待發送完畢 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

/// 重定向c庫函式scanf到USART1
int fgetc(FILE *f)
{
		/* 等待串列埠1輸入數據 */
		while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(USART1);
}
/*
********************************************************************************************************
函式名稱：void USART1_Send(unsigned char *tx_buf, int len)
函式功能：串列埠1發送函式
硬體連線：PA9----TXD，PA10----RXD
備    註：
日    期:  2017-2-8
*********************************************************************************************************
*/
void USART1_Send(unsigned char *tx_buf, int len)
{
		USART_ClearFlag(USART1, USART_FLAG_TC);
		USART_ClearITPendingBit(USART1, USART_FLAG_TXE);
	while(len--)
	{
		USART_SendData(USART1, *tx_buf);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) != 1);
		USART_ClearFlag(USART1, USART_FLAG_TC);
		USART_ClearITPendingBit(USART1, USART_FLAG_TXE);
		tx_buf++;
	}
	
}

/*
********************************************************************************************************
函式名稱：void USART1_Receive(unsigned char *rx_buf, int len)
函式功能：串列埠1接收函式
硬體連線：PA9----TXD，PA10----RXD
備    註：
日    期:  2017-2-8
*********************************************************************************************************
*/
void USART1_Receive(unsigned char *rx_buf, int len)
{
	rx_count = 0;
	rx_length = len;
	rx_address = rx_buf;
}
/*
********************************************************************************************************
函式名稱：void USART1_IRQHandler(void)
函式功能：串列埠1中斷服務函式
硬體連線：PA9----TXD，PA10----RXD
備    註：
日    期:  2017-2-8
*********************************************************************************************************
*/
void USART1_IRQHandler(void)
{
	unsigned char Res;
	//u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中斷(接收到的數據必須是0x0d 0x0a結尾)
	{
		
		Res = USART_ReceiveData(USART1);
	//	 printf("[USART1_IRQHandler],Rec_data = %x\r\n", Res);

		if(rx_length > rx_count) {
			*rx_address = Res;
			rx_address++;
			rx_count++;	
  	}
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
			 
  }
}


/*********************************************END OF FILE**********************/
