#include "exfun.h"
#include "FileBrowser.h"
#include "string.h"
#include "uart.h"
#include "myfun.h"
#include "LG4525.H"

data FRESULT res;	//FAT返回结果
data FATFS fs;		//FAT文件系统结构体
data DIR dir;		//文件夹参数集合
data FILINFO fno;	//文件参数集合
data u16 br;		//每次读取文件字节数暂存变量

u8 Init_Fatfs()
{
	UART_SendStr("Init Fatfs...		");
	res = pf_mount(&fs);
	if(res == FR_OK)
		UART_SendStr("OK\n");
	else 
	{
		UART_SendStr("Failed\n");
		UART_PutInf("res=",res);
		UART_SendStr("Please insert SD card!\n");
		LCD_ShowStrMid(0,102,"请插入SD卡",16,176,Red,White);
		Delay_Ms(500);
		LCD_DrawBlock(48,102,80,16,White);
		Delay_Ms(500);
		return 1;
	}
#ifdef	_SHOW_DISK_CAPACITY
	res = f_getfree(&fs);
	UART_SendStr("Total(Mb)=");UART_PutNum((fs.n_fatent)*4/1024);
	UART_SendStr("   ");
	UART_SendStr("Free(Mb)="); UART_PutNum((fs.free_clust)*4/1024);
	UART_SendStr("\n");
#endif
	BrowserInfo.path[0]='C';		//初始化系统路径
	BrowserInfo.path[1]=':';
	BrowserInfo.path[2]=0;

	return 0;
}

//获得下一个文件夹（再已经打开文件夹的前提下）
//0：成功，1：失败
u8 GetNextFolder()
{
	while(1)
	{
	    res = pf_readdir(&dir, &fno);
	    if(res != FR_OK || fno.fname[0] == 0)
			return 1;
	    if(fno.fattrib & AM_DIR)
		{						//文件夹
			return 0;
	    }
	}
}
//获得下一个文件（再已经打开文件夹的前提下）
//0：成功，1：失败
u8 GetNextFile()
{
	while(1)
	{
	    res = pf_readdir(&dir, &fno);
	    if(res != FR_OK || fno.fname[0] == 0)
			return 1;
	    if(fno.fattrib & AM_DIR)	//文件夹
			continue;
		else
			return 0;
	}
}

//跳过x个文件夹（再已经打开文件夹的前提下）
//0：成功，1：失败
u8 SkipXFolder(u8 x)
{
	while(x)
	{
	    res = pf_readdir(&dir, &fno);
		if(res != FR_OK || fno.fname[0] == 0)
			return 1;
		if(fno.fattrib & AM_DIR)	//文件夹
		{
			x--;
		}
	}
	return 0;
}
//跳过x个文件（再已经打开文件夹的前提下）
//0：成功，1：失败
u8 SkipXFile(u8 x)
{
	while(x)
	{
	    res = pf_readdir(&dir, &fno);
	    if(res != FR_OK || fno.fname[0] == 0)
			return 1;
	    if(fno.fattrib & AM_DIR)	//文件夹
			continue;
		else
			x--;
	}
	return 0;
}

//在现有路径下添加新的文件或文件夹名
//前提是文件或文件夹已经被读取
void Browser_AddName()
{
	u8 len;
	len=strlen(BrowserInfo.path);
	BrowserInfo.path[len]='/';
	strcpy(&BrowserInfo.path[len+1],fno.fname);
	len+=strlen(fno.fname);
	len++;
	BrowserInfo.path[len]=0;
}

//在现有路径下截断最后的文件或文件夹
void Browser_CutName()
{
	u8 len;
	len=strlen(BrowserInfo.path);
	while(1)
	{
		len--;
		if(BrowserInfo.path[len]=='/')	//搜索到文件夹分割点
		{
			BrowserInfo.path[len]=0;//截断
			break;
		}
		else if(BrowserInfo.path[len]==':') //已经到达根目录
		{
			len++;
			BrowserInfo.path[len]=0;//截断
			break;
		}
	}
}

//检测文件格式
//返回值：0格式不被支持
//		  1音频文件
//		  2图片文件
//		  3文本文件
u8 Browser_CheckType()
{
	u8 temp=0;

	while(fno.fname[temp++]!='.');

	if( (strcmp(&fno.fname[temp],"MP3")==0) || 
		(strcmp(&fno.fname[temp],"WMA")==0) ||
		(strcmp(&fno.fname[temp],"MID")==0) ||
		(strcmp(&fno.fname[temp],"WAV")==0) )
		{return 1;}			//音频文件
	
	else if( (strcmp(&fno.fname[temp],"BMP")==0) )
		{return 2;}			//图片文件
	
	else if( (strcmp(&fno.fname[temp],"TXT")==0) || 
		(strcmp(&fno.fname[temp],"LOG")==0) ||
		(strcmp(&fno.fname[temp],"INI")==0) ||
		(strcmp(&fno.fname[temp],"C")==0)   ||
		(strcmp(&fno.fname[temp],"H")==0)   )
		{return 3;}			//文字文件
	
	else
		return 0;			//格式不被支持
}

























