#include "myfun.h"
#include "string.h"
#include "intrins.h"
#include "math.h"

void Delay(u16 time) 
{
	while(time--);
}

#ifdef	XTAL33_1776
void delay1us(void)   //��� -0.005353009259us
{
    unsigned char a,b;
    for(b=1;b>0;b--)
        for(a=5;a>0;a--);
    _nop_();  //if Keil,require use intrins.h
}
void Delay_Us(u16 j)
{
	while(j--)
		delay1us();
}
void Delay_Ms(u16 j)   //��� -0.018084490741us
{
    u8 a,b,c;
	while(j--)
	{
	    for(c=2;c>0;c--)
	        for(b=224;b>0;b--)
	            for(a=17;a>0;a--);
	    _nop_();  		//if Keil,require use intrins.h
	}
}
#endif

#ifdef	XTAL22_1184
void delay1us(void)   //��� 0us
{
    unsigned char a;
    for(a=1;a>0;a--);
    _nop_();  //if Keil,require use intrins.h
    _nop_();  //if Keil,require use intrins.h
}

void Delay_Us(u16 j)
{
	while(j--)
		delay1us();
}

void Delay_Ms(u16 j)   //��� -0.018084490741us
{
    u8 a,b,c;
	while(j--)
	{
	    for(c=1;c>0;c--)
	        for(b=170;b>0;b--)
	            for(a=31;a>0;a--);
	}
}
#endif

/******************************************************************
 - ������������һ��32λ�ı���datתΪ�ַ����������1234תΪ"1234"
 - ����ģ�飺��������ģ��
 - �������ԣ��ⲿ���û��ɵ���
 - ����˵����dat:��ת��long�͵ı���
             str:ָ���ַ������ָ�룬ת������ֽڴ���������           
 - ����˵������
 ******************************************************************/
void u32tostr(u32 dat,s8 *str) 
{
	u8 i=0,j=0,temp=0,end=0;

	while(dat)						//1234
	{								//"4321"	i=4
		str[i]=dat%10+0x30;
		i++;
		dat/=10;
	}

	if(!i)
	{
		str[i++]='0';
		str[i]=0;
	}
	else
	{
		end=i-1;			//���һ���ַ�
		i/=2;				//��Ҫ�����Ĵ���
		for(j=0;j<i;j++)
		{
			temp=str[j];
			str[j]=str[end-j];
			str[end-j]=temp;
		}
		str[end+1]=0;
	}
}

/******************************************************************
 - ������������һ���ַ���תΪ32λ�ı���������"1234"תΪ1234
 - ����ģ�飺��������ģ��
 - �������ԣ��ⲿ���û��ɵ���
 - ����˵����str:ָ���ת�����ַ���           
 - ����˵����ת�������ֵ
 ******************************************************************/

unsigned long strtou32(s8 *str) 
{
	u32 temp=0;
	u32 fact=1;
	u8 len=strlen(str);
	u8 i;
	for(i=len;i>0;i--)
	{
		temp+=((str[i-1]-0x30)*fact);
		fact*=10;
	}
	return temp;
}

void Int2Str(s16 i,u8 * str)
{
	if(i<0)
	{
		str[0]='-';
		i=abs(i);
	}
	else
		str[0]='+';

	str[1]=i/10000+'0';
	str[2]=i%10000/1000+'0';
	str[3]=i%1000/100+'0';
	str[4]=i%100/10+'0';
	str[5]=i%10+'0';
}

void Float2Str(float i,u8 * str)
{
	u32 j=0;
	if(i<0)
	{
		str[0]='-';
		i=abs(i);
	}
	else
		str[0]='+';
	i*=1000000;
	j=(u32)i;
	str[1]=j/1000000+'0';
	str[2]=j%1000000/100000+'0';
	str[3]=j%100000/10000+'0';
	str[4]=j%10000/1000+'0';
	str[5]=j%1000/100+'0';
	str[6]=j%100/10+'0';
	str[7]=j%10+'0';
	str[8]='e';
	str[9]='-';
	str[10]='6';
}









