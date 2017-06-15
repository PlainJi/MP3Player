#ifndef __EXFUN_H
#define __EXFUN_H

#include "sys.h"
#include "pff.h"

extern data FRESULT res;	//FAT���ؽ��
extern data FATFS fs;		//FAT�ļ�ϵͳ�ṹ��
extern data DIR dir;		//�ļ��в�������
extern data FILINFO fno;	//�ļ���������
extern data u16 br;		//ÿ�ζ�ȡ�ļ��ֽ����ݴ����

u8 Init_Fatfs();
u8 GetNextFolder();
u8 GetNextFile();
u8 SkipXFolder(u8 x);
u8 SkipXFile(u8 x);
void Browser_AddName();
void Browser_CutName();
u8 Browser_CheckType();

#endif