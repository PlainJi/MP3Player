#include "malloc.h"
#include "LG4525.h"
#include "uart.h"

xdata u8 mem1base[MEM1_MAX_SIZE];				//�ڲ�SRAM�ڴ��	384
xdata u8 mem1mapbase[MEM1_ALLOC_TABLE_SIZE];		//�ڲ�SRAM�ڴ��MAP 12
u8 ram_is_init=0;

//�����ڴ�
//*s:�ڴ��׵�ַ
//c :Ҫ���õ�ֵ
//count:��Ҫ���õ��ڴ��С(�ֽ�Ϊ��λ)
void mymemset(void *s,u8 c,u16 count)
{  
	u8 *xs = s;
    while(count--)
		*xs++=c;
}
	   
//�ڴ�����ʼ��
//memx:�����ڴ��
void RAM_Init()	
{  
	mymemset(mem1mapbase,0,MEM1_ALLOC_TABLE_SIZE);	//�ڴ�״̬����������
	mymemset(mem1base,0,MEM1_MAX_SIZE);				//�ڴ��������������  
	ram_is_init=1;									//�ڴ�����ʼ��OK
	UART_SendStr("RAM Init...		OK\n");
#ifdef	SHOW_RAM_USED
	RAM_LCDShowUsed();
#endif
}

//�����ڴ�
//*des:Ŀ�ĵ�ַ
//*src:Դ��ַ
//n:��Ҫ���Ƶ��ڴ泤��(�ֽ�Ϊ��λ)
void mymemcpy(void *des,void *src,u16 n)  
{  
    u8 *xdes=des;
	u8 *xsrc=src; 
    while(n--)*xdes++=*xsrc++;  
}  
  
//��ȡ�ڴ�ʹ����
//memx:�����ڴ��
//����ֵ:ʹ����(0~100)
u8 RAM_Perused()
{  
    u16 used=0;
    u8 i;
    for(i=0;i<MEM1_ALLOC_TABLE_SIZE;i++)
    {  
        if(mem1mapbase[i])
			used++;
    }
    return (used*100)/(MEM1_ALLOC_TABLE_SIZE);		//memtblsize 12bytes
}
 
//�ڴ����(�ڲ�����)
//size:Ҫ������ڴ��С(�ֽ�)
//����ֵ:0XFFFF,�������;����,�ڴ�ƫ�Ƶ�ַ 
u32 RAM_Malloc(u16 size)
{  
    long offset=0;
    u8 nmemb;	//��Ҫ���ڴ����  
	u8 cmemb=0; //�������ڴ����
    u8 i;
    if(!ram_is_init)
		RAM_Init();
    if(size==0)
		return 0XFFFFFFFF;//����Ҫ����
    nmemb=size/MEM1_BLOCK_SIZE;							//��ȡ��Ҫ����������ڴ����	=4
    if(size%MEM1_BLOCK_SIZE)
		nmemb++;
    for(offset=MEM1_ALLOC_TABLE_SIZE-1;offset>=0;offset--)//���������ڴ������			=3=2=1=0
    {
		if(!mem1mapbase[offset])
			cmemb++;									//�������ڴ��������			=1=2=3=4
		else 
			cmemb=0;									//�����ڴ������
		if(cmemb==nmemb)								//�ҵ�������nmemb�����ڴ��		4=4
		{
            for(i=0;i<nmemb;i++)  						//��ע�ڴ��ǿ�
            {  
                mem1mapbase[offset+i]=nmemb;										//	=4
            }  
            return (offset*MEM1_BLOCK_SIZE);//����ƫ�Ƶ�ַ
		}
	}
    return 0XFFFFFFFF;//δ�ҵ����Ϸ����������ڴ��
} 
 
//�ͷ��ڴ�(�ڲ�����) 
//offset:�ڴ��ַƫ��
//����ֵ:0,�ͷųɹ�;1,�ͷ�ʧ��;
u8 RAM_Free(u32 offset)
{  
    u8 i;
    if(!ram_is_init)
	{
		RAM_Init();
        return 1;//δ��ʼ��  
    }  
    if(offset<MEM1_MAX_SIZE)//ƫ�����ڴ����. 
    {  
        u8 index=offset/MEM1_BLOCK_SIZE;			//ƫ�������ڴ�����  
        u8 nmemb=mem1mapbase[index];				//�ڴ������
        for(i=0;i<nmemb;i++)  						//�ڴ������
        {  
            mem1mapbase[index+i]=0;
        }  
        return 0;  
    }else 
		return 2;//ƫ�Ƴ�����.
}

/*************************************************************************/
/*****************************���º���Ϊ�ⲿ����**************************/
/*************************************************************************/
//�ͷ��ڴ�(�ⲿ����) 
//ptr:�ڴ��׵�ַ 
void Free(void *ptr)  
{  
	u32 offset;  
    if(ptr==NULL)return;//��ַΪ0.  
 	offset=(u32)ptr-(u32)mem1base;
    RAM_Free(offset);//�ͷ��ڴ�
	#ifdef	SHOW_RAM_USED
	RAM_LCDShowUsed();	
	#endif     
}

//�����ڴ�(�ⲿ����)
//size:�ڴ��С(�ֽ�)
//����ֵ:���䵽���ڴ��׵�ַ.
//		 0,ʧ�ܣ�
void *Malloc(u16 size)  
{  
    u32 offset;
	offset=RAM_Malloc(size);
    if(offset==0XFFFFFFFF)
	{
		return NULL;
	}
    else
	{ 
		#ifdef	SHOW_RAM_USED
		RAM_LCDShowUsed();	
		#endif
		return (void*)((u32)mem1base+offset);
	}
}
 
//���·����ڴ�(�ⲿ����)
//*ptr:���ڴ��׵�ַ
//size:Ҫ������ڴ��С(�ֽ�)
//����ֵ:�·��䵽���ڴ��׵�ַ.
void *Realloc(void *ptr,u16 size)  
{  
    u32 offset;  
    offset=RAM_Malloc(size);  
    if(offset==0XFFFFFFFF)return NULL;     
    else
    {
	    mymemcpy((void*)((u32)mem1base+offset),ptr,size);	//�������ڴ����ݵ����ڴ�   
        Free(ptr);  										//�ͷž��ڴ�
        return (void*)((u32)mem1base+offset);  				//�������ڴ��׵�ַ
    }  
}

//����Ļ���Ͻ���ʾ��ǰ�ڴ�ʹ����
void RAM_LCDShowUsed()
{	
	LCD_WriteASCIIStr1216(176-32,0,"    ",16,Red,Blue,1);
	LCD_WriteNumLen(176-32,0,RAM_Perused(),3,16,Red,Blue,1);
	LCD_WriteASCIIStr1216(176-8,0,"%",16,Red,Blue,1);
}

#ifdef _TEST_MEM
void TEST_RAM()
{
	u8 *addr1,*addr2,percent;
//	u16 i;

//�ڴ�س�ʼ��
	RAM_Init();
//�����ڴ�
	addr1=Malloc(128);
	if(0==(u32)addr1)
		UART_SendStr("malloc failed\n");
	else
	{
		percent=RAM_Perused();
		UART_PutInf("Memory have used:",(u32)percent);
	}
//�ٴ������ڴ�
	addr2=Malloc(32);
	if(0==(u32)addr2)
		UART_SendStr("malloc failed\n");
	else
	{
		percent=RAM_Perused();
		UART_PutInf("Memory have used=",(u32)percent);
	}
//�ͷ��ڴ�
	Free(addr1);
	UART_PutInf("Memory Have used=",RAM_Perused());
	Free(addr2);
	UART_PutInf("Memory Have used=",RAM_Perused());
	
	percent=RAM_Perused();
	if(0==percent)
	{
		UART_SendStr("mem free OK!\n");
	}
	else
		UART_SendStr("mem free failed!\n");
}
#endif


