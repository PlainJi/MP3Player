#ifndef __SPIFLASH_H__
#define __SPIFLASH_H__

#include "sys.h"

//W25X系列/Q系列芯片列表	   	
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
//功能寄存器地址
#define WRITE_ENABLE 	0X06
#define WRITE_DISABLE 	0X04
#define READ_ID 		0X9F
#define READ_STAUS_REG 	0X05
#define WRITE_STAUS_REG 0X01
#define READ_DATA 		0X03
#define FAST_READ_DATA 	0X0B
#define PAGE_PROGRAM 	0X02
#define SECTOR_ERASE 	0X20			//20H	是sector erase
#define BLOCK_ERASE		0XD8			//D8H	是block erase
#define BULK_ERASE 		0XC7
#define DEEP_POWER_DOWN 0XB9
#define WAKE_UP 		0XAB

//片选信号
sbit SPI_CS = P3^6;
#define Select_Flash()     SPI_CS=0
#define NotSelect_Flash()  SPI_CS=1
//调用函数
void W25Q16_ReadID(u8 *id);
void W25Q16_WriteEnable(void);
void W25Q16_WriteDisable(void);
u8 W25Q16_ReadStatusReg(void);
void W25Q16_WriteStatusReg(u8 reg);
void W25Q16_PageProgram(u32 addr,u16 len,u8 *buf);
void W25Q16_SectorErase(u32 addr);
void W25Q16_BulkErase(void);
void W25Q16_DeepPowerDown(void);
u8 W25Q16_WakeUp(void);
void W25Q16_FastReadData(u32 addr,u16 len,u8 *buf);
void W25Q16_SendPage(u32 addr);
void W25Q16_Init(void);
u8 Test_FlashRW();
void UpdateFileBySD(u8 num);
void LCD_Show_Anniu(u8 x,u8 y,u8 width,u8 height,u32 addr);

#endif