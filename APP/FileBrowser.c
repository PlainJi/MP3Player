#include "FileBrowser.h"
#include "GUI_FileBrowser.h"
#include "exfun.h"
#include "uart.h"
#include "string.h"
#include "myfun.h"
#include "LG4525.h"
#include "mp3_player.h"
#include "e_book.h"
#include "BMP_Decode.h"
#include "touch.h"
#include "timer.h"

idata _BrowserInfo BrowserInfo;	//文件浏览器控制参数集合

u8 Browser_CountFileNum()
{
	u16 temp=0;

	res = pf_opendir(&dir,&BrowserInfo.path[2]);
    if (res == FR_OK) 
	{
        while(1)
		{
            res = pf_readdir(&dir, &fno);
            if(res != FR_OK || fno.fname[0] == 0) 
				break;
            if(fno.fattrib & AM_DIR) 
			{				//文件夹
				BrowserInfo.foldernum++;
            }
			else 
			{									//文件
				BrowserInfo.filenum++;
            }
		}
		temp=BrowserInfo.filenum+BrowserInfo.foldernum+1;		//+1为“返回上级目录”
		BrowserInfo.totalpage=temp/9;
		if(temp%9!=0)
			BrowserInfo.totalpage++;
		BrowserInfo.pagenow=1;									//初始化第1页为当前页
		BrowserInfo.itemnum=1;
		BrowserInfo.remainder=temp%9;							//最后一页的余数
		return 0;
    }
	else
	{
		UART_SendStr("Open Filed!\n");
		return res;
	}
}

//根据文件信息结构体画出本条目
//显示文件名，显示图标，显示文件大小
//num范围：0-9，0显示返回上级菜单
void Browser_DrawItem(u8 num)
{
	u8 temp=0;
	u32 size=0;

	if(num>9)
	{
		UART_SendStr("Item num overflow!");
		return;
	}
	if(num==0)					//返回，显示文件夹图标、“...”
	{
		GUIBrowser_ShowIcon(0,1);
		LCD_ShowStr(22,37,80,13,"...",12,Black,White,0);
		return;
	}
	if(fno.fattrib & AM_DIR) 	//文件夹，显示文件夹图标、文件夹名
	{
		GUIBrowser_ShowIcon(0,num);
		LCD_ShowStr(22,20+17*num,80,13,fno.fname,12,Black,White,0);
	}
	else						//文件，显示文件图标、文件名、文件大小
	{
		while(fno.fname[temp++]!='.');
		GUIBrowser_ShowIcon(&fno.fname[temp],num);
		LCD_ShowStr(22,20+17*num,80,13,fno.fname,12,Black,White,0);		//显示文件名
		if(fno.fsize<1024U)
		{
			size=fno.fsize;
			LCD_ShowStr(128,20+17*num,12,13,"Bs",12,Black,White,0);
		}
		else if(fno.fsize<1048576U)
		{
			size=fno.fsize/1024;
			LCD_ShowStr(128,20+17*num,12,13,"KB",12,Black,White,0);
		}
		else if(fno.fsize<1073741824U)
		{
			size=fno.fsize/1024/1024;
			LCD_ShowStr(128,20+17*num,12,13,"MB",12,Black,White,0);
		}
		else
		{
			size=fno.fsize/1024/1024/1024;
			LCD_ShowStr(128,20+17*num,12,13,"GB",12,Black,White,0);
		}
		LCD_WriteNumLen(110,20+17*num,size,3,12,Black,White,0);		//文件大小
	}
}

//根据页数，画出本页的各条目信息
//先根据页数算出本页要画的文件夹数，文件数，是否画返回上级菜单
void Browser_DrawList(u8 page,u8 *path)
{
	u8 temp=0,itemnum=0;
	u8 file2draw=0,folder2draw=0;
	u16 shownfile=0,shownfolder=0;

	if(page>BrowserInfo.totalpage)
	{
		UART_SendStr("page overflow!\n");
		return;
	}
	else
		UART_PutInf("now page=",BrowserInfo.pagenow);
	if(page==1)								//如果是第一页，需要显示返回上级菜单
	{
		if(BrowserInfo.foldernum>8)
		{
			folder2draw=8;
			file2draw=0;
		}
		else
		{
			folder2draw=BrowserInfo.foldernum;
			temp=8-folder2draw;				//剩余的条目数
			if(BrowserInfo.filenum>temp)
				file2draw=temp;
			else
				file2draw=BrowserInfo.filenum;
		}
		itemnum=2;							//初始化为2
		if(pf_opendir(&dir,path))			//打开文件夹
			return;
		LCD_DrawBlock(0,33,176,154,White);	//清空列表区域
		Browser_DrawItem(0);				//在第一页画出返回上级菜单
	}
	else									//不为第一页的
	{
//先计算已经显示的总数量
		temp=(page-2)*9+8;					//计算前(page-1)页可以显示的最大文件夹数量
		if(BrowserInfo.foldernum< temp)		//已经显示完了
			shownfolder=BrowserInfo.foldernum;
		else
			shownfolder=temp;				//没显示完就=可以显示的最大数量
		shownfile=temp-shownfolder;			//已经显示的文件数

//计算本页要显示的文件夹数和文件数
		folder2draw=BrowserInfo.foldernum-shownfolder;
		if(pf_opendir(&dir,path))			//打开文件夹
			return;
		if(folder2draw!=0)
		{
			SkipXFolder(shownfolder);		//跳过shownfolder个文件夹
		}
		else
		{
			SkipXFile(shownfile);			//跳过shownfile个文件
		}
		if(folder2draw>9)
		{
			folder2draw=9;
			file2draw=0;
		}
		else
		{
			file2draw=9-folder2draw;
			if( file2draw>(BrowserInfo.filenum-shownfile) )
				file2draw=BrowserInfo.filenum-shownfile;
		}
		itemnum=1;							//初始化为1
		LCD_DrawBlock(0,33,176,154,White);	//清空列表区域
	}
//根据file2draw，folder2draw画出列表
	for(temp=0;temp<folder2draw;temp++)
	{
		GetNextFolder();
		Browser_DrawItem(itemnum++);
	}
//判断是否需要重新打开文件夹
	if(folder2draw!=0 && file2draw!=0)
		if(pf_opendir(&dir,path))
			return;
	for(temp=0;temp<file2draw;temp++)
	{
		GetNextFile();
		Browser_DrawItem(itemnum++);
	}
	GUIBrowser_ProgessBar();
	BrowserInfo.itemnum=1;
}

//显示文件或文件夹的序号
void Browser_ShowItemNum()
{
	u16 temp;
	temp=BrowserInfo.itemnum+(BrowserInfo.pagenow-1)*9-1;	//当前的条目计数
	if(temp==0)
	{
		GUIBrowser_ShowItemNum(0,1);						//文件数归零
		GUIBrowser_ShowItemNum(0,0);						//文件夹数归零
	}
	else if(temp>BrowserInfo.foldernum)						//当前条目是文件
	{
		temp-=BrowserInfo.foldernum;						//文件的序号
		GUIBrowser_ShowItemNum(temp,1);						//显示文件序号
		GUIBrowser_ShowItemNum(0,0);
	}
	else
	{
		GUIBrowser_ShowItemNum(temp,0);						//显示文件夹序号
		GUIBrowser_ShowItemNum(0,1);
	}
}

//通过pagenow和itemnum打开文件或文件夹
//返回值：1，为文件夹，需要再次打开
//获得条目名
//分析是否为支持的格式
//调用相应的应用程序
u8 Browser_OpenFile()
{
	u16 temp;
	temp=BrowserInfo.itemnum+(BrowserInfo.pagenow-1)*9-1;	//当前的条目计数
	if(temp==0)												//返回上级目录
	{
		UART_SendStr("返回上级目录...\n");
		Browser_CutName();
		BrowserInfo.filenum=0;
		BrowserInfo.foldernum=0;
		BrowserInfo.totalpage=0;
		return 1;										//返回上级目录，重新打开浏览器
	}
	else if(temp>BrowserInfo.foldernum)					//超过文件夹数目，那肯定是文件了……
	{
		temp-=BrowserInfo.foldernum;
		temp--;
		pf_opendir(&dir,&BrowserInfo.path[2]);
		SkipXFile(temp);								//跳过temp个文件
		GetNextFile();									//打开文件
		temp=Browser_CheckType();						//检测文件类型
		switch(temp)
		{
			case 1:	Browser_AddName();
					UART_SendStr("打开MP3播放器！\n");
					MP3_Play(1);
					break;
			case 2:	Browser_AddName();
					BMP_SHOWPIC(1);
					break;
			case 3:	Browser_AddName();
					UART_SendStr("打开电子书！\n");
					Ebook(1);
					break;
			default:break;
		}
		BrowserInfo.filenum=0;
		BrowserInfo.foldernum=0;
		BrowserInfo.totalpage=0;
		return 1;										//播放完重新打开浏览器
	}
	else												//是文件夹
	{
		pf_opendir(&dir,&BrowserInfo.path[2]);
		SkipXFolder(temp-1);							//跳过temp个文件
		GetNextFolder();								//打开文件
		Browser_AddName();
		UART_SendStr("Open Folder:");
		UART_SendStr(fno.fname);
		UART_SendStr("\n");
		BrowserInfo.filenum=0;
		BrowserInfo.foldernum=0;
		BrowserInfo.totalpage=0;
		return 1;										//更新路径后重新打开浏览器
	}
}

//文件浏览器
void File_Browser(u8 *scanpath)
{
	BrowserInfo.filenum=0;
	BrowserInfo.foldernum=0;
	BrowserInfo.totalpage=0;
	BrowserInfo.pagenow=0;
	BrowserInfo.itemnum=0;
	BrowserInfo.remainder=0;
	strcpy(&BrowserInfo.path[2],scanpath);
	LCD_DrawBlock(0,0,176,220,White);					//清屏
	GUI_Box("浏览器","　　欢迎使用文件浏览器。");
	UART_SendStr("================================\n");
	UART_SendStr("Open File Browser!\n");
	Delay_Ms(1000);
	LCD_DrawBlock(0,0,176,220,White);					//清屏
start:
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
				if(pingbaoflag==1)									//如果屏保已打开，则关闭
				{
					if(TP_HOME)
						ResumePingBao();
				}
				else
				{
					if(TP_FILEBROWSER_ITEM)
					{
	/*计算有效区域*/	if(BrowserInfo.pagenow==BrowserInfo.totalpage)		//如果已经是最后一页
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
						if(Browser_OpenFile())	//如果为文件夹
							goto start;
					}
					else if(TP_FILEBROWSER_EXTI)
					{
						break;
					}
#ifdef _ENABLE_CATCH_SCREEN
					else if(TP_CATCH)
					{
						APP_CatchScreen();
					}
#endif
				}
			}
			Delay_Ms(100);
		}
	}
	else
	{
		UART_PutInf("Open Filed! res=",res);
	}
	UART_SendStr("Exit File Browser!\n");
	GUI_Box("文件浏览器","　　退出文件浏览器。");
	Delay_Ms(1000);
	LCD_DrawBlock(0,0,176,220,White);
}









