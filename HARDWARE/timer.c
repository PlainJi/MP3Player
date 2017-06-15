#include "sys.h"
#include "timer.h"
#include "uart.h"
#include "touch.h"
#include "adc.h"
#include "GUI_Index.h"
#include "uart.h"
#include "pwm.h"

data u8  cnt=0;
data u16 second=0;			//开机时间？
idata u8 pingbaotime=0;
idata u8 pingbaoflag=0;

#if defined (XTAL22_1184)
void InitTimer0(void)	//10'000us
{
	TMOD |= 0x01;
	TH0 = 0x0B8;
	TL0 = 0x00;
	EA = 1;
	ET0 = 1;
	TR0 = 1;
	UART_SendStr("Timer Init...		OK\n");
}
#elif defined XTAL33_1776
void InitTimer0(void)	//10'000us
{
    TMOD |= 0x01;
    TH0 = 0x94;
    TL0 = 0x00;
    EA = 1;
    ET0 = 1;
    TR0 = 1;
	UART_SendStr("Timer Init...		OK\n");
}
#endif

void Timer0Interrupt(void) interrupt 1
{
#if defined XTAL22_1184
    TH0 = 0x0B8;
    TL0 = 0x00;
#elif defined XTAL33_1776
    TH0 = 0x94;
    TL0 = 0x00;
#endif
	cnt++;
	if(cnt==100)					//到达1s
	{
		cnt=0;
		second++;
		if(ScreenX==255)			//未按下
		{
			if(pingbaoflag==0)		//当前未屏保
			{
				pingbaotime++;
				if(pingbaotime==10)
				{
					pingbaoflag=1;	//进入屏保
					PWM0_OffForInt();
				}
			}
		}
	}
}

void PingBao()
{
	pingbaoflag=1;	//进入屏保
	PWM0_Off();
}

void ResumePingBao()
{
	pingbaoflag=0;
	PWM0_On();
	SetLight(light);
}










