#include "ADC.h"
#include "myfun.h"
#include "uart.h"
#include "LG4525.H"
#include "pwm.h"
#include "spiflash.h"

void ADC_Init()
{
	P1M0|=0x10;			//��P1_4Ϊ��©ģʽ
	P1M1|=0x10;
	P1ASF=0x10;
	ADC_CONTR=0x04;		//�ر�ADC��Դ��ת���ٶ�����������P1.4Ϊ����ADת���� 
	ADC_PowerOn();
	UART_SendStr("ADC Init...		OK\n");
	if(ADC_GetResult()<340)
	{
		PowerOff();				//�ػ�
	}
}

void ADC_PowerOff()
{
	ADC_CONTR&=0x7f;
}

void ADC_PowerOn()
{
	ADC_CONTR|=0x80;
	Delay_Ms(50);		//��ʱһ����ȴ�ADC��Դ�ȶ�
}

//10λ����
//ȡ10�ε�ƽ��ֵ
//��ѹ���㹫ʽΪ��(value/10)*3.3/1024
//����ֵ��ʽ����1.23V���򷵻�123
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
		ADC_CONTR|=0x08;			//����ADת��
		while(!temp)      			//�ȴ�ADת�����
		{
			temp=0x10;
			temp&=ADC_CONTR;
		}
		ADC_CONTR&=0xe7;			//���ת����ɱ�־
		temp=ADC_RES;				//��8λ
		temp<<=2;
		temp|=ADC_RESL;				//��2λ
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
	P1M0=0x00;				//�ر�SPI�ӿڵ��������
	P1M1=0x00;
	ADC_PowerOff();			//�ر�ADC
	W25Q16_DeepPowerDown();	//Flash�������˯��
	PCON|=0X02;				//CPU�������ģʽ
	while(1);
}















