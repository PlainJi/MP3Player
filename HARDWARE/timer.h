#ifndef __TIMER_H
#define __TIMER_H

#include "sys.h"

extern data u16 second;			//����ʱ�䣿
extern idata u8 pingbaotime;
extern idata u8 pingbaoflag;

void InitTimer0(void);
void ResumePingBao();
void PingBao();

#endif