#include "GUI_E_Book.h"
#include "LG4525.h"
#include "GUI_FileBrowser.h"
#include "FileBrowser.h"
#include "exfun.h"

void GUIEBook_ShowTitle()
{
	LCD_ShowStrMid(0,1,fno.fname,16,176,String_Color,Black);
}

void GUIEBook_DrawBarBack()
{
	GUIBrowser_ButtonUp(156,18);
	GUIBrowser_ButtonDown(156,200);
	LCD_DrawBlock(156,38,20,162,0xce59);	//240	0xF79E
}

//
void GUIEBook_ProgessBar()
{
	GUIEBook_DrawBarBack();
	GUIBrowser_DrawBar(156,38,162,BrowserInfo.pagenow,BrowserInfo.totalpage);
}

void GUIEBook_Back()
{
	u8 temp;
	LCD_DrawBlock(0,18,176,202,White);					//清屏
	for(temp=1;temp<15;temp++)							//分割线
		LCD_DrawBlock(4,20+14*temp,144+18+6,1,0xb2c0);
	BrowserInfo.pagenow=3;
	BrowserInfo.totalpage=3;
//	GUIEBook_ProgessBar();

}

//14*14=196个字（378字节，申请384字节呗）
void GUI_EBook()
{
	LCD_DrawBlock(0,0,176,220,White);					//清屏
	LCD_ColourBar(0,0,176,9,0,1);
	LCD_ColourBar(0,9,176,9,30,0);						//标题用
	LCD_ShowStr(0,3,36,12,"上一篇",12,Red,Blue,0);
	LCD_ShowStr(140,3,36,12,"下一篇",12,Red,Blue,0);
}

















