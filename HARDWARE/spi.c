#include "spi.h"

/****************************************************************************************************
STC硬件SPI接口，SD卡和VS1003共用
** SD_CS   P1^4
** SD_MOSI P1^5
** SD_MISO P1^6 
** SD_SCLK P1^7   
****************************************************************************************************/
void spi_init(u8 n) //STC 硬件SPI初始化，n为分频数，影响SPI的通信速度
{
	switch(n)
	{
	 case 4:
	     SPCTL=0xdc;
		 break;
	 case 16:
	     SPCTL=0xdd;
		 break;
	 case 64:
	     SPCTL=0xde;
		 break;
	 case 128:
	     SPCTL=0xdf;
		 break;
	}
	SPSTAT=0xc0;
}

void SD_spi_write(u8 x)	//SPI 写入一个字节(STC 硬件SPI)
{
	SPDAT=x;
	while(!(SPSTAT&0x80));
	SPSTAT=0xc0;
}

u8 SD_spi_read() //SPI 读取一个字节(STC 硬件SPI)
{
	SPDAT=0xff;
	while(!(SPSTAT&0x80));
	SPSTAT=0xc0;
	return SPDAT;
}

u8 SD_SPI_ReadWriteByte(u8 dat)
{
	SPDAT=dat;
	while(!(SPSTAT&0x80));
	SPSTAT=0xc0;
	return SPDAT;
}

void VS_WriteByte(u8 w_dat) 
{
	SD_SPI_ReadWriteByte(w_dat);
}

u8 VS_ReadByte(void)
{  
	return SD_SPI_ReadWriteByte(0xff);
}

/***************************************************************************************************/
//IO口模拟SPI，供Flash存储芯片用
sbit SPI_SCLK =P3^4;
sbit SPI_MOSI =P3^5;
//sbit SPI_CS =P3^6;
sbit SPI_MISO =P3^7;
/***************************************************************************************************/
u8 bdata dat;
sbit dat7=dat^7;
sbit dat6=dat^6;
sbit dat5=dat^5;
sbit dat4=dat^4;
sbit dat3=dat^3;
sbit dat2=dat^2;
sbit dat1=dat^1;
sbit dat0=dat^0;

void SPI_Flash_Write(u8 w_dat)
{
	dat=w_dat; 
	SPI_MOSI=dat7;SPI_SCLK=0;SPI_SCLK=1;
	SPI_MOSI=dat6;SPI_SCLK=0;SPI_SCLK=1;
	SPI_MOSI=dat5;SPI_SCLK=0;SPI_SCLK=1;
	SPI_MOSI=dat4;SPI_SCLK=0;SPI_SCLK=1;
	SPI_MOSI=dat3;SPI_SCLK=0;SPI_SCLK=1;
	SPI_MOSI=dat2;SPI_SCLK=0;SPI_SCLK=1;
	SPI_MOSI=dat1;SPI_SCLK=0;SPI_SCLK=1;
	SPI_MOSI=dat0;SPI_SCLK=0;SPI_SCLK=1;
}

u8 SPI_Flash_Read(void)
{  
	SPI_MISO=1;
	SPI_SCLK=1;SPI_SCLK=0;dat7=SPI_MISO; 
	SPI_SCLK=1;SPI_SCLK=0;dat6=SPI_MISO;
	SPI_SCLK=1;SPI_SCLK=0;dat5=SPI_MISO;
	SPI_SCLK=1;SPI_SCLK=0;dat4=SPI_MISO;
	SPI_SCLK=1;SPI_SCLK=0;dat3=SPI_MISO;
	SPI_SCLK=1;SPI_SCLK=0;dat2=SPI_MISO;
	SPI_SCLK=1;SPI_SCLK=0;dat1=SPI_MISO;
	SPI_SCLK=1;SPI_SCLK=0;dat0=SPI_MISO;
	return (dat);
}










