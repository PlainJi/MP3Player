#ifndef __GUI_INDEX_H
#define __GUI_INDEX_H

#include "sys.h"
#define TP_INDEX_MUSIC		(ScreenX>20 && ScreenX<70 && ScreenY>10 && ScreenY<60)
#define TP_INDEX_EBOOK		(ScreenX>105&& ScreenX<155&& ScreenY>10 && ScreenY<60)
#define TP_INDEX_PICTURE	(ScreenX>20 && ScreenX<70 && ScreenY>78 && ScreenY<128)
#define TP_INDEX_BROWSER	(ScreenX>105&& ScreenX<155&& ScreenY>78 && ScreenY<128)
#define TP_INDEX_ATTACH		(ScreenX>20 && ScreenX<70 && ScreenY>147&& ScreenY<197)
#define TP_INDEX_SETUP		(ScreenX>105&& ScreenX<155&& ScreenY>147&& ScreenY<197)

void Index();

#endif