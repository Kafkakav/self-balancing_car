#include "car.h"
#include "math.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"


int Timer_100ms=0;//100ms時間片
int Timer_1S;//1s時間片
char Balance[4];
u8 Encoder_Timer = 0;

u8 OLED_ADC_Flag=0;
u8 OLED_JIAODU_Flag=0;

int Encoder_A,Encoder_B;


float adc11;
float adc22;

char adc1[5];
char adc2[4];
u16 ADC_BAT_Val,ADC_BAT_temp;
u16 ADC_SYS_Val,ADC_SYS_temp;

//MPU6050讀取
short Acce[3];     //設定加速度計快取，測量加速度
short Gryo[3];     //設定陀螺儀快取，測量角速度
//小車角度，平衡傾角 平衡陀螺儀 轉向陀螺儀
float Car_Angle_Balance,Angle_Balance,Gyro_Balance,Gyro_Turn,Car_Gyro_Balance; 
extern float angle;     //卡爾曼濾波后角度

int PWMA,PWMB;
int Balance_Pwm;     //平衡pwm
int Velocity_Pwm;    //速度pwm
int Turn_Pwm;        //轉向pwm


/********************************************
以下是平衡小車的PID參數除錯整定
安裝好MPU6050 測量平衡車的角度和角速度，只需測量Y軸和水平面的角度，和X軸的角速度即可。
陀螺儀可以用來測量物體的旋轉角速度
*********************************************/
//藍芽遙控相關的變數
extern u8 Flag_Qian,Flag_Hou,Flag_Left,Flag_Right; 
extern u8 Flag_sudu;
//平衡小車重要參數除錯
//第一步，確定機械中值
float ZHONGZHI = -1.0;             //機械中值，即小車不上電時候的平衡值。
//第二部，除錯直立環PID
float Balance_Kp = 1200.00;         //平衡環PID 的P值   
float Balance_Kd = 0.85;           //平衡環PID 的D值   
//第三步，除錯速度環PID
float Velocity_Kp = 120.00;         //速度環PID 的P值
float Velocity_Ki = 0.6;         //速度環PID 的I值 = P/200;
//第四步，除錯轉向環PID，需要在函式中調節



//利用MPU6050產生5ms中斷，中斷進入后開始做小車的控制及PID演算法
void EXTI2_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line2) != RESET)   //時間到了
	{
		EXTI_ClearITPendingBit(EXTI_Line2); // 清除中斷標誌位
		LED_Flash(50);//250ms閃爍一次
				
		Timer_100ms++;
		Timer_1S++;
		Encoder_A = -Read_Encoder(2);              //===讀取編碼器的值，因為兩個電機的旋轉了180度的，所以對其中一個取反，保證輸出極性一致
		Encoder_B = Read_Encoder(3);               //===讀取編碼器的值
		Car_Read_Angle();//獲取小車的平衡角度
		
		Balance_Pwm = Car_Balance(Car_Angle_Balance,Gyro_Balance);
		Velocity_Pwm = Car_Velocity(Encoder_A,Encoder_B);
		if(1==Flag_Left||1==Flag_Right)    
		Turn_Pwm =Car_Turn(Encoder_A,Encoder_B,Gyro_Turn);        //===轉向環PID控制
		else Turn_Pwm=-0.4*Gyro_Turn;

		
		PWMA = Balance_Pwm + Velocity_Pwm + Turn_Pwm;
		PWMB = Balance_Pwm + Velocity_Pwm - Turn_Pwm;
		
		Set_Pwm(PWMA,PWMB);//pwm更新

		if(Timer_1S >= 200)//1S 更新一次ADC
		{
			Timer_1S=0;
			ADC_BAT_Val = Get_BAT_ADC();//電壓值採集
			ADC_SYS_Val = Get_SYS_ADC();
			//計算電池電壓
			adc11=ADC_BAT_Val*3.3/4096*11;
			sprintf(adc1,"%2.1f",adc11);
			OLED_ShowString(16,0, adc1,12);  //OLED顯示電池電壓
			if(adc11<10.1) {
			  OLED_ShowString(0,0,"Low Power",12);//如果電池低於7.4V，提示低電壓，需要充電
			}

			//計算系統電壓
			adc22=ADC_SYS_Val*3.3/4096*2;
			sprintf(adc2,"%2.1f",adc22);
			OLED_ShowString(96,0, adc2,12);  //OLED顯示電池電壓
		}
		
		if(Timer_100ms >= 10)//50ms 更新一次角度
		{
			Timer_100ms=0;
			OLED_ShowNum(16,1,Encoder_A,5,12);
			OLED_ShowNum(16,2,Encoder_B,5,12);
			sprintf(Balance,"%2.1f",Car_Angle_Balance);
			OLED_ShowString(48,3, Balance,12);  //OLED顯示角度

		}

	}
	
		
}


/********************************************
函式功能：獲取角度
入口參數：無
返回  值：直立控制PWM
作    者：南京邁辰微
安裝好MPU6050 測量平衡車的角度和角速度，只需測量Y軸和水平面的角度，和X軸的角速度即可。
陀螺儀可以用來測量物體的旋轉角速度
*********************************************/
void Car_Read_Angle(void)
{
	float Accel_Y,Accel_Z,Gyro_X,Gyro_Z;
	
	MPU_Get_Gyroscope(&Gryo[0],&Gryo[1],&Gryo[2]);      //獲取角速度
	MPU_Get_Accelerometer(&Acce[0],&Acce[1],&Acce[2]);  //獲取加速度
	
	Gyro_X=(float)Gryo[0];    //X軸加速度原始
	Gyro_Z=(float)Gryo[2];		//Z軸加速度原始
	Accel_Y=(float)Acce[1];		//Y軸角速度原始
	Accel_Z=(float)Acce[2];		//X軸角速度原始
	
	if(Gyro_X>32768)  Gyro_X-=65536;     //數據型別轉換  也可通過short強制型別轉換
	if(Gyro_Z>32768)  Gyro_Z-=65536;     //數據型別轉換
	if(Accel_Y>32768) Accel_Y-=65536;    //數據型別轉換
	if(Accel_Z>32768) Accel_Z-=65536;    //數據型別轉換
	
	Gyro_Balance=Gyro_X;                                  	//更新平衡角速度
	Gyro_Turn=Gyro_Z;                                       //更新轉向角速度
	
	//加速度轉傾角
	//量程為±2g時，靈敏度：16384 LSB/g
	Angle_Balance=atan2(Accel_Y,Accel_Z)*180/PI;            //計算小車傾角
	//角速度
	//範圍為2000deg/s時，換算關係：16.4 LSB/(deg/s)
	Car_Gyro_Balance=Gyro_X/16.4;                          //陀螺儀量程轉換-角速度
	Kalman_Filter(Angle_Balance,Car_Gyro_Balance);				 //卡爾曼濾波
	Car_Angle_Balance=angle;                               //更新平衡傾角卡爾曼濾波后,融合后的角度需要和角速度方向一致		

}


/**************************************************************************
函式功能：直立PD控制
入口參數：角度、角速度
返回  值：直立控制PWM
作    者：南京邁辰微
**************************************************************************/
int Car_Balance(float Angle,float Gyro)
{  
	float Bias;
	int balance;
	Bias=Angle-ZHONGZHI;                       //===求出平衡的角度中值 和機械相關
	balance=Balance_Kp*Bias+Gyro*Balance_Kd;   //===計算平衡控制的電機PWM  PD控制   
	//kp是P係數 kd是D係數 
	return balance;
}

/**************************************************************************
函式功能：速度PI控制 修改前進後退速度，請修Target_Velocity，比如，改成60就比較慢了
入口參數：左輪編碼器、右輪編碼器
返回  值：速度控制PWM
作    者：南京邁辰微
**************************************************************************/
int Car_Velocity(int encoder_left,int encoder_right)
{  
	static float Velocity,Encoder_Least,Encoder,Movement;
	static float Encoder_Integral,Target_Velocity;
	//=============遙控前進後退部分=======================// 
	                         Target_Velocity=50;                 
	if(1==Flag_Qian)    	Movement=Target_Velocity;	         //===前進標誌位置1 
	else if(1==Flag_Hou)	Movement=-Target_Velocity;         //===後退標誌位置1
	else  Movement=0;	
	 
   //=============速度PI控制器=======================//	
	Encoder_Least =(Encoder_A+Encoder_B)-0;                    //===獲取最新速度偏差==測量速度（左右編碼器之和）-目標速度（此處為零） 
	Encoder *= 0.8;		                                         //===一階低通濾波器       
	Encoder += Encoder_Least*0.2;	                             //===一階低通濾波器    
	Encoder_Integral +=Encoder;                                //===積分出位移 積分時間：10ms
	Encoder_Integral=Encoder_Integral-Movement;                //===接收遙控器數據，控制前進後退
	if(Encoder_Integral>10000)  	Encoder_Integral=10000;      //===積分限幅
	if(Encoder_Integral<-10000)	Encoder_Integral=-10000;       //===積分限幅	
	Velocity=Encoder*Velocity_Kp+Encoder_Integral*Velocity_Ki;                          //===速度控制	
	return Velocity;
}


/**************************************************************************
函式功能：轉向控制  修改轉向速度，請修改Turn_Amplitude即可
入口參數：左輪編碼器、右輪編碼器、Z軸陀螺儀
返回  值：轉向控制PWM
作    者：南京邁辰微
**************************************************************************/
int Car_Turn(int encoder_a,int encoder_b,float gyro)//轉向控制
{
	static float Turn_Target,Turn,Encoder_temp,Turn_Convert=10,Turn_Count;
	float Turn_Amplitude=100,Kp=20,Kd=0;     
	//=============遙控左右旋轉部分=======================//
	//這一部分主要是根據旋轉前的速度調整速度的起始速度，增加小車的適應性
  	if(1==Flag_Left||1==Flag_Right)                      
		{
			if(++Turn_Count==1)
			Encoder_temp=myabs(encoder_a+encoder_b);      
			Turn_Convert=55/Encoder_temp;
			if(Turn_Convert<0.6)Turn_Convert=0.6;
			if(Turn_Convert>3)Turn_Convert=3;
		}	
	  else
		{
			Turn_Convert=10;
			Turn_Count=0;
			Encoder_temp=0;
		}			
		if(1==Flag_Left)	           Turn_Target+=Turn_Convert;
		else if(1==Flag_Right)	     Turn_Target-=Turn_Convert; 
		else Turn_Target=0;
	if(Turn_Target>Turn_Amplitude)  Turn_Target=Turn_Amplitude;    //===轉向	速度限幅
	if(Turn_Target<-Turn_Amplitude) Turn_Target=-Turn_Amplitude;
	if(Flag_Qian==1||Flag_Hou==1)  Kd=0.5;        
	else Kd=0;   //轉向的時候取消陀螺儀的糾正 有點模糊PID的思想
	//=============轉向PD控制器=======================//
	Turn=-Turn_Target*Kp-gyro*Kd;                 //===結合Z軸陀螺儀進行PD控制
	return Turn;
}


/**************************************************************************
函式功能：絕對值函式
入口參數：int
返回  值：unsigned int
**************************************************************************/
int myabs(int a)
{ 		   
	int temp;
	if(a<0)  temp=-a;  
	else temp=a;
	return temp;
}

void Set_Pwm(int motoA,int motoB)
{
	if(motoA<0){AIN1(1);AIN2(0);}
	else{AIN1(0);AIN2(1);}
	TIM4->CCR3=myabs(motoA);
	
	if(motoB<0){BIN1(1);BIN2(0);}
	else{BIN1(0);BIN2(1);}
	TIM4->CCR4=myabs(motoB);	
}






