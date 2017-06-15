#ifndef __SYSTEM_SETUP_H
#define __SYSTEM_SETUP_H

#include "sys.h"
#define TP_LIGHT	(ScreenX>0 && ScreenX<(176) && ScreenY>(23+19*0) && ScreenY<(23+19*0+16))
#define TP_VOLUME	(ScreenX>0 && ScreenX<(176) && ScreenY>(23+19*1) && ScreenY<(23+19*1+16))
#define TP_EFFECT	(ScreenX>0 && ScreenX<(176) && ScreenY>(23+19*2) && ScreenY<(23+19*2+16))
#define TP_UPDATE	(ScreenX>0 && ScreenX<(176) && ScreenY>(23+19*3) && ScreenY<(23+19*3+16))
#define TP_ABOUT	(ScreenX>0 && ScreenX<(176) && ScreenY>(23+19*4) && ScreenY<(23+19*4+16))
#define TP_EXIT_LIGHT	(ScreenX>133 && ScreenX<149 && ScreenY>122 && ScreenY<138)
#define TP_MOVE_LIGHT	(ScreenX>=33 && ScreenX<=143&& ScreenY>140 && ScreenY<186)
#define TP_UPDATE_YES	(ScreenX>=50 && ScreenX<=82 && ScreenY>167 && ScreenY<187)
#define TP_UPDATE_NO	(ScreenX>=98 && ScreenX<=130&& ScreenY>167 && ScreenY<187)
#define TP_MOVE_1		(ScreenX>=18 && ScreenX<=163&& ScreenY>60  && ScreenY<90)
#define TP_MOVE_2		(ScreenX>=18 && ScreenX<=163&& ScreenY>90 && ScreenY<120)
#define TP_MOVE_3		(ScreenX>=18 && ScreenX<=163&& ScreenY>140 && ScreenY<170)
#define TP_MOVE_4		(ScreenX>=18 && ScreenX<=163&& ScreenY>170 && ScreenY<200)

void SystemSetup();

#endif