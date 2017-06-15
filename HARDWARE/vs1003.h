#ifndef __VS1003_H__
#define __VS1003_H__

//����ͷ�ļ�
#include "sys.h"
//Ӳ���ӿڶ���
sbit VS_CS   =P2^0;
sbit VS_DCS  =P2^7;
sbit VS_RST  =P4^2;
sbit VS_DREQ =P2^1;

//VS1003�Ĵ����궨��
#define VS_WRITE_CMD        0x02    //д����
#define VS_Read             0x03    //������
#define VS_MODE             0x00    //����ģʽ
#define VS_STATUS           0x01    //VS1003 ״̬
#define VS_BASS             0x02    //���ø�/������ǿ��
#define VS_CLOCKF           0x03    //ʱ��Ƶ�� + ��Ƶ��
#define VS_DECODE_TIME      0x04    //ÿ��������
#define VS_AUDATA           0x05    //��Ƶ����
#define VS_WRAM             0x06    //RAM ��д
#define VS_WRAMADDR         0x07    //RAM ��д��ַ
#define VS_HDAT0            0x08    //��ͷ����0
#define VS_HDAT1            0x09    //��ͷ����1
#define VS_AIADDR           0x0A    //Ӧ�ó�����ʼ��ַ
#define VS_VOL              0x0B    //��������
#define VS_AICTRL0          0x0C    //Ӧ�ÿ��ƼĴ���0
#define VS_AICTRL1          0x0D    //Ӧ�ÿ��ƼĴ���1
#define VS_AICTRL2          0x0E    //Ӧ�ÿ��ƼĴ���2
#define VS_AICTRL3          0x0F    //Ӧ�ÿ��ƼĴ���3
//��������
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
