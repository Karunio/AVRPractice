/*
* PWM.c
*
* Created: 2020-06-17 오전 10:46:46
* Author : Karunio
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile unsigned char light = 0;
volatile unsigned char lightFlag = 0;

unsigned int melodies[] = {523, 587, 659, 698, 783, 880, 987, 1046};

int main(void)
{
	unsigned char melody = 0;
	DDRB = (1 << DDRB7);
	
	TCCR1A |= (1 << COM1C1) | (1 << WGM31);
	TCCR1B |= (1 << WGM33) | (1 << WGM32) | (1 << CS30);
	TCCR1C = 0x00;
	TCNT1 = 0;
	
	sei();
	/* Replace with your application code */
	while (1)
	{
		ICR1 = F_CPU / 225;
		OCR1C = ICR1 * 0.9;
		_delay_ms(1000);
	}
}