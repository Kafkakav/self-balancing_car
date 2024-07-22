/*
********************************************************************************************************
檔名：bsp_iic.c
功  能：在訪問I2C裝置前，請先呼叫 i2c_CheckDevice() 檢測I2C裝置是否正常，該函式會配置GPIO
備  註：模擬iic
淘  寶：https://shop60670850.taobao.com
作  者：lilianhe
日  期: 2017-2-6
*********************************************************************************************************
*/
#include "bsp_iic.h"

/*
********************************************************************************************************
更新日誌：
1.2017-2-8，使用PB10----SCL，PB11----SDA


*********************************************************************************************************
*/

/**************************************************************************
 
 淘寶地址：https://shop60670850.taobao.com/
 專業平衡小車教學
  
  提供線上技術支援
  
 作    者：MCW
**************************************************************************/


/*
 *  I2C 讀取一個緩衝區
    I2C：I2C埠
    dev_addr：裝置地址（需要左移移位）
    reg_addr：暫存器地址
    pBuf：讀取的緩衝區
    num：讀取數據的大小
*/
void IIC_Read(I2C_TypeDef* I2C, uint8_t dev_addr, uint8_t reg_addr, uint8_t* pBuf, uint16_t num)
{
    /* 等待匯流排空閑 */
    while(RESET != I2C_GetFlagStatus(I2C, I2C_FLAG_BUSY)){}

    /* 產生起始位 */
    I2C_GenerateSTART(I2C, ENABLE);
    while(ERROR == I2C_CheckEvent(I2C, I2C_EVENT_MASTER_MODE_SELECT)){}

    /* 發送裝置地址 */
    I2C_Send7bitAddress(I2C, dev_addr, I2C_Direction_Transmitter);
    while(ERROR == I2C_CheckEvent(I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){}

    /* 發送暫存器地址 */
    I2C_SendData(I2C, reg_addr);
    while(ERROR == I2C_CheckEvent(I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){}

    /* 重複起始位 */
    I2C_GenerateSTART(I2C, ENABLE);
    while(ERROR == I2C_CheckEvent(I2C, I2C_EVENT_MASTER_MODE_SELECT)){}

    /* 發送裝置地址 */
    I2C_Send7bitAddress(I2C, dev_addr, I2C_Direction_Receiver);
    while(ERROR == I2C_CheckEvent(I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){}

    /*使能 ACK */
    I2C_AcknowledgeConfig(I2C, ENABLE);
        
    while(num > 1)
    {
        /* 等待數據接收 */
        while(ERROR == I2C_CheckEvent(I2C, I2C_EVENT_MASTER_BYTE_RECEIVED)){}
        *pBuf = I2C_ReceiveData(I2C);

        num--;
        pBuf++;
    }

    /* 在最後一個位元組讀取前，提前打開 NACK */
    I2C_AcknowledgeConfig(I2C, DISABLE);

    /* 在最後一個位元組讀取前， 提前使能結束位 */
    I2C_GenerateSTOP(I2C, ENABLE);

    /* 最後一個位元組 */
    while(ERROR == I2C_CheckEvent(I2C, I2C_EVENT_MASTER_BYTE_RECEIVED)){}
    *pBuf = I2C_ReceiveData(I2C);

}


/*
 *  I2C 寫入一個緩衝區
    I2C：I2C埠
    dev_addr：裝置地址（需要左移移位）
    reg_addr：暫存器地址
    pBuf：寫入的緩衝區
    num：寫入數據的大小
*/
void IIC_Write(I2C_TypeDef* I2C, uint8_t dev_addr, uint8_t reg_addr, uint8_t* pBuf, uint16_t num)
{
    /* 等待I2C匯流排空閑 */
    while(RESET != I2C_GetFlagStatus(I2C, I2C_FLAG_BUSY)){}

    /* 產生起始位 */
    I2C_GenerateSTART(I2C, ENABLE);
    while(ERROR == I2C_CheckEvent(I2C, I2C_EVENT_MASTER_MODE_SELECT)){}

    /* 發送從裝置地址 */
    I2C_Send7bitAddress(I2C, dev_addr, I2C_Direction_Transmitter);
    while(ERROR == I2C_CheckEvent(I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){}

    /* 寫暫存器地址 */
    I2C_SendData(I2C, reg_addr);
    while(ERROR == I2C_CheckEvent(I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING)){}

    /* 寫數據 */
    while(num > 1)
    {
        I2C_SendData(I2C, *pBuf);
        while(ERROR == I2C_CheckEvent(I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING)){}
        num--;
        pBuf++;
    }

    /* 最後一個位元組 */
    I2C_SendData(I2C, *pBuf);
    while(ERROR == I2C_CheckEvent(I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){}

    /* 產生結束位 */
    I2C_GenerateSTOP(I2C, ENABLE);
}


/*
 *  I2C 讀取一個位元組
    I2C：I2C埠
    dev_addr：裝置地址（需要左移移位）
    reg_addr：暫存器地址
    data：讀取的值
*/
void IIC_ReadByte(I2C_TypeDef* I2C, uint8_t dev_addr, uint8_t reg_addr, uint8_t* pData)
{
    /* 等待匯流排空閑 */
    while(RESET != I2C_GetFlagStatus(I2C, I2C_FLAG_BUSY)){}

    /* 產生起始位 */
    I2C_GenerateSTART(I2C, ENABLE);
    while(ERROR == I2C_CheckEvent(I2C, I2C_EVENT_MASTER_MODE_SELECT)){}

    /* 發送裝置地址 */
    I2C_Send7bitAddress(I2C, dev_addr, I2C_Direction_Transmitter);
    while(ERROR == I2C_CheckEvent(I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){}

    /* 發送暫存器地址 */
    I2C_SendData(I2C, reg_addr);
    while(ERROR == I2C_CheckEvent(I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){}

    /* 重複起始位 */
    I2C_GenerateSTART(I2C, ENABLE);
    while(ERROR == I2C_CheckEvent(I2C, I2C_EVENT_MASTER_MODE_SELECT)){}

    /* 發送裝置地址 */
    I2C_Send7bitAddress(I2C, dev_addr, I2C_Direction_Receiver);
    while(ERROR == I2C_CheckEvent(I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){}

    /* NACK */
    I2C_AcknowledgeConfig(I2C, DISABLE);

    /* 結束 */
    I2C_GenerateSTOP(I2C, ENABLE);

    /* 等待數據接收 */
    while(ERROR == I2C_CheckEvent(I2C, I2C_EVENT_MASTER_BYTE_RECEIVED)){}
    *pData = I2C_ReceiveData(I2C);
}



/*
 *  I2C 寫入一個位元組
    I2C：I2C埠
    dev_addr：裝置地址（需要左移移位）
    reg_addr：暫存器地址
    data：寫入的值
*/
void IIC_WriteByte(I2C_TypeDef* I2C, uint8_t dev_addr, uint8_t reg_addr, uint8_t data)
{
    /* 等待I2C匯流排空閑 */
    while(RESET != I2C_GetFlagStatus(I2C, I2C_FLAG_BUSY)){}

    /* 產生起始位 */
    I2C_GenerateSTART(I2C, ENABLE);
    while(ERROR == I2C_CheckEvent(I2C, I2C_EVENT_MASTER_MODE_SELECT)){}

    /* 發送從裝置地址 */
    I2C_Send7bitAddress(I2C, dev_addr, I2C_Direction_Transmitter);
    while(ERROR == I2C_CheckEvent(I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){}

    /* 寫暫存器地址 */
    I2C_SendData(I2C, reg_addr);
    while(ERROR == I2C_CheckEvent(I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTING)){}

    /* 寫數據 */
    I2C_SendData(I2C, data);
    while(ERROR == I2C_CheckEvent(I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){}

    /* 產生結束位 */
    I2C_GenerateSTOP(I2C, ENABLE);
}


/*
 * I2C 初始化
*/
uint8_t i2c_reinit = 0;
void IIC_Init(void)
{
    /* IIC 與 GPIO配置結構體 */
    GPIO_InitTypeDef  GPIO_InitStructure;
    I2C_InitTypeDef   I2C_InitStructure;

    if(i2c_reinit == 1) {
      //printf("i2c reinit\n");
      return;
    }
    i2c_reinit = 1;

    /* 使能時鐘 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /* 引腳配置 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 起始設定高電平 */
    GPIO_SetBits(GPIOB, GPIO_Pin_10 | GPIO_Pin_11);

    /* IIC 配置 I2C2--用於與MPU6050通訊, OLED通訊 */
    I2C_InitStructure.I2C_ClockSpeed = 400000;                                  /* 400K */
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;                                  /* IIC 模式 */
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;                          /* 2倍週期 */
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;                                 /* 使能ACK */
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;   /* 7bit */
    I2C_InitStructure.I2C_OwnAddress1 = 0x0A;                                   /* 自身地址 */
    I2C_Init(I2C2, &I2C_InitStructure);

    /* I2C使能 */
    I2C_Cmd(I2C2, ENABLE);
}

