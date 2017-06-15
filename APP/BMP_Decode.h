#ifndef __BMP_DECODE_H__
#define __BMP_DECODE_H__

#include "sys.h"

#define BUFFSIZE	255 //每次读出BMP数据大小 一定为3的倍数
#define DISWIDTH	176 //设置显示宽度
#define DISHEIGHT	220 //设置显示高度
#define TP_SHOWPIC_PRE  (ScreenX>0   && ScreenX<176 && ScreenY>0   && ScreenY<110)
#define	TP_SHOWPIC_NEXT (ScreenX>0   && ScreenX<176 && ScreenY>110 && ScreenY<220)

u8 BmpDecode(u8 x,u8 y,u8 center);
u8 APP_ShowPicture(u8 x,u8 y,u8 *lujing,u8 center);
u8 APP_ReadScreen();
u8 BMP_SHOWPIC(u8 state);
u8 APP_CatchScreen();

#endif