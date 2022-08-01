#include "stm32f10x.h"	// Device header
#include "usart.h"
#include "delay.h"
#include "OLED.h"
#include <math.h>

int main(void)
{	
	uint32_t i=0;
	delay_init();
	usart_init(115200);
	OLED_Init();
	while(1)
	{
		sendCommand((int)(sin(i)*100.0));
		OLED_ShowNum(1, 1, i, 6);
		delay_ms(100);
		i++;
	}
}
