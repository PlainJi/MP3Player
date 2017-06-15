#include "BMP_Decode.h"
#include "LG4525.h"
#include "uart.h"
#include "malloc.h"
#include "pff.h"
#include "myfun.h"
#include "exfun.h"
#include "filebrowser.h"
#include "MP3_Player.h"
#include "string.h"
#include "GUI_FileBrowser.h"
#include "touch.h"
#include "pwm.h"
#include "SystemSetup.h"
#include "timer.h"

//mode=0Ϊ��ʾ��ɫ����
//mode=1Ϊ���԰�ɫ��������ʾԭ����ɫ(����������ͼ�����ʾ)
u8 BmpDecode(u8 x,u8 y,u8 center)
{
	u16 color,offset;
	u8 *buff,BitCount,cnt,width,height;

	buff=Malloc(BUFFSIZE);
	if(buff==0)
	{
		UART_SendStr("malloc error!\n");
		return 0x0f;
	}
	res=pf_read(buff,255,&br);
	if(res)
	{	
		UART_SendStr("pf_read error!\n");
		Free(buff);
		return res;
	}
	BitCount = buff[0x1C]/8; 	  			// ������ر�����
	offset = (buff[0x0b]<<8) | buff[0x0A];	// �õ�ͼ�����ݵ�ƫ���� 10 - 13,ƫ������54
	width  = buff[0x12];					// �õ�ͼ���� 18-21�ֽ�
	height = buff[0x16];         			// �õ�ͼ��߶� 22-25
	if(center)								//ͼƬ������ʾ
	{
		if( (DISWIDTH<width) || (DISHEIGHT<height) )
		{
			UART_SendStr("��ʾ����~_~\n");
			Free(buff);
			return 0xf0;					//����Ƿ���ʾ����
		}
		else
		{
			x=(DISWIDTH-width )/2;
			y=(DISHEIGHT-height)/2;
		}
	}
	else
	{
		if((x+width)>DISWIDTH || (y+height)>DISHEIGHT)
		{
			UART_SendStr("��ʾ����~_~\n");
			Free(buff);
			return 0xf0;
		}
	}

	if(width <= DISWIDTH && height <= DISHEIGHT && buff[0] == 'B' && buff[1] == 'M')
	{
		LCD_SET_XY_BMP(x,x+width-1,y,y+height-1);
		res = pf_lseek(offset);
//���=24λ���
		switch(BitCount)
		{
			case 3:
				do{
					res=pf_read(buff,255,&br);
					for(cnt=0;cnt<br/BitCount;cnt++)
					{	//3���ֽڵ�RGBת��ΪRGB565
						color = LCD_RGB8Conv(buff[cnt*3+2],buff[cnt*3+1],buff[cnt*3]);
						LCD_Write_Data(color);
					}
					if(res || br == 0)
					{
//						UART_SendStr("Read Over!\n");
						break;	 /*���ļ�����*/
					}
				}while(1);
			break;
//���=16λ���
			case 2:
				do{
					res=pf_read(buff,254,&br);
					for(cnt=0;cnt<br/BitCount;cnt++)
					{
						//rgb555 to rgb565
						color =  buff[cnt*2]&0x1f;			//r
						color+=((buff[cnt*2]&0xe0))<<1;		//b
						color+=((buff[cnt*2+1]))<<9;		//g
						LCD_Write_Data(color);
					}
					if(res || br == 0)
					{
//						UART_SendStr("Read Over!\n");
						break;
					}
				}while(1);
			break;
//���=8λ�Ҷ�ͼ
			case 1:
				do{
					res=pf_read(buff,255,&br);
					for(cnt=0;cnt<br;cnt++)
					{	//ת����ɫΪ�Ҷ�
						color = LCD_RGB8Conv(buff[cnt],buff[cnt],buff[cnt]);
						LCD_Write_Data(color);
					}
					if(res || br == 0)
					{
//						UART_SendStr("Read Over!\n");
						break;
					}
				}while(1);
			break;
			default :	break;
		}
	}
	else
	{
		UART_SendStr("NOT BMP!!!\n");
	}
	Free(buff);
	return 0;
}

u8 APP_ShowPicture(u8 x,u8 y,u8 *lujing,u8 center)
{
	res = pf_open(lujing);
	if(res == FR_OK)
	{
//		UART_SendStr("Open File OK!\n");
//		UART_PutInf("File Size=",fs.fsize);
		return BmpDecode(x,y,center);				//��ʾ������ɫ
	}
	else
	{
		UART_SendStr("Open File Filed,does it exist?\n");
		return 2;
	}
}

u8 BmpCoder()
{
	u16 color,color_cnt=0,wr=0;
	u8 *buff=0,x,y;
//��������
	u16 cnt=0;
	u8 num=0;
	
	buff=Malloc(512);		//255
	if(buff==0)
	{
		UART_SendStr("malloc error!\n");
		return 0x0f;
	}
	res=pf_read(buff,256,&br);
	if(res)
	{	
		UART_SendStr("pf_read error!\n");
		Free(buff);
		return res;
	}
//��һ����
	for(cnt=0;cnt<229;cnt++)
	{
		x=color_cnt%176;
		y=219-(color_cnt/176);
		color=LCD_GetPoint(x,y);
		color_cnt++;
		color=LCD_RGB565To555(color);
		buff[2*cnt+54]=color&0x00ff;
		buff[2*cnt+1+54]=color>>8;
	}
	res=pf_write(buff,512,&wr);
	if(res)
	{
		UART_SendStr("Error!\n");
		UART_PutInf("res=",res);
		UART_PutInf("wr=",wr);
		goto error;
	}
//�ڶ�����
	for(num=0;num<150;num++)
	{
		for(cnt=0;cnt<256;cnt++)
		{
			x=color_cnt%176;
			y=219-(color_cnt/176);
			color=LCD_GetPoint(x,y);
			color_cnt++;
			color=LCD_RGB565To555(color);
			buff[2*cnt]=color&0x00ff;
			buff[2*cnt+1]=color>>8;
		}
		res=pf_write(buff,512,&wr);
		if(res)
		{
			UART_SendStr("Error!\n");
			UART_PutInf("res=",res);
			UART_PutInf("wr=",wr);
			goto error;
		}
	}
//��������
	for(cnt=0;cnt<91;cnt++)
	{
		x=color_cnt%176;
		y=219-(color_cnt/176);
		color=LCD_GetPoint(x,y);
		color_cnt++;
		color=LCD_RGB565To555(color);
		buff[2*cnt]=color&0x00ff;
		buff[2*cnt+1]=color>>8;
	}
	res=pf_write(buff,512,&wr);
	if(res)
	{
		UART_SendStr("Error!\n");
		UART_PutInf("res=",res);
		UART_PutInf("wr=",wr);
		goto error;
	}
	res=pf_write(0,0,&wr);				//����д����
	if(res)
	{
		UART_SendStr("Error!\n");
		UART_PutInf("res=",res);
		UART_PutInf("wr=",wr);
		goto error;
	}
	UART_SendStr("Catch Screen Succeed!\n");
	Free(buff);
	return 0;
error:
	Free(buff);
	return 0x0f;
}

u8 APP_ReadScreen()
{
	res = pf_open("photos/kong.bmp");
	if(res == FR_OK)
	{
		UART_SendStr("Open File OK!\n");
		UART_PutInf("File Size=",fs.fsize);
		return BmpCoder();
	}
	else
	{
		UART_SendStr("Open File Failed,does it exist?\n");
		return 2;
	}
}

u8 APP_CatchScreen()
{
	UART_SendStr("================================\n");
	UART_SendStr("Catch Screen...\n");
	APP_ReadScreen();
	SetLight(0);			//��˸һ�£���ʾ��ͼ���
	Delay_Ms(1000);
	SetLight(light);
	UART_SendStr("================================\n");
	return 0;
}

u8 PHOTOS_CountFileNum()
{
	u16 temp=0;
	BrowserInfo.filenum=0;					//ͼƬ��������

	res = pf_opendir(&dir,&BrowserInfo.path[2]);
    if (res == FR_OK)
	{
        while(1)
		{
            res = pf_readdir(&dir, &fno);
            if(res != FR_OK || fno.fname[0] == 0)
				break;
            if(fno.fattrib & AM_DIR)
			{								//�ļ���
				continue;
            }
			else 							//�ļ�
			{
				if(2==Browser_CheckType())	//ȷ��ΪͼƬ�ļ�
				{
					BrowserInfo.filenum++;
				}
            }
		}
		if(BrowserInfo.filenum!=0)			//�ҵ�ͼƬ
			return 0;
		else
		{
			UART_SendStr("No Pictures in 'Photos' Folder,Please copy file of '.bmp' to the folder 'Photos' on computer!\n");
			return 0x0f;					//photos�ļ�����û��ͼƬ
		}
    }
	else
	{
		UART_SendStr("No Folder 'PHOTOS',Please create it on computer!\n");
		return res;
	}
}

u8 PHOTOS_OpenFile(u16 cnt)
{
	pf_opendir(&dir,&BrowserInfo.path[2]);
	if( (cnt>BrowserInfo.filenum) || (cnt==0) )
		return 1;				//������������
	while(cnt)
	{
		res = pf_readdir(&dir, &fno);
        if(res != FR_OK || fno.fname[0] == 0)
		{
			return 2;
		}
        if(fno.fattrib & AM_DIR)
		{								//�ļ���
			continue;
        }
		else 							//�ļ�
		{
			if(2==Browser_CheckType())	//ȷ��Ϊ�����ļ�
			{
				cnt--;
			}
        }
	}
	Browser_AddName();
	return 0;					//�ɹ�
}

u8 PHOTOS_NextOrPrePic(u8 i)
{
	if(i)
		playingnow++;
	else
		playingnow--;
	if(playingnow>BrowserInfo.filenum)
	{
		playingnow=1;							//�ص���һ��
	}
	else if(playingnow==0)
	{
		playingnow=BrowserInfo.filenum;		//���һ��
	}
	Browser_CutName();						//��cut��һ��
	return PHOTOS_OpenFile(playingnow);
}

u8 BMP_SHOWPIC(u8 state)
{
	u8 settings=0;
	playingnow=1;				//��ʼ���������Ϊ1
	if(!state)					//���Ǵ����������ͼƬ�������ģ���Ҫ�ȳ�ʼ����������Ϣ
	{
		strcpy(&BrowserInfo.path[2],"PHOTOS");			//·����ʼ��
		if(!PHOTOS_CountFileNum())						//ͳ��ͼƬ����
		{
			PHOTOS_OpenFile(playingnow);
		}
		else
		{
			return 1;
		}
	}
	LCD_DrawBlock(0,0,176,220,White);					//����
	GUI_Box("E-PHOTO","������ӭʹ�õ�����ᡣ");
	UART_SendStr("================================\n");
	UART_SendStr("Open E-PHOTO!\n");
	Delay_Ms(1000);
PHOTOS_Start:
	UART_SendStr("��ͼƬ:");UART_SendStr(fno.fname);UART_SendStr("\n");
	settings=0;
	LCD_DrawBlock(0,0,176,220,White);				//����
	APP_ShowPicture(0,0,&BrowserInfo.path[2],1);
	LCD_ShowStrMid(0,0,fno.fname,12,176,Red,Black);	//��ʾ�ļ���
	do{
		TP_GetScreenXY(&ScreenX,&ScreenY);
		if(CHECK_TOUCH)								//����
		{
			if(pingbaoflag==1)									//��������Ѵ򿪣���ر�
			{
				if(TP_HOME)
					ResumePingBao();
			}
			else
			{
				if(TP_EXIT)
				{
					settings=0x20;						//�˳�ͼƬ���
				}
				else if(TP_SHOWPIC_PRE)
				{
					if(!state)
					{
						res=PHOTOS_NextOrPrePic(0);
						if(res)
							{UART_SendStr("Pre Pic Error!\n");settings=0x20;}
						else
							{settings=0x10;}
					}
				}
				else if(TP_SHOWPIC_NEXT)
				{
					if(!state)
					{
						res=PHOTOS_NextOrPrePic(1);
						if(res)
							{UART_SendStr("Next Pic Error!\n");settings=0x20;}
						else
							{settings=0x10;}
					}
				}
#ifdef _ENABLE_CATCH_SCREEN
				else if(TP_CATCH)
				{
					APP_CatchScreen();
				}
#endif
			}
		}
		if((settings&0x30)!=0)				//��Ҫ��ת����ʼ���߽���λ��
			break;
	}while(1);
	if(settings==0x10)
		goto PHOTOS_Start;

	Browser_CutName();
	GUI_Box("E-PHOTO","�����˳�������ᡣ");
	Delay_Ms(1000);
	LCD_DrawBlock(0,0,176,220,White);
	UART_SendStr("Exit E-PHOTO!\n");

	return 0;
}



