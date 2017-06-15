#include "pwm.h"
#include "uart.h"
#include "myfun.h"

u8 light=100;
/*****************************************************************************
设置PWM时钟信号来源函数
参数：Clock
	0: 系统时钟/12(即12分频);                                          
    1：系统时钟/2(即2分频);
	3: ECI/P1.2(或P4.1)脚输入的外部时钟;
	4：系统时钟(即不分频) ;
	5：系统时钟/4(即4分频);
	6：系统时钟/6(即6分频);
	7：系统时钟/8(即8分频); 	  			       
/*****************************************************************************/
void PWM_Clock(unsigned char clock)
{ 	
	CMOD |= (clock<<1);
	CL = 0;
	CH = 0; 	
}

//屏幕背光控制管脚链接到P1^3,即PWM0模块
void PWM0_ChangeValue(u8 R0)
{  
	CCAP0L = 0XFF-R0;
	CCAP0H = 0XFF-R0;
}

void PWM0_Init(u8 start,u8 val)
{
	P1M0|=0X08;							//0000 1000
	P1M1|=0X00;							//推挽输出
	PWM_Clock(0);
	CCAPM0 = 0X42;						//模块0设置为8位PWM输出，无中断
	PWM0_ChangeValue(val);
	if(start)
		CR=1;
	else
		CR=0;
	UART_SendStr("PWM Init...		OK\n");
}

void PWM0_On()
{
	P1M0|=0X08;							//推挽输出1110 1000
	CCAPM0 = 0X42;						//模块0设置为8位PWM输出，无中断
	CR=1;
	Delay_Ms(10);
}

void PWM0_Off()
{
	CCAPM0=0x00;
	CR=0;								//关闭PWM
	P1M0&=0Xf7;							//关闭推挽
	LCD_BK=0;							//关闭背光
}

void PWM0_OffForInt()
{
	CCAPM0=0x00;
	CR=0;								//关闭PWM
	P1M0&=0Xf7;							//关闭推挽
	LCD_BK=0;							//关闭背光
}

void SetLight(u8 val)
{
	PWM0_ChangeValue((u16)val*255/100);
}







