#ifndef __VS1003_H__
#define __VS1003_H__

//包含头文件
#include "sys.h"
//硬件接口定义
sbit VS_CS   =P2^0;
sbit VS_DCS  =P2^7;
sbit VS_RST  =P4^2;
sbit VS_DREQ =P2^1;

//VS1003寄存器宏定义
#define VS_WRITE_CMD        0x02    //写命令
#define VS_Read             0x03    //读命令
#define VS_MODE             0x00    //控制模式
#define VS_STATUS           0x01    //VS1003 状态
#define VS_BASS             0x02    //内置高/低音增强器
#define VS_CLOCKF           0x03    //时钟频率 + 倍频数
#define VS_DECODE_TIME      0x04    //每秒解码次数
#define VS_AUDATA           0x05    //音频数据
#define VS_WRAM             0x06    //RAM 读写
#define VS_WRAMADDR         0x07    //RAM 读写地址
#define VS_HDAT0            0x08    //流头数据0
#define VS_HDAT1            0x09    //流头数据1
#define VS_AIADDR           0x0A    //应用程序起始地址
#define VS_VOL              0x0B    //音量控制
#define VS_AICTRL0          0x0C    //应用控制寄存器0
#define VS_AICTRL1          0x0D    //应用控制寄存器1
#define VS_AICTRL2          0x0E    //应用控制寄存器2
#define VS_AICTRL3          0x0F    //应用控制寄存器3
//函数声明
void VS_WriteReg(u8 addr,u16 vsdata);
u16  VS_ReadReg(u8 addr);
void VS_LoadPatch(void);
void VS_GetSpec(u16 *p);
void VS_SetVolume(u8 volume);
void VS_Init(void);
void VS_SendDat(u8 dat);
void VS_SendDat32(u8 *dat);
void VS_FlushBuffer(void);
void VS_SinTest(u8 x);
void VS_SoftReset();
void VS_HardReset();
u16 VS1003B_ReadDecodeTime();
void VS1003B_SetDecodeTime0();
u16 VS_Get_HeadInfo(void);
void VS_SetBass(u8 treble,u8 tfreq,u8 bass,u8 bfreq);

#endif	/*END __VS1003_H__ */
