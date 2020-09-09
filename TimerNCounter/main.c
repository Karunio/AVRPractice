/*
* TimerNCounter.c
*
* Created: 2020-06-15 오후 4:03:38
* Author : Karunio
*/

#include <avr/io.h>
#include <avr/interrupt.h>

SIGNAL(TIMER0_COMP_vect);

const char fndChars[] = { 0x3F, 0x06, 0x5B, 0x4F,
                      0x66, 0x6D, 0x7D, 0x27,
                      0x7F, 0x6F, 0x77, 0x7C,
                      0x39, 0x5E, 0x79, 0x71 };

volatile unsigned char charIndex = 0;
volatile unsigned char enable = 1;
unsigned int timerCnt = 0;

int main(void)
{
    DDRA = 0xFF;
    DDRE = 0x00;

    TCCR2 = (1 << CS20) | (1 << CS22);
    TIMSK |= 1 << OCIE2;
    TIFR |= 1 << OCF2;
    OCR2 = 72;

    EICRB = (1 << ISC40) | (1 << ISC41);
    EIFR = (1 << INTF4);
    EIMSK = (1 << INT4);
    sei();
    /* Replace with your application code */
    while (1)
    {
        PORTA = fndChars[charIndex];
    }
}

SIGNAL(TIMER2_COMP_vect)
{
    cli();
    OCR2 += 72;
    timerCnt++;
    
    if(timerCnt == 50)
    {
        if(enable == 1)
        {
            if(charIndex >= sizeof(fndChars) - 1)
            {
                charIndex = 0;
            }
            else
            {
                charIndex++;
            }
        }
        timerCnt = 0;
    }
    sei();
}

SIGNAL(INT4_vect)
{
    cli();
    enable = !enable;
    sei();
}