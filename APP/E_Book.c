#include "E_Book.h"
#include "GUI_E_Book.h"
#include "LG4525.h"
#include "pff.h"
#include "malloc.h"
#include "uart.h"
#include "exfun.h"
#include "filebrowser.h"
#include "gui_filebrowser.h"
#include "touch.h"
#include "myfun.h"
#include "string.h"
#include "mp3_player.h"
#include "BMP_Decode.h"
#include "timer.h"

u8 *buff=0;
idata u32 last_page=0;						//���ڱ�����һҳ��ƫ����
idata u32 this_page=0;						//���ڱ��汾ҳ��ƫ����

u8 Ebook_NextPage()
{
	u8 flag=0;
	u16 have_show=0;
//���
	if(fs.fptr==0)							//��һ�ν���
		flag=1;
	else if(fs.fptr==fs.fsize)				//�Ѿ���ʾ���
		return 1;
	else
		flag=0;
//��ʼ���ڴ漰����
	for(have_show=0;have_show<392;have_show++)
		buff[have_show]=0;
	GUIEBook_Back();
//��ȡ����ʾ
	res=pf_lseek(this_page);
	res=pf_read(buff,392,&br);				//��ȡ392���ֽ�
	have_show=LCD_ShowStr(4,22,144+18+6,196,buff,12,Black,White,0);
	pf_lseek(this_page+have_show);			//�����ļ�ָ��
	last_page=this_page;
	this_page+=have_show;
	if(res || br == 0)
		return 2;
	return 0;
}

//ȷ��·���Ѿ���λ��music
u8 EBOOK_CountFileNum()
{
	u16 temp=0;
	BrowserInfo.filenum=0;					//��������������

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
				if(3==Browser_CheckType())	//ȷ��Ϊ�ı��ļ�
				{
					BrowserInfo.filenum++;
				}
            }
		}
		if(BrowserInfo.filenum!=0)			//�ҵ�������
			return 0;
		else
		{
			UART_SendStr("No file in Folder 'EBOOK',Please copy TXT file to the folder 'ebook' on computer!\n");
			return 0x0f;
		}
    }
	else
	{
		UART_SendStr("No Folder 'ebook',Please create it on computer!\n");
		return res;
	}
}

//�򿪵�cnt�������ļ�
//1<=cnt<=BrowserInfo.filenum
u8 EBOOK_OpenFile(u16 cnt)
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
			if(3==Browser_CheckType())	//ȷ��Ϊ�ı��ļ�
			{
				cnt--;
			}
        }
	}
	Browser_AddName();
	return 0;					//�ɹ�
}

u8 EBOOK_NextOrPreFile(u8 i)
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
	return EBOOK_OpenFile(playingnow);
}

/*
void Ebook_PrePage()
{
	u16 have_show=0;

	for(have_show=0;have_show<392;have_show++)
		buff[have_show]=0;
	GUIEBook_Back();

	res=pf_lseek(last_page);
	res=pf_read(buff,392,&br);			//��ȡ336���ֽ�
	if(res || br == 0)
		return;
	have_show=LCD_ShowStr(4,22,144+18+6,196,buff,12,Black,White,0);

	
//	this_page=last_page;
//	this_page-=have_show;

	UART_PutInf("have_show=",have_show);
	UART_PutInf("last_page=",last_page);
	UART_PutInf("this_page=",this_page);
}
*/

u8 Ebook(u8 state)
{
	u8 settings;

	LCD_DrawBlock(0,0,176,220,White);					//����
	GUI_Box("E-BOOK","������ӭʹ�õ����顣");
	Delay_Ms(1000);
	LCD_DrawBlock(0,0,176,220,White);					//����

	buff=Malloc(392);
	if(buff==0){UART_SendStr("malloc error!\n");return 0x0f;}
	playingnow=1;				//��ʼ���ı����Ϊ1
	if(!state)					//���Ǵ������������������ģ���Ҫ�ȳ�ʼ����������Ϣ
	{
		strcpy(&BrowserInfo.path[2],"EBOOK");			//·����ʼ��
		if(!EBOOK_CountFileNum())						//ͳ���ı��ļ�����
		{
			EBOOK_OpenFile(playingnow);
			GUI_Box("E-Book","������Ϊ���򿪵�1�������顣");
			Delay_Ms(1000);
		}
		else
		{
			return 1;
		}
	}
	UART_SendStr("================================\n");
	UART_SendStr("Open E-Book!\n");
EBOOK_Start:
	settings=0;
	LCD_DrawBlock(0,0,176,220,White);					//����
	GUI_EBook();										//��ʼ����
	res = pf_open(&BrowserInfo.path[2]);
	if(res == FR_OK)
	{
		UART_SendStr("���ı��ļ���");
		UART_SendStr(fno.fname);UART_SendStr("\n");
		GUIEBook_ShowTitle();
		last_page=0;
		this_page=0;
		Ebook_NextPage();
		do
		{
			TP_GetScreenXY(&ScreenX,&ScreenY);
			if(CHECK_TOUCH)											//����
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
						settings=0x20;
					}
					else if(TP_EBOOK_PREPAGE)
					{
						
					}
					else if(TP_EBOOK_NEXTPAGE)
					{
						res=Ebook_NextPage();
						if(res!=1)
							UART_SendStr("��һҳ\n");								//��һҳ
					}
					else if(TP_EBOOK_PREONE)
					{
						if(!state)
						{
							if(EBOOK_NextOrPreFile(0))
								{UART_SendStr("Pre Book Error!\n");settings=0x20;}	//�˳�������
							else
								{settings=0x10;}									//�ص������鿪ʼ��
						}
					}
					else if(TP_EBOOK_NEXTONE)
					{
						if(!state)
						{
							if(EBOOK_NextOrPreFile(1))
								{UART_SendStr("Next Book Error!\n");settings=0x20;}	//�˳�������
							else
								{settings=0x10;}									//�ص������鿪ʼ��
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
			if((settings&0x30)!=0)						//�����˳�
				break;
		}while(1);
	}
	else
		UART_SendStr("Open File Filed,does it exist?\n");

	if(settings==0x10)
		goto EBOOK_Start;
	Free(buff);
	Browser_CutName();
	GUI_Box("E-Book","�����˳������顣");
	Delay_Ms(1000);
	LCD_DrawBlock(0,0,176,220,White);
	UART_SendStr("Exit E-Book!\n");

	return 0;
}