#ifndef __SYS_H
#define __SYS_H

#include "stc12c5a.h"

#define XTAL33_1776
//GBK16_12.BIN
#define FONT_START_ADDR	0x0			//字库存放起始地址
#define FONT_FILE_SIZE	1340640		//字库文件大小,只是在这里标明一下，程序中并未使用此宏定义
//ICO.BIN
#define ICO_START_ADDR	0X148000	//播放器和主页面图标和主页面背景bin文件存储起始地址
#define ICO_FILE_SIZE	123376		//播放器和主页面图标和主页面背景bin文件大小，程序中并未使用此宏定义
#define gImage_pause	(0x148000+3872*0+2048*0)	//每个图标的地址
#define gImage_play		(0x148000+3872*1+2048*0)
#define gImage_exit		(0x148000+3872*2+2048*0)
#define gImage_list		(0x148000+3872*2+2048*1)
#define gImage_pre		(0x148000+3872*2+2048*2)
#define gImage_next		(0x148000+3872*2+2048*3)
#define gImage_music	(0x148000+3872*2+2048*4+5000*0)
#define gImage_ebook	(0x148000+3872*2+2048*4+5000*1)
#define gImage_picture	(0x148000+3872*2+2048*4+5000*2)
#define gImage_browser	(0x148000+3872*2+2048*4+5000*3)
#define gImage_attach	(0x148000+3872*2+2048*4+5000*4)
#define gImage_sys		(0x148000+3872*2+2048*4+5000*5)
#define gImage_BackWhite	(0x148000+3872*2+2048*4+5000*6)

//test flash address
#define TEST_FLASH_ADDR	0X1FF000	//flash读写测试地址，2M芯片的最后一个sector

#define TP_EXIT	 (ScreenX>0   && ScreenX<58  && ScreenY>220 && ScreenY<250)
#define TP_HOME	 (ScreenX>80  && ScreenX<100 && ScreenY>220 && ScreenY<250)
#define TP_CATCH (ScreenX>116 && ScreenX<176 && ScreenY>220 && ScreenY<250)

//#define SHOW_RAM_USED
//#define _TEST_MEM
//#define _TEST_TFTCopy
//#define _TEST_LCDAlpha
//#define ENABLE_SPECTRUM

//#define _TEST_SD_CARD
//#define _TEST_DISKIO

//#define _TEST_FATFS_READ
//#define _TEST_FATFS_Sudu
//#define _TEST_FATFS_WRITE
//#define _TEST_VS1003MP3

//#define _SHOW_DISK_CAPACITY
//#define _ENABLE_CATCH_SCREEN

typedef unsigned char  bool;       	  /* 注意：不要使用bit定义，因为在结构体里无法使用 */
typedef unsigned char  u8;         /* 无符号8位数  */
typedef signed   char  s8;         /* 有符号8位数  */
typedef unsigned int   u16;        /* 无符号16位数 */
typedef signed   int   s16;        /* 有符号16位数 */
typedef unsigned long  u32;        /* 无符号32位数 */
typedef signed   long  s32;        /* 有符号32位数 */
typedef float          f32;          /* 单精度浮点数 */
typedef double         f64;          /* 双精度浮点数 */

#define WaitForDownload	while(1){if(P3==(P3&0XFE))IAP_CONTR=0X60;}
#define CheckDownload	{if(P3==(P3&0XFE))IAP_CONTR=0X60;}

#endif	/*END __INCLUDES__ */












