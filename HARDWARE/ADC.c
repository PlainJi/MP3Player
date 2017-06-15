#include "ADC.h"
#include "myfun.h"
#include "uart.h"
#include "LG4525.H"
#include "pwm.h"
#include "spiflash.h"

void ADC_Init()
{
	P1M0|=0x10;			//设P1_4为开漏模式
	P1M1|=0x10;
	P1ASF=0x10;
	ADC_CONTR=0x04;		//关闭ADC电源，转换速度最慢，设置P1.4为输入AD转换口 
	ADC_PowerOn();
	UART_SendStr("ADC Init...		OK\n");
	if(ADC_GetResult()<340)
	{
		PowerOff();				//关机
	}
}

void ADC_PowerOff()
{
	ADC_CONTR&=0x7f;
}

void ADC_PowerOn()
{
	ADC_CONTR|=0x80;
	Delay_Ms(50);		//延时一会儿等待ADC电源稳定
}

//10位采样
//取10次的平均值
//电压计算公式为：(value/10)*3.3/1024
//返回值格式：如1.23V，则返回123
u16 ADC_GetResult()
{  
	u8  i;
	u16 temp;
	f32 value;

	value=0;
	for(i=0;i<10;i++)
	{
		temp=0;
		ADC_RES=0;
		ADC_RESL=0;
		ADC_CONTR|=0x08;			//启动AD转换
		while(!temp)      			//等待AD转换完成
		{
			temp=0x10;
			temp&=ADC_CONTR;
		}
		ADC_CONTR&=0xe7;			//清除转换完成标志
		temp=ADC_RES;				//高8位
		temp<<=2;
		temp|=ADC_RESL;				//低2位
		value+=temp;

	}
	return (u16)((value*3.3*2*100/10/1024));
}

void PowerOff()
{
	SetLight(100);
	UART_SendStr("Low Power,Please charge!\n");
	LCD_Clear(White);
	LCD_ShowStrMid(0,90,"Low Power",16,176,Red,Black);
	LCD_ShowStrMid(0,114,"Please charge!",16,176,Red,Black);
	Delay_Ms(1000);
	PWM0_Off();
	P1M0=0x00;				//关闭SPI接口的推挽输出
	P1M1=0x00;
	ADC_PowerOff();			//关闭ADC
	W25Q16_DeepPowerDown();	//Flash进入深度睡眠
	PCON|=0X02;				//CPU进入掉电模式
	while(1);
}















