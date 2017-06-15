#include "pwm.h"
#include "uart.h"
#include "myfun.h"

u8 light=100;
/*****************************************************************************
����PWMʱ���ź���Դ����
������Clock
	0: ϵͳʱ��/12(��12��Ƶ);                                          
    1��ϵͳʱ��/2(��2��Ƶ);
	3: ECI/P1.2(��P4.1)��������ⲿʱ��;
	4��ϵͳʱ��(������Ƶ) ;
	5��ϵͳʱ��/4(��4��Ƶ);
	6��ϵͳʱ��/6(��6��Ƶ);
	7��ϵͳʱ��/8(��8��Ƶ); 	  			       
/*****************************************************************************/
void PWM_Clock(unsigned char clock)
{ 	
	CMOD |= (clock<<1);
	CL = 0;
	CH = 0; 	
}

//��Ļ������ƹܽ����ӵ�P1^3,��PWM0ģ��
void PWM0_ChangeValue(u8 R0)
{  
	CCAP0L = 0XFF-R0;
	CCAP0H = 0XFF-R0;
}

void PWM0_Init(u8 start,u8 val)
{
	P1M0|=0X08;							//0000 1000
	P1M1|=0X00;							//�������
	PWM_Clock(0);
	CCAPM0 = 0X42;						//ģ��0����Ϊ8λPWM��������ж�
	PWM0_ChangeValue(val);
	if(start)
		CR=1;
	else
		CR=0;
	UART_SendStr("PWM Init...		OK\n");
}

void PWM0_On()
{
	P1M0|=0X08;							//�������1110 1000
	CCAPM0 = 0X42;						//ģ��0����Ϊ8λPWM��������ж�
	CR=1;
	Delay_Ms(10);
}

void PWM0_Off()
{
	CCAPM0=0x00;
	CR=0;								//�ر�PWM
	P1M0&=0Xf7;							//�ر�����
	LCD_BK=0;							//�رձ���
}

void PWM0_OffForInt()
{
	CCAPM0=0x00;
	CR=0;								//�ر�PWM
	P1M0&=0Xf7;							//�ر�����
	LCD_BK=0;							//�رձ���
}

void SetLight(u8 val)
{
	PWM0_ChangeValue((u16)val*255/100);
}







