#ifndef __TOUCH_H
#define __TOUCH_H

#include "sys.h"

extern data u8 ScreenX,ScreenY;
extern data u8 touched;
#define CHECK_TOUCH (ScreenX<255)	//1按下，0未按下

sbit TCLK	=P2^2;
sbit TCS	=P2^3;
sbit TDIN	=P2^4;
sbit DOUT	=P5^0;
sbit IRQ	=P3^3;	//INT

void TP_Init();
u8 TP_GetPhysicalXY(u16 *x,u16 *y);
u8 TP_GetScreenXY(u8 *x,u8 *y);
u8 TP_Adjust();

#endif