#include "sys.h"
#include "spiflash.h"
#include "spi.h"
#include "uart.h"
#include "malloc.h"
#include "pff.h"
#include "exfun.h"
#include "LG4525.h"

void W25Q16_ReadID(u8 *id)
{
	u8 i;
	Select_Flash();
	SPI_Flash_Write(READ_ID);
	for(i=0;i<3;i++)
	{
		id[i] = SPI_Flash_Read();
	}
	NotSelect_Flash();
}

void W25Q16_WriteEnable(void)
{
	Select_Flash();
	SPI_Flash_Write(WRITE_ENABLE);
	NotSelect_Flash();
}

void W25Q16_WriteDisable(void)
{
	Select_Flash();	
	SPI_Flash_Write(WRITE_DISABLE);	
	NotSelect_Flash();
}

u8 W25Q16_ReadStatusReg(void)
{
	u8 sta;
	
	Select_Flash();	
	SPI_Flash_Write(READ_STAUS_REG);	
 
	sta= SPI_Flash_Read();	
	
	NotSelect_Flash();
	
	return sta;
}

void W25Q16_WriteStatusReg(u8 reg)
{
	Select_Flash();	
	SPI_Flash_Write(WRITE_STAUS_REG);	
	SPI_Flash_Write(reg);
	NotSelect_Flash();
}

void W25Q16_PageProgram(u32 addr,u16 len,u8 *buf)
{
	u16 i;
	
	W25Q16_WriteEnable();
	Select_Flash();	
	SPI_Flash_Write(PAGE_PROGRAM);	
	SPI_Flash_Write((addr>>16) & 0xff);
	SPI_Flash_Write((addr>>8) & 0xff);
	SPI_Flash_Write(addr & 0xff);

	for(i=0;i<len;i++)
		SPI_Flash_Write(buf[i]);

	NotSelect_Flash();

	while(W25Q16_ReadStatusReg()&0x01);	
}

void W25Q16_SectorErase(u32 addr)
{
	W25Q16_WriteEnable();

	Select_Flash();	
	SPI_Flash_Write(SECTOR_ERASE);	
	SPI_Flash_Write((addr>>16) & 0xff);
	SPI_Flash_Write((addr>>8) & 0xff);
	SPI_Flash_Write(addr & 0xff);
	NotSelect_Flash();

	while(W25Q16_ReadStatusReg()&0x01);
}

void W25Q16_BulkErase(void)
{
	W25Q16_WriteEnable();
	Select_Flash();	
	SPI_Flash_Write(BULK_ERASE);	
	NotSelect_Flash();

	while(W25Q16_ReadStatusReg()&0x01);
}

void W25Q16_DeepPowerDown(void)
{
	Select_Flash();	
	SPI_Flash_Write(DEEP_POWER_DOWN);	
	NotSelect_Flash();
}

u8 W25Q16_WakeUp(void)
{
	u8 res;

	Select_Flash();	
	SPI_Flash_Write(WAKE_UP);	
	SPI_Flash_Write(0);	
	SPI_Flash_Write(0);	
	SPI_Flash_Write(0);	
	res=SPI_Flash_Read();
	
	NotSelect_Flash();
    
	return res;
}

void W25Q16_FastReadData(u32 addr,u16 len,u8 *buf)
{
	u16 i;

	Select_Flash();	
	SPI_Flash_Write(FAST_READ_DATA);		//
	SPI_Flash_Write((addr>>16) & 0xff);
	SPI_Flash_Write((addr>>8) & 0xff);
	SPI_Flash_Write(addr & 0xff);
	SPI_Flash_Write(0);
	
	for(i=0;i<len;i++)
	{
		buf[i]=SPI_Flash_Read();
	}
	NotSelect_Flash();
}

//读取芯片ID W25X16的ID:0XEF14
u16 SPI_FlashReadID(void)
{
	u16 Temp = 0;	  

	Select_Flash();
	SPI_Flash_Write(0x90);//发送读取ID命令
	SPI_Flash_Write(0x00);
	SPI_Flash_Write(0x00); 	    
	SPI_Flash_Write(0x00); 	 			   
	Temp|=SPI_Flash_Read()<<8;
	Temp|=SPI_Flash_Read();
	NotSelect_Flash();

	return Temp;
}

void W25Q16_Init(void)
{	
	u16 temp=0;

	temp=SPI_FlashReadID();
	switch(temp)
	{
		case W25Q64:	UART_SendStr("Flash:25Q64..		OK\n");break;
		case W25Q32:	UART_SendStr("Flash:25Q32..		OK\n");break;
		case W25Q16:	UART_SendStr("Flash:25Q16..		OK\n");break;
		case W25Q80:	UART_SendStr("Flash:25Q80..		OK\n");break;
		default:		UART_SendStr("Flash:");UART_PutHex(temp);UART_SendEnter();break;
	}
	W25Q16_WriteDisable();	//FLASH写保护，防止字库被意外破坏
}

u8 Test_FlashRW()
{
	s16	counter=0;
	u8	flag=0;
	u8	*buffer=Malloc(256);

	if(buffer==0){UART_SendStr("Malloc error when running Test_FlashRW();\n");return 0xf0;}

	W25Q16_SectorErase(TEST_FLASH_ADDR);
	for(counter=0;counter<256;counter++)
	{
		buffer[counter]=counter;
	}
	W25Q16_PageProgram(TEST_FLASH_ADDR,256,buffer);
	for(counter=0;counter<256;counter++)
	{
		buffer[counter]=0;
	}
	W25Q16_FastReadData(TEST_FLASH_ADDR,256,buffer);
	for(counter=0;counter<256;counter++)
	{
		if(buffer[counter]!=counter)
			flag=1;
	}
	Free(buffer);
	if(flag==1)
	{
		UART_SendStr("Flash Test failed!\n");
		return 1;
	}
	if(flag==0)
	{
		UART_SendStr("Flash Test OK!\n");
	}
	return 0;
}

//num=0时更新：System/BIN/GBK16_12.BIN
//num=1时更新：System/BIN/ICO.BIN
code u8 GBK_BIN[]={"System/BIN/GBK16_12.BIN"};
code u8 ICO_BIN[]={"System/BIN/ICO.BIN"};
void UpdateFileBySD(u8 num)
{
	u32 sectors,count;
	u32 addr;
	u8 *buff=Malloc(256);
	if(buff==0)
	{
		UART_SendStr("Malloc error when running UpdateFontBySD();\r\n");
		return;
	}

	if(Test_FlashRW())
		return;								//FLASH读写异常，退出
	else
	{
		if(num==0)
			{res=pf_open(GBK_BIN);}
		else if(num==1)
			{res=pf_open(ICO_BIN);}
		if(res)
		{
			UART_SendStr("Failed to open '");
			if(num==0)
				UART_SendStr(GBK_BIN);
			else if(num==1)
				UART_SendStr(ICO_BIN);
			UART_SendStr("',please check...\r\n");
			return;
		}
		UART_PutInf("File Size=",fs.fsize);
		sectors=fs.fsize/4096;						//计算需要的Sectors
		if(fs.fsize%4096!=0)
			sectors++;
		UART_PutInf("Need Sectors=",sectors);
		UART_SendStr("Erasing...\r\n");
		for(count=0;count<sectors;count++)
		{
			addr=(count<<12);
			if(num==0)
				addr+=FONT_START_ADDR;
			else if(num==1)
				addr+=ICO_START_ADDR;
			W25Q16_SectorErase(addr);
		}
		UART_SendStr("Erase OK！\r\n");
		if(num==0)
		{
			UART_SendStr("Start to update 'GBK16_12.BIN',Please wait...\n");
			addr=FONT_START_ADDR;							//Addr回到起始位置
		}
		else if(num==1)
		{
			UART_SendStr("Start to update 'ICO.BIN',Please wait...\n");
			addr=ICO_START_ADDR;							//Addr回到起始位置
		}
		while(1)
		{
			res=pf_read(buff,256,&br);
			if(res || br == 0)
			{
				UART_SendStr("Finished!\n");
				break;	 /*读文件结束*/
			}
			W25Q16_PageProgram(addr,256,buff);
			addr+=0x100;
		}
		Free(buff);
		W25Q16_WriteDisable();	//FLASH写保护，防止字库被意外破坏
	}
}

//通过读取Flash显示Anniu
//MP3的按钮
void LCD_Show_Anniu(u8 x,u8 y,u8 width,u8 height,u32 addr)
{
	u16 color;
	u32 remaind;
	u8 v,h,have_read;
	u8 *pic,*buff=Malloc(128);
	if(buff==0)
	{
		UART_SendStr("Malloc error when running LCD_Show_Anniu();\r\n");
		return;
	}
	remaind=(u32)width*(u32)height*2;
	have_read=0;
	h=0;v=0;
	while(remaind>0)						//还没读取完毕
	{
		if(remaind>128)						//够一个buffer
		{
			W25Q16_FastReadData(addr,128,buff);
			have_read=128;					//实际读取的个数
			remaind-=128;
		}
		else								//剩余的不够一个buffer
		{
			W25Q16_FastReadData(addr,remaind,buff);
			have_read=remaind;
			remaind=0;
		}
		addr+=128;
		pic=buff;
		while(have_read)					//实际读取的个数
		{
			color=*pic++;
			color|=((*pic++)<<8);
			have_read-=2;
			if(color!=Green)
				LCD_DrawPoint(x+h,y+v,color);
			h++;
			if(h==width)
				{h=0;v++;}
		}
	}
	Free(buff);
}

void W25Q16_SendPage(u32 addr)
{
	u8 i;
	u8 *buff=Malloc(128);
	if(buff==0)
	{
		UART_SendStr("Malloc error when running LCD_Show_Anniu();\r\n");
		return;
	}
	W25Q16_FastReadData(addr,128,buff);
	for(i=0;i<128;i++)
		UART_SendByte(buff[i]);
	Free(buff);
}













