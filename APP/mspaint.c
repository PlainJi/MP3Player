#include "mspaint.h"
#include "LG4525.h"
#include "touch.h"
#include "GUI_FileBrowser.h"
#include "myfun.h"
#include "BMP_Decode.h"
#include "uart.h"
#include "timer.h"

void Paint()
{
	u16 pen_color=Red;									//��ˢ��ɫ
	LCD_Clear(White);
	GUI_Box("MS_Paint","������ӭʹ�û�ͼ���ߡ�");
	UART_SendStr("================================\n");
	UART_SendStr("Open MsPaint!\n");
	Delay_Ms(1000);
paint_start:
	LCD_Clear(White);
	LCD_DrawBlock(0,204,16,16,Red);
	LCD_DrawBlock(16,204,16,16,Green);
	LCD_DrawBlock(32,204,16,16,Blue);
	LCD_DrawBlock(64,204,16,16,Cyan);
	LCD_DrawBlock(48,204,16,16,String_Color);
	LCD_DrawBlock(80,204,16,16,Blue2);
	LCD_DrawBlock(96,204,16,16,Yellow);
	LCD_DrawBlock(112,204,16,16,Magenta);
	LCD_DrawBlock(128,204,16,16,Gray);
	LCD_DrawBlock(144,204,16,16,Black);
	LCD_DrawBlock(160,204,16,16,pen_color);				//��ǰ��ˢ��ɫ
	LCD_ShowFont(161,204,"��",16,White,Black,0);//������ť

	while(1)
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
				if(TP_PAINT_PAPER)
				{
					LCD_DrawBlock(ScreenX-1,ScreenY-1,3,3,pen_color);
				}
				else if(TP_PAINT_COLOR)
				{
					pen_color=LCD_GetPoint(ScreenX,ScreenY);		//ȡɫ
					LCD_DrawBlock(160,204,16,16,pen_color);				//��ǰ��ˢ��ɫ
					LCD_ShowFont(161,204,"��",16,White,Black,0);//������ť
				}
				else if(TP_PAINT_CLEAR)
				{
					goto paint_start;
				}
				else if(TP_EXIT)
				{
					GUI_Box("MS_Paint","�����˳���ͼ���ߡ�");
					Delay_Ms(1000);
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
	}
	UART_SendStr("Exit MsPaint!\n");
}





