#include "sys.h"
#include "LG4525.h"
#include "myfun.h"
#include "ascii.h"
#include "spiflash.h"
#include "string.h"
#include "uart.h"

void LCD_Write_CMD(u16 iadd)
{
	LCD_CS_Low();	
	LCD_RS_Low();
	LCD_DATA_PORT=(iadd>>8);
	LCD_WR_Low();
	LCD_WR_High();
	LCD_DATA_PORT=(iadd&0xff);
	LCD_WR_Low();
	LCD_WR_High();	
	LCD_CS_High();
}

void LCD_Write_Data(u16 idat)
{
	LCD_CS_Low();	
	LCD_RS_High();
	LCD_DATA_PORT=(idat>>8);
	LCD_WR_Low();
	LCD_WR_High();
	LCD_DATA_PORT=(idat&0xff);
	LCD_WR_Low();
	LCD_WR_High();	
	LCD_CS_High();	
}

void LCD_Write_Data_Byte(u8 DH,u8 DL)		   
{	
	LCD_CS_Low();
	
	LCD_RS_High();
	LCD_DATA_PORT=DH;			 //高八位
	LCD_WR_Low();
    LCD_WR_High();
	LCD_DATA_PORT=DL;			 //低八位
	LCD_WR_Low();
	LCD_WR_High();

	LCD_CS_High();
}

u16 LCD_Read_Data(void)
{
	u16 high,low;
	LCD_DATA_PORT = 0XFF;  /* 上拉输入 */
	LCD_CS_Low();
	LCD_WR_High();
	LCD_RS_High();
	LCD_RD_Low();
	high = LCD_DATA_PORT;	
	LCD_RD_High();
	LCD_RD_Low();
	low = LCD_DATA_PORT;	
	LCD_RD_High();
	LCD_CS_High();
	return (low | (high<<8) );
} 

void LCD_WriteReg(u16 iadd,u16 iidata)
{
	LCD_Write_CMD(iadd);
	LCD_Write_Data(iidata);
}

void LCD_SetXY(u8 x1,u8 x2,u8 y1,u8 y2)
{	
	LCD_WriteReg(0X03,0X1030);
	LCD_WriteReg(0x0044,(x2<<8)+x1);
	LCD_WriteReg(0x0045,(y2<<8)+y1);
	LCD_WriteReg(0x0021,(y1<<8)+x1);
	LCD_Write_CMD(0x0022);							 
}

void LCD_SET_XY_BMP(u8 x1,u8 x2,u8 y1,u8 y2)
{	
	LCD_WriteReg(0X03,0X1010);				//0001 0000 0001 0000
	LCD_WriteReg(0x0044,(x2<<8)+x1);
	LCD_WriteReg(0x0045,(y2<<8)+y1);
	LCD_WriteReg(0x0021,(y2<<8)+x1);
	LCD_Write_CMD(0x0022);							 
}

void LCD_Init(u16 color)
{
	LCD_WriteReg(0X14,0X0030); 
	LCD_WriteReg(0X0F,0X0008); 
	LCD_WriteReg(0X11,0X0231); 
	LCD_WriteReg(0X12,0X0009); 
	LCD_WriteReg(0X13,0X2958); 
	LCD_WriteReg(0X10,0X5268);
	Delay_Ms(40);
	LCD_WriteReg(0X12,0X0019);
	Delay_Ms(40);
	LCD_WriteReg(0X10,0X3260); 
	LCD_WriteReg(0X13,0X1958); 
	Delay_Ms(40);
	//Gamma Setting
	LCD_WriteReg(0X30,0X0000);
	LCD_WriteReg(0X31,0X0501);
	LCD_WriteReg(0X32,0X0207);
	LCD_WriteReg(0X33,0X0502);
	LCD_WriteReg(0X34,0X0007);
	LCD_WriteReg(0X35,0X0601);
	LCD_WriteReg(0X36,0X0707);
	LCD_WriteReg(0X37,0X0305);
	LCD_WriteReg(0X38,0X040e);
	LCD_WriteReg(0X39,0X040e);
	LCD_WriteReg(0X3a,0X0101);
	LCD_WriteReg(0X3b,0X0101);
	LCD_WriteReg(0X3c,0X0101);
	LCD_WriteReg(0X3d,0X0101);
	LCD_WriteReg(0X3e,0X0001);
	LCD_WriteReg(0X3f,0X0001);

	LCD_WriteReg(0X21,0x0000);

	//Display Mode	
	LCD_WriteReg(0X01,0X011B);//竖屏0x011b横屏0x031B
	LCD_WriteReg(0X02,0X0700);
	LCD_WriteReg(0X03,0X1030);//竖屏1030 横屏//1038
	LCD_WriteReg(0X08,0X1010);
	LCD_WriteReg(0X0A,0X4420);
	LCD_WriteReg(0X0B,0X5030);
	LCD_WriteReg(0X0F,0X000D);
	Delay_Ms(10);
	//Display ON sequence
	LCD_WriteReg(0X07,0X0005);
	LCD_WriteReg(0X07,0X0025);
	LCD_WriteReg(0X07,0X0027);
	LCD_WriteReg(0X07,0X0037);
	LCD_Clear(color);
	UART_SendStr("LCD Init...		OK\n");
}

void LCD_Clear(u16 color)
{
    u8 x;
    u8 y;
    LCD_SetXY(0,175,0,219);
    for(x=0;x<LCD_XSIZE_TFT;x++)
    {
        for(y=0;y<LCD_YSIZE_TFT;y++)
        {
            LCD_Write_Data(color);
        }
    }
}

/*基础图形************************************************************/
void LCD_DrawPoint(u8 x,u8 y,u16 color)
{
	LCD_WriteReg(0x0021,(y<<8)|x);
	LCD_Write_CMD(0x0022);
 	LCD_Write_Data(color);
}

u16 LCD_GetPoint(u8 x,u8 y)
{
	LCD_WriteReg(0x0021,y<<8|x);
	LCD_Write_CMD(0x0022);
	LCD_Read_Data();  			//虚读
	return LCD_Read_Data();
}

void LCD_DisplaySeg(u8 x,u8 y,u8 seg,u16 fontcolor,u16 backcolor)
{
    u8 i;
    for(i=0; i<8; i++)
    {
         if(seg&(1<<(7-i)))
             LCD_DrawPoint((x<<3)+i,y,fontcolor); 
         else
             LCD_DrawPoint((x<<3)+i,y,backcolor); 
    }
} 

void LCD_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u16 color)
{
	u8 x,y,temp;
	s16 dx,dy,signx,signy,p;

	x = x1;
	y = y1;
	if(x2 > x1)								
	{
	    dx = x2 - x1;	
		signx = 1;					 
	}
	else if(x2 < x1)
	{
	    dx = x1 - x2;	
		signx = -1;					 
	}
	else
	{
	    dx = 0;
		signx = 0;
	}
	if(y2 > y1)								
	{
	    dy = y2 - y1;	
		signy = 1;					 
	}
	else if(y2 < y1)
	{
	    dy = y1 - y2;	
		signy = -1;					 
	}
	else
	{
	    dy = 0;	
		signy = 0;
	}
	p = 2*dy -dx;	
	if(dx >= dy)								
	{
		for(temp=0; temp<dx; temp++)
		{
	        LCD_DrawPoint(x,y,color);	
			if(p >= 0)			
			{
			    x = x + signx;
				y = y + signy;
				p = p + 2*(dy - dx);
			}
			else
			{
				x = x + signx;
				y = y;
				p = p + 2*dy;
			}
		}
    }
	else								
	{
		for(temp=0; temp<dy; temp++)
		{
	        LCD_DrawPoint(x,y,color);
			if(p >= 0)
			{
			    y = y + signy;
				x = x + signx;
				p = p + 2*(dx - dy);
			}
			else
			{
				y = y + signy;
				x = x;
				p = p + 2*dx;
			}
		}
    }
    LCD_DrawPoint(x2,y2,color);
}

void LCD_DrawSquare(u8 x0, u8 y0, u8 x1, u8 y1,u16 color)
{
	LCD_DrawLine(x0,y0,x0,y1,color);
	LCD_DrawLine(x0,y0,x1,y0,color);
	LCD_DrawLine(x0,y1,x1,y1,color);
	LCD_DrawLine(x1,y0,x1,y1,color);
}

void LCD_DrawBlock(u8 x,u8 y,u8 width,u8 high,u16 color)
{
	u8 i=0,j=0;
	LCD_SetXY(x,x+width-1,y,y+high-1);

	for(i=0;i<high;i++)
	{
		for(j=0;j<width;j++)	  
		{
			LCD_Write_Data(color);   
		}
	}
}

void LCD_DrawCircle(u8 x0,u8 y0,u8 r,u16 color)
{
	int a,b,di;

	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		LCD_DrawPoint(x0+a,y0-b,color);             //5
 		LCD_DrawPoint(x0+b,y0-a,color);             //0           
		LCD_DrawPoint(x0+b,y0+a,color);             //4               
		LCD_DrawPoint(x0+a,y0+b,color);             //6 
		LCD_DrawPoint(x0-a,y0+b,color);             //1       
 		LCD_DrawPoint(x0-b,y0+a,color);             
		LCD_DrawPoint(x0-a,y0-b,color);             //2             
  		LCD_DrawPoint(x0-b,y0-a,color);             //7     	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
}

/*字体显示************************************************************/
void LCD_WriteASCII1216(u8 x,u8 y,u8 charr,u8 size,u16 color,u16 backcolor,u8 backcolortrue)
{
	u8 i,j,y0;				//函数内部坐标
	u8 *pZK,mask,buf;       /* 定义缓存变量 指针 */

	y0=y;
	if(size==12)
		pZK = &ascii1206[charr*12];
	else if(size==16)
		pZK = &ascii1608[charr*16];

	for( i = 0 ; i < size ; i++ )
	{
		mask = 0x80;
		buf = *(pZK++);
		for( j = 0 ; j < 8 ; j++ )
		{
			if( buf & mask )		         /* 以上与运算为真则打印 */
			{
				LCD_DrawPoint(x,y,color);
			}
			else
	        {
				if(backcolortrue)
	           		LCD_DrawPoint(x,y,backcolor);
	        }
			y++;
			if((y-y0)==size)			//写完第一列
			{
				y=y0;					//从上方开始写第二列
				x++;					//换列
				if(x>=176){return;}		//超区域了
				break;
			}
			mask >>= 1;			        /* 右移位后进行下一行 */
		}
	}
}

void LCD_WriteASCIIStr1216(u8 x,u8 y,u8 *str,u8 size,u16 color,u16 backcolor,u8 backcolortrue)
{
	while ( * str )
	{
        LCD_WriteASCII1216(x,y,( *str++),size,color,backcolor,backcolortrue);
        x+=size/2;
		if(x>=176)
			return;
	}
}

void LCD_WriteNum(u8 x,u8 y,u32 num,u16 color,u16 backcolor,u8 backcolortrue)
{
    u8 length = 0,i = 0;		 			 /* 数据长度 */
	u32 temp,databuf;
	u8 asciibit = 8;						 /* ASCII位数 */
	databuf	= num;							 /* 缓存数据 因计算长度会使数据为0 */
	if(num == 0)							 /* 如果数据为0 则显示0 */
		LCD_WriteASCII1216(x,y,'0',16,White,Black,1);	 /* 48为0的ASCII值 */
	while(num!=0)							 /* 计算数据长度 */
	{
		num = num / 10;
		length++;
	}
	for(i=0;i<length;i++)					 /* 打印数据 */
	{
		temp = databuf % 10;			     /* 从最高位开始提取数据 */
		databuf = databuf / 10;				 /* 将剩余位数缓存 */
		/* 从最高位开始打印 如4+48=52正是 4的ASCII码值 从ASCII表查找 */				
		LCD_WriteASCII1216(x+(length*asciibit-asciibit),y,temp+48,16,color,backcolor,backcolortrue);
		x -= asciibit;
	}
}

void LCD_WriteNumLen(u8 x,u8 y,u32 num,u8 length,u8 size,u16 color,u16 backcolor,u8 backcolortrue)
{
    u8 i = 0 ; 								//数据长度
	u32 temp;
	u8 asciibit = size/2;				 	//ASCII位数
	for(i=0;i<length;i++)					//打印数据
	{
		temp = num % 10;			     	//从最高位开始提取数据
		num = num / 10;				 		//将剩余位数缓存
		LCD_WriteASCII1216(x+(length*asciibit-asciibit),y,temp+48,size,color,backcolor,backcolortrue);
		x -= asciibit;
	}
}
/************************************************************************************************/
void GetHzMat(u8 *codee,u8 *mat,u8 size)
{		    
	u8 qh,ql;
	u8 i;
	u32 offset;

	qh=*codee;
	ql=*(++codee);
	if(qh<0x81||ql<0x40||ql==0xff||qh==0xff)//非常用汉字
	{   		    
	    for(i=0;i<(size*2);i++)*mat++=0x00;//填充满格
	    return; //结束访问
	}										//以12号的"壹"为例
	if(ql<0x7f)ql-=0x40;					//qh=D2		
	else ql-=0x41;							//ql=BC		ql=0X7b=123
	qh-=0x81;								//			qh=0X51=81
											//offset=0x5AE58=372312		（在GBK12里的偏移量）
	if(size==16)							//offset=0x116E58=1142360	（在Flash芯片里的偏移量）
	{													//offset=190*124*32=0xb8100
		offset=((u32)190*qh+ql)*(size*2);				//得到字库中的字节偏移量
		W25Q16_FastReadData(offset,32,mat);				//GBK16的基地址为0
	}
	if(size==12)
	{	
		offset=((u32)190*qh+ql)*(size*2)+766080;		//得到字库中的字节偏移量
		W25Q16_FastReadData(offset,24,mat);				//GBK12基地址为766080
	}
}

//显示汉字
void LCD_ShowFont(u8 x,u8 y,u8 *font,u8 size,u16 color,u16 bak_color,u8 mode)
{
	u8 temp,t,t1;
	u16 y0=y;
	u8 dzk[32];
	if(size!=12&&size!=16)return;	//不支持的size
	GetHzMat(font,dzk,size);		//得到相应大小的点阵数据
    for(t=0;t<size*2;t++)
    {   												   
	    temp=dzk[t];
        for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
				LCD_DrawPoint(x,y,color);
			else
				if(mode)
					LCD_DrawPoint(x,y,bak_color);
			temp<<=1;
			y++;
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x==176)
					return;
				break;
			}
		}  	 
	}    
}

void LCD_ShowFontStr(u8 x,u8 y,u8* str,u8 size,u16 color,u16 bak_color,u8 mode)
{
	while(*str!=0)
	{
		if(*str<0x80)	//为ASCII码
		{
			str++;
		}
		else			//为汉字
		{
			LCD_ShowFont(x,y,str,size,color,bak_color,mode);
			str+=2;
			x+=size;
			if(x>=176)
				return;
		}
	}
}

//在指定位置开始显示一个字符串(ASCII和汉字混排)
//支持自动换行
//(x,y):起始坐标
//width,height:区域
//str  :字符串
//size :字体大小
//在之前的基础上，每次换行自动增加2行间距，使段落看起来松弛些
//bug：换行后，下一行第一个字符前会有个空格？
//	   如果换行的前一个字符已达屏幕最右侧，换行可以取消。
u16 LCD_ShowStr(u8 x,u8 y,u8 width,u8 height,u8 *str,u8 size,u16 color,u16 bak_color,u8 mode)
{					
	u16 x0=x;
	u16 y0=y;
	u8 bHz=0;     //字符或者中文
	u8 *str_start=str;

	while(*str!=0)//数据未结束
	{
		if(*str>0x80)
				bHz=1;//中文
		if(!bHz)//英文
		{
			if(x>(x0+width-size/2))//换行
			{
				if(mode)
					LCD_DrawBlock(x,y,width+x0-x,size,bak_color);
				y+=size;
				y+=2;
				x=x0;
			}
			if(y>(y0+height-size))
				break;//越界返回
			if(*str==13)//换行符号
			{
				if(mode)
					LCD_DrawBlock(x,y,width+x0-x,size,bak_color);

				if(x!=x0)		//如果不是新的一行，就换行
				{
					y+=size;
					y+=2;
				}
				x=x0;
				str+=2;			//回车：0x0D,0x0A
			}
			else
			{
				LCD_WriteASCII1216(x,y,*str,size,color,bak_color,mode);
				str++;
				x+=size/2; //字符,为全字的一半
			}
        }
		else//中文
		{
            bHz=0;
			if(x>(x0+width-size))//不够显示一个字的空间，换行处理
			{
				if(mode)
					LCD_DrawBlock(x,y,width+x0-x,size,bak_color);
				y+=size;
				y+=2;
				x=x0;
			}
	        if(y>(y0+height-size))
				break;//越界返回
			LCD_ShowFont(x,y,str,size,color,bak_color,mode);
	        str+=2;
	        x+=size;//下一个汉字偏移
        }
    }
	return (str-str_start);
}

//在指定宽度的中间显示字符串
//如果字符长度超过了len,则用Show_Str显示
//len:指定要显示的宽度
void LCD_ShowStrMid(u8 x,u8 y,u8*str,u8 size,u8 len,u16 color,u16 bak_color)
{
	u8 strlenth=0;
   	strlenth=strlen((const char*)str);
	strlenth*=size/2;
	if(strlenth>len)
		LCD_ShowStr(x,y,176,220,str,size,color,bak_color,0);
	else
	{
		strlenth=(len-strlenth)/2;
	    LCD_ShowStr(strlenth+x,y,176,220,str,size,color,bak_color,0);
	}
}

/*高级操作**********************************************************************************/
u16 LCD_RGB5Conv(u8 r, u8 g, u8 b)
{
	return  (r<<11)|(g<<5)| (b);
}

u16 LCD_RGB8Conv(u8 r, u8 g, u8 b)
{
	return  ((r>>3)<<11)|((g>>2)<<5)|(b>>3);
}

u16 LCD_RGB565To555(u16 color)
{
	u8 temp_b=0;
	temp_b=color&0x1f;
	color>>=1;
	color=color&0xffe0|temp_b;
	return color;
}

void LCD_Show_Photo(u8 x,u8 y,u8 width,u8 height,u8 *pic)
{
	u16 color;
	u8 v,h;

	LCD_SetXY(x,x+width-1,y,y+height-1);

	for(v=0;v<height;v++)
	{
		for(h=0;h<width;h++)
		{
			color=*pic++;
			color|=((*pic++)<<8);
//			if(color!=0xffff)
				LCD_Write_Data(color);
//			else
//				LCD_Write_Data(LCD_GetPoint(x+h,y+v));
		}
	}
}
//白色的按钮，透明效果
//void LCD_Show_Anniu(u8 x,u8 y,u8 width,u8 height,u8 *pic)
//{
//	u16 color;
//	u8 v,h;
//
//	for(v=0;v<height;v++)
//	{
//		for(h=0;h<width;h++)
//		{
//			color=*pic++;
//			color|=((*pic++)<<8);
//			if(color==0xffff)
//				continue;
//			else
//				LCD_DrawPoint(x+h,y+v,0xffff);
//		}
//	}
//}

void LCD_ColourBar(u8 x,u8 y,u8 width,u8 height,u16 color,u8 dir)
{
	u8 v,h;
	
	for(v=0;v<height;v++)
	{
		LCD_SetXY(x,x+width-1,y+v,y+v);
		for(h=0;h<width;h++)
		{
			LCD_Write_Data(color);
		}
		if(dir)
			color+=3;
		else
			color-=3;
		if(color>31)
			color=31;
	}
}

void LCD_CopyBMP(u8 x0,u8 y0,u8 width,u8 height,u8 x1,u8 y1)
{
	u8 v,h;

	for(v=0;v<height;v++)
	{
		for(h=0;h<width;h++)
		{
			LCD_DrawPoint(x1+h,y1+v,LCD_GetPoint(x0+h,y0+v));
		}
	}
}

//r1,g1,b1为原色值分量
//r2,g2,b2为新色值分量
//alpha为透明度(1-255),必须为非0值！！alpha值越大，越显示新图像,1显示原图像
void LCD_Alpha(u8 x0,u8 y0,u8 width,u8 height,u8 *source,u8 alpha)
{
	u8 v,h,r1,g1,b1,r2,g2,b2;
	u16 color;

	if(alpha!=0)
	{
		for(v=0;v<height;v++)
		{
			for(h=0;h<width;h++)
			{
				color=LCD_GetPoint(x0+h,y0+v);
				r1=color>>11; 
				g1=(color>>5)&0x003f; 						//原色值分量
				b1=color&0x001f;
				color=*source++;
				color|=((*source++)<<8);
				r2=color>>11;
				g2=(color>>5)&0x003f; 						//新色值分量
				b2=color&0x001f;	
				r1 = r1/alpha + r2/alpha*(alpha-1);
				g1 = g1/alpha + g2/alpha*(alpha-1);			//颜色混和
				b1 = b1/alpha + b2/alpha*(alpha-1);
				color=LCD_RGB5Conv(r1,g1,b1);				//转换为RGB565
				LCD_DrawPoint(x0+h,y0+v,color);				//刷新混合后的色点
			}
		}
	}
}

void LCD_AlphaWithColor(u8 x0,u8 y0,u8 width,u8 height,u16 SourceColor,u8 alpha)
{
	u8 v,h,r1,g1,b1,r2,g2,b2;
	u16 color;

	if(alpha!=0)
	{
		for(v=0;v<height;v++)
		{
			for(h=0;h<width;h++)
			{
				color=LCD_GetPoint(x0+h,y0+v);
				r1=color>>11; 
				g1=(color>>5)&0x003f; 						//原色值分量
				b1=color&0x001f;
//				color=*source++;
//				color|=((*source++)<<8);
				r2=SourceColor>>11;
				g2=(SourceColor>>5)&0x003f; 				//新色值分量
				b2=SourceColor&0x001f;
				r1 = r1/alpha + r2*(alpha-1)/alpha;
				g1 = g1/alpha + g2*(alpha-1)/alpha;			//颜色混和
				b1 = b1/alpha + b2*(alpha-1)/alpha;
				color=LCD_RGB5Conv(r1,g1,b1);				//转换为RGB565
				LCD_DrawPoint(x0+h,y0+v,color);				//刷新混合后的色点
			}
		}
	}
}














