#include "uart.h"
#include "myfun.h"
#include <string.h>

/**************************************************************************
 - 功能描述：51单片机的串口初始化
 - 参数说明：波特率
 - 注：暂时只支持22.1184M和33.1776M晶振
**************************************************************************/
void UART_Init(u16 bps)
{
	PS=1;				//串口优先级最高
	PCON|=0x80; 		//PCON的最高位SMOD=1时波特率加倍
	TMOD|=0x20;  		//时器1为方式2 初值自动装入 产生波特率
#if defined	(XTAL22_1184)
	switch(bps)
	{
		case 600:		TH1=0x40;TL1=0x40;	break;
		case 4800:		TH1=0xe8;TL1=0xe8;	break;
		case 9600:		TH1=0xf4;TL1=0xf4;	break;
		case 38400:		TH1=0xfd;TL1=0xfd;	break;
		case 57600:		TH1=0xfe;TL1=0xfe;	break;
		default	:		break;
	}
#elif defined (XTAL33_1776)
	switch(bps)
	{
		case 600:		TH1=0x40;TL1=0x40;	break;
		case 4800:		TH1=0xdc;TL1=0xdc;	break;
		case 9600:		TH1=0xee;TL1=0xee;	break;
		case 19200:		TH1=0xf7;TL1=0xf7;	break;
//		case 38400:		TH1=0xfd;TL1=0xfd;	break;	//不支持，误码率太大!!!
		case 57600:		TH1=0xfd;TL1=0xfd;	break;
		default	:		break;
	}
#endif
	SCON|=0x50;			//串口设置为方式1,REN=1,允许接收
	TR1=1;      		//启动定时器1
	ES=1;       		//使能串口接收中断，
	EA=1;       		//打开所有中断 
	UART_PutBuildingTime();
	UART_SendStr("Uart Init...		OK\n");
}


void sio_int() interrupt 4 using 3   //串口中断函数
{
	ES=0;
	//串口中断处理
	/*
	if(RI)
	{
		if(SBUF!=0x08)  //如果接收到的是退格(ASCII码为0x08)
			cmd_buf[counter++]=SBUF;
		else
			counter--;
		RI=0;
	}
	if(SBUF==0x0d)
	{
		cmd_buf[counter-1]=0;
		counter=0;
		flag=1;
	}
	*/
	ES=1;
}


void UART_SendByte(u8 mydata)	
{
	ES=0;
	TI=0;
	SBUF=mydata;
	while(!TI);
	TI=0;
	ES=1;
}

void UART_SendEnter()
{
	UART_SendByte(0x0d);
	UART_SendByte(0x0a);
}

void UART_SendStr(s8 *s)
{
	while(*s!=0)
	{
		UART_SendByte(*s++);
	}
}

void UART_PutNum(u32 dat)
{
	u8 temp[11];
	u32tostr(dat,temp);
	UART_SendStr(temp);
}

u8 Hex2Str_16b(u16 hex,s8 *str)
{
	u8 temp=0;
	
	temp=((hex&0xf000)>>12);
	str[0]=(temp>=10)?(temp-10+'A'):(temp+0x30);
	
	temp=((hex&0x0f00)>>8);
	str[1]=(temp>=10)?(temp-10+'A'):(temp+0x30);
	
	temp=((hex&0x00f0)>>4);
	str[2]=(temp>=10)?(temp-10+'A'):(temp+0x30);
	
	temp=((hex&0x000f)>>0);
	str[3]=(temp>=10)?(temp-10+'A'):(temp+0x30);
	
	str[4]=0;
	
	return 0;
}

void UART_PutHex(u16 hex)
{
	char temp[11];
	Hex2Str_16b(hex,temp);
	UART_SendStr(temp);
}


void UART_PutInf(s8 *inf,u32 dat)
{
	UART_SendStr(inf);
	UART_PutNum(dat);
	UART_SendStr("\n");
}

void Binary(u8 dat)
{
	u8 i;
	u8 a[9];
	for(i=0;i<8;i++)
	{
		a[i]=((dat<<i)&0x80)?'1':'0';
	}
	a[i]=0;
	for(i=0;i<strlen(a);i++)
	{
		UART_SendByte(a[i]);
		UART_SendByte(' ');
	}
}

void UART_PutBuildingTime()
{
	UART_SendStr("================================\n");
	UART_SendStr("======");
	UART_SendStr(__TIME__);
	UART_SendStr(" ");
	UART_SendStr(__DATE__);
	UART_SendStr("======\n");
	UART_SendStr("================================\n");
}

