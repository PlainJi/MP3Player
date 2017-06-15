#include "E_Book.h"
#include "GUI_E_Book.h"
#include "LG4525.h"
#include "pff.h"
#include "malloc.h"
#include "uart.h"
#include "exfun.h"
#include "filebrowser.h"
#include "gui_filebrowser.h"
#include "touch.h"
#include "myfun.h"
#include "string.h"
#include "mp3_player.h"
#include "BMP_Decode.h"
#include "timer.h"

u8 *buff=0;
idata u32 last_page=0;						//用于保存上一页的偏移量
idata u32 this_page=0;						//用于保存本页的偏移量

u8 Ebook_NextPage()
{
	u8 flag=0;
	u16 have_show=0;
//检测
	if(fs.fptr==0)							//第一次进入
		flag=1;
	else if(fs.fptr==fs.fsize)				//已经显示完毕
		return 1;
	else
		flag=0;
//初始化内存及界面
	for(have_show=0;have_show<392;have_show++)
		buff[have_show]=0;
	GUIEBook_Back();
//读取并显示
	res=pf_lseek(this_page);
	res=pf_read(buff,392,&br);				//读取392个字节
	have_show=LCD_ShowStr(4,22,144+18+6,196,buff,12,Black,White,0);
	pf_lseek(this_page+have_show);			//修正文件指针
	last_page=this_page;
	this_page+=have_show;
	if(res || br == 0)
		return 2;
	return 0;
}

//确认路径已经定位至music
u8 EBOOK_CountFileNum()
{
	u16 temp=0;
	BrowserInfo.filenum=0;					//电子书数量清零

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
				if(3==Browser_CheckType())	//确认为文本文件
				{
					BrowserInfo.filenum++;
				}
            }
		}
		if(BrowserInfo.filenum!=0)			//找到电子书
			return 0;
		else
		{
			UART_SendStr("No file in Folder 'EBOOK',Please copy TXT file to the folder 'ebook' on computer!\n");
			return 0x0f;
		}
    }
	else
	{
		UART_SendStr("No Folder 'ebook',Please create it on computer!\n");
		return res;
	}
}

//打开第cnt个音乐文件
//1<=cnt<=BrowserInfo.filenum
u8 EBOOK_OpenFile(u16 cnt)
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
			if(3==Browser_CheckType())	//确认为文本文件
			{
				cnt--;
			}
        }
	}
	Browser_AddName();
	return 0;					//成功
}

u8 EBOOK_NextOrPreFile(u8 i)
{
	if(i)
		playingnow++;
	else
		playingnow--;
	
	if(playingnow>BrowserInfo.filenum)
	{
		playingnow=1;							//回到第一首
	}
	else if(playingnow==0)
	{
		playingnow=BrowserInfo.filenum;		//最后一首
	}
	Browser_CutName();						//先cut上一曲
	return EBOOK_OpenFile(playingnow);
}

/*
void Ebook_PrePage()
{
	u16 have_show=0;

	for(have_show=0;have_show<392;have_show++)
		buff[have_show]=0;
	GUIEBook_Back();

	res=pf_lseek(last_page);
	res=pf_read(buff,392,&br);			//读取336个字节
	if(res || br == 0)
		return;
	have_show=LCD_ShowStr(4,22,144+18+6,196,buff,12,Black,White,0);

	
//	this_page=last_page;
//	this_page-=have_show;

	UART_PutInf("have_show=",have_show);
	UART_PutInf("last_page=",last_page);
	UART_PutInf("this_page=",this_page);
}
*/

u8 Ebook(u8 state)
{
	u8 settings;

	LCD_DrawBlock(0,0,176,220,White);					//清屏
	GUI_Box("E-BOOK","　　欢迎使用电子书。");
	Delay_Ms(1000);
	LCD_DrawBlock(0,0,176,220,White);					//清屏

	buff=Malloc(392);
	if(buff==0){UART_SendStr("malloc error!\n");return 0x0f;}
	playingnow=1;				//初始化文本序号为1
	if(!state)					//不是从浏览器进入电子书程序的，需要先初始化数量等信息
	{
		strcpy(&BrowserInfo.path[2],"EBOOK");			//路径初始化
		if(!EBOOK_CountFileNum())						//统计文本文件数量
		{
			EBOOK_OpenFile(playingnow);
			GUI_Box("E-Book","　　将为您打开第1个电子书。");
			Delay_Ms(1000);
		}
		else
		{
			return 1;
		}
	}
	UART_SendStr("================================\n");
	UART_SendStr("Open E-Book!\n");
EBOOK_Start:
	settings=0;
	LCD_DrawBlock(0,0,176,220,White);					//清屏
	GUI_EBook();										//初始界面
	res = pf_open(&BrowserInfo.path[2]);
	if(res == FR_OK)
	{
		UART_SendStr("打开文本文件：");
		UART_SendStr(fno.fname);UART_SendStr("\n");
		GUIEBook_ShowTitle();
		last_page=0;
		this_page=0;
		Ebook_NextPage();
		do
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
					if(TP_EXIT)
					{
						settings=0x20;
					}
					else if(TP_EBOOK_PREPAGE)
					{
						
					}
					else if(TP_EBOOK_NEXTPAGE)
					{
						res=Ebook_NextPage();
						if(res!=1)
							UART_SendStr("下一页\n");								//下一页
					}
					else if(TP_EBOOK_PREONE)
					{
						if(!state)
						{
							if(EBOOK_NextOrPreFile(0))
								{UART_SendStr("Pre Book Error!\n");settings=0x20;}	//退出电子书
							else
								{settings=0x10;}									//回到电子书开始处
						}
					}
					else if(TP_EBOOK_NEXTONE)
					{
						if(!state)
						{
							if(EBOOK_NextOrPreFile(1))
								{UART_SendStr("Next Book Error!\n");settings=0x20;}	//退出电子书
							else
								{settings=0x10;}									//回到电子书开始处
						}
					}
#ifdef _ENABLE_CATCH_SCREEN
					else if(TP_CATCH)
					{
						APP_CatchScreen();
					}
#endif
				}
			}
			if((settings&0x30)!=0)						//请求退出
				break;
		}while(1);
	}
	else
		UART_SendStr("Open File Filed,does it exist?\n");

	if(settings==0x10)
		goto EBOOK_Start;
	Free(buff);
	Browser_CutName();
	GUI_Box("E-Book","　　退出电子书。");
	Delay_Ms(1000);
	LCD_DrawBlock(0,0,176,220,White);
	UART_SendStr("Exit E-Book!\n");

	return 0;
}