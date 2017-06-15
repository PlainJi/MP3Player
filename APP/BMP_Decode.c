#include "BMP_Decode.h"
#include "LG4525.h"
#include "uart.h"
#include "malloc.h"
#include "pff.h"
#include "myfun.h"
#include "exfun.h"
#include "filebrowser.h"
#include "MP3_Player.h"
#include "string.h"
#include "GUI_FileBrowser.h"
#include "touch.h"
#include "pwm.h"
#include "SystemSetup.h"
#include "timer.h"

//mode=0为显示白色背景
//mode=1为忽略白色背景，显示原背景色(用于主界面图标的显示)
u8 BmpDecode(u8 x,u8 y,u8 center)
{
	u16 color,offset;
	u8 *buff,BitCount,cnt,width,height;

	buff=Malloc(BUFFSIZE);
	if(buff==0)
	{
		UART_SendStr("malloc error!\n");
		return 0x0f;
	}
	res=pf_read(buff,255,&br);
	if(res)
	{	
		UART_SendStr("pf_read error!\n");
		Free(buff);
		return res;
	}
	BitCount = buff[0x1C]/8; 	  			// 获得像素比特数
	offset = (buff[0x0b]<<8) | buff[0x0A];	// 得到图像数据的偏移量 10 - 13,偏移量是54
	width  = buff[0x12];					// 得到图像宽度 18-21字节
	height = buff[0x16];         			// 得到图像高度 22-25
	if(center)								//图片居中显示
	{
		if( (DISWIDTH<width) || (DISHEIGHT<height) )
		{
			UART_SendStr("显示不下~_~\n");
			Free(buff);
			return 0xf0;					//检查是否显示得下
		}
		else
		{
			x=(DISWIDTH-width )/2;
			y=(DISHEIGHT-height)/2;
		}
	}
	else
	{
		if((x+width)>DISWIDTH || (y+height)>DISHEIGHT)
		{
			UART_SendStr("显示不下~_~\n");
			Free(buff);
			return 0xf0;
		}
	}

	if(width <= DISWIDTH && height <= DISHEIGHT && buff[0] == 'B' && buff[1] == 'M')
	{
		LCD_SET_XY_BMP(x,x+width-1,y,y+height-1);
		res = pf_lseek(offset);
//深度=24位真彩
		switch(BitCount)
		{
			case 3:
				do{
					res=pf_read(buff,255,&br);
					for(cnt=0;cnt<br/BitCount;cnt++)
					{	//3个字节的RGB转化为RGB565
						color = LCD_RGB8Conv(buff[cnt*3+2],buff[cnt*3+1],buff[cnt*3]);
						LCD_Write_Data(color);
					}
					if(res || br == 0)
					{
//						UART_SendStr("Read Over!\n");
						break;	 /*读文件结束*/
					}
				}while(1);
			break;
//深度=16位真彩
			case 2:
				do{
					res=pf_read(buff,254,&br);
					for(cnt=0;cnt<br/BitCount;cnt++)
					{
						//rgb555 to rgb565
						color =  buff[cnt*2]&0x1f;			//r
						color+=((buff[cnt*2]&0xe0))<<1;		//b
						color+=((buff[cnt*2+1]))<<9;		//g
						LCD_Write_Data(color);
					}
					if(res || br == 0)
					{
//						UART_SendStr("Read Over!\n");
						break;
					}
				}while(1);
			break;
//深度=8位灰度图
			case 1:
				do{
					res=pf_read(buff,255,&br);
					for(cnt=0;cnt<br;cnt++)
					{	//转换颜色为灰度
						color = LCD_RGB8Conv(buff[cnt],buff[cnt],buff[cnt]);
						LCD_Write_Data(color);
					}
					if(res || br == 0)
					{
//						UART_SendStr("Read Over!\n");
						break;
					}
				}while(1);
			break;
			default :	break;
		}
	}
	else
	{
		UART_SendStr("NOT BMP!!!\n");
	}
	Free(buff);
	return 0;
}

u8 APP_ShowPicture(u8 x,u8 y,u8 *lujing,u8 center)
{
	res = pf_open(lujing);
	if(res == FR_OK)
	{
//		UART_SendStr("Open File OK!\n");
//		UART_PutInf("File Size=",fs.fsize);
		return BmpDecode(x,y,center);				//显示背景白色
	}
	else
	{
		UART_SendStr("Open File Filed,does it exist?\n");
		return 2;
	}
}

u8 BmpCoder()
{
	u16 color,color_cnt=0,wr=0;
	u8 *buff=0,x,y;
//计数变量
	u16 cnt=0;
	u8 num=0;
	
	buff=Malloc(512);		//255
	if(buff==0)
	{
		UART_SendStr("malloc error!\n");
		return 0x0f;
	}
	res=pf_read(buff,256,&br);
	if(res)
	{	
		UART_SendStr("pf_read error!\n");
		Free(buff);
		return res;
	}
//第一部分
	for(cnt=0;cnt<229;cnt++)
	{
		x=color_cnt%176;
		y=219-(color_cnt/176);
		color=LCD_GetPoint(x,y);
		color_cnt++;
		color=LCD_RGB565To555(color);
		buff[2*cnt+54]=color&0x00ff;
		buff[2*cnt+1+54]=color>>8;
	}
	res=pf_write(buff,512,&wr);
	if(res)
	{
		UART_SendStr("Error!\n");
		UART_PutInf("res=",res);
		UART_PutInf("wr=",wr);
		goto error;
	}
//第二部分
	for(num=0;num<150;num++)
	{
		for(cnt=0;cnt<256;cnt++)
		{
			x=color_cnt%176;
			y=219-(color_cnt/176);
			color=LCD_GetPoint(x,y);
			color_cnt++;
			color=LCD_RGB565To555(color);
			buff[2*cnt]=color&0x00ff;
			buff[2*cnt+1]=color>>8;
		}
		res=pf_write(buff,512,&wr);
		if(res)
		{
			UART_SendStr("Error!\n");
			UART_PutInf("res=",res);
			UART_PutInf("wr=",wr);
			goto error;
		}
	}
//第三部分
	for(cnt=0;cnt<91;cnt++)
	{
		x=color_cnt%176;
		y=219-(color_cnt/176);
		color=LCD_GetPoint(x,y);
		color_cnt++;
		color=LCD_RGB565To555(color);
		buff[2*cnt]=color&0x00ff;
		buff[2*cnt+1]=color>>8;
	}
	res=pf_write(buff,512,&wr);
	if(res)
	{
		UART_SendStr("Error!\n");
		UART_PutInf("res=",res);
		UART_PutInf("wr=",wr);
		goto error;
	}
	res=pf_write(0,0,&wr);				//结束写操作
	if(res)
	{
		UART_SendStr("Error!\n");
		UART_PutInf("res=",res);
		UART_PutInf("wr=",wr);
		goto error;
	}
	UART_SendStr("Catch Screen Succeed!\n");
	Free(buff);
	return 0;
error:
	Free(buff);
	return 0x0f;
}

u8 APP_ReadScreen()
{
	res = pf_open("photos/kong.bmp");
	if(res == FR_OK)
	{
		UART_SendStr("Open File OK!\n");
		UART_PutInf("File Size=",fs.fsize);
		return BmpCoder();
	}
	else
	{
		UART_SendStr("Open File Failed,does it exist?\n");
		return 2;
	}
}

u8 APP_CatchScreen()
{
	UART_SendStr("================================\n");
	UART_SendStr("Catch Screen...\n");
	APP_ReadScreen();
	SetLight(0);			//闪烁一下，表示截图完成
	Delay_Ms(1000);
	SetLight(light);
	UART_SendStr("================================\n");
	return 0;
}

u8 PHOTOS_CountFileNum()
{
	u16 temp=0;
	BrowserInfo.filenum=0;					//图片数量清零

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
				if(2==Browser_CheckType())	//确认为图片文件
				{
					BrowserInfo.filenum++;
				}
            }
		}
		if(BrowserInfo.filenum!=0)			//找到图片
			return 0;
		else
		{
			UART_SendStr("No Pictures in 'Photos' Folder,Please copy file of '.bmp' to the folder 'Photos' on computer!\n");
			return 0x0f;					//photos文件夹里没有图片
		}
    }
	else
	{
		UART_SendStr("No Folder 'PHOTOS',Please create it on computer!\n");
		return res;
	}
}

u8 PHOTOS_OpenFile(u16 cnt)
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
			if(2==Browser_CheckType())	//确认为歌曲文件
			{
				cnt--;
			}
        }
	}
	Browser_AddName();
	return 0;					//成功
}

u8 PHOTOS_NextOrPrePic(u8 i)
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
	return PHOTOS_OpenFile(playingnow);
}

u8 BMP_SHOWPIC(u8 state)
{
	u8 settings=0;
	playingnow=1;				//初始化歌曲序号为1
	if(!state)					//不是从浏览器进入图片浏览程序的，需要先初始化数量等信息
	{
		strcpy(&BrowserInfo.path[2],"PHOTOS");			//路径初始化
		if(!PHOTOS_CountFileNum())						//统计图片数量
		{
			PHOTOS_OpenFile(playingnow);
		}
		else
		{
			return 1;
		}
	}
	LCD_DrawBlock(0,0,176,220,White);					//清屏
	GUI_Box("E-PHOTO","　　欢迎使用电子相册。");
	UART_SendStr("================================\n");
	UART_SendStr("Open E-PHOTO!\n");
	Delay_Ms(1000);
PHOTOS_Start:
	UART_SendStr("打开图片:");UART_SendStr(fno.fname);UART_SendStr("\n");
	settings=0;
	LCD_DrawBlock(0,0,176,220,White);				//清屏
	APP_ShowPicture(0,0,&BrowserInfo.path[2],1);
	LCD_ShowStrMid(0,0,fno.fname,12,176,Red,Black);	//显示文件名
	do{
		TP_GetScreenXY(&ScreenX,&ScreenY);
		if(CHECK_TOUCH)								//按下
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
					settings=0x20;						//退出图片浏览
				}
				else if(TP_SHOWPIC_PRE)
				{
					if(!state)
					{
						res=PHOTOS_NextOrPrePic(0);
						if(res)
							{UART_SendStr("Pre Pic Error!\n");settings=0x20;}
						else
							{settings=0x10;}
					}
				}
				else if(TP_SHOWPIC_NEXT)
				{
					if(!state)
					{
						res=PHOTOS_NextOrPrePic(1);
						if(res)
							{UART_SendStr("Next Pic Error!\n");settings=0x20;}
						else
							{settings=0x10;}
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
		if((settings&0x30)!=0)				//需要跳转到开始或者结束位置
			break;
	}while(1);
	if(settings==0x10)
		goto PHOTOS_Start;

	Browser_CutName();
	GUI_Box("E-PHOTO","　　退出电子相册。");
	Delay_Ms(1000);
	LCD_DrawBlock(0,0,176,220,White);
	UART_SendStr("Exit E-PHOTO!\n");

	return 0;
}



