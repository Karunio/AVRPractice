#include <avr/io.h>
#include <util/delay.h>

char getch();

unsigned int melodies[] = {523, 587, 659, 698, 783, 880, 987, 1046};

int main(void){
	TCCR1A |= (1 << COM1C1) | (1 << WGM31);
	TCCR1B |= (1 << WGM33) | (1 << WGM32) | (1 << CS30);
	TCCR1C = 0x00;
	TCNT1 = 0;
	
	UCSR0A = 0x00;
	UCSR0B = (1 << RXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	
	UBRR0H = 0x00;
	UBRR0L = 0x03;
	
	while(1){
		char data = getch();
		if('1' <= data && data <= '8'){
			DDRB = (1 << DDRB7);
			ICR1 = F_CPU / melodies[data - '1'];
			OCR1C = ICR1 / 8;
		}
		else{
			DDRB = 0x00;
		}
	}
}

char getch(){
	char data;
	while((UCSR0A & (1 << RXC1)) == 0);
	data = UDR0;
	UCSR0A |= (1 << RXC1);
	return data;
}