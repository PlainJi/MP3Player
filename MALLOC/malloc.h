#ifndef __MALLOC_H
#define __MALLOC_H

#include "sys.h"

#define NULL 0

//mem1�ڴ�����趨.mem1��ȫ�����ڲ�SRAM����
#define MEM1_BLOCK_SIZE			32  	  						//�ڴ���СΪ32�ֽ�
#define MEM1_MAX_SIZE			768 							//�������ڴ� 384�ֽ�=12��32
#define MEM1_ALLOC_TABLE_SIZE	(MEM1_MAX_SIZE/MEM1_BLOCK_SIZE) 	//�ڴ���С=24�ֽ�

//����Ϊ�ڲ�����
void mymemset(void *s,u8 c,u16 count);	 //�����ڴ�
void mymemcpy(void *des,void *src,u16 n);
void RAM_Init();					 //�ڴ�����ʼ������(��/�ڲ�����)
u8   RAM_Perused();				 //����ڴ�ʹ����(��/�ڲ�����) 
u32  RAM_Malloc(u16 size);		 //�ڴ����(�ڲ�����)
u8   RAM_Free(u32 offset);		 //�ڴ��ͷ�(�ڲ�����)
//����Ϊ�ⲿ����
void *Malloc(u16 size);		//void * Ϊ������ָ�룬����ָ���κ���������
void Free(void *ptr);
void *Realloc(void *ptr,u16 size);
void RAM_LCDShowUsed();				//��LCD���Ͻ���ʾ��ǰRAMʹ����
void TEST_RAM();

#endif













