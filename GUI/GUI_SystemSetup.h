#ifndef __GUI_SYSTEM_SETUP_H
#define __GUI_SYSTEM_SETUP_H

#include "sys.h"

void GUI_SystemSetup();
void GUI_SystemInfo();
void GUI_MessageBox(u8 *title);
void GUI_SetLight();
void GUI_DrawLightBar(u8 val);
void GUI_SetVolume();
void GUI_ShowVol(u8 val);
void GUI_UpdateBIN();
void GUI_StartUpdate();
void GUI_SetEffect();
void GUI_DrawBar(u8 y,u8 val);
void GUI_ShowEffect1(u8 val);
void GUI_ShowEffect2(u8 val);
void GUI_ShowEffect3(u8 val);
void GUI_ShowEffect4(u8 val);

#endif