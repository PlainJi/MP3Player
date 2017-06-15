#include "GUI_SystemSetup.h"
#include "LG4525.h"
#include "exfun.h"
#include "timer.h"
#include "ADC.h"
#include "PWM.h"
#include "touch.h"
#include "bmptable.h"
#include "mp3_player.h"
#include "BMP_Decode.h"
#include "SystemSetup.h"

void GUI_SystemInfo()
{
	u16 LastSecond=0;
	u16 ADC_Result;
	u8  temp=0;
	LCD_Clear(White);
	LCD_ColourBar(0,0,176,9,0,1);
	LCD_ColourBar(0,9,176,9,30,0);
	LCD_ShowStrMid(0,1,"系统信息",16,176,White,Black);
	LCD_DrawBlock(0,18,176,1,Black);
	LCD_DrawBlock(0,19,176,201,0xce59);
	LCD_ShowStr(2,23+15*0,174,12,"主控:STC12LE5A60S2 @ 33MHz",12,Black,Gray,0);
	LCD_ShowStr(2,23+15*1,174,12,"内存:256B+1KB",12,Black,Gray,0);
	LCD_ShowStr(2,23+15*2,174,12,"SD卡:    MB/    MB",12,Black,Gray,0);
	LCD_WriteNumLen(32,23+15*2,(fs.free_clust)*4/1024,4,12,Black,Gray,0);
	LCD_WriteNumLen(74,23+15*2,(fs.n_fatent)*4/1024  ,4,12,Black,Gray,0);
	LCD_ShowStr(2,23+15*3,174,12,"字库芯片: W25Q64 8MB",12,Black,Gray,0);
	LCD_ShowStr(2,23+15*4,174,12,"文件系统: Petit FAT",12,Black,Gray,0);
	LCD_ShowStr(2,23+15*5,174,12,"HardWare: V3.0",12,Black,Gray,0);
	LCD_ShowStr(2,23+15*6,174,12,"SoftWare: V2.10",12,Black,Gray,0);
	LCD_ShowStr(2,23+15*7,174,12,"电池电压: 3.73V",12,Black,Gray,0);
	LCD_ShowStr(2,23+15*8,174,12,"运行时间: 0:00:00",12,Black,Gray,0);
	LCD_ShowStr(2,23+15*9,174,12,"版权所有: 电航学社 Plain",12,Black,Gray,0);
	while(1)
	{
		if(LastSecond!=second)		//时间有变化
		{
			LastSecond=second;
			LCD_DrawBlock(  62,	   23+15*8,42,12,0xce59);
			LCD_WriteNumLen(62+6*5,23+15*8,second%60   ,2,12,Black,Gray,0);	//秒
			LCD_ShowStr(    62+6*4,23+15*8,6,12,":",12,Black,Gray,0);
			LCD_WriteNumLen(62+6*2,23+15*8,second/60%60,2,12,Black,Gray,0);	//分
			LCD_ShowStr(    62+6*1,23+15*8,6,12,":",12,Black,Gray,0);
			LCD_WriteNumLen(62    ,23+15*8,second/60/60,1,12,Black,Gray,0);	//时
			ADC_Result=ADC_GetResult();
			LCD_DrawBlock(  62,	   23+15*7,24,12,0xce59);
			LCD_WriteNumLen(62,	   23+15*7,ADC_Result/100,1,12,Black,Gray,0);		//电压整数部分
			LCD_ShowStr(    62+6*1,23+15*7,6,12,".",12,Black,Gray,0);				//小数点
			LCD_WriteNumLen(62+6*2,23+15*7,(u16)ADC_Result%100,2,12,Black,Gray,0);	//电压小数部分
		}
		else
		{
			TP_GetScreenXY(&ScreenX,&ScreenY);
			if(CHECK_TOUCH)													//按下
			{
				if(pingbaoflag==1)									//如果屏保已打开，则关闭
				{
					if(TP_HOME)
						ResumePingBao();
				}
				else
				{
					if(TP_EXIT)
						break;
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
}		  

void GUI_SystemSetup()
{
	LCD_Clear(White);
	LCD_ColourBar(0,0,176,9,0,1);
	LCD_ColourBar(0,9,176,9,30,0);
	LCD_ShowStrMid(0,1,"系统设置",16,176,White,Black);
	LCD_DrawBlock(0,18,176,1,Black);

	LCD_ShowStr(2,23+19*0,80,16,"屏幕亮度",16,Black,White,0);
	LCD_ShowStr(2,23+19*1,80,16,"播放器音量",16,Black,White,0);
	LCD_ShowStr(2,23+19*2,80,16,"播放器效果",16,Black,White,0);
	LCD_ShowStr(2,23+19*3,80,16,"更新字库",16,Black,White,0);
	LCD_ShowStr(2,23+19*4,80,16,"系统信息",16,Black,White,0);
}

void GUI_MessageBox(u8 *title)
{
	LCD_DrawBlock(28,120,120,1,Blue);		//标题用
	LCD_DrawBlock(27,121,122,1,Blue);		//标题用
	LCD_DrawBlock(26,122,124,1,Blue);		//标题用
	LCD_DrawBlock(25,123,126,17,Blue);		//标题用

	LCD_DrawBlock(25,140,126,46,Gray);		//内容
	LCD_DrawBlock(26,186,124,1,Gray);		//内容
	LCD_DrawBlock(27,187,122,1,Gray);		//内容
	LCD_DrawBlock(28,188,120,1,Gray);		//内容

	LCD_ShowStr(28,122,120,16,title,16,White,Blue,1);
}

//条范围：38-137
void GUI_SetLight()
{
	GUI_MessageBox("亮度调整");
	LCD_Show_Photo(133 ,122,  16,16,gImage_CHA);
	LCD_DrawBlock(38   ,163,  100,1,White);
}

void GUI_DrawLightBar(u8 val)
{
	LCD_DrawBlock(36+LastProBar,   161,5,5,Gray);	//擦除方块5
	LCD_DrawBlock(38   ,162,  LastProBar,3,Gray);	//擦除3线
	
	LCD_DrawBlock(38   ,163,  100,1,White);			//画单线
	LCD_DrawBlock(38   ,162,  val,3,White);			//画3线
	LCD_DrawBlock(38+val-2,161,5,5,Cyan);			//画方块5
	LastProBar=val;
}

void GUI_SetVolume()
{
	GUI_MessageBox("音量调整");
	LCD_Show_Photo(133 ,122,  16,16,gImage_CHA);
	LCD_DrawBlock(38   ,163,  100,1,White);
	LCD_ShowStr(70,145,24,12,"VOL=",12,Red,White,0);
}

void GUI_ShowVol(u8 val)
{
	LCD_DrawBlock(94,145,12,12,Gray);
	LCD_WriteNumLen(94,145,val,2,12,Red,Gray,0);
}

void GUI_UpdateBIN()
{
	GUI_MessageBox("更新Flash");
	LCD_Show_Photo(133 ,122,  16,16,gImage_CHA);
	LCD_ShowStr(28,142,120,45,"　　此操作将更新字库芯片,请谨慎操作！",12,Yellow,Blue,0);
	LCD_ShowStr(54,171,24,12,"继续",12,Red,Blue,0);
	LCD_ShowStr(102,171,24,12,"取消",12,Red,Blue,0);
}

void GUI_StartUpdate()
{
	GUI_MessageBox("更新Flash");
}

//音效设置用
void GUI_DrawBar(u8 y,u8 val)
{
	LCD_DrawBlock(16+LastProBar-20,y-2,  40,5,Black);		//擦除方块5
	LCD_DrawBlock(18,           y-1,  LastProBar+10,3,Black);//擦除3线
	
	LCD_DrawBlock(18,           y,    140,1,Gray);			//画单线
	LCD_DrawBlock(18,           y-1,  val,3,Gray);			//画3线
	LCD_DrawBlock(18+val-2,     y-2,  5,5,Cyan);			//画方块5
}

void GUI_SetEffect()
{
	LCD_Clear(White);
	LCD_ColourBar(0,0,176,9,0,1);
	LCD_ColourBar(0,9,176,9,30,0);
	LCD_ShowStrMid(0,1,"音效设置",16,176,White,Black);
	LCD_DrawBlock(0,18,176,202,Black);

	LCD_ShowStrMid(0,45 ,"10KHz以上提升10.5dB",12,176,White,Black);
	LCD_ShowStrMid(0,125,"000Hz以下提升00.0dB",12,176,White,Black);

	LastProBar=140;
	light=VS_treble*140/15;
	GUI_DrawBar(75,light);
	GUI_ShowEffect1(VS_treble);
	LastProBar=140;
	light=VS_tfreq*140/15;
	GUI_DrawBar(105,light);
	GUI_ShowEffect2(VS_tfreq);
	LastProBar=140;
	light=VS_bass*140/15;
	GUI_DrawBar(155,light);
	GUI_ShowEffect3(VS_bass);
	LastProBar=140;
	light=VS_bfreq*140/15;
	GUI_DrawBar(185,light);
	GUI_ShowEffect4(VS_bfreq);
}
//10->2
//9-->1
//8-->-1
//7-->-2
void GUI_ShowEffect1(u8 val)
{
	val&=0x0f;
	LCD_DrawBlock(85,45,48,12,Black);
	if(val>8)			//提升
	{
		val-=8;
		val*=15;
		LCD_ShowStr(85,45,108,12,"提升",12,White,Blue,0);
		LCD_WriteNumLen(109,45,val/10,2,12,White,Gray,0);
		LCD_ShowStr(121,45,108,12,".",12,White,Blue,0);
		LCD_WriteNumLen(127,45,val%10,1,12,White,Gray,0);
	}
	else if(val>0)		//衰减
	{
		val=9-val;
		val*=15;
		LCD_ShowStr(85,45,108,12,"衰减",12,White,Blue,0);
		LCD_WriteNumLen(109,45,val/10,2,12,White,Gray,0);
		LCD_ShowStr(121,45,108,12,".",12,White,Blue,0);
		LCD_WriteNumLen(127,45,val%10,1,12,White,Gray,0);
	}
	else				//关闭
	{
		LCD_ShowStr(85,45,108,12,"衰减",12,White,Blue,0);
		LCD_WriteNumLen(109,45,0,2,12,White,Gray,0);
		LCD_ShowStr(121,45,108,12,".",12,White,Blue,0);
		LCD_WriteNumLen(127,45,0,1,12,White,Gray,0);
	}
}

void GUI_ShowEffect2(u8 val)
{
	val&=0x0f;
	LCD_DrawBlock(31,45,12,12,Black);
	LCD_WriteNumLen(31,45,val,2,12,White,Gray,0);
}

void GUI_ShowEffect3(u8 val)
{
	val&=0x0f;
	LCD_DrawBlock(109,125,12,12,Black);
	LCD_WriteNumLen(109,125,val,2,12,White,Gray,0);
}

void GUI_ShowEffect4(u8 val)
{
	val&=0x0f;
	val*=10;
	LCD_DrawBlock(31,125,18,12,Black);
	LCD_WriteNumLen(31,125,val,3,12,White,Gray,0);
}










