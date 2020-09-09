/*
* Input.c
*
* Created: 2020-06-12 오후 5:16:00
* Author : Karunio
*/

#include <avr/io.h>
#include <util/delay.h>

char fnds[] = { 0x3F, 0x06, 0x5B, 0x4F,
				0x66, 0x6D, 0x7D, 0x27,
				0x7F, 0x6F, 0x77, 0x7C,
				0x39, 0x5E, 0x79, 0x71,
				0x40, 0x80 };

int main(void)
{
	DDRA = 0xFF;
	DDRD = 0x0F;
	
	int digit_counts[] = {0, 0, 0, 0};
	char digits[] = {0x07, 0x0B, 0x0D, 0x0E};
	
	while (1)
	{
		digit_counts[0]++;
		
		for(int i = 0; i < 3; i++)
		{
			if(digit_counts[i] >= 10)
			{
				digit_counts[i+1]++;
				digit_counts[i] = 0;
			}
		}
		
		if(digit_counts[3] >= 10)
		{
			for(int i = 0; i < 4; i++)
			{
				digit_counts[i] = 0;
			}
		}
		
		for(int i = 0; i < 4; i++)
		{
			PORTD = digits[i];
			PORTA = fnds[digit_counts[i]];
			_delay_ms(5);
		}
	}
}