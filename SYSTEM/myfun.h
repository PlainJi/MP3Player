#ifndef  _MYFUN_H_
#define  _MYFUN_H_

#include "sys.h"

void Delay(u16 time);
void Delay_Us(u16 j);
void Delay_Ms(u16 j);
void u32tostr(u32 dat,s8 *str);		//��һ��32λ�ı���datתΪ�ַ���
u32 strtou32(s8 *str); 				//��һ���ַ���תΪ32λ�ı���
void Int2Str(s16 i,u8 * str);
void Float2Str(float i,u8 * str);

#endif