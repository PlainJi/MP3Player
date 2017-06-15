/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for Petit FatFs (C)ChaN, 2009      */
/*-----------------------------------------------------------------------*/
#include "diskio.h"
#include "mmc_sd.h"
#include "spi.h"
#include "uart.h"
#include "malloc.h"

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize (void)
{
	DSTATUS stat = STA_NOINIT;

	stat = SD_Initialize();
	
	return stat;
}

/*-----------------------------------------------------------------------*/
/* Read Partial Sector                                                   */
/*-----------------------------------------------------------------------*/
DRESULT disk_readp(
	BYTE* dest,			/* Pointer to the destination object */
	DWORD sector,		/* Sector number (LBA) */
	WORD sofs,			/* Offset in the sector */
	WORD count			/* Byte count (bit15:destination) */
)
{
	DRESULT res;
	u8 response=0xff;
	u16 byteLeft,retry=0;

	byteLeft=512-sofs-count;                //接收数据后，跳过的字节数

	if((sofs+count)>512)return RES_PARERR;	//读取字节超出扇区
	if(SD_Type!=SD_TYPE_V2HC)sector <<= 9;	//转换为字节地址

	res=SD_SendCmd(CMD17,sector,0X01);		//读命令,同时已经片选
	if(res==0)//指令发送成功
	{
		if(SD_GetResponse(0xFE))
		{
			SD_DisSelect();//取消片选
			return RES_ERROR;
		}
		if(sofs) 				//跳过offset（偏移字节数）个数据
		{	
			do
			{
				SD_SPI_ReadWriteByte(0xff);
			}while(--sofs);
		}
		if(dest)
		{
			do 					//接收到的数据块数据存入缓冲区
			{
				*dest++=SD_SPI_ReadWriteByte(0xff);
			}while(--count);
	
		}
		if(byteLeft) 				//跳过byteLeft个数据
		{
			do
			{
				SD_SPI_ReadWriteByte(0xff);
			}while(--byteLeft);
		}
		//下面是2个伪CRC（dummy CRC）
	    SD_SPI_ReadWriteByte(0xFF);
	    SD_SPI_ReadWriteByte(0xFF);
	}
	else
	{
		UART_PutInf("SD Send CMD ERROR:",res);
		return RES_ERROR;	//读命令发送失败
	}
	SD_DisSelect();//取消片选

	return res;
}

/*-----------------------------------------------------------------------*/
/* Write Partial Sector                                                  */
/*-----------------------------------------------------------------------*/
DRESULT disk_writep (
	BYTE* buff,		/* Pointer to the data to be written, NULL:Initiate/Finalize write operation */
	DWORD sc		/* Sector number (LBA) or Number of bytes to send */
)
{
	DRESULT res;
	u8 r1;
	u16 i;
	
	res = RES_ERROR;
	if(!buff)
	{
		if(sc)	//Initiate write process
		{
			r1 = SD_SendCmd(CMD24,sc<<9,0X01);
			if(SD_WaitReady())return 1;			//等待准备失效
			SD_SPI_ReadWriteByte(0xfe);			//发送0xfe后，可以开始写入数据

			res = RES_OK;
		} 
		else 	// Finalize write process
		{
			SD_SPI_ReadWriteByte(0xFF);		//忽略crc
		    SD_SPI_ReadWriteByte(0xFF);
			
			r1 = SD_SPI_ReadWriteByte(0xFF);//接收响应

			if( (r1&0x1f) != 0x05)			//等待是否成功
			{
				SD_DisSelect();				//取消片选
				return res;
			}
			//等待操作完
			while(!SD_SPI_ReadWriteByte(0xff));

			SD_DisSelect();
			res = RES_OK;
		}
	} 
	else 
	{
		// Send data to the disk
		for(i=0;i<512;i++)
			SD_SPI_ReadWriteByte(buff[i]);//提高速度,减少函数传参时间
		res = RES_OK;
	}

	return res;
}

#ifdef _TEST_DISKIO
void TEST_DISKIO()
{
	u8 *buff=0;
	u16 i;
	SD_Initialize();

	buff=Malloc(512);
	for(i=0;i<512;i++)
		buff[i]=i;
	disk_writep(0,256);
	disk_writep(buff,256);
	disk_writep(0,0);
	UART_SendStr("write over!\n");
	for(i=0;i<512;i++)
		buff[i]=0;
	UART_SendStr("clear buff over!\n");
	disk_readp(buff,256,0,512);
	UART_SendStr("read over!\n");
	for(i=0;i<512;i++)
		UART_SendByte(buff[i]);
	Free(buff);
}
#endif






