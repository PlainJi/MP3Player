#include "mp3_player.h"
#include "GUI_MP3.h"
#include "pff.h"
#include "filebrowser.h"
#include "gui_filebrowser.h"
#include "myfun.h"
#include "malloc.h"
#include "string.h"
#include "uart.h"
#include "exfun.h"
#include "vs1003.h"
#include "LG4525.h"
#include "timer.h"
#include "stdlib.h"
#include "touch.h"
#include "BMP_Decode.h"
#include "ADC.H"

data u8  LastProBar=0;		//�ϴν�����λ��
data u8  NowProBar=0;		//���ν�����λ��
data u16 DecodeTime=0;		//���ν���ʱ��
data u16 LastDecodeTime=0;	//�ϴν���ʱ��
data u16 TotalTime=0;		//������ʱ��
data u16 Kbps=0;			//������λ��
data u8  volume=20;			//����������
data u16 playingnow;		//��ǰ��Ŀ��
data u8  playorder;			//1,Ŀ¼ѭ��;2,�������;3,����ѭ��
//��Ч����
data u8  VS_treble=0x07;
data u8  VS_tfreq=0x0a;
data u8  VS_bass=0x0a;
data u8  VS_bfreq=0x0a;

void MP3_Playing()
{
	UART_SendStr("Playing...\n");
	LCD_DrawBlock(92,21,48,12,0x4228);
	LCD_ShowStrMid(92,21,"���ڲ���",12,48,Red,Black);
}

void MP3_SetVol()
{
	UART_SendStr("Setting Volume...\n");
	LCD_DrawBlock(92,21,48,12,0x4228);
	LCD_WriteASCIIStr1216(100,21,"VOL=",12,Blue,Black,0);
	LCD_WriteNumLen(100+6*4,21,volume,2,12,Blue,Black,0);
}

//1:Up	0:Down
void MP3_VolUpOrDown(u8 i)
{
	volume=VS_ReadReg(VS_VOL);
	volume=(254-volume)>>3;
	if(i)
		volume++;
	else
		volume--;
	if(volume==32)
		volume=31;
	else if(volume==0)
		volume=1;
	LCD_DrawBlock(100+6*4,21,12,12,0x4228);
	LCD_WriteNumLen(100+6*4,21,volume,2,12,Blue,Black,0);
	UART_PutInf("volume=",volume);
	VS_SetVolume(volume);
}

void MP3_SetPlayOrder()
{
	UART_SendStr("Setting Play Order...\n");
	LCD_DrawBlock(92,21,48,12,0x4228);
	switch(playorder)
	{
		case 1:LCD_ShowStrMid(92,21,"Ŀ¼ѭ��",12,48,Blue,Black);break;
		case 2:LCD_ShowStrMid(92,21,"�������",12,48,Blue,Black);break;
		case 3:LCD_ShowStrMid(92,21,"����ѭ��",12,48,Blue,Black);break;
		default:break;
	}
}

void MP3_OrderUpOrDown(u8 i)
{
	if(i)
		playorder++;
	else
		playorder--;
	if(playorder==4)
		playorder=1;
	else if(playorder==0)
		playorder=3;
	LCD_DrawBlock(92,21,48,12,0x4228);
	switch(playorder)
	{
		case 1:	UART_SendStr("Play Order=Ŀ¼ѭ��\n");
				LCD_ShowStrMid(92,21,"Ŀ¼ѭ��",12,48,Blue,Black);
				break;
		case 2:	UART_SendStr("Play Order=�������\n");
				LCD_ShowStrMid(92,21,"�������",12,48,Blue,Black);
				break;
		case 3:	UART_SendStr("Play Order=����ѭ��\n");
				LCD_ShowStrMid(92,21,"����ѭ��",12,48,Blue,Black);
				break;
		default:break;
	}
}

//�����Ѳ���ʱ��ͽ�����
void MP3_UpdatePlayedTime()
{
	DecodeTime=VS1003B_ReadDecodeTime();
	if(DecodeTime!=LastDecodeTime)
	{
		GUI_MP3PlayedTime();			//�����Ѳ���ʱ��
		LastDecodeTime=DecodeTime;
		GUI_MP3ShowProBar();			//���½�����
	}
}

//���¸�����ʱ��
void MP3_UpdateTotalTime()
{
	u16 temp;

	temp=VS_Get_HeadInfo();				//����Kbps
	if(temp!=Kbps)
	{
		Kbps=temp;
		GUI_MP3TotalTime();
	}
}

//ȷ��·���Ѿ���λ��music
u8 MP3_CountMusicNum()
{
	u16 temp=0;
	BrowserInfo.filenum=0;					//ͳ��֮ǰ������
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
				if(1==Browser_CheckType())	//ȷ��Ϊ�����ļ�
				{
					BrowserInfo.filenum++;
				}
            }
		}
		if(BrowserInfo.filenum!=0)			//�ҵ�����
			return 0;
		else
		{
			UART_SendStr("No Music in 'music' Folder,Please copy music to the folder 'music' on computer!\n");
			return 0x0f;					//music�ļ�����û�и���
		}
    }
	else
	{
		UART_SendStr("No Folder 'Music',Please create it on computer!\n");
		return res;
	}
}

//�򿪵�cnt�������ļ�
//1<=cnt<=BrowserInfo.filenum
u8 MP3_OpenMusic(u16 cnt)
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
			if(1==Browser_CheckType())	//ȷ��Ϊ�����ļ�
			{
				cnt--;
			}
        }
	}
	Browser_AddName();
	return 0;					//�ɹ�
}

//1,��һ��	0,��һ��
//����ֵ��0�ɹ�������ʧ��
u8 MP3_NextOrPreMusic(u8 i)
{
	if(ADC_GetResult()<340)		//��������
	{
		PowerOff();
	}
	if(playorder==1)			//Ŀ¼����
	{
		if(i)
			playingnow++;
		else
			playingnow--;
	}
	else if(playorder==2)		//�������
	{
		playingnow=rand()%(BrowserInfo.filenum);
		playingnow++;			//����������Ӽ�����Ŀ��
	}
	else if(playorder==3)		//����ѭ��
	{
								//��Ŀ�Ų�������
	}
	if(playingnow>BrowserInfo.filenum)
	{
		playingnow=1;						//�ص���һ��
	}
	else if(playingnow==0)
	{
		playingnow=BrowserInfo.filenum;		//���һ��
	}
	Browser_CutName();						//��cut��һ��
	return MP3_OpenMusic(playingnow);
}

void MP3_ShowMusicNum()
{
	LCD_DrawBlock(40,21,42,12,0x4228);
	LCD_WriteNumLen(40,21,playingnow,3,12,Red,Black,0);
	LCD_WriteASCII1216(40+6*3,21,'/',12,Red,Black,0);
	LCD_WriteNumLen(64,21,BrowserInfo.filenum,3,12,Red,Black,0);
}

void MP3_End()
{
	VS_FlushBuffer();
	VS_SoftReset();
	Kbps=0;
	LastProBar=0;		//�ϴν�����λ��
	NowProBar=0;		//���ν�����λ��
	DecodeTime=0;		//���ν���ʱ��
	LastDecodeTime=0;	//�ϴν���ʱ��
	TotalTime=0;		//������ʱ��
	VS1003B_SetDecodeTime0();
}

/*
u8 MP3_Browser_OpenFile()
{
	u16 temp;
	temp=BrowserInfo.itemnum+(BrowserInfo.pagenow-1)*9-1;	//��ǰ����Ŀ����
	if(temp==0)												//�����ϼ�Ŀ¼
	{
		return 1;
	}
	else if(temp>BrowserInfo.foldernum)						//�����ļ�����Ŀ���ǿ϶����ļ��ˡ���
	{
		temp-=BrowserInfo.foldernum;
		temp--;
		pf_opendir(&dir,&BrowserInfo.path[2]);
		SkipXFile(temp);									//����temp���ļ�
		GetNextFile();										//���ļ�
		temp=Browser_CheckType();							//����ļ�����
		if(temp==1)											//�������Ƶ�ļ�
			Browser_AddName();
		return 1;											//���������´������
	}
	else													//���ļ���
	{
		return 1;											//����·�������´������
	}
}

void MP3_File_Browser()
{
	BrowserInfo.filenum=0;
	BrowserInfo.foldernum=0;
	BrowserInfo.totalpage=0;
	BrowserInfo.pagenow=0;
	BrowserInfo.itemnum=0;
	BrowserInfo.remainder=0;
	strcpy(&BrowserInfo.path[2],"music");				//��ʼ������Ŀ¼
	LCD_DrawBlock(0,0,176,220,White);					//����
	GUI_Box("�����","������ӭʹ���ļ��������");
	Delay_Ms(1000);
	LCD_DrawBlock(0,0,176,220,White);					//����

	UART_SendStr("================================\n");
	if(!Browser_CountFileNum())
	{
		UART_SendStr("Current path: ");
		UART_SendStr(BrowserInfo.path);
		UART_SendEnter();
		LCD_DrawBlock(0,0,176,220,White);
		GUIBrowser_ShowBack();
		GUIBrowser_ShowPath();
		GUIBrowser_ShowNum();
		Browser_ShowItemNum();
		Browser_DrawList(BrowserInfo.pagenow,&BrowserInfo.path[2]);
		GUIBrowser_FanBai(BrowserInfo.itemnum,1);						//��һ����Ŀ������ʾ
		while(1)
		{
			TP_GetScreenXY(&ScreenX,&ScreenY);
			if(CHECK_TOUCH)												//����
			{
				if(TP_FILEBROWSER_ITEM)
				{
					if(BrowserInfo.pagenow==BrowserInfo.totalpage)		//����Ѿ������һҳ
					{													//��Ҫ������Ч��������
							if(ScreenY>((BrowserInfo.remainder+2)*17-2))//���������Ч������
								{continue;}								//����
					}
					GUIBrowser_FanBai(BrowserInfo.itemnum,0);	//�ָ���һ���ķ���״̬
					BrowserInfo.itemnum=(ScreenY-17)/17;
					GUIBrowser_FanBai(BrowserInfo.itemnum,1);	//���׵�ǰ
					Browser_ShowItemNum();
				}
				else if(TP_FILEBROWSER_PREPAGE)
				{
					GUIBrowser_FanBai(BrowserInfo.itemnum,0);			//�ָ���һ���ķ���״̬
					BrowserInfo.pagenow--;
					if(BrowserInfo.pagenow==0)
						BrowserInfo.pagenow=1;
					Browser_DrawList(BrowserInfo.pagenow,&BrowserInfo.path[2]);
					if(BrowserInfo.totalpage==1)						//���ֻ��һҳ
						BrowserInfo.itemnum=BrowserInfo.filenum+BrowserInfo.foldernum+1;
					else
						BrowserInfo.itemnum=9;
					GUIBrowser_FanBai(BrowserInfo.itemnum,1);			//���׵�ǰ
					Browser_ShowItemNum();
				}
				else if(TP_FILEBROWSER_NEXTPAGE)
				{
					GUIBrowser_FanBai(BrowserInfo.itemnum,0);			//�ָ���һ���ķ���״̬
					BrowserInfo.pagenow++;
					if(BrowserInfo.pagenow>BrowserInfo.totalpage)
						BrowserInfo.pagenow=BrowserInfo.totalpage;
					Browser_DrawList(BrowserInfo.pagenow,&BrowserInfo.path[2]);
					GUIBrowser_FanBai(BrowserInfo.itemnum,1);			//���׵�ǰ
					Browser_ShowItemNum();
				}
				else if(TP_FILEBROWSER_OPEN)
				{
					MP3_Browser_OpenFile();
				}
				else if(TP_FILEBROWSER_EXTI)
				{
					break;
				}
			}
			Delay_Ms(100);
		}
	}
	else
	{
		UART_PutInf("Open Folder Filed! res=",res);
	}
}
*/

//0����ϵͳ���벥����
//1�����ļ���������벥����
u8 MP3_Play(u8 state)
{
	u8 *buff=0;
	u8 i=0;
	u8 PlayPause=0;				//0���ţ�1��ͣ
	u8 settings=1;				//1,���ڲ���;2,��������;3,���ò���ģʽ;
	u8 flag=0;
	
	LCD_DrawBlock(0,0,176,220,White);					//����
	GUI_Box("MP3 Player","������ӭʹ��MP3��������");
	Delay_Ms(1000);
	LCD_DrawBlock(0,0,176,220,White);					//����

	GUI_MP3();					//�����ʼ����
	volume=VS_ReadReg(VS_VOL);
	volume=(254-volume)>>3;		//��ʼ������
	playingnow=1;				//��ʼ���������Ϊ1
	playorder=1;				//��ʼ������˳��ΪĿ¼ѭ��
	buff=Malloc(512);
	if(buff==0){UART_SendStr("malloc error!\n");return 0x0f;}
	if(!state)					//���Ǵ����������MP3���ų���ģ���Ҫ�ȳ�ʼ����������Ϣ
	{
		strcpy(&BrowserInfo.path[2],"MUSIC");			//·����ʼ��
		if(!MP3_CountMusicNum())						//ͳ�Ƹ�������
		{
			MP3_OpenMusic(playingnow);
		}
		else
		{
			return 1;
		}
	}
	UART_SendStr("================================\n");
	UART_SendStr("Open MP3 Player!\n");
MP3_Start:
	UART_SendStr("================================\n");
	settings=1;					//��ʼ������Ϊ���ڲ���
	GUI_MP3PlayedTime();		//�Ѳ���ʱ�����
	res = pf_open(&BrowserInfo.path[2]);
	if(res == FR_OK)
	{
		UART_SendStr("���������ļ���");
		UART_SendStr(fno.fname);UART_SendStr("\n");
		GUI_MP3ShowName();
		MP3_ShowMusicNum();								//��ʾ�������
		MP3_Playing();									//��ʾ�����ڲ��š�
		LCD_DrawBlock(0,169,176,2,Blue);				//��һ����һ��ʱ��Ҫ����������
		flag=0;
		do
		{
			if(!PlayPause)								//û�б���ͣ��������
			{
				res=pf_read(buff,512,&br);
				for(i=0;i<16;i++)
					VS_SendDat32(buff+i*32);
//���²���ʱ�䡢������			
				flag++;
				if(flag%15==0)
				{
					MP3_UpdatePlayedTime();
					MP3_UpdateTotalTime();
				}
				if(res || br == 0)
				{
					UART_SendStr("Play Over!\n");
					if(state)
						settings=0x20;
					else
					{
						res=MP3_NextOrPreMusic(1);
						if(res)//��һ��
							{UART_SendStr("Next Music Error!\n");settings=0x20;}
						else					//��ϵͳ���벥�Ž�������������һ��
							{settings=0x10;}
					}
				}
			}
			TP_GetScreenXY(&ScreenX,&ScreenY);
			if(CHECK_TOUCH)											//����
			{
				if(pingbaoflag==1)									//��������Ѵ򿪣���ر�
				{
					if(TP_HOME)
					{
						ResumePingBao();
						Delay_Ms(200);
					}
				}
				else
				{
					if(TP_MP3_PLAY)
					{
						PlayPause^=1;				//������ͣ��ť
						Delay_Ms(200);
						if(PlayPause)
							{GUI_MP3Pause();UART_SendStr("Pause...\n");}
						else
							{GUI_MP3Play();UART_SendStr("Play...\n");}
					}
					else if(TP_MP3_PRE)
					{
						if(settings==1)				//1ʱΪ�и裬��һ��
						{
							if(!state)
							{
								res=MP3_NextOrPreMusic(0);
								if(res)
									{UART_SendStr("Pre Music Error!\n");settings=0x20;}
								else
									{settings=0x10;}
							}
						}
						else if(settings==2)		//2ʱΪ��������������+1
							{MP3_VolUpOrDown(0);}
						else if(settings==3)		//3ʱΪ���ڲ���ģʽ
							{MP3_OrderUpOrDown(0);}
						Delay_Ms(10);
					}
					else if(TP_MP3_NEXT)
					{
						if(settings==1)				//1ʱΪ�и裬��һ��
						{
							if(!state)
							{
								res=MP3_NextOrPreMusic(1);
								if(res)
									{UART_SendStr("Next Music Error!\n");settings=0x20;}
								else
									{settings=0x10;}
							}
						}
						else if(settings==2)		//2ʱΪ��������������+1
							{MP3_VolUpOrDown(1);}
						else if(settings==3)		//3ʱΪ���ڲ���ģʽ
							{MP3_OrderUpOrDown(1);}
						Delay_Ms(10);
					}
					else if(TP_MP3_EXIT)
					{
						settings=0x20;				//�˳�������
					}
					else if(TP_MP3_CHANGE)
					{
						settings++;					//���ð�ť
						if(settings==4)settings=1;	//123ѭ��
						switch(settings)
						{
							case 1:MP3_Playing();break;
							case 2:MP3_SetVol();break;
							case 3:MP3_SetPlayOrder();break;
							default:break;
						}
						Delay_Ms(10);
					}
					else if(TP_HOME)
					{
						PingBao();
						Delay_Ms(200);
					}
//					else if(TP_mp3_LIST)
//					{
//						MP3_File_Browser();			//�����⣬�򿪱�������MP3���ź��ļ�������ܶ���ʹ��������
//						settings=0x10;
//					}
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
	}
	else
		UART_SendStr("Open File Filed,does it exist?\n");
//�ڴ˴���ת��MP3_Start
	MP3_End();
	if(settings==0x10)
		goto MP3_Start;

	Free(buff);
	Browser_CutName();
	GUI_Box("MP3 PlAYER","�����˳�MP3��������");
	Delay_Ms(1000);
	LCD_DrawBlock(0,0,176,220,White);
	UART_SendStr("Exit MP3 Player!\n");

	return 0;
}



