#ifndef __GUI_FILEBROWSER_H
#define __GUI_FILEBROWSER_H

#include "sys.h"

void GUIBrowser_ShowBack();
void GUIBrowser_ShowPath();
void GUIBrowser_ShowNum();
void GUIBrowser_ShowIcon(u8 *type,u8 num);
void GUIBrowser_ButtonUp(u8 x,u8 y);
void GUIBrowser_ButtonDown(u8 x,u8 y);
void GUIBrowser_DrawBarBack();
void GUIBrowser_DrawBar(u8 x,u8 y,u8 len,u8 num,u8 total);
void GUIBrowser_ProgessBar();
void GUIBrowser_FanBai(u8 num,u8 mode);
void GUIBrowser_ShowItemNum(u16 num,u8 state);
void GUI_Box(u8 *title,u8 *str);

void TEST_TFTCopy();
void TEST_LCDAlpha();

#endif