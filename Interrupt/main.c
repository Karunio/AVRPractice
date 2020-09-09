/*
* Interrupt.c
*
* Created: 2020-06-15 오후 1:54:58
* Author : Karunio
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile unsigned char timeStop = 0;
volatile unsigned char fnIndex = 0;

char fnds[] = { 0x3F, 0x06, 0x5B, 0x4F,
	0x66, 0x6D, 0x7D, 0x27,
	0x7F, 0x6F, 0x77, 0x7C,
	0x39, 0x5E, 0x79, 0x71};

int main(void)
{
	DDRA = 0xFF;
	DDRD = 0x00;
	
	EICRA = (1 << ISC00) | (1 << ISC01) | (1 << ISC30) | (1 << ISC31);
	EIMSK = (1 << INT0) | (1 << INT3);
	EIFR = (1 << INTF0) | (1 << INTF3);
	sei();
	/* Replace with your application code */
	while (1)
	{
		PORTA = fnds[fnIndex];
		if(timeStop == 0)
		{
			fnIndex++;
			if(fnIndex >= sizeof(fnds))
			{
				fnIndex = 0;
			}
		}
		_delay_ms(500);
	}
}

SIGNAL(INT0_vect)
{
	cli();
	fnIndex = 0;
	sei();
}

SIGNAL(INT3_vect)
{
	cli();
	timeStop = !timeStop;
	sei();
}