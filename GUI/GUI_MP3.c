#include "GUI_MP3.h"
#include "mp3_player.h"
#include "LG4525.h"
#include "BMP_Decode.h"
#include "BmpTable.h"
#include "exfun.h"
#include "spiflash.h"

void GUI_MP3Play()
{
	LCD_DrawBlock(64,174,44,44,0x10a2);
	LCD_Show_Anniu(64,174,44,44,gImage_pause);
}

void GUI_MP3Pause()
{
	LCD_DrawBlock(64,174,44,44,0x10a2);
	LCD_Show_Anniu(64,174,44,44,gImage_play);
}

//已播放时间
void GUI_MP3PlayedTime()
{
	u16 temp;

	LCD_DrawBlock(1,171,30,12,0x10a2);
	temp=DecodeTime/60;
	LCD_WriteNumLen(1,171,temp,2,12,Red,Black,0);
	LCD_WriteASCII1216(13,171,':',12,Red,Black,0);
	temp=DecodeTime%60;
	LCD_WriteNumLen(19,171,temp,2,12,Red,Black,0);
}

//总时间
void GUI_MP3TotalTime()
{
	u16 temp;

	TotalTime=fs.fsize/Kbps/128;
	LCD_DrawBlock(146,171,30,12,0x10a2);
	temp=TotalTime/60;
	LCD_WriteNumLen(146,171,temp,2,12,Red,Black,0);
	LCD_WriteASCII1216(158,171,':',12,Red,Black,0);
	temp=TotalTime%60;
	LCD_WriteNumLen(164,171,temp,2,12,Red,Black,0);
}

void GUI_MP3ShowProBar()
{
	NowProBar=fs.fptr*176/fs.fsize;
	if(NowProBar!=LastProBar)
	{
		LCD_DrawBlock(LastProBar,169,NowProBar-LastProBar,2,Red);//Green
		LastProBar=NowProBar;
	}
}

void GUI_MP3ShowName()
{
	LCD_DrawBlock(50,6,80,12,0x4228);
	LCD_ShowStrMid(0,6,fno.fname,12,176,Red,Black);
}

void GUI_MP3()
{
//退出、列表
	LCD_DrawBlock(0,0,176,37,0x4228);
	LCD_Show_Anniu(3,3,32,32,gImage_exit);
//	LCD_Show_Anniu(141,3,32,32,gImage_list);
//播放、上一曲、下一曲
	LCD_DrawBlock(0,171,176,49,0x10a2);
	LCD_Show_Anniu(16,185,32,32,gImage_pre);
	LCD_Show_Anniu(128,185,32,32,gImage_next);
	GUI_MP3Play();
//进度条
	LCD_DrawBlock(0,169,176,2,Blue);//Gray
	NowProBar=0;
	LastProBar=0;
	DecodeTime=0;		//本次解码时长
	LastDecodeTime=0;	//上次解码时长
	TotalTime=0;
	GUI_MP3PlayedTime();//更新初始时间
//背景图片
	APP_ShowPicture(0,37,"system/MP3_BACK/back3.bmp",0);
}





