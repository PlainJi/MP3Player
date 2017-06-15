#ifndef __MALLOC_H
#define __MALLOC_H

#include "sys.h"

#define NULL 0

//mem1内存参数设定.mem1完全处于内部SRAM里面
#define MEM1_BLOCK_SIZE			32  	  						//内存块大小为32字节
#define MEM1_MAX_SIZE			768 							//最大管理内存 384字节=12×32
#define MEM1_ALLOC_TABLE_SIZE	(MEM1_MAX_SIZE/MEM1_BLOCK_SIZE) 	//内存表大小=24字节

//以下为内部函数
void mymemset(void *s,u8 c,u16 count);	 //设置内存
void mymemcpy(void *des,void *src,u16 n);
void RAM_Init();					 //内存管理初始化函数(外/内部调用)
u8   RAM_Perused();				 //获得内存使用率(外/内部调用) 
u32  RAM_Malloc(u16 size);		 //内存分配(内部调用)
u8   RAM_Free(u32 offset);		 //内存释放(内部调用)
//以下为外部调用
void *Malloc(u16 size);		//void * 为无类型指针，可以指向任何数据类型
void Free(void *ptr);
void *Realloc(void *ptr,u16 size);
void RAM_LCDShowUsed();				//在LCD右上角显示当前RAM使用量
void TEST_RAM();

#endif













