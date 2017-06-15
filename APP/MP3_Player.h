#ifndef __MP3_PLAYER_H
#define __MP3_PLAYER_H

#include "sys.h"
#define TP_MP3_PLAY		(ScreenX>64 && ScreenX<108&& ScreenY>174&& ScreenY<218)
#define TP_MP3_PRE		(ScreenX>16 && ScreenX<48 && ScreenY>185&& ScreenY<217)
#define TP_MP3_NEXT 	(ScreenX>128&& ScreenX<160&& ScreenY>185&& ScreenY<217)
#define TP_MP3_EXIT 	(ScreenX>3  && ScreenX<35 && ScreenY>3  && ScreenY<35)
#define TP_MP3_CHANGE 	(ScreenX>92&& ScreenX<140&& ScreenY>21 && ScreenY<33)
#define TP_mp3_LIST 	(ScreenX>141&& ScreenX<173&& ScreenY>3  && ScreenY<35)

extern data u8  LastProBar;		//上次进度条位置
extern data u8  NowProBar;		//本次进度条位置
extern data u16 DecodeTime;		//本次解码时长
extern data u16 LastDecodeTime;	//上次解码时长
extern data u16 TotalTime;		//歌曲总时长
extern data u16 Kbps;			//歌曲的位速
extern data u16 playingnow;		//当前曲目号
extern data u8  volume;
extern data u8  VS_treble;
extern data u8  VS_tfreq;
extern data u8  VS_bass;
extern data u8  VS_bfreq;

u8 MP3_Play(u8 state);

#endif