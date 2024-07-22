#include "mpu6050.h"
#include "bsp_sys.h"
#include "bsp_iic.h"
#include "delay.h"
#include "bsp_usart.h"   

u8 MPU_Write_Len(uint8_t addr,uint8_t reg,uint16_t num, uint8_t* pBuf )
{
  /* 地址位左移一位，用於寫 */
  IIC_Write(I2C2, addr << 1, reg, pBuf, num);
	return 0;
}

u8 MPU_Read_Len(uint8_t addr,uint8_t reg, uint16_t num, uint8_t* pBuf)
{
  /* 地址位左移一位，用於讀 */
  IIC_Read(I2C2, addr << 1, reg, pBuf, num);
	return 0;
}
//IIC寫一個位元組 
//reg:暫存器地址
//data:數據
//返回值:0,正常
//    其他,錯誤程式碼
u8 MPU_Write_Byte(uint8_t reg, uint8_t val) 				 
{ 
  /* 地址位左移一位，用於寫 */
  IIC_WriteByte(I2C2, MPU_IIC_ADDR << 1, reg, val);
	return 0;
}
//IIC讀一個位元組 
//reg:暫存器地址 
//返回值:讀到的數據
u8 MPU_Read_Byte(uint8_t reg)
{
	uint8_t res;
	IIC_ReadByte(I2C2, MPU_IIC_ADDR << 1, reg, &res);
	return res;
}

//初始化MPU6050
//返回值:0,成功
//其他,錯誤程式碼
u8 MPU_Init(void)
{ 
	u8 res; 
	//MPU_IIC_Init();//初始化IIC匯流排
	IIC_Init();
	MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X80);	//復位MPU6050
	Delay_ms(100);
	MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X00);	//喚醒MPU6050 
  Delay_ms(50);
	MPU_Set_Gyro_Fsr(3);					//陀螺儀感測器,±2000dps
	MPU_Set_Accel_Fsr(0);					//加速度感測器,±2g
	MPU_Set_Rate(200);						//設定採樣率50Hz
	MPU_Write_Byte(MPU_INT_EN_REG,0X00);	//關閉所有中斷
	MPU_Write_Byte(MPU_USER_CTRL_REG,0X00);	//I2C主模式關閉
	MPU_Write_Byte(MPU_FIFO_EN_REG,0X00);	//關閉FIFO
	MPU_Write_Byte(MPU_INTBP_CFG_REG,0X80);	//INT引腳低電平有效
	res=MPU_Read_Byte(MPU_DEVICE_ID_REG);
	if(res==MPU_ADDR) {//器件ID正確, //0x68
    printf("MPU:id=%x\n", res);
		MPU_Write_Byte(MPU_PWR_MGMT1_REG,0X01);	//設定CLKSEL,PLL X軸為參考
		MPU_Write_Byte(MPU_PWR_MGMT2_REG,0X00);	//加速度與陀螺儀都工作
		MPU_Set_Rate(100);						//設定採樣率為100Hz,週期為10ms
 	}else return 1;

	if(1) {
		short vX=0, vY=0, vZ=0;
		printf("MPU:GetTemp:%d\n", MPU_Get_Temperature());
		MPU_Get_Gyroscope(&vX, &vY, &vZ);
		printf("MPU:GetGyro:%d,%d,%d\n", vX,vY,vZ);
		vX=0; vY=0; vZ=0;
		MPU_Get_Accelerometer(&vX, &vY, &vZ);
		printf("MPU:GetAcc:%d,%d,%d\n", vX,vY,vZ);
	}

	return 0;
}
//設定MPU6050陀螺儀感測器滿量程範圍
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//返回值:0,設定成功
//    其他,設定失敗 
u8 MPU_Set_Gyro_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU_GYRO_CFG_REG,fsr<<3);//設定陀螺儀滿量程範圍  
}
//設定MPU6050加速度感測器滿量程範圍
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
//返回值:0,設定成功
//    其他,設定失敗 
u8 MPU_Set_Accel_Fsr(u8 fsr)
{
	return MPU_Write_Byte(MPU_ACCEL_CFG_REG,fsr<<3);//設定加速度感測器滿量程範圍  
}
//設定MPU6050的數字低通濾波器
//lpf:數字低通濾波頻率(Hz)
//返回值:0,設定成功
//    其他,設定失敗 
u8 MPU_Set_LPF(u16 lpf)
{
	u8 data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return MPU_Write_Byte(MPU_CFG_REG,data);//設定數字低通濾波器  
}
//設定MPU6050的採樣率(假定Fs=1KHz)
//rate:4~1000(Hz)
//返回值:0,設定成功
//    其他,設定失敗 
u8 MPU_Set_Rate(u16 rate)
{
	u8 data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=MPU_Write_Byte(MPU_SAMPLE_RATE_REG,data);	//設定數字低通濾波器
 	return MPU_Set_LPF(rate/2);	//自動設定LPF為採樣率的一半
}

//得到溫度值
//返回值:溫度值(擴大了100倍)
short MPU_Get_Temperature(void)
{
  u8 buf[2]; 
  short raw;
	float temp;
	MPU_Read_Len(MPU_ADDR,MPU_TEMP_OUTH_REG,2,buf); 
  raw=((u16)buf[0]<<8)|buf[1];  
  temp=36.53+((double)raw)/340;  
  return temp*100;;
}
//得到陀螺儀值(原始值)
//gx,gy,gz:陀螺儀x,y,z軸的原始讀數(帶符號)
//返回值:0,成功
//    其他,錯誤程式碼
u8 MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
    u8 buf[6],res;  
	res=MPU_Read_Len(MPU_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
	if(res==0)
	{
		*gx=((u16)buf[0]<<8)|buf[1];  
		*gy=((u16)buf[2]<<8)|buf[3];  
		*gz=((u16)buf[4]<<8)|buf[5];
	} 	
    return res;;
}
//得到加速度值(原始值)
//gx,gy,gz:陀螺儀x,y,z軸的原始讀數(帶符號)
//返回值:0,成功
//    其他,錯誤程式碼
u8 MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
    u8 buf[6],res;  
	res=MPU_Read_Len(MPU_ADDR,MPU_ACCEL_XOUTH_REG,6,buf);
	if(res==0)
	{
		*ax=((u16)buf[0]<<8)|buf[1];  
		*ay=((u16)buf[2]<<8)|buf[3];  
		*az=((u16)buf[4]<<8)|buf[5];
	} 	
    return res;;
}
 





