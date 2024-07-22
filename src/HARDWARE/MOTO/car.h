#ifndef __CAR_H
#define __CAR_H

#include "stm32f10x.h"
#include "bsp_sys.h"

#define PI 3.14159265
void Car_Read_Angle(void);
int myabs(int a);
void Set_Pwm(int motoA,int motoB);
int Car_Balance(float Angle,float Gyro);
int Car_Velocity(int encoder_left,int encoder_right);
int Car_Turn(int encoder_a,int encoder_b,float gyro);

#endif
