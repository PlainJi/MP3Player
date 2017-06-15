#include "touch.h"
#include "LG4525.h"
#include "myfun.h"
#include "uart.h"
#include "intrins.h"
#include "malloc.h"
#include "timer.h"

code u8 CMD_RDX=0Xd0;//1001 0000
code u8 CMD_RDY=0X90;//1101 0000
idata float xfac=0.048862;
idata float yfac=0.065359;
data s16	xoff=-9;
data s16	yoff=-28;
data u8 ScreenX,ScreenY;

void TP_Init()
{
	TCS=1;
	TCLK=1;
	TDIN=1;
	TCLK=1;
	UART_SendStr("TP Init...		OK\n");
}

//SPI写数据
//向触摸屏IC写入1byte数据    
//num:要写入的数据
void TP_WriteByte(u8 num)    
{  
	u8 count=0;
	TCLK=0;
	for(count=0;count<8;count++)
	{
		if(num&0x80)
			TDIN=1;
		else 
			TDIN=0;
		num<<=1;
		TCLK=0; _nop_();_nop_();_nop_();                //上升沿有效
		TCLK=1; _nop_();_nop_();_nop_();
	}
}
//SPI读数据
//从触摸屏IC读取adc值
//CMD:指令
//返回值:读到的数据	   
s16 TP_ReadAD(u8 CMD)	  
{ 	 
	u8 count=0;
	s16 Num=0;
	TCS=0; 		//选中触摸屏IC
	TP_WriteByte(CMD);//发送命令字
	Delay_Us(10);
	TCLK=1;_nop_();_nop_();_nop_();_nop_();		//给1个时钟，清除BUSY
	TCLK=0;_nop_();_nop_();_nop_();_nop_();
	for(count=0;count<12;count++)//读出16位数据,只有高12位有效
	{
		Num<<=1;
		TCLK=1;_nop_();_nop_();_nop_();                //下降沿有效
		TCLK=0;_nop_();_nop_();_nop_();
		if(DOUT)
			Num++;
	}
	TCS=1;		//释放片选
	return	Num;
}

//读取x,y坐标(两次取平均值)
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
void TP_Read_XY1(u16 *x,u16 *y)
{
	u16 read=0;

	read=TP_ReadAD(CMD_RDX);
	read+=TP_ReadAD(CMD_RDX);
	read/=2;
	*x=read;

	read=TP_ReadAD(CMD_RDY);
	read+=TP_ReadAD(CMD_RDY);
	read/=2;
	*y=read;
}
//连续2次读取触摸屏IC,且这两次的偏差不能超过
//ERR_RANGE,满足条件,则认为读数正确,否则读数错误.
//x,y:读取到的坐标值
//返回值:0,成功；1、2,失败。
#define ERR_RANGE 30 //误差范围
u8 TP_GetPhysicalXY(u16 *x,u16 *y)
{
	u16 x1,y1;
	u16 x2,y2;

	if(IRQ==0)						//按下了
	{
		TP_Read_XY1(&x1,&y1);
		TP_Read_XY1(&x2,&y2);
		if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))
		&&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
		{
			*x=(x1+x2)/2;
			*y=(y1+y2)/2;
			return 0;
		}
		else
			return 1;				//按下无效
	}
	else
		return 2;					//未按下
}

u8 TP_GetScreenXY(u8 *x,u8 *y)
{
	u16 xx,yy;
	if(!TP_GetPhysicalXY(&xx,&yy))
	{
		*x=xfac*xx+xoff;
		*y=220-(yfac*yy+yoff);
		pingbaotime=0;					//如果按下了，则清零屏保计时变量
		return 0;
	}
	else
	{
		*x=255;						//无效
		*y=255;						//无效
		return 1;
	}
}

u8 TP_Adjust()
{
	u16 x1,y1;
	u16 x2,y2;
	u8 i;
	u8 *tab=Malloc(32);
	if(tab==0)
	{
		UART_SendStr("Malloc error When running TP_Adjust!");
		return 1;
	}
//1
	LCD_DrawBlock(24,30,13,1,Red);		//(30,30)
	LCD_DrawBlock(30,24,1,13,Red);
	while(TP_GetPhysicalXY(&x1,&y1));
	LCD_DrawBlock(24,30,13,1,White);
	LCD_DrawBlock(30,24,1,13,White);
	UART_PutInf("x1=",x1);
	UART_PutInf("y1=",y1);
	Delay_Ms(500);
//2
	LCD_DrawBlock(140,30,13,1,Red);		//(146,30)
	LCD_DrawBlock(146,24,1,13,Red);
	while(TP_GetPhysicalXY(&x2,&y2));
	LCD_DrawBlock(140,30,13,1,White);
	LCD_DrawBlock(146,24,1,13,White);
	UART_PutInf("x2=",x2);
	UART_PutInf("y2=",y2);
	Delay_Ms(500);
	xfac=(float)(176-60)/(x2-x1);		//得到xfac
	xoff=(176-xfac*(x2+x1))/2;			//得到xoff

	for(i=0;i<32;i++)
		tab[i]=0;
	Float2Str(xfac,tab);
	UART_SendStr(tab);
	UART_SendStr("\n");

	for(i=0;i<32;i++)
		tab[i]=0;
	Int2Str(xoff,tab);
	UART_SendStr(tab);
	UART_SendStr("\n");
	UART_SendStr("\n");

//3
	LCD_DrawBlock(24,190,13,1,Red);		//(30,190)
	LCD_DrawBlock(30,184,1,13,Red);
	while(TP_GetPhysicalXY(&x2,&y2));
	LCD_DrawBlock(24,190,13,1,White);
	LCD_DrawBlock(30,184,1,13,White);
	UART_PutInf("x3=",x2);
	UART_PutInf("y3=",y2);
	Delay_Ms(500);
	yfac=(float)(220-60)/(y1-y2);		//得到yfac
	yoff=(220-yfac*(y2+y1))/2;			//得到yoff

	for(i=0;i<32;i++)
		tab[i]=0;
	Float2Str(yfac,tab);
	UART_SendStr(tab);
	UART_SendStr("\n");

	for(i=0;i<32;i++)
		tab[i]=0;
	Int2Str(yoff,tab);
	UART_SendStr(tab);
	UART_SendStr("\n");
	UART_SendStr("\n");

//4
	LCD_DrawBlock(140,190,13,1,Red);	//(146,190)
	LCD_DrawBlock(146,184,1,13,Red);
	while(TP_GetPhysicalXY(&x2,&y2));
	LCD_DrawBlock(140,190,13,1,White);
	LCD_DrawBlock(146,184,1,13,White);
	Free(tab);
	return 0;
}




