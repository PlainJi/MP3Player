#include "mp3_player.h"
#include "GUI_MP3.h"
#include "pff.h"
#include "filebrowser.h"
#include "gui_filebrowser.h"
#include "myfun.h"
#include "malloc.h"
#include "string.h"
#include "uart.h"
#include "exfun.h"
#include "vs1003.h"
#include "LG4525.h"
#include "timer.h"
#include "stdlib.h"
#include "touch.h"
#include "BMP_Decode.h"
#include "ADC.H"

data u8  LastProBar=0;		//上次进度条位置
data u8  NowProBar=0;		//本次进度条位置
data u16 DecodeTime=0;		//本次解码时长
data u16 LastDecodeTime=0;	//上次解码时长
data u16 TotalTime=0;		//歌曲总时长
data u16 Kbps=0;			//歌曲的位速
data u8  volume=20;			//播放器音量
data u16 playingnow;		//当前曲目号
data u8  playorder;			//1,目录循环;2,随机播放;3,单曲循环
//音效设置
data u8  VS_treble=0x07;
data u8  VS_tfreq=0x0a;
data u8  VS_bass=0x0a;
data u8  VS_bfreq=0x0a;

void MP3_Playing()
{
	UART_SendStr("Playing...\n");
	LCD_DrawBlock(92,21,48,12,0x4228);
	LCD_ShowStrMid(92,21,"正在播放",12,48,Red,Black);
}

void MP3_SetVol()
{
	UART_SendStr("Setting Volume...\n");
	LCD_DrawBlock(92,21,48,12,0x4228);
	LCD_WriteASCIIStr1216(100,21,"VOL=",12,Blue,Black,0);
	LCD_WriteNumLen(100+6*4,21,volume,2,12,Blue,Black,0);
}

//1:Up	0:Down
void MP3_VolUpOrDown(u8 i)
{
	volume=VS_ReadReg(VS_VOL);
	volume=(254-volume)>>3;
	if(i)
		volume++;
	else
		volume--;
	if(volume==32)
		volume=31;
	else if(volume==0)
		volume=1;
	LCD_DrawBlock(100+6*4,21,12,12,0x4228);
	LCD_WriteNumLen(100+6*4,21,volume,2,12,Blue,Black,0);
	UART_PutInf("volume=",volume);
	VS_SetVolume(volume);
}

void MP3_SetPlayOrder()
{
	UART_SendStr("Setting Play Order...\n");
	LCD_DrawBlock(92,21,48,12,0x4228);
	switch(playorder)
	{
		case 1:LCD_ShowStrMid(92,21,"目录循环",12,48,Blue,Black);break;
		case 2:LCD_ShowStrMid(92,21,"随机播放",12,48,Blue,Black);break;
		case 3:LCD_ShowStrMid(92,21,"单曲循环",12,48,Blue,Black);break;
		default:break;
	}
}

void MP3_OrderUpOrDown(u8 i)
{
	if(i)
		playorder++;
	else
		playorder--;
	if(playorder==4)
		playorder=1;
	else if(playorder==0)
		playorder=3;
	LCD_DrawBlock(92,21,48,12,0x4228);
	switch(playorder)
	{
		case 1:	UART_SendStr("Play Order=目录循环\n");
				LCD_ShowStrMid(92,21,"目录循环",12,48,Blue,Black);
				break;
		case 2:	UART_SendStr("Play Order=随机播放\n");
				LCD_ShowStrMid(92,21,"随机播放",12,48,Blue,Black);
				break;
		case 3:	UART_SendStr("Play Order=单曲循环\n");
				LCD_ShowStrMid(92,21,"单曲循环",12,48,Blue,Black);
				break;
		default:break;
	}
}

//更新已播放时间和进度条
void MP3_UpdatePlayedTime()
{
	DecodeTime=VS1003B_ReadDecodeTime();
	if(DecodeTime!=LastDecodeTime)
	{
		GUI_MP3PlayedTime();			//更新已播放时间
		LastDecodeTime=DecodeTime;
		GUI_MP3ShowProBar();			//更新进度条
	}
}

//更新歌曲总时间
void MP3_UpdateTotalTime()
{
	u16 temp;

	temp=VS_Get_HeadInfo();				//返回Kbps
	if(temp!=Kbps)
	{
		Kbps=temp;
		GUI_MP3TotalTime();
	}
}

//确认路径已经定位至music
u8 MP3_CountMusicNum()
{
	u16 temp=0;
	BrowserInfo.filenum=0;					//统计之前先清零
	res = pf_opendir(&dir,&BrowserInfo.path[2]);
    if (res == FR_OK)
	{
        while(1)
		{
            res = pf_readdir(&dir, &fno);
            if(res != FR_OK || fno.fname[0] == 0)
				break;
            if(fno.fattrib & AM_DIR)
			{								//文件夹
				continue;
            }
			else 							//文件
			{
				if(1==Browser_CheckType())	//确认为歌曲文件
				{
					BrowserInfo.filenum++;
				}
            }
		}
		if(BrowserInfo.filenum!=0)			//找到歌曲
			return 0;
		else
		{
			UART_SendStr("No Music in 'music' Folder,Please copy music to the folder 'music' on computer!\n");
			return 0x0f;					//music文件夹里没有歌曲
		}
    }
	else
	{
		UART_SendStr("No Folder 'Music',Please create it on computer!\n");
		return res;
	}
}

//打开第cnt个音乐文件
//1<=cnt<=BrowserInfo.filenum
u8 MP3_OpenMusic(u16 cnt)
{
	pf_opendir(&dir,&BrowserInfo.path[2]);
	if( (cnt>BrowserInfo.filenum) || (cnt==0) )
		return 1;				//超出歌曲总数
	while(cnt)
	{
		res = pf_readdir(&dir, &fno);
        if(res != FR_OK || fno.fname[0] == 0)
		{
			return 2;
		}
        if(fno.fattrib & AM_DIR)
		{								//文件夹
			continue;
        }
		else 							//文件
		{
			if(1==Browser_CheckType())	//确认为歌曲文件
			{
				cnt--;
			}
        }
	}
	Browser_AddName();
	return 0;					//成功
}

//1,下一曲	0,上一曲
//返回值：0成功，其他失败
u8 MP3_NextOrPreMusic(u8 i)
{
	if(ADC_GetResult()<340)		//电量不足
	{
		PowerOff();
	}
	if(playorder==1)			//目录播放
	{
		if(i)
			playingnow++;
		else
			playingnow--;
	}
	else if(playorder==2)		//随机播放
	{
		playingnow=rand()%(BrowserInfo.filenum);
		playingnow++;			//由随机数种子计算曲目号
	}
	else if(playorder==3)		//单曲循环
	{
								//曲目号不做处理
	}
	if(playingnow>BrowserInfo.filenum)
	{
		playingnow=1;						//回到第一首
	}
	else if(playingnow==0)
	{
		playingnow=BrowserInfo.filenum;		//最后一首
	}
	Browser_CutName();						//先cut上一曲
	return MP3_OpenMusic(playingnow);
}

void MP3_ShowMusicNum()
{
	LCD_DrawBlock(40,21,42,12,0x4228);
	LCD_WriteNumLen(40,21,playingnow,3,12,Red,Black,0);
	LCD_WriteASCII1216(40+6*3,21,'/',12,Red,Black,0);
	LCD_WriteNumLen(64,21,BrowserInfo.filenum,3,12,Red,Black,0);
}

void MP3_End()
{
	VS_FlushBuffer();
	VS_SoftReset();
	Kbps=0;
	LastProBar=0;		//上次进度条位置
	NowProBar=0;		//本次进度条位置
	DecodeTime=0;		//本次解码时长
	LastDecodeTime=0;	//上次解码时长
	TotalTime=0;		//歌曲总时长
	VS1003B_SetDecodeTime0();
}

/*
u8 MP3_Browser_OpenFile()
{
	u16 temp;
	temp=BrowserInfo.itemnum+(BrowserInfo.pagenow-1)*9-1;	//当前的条目计数
	if(temp==0)												//返回上级目录
	{
		return 1;
	}
	else if(temp>BrowserInfo.foldernum)						//超过文件夹数目，那肯定是文件了……
	{
		temp-=BrowserInfo.foldernum;
		temp--;
		pf_opendir(&dir,&BrowserInfo.path[2]);
		SkipXFile(temp);									//跳过temp个文件
		GetNextFile();										//打开文件
		temp=Browser_CheckType();							//检测文件类型
		if(temp==1)											//如果是音频文件
			Browser_AddName();
		return 1;											//播放完重新打开浏览器
	}
	else													//是文件夹
	{
		return 1;											//更新路径后重新打开浏览器
	}
}

void MP3_File_Browser()
{
	BrowserInfo.filenum=0;
	BrowserInfo.foldernum=0;
	BrowserInfo.totalpage=0;
	BrowserInfo.pagenow=0;
	BrowserInfo.itemnum=0;
	BrowserInfo.remainder=0;
	strcpy(&BrowserInfo.path[2],"music");				//初始化音乐目录
	LCD_DrawBlock(0,0,176,220,White);					//清屏
	GUI_Box("浏览器","　　欢迎使用文件浏览器。");
	Delay_Ms(1000);
	LCD_DrawBlock(0,0,176,220,White);					//清屏

	UART_SendStr("================================\n");
	if(!Browser_CountFileNum())
	{
		UART_SendStr("Current path: ");
		UART_SendStr(BrowserInfo.path);
		UART_SendEnter();
		LCD_DrawBlock(0,0,176,220,White);
		GUIBrowser_ShowBack();
		GUIBrowser_ShowPath();
		GUIBrowser_ShowNum();
		Browser_ShowItemNum();
		Browser_DrawList(BrowserInfo.pagenow,&BrowserInfo.path[2]);
		GUIBrowser_FanBai(BrowserInfo.itemnum,1);						//第一个条目反白显示
		while(1)
		{
			TP_GetScreenXY(&ScreenX,&ScreenY);
			if(CHECK_TOUCH)												//按下
			{
				if(TP_FILEBROWSER_ITEM)
				{
					if(BrowserInfo.pagenow==BrowserInfo.totalpage)		//如果已经是最后一页
					{													//需要计算有效触摸区域
							if(ScreenY>((BrowserInfo.remainder+2)*17-2))//如果不在有效区域内
								{continue;}								//忽略
					}
					GUIBrowser_FanBai(BrowserInfo.itemnum,0);	//恢复上一个的反白状态
					BrowserInfo.itemnum=(ScreenY-17)/17;
					GUIBrowser_FanBai(BrowserInfo.itemnum,1);	//反白当前
					Browser_ShowItemNum();
				}
				else if(TP_FILEBROWSER_PREPAGE)
				{
					GUIBrowser_FanBai(BrowserInfo.itemnum,0);			//恢复上一个的反白状态
					BrowserInfo.pagenow--;
					if(BrowserInfo.pagenow==0)
						BrowserInfo.pagenow=1;
					Browser_DrawList(BrowserInfo.pagenow,&BrowserInfo.path[2]);
					if(BrowserInfo.totalpage==1)						//如果只有一页
						BrowserInfo.itemnum=BrowserInfo.filenum+BrowserInfo.foldernum+1;
					else
						BrowserInfo.itemnum=9;
					GUIBrowser_FanBai(BrowserInfo.itemnum,1);			//反白当前
					Browser_ShowItemNum();
				}
				else if(TP_FILEBROWSER_NEXTPAGE)
				{
					GUIBrowser_FanBai(BrowserInfo.itemnum,0);			//恢复上一个的反白状态
					BrowserInfo.pagenow++;
					if(BrowserInfo.pagenow>BrowserInfo.totalpage)
						BrowserInfo.pagenow=BrowserInfo.totalpage;
					Browser_DrawList(BrowserInfo.pagenow,&BrowserInfo.path[2]);
					GUIBrowser_FanBai(BrowserInfo.itemnum,1);			//反白当前
					Browser_ShowItemNum();
				}
				else if(TP_FILEBROWSER_OPEN)
				{
					MP3_Browser_OpenFile();
				}
				else if(TP_FILEBROWSER_EXTI)
				{
					break;
				}
			}
			Delay_Ms(100);
		}
	}
	else
	{
		UART_PutInf("Open Folder Filed! res=",res);
	}
}
*/

//0，从系统进入播放器
//1，从文件浏览器进入播放器
u8 MP3_Play(u8 state)
{
	u8 *buff=0;
	u8 i=0;
	u8 PlayPause=0;				//0播放，1暂停
	u8 settings=1;				//1,正在播放;2,设置音量;3,设置播放模式;
	u8 flag=0;
	
	LCD_DrawBlock(0,0,176,220,White);					//清屏
	GUI_Box("MP3 Player","　　欢迎使用MP3播放器。");
	Delay_Ms(1000);
	LCD_DrawBlock(0,0,176,220,White);					//清屏

	GUI_MP3();					//载入初始界面
	volume=VS_ReadReg(VS_VOL);
	volume=(254-volume)>>3;		//初始化音量
	playingnow=1;				//初始化歌曲序号为1
	playorder=1;				//初始化播放顺序为目录循环
	buff=Malloc(512);
	if(buff==0){UART_SendStr("malloc error!\n");return 0x0f;}
	if(!state)					//不是从浏览器进入MP3播放程序的，需要先初始化数量等信息
	{
		strcpy(&BrowserInfo.path[2],"MUSIC");			//路径初始化
		if(!MP3_CountMusicNum())						//统计歌曲数量
		{
			MP3_OpenMusic(playingnow);
		}
		else
		{
			return 1;
		}
	}
	UART_SendStr("================================\n");
	UART_SendStr("Open MP3 Player!\n");
MP3_Start:
	UART_SendStr("================================\n");
	settings=1;					//初始化设置为正在播放
	GUI_MP3PlayedTime();		//已播放时间归零
	res = pf_open(&BrowserInfo.path[2]);
	if(res == FR_OK)
	{
		UART_SendStr("播放音乐文件：");
		UART_SendStr(fno.fname);UART_SendStr("\n");
		GUI_MP3ShowName();
		MP3_ShowMusicNum();								//显示歌曲序号
		MP3_Playing();									//显示“正在播放”
		LCD_DrawBlock(0,169,176,2,Blue);				//上一曲下一曲时需要进度条归零
		flag=0;
		do
		{
			if(!PlayPause)								//没有被暂停正常播放
			{
				res=pf_read(buff,512,&br);
				for(i=0;i<16;i++)
					VS_SendDat32(buff+i*32);
//更新播放时间、进度条			
				flag++;
				if(flag%15==0)
				{
					MP3_UpdatePlayedTime();
					MP3_UpdateTotalTime();
				}
				if(res || br == 0)
				{
					UART_SendStr("Play Over!\n");
					if(state)
						settings=0x20;
					else
					{
						res=MP3_NextOrPreMusic(1);
						if(res)//下一曲
							{UART_SendStr("Next Music Error!\n");settings=0x20;}
						else					//从系统进入播放结束继续播放下一曲
							{settings=0x10;}
					}
				}
			}
			TP_GetScreenXY(&ScreenX,&ScreenY);
			if(CHECK_TOUCH)											//按下
			{
				if(pingbaoflag==1)									//如果屏保已打开，则关闭
				{
					if(TP_HOME)
					{
						ResumePingBao();
						Delay_Ms(200);
					}
				}
				else
				{
					if(TP_MP3_PLAY)
					{
						PlayPause^=1;				//播放暂停按钮
						Delay_Ms(200);
						if(PlayPause)
							{GUI_MP3Pause();UART_SendStr("Pause...\n");}
						else
							{GUI_MP3Play();UART_SendStr("Play...\n");}
					}
					else if(TP_MP3_PRE)
					{
						if(settings==1)				//1时为切歌，上一曲
						{
							if(!state)
							{
								res=MP3_NextOrPreMusic(0);
								if(res)
									{UART_SendStr("Pre Music Error!\n");settings=0x20;}
								else
									{settings=0x10;}
							}
						}
						else if(settings==2)		//2时为调节音量，音量+1
							{MP3_VolUpOrDown(0);}
						else if(settings==3)		//3时为调节播放模式
							{MP3_OrderUpOrDown(0);}
						Delay_Ms(10);
					}
					else if(TP_MP3_NEXT)
					{
						if(settings==1)				//1时为切歌，下一曲
						{
							if(!state)
							{
								res=MP3_NextOrPreMusic(1);
								if(res)
									{UART_SendStr("Next Music Error!\n");settings=0x20;}
								else
									{settings=0x10;}
							}
						}
						else if(settings==2)		//2时为调节音量，音量+1
							{MP3_VolUpOrDown(1);}
						else if(settings==3)		//3时为调节播放模式
							{MP3_OrderUpOrDown(1);}
						Delay_Ms(10);
					}
					else if(TP_MP3_EXIT)
					{
						settings=0x20;				//退出播放器
					}
					else if(TP_MP3_CHANGE)
					{
						settings++;					//设置按钮
						if(settings==4)settings=1;	//123循环
						switch(settings)
						{
							case 1:MP3_Playing();break;
							case 2:MP3_SetVol();break;
							case 3:MP3_SetPlayOrder();break;
							default:break;
						}
						Delay_Ms(10);
					}
					else if(TP_HOME)
					{
						PingBao();
						Delay_Ms(200);
					}
//					else if(TP_mp3_LIST)
//					{
//						MP3_File_Browser();			//有问题，打开本句启动MP3播放和文件浏览功能都会使机器重启
//						settings=0x10;
//					}
#ifdef _ENABLE_CATCH_SCREEN
					else if(TP_CATCH)
					{
						APP_CatchScreen();
					}
#endif
				}
			}
			if((settings&0x30)!=0)				//需要跳转到开始或者结束位置
				break;
		}while(1);
	}
	else
		UART_SendStr("Open File Filed,does it exist?\n");
//在此处跳转到MP3_Start
	MP3_End();
	if(settings==0x10)
		goto MP3_Start;

	Free(buff);
	Browser_CutName();
	GUI_Box("MP3 PlAYER","　　退出MP3播放器。");
	Delay_Ms(1000);
	LCD_DrawBlock(0,0,176,220,White);
	UART_SendStr("Exit MP3 Player!\n");

	return 0;
}



