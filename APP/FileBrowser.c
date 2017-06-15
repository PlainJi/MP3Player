#include "FileBrowser.h"
#include "GUI_FileBrowser.h"
#include "exfun.h"
#include "uart.h"
#include "string.h"
#include "myfun.h"
#include "LG4525.h"
#include "mp3_player.h"
#include "e_book.h"
#include "BMP_Decode.h"
#include "touch.h"
#include "timer.h"

idata _BrowserInfo BrowserInfo;	//�ļ���������Ʋ�������

u8 Browser_CountFileNum()
{
	u16 temp=0;

	res = pf_opendir(&dir,&BrowserInfo.path[2]);
    if (res == FR_OK) 
	{
        while(1)
		{
            res = pf_readdir(&dir, &fno);
            if(res != FR_OK || fno.fname[0] == 0) 
				break;
            if(fno.fattrib & AM_DIR) 
			{				//�ļ���
				BrowserInfo.foldernum++;
            }
			else 
			{									//�ļ�
				BrowserInfo.filenum++;
            }
		}
		temp=BrowserInfo.filenum+BrowserInfo.foldernum+1;		//+1Ϊ�������ϼ�Ŀ¼��
		BrowserInfo.totalpage=temp/9;
		if(temp%9!=0)
			BrowserInfo.totalpage++;
		BrowserInfo.pagenow=1;									//��ʼ����1ҳΪ��ǰҳ
		BrowserInfo.itemnum=1;
		BrowserInfo.remainder=temp%9;							//���һҳ������
		return 0;
    }
	else
	{
		UART_SendStr("Open Filed!\n");
		return res;
	}
}

//�����ļ���Ϣ�ṹ�廭������Ŀ
//��ʾ�ļ�������ʾͼ�꣬��ʾ�ļ���С
//num��Χ��0-9��0��ʾ�����ϼ��˵�
void Browser_DrawItem(u8 num)
{
	u8 temp=0;
	u32 size=0;

	if(num>9)
	{
		UART_SendStr("Item num overflow!");
		return;
	}
	if(num==0)					//���أ���ʾ�ļ���ͼ�ꡢ��...��
	{
		GUIBrowser_ShowIcon(0,1);
		LCD_ShowStr(22,37,80,13,"...",12,Black,White,0);
		return;
	}
	if(fno.fattrib & AM_DIR) 	//�ļ��У���ʾ�ļ���ͼ�ꡢ�ļ�����
	{
		GUIBrowser_ShowIcon(0,num);
		LCD_ShowStr(22,20+17*num,80,13,fno.fname,12,Black,White,0);
	}
	else						//�ļ�����ʾ�ļ�ͼ�ꡢ�ļ������ļ���С
	{
		while(fno.fname[temp++]!='.');
		GUIBrowser_ShowIcon(&fno.fname[temp],num);
		LCD_ShowStr(22,20+17*num,80,13,fno.fname,12,Black,White,0);		//��ʾ�ļ���
		if(fno.fsize<1024U)
		{
			size=fno.fsize;
			LCD_ShowStr(128,20+17*num,12,13,"Bs",12,Black,White,0);
		}
		else if(fno.fsize<1048576U)
		{
			size=fno.fsize/1024;
			LCD_ShowStr(128,20+17*num,12,13,"KB",12,Black,White,0);
		}
		else if(fno.fsize<1073741824U)
		{
			size=fno.fsize/1024/1024;
			LCD_ShowStr(128,20+17*num,12,13,"MB",12,Black,White,0);
		}
		else
		{
			size=fno.fsize/1024/1024/1024;
			LCD_ShowStr(128,20+17*num,12,13,"GB",12,Black,White,0);
		}
		LCD_WriteNumLen(110,20+17*num,size,3,12,Black,White,0);		//�ļ���С
	}
}

//����ҳ����������ҳ�ĸ���Ŀ��Ϣ
//�ȸ���ҳ�������ҳҪ�����ļ��������ļ������Ƿ񻭷����ϼ��˵�
void Browser_DrawList(u8 page,u8 *path)
{
	u8 temp=0,itemnum=0;
	u8 file2draw=0,folder2draw=0;
	u16 shownfile=0,shownfolder=0;

	if(page>BrowserInfo.totalpage)
	{
		UART_SendStr("page overflow!\n");
		return;
	}
	else
		UART_PutInf("now page=",BrowserInfo.pagenow);
	if(page==1)								//����ǵ�һҳ����Ҫ��ʾ�����ϼ��˵�
	{
		if(BrowserInfo.foldernum>8)
		{
			folder2draw=8;
			file2draw=0;
		}
		else
		{
			folder2draw=BrowserInfo.foldernum;
			temp=8-folder2draw;				//ʣ�����Ŀ��
			if(BrowserInfo.filenum>temp)
				file2draw=temp;
			else
				file2draw=BrowserInfo.filenum;
		}
		itemnum=2;							//��ʼ��Ϊ2
		if(pf_opendir(&dir,path))			//���ļ���
			return;
		LCD_DrawBlock(0,33,176,154,White);	//����б�����
		Browser_DrawItem(0);				//�ڵ�һҳ���������ϼ��˵�
	}
	else									//��Ϊ��һҳ��
	{
//�ȼ����Ѿ���ʾ��������
		temp=(page-2)*9+8;					//����ǰ(page-1)ҳ������ʾ������ļ�������
		if(BrowserInfo.foldernum< temp)		//�Ѿ���ʾ����
			shownfolder=BrowserInfo.foldernum;
		else
			shownfolder=temp;				//û��ʾ���=������ʾ���������
		shownfile=temp-shownfolder;			//�Ѿ���ʾ���ļ���

//���㱾ҳҪ��ʾ���ļ��������ļ���
		folder2draw=BrowserInfo.foldernum-shownfolder;
		if(pf_opendir(&dir,path))			//���ļ���
			return;
		if(folder2draw!=0)
		{
			SkipXFolder(shownfolder);		//����shownfolder���ļ���
		}
		else
		{
			SkipXFile(shownfile);			//����shownfile���ļ�
		}
		if(folder2draw>9)
		{
			folder2draw=9;
			file2draw=0;
		}
		else
		{
			file2draw=9-folder2draw;
			if( file2draw>(BrowserInfo.filenum-shownfile) )
				file2draw=BrowserInfo.filenum-shownfile;
		}
		itemnum=1;							//��ʼ��Ϊ1
		LCD_DrawBlock(0,33,176,154,White);	//����б�����
	}
//����file2draw��folder2draw�����б�
	for(temp=0;temp<folder2draw;temp++)
	{
		GetNextFolder();
		Browser_DrawItem(itemnum++);
	}
//�ж��Ƿ���Ҫ���´��ļ���
	if(folder2draw!=0 && file2draw!=0)
		if(pf_opendir(&dir,path))
			return;
	for(temp=0;temp<file2draw;temp++)
	{
		GetNextFile();
		Browser_DrawItem(itemnum++);
	}
	GUIBrowser_ProgessBar();
	BrowserInfo.itemnum=1;
}

//��ʾ�ļ����ļ��е����
void Browser_ShowItemNum()
{
	u16 temp;
	temp=BrowserInfo.itemnum+(BrowserInfo.pagenow-1)*9-1;	//��ǰ����Ŀ����
	if(temp==0)
	{
		GUIBrowser_ShowItemNum(0,1);						//�ļ�������
		GUIBrowser_ShowItemNum(0,0);						//�ļ���������
	}
	else if(temp>BrowserInfo.foldernum)						//��ǰ��Ŀ���ļ�
	{
		temp-=BrowserInfo.foldernum;						//�ļ������
		GUIBrowser_ShowItemNum(temp,1);						//��ʾ�ļ����
		GUIBrowser_ShowItemNum(0,0);
	}
	else
	{
		GUIBrowser_ShowItemNum(temp,0);						//��ʾ�ļ������
		GUIBrowser_ShowItemNum(0,1);
	}
}

//ͨ��pagenow��itemnum���ļ����ļ���
//����ֵ��1��Ϊ�ļ��У���Ҫ�ٴδ�
//�����Ŀ��
//�����Ƿ�Ϊ֧�ֵĸ�ʽ
//������Ӧ��Ӧ�ó���
u8 Browser_OpenFile()
{
	u16 temp;
	temp=BrowserInfo.itemnum+(BrowserInfo.pagenow-1)*9-1;	//��ǰ����Ŀ����
	if(temp==0)												//�����ϼ�Ŀ¼
	{
		UART_SendStr("�����ϼ�Ŀ¼...\n");
		Browser_CutName();
		BrowserInfo.filenum=0;
		BrowserInfo.foldernum=0;
		BrowserInfo.totalpage=0;
		return 1;										//�����ϼ�Ŀ¼�����´������
	}
	else if(temp>BrowserInfo.foldernum)					//�����ļ�����Ŀ���ǿ϶����ļ��ˡ���
	{
		temp-=BrowserInfo.foldernum;
		temp--;
		pf_opendir(&dir,&BrowserInfo.path[2]);
		SkipXFile(temp);								//����temp���ļ�
		GetNextFile();									//���ļ�
		temp=Browser_CheckType();						//����ļ�����
		switch(temp)
		{
			case 1:	Browser_AddName();
					UART_SendStr("��MP3��������\n");
					MP3_Play(1);
					break;
			case 2:	Browser_AddName();
					BMP_SHOWPIC(1);
					break;
			case 3:	Browser_AddName();
					UART_SendStr("�򿪵����飡\n");
					Ebook(1);
					break;
			default:break;
		}
		BrowserInfo.filenum=0;
		BrowserInfo.foldernum=0;
		BrowserInfo.totalpage=0;
		return 1;										//���������´������
	}
	else												//���ļ���
	{
		pf_opendir(&dir,&BrowserInfo.path[2]);
		SkipXFolder(temp-1);							//����temp���ļ�
		GetNextFolder();								//���ļ�
		Browser_AddName();
		UART_SendStr("Open Folder:");
		UART_SendStr(fno.fname);
		UART_SendStr("\n");
		BrowserInfo.filenum=0;
		BrowserInfo.foldernum=0;
		BrowserInfo.totalpage=0;
		return 1;										//����·�������´������
	}
}

//�ļ������
void File_Browser(u8 *scanpath)
{
	BrowserInfo.filenum=0;
	BrowserInfo.foldernum=0;
	BrowserInfo.totalpage=0;
	BrowserInfo.pagenow=0;
	BrowserInfo.itemnum=0;
	BrowserInfo.remainder=0;
	strcpy(&BrowserInfo.path[2],scanpath);
	LCD_DrawBlock(0,0,176,220,White);					//����
	GUI_Box("�����","������ӭʹ���ļ��������");
	UART_SendStr("================================\n");
	UART_SendStr("Open File Browser!\n");
	Delay_Ms(1000);
	LCD_DrawBlock(0,0,176,220,White);					//����
start:
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
				if(pingbaoflag==1)									//��������Ѵ򿪣���ر�
				{
					if(TP_HOME)
						ResumePingBao();
				}
				else
				{
					if(TP_FILEBROWSER_ITEM)
					{
	/*������Ч����*/	if(BrowserInfo.pagenow==BrowserInfo.totalpage)		//����Ѿ������һҳ
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
						if(Browser_OpenFile())	//���Ϊ�ļ���
							goto start;
					}
					else if(TP_FILEBROWSER_EXTI)
					{
						break;
					}
#ifdef _ENABLE_CATCH_SCREEN
					else if(TP_CATCH)
					{
						APP_CatchScreen();
					}
#endif
				}
			}
			Delay_Ms(100);
		}
	}
	else
	{
		UART_PutInf("Open Filed! res=",res);
	}
	UART_SendStr("Exit File Browser!\n");
	GUI_Box("�ļ������","�����˳��ļ��������");
	Delay_Ms(1000);
	LCD_DrawBlock(0,0,176,220,White);
}









