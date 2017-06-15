#include "uart.h"
#include "LG4525.h"
#include "myfun.h"
#include "spiflash.h"
#include "malloc.h"
#include "timer.h"
#include "vs1003.h"
#include "exfun.h"
#include "touch.h"
#include "GUI_Index.h"
#include "GUI_MP3.H"
#include "ADC.h"
#include "PWM.h"
#include "mmc_sd.h"

void main(void)
{
	Delay_Ms(500);
	UART_Init(57600);
	PWM0_Init(1,255);
	LCD_Init(White);
	InitTimer0();
	ADC_Init();
	TP_Init();
	RAM_Init();
	W25Q16_Init();
	VS_Init();
	while(Init_Fatfs());
	Index();
	WaitForDownload;
}
















