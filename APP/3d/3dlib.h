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
#ifndef _3dlib_h
#define _3dlib_h

#include "../includes.h"

/* 描述一个2D像素点 */
typedef struct _TYPE_2D_PIXEL{
    INT16U    x;
    INT16U    y;
	INT16U    color;
}type_2d_pixel;
/* 描述一个3D像素点 */
typedef struct _TYPE_3D_PIXEL{
    INT16S    x;
    INT16S    y;
    INT16S    z;
	INT16U    color;
}type_3d_pixel;

/* 描述旋转角度 */
typedef struct _TYPE_ROTATION_ANGLE{
    FP32     x;
    FP32     y;
	FP32     z; 
}type_rot_ang;

/* 描述移动位置 */
typedef struct _TYPE_TRANSLATION_DIST{
    FP32    x;
	FP32    y;
	FP32    z;
}type_tran_dis;

void Pixel3dTransform(type_rot_ang *pangle, type_tran_dis *ptrandis, type_3d_pixel *ppixel, type_2d_pixel *ppreturn);

#endif
