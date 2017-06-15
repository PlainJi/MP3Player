#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include "sys.h"

sbit mode=P5^3;
sbit vol=P4^7;
sbit enter=P4^3;
sbit next=P5^2;
sbit back=P1^4;

extern data u8 key;

u8 KEY_Value();
u8 KEY_Value4Int();

#endif



