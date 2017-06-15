#ifndef  _MYFUN_H_
#define  _MYFUN_H_

#include "sys.h"

void Delay(u16 time);
void Delay_Us(u16 j);
void Delay_Ms(u16 j);
void u32tostr(u32 dat,s8 *str);		//将一个32位的变量dat转为字符串
u32 strtou32(s8 *str); 				//将一个字符串转为32位的变量
void Int2Str(s16 i,u8 * str);
void Float2Str(float i,u8 * str);

#endif