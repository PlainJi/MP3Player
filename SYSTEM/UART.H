#ifndef __UART_H__
#define __UART_H__

#include "sys.h"

//UART Ïà¹Øº¯Êý------------------------
void UART_Init(u16 bps);
void UART_SendByte(u8 mydata);
void UART_SendStr(s8 *s);
void UART_SendEnter();
void UART_PutNum(u32 dat);
void UART_PutHex(u16 hex);
void UART_PutInf(s8 *inf,u32 dat);
void Binary(u8 dat);
void UART_PutBuildingTime();
//UART----END-------------------------------

#endif