#ifndef __PWM_H
#define __PWM_H

#include "sys.h"
sbit LCD_BK	= P1^3;			//TFT±³¹â
extern u8 light;

void PWM0_Init(u8 start,u8 val);
void PWM0_On();
void PWM0_Off();
void PWM0_OffForInt();
void PWM0_ChangeValue(u8 R0);
void SetLight(u8 val);

#endif