#include "exfun.h"
#include "FileBrowser.h"
#include "string.h"
#include "uart.h"
#include "myfun.h"
#include "LG4525.H"

data FRESULT res;	//FAT���ؽ��
data FATFS fs;		//FAT�ļ�ϵͳ�ṹ��
data DIR dir;		//�ļ��в�������
data FILINFO fno;	//�ļ���������
data u16 br;		//ÿ�ζ�ȡ�ļ��ֽ����ݴ����

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
		LCD_ShowStrMid(0,102,"�����SD��",16,176,Red,White);
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
	BrowserInfo.path[0]='C';		//��ʼ��ϵͳ·��
	BrowserInfo.path[1]=':';
	BrowserInfo.path[2]=0;

	return 0;
}

//�����һ���ļ��У����Ѿ����ļ��е�ǰ���£�
//0���ɹ���1��ʧ��
u8 GetNextFolder()
{
	while(1)
	{
	    res = pf_readdir(&dir, &fno);
	    if(res != FR_OK || fno.fname[0] == 0)
			return 1;
	    if(fno.fattrib & AM_DIR)
		{						//�ļ���
			return 0;
	    }
	}
}
//�����һ���ļ������Ѿ����ļ��е�ǰ���£�
//0���ɹ���1��ʧ��
u8 GetNextFile()
{
	while(1)
	{
	    res = pf_readdir(&dir, &fno);
	    if(res != FR_OK || fno.fname[0] == 0)
			return 1;
	    if(fno.fattrib & AM_DIR)	//�ļ���
			continue;
		else
			return 0;
	}
}

//����x���ļ��У����Ѿ����ļ��е�ǰ���£�
//0���ɹ���1��ʧ��
u8 SkipXFolder(u8 x)
{
	while(x)
	{
	    res = pf_readdir(&dir, &fno);
		if(res != FR_OK || fno.fname[0] == 0)
			return 1;
		if(fno.fattrib & AM_DIR)	//�ļ���
		{
			x--;
		}
	}
	return 0;
}
//����x���ļ������Ѿ����ļ��е�ǰ���£�
//0���ɹ���1��ʧ��
u8 SkipXFile(u8 x)
{
	while(x)
	{
	    res = pf_readdir(&dir, &fno);
	    if(res != FR_OK || fno.fname[0] == 0)
			return 1;
	    if(fno.fattrib & AM_DIR)	//�ļ���
			continue;
		else
			x--;
	}
	return 0;
}

//������·��������µ��ļ����ļ�����
//ǰ�����ļ����ļ����Ѿ�����ȡ
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

//������·���½ض������ļ����ļ���
void Browser_CutName()
{
	u8 len;
	len=strlen(BrowserInfo.path);
	while(1)
	{
		len--;
		if(BrowserInfo.path[len]=='/')	//�������ļ��зָ��
		{
			BrowserInfo.path[len]=0;//�ض�
			break;
		}
		else if(BrowserInfo.path[len]==':') //�Ѿ������Ŀ¼
		{
			len++;
			BrowserInfo.path[len]=0;//�ض�
			break;
		}
	}
}

//����ļ���ʽ
//����ֵ��0��ʽ����֧��
//		  1��Ƶ�ļ�
//		  2ͼƬ�ļ�
//		  3�ı��ļ�
u8 Browser_CheckType()
{
	u8 temp=0;

	while(fno.fname[temp++]!='.');

	if( (strcmp(&fno.fname[temp],"MP3")==0) || 
		(strcmp(&fno.fname[temp],"WMA")==0) ||
		(strcmp(&fno.fname[temp],"MID")==0) ||
		(strcmp(&fno.fname[temp],"WAV")==0) )
		{return 1;}			//��Ƶ�ļ�
	
	else if( (strcmp(&fno.fname[temp],"BMP")==0) )
		{return 2;}			//ͼƬ�ļ�
	
	else if( (strcmp(&fno.fname[temp],"TXT")==0) || 
		(strcmp(&fno.fname[temp],"LOG")==0) ||
		(strcmp(&fno.fname[temp],"INI")==0) ||
		(strcmp(&fno.fname[temp],"C")==0)   ||
		(strcmp(&fno.fname[temp],"H")==0)   )
		{return 3;}			//�����ļ�
	
	else
		return 0;			//��ʽ����֧��
}

























