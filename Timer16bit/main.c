/*
* Timer16bit.c
*
* Created: 2020-06-16 오전 11:21:07
* Author : Karunio
*/

#include <avr/io.h>
#include <avr/interrupt.h>

const char fndChars[] = {	0x3F, 0x06, 0x5B, 0x4F,
							0x66, 0x6D, 0x7D, 0x27,
							0x7F, 0x6F, 0x77, 0x7C,
							0x39, 0x5E, 0x79, 0x71 };


volatile unsigned char charIndex = 0;
volatile char fndFlag = 0, edgeFlag = 0;

int main(void)
{
	DDRA = 0xFF;
	DDRE = 0x00;
	
	TCCR3A = 0x00;
	TCCR3B = (1 << CS12) | (1 << CS10) | (1 << ICES3);
	
	ETIMSK |= (1 << TICIE3);
	ETIFR |= (1 << ICF3);
	sei();
	PORTA = fndChars[0];
	/* Replace with your application code */
	while (1)
	{
		if(fndFlag == 1)
		{
			if(charIndex >= sizeof(fndChars) - 1)
			{
				charIndex = sizeof(fndChars) - 1;
			}
			PORTA = fndChars[charIndex];
			fndFlag = 0;
		}
	}
	
	return 0;
}

SIGNAL(TIMER3_CAPT_vect)
{
	cli();
	
	unsigned int countCheck = 0;
	
	if(edgeFlag == 0)
	{
		TCCR3B &= ~(1 << ICES3);
		TCNT3 = 0;
		ICR3 = 0;
		edgeFlag = 1;
	}
	else
	{
		TCCR3B |= (1 << ICES3);
		countCheck = ICR3;
		charIndex = countCheck / 720;
		fndFlag = 1;
		edgeFlag = 0;
	}
	sei();
}