#ifndef __FILEBROWSER_H
#define __FILEBROWSER_H

#include "sys.h"
#define TP_FILEBROWSER_ITEM		(ScreenX>0   && ScreenX<160 && ScreenY>34 && ScreenY<187)
#define TP_FILEBROWSER_PREPAGE	(ScreenX>156 && ScreenX<176 && ScreenY>33 && ScreenY<53 )
#define TP_FILEBROWSER_NEXTPAGE	(ScreenX>156 && ScreenX<176 && ScreenY>167&& ScreenY<187)
#define TP_FILEBROWSER_OPEN		(ScreenX>2   && ScreenX<34  && ScreenY>203&& ScreenY<219)
#define TP_FILEBROWSER_EXTI		(ScreenX>143 && ScreenX<175 && ScreenY>203&& ScreenY<219)

typedef struct
{
	u16 filenum;
	u16 foldernum;
	u8 totalpage;
	u8 pagenow;
	u8 itemnum;
	u8 remainder;
	u8 path[30];	//"C:music/·ç´µÂóÀË.mp3"
}_BrowserInfo;
extern idata _BrowserInfo BrowserInfo;

u8 Browser_CountFileNum();
void Browser_DrawItem(u8 num);
void Browser_DrawList(u8 page,u8 *path);
u8 Browser_OpenFile();
void Browser_ShowItemNum();
void File_Browser(u8 *scanpath);

#endif