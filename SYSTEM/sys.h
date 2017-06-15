#ifndef __SYS_H
#define __SYS_H

#include "stc12c5a.h"

#define XTAL33_1776
//GBK16_12.BIN
#define FONT_START_ADDR	0x0			//�ֿ�����ʼ��ַ
#define FONT_FILE_SIZE	1340640		//�ֿ��ļ���С,ֻ�����������һ�£������в�δʹ�ô˺궨��
//ICO.BIN
#define ICO_START_ADDR	0X148000	//����������ҳ��ͼ�����ҳ�汳��bin�ļ��洢��ʼ��ַ
#define ICO_FILE_SIZE	123376		//����������ҳ��ͼ�����ҳ�汳��bin�ļ���С�������в�δʹ�ô˺궨��
#define gImage_pause	(0x148000+3872*0+2048*0)	//ÿ��ͼ��ĵ�ַ
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
#define TEST_FLASH_ADDR	0X1FF000	//flash��д���Ե�ַ��2MоƬ�����һ��sector

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

typedef unsigned char  bool;       	  /* ע�⣺��Ҫʹ��bit���壬��Ϊ�ڽṹ�����޷�ʹ�� */
typedef unsigned char  u8;         /* �޷���8λ��  */
typedef signed   char  s8;         /* �з���8λ��  */
typedef unsigned int   u16;        /* �޷���16λ�� */
typedef signed   int   s16;        /* �з���16λ�� */
typedef unsigned long  u32;        /* �޷���32λ�� */
typedef signed   long  s32;        /* �з���32λ�� */
typedef float          f32;          /* �����ȸ����� */
typedef double         f64;          /* ˫���ȸ����� */

#define WaitForDownload	while(1){if(P3==(P3&0XFE))IAP_CONTR=0X60;}
#define CheckDownload	{if(P3==(P3&0XFE))IAP_CONTR=0X60;}

#endif	/*END __INCLUDES__ */












