#ifndef __MSPAINT_H
#define __MSPAINT_H

#include "sys.h"
#define TP_PAINT_PAPER	(ScreenX>0   && ScreenX<176 && ScreenY>0   && ScreenY<204)
#define TP_PAINT_COLOR	(ScreenX>0   && ScreenX<160 && ScreenY>204 && ScreenY<220)
#define TP_PAINT_CLEAR	(ScreenX>160 && ScreenX<176 && ScreenY>204 && ScreenY<220)

void Paint();

#endif