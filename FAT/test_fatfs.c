#include "test_fatfs.h"
#include "exfun.h"
#include "pff.h"
#include "diskio.h"
#include "uart.h"
#include "malloc.h"
#include "myfun.h"
#include "vs1003.h"
#include "mmc_sd.h"

#ifdef _TEST_FATFS_READ
u8 TEST_FATFS_READ()
{
	u16 cnt=0;
	u8 *buff=0;
	
	buff=Malloc(512);

	res = pf_open("ebook/vs1003.txt");
	if(res == FR_OK)
	{
		UART_SendStr("Open OK!\n");
		UART_PutInf("File Size(bytes)=",fs.fsize);
		Delay_Ms(1000);
	}
	else
		UART_SendStr("Open File Filed,does it exist?\n");

	do{
		res=pf_read(buff,512,&br);
		if(res || br == 0)
		{
			break;	 /*读文件结束*/
		}
		for(cnt=0;cnt<br;cnt++)
			UART_SendByte(buff[cnt]);
	}while(1);
	Free(buff);
	return 0;
}
#endif

#ifdef _TEST_FATFS_Sudu
u8 TEST_FATFS_Sudu()
{
	u8 *buff=0;
	u32 have_read=0;
	
	buff=Malloc(512);

	res = pf_open("music/320_1.mp3");
	if(res == FR_OK)
	{
		UART_SendStr("Open File OK!\n");
		UART_PutInf("File Size=",fs.fsize);
		Delay_Ms(1000);
		UART_SendStr("Start to read!\n");
		TR0=1;									//打开定时器
	}
	else
		UART_SendStr("Open File Filed,does it exist?\n");

	do{
		res=pf_read(buff,512,&br);
		if(res || br == 0)
		{
			UART_SendStr("read over!\n");
			break;	 /*读文件结束*/
		}
		have_read+=br;
	}while(1);
	Free(buff);
	return 0;
}
#endif

#ifdef _TEST_FATFS_WRITE
#pragma message "*** TEST_FATFS_WRITE ***"
u8 TEST_FATFS_WRITE()
{
	u16 cnt=0,wr=0;
	u8 *buff=0;
	
	buff=Malloc(512);

	res = pf_open("test.txt");
	if(res == FR_OK)
	{
		UART_SendStr("Open File OK!\n");
		UART_PutInf("File Size=",fs.fsize);
		Delay_Ms(1000);

		res = pf_lseek(0);			//偏移
		for(cnt=0;cnt<512;cnt++)
			buff[cnt]=cnt;
		do
		{
			res=pf_write(buff,512,&wr);
			if(res || wr!=512)
			{
				res=pf_write(0,0,&wr);				//结束写操作
				UART_SendStr("Write over!\n");
				break;
			}
		}while(1);
	}
	else
	{
		UART_SendStr("Open File Filed,does it exist?\n");
	}
	Free(buff);
	return 0;
}
#endif

#ifdef _TEST_VS1003MP3
u8 TEST_VS1003MP3()
{
	u8 *buff=0;
	u8 i=0;
	
	buff=Malloc(512);

	res = pf_open("Music/320_1.mp3");
	if(res == FR_OK){
		UART_SendStr("Open 320_1.mp3 OK!\n");
		UART_PutInf("File Size=",fs.fsize);
		do{
			res=pf_read(buff,512,&br);
			for(i=0;i<16;i++)
				VS_SendDat32(buff+i*32);
	
			if(res || br == 0) 
			{	
				UART_SendStr("Read Over!\n");
				break;	 /*读文件结束*/
			}
		}while(1);
	}
	else{
		UART_SendStr("Open File Filed,does it exist?\n");
	}
	Free(buff);
	return 0;
}
#endif


