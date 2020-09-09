/*
 * ADConvert.c
 *
 * Created: 2020-06-16 오후 4:56:33
 * Author : Karunio
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "Lcd.h"

#define MAX_STR	16

int main(void)
{
	unsigned int adData = 0;
	Byte str[MAX_STR];
	DDRA = 0xFF;
	DDRF = (1 << DDRF1);
	ADMUX = 0x00;
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);

	LcdInit_4bit();
    /* Replace with your application code */
    while (1) 
    {
		Lcd_Clear();
		Lcd_STR("Sampling..");
		PORTF = (1 << PORTF1);
		ADCSRA |= (1 << ADSC);

		while((ADCSRA & (1 << ADIF)) == 0x00);
		adData = ADC;
		
		sprintf(str, "Data : %d", adData);
		Lcd_Clear();
		Lcd_Pos(0, 0);
		Lcd_STR(str);
		_delay_ms(300);
    }
}

