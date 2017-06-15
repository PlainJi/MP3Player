#include "malloc.h"
#include "LG4525.h"
#include "uart.h"

xdata u8 mem1base[MEM1_MAX_SIZE];				//内部SRAM内存池	384
xdata u8 mem1mapbase[MEM1_ALLOC_TABLE_SIZE];		//内部SRAM内存池MAP 12
u8 ram_is_init=0;

//设置内存
//*s:内存首地址
//c :要设置的值
//count:需要设置的内存大小(字节为单位)
void mymemset(void *s,u8 c,u16 count)
{  
	u8 *xs = s;
    while(count--)
		*xs++=c;
}
	   
//内存管理初始化
//memx:所属内存块
void RAM_Init()	
{  
	mymemset(mem1mapbase,0,MEM1_ALLOC_TABLE_SIZE);	//内存状态表数据清零
	mymemset(mem1base,0,MEM1_MAX_SIZE);				//内存池所有数据清零  
	ram_is_init=1;									//内存管理初始化OK
	UART_SendStr("RAM Init...		OK\n");
#ifdef	SHOW_RAM_USED
	RAM_LCDShowUsed();
#endif
}

//复制内存
//*des:目的地址
//*src:源地址
//n:需要复制的内存长度(字节为单位)
void mymemcpy(void *des,void *src,u16 n)  
{  
    u8 *xdes=des;
	u8 *xsrc=src; 
    while(n--)*xdes++=*xsrc++;  
}  
  
//获取内存使用率
//memx:所属内存块
//返回值:使用率(0~100)
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
 
//内存分配(内部调用)
//size:要分配的内存大小(字节)
//返回值:0XFFFF,代表错误;其他,内存偏移地址 
u32 RAM_Malloc(u16 size)
{  
    long offset=0;
    u8 nmemb;	//需要的内存块数  
	u8 cmemb=0; //连续空内存块数
    u8 i;
    if(!ram_is_init)
		RAM_Init();
    if(size==0)
		return 0XFFFFFFFF;//不需要分配
    nmemb=size/MEM1_BLOCK_SIZE;							//获取需要分配的连续内存块数	=4
    if(size%MEM1_BLOCK_SIZE)
		nmemb++;
    for(offset=MEM1_ALLOC_TABLE_SIZE-1;offset>=0;offset--)//搜索整个内存控制区			=3=2=1=0
    {
		if(!mem1mapbase[offset])
			cmemb++;									//连续空内存块数增加			=1=2=3=4
		else 
			cmemb=0;									//连续内存块清零
		if(cmemb==nmemb)								//找到了连续nmemb个空内存块		4=4
		{
            for(i=0;i<nmemb;i++)  						//标注内存块非空
            {  
                mem1mapbase[offset+i]=nmemb;										//	=4
            }  
            return (offset*MEM1_BLOCK_SIZE);//返回偏移地址
		}
	}
    return 0XFFFFFFFF;//未找到符合分配条件的内存块
} 
 
//释放内存(内部调用) 
//offset:内存地址偏移
//返回值:0,释放成功;1,释放失败;
u8 RAM_Free(u32 offset)
{  
    u8 i;
    if(!ram_is_init)
	{
		RAM_Init();
        return 1;//未初始化  
    }  
    if(offset<MEM1_MAX_SIZE)//偏移在内存池内. 
    {  
        u8 index=offset/MEM1_BLOCK_SIZE;			//偏移所在内存块号码  
        u8 nmemb=mem1mapbase[index];				//内存块数量
        for(i=0;i<nmemb;i++)  						//内存块清零
        {  
            mem1mapbase[index+i]=0;
        }  
        return 0;  
    }else 
		return 2;//偏移超区了.
}

/*************************************************************************/
/*****************************以下函数为外部调用**************************/
/*************************************************************************/
//释放内存(外部调用) 
//ptr:内存首地址 
void Free(void *ptr)  
{  
	u32 offset;  
    if(ptr==NULL)return;//地址为0.  
 	offset=(u32)ptr-(u32)mem1base;
    RAM_Free(offset);//释放内存
	#ifdef	SHOW_RAM_USED
	RAM_LCDShowUsed();	
	#endif     
}

//分配内存(外部调用)
//size:内存大小(字节)
//返回值:分配到的内存首地址.
//		 0,失败！
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
 
//重新分配内存(外部调用)
//*ptr:旧内存首地址
//size:要分配的内存大小(字节)
//返回值:新分配到的内存首地址.
void *Realloc(void *ptr,u16 size)  
{  
    u32 offset;  
    offset=RAM_Malloc(size);  
    if(offset==0XFFFFFFFF)return NULL;     
    else
    {
	    mymemcpy((void*)((u32)mem1base+offset),ptr,size);	//拷贝旧内存内容到新内存   
        Free(ptr);  										//释放旧内存
        return (void*)((u32)mem1base+offset);  				//返回新内存首地址
    }  
}

//在屏幕右上角显示当前内存使用率
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

//内存池初始化
	RAM_Init();
//申请内存
	addr1=Malloc(128);
	if(0==(u32)addr1)
		UART_SendStr("malloc failed\n");
	else
	{
		percent=RAM_Perused();
		UART_PutInf("Memory have used:",(u32)percent);
	}
//再次申请内存
	addr2=Malloc(32);
	if(0==(u32)addr2)
		UART_SendStr("malloc failed\n");
	else
	{
		percent=RAM_Perused();
		UART_PutInf("Memory have used=",(u32)percent);
	}
//释放内存
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


