#include "GUI_Index.h"
#include "LG4525.h"
#include "touch.h"
#include "FileBrowser.h"
#include "MP3_Player.h"
#include "e_book.h"
#include "BMP_Decode.h"
#include "mspaint.h"
#include "SystemSetup.h"
#include "BMP_Decode.h"
#include "ADC.h"
#include "myfun.h"
#include "timer.h"
#include "pwm.h"
#include "spiflash.h"

//4.10V以上为满电
//3.35V以下为空电
//75/15=5
void GUI_IndexShowBattery(u16 voltage)
{
	if(voltage<335)
		voltage=335;
	LCD_DrawBlock(156,2,17,8,White);
	LCD_DrawBlock(173,4,1,4,White);
	if(voltage>410)							//4.05V以上视为满电
	{
		LCD_DrawBlock(157,3,15,6,Green);
	}
	else									//4.05V以下
	{
		voltage-=335;
		voltage/=5;
		if(voltage>10)
			LCD_DrawBlock(157,3,voltage,6,Green);
		else if(voltage>5)
			LCD_DrawBlock(157,3,voltage,6,Tuhuang);
		else
			LCD_DrawBlock(157,3,voltage,6,Red);
	}
}

//在屏幕最上方显示信息栏
void GUI_IndexShowInfBar()
{
	LCD_AlphaWithColor(0,0,176,12,0x0000,2);
	LCD_ShowStr(1,0,36,12,"iPlain",12,White,Black,0);
}

void Index()
{
	idata u8 cnt=0;
IndexStart:
//主页面背景
	LCD_Clear(White);
	LCD_Show_Anniu(0,0,176,220,gImage_BackWhite);
//应用程序入口
	LCD_Show_Anniu(20 ,10 +4,50,50,gImage_music);
	LCD_ShowStrMid(20 ,62 +4+2,"播放器",12,50,Blue,White);
	LCD_Show_Anniu(105,10 +4,50,50,gImage_ebook);
	LCD_ShowStrMid(105,62 +4+2,"电子书",12,50,Blue,White);

	LCD_Show_Anniu(20 ,78 +4  ,50,50,gImage_picture);
	LCD_ShowStrMid(20 ,130+4+2,"图 库",12,50,Blue,White);
	LCD_Show_Anniu(105,78 +4  ,50,50,gImage_browser);
	LCD_ShowStrMid(105,130+4+2,"浏览器",12,50,Blue,White);

	LCD_Show_Anniu(20 ,147+4  ,50,50,gImage_attach);
	LCD_ShowStrMid(20 ,199+4+2,"附 件",12,50,Blue,White);		//关于
	LCD_Show_Anniu(105,147+4  ,50,50,gImage_sys);
	LCD_ShowStrMid(105,199+4+2,"设 置",12,50,Blue,White);		//画图、计算器等

	GUI_IndexShowInfBar();
	GUI_IndexShowBattery(ADC_GetResult());

	while(1)
	{
		TP_GetScreenXY(&ScreenX,&ScreenY);
		if(CHECK_TOUCH)											//按下
		{
			
			if(pingbaoflag==1)									//如果屏保已打开，则关闭
			{
				if(TP_HOME)
					ResumePingBao();
			}
			else
			{
				if(TP_INDEX_MUSIC)
				{
					MP3_Play(0);
				 	goto IndexStart;
				}
				else if(TP_INDEX_EBOOK)
				{
					Ebook(0);
					goto IndexStart;
				}
				else if(TP_INDEX_PICTURE)
				{
					BMP_SHOWPIC(0);
					goto IndexStart;
				}
				else if(TP_INDEX_BROWSER)
				{
					File_Browser("");
					goto IndexStart;
				}
				else if(TP_INDEX_ATTACH)
				{
					Paint();
					goto IndexStart;
				}
				else if(TP_INDEX_SETUP)
				{
					SystemSetup();
					goto IndexStart;
				}
#ifdef _ENABLE_CATCH_SCREEN
				else if(TP_CATCH)
				{
					APP_CatchScreen();
				}
#endif
			}
		}
		Delay_Ms(10);
		if(++cnt==100)								//1s刷新一次电池电量
		{
			GUI_IndexShowBattery(ADC_GetResult());
		}
		CheckDownload;
	}
}






































