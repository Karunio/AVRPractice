/*
* LcdTest.c
*
* Created: 2020-06-16 오후 1:50:55
* Author : Karunio
*/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "Lcd.h"
#define STR_MAX	16

char getNum(char result);

int main(void)
{
	Byte input[] = "Input Button";
	Byte result[STR_MAX];
	DDRA = 0xFF;
	DDRD = 0x00;

	char buttonNum = 0;
	LcdInit_4bit();
	/* Replace with your application code */
	while (1)
	{
		buttonNum = PIND;

		if(buttonNum == 0x00)
		{
			Lcd_Pos(0, 0);
			Lcd_STR(input);
		}
		else
		{
			Lcd_Clear();
			int num = getNum(buttonNum);
			for(int i = 0; i < 9; i++)
			{
				sprintf(result, "%d * %d = %d", num, i + 1, num * (i + 1));
				Lcd_Pos(0, 0);
				Lcd_STR(result);
				_delay_ms(500);
			}
			Lcd_Clear();
		}
	}
}

char getNum(char result)
{
	switch(result)
	{
		case 0x01:
		return 2;
		case 0x02:
		return 3;
		case 0x04:
		return 4;
		case 0x08:
		return 5;
		case 0x10:
		return 6;
		case 0x20:
		return 7;
		case 0x40:
		return 8;
		case 0x80:
		return 9;
	}
	return 0;
}