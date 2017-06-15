#ifndef __ILI9220_H__
#define __ILI9220_H__

#include "sys.h"
/*��������********************************/
sbit LCD_RD   = P4^2;		   	//������
sbit LCD_WR   = P1^0;		  	//д����
sbit LCD_RS   = P1^1;  	 		//����/�����л�
sbit LCD_CS   = P1^2;			//Ƭѡ
sbit LCD_RST  = P5^2;	      	//��λ
//sbit LCD_BK   = P1^3;			//����
/*IO����**********************************/
#define LCD_CS_Low()   LCD_CS=0
#define LCD_CS_High()  LCD_CS=1
#define LCD_RS_Low()   LCD_RS=0
#define LCD_RS_High()  LCD_RS=1
#define LCD_WR_Low()   LCD_WR=0
#define LCD_WR_High()  LCD_WR=1
#define LCD_RD_Low()   LCD_RD=0
#define LCD_RD_High()  LCD_RD=1
#define LCD_RST_Low()  LCD_RST=0
#define LCD_RST_High() LCD_RST=1
#define LCD_DATA_PORT P0
/*��ɫ����********************************/
#define Red            0xF800
#define Green          0x07E0
#define Blue           0x001F
#define Yellow         0xFFE0
#define Tuhuang        0xFC00	//1111 1100 0000 0000
#define Cyan           0x7FFF    /* ���� */
#define Magenta        0xF81F
#define White          0xFFFF
#define Gray           0x738E	 /* ��ɫ */
#define Black          0x0000
#define Grey           0xF7DE
#define Blue2          0x051F
#define Back_Color 	   0x0000
#define String_Color   0x07ff
#define NotselColor	   0xFFFF
#define SelectedColor  0x07E0
/*������********************************/
#define LCD_XSIZE_TFT 	(220)	
#define LCD_YSIZE_TFT 	(176)
/*��������********************************/
//void LCD_Write_CMD(u16 iadd);
void LCD_Write_Data(u16 idat);
//void LCD_Write_Data_Byte(u8 DH,u8 DL);
//u16 LCD_Read_Data(void);
void LCD_SetXY(u8 x0,u8 x1,u8 y0,u8 y1);
void LCD_SET_XY_BMP(u8 x0,u8 x1,u8 y0,u8 y1);
void LCD_Init(u16 color);
void LCD_Clear(u16 color);
/*����ͼ��********************************/
void LCD_DrawPoint(u8 x,u8 y,u16 color);
u16  LCD_GetPoint(u8 x,u8 y);
void LCD_DisplaySeg(u8 x,u8 y,u8 seg,u16 fontcolor,u16 backcolor);
void LCD_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u16 color);
void LCD_DrawSquare(u8 x0, u8 y0, u8 x1, u8 y1,u16 color);
void LCD_DrawBlock(u8 x,u8 y,u8 width,u8 high,u16 color);
void LCD_DrawCircle(u8 x0,u8 y0,u8 r,u16 color);
/*������ʾ********************************/
void LCD_WriteASCII1216(u8 x,u8 y,u8 charr,u8 size,u16 color,u16 backcolor,u8 backcolortrue);
void LCD_WriteASCIIStr1216(u8 x,u8 y,u8 *str,u8 size,u16 color,u16 backcolor,u8 backcolortrue);
void LCD_WriteNum(u8 x,u8 y,u32 num,u16 color,u16 backcolor,u8 backcolortrue);
void LCD_WriteNumLen(u8 x,u8 y,u32 num,u8 length,u8 size,u16 color,u16 backcolor,u8 backcolortrue);
void GetHzMat(u8 *codee,u8 *mat,u8 size);
void LCD_ShowFont(u8 x,u8 y,u8 *font,u8 size,u16 color,u16 bak_color,u8 mode);
void LCD_ShowFontStr(u8 x,u8 y,u8* str,u8 size,u16 color,u16 bak_color,u8 mode);
u16  LCD_ShowStr(u8 x,u8 y,u8 width,u8 height,u8 *str,u8 size,u16 color,u16 bak_color,u8 mode);
void LCD_ShowStrMid(u8 x,u8 y,u8*str,u8 size,u8 len,u16 color,u16 bak_color);
/*�߼�����********************************/
u16 LCD_RGB5Conv(u8 r, u8 g, u8 b);
u16 LCD_RGB8Conv(u8 red, u8 green, u8 blue);
u16 LCD_RGB565To555(u16 color);
void LCD_Show_Photo(u8 x,u8 y,u8 width,u8 height,u8 *pic);
//void LCD_Show_Anniu(u8 x,u8 y,u8 width,u8 height,u8 *pic);
void LCD_ColourBar(u8 x,u8 y,u8 width,u8 height,u16 color,u8 dir);
void LCD_CopyBMP(u8 x0,u8 y0,u8 width,u8 height,u8 x1,u8 y1);
void LCD_Alpha(u8 x0,u8 y0,u8 width,u8 height,u8 *source,u8 alpha);
void LCD_AlphaWithColor(u8 x0,u8 y0,u8 width,u8 height,u16 SourceColor,u8 alpha);

#endif /*NED  __LCD5420_H__ */