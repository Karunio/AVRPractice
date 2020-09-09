/*
* LedTest1.c
*
* Created: 2020-06-12 오후 3:39:33
*  Author: Karunio
*/

#include <avr/io.h>
#include <util/delay.h>

int main()
{
	unsigned char LED_Data = 0x01;
	DDRA = 0xFF;
	
	while(1)
	{
		for(int i = 0; i < 7; i++)
		{
			PORTA = LED_Data;
			LED_Data <<= 1;
			_delay_ms(100);
		}
		
		for(int i = 0; i < 7; i++)
		{
			PORTA = LED_Data;
			LED_Data >>= 1;
			_delay_ms(100);
		}
	}
	return 0;
}