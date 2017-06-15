#include "../gui/bgui.h"
#include "3d_yanshi.h"
#include "math.h"
#include "3dlib.h"
#include "../lcddrv/lcd5420.h"
#include "../gui/SIMPLE_GUI_LIB.h"

#define     OFFSETX 88
#define     OFFSETY 110
#define     OFFSETZ 60

///////////////////////void Disply3D(float xpos,float ypos,float zpos) 函数用/////////////////////////////////////////////////////////
/* 以下部分定义了一个立方体 */ 
const signed int code aa[23]={20,-20,-20,20,   20,-20,-20,20}; 		// x data for shape vertex 
const signed int code bb[23]={20,20,-20,-20,   20,20,-20,-20}; 		// y data for shape vertex 
const signed int code cc[23]={-20,-20,-20,-20, 20,20,20,20};   		// z data for shape vertex 

const int code ff[22]={1,2,3,4,5,6,7,8,1,2,3,4,9,9,12,14,14,17,19,20,21,22};		// start
const int code gg[22]={2,3,4,1,6,7,8,5,5,6,7,8,10,11,13,15,16,19,20,21,18,23};		// end

/* 测试1，角度旋转 */
void Test3D1(void)
{
    INT16U i,j,k;
	type_rot_ang angle;	
	type_tran_dis position;
	type_2d_pixel object2d[36];		//转换后的存放位置
	type_3d_pixel object3d[36];		//原
   
    k = 0;
    for(i=0;i<6;i++)
	{
        for(j=0; j<6; j++)
		{
		    object3d[k].x = j*8;					//点阵间距
	        object3d[k].y = 0;
	        object3d[k].z = i*8;
			object3d[k].color = LcdRGBConv(i*5,40,j*5);	
			k++;
		}
    }
    angle.x = 0.0;
    angle.y = 0.0;
    angle.z = 0.0;

    position.x = 0.0;
    position.y = 0.0;
	position.z = 0.0;

    LCD_Clear(0x0000);
	GuiDisStringF8X16(0,0,"3D Demo       Plain_Ji",String_Color,Back_Color);
	while(1)
	{	
		if(P3==(P3&0xFE))IAP_CONTR=0x60; /* 自动检测下载 */
	    angle.x = angle.x + 0.07;
	    angle.y = angle.y + 0.1;		 //选转方向
	    angle.z = angle.z + 0.05;
	
	    position.x = 0;
	    position.y = 0;
		position.z = 0;
	
	    for(i=0; i<36; i++)
		{
	        GuiDrawCircle(object2d[i].x, object2d[i].y,1, 0);
			LCD_DrawPoint(object2d[i].x, object2d[i].y, 0);
		    Pixel3dTransform(&angle, &position, &object3d[i], &object2d[i]);
		    LCD_DrawPoint(object2d[i].x, object2d[i].y, object2d[i].color);
	        GuiDrawCircle(object2d[i].x, object2d[i].y,1, object2d[i].color);
	    }
	
	}
}

/* 测试2，移位 */
void Test3D2(void)
{
	INT8U i,j,sign;
	type_rot_ang  angle;	
	type_tran_dis position;
	type_2d_pixel xdata object2d[20];
	type_3d_pixel xdata object3d[20];
	if(P3==(P3&0xFE))
		IAP_CONTR=0x60; /* 自动检测下载 */
	for(i=0; i<10; i++)
	{
        object3d[i].x = i*5-30;
        object3d[i].y = i*5-20;
        object3d[i].z = 10;
	    object3d[i].color = LcdRGBConv(i*3, 45, i*3);
	}
	for(i=10; i<20; i++)
	{
        object3d[i].x = (i-10)*5-30;
        object3d[i].y = (i-10)*5-20;
        object3d[i].z = -10;
	    object3d[i].color = LcdRGBConv((i-10)*3, 45, (i-10)*3);
	}

    angle.x = 0.0;
    angle.y = 0.0;
    angle.z = 0.0;

    position.x = 0.0;
    position.y = 0.0;
	position.z = 0.0;

    LCD_Clear(0x0000);
    GuiDisStringF8X16(0,0,"3D Demo       Plain_Ji",String_Color,Back_Color);
	while(1)
	{	
		if(P3==(P3&0xFE))IAP_CONTR=0x60; /* 自动检测下载 */
	    angle.x = angle.x + 0.08;
	    angle.y = angle.y + 0.1;
	    angle.z = angle.z + 0.05;
	
	    position.x = 0;
	    position.y = 0;
		if(position.z==15)
		    sign = 1;
	    if(position.z==-15)
		    sign = 0;
	    if(sign == 0)
		    position.z = position.z+1;
	    else
		    position.z = position.z-1;
	
	    for(i=0,j=10; i<10; i++,j++)
		{
			GuiDrawLine(object2d[i].x, object2d[i].y, object2d[j].x, object2d[j].y, 0);
		    Pixel3dTransform(&angle, &position, &object3d[i], &object2d[i]);
		    Pixel3dTransform(&angle, &position, &object3d[j], &object2d[j]);
		    GuiDrawLine(object2d[i].x, object2d[i].y, object2d[j].x, object2d[j].y, (object2d[i].color+object2d[i].color)/2);
	    }
	
	}

}

/* 测试3 */
void Test3D3(void)
{
    INT16U i,j,k,l;
	type_rot_ang angle;	
	type_tran_dis position;
	type_2d_pixel xdata object2d[27];
	type_3d_pixel xdata object3d[27];
   
    k = 0;
	for(l=0;l<3;l++)
	{
	    for(i=0;i<3;i++)
		{
	        for(j=0; j<3; j++)
			{
			    object3d[k].x = j*20 - 30;
		        object3d[k].y = i*20 - 30;
		        object3d[k].z = l*20 - 30;
				object3d[k].color = 0xffff;
				k++;
			}
	
	     }
	}
    angle.x = 0.0;
    angle.y = 0.0;
    angle.z = 0.0;

    position.x = 0.0;
    position.y = 0.0;
	position.z = 0.0;

    LCD_Clear(0x0000);
	while(1)
	{	
		if(P3==(P3&0xFE))IAP_CONTR=0x60; /* 自动检测下载 */
	    angle.x = angle.x + 0.18;
	    angle.y = angle.y + 0.08;
	    angle.z = angle.z + 0.03;
		GuiDrawLogo(176-35, 0, 35, 13);
//		GuiDrawImage( 0, 1, 2, 3, texture_logo);
	    for(i=0; i<27; i++)
		{
	        GuiDrawCircle(object2d[i].x, object2d[i].y,1, 0);
			LCD_DrawPoint(object2d[i].x, object2d[i].y, 0);
		    Pixel3dTransform(&angle, &position, &object3d[i], &object2d[i]);
		    LCD_DrawPoint(object2d[i].x, object2d[i].y, object2d[i].color);
	        GuiDrawCircle(object2d[i].x, object2d[i].y,1, object2d[i].color);
	    }	        
	}		   
}

/**************************************************************************
*                          3D显示主程序
入口参数：图形坐标
出口参数：无
***************************************************************************/
void Disply3D(float xpos,float ypos,float zpos)     
{
	int newx[23];                               // translated screen x co-ordinates for vertex
	int newy[23];                               // translated screen y co-ordinates for vertex
	int i;                                    // temp variable for loops
	int vertex;
	float xt,yt,zt,x,y,z,sinax,cosax,sinay,cosay,sinaz,cosaz;     // lots of work variables
	float rotx=0.0;					// starting amount of x rotation
	float roty=0.0;					// starting amount of y rotation
	float rotz=0.0;					// starting amount of z rotation	
	int sx,sy,ex,ey;				// define global vars for calling graphics subroutines

	while(1)
	{
		if(P3==(P3&0xFE))IAP_CONTR=0x60; /* 自动检测下载 */
		xpos=xpos+0.0;                      // move the object
		ypos=ypos+0.0;                     // it would wander off screen
		zpos=zpos+0.0;                      // really quick, so leave it centered
		rotx=rotx+0.12;                        // rotate the cube on X axis               	越大，此方向转的越快
		roty=roty+0.03;                        // and on its y axis
		rotz=rotz+0.01;                        // dont bother with z or it gets confusing
		
		sinax=sin(rotx);                     // precalculate the sin and cos values
		cosax=cos(rotx);                  // for the rotation as this saves a 
		
		sinay=sin(roty);                    // little time when running as we
		cosay=cos(roty);                  // call sin and cos less often
		
		sinaz=sin(rotz);                    // they are slow routines
		cosaz=cos(rotz);                 // and we dont want slow!
	
		for (i=0; i<23; i++)              // translate 3d vertex position to 2d screen position
		{
			x=aa[i];                            // get x for vertex i
			y=bb[i];                            // get y for vertex i
			z=cc[i];                             // get z for vertex i
			
			yt = y * cosax - z * sinax;            // rotate around the x axis
			zt = y * sinax + z * cosax;           // using the Y and Z for the rotation
			y = yt;
			z = zt;
			
			xt = x * cosay - z * sinay;           // rotate around the Y axis
			zt = x * sinay + z * cosay;          // using X and Z
			x = xt;
			z = zt;
			
			xt = x * cosaz - y * sinaz;            // finaly rotate around the Z axis
			yt = x * sinaz + y * cosaz;           // using X and Y
			x = xt;
			y = yt;
			
			x=x+xpos;                                    // add the object position offset
			y=y+ypos;                                    // for both x and y
			z=z+OFFSETZ-zpos;                 // as well as Z
			
			newx[i]=(x*64/z)+OFFSETX;          // translate 3d to 2d coordinates for screen
			newy[i]=(y*64/z)+OFFSETY;          // drawing so we can see the cube
		}
		
		for (i=0; i<22; i++)                // draw the lines that make up the object
		{
			vertex=ff[i]-1;                // temp = start vertex for this line
			sx=newx[vertex];               // set line start x to vertex[i] x position
			sy=newy[vertex];               // set line start y to vertex[i] y position
			vertex=gg[i]-1;                   // temp = end vertex for this line
			ex=newx[vertex];               // set line end x to vertex[i+1] x position
			ey=newy[vertex];               // set line end y to vertex[i+1] y position
			LCD_DrawLine(sx, sy, ex, ey,Green);
		}
		/* 清屏 */
		for (i=0; i<22; i++)                                    
		{
			vertex=ff[i]-1;      
			sx=newx[vertex];       
			sy=newy[vertex];  
			vertex=gg[i]-1;      
			ex=newx[vertex]; 
			ey=newy[vertex];
			LCD_DrawLine(sx, sy, ex, ey, Black);
		
		}
	}
}