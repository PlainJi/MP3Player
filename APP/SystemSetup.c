#include "SystemSetup.h"
#include "GUI_SystemSetup.h"
#include "touch.h"
#include "uart.h"
#include "myfun.h"
#include "pwm.h"
#include "MP3_Player.h"
#include "lg4525.h"
#include "vs1003.h"
#include "MP3_Player.h"
#include "spiflash.h"
#include "BMP_Decode.h"
#include "uart.h"
#include "timer.h"

void SystemSetup()
{
	UART_SendStr("================================\n");
	UART_SendStr("Open System Setup!\n");
SystemSetup_Start:
	GUI_SystemSetup();
	while(1)
	{
		TP_GetScreenXY(&ScreenX,&ScreenY);
		if(CHECK_TOUCH)
		{
			if(pingbaoflag==1)									//如果屏保已打开，则关闭
			{
				if(TP_HOME)
					ResumePingBao();
			}
			else
			{
				if(TP_LIGHT)
				{
					LastProBar=100;
					GUI_SetLight();
					GUI_DrawLightBar(light);
					while(1)
					{
						TP_GetScreenXY(&ScreenX,&ScreenY);
						if(CHECK_TOUCH)
						{
							if(pingbaoflag==1)									//如果屏保已打开，则关闭
							{
								if(TP_HOME)
									ResumePingBao();
							}
							else
							{
								if(TP_EXIT_LIGHT)
								{
									LCD_DrawBlock(25,120,126,69,White);
									break;
								}
								else if(TP_MOVE_LIGHT)
								{
									if(ScreenX>138)
										ScreenX=138;
									else if(ScreenX<38)
										ScreenX=38;
									light=ScreenX-38;
									GUI_DrawLightBar(light);
									SetLight(light);
								}
#ifdef _ENABLE_CATCH_SCREEN
								else if(TP_CATCH)
								{
									APP_CatchScreen();
								}
#endif
							}
						}
						Delay_Ms(20);
					}
				}
				else if(TP_VOLUME)
				{
					LastProBar=100;
					GUI_SetVolume();
					light=volume*100/31;
					GUI_DrawLightBar(light);		//此时的light表示音量的进度
					GUI_ShowVol(volume);
					while(1)
					{
						TP_GetScreenXY(&ScreenX,&ScreenY);
						if(CHECK_TOUCH)
						{
							if(pingbaoflag==1)									//如果屏保已打开，则关闭
							{
								if(TP_HOME)
									ResumePingBao();
							}
							else
							{
								if(TP_EXIT_LIGHT)
								{
									LCD_DrawBlock(25,120,126,69,White);
									break;
								}
								else if(TP_MOVE_LIGHT)
								{
									light=ScreenX-38;
									GUI_DrawLightBar(light);
									volume=light*31/100;
									VS_SetVolume(volume);
									GUI_ShowVol(volume);
								}
#ifdef _ENABLE_CATCH_SCREEN
								else if(TP_CATCH)
								{
									APP_CatchScreen();
								}
#endif
							}
						}
						Delay_Ms(20);
					}
				}
				else if(TP_EFFECT)
				{
					GUI_SetEffect();
					while(1)
					{
						TP_GetScreenXY(&ScreenX,&ScreenY);
						if(CHECK_TOUCH)
						{
							if(pingbaoflag==1)									//如果屏保已打开，则关闭
							{
								if(TP_HOME)
									ResumePingBao();
							}
							else
							{
								if(TP_EXIT)
								{
									goto SystemSetup_Start;
								}
								else if(TP_MOVE_1)
								{
									light=VS_treble*140/15;
									LastProBar=light;
									light=ScreenX-18;
									GUI_DrawBar(75 ,light);
									VS_treble=light*15/140;
									GUI_ShowEffect1(VS_treble);
									VS_SetBass(VS_treble,VS_tfreq,VS_bass,VS_bfreq);
								}
								else if(TP_MOVE_2)
								{
									light=VS_tfreq*140/15;			//由VS_tfreq计算上次的进度条位置
									LastProBar=light;				//保存上次进度条位置
									light=ScreenX-18;				//计算本次进度条位置
									GUI_DrawBar(105,light);			//画进度条
									VS_tfreq=light*15/140;			//计算新的VS_tfreq
									if(VS_tfreq<1)VS_tfreq=1;		//根据DATASHEET,最小值为2
									GUI_ShowEffect2(VS_tfreq);
									VS_SetBass(VS_treble,VS_tfreq,VS_bass,VS_bfreq);
								}
								else if(TP_MOVE_3)
								{
									light=VS_bass*140/15;
									LastProBar=light;
									light=ScreenX-18;
									GUI_DrawBar(155,light);
									VS_bass=light*15/140;
									GUI_ShowEffect3(VS_bass);
									VS_SetBass(VS_treble,VS_tfreq,VS_bass,VS_bfreq);
								}
								else if(TP_MOVE_4)
								{
									light=VS_bfreq*140/15;
									LastProBar=light;
									light=ScreenX-18;
									GUI_DrawBar(185,light);
									VS_bfreq=light*15/140;
									if(VS_bfreq<2)VS_bfreq=2;
									GUI_ShowEffect4(VS_bfreq);
									VS_SetBass(VS_treble,VS_tfreq,VS_bass,VS_bfreq);
								}
#ifdef _ENABLE_CATCH_SCREEN
								else if(TP_CATCH)
								{
									APP_CatchScreen();
								}
#endif
							}
							Delay_Ms(20);
						}
					}
				}
				else if(TP_UPDATE)
				{
					GUI_UpdateBIN();
					while(1)
					{
						TP_GetScreenXY(&ScreenX,&ScreenY);
						if(CHECK_TOUCH)
						{
							if(pingbaoflag==1)									//如果屏保已打开，则关闭
							{
								if(TP_HOME)
									ResumePingBao();
							}
							else
							{
								if(TP_EXIT_LIGHT)
								{
									LCD_DrawBlock(25,120,126,69,White);
									break;
								}
								else if(TP_UPDATE_YES)
								{
									LCD_DrawBlock(25,120,126,69,White);
									GUI_StartUpdate();
									LCD_ShowStr(34,148,108,12,"正在写入GBK.BIN...",12,Red,Blue,0);
									UpdateFileBySD(0);
									LCD_DrawBlock(34,148,108,12,Gray);
									LCD_ShowStr(34,148,108,12,"已写入GBK.BIN",12,Red,Blue,0);
									LCD_ShowStr(34,166,108,12,"正在写入ICO.BIN...",12,Red,Blue,0);
									UpdateFileBySD(1);
									LCD_DrawBlock(34,166,108,12,Gray);
									LCD_ShowStr(34,166,108,12,"已写入ICO.BIN",12,Red,Blue,0);
									Delay_Ms(1000);
									LCD_DrawBlock(25,120,126,69,White);
									break;
								}
								else if(TP_UPDATE_NO)
								{
									LCD_DrawBlock(25,120,126,69,White);
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
				}
				else if(TP_ABOUT)
				{
					GUI_SystemInfo();
					goto SystemSetup_Start;
				}
				else if(TP_EXIT)
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
	}
	UART_SendStr("Exit System Setup!\n");
}