/*
 * Motor.c
 *
 * Created: 2020-06-18 오전 9:25:29
 * Author : Karunio
 */ 

#include <avr/io.h>
#include <util/delay.h>


int main(void)
{
	DDRD = 0xFF;
	DDRB = (1 << DDRB0);
	PORTB &= ~(1 << PORTB0);
	char index = 0x01;
	char count = 1;
    /* Replace with your application code */
    while (1) 
    {
		char tmpIndex = index;
		if(count % 2 == 0)
		{
			if(tmpIndex == 0x01)
			{
				tmpIndex |= 0x08;
			}
			else
			{
				tmpIndex |= (index >> 1);
			}
		}
		PORTD = tmpIndex;
		count++;
		index <<= 1;
		if((index & 0x10) == 0x10)
		{
			index &= ~0x10;
			index |= 0x01;
		}
		_delay_ms(10);
    }
}

