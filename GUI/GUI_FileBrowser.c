#include "GUI_FileBrowser.h"
#include "FileBrowser.h"
#include "LG4525.h"
#include "BmpTable.h"
#include "uart.h"
#include "myfun.h"
#include "string.h"

code u8 FileType[11][4]=
{"MP3","MID","WMA","WAV","BMP","DZK","TXT","LOG","INI","C","H"};

void GUIBrowser_ShowBack()
{
	LCD_Clear(White);
	LCD_ColourBar(0,0,176,9,0,1);
	LCD_ColourBar(0,9,176,9,30,0);
	LCD_ShowStrMid(0,1,"�ļ������",16,176,White,Black);
	LCD_DrawBlock(0,18,176,1,Black);
	LCD_DrawBlock(0,19,176,13,Gray);
	LCD_DrawBlock(0,32,176,1,Black);

	LCD_DrawBlock(0,187,176,1,Black);
	LCD_DrawBlock(0,188,176,13,Gray);
							//�ļ���:10/99 �ļ�:10/99 0.12M
	LCD_ShowStr(1,189,176,13," �ļ���:   /    �ļ�:   /    ",12,Black,White,0);
	LCD_DrawBlock(0,201,176,1,Black);
	LCD_ColourBar(0,202,176,9,10,1);
	LCD_ColourBar(0,211,176,9,31,0);
	LCD_ShowStr(2,203,33,18,"��",16,White,Black,0);
	LCD_ShowStr(143,203,33,18,"�˳�",16,White,Black,0);
}

void GUIBrowser_ShowPath()
{
	LCD_ShowStr(1,19,176,13,BrowserInfo.path,12,Black,White,0);
}

void GUIBrowser_ShowNum()
{
	LCD_DrawBlock(73,189,18,12,Gray);
	LCD_DrawBlock(151,189,18,12,Gray);
	LCD_WriteNumLen(73,189,BrowserInfo.foldernum,3,12,Black,White,0);
	LCD_WriteNumLen(151,189,BrowserInfo.filenum,3,12,Black,White,0);
}

//�����ļ���ʽ����ͼ��		�ļ���:----->gImage_FOLDER
//֧�ֵĸ�ʽ����Ƶ��MP3 MID WMA WAV----->gImage_MP3
//			  ͼƬ��BMP			   ----->gImage_PIC
//			  ���֣�TXT DZK LOG INI----->gImage_TXT(gImage_FONT)
//			  �������ݲ�ʶ��	   ----->gImage_FILE
//num��Χ��1-9
void GUIBrowser_ShowIcon(u8 *type,u8 num)
{
	u8 i;
	if(num>9)
		return;
	if(!type)		//�ļ���
		LCD_Show_Photo(2,17+17*num,16,16,gImage_FOLDER);
	else
	{
		for(i=0;i<11;i++)
		{
			if(!strcmp(&FileType[i][0],type))		//ƥ����
				break;
		}
		if(i==11)
			LCD_Show_Photo(2,17+17*num,16,16,gImage_FILE);//δʶ����ļ�
		else
		{
			switch(i)
			{
				case 0:
				case 1:
				case 2:
				case 3:LCD_Show_Photo(2,17+17*num,16,16,gImage_MP3);break;
				case 4:LCD_Show_Photo(2,17+17*num,16,16,gImage_PIC);break;
				case 5:LCD_Show_Photo(2,17+17*num,16,16,gImage_FONT);break;
				case 6:
				case 7:
				case 8:
				case 9:
				case 10:LCD_Show_Photo(2,17+17*num,16,16,gImage_TXT);break;
				default:											 break;
			}
		}
	}
}

//20*20�İ�ť
void GUIBrowser_ButtonUp(u8 x,u8 y)
{
//����
	LCD_DrawBlock(x   ,y   ,19,1 ,0xE71C);		//�߽���227
	LCD_DrawBlock(x+1 ,y+1 ,17,1 ,0xFFFF);		//�߽���255
	LCD_DrawBlock(x+2 ,y+2 ,16,16,0xF79E);		//�󱳾�240
	LCD_DrawBlock(x+10,y+7 ,1 ,1 ,Black);		//����0
	LCD_DrawBlock(x+9 ,y+8 ,3 ,1 ,Black);		//����0
	LCD_DrawBlock(x+8 ,y+9 ,5 ,1 ,Black);		//����0
	LCD_DrawBlock(x+7 ,y+10,7 ,1 ,Black);		//����0
	LCD_DrawBlock(x+6 ,y+11,9 ,1 ,Black);		//����0
	LCD_DrawBlock(x+1 ,y+18,18,1 ,0xA514);		//�߽���160
	LCD_DrawBlock(x   ,y+19,20,1 ,0x6B4D);		//�߽���105
//����
	LCD_DrawBlock(x   ,y+1,1,19,0xE71C);		//�߽���227
	LCD_DrawBlock(x+1 ,y+2,1,17,0xFFFF);		//�߽���255
	LCD_DrawBlock(x+18,y+1,1,18,0xA514);		//�߽���160
	LCD_DrawBlock(x+19,y  ,1,20,0x6B4D);		//�߽���105
}

//(156,167)
void GUIBrowser_ButtonDown(u8 x,u8 y)
{
//����
	LCD_DrawBlock(x   ,y+0 ,19,1 ,0xE71C);		//�߽���227
	LCD_DrawBlock(x+1 ,y+1 ,17,1 ,0xFFFF);		//�߽���255
	LCD_DrawBlock(x+2 ,y+2 ,16,16,0xF79E);	//�󱳾�240
	LCD_DrawBlock(x+6 ,y+8 ,9 ,1 ,Black);		//����0
	LCD_DrawBlock(x+7 ,y+9 ,7 ,1 ,Black);		//����0
	LCD_DrawBlock(x+8 ,y+10,5 ,1 ,Black);		//����0
	LCD_DrawBlock(x+9 ,y+11,3 ,1 ,Black);		//����0
	LCD_DrawBlock(x+10,y+12,1 ,1 ,Black);		//����0
	LCD_DrawBlock(x+1 ,y+18,18,1 ,0xA514);		//�߽���160
	LCD_DrawBlock(x   ,y+19,20,1 ,0x6B4D);		//�߽���105
//����
	LCD_DrawBlock(x+0 ,y+1,1,19,0xE71C);		//�߽���227
	LCD_DrawBlock(x+1 ,y+2,1,17,0xFFFF);		//�߽���255
	LCD_DrawBlock(x+18,y+1,1,18,0xA514);		//�߽���160
	LCD_DrawBlock(x+19,y+0,1,20,0x6B4D);		//�߽���105
}

void GUIBrowser_DrawBarBack()
{
	GUIBrowser_ButtonUp(156,33);
	GUIBrowser_ButtonDown(156,167);
	LCD_DrawBlock(156,53,20,114,0xce59);	//240	0xF79E
}

//lenΪ��ֱ���ܸ߶�
//(156,53,114)
void GUIBrowser_DrawBar(u8 x,u8 y,u8 len,u8 num,u8 total)
{
	u8 length=0,lastlength=0;

	length=len/total;							//��Ҫ���ĳ���
	lastlength=length;
	if(num==total)								//���һ��״̬������Ƿ�����©
		lastlength=len-length*(num-1);
	num--;
	LCD_DrawBlock(x   ,y+num*length,20,lastlength,0xF79E);		//240
//����
	LCD_DrawBlock(x   ,y+num*length,20,1,0xE71C);				//227
	LCD_DrawBlock(x   ,y+1+num*length,20,1,0xFFFF);				//255
	LCD_DrawBlock(x   ,y+num*length+lastlength-1,20,1,0xA514);	//160
	LCD_DrawBlock(x+1 ,y+num*length+lastlength  ,20,1,0x6B4D);	//105
//����
	LCD_DrawBlock(x   ,y+num*length  ,1,lastlength,0xE71C);		//227
	LCD_DrawBlock(x+1 ,y+1+num*length,1,lastlength-1,0xFFFF);	//255
	LCD_DrawBlock(x+18,y+num*length  ,1,lastlength,0xA514);		//160
	LCD_DrawBlock(x+19,y+num*length  ,1,lastlength,0x6B4D);		//105
}

void GUIBrowser_ProgessBar()
{
	GUIBrowser_DrawBarBack();
	GUIBrowser_DrawBar(156,53,114,BrowserInfo.pagenow,BrowserInfo.totalpage);
}

//mode=1	����
//mode=0	�ָ�Ϊ����
void GUIBrowser_FanBai(u8 num,u8 mode)
{
	u8 v,h;
	u8 temp;
		
	if(num>9)
		return;
	temp=17+17*num;

	for(v=0;v<16;v++)
	{
		for(h=20;h<155;h++)
		{
			if(mode)
			{
				if(LCD_GetPoint(h,v+temp)==Black)		//��ɫ����
					LCD_DrawPoint(h,v+temp,White);
				else									//��ɫ�ı���
					LCD_DrawPoint(h,v+temp,Blue);
			}
			else
			{
				if(LCD_GetPoint(h,v+temp)==White)		//��ɫ����
					LCD_DrawPoint(h,v+temp,Black);
				else									//��ɫ�ı���
					LCD_DrawPoint(h,v+temp,White);
			}
		}
	}
}

//state=1 ��ʾ�ļ���
//state=0 ��ʾ�ļ�����
void GUIBrowser_ShowItemNum(u16 num,u8 state)
{
	if(state)													//���Ҫ��ʾ�ļ���
	{
		LCD_DrawBlock(127,189,18,12,Gray);
		LCD_WriteNumLen(127,189,num,3,12,Black,White,0);
	}
	else														//���Ҫ��ʾ�ļ�����
	{
		LCD_DrawBlock(49,189,18,12,Gray);
		LCD_WriteNumLen(49,189,num,3,12,Black,White,0);
	}
}

void GUI_Box(u8 *title,u8 *str)
{
	LCD_DrawBlock(28,79,120,1,Blue);		//������
	LCD_DrawBlock(27,80,122,1,Blue);		//������
	LCD_DrawBlock(26,81,124,1,Blue);		//������
	LCD_DrawBlock(25,82,126,17,Blue);		//������

	LCD_DrawBlock(25,99,126,46,Gray);		//����
	LCD_DrawBlock(26,145,124,1,Gray);		//����
	LCD_DrawBlock(27,146,122,1,Gray);		//����
	LCD_DrawBlock(28,147,120,1,Gray);		//����

	LCD_ShowStr(28,81,120,16,title,16,White,Blue,1);
	LCD_ShowStr(28,102,120,45,str,12,Black,Blue,0);
}

#ifdef _TEST_TFTCopy
void TEST_TFTCopy()
{
	LCD_Show_Photo(50,50,16,16,gImage_FOLDER);
	LCD_CopyBMP(50,50,16,16,50,70);
}
#endif

#ifdef _TEST_LCDAlpha
void TEST_LCDAlpha()
{
	LCD_Show_Photo(50,30,16,16,gImage_MP3);
	LCD_Show_Photo(50,50,16,16,gImage_MP3);
	LCD_Alpha(90,40,16,16,gImage_MP3,1);
}
#endif
