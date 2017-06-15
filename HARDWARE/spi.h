#ifndef __SPI_H__
#define __SPI_H__

#include "sys.h"

void spi_init(u8 n);
void SD_spi_write(u8 x);
u8 SD_spi_read();
u8 SD_SPI_ReadWriteByte(u8 dat);

//SPI FOR SPI_FLASH
void SPI_Flash_Write(u8 w_dat);
u8 SPI_Flash_Read(void);

//SPI FOR vs1003
void VS_WriteByte(u8 w_dat);
u8 VS_ReadByte(void);

#endif