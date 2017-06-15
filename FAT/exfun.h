#ifndef __EXFUN_H
#define __EXFUN_H

#include "sys.h"
#include "pff.h"

extern data FRESULT res;	//FAT返回结果
extern data FATFS fs;		//FAT文件系统结构体
extern data DIR dir;		//文件夹参数集合
extern data FILINFO fno;	//文件参数集合
extern data u16 br;		//每次读取文件字节数暂存变量

u8 Init_Fatfs();
u8 GetNextFolder();
u8 GetNextFile();
u8 SkipXFolder(u8 x);
u8 SkipXFile(u8 x);
void Browser_AddName();
void Browser_CutName();
u8 Browser_CheckType();

#endif