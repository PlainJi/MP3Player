/*******************************************************************************************
 * This file is part of avr 3d  program
 *
 * AVR S65 3D Project 
 * By MacroXu <kk20y@yahoo.com.cn> Copyright (C) 2008,2009 
 * 
 * This code is free, You can redistribute it and/or modify it on the following conditions,
 * Send me the modified source code and comment the code you changed.I am credited for the 
 * original work in all derivative code.
 *******************************************************************************************
 */
#include "../stc12c5a60s2.h"
#include "math.h"
#include "3dlib.h"

/* 用户和显示屏的视角关系定义 */
#define     USRX    0.0
#define     USRY    0.0
#define     USRZ    46.0   // OFFSETZ*0.707,1:1比例显示

/* 显示屏中心偏移位置定义,3D坐标的零点位置 */
#define     OFFSETX 80
#define     OFFSETY 120
#define     OFFSETZ 60

/* 3d转2D函数, pangle-旋转角度，ptrandis-移动位置, ppixel－要转换的像素, ppreturn-转换后的2D值保存到这 */
void Pixel3dTransform(type_rot_ang *pangle, type_tran_dis *ptrandis, type_3d_pixel *ppixel, type_2d_pixel *ppreturn)
{
    /* 定义一些变量 */
	FP32 sinx,cosx,siny,cosy,sinz,cosz;
	FP32 tempx,tempy,tempz;
	FP32 dx,dy,dz;

    /* 将需要用到的三角函数先计算完成，加快程序执行速度 */
	sinx = sin(pangle->x);
  	cosx = cos(pangle->x);
  	siny = sin(pangle->y);
  	cosy = cos(pangle->y);
  	sinz = sin(pangle->z);
  	cosz = cos(pangle->z);

    /* 开始计算和转换 */
    dx = ppixel->x;
	dy = ppixel->y;
	dz = ppixel->z;
    tempy = dy*cosx - dz*sinx;
    tempz = dy*sinx + dz*cosx;	
    dy = tempy;
    dz = tempz;
    tempx = dx*cosy - dz*siny;	
    tempz = dx*siny + dz*cosy;	
    dx = tempx;
    dz = tempz;
    tempx = dx * cosz - dy * sinz;	
    tempy = dx * sinz + dy * cosz;	
    dx = tempx;
    dy = tempy;

    dx = dx + ptrandis->x;
	dy = dy + ptrandis->y;
	dz = dz - ptrandis->z + OFFSETZ;

    ppreturn->x = ((dx-USRX)*(USRZ/dz)) + OFFSETX;
	ppreturn->y = ((dy-USRY)*(USRZ/dz)) + OFFSETY;
    ppreturn->color = ppixel->color;
}


