/*
********************************************************************************************************
檔名：Filter.c
功  能：濾波演算法
備  註：
淘  寶：https://shop60670850.taobao.com
作  者：lilianhe
日  期: 2017-2-6
*********************************************************************************************************
*/

#include "Filter.h"

//卡爾曼濾波參數
float K1 =0.02; 
float angle, angle_dot; 	
float Q_angle=0.001;// 過程噪聲的協方差
float Q_gyro=0.003;//0.03 過程噪聲的協方差 過程噪聲的協方差為一個一行兩列矩陣
float R_angle=0.5;// 測量噪聲的協方差 既測量偏差
float dt=0.005;//                 
char  C_0 = 1;
float Q_bias, Angle_err;
float PCt_0, PCt_1, E;
float K_0, K_1, t_0, t_1;
float Pdot[4] ={0,0,0,0};
float PP[2][2] = { { 1, 0 },{ 0, 1 } };

//簡易卡爾曼濾波
//加速度、角速度
void Kalman_Filter(float Accel,float Gyro)		
{
	angle+=(Gyro - Q_bias) * dt; //先驗估計
	Pdot[0]=Q_angle - PP[0][1] - PP[1][0]; // Pk-先驗估計誤差協方差的微分

	Pdot[1]=-PP[1][1];
	Pdot[2]=-PP[1][1];
	Pdot[3]=Q_gyro;
	PP[0][0] += Pdot[0] * dt;   // Pk-先驗估計誤差協方差微分的積分
	PP[0][1] += Pdot[1] * dt;   // =先驗估計誤差協方差
	PP[1][0] += Pdot[2] * dt;
	PP[1][1] += Pdot[3] * dt;
		
	Angle_err = Accel - angle;	//zk-先驗估計
	
	PCt_0 = C_0 * PP[0][0];
	PCt_1 = C_0 * PP[1][0];
	
	E = R_angle + C_0 * PCt_0;
	
	K_0 = PCt_0 / E;
	K_1 = PCt_1 / E;
	
	t_0 = PCt_0;
	t_1 = C_0 * PP[0][1];

	PP[0][0] -= K_0 * t_0;		 //后驗估計誤差協方差
	PP[0][1] -= K_0 * t_1;
	PP[1][0] -= K_1 * t_0;
	PP[1][1] -= K_1 * t_1;
		
	angle	+= K_0 * Angle_err;	 //后驗估計
	Q_bias	+= K_1 * Angle_err;	 //后驗估計
	angle_dot   = Gyro - Q_bias;	 //輸出值(后驗估計)的微分=角速度
}
