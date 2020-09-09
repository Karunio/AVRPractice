/*
* Test.c
*
* Created: 2020-06-18 오전 10:14:59
* Author : Karunio
*	L : LED -> 좌우로 500ms 이동
C : CDS -> 값을 시리얼로 출력
0 ~ 7   -> buzzor음계
S       -> StepMortor (f 정방향, r 역방향)
push button -> 인터럽트 모든행동 종료
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdbool.h>

#define LED_PIN			DDRA
#define LED_OUT			PORTA
#ifdef	LED_PIN
	#define LED_PIN_MODE	0xFF
#endif
#define LED_RIGHT		0
#define LED_LEFT		1

#define BTN_PIN			DDRD
#define BTN_IN			PIND
#define BTN_IN_PINS		(1 << PIND0)
#ifdef	BTN_PIN
	#ifdef BTN_IN_PINS
		#define BTN_PIN_MODE	(BTN_PIN & ~BTN_IN_PINS)
	#endif
#endif

#define CDS_PIN			DDRF
#define CDS_OUT			PORTF
#define CDS_IN_PINS		(1 << PINF1)
#define CDS_OUT_PINS	(1 << PORTF0)
#ifdef	CDS_PIN
	#ifdef	CDS_OUT_PINS
		#ifdef	CDS_IN_PINS
			#define CDS_PIN_MODE	(CDS_OUT_PINS | (CDS_PIN & ~CDS_IN_PINS))
		#endif
	#endif
#endif

#define BUZZOR_PIN		DDRB
#define BUZZOR_OUT		PORTB
#define BUZZOR_OUT_PINS	(1 << PORTB7)
#ifdef	BUZZOR_PIN
	#ifdef BUZZOR_OUT_PINS
		#define BUZZOR_PIN_MODE (BUZZOR_OUT_PINS)
	#endif
#endif
#define BUZZOR_DUTY		8

#define MOTOR_PIN		DDRC
#define MOTOR_OUT		PORTC
#define MOTOR_OUT_PINS	((1 << PORTC0) | (1 << PORTC1) | (1 << PORTC2) | (1 << PORTC3))
#ifdef	MOTOR_PIN
	#ifdef MOTOR_OUT_PINS
		#define MOTOR_PIN_MODE	MOTOR_OUT_PINS
	#endif
#endif

#define UART_PIN	DDRE
#define UART_RX_PIN	(1 << PORTE0)
#define UART_TX_PIN	(1 << PINE1)
#ifdef  UART_PIN
	#ifdef  UART_RX_PIN
		#ifdef	UART_TX_PIN
			#define UART_PIN_MODE	(UART_TX_PIN | (UART_PIN & ~(UART_RX_PIN)))
		#endif
	#endif
#endif

#define TIMER	0
#if TIMER == 0
	#define TCCR	TCCR0
	#define	PRESCALER		 (1 << CS00) | (1 << CS01) | (1 << CS02)
	#define PRESCALE_VALUE	 1024
	#define PER_INTER_MILLIS 10
	#define OUTPUT_COMPARE
	#ifdef	OUTPUT_COMPARE
		#define OCR		   OCR0
		#define TIMER_MASK (1 << OCIE0)
		#define TIMER_MODE (1 << WGM01)
		#define TIMER_FLAG (1 << OCF0)
		#define OCR_TARGET (F_CPU / PRESCALE_VALUE / 100)
		#define TIMER_INTER_VECT	TIMER0_COMP_vect
	#endif
#endif

#define BFR_MAX		16

#define INPUT_KEY_LED_UPPER		'L'
#define INPUT_KEY_LED_LOWER		'l'
#define INPUT_KEY_CDS_UPPER		'C'
#define INPUT_KEY_CDS_LOWER		'c'
#define INPUT_KEY_MEL_C_LOW		'0'
#define INPUT_KEY_MEL_D			'1'
#define INPUT_KEY_MEL_E			'2'
#define INPUT_KEY_MEL_F			'3'
#define INPUT_KEY_MEL_G			'4'
#define INPUT_KEY_MEL_A			'5'
#define INPUT_KEY_MEL_B			'6'
#define INPUT_KEY_MEL_C_HIGH	'7'
#define INPUT_KEY_MOTOR_UPPER	'S'
#define INPUT_KEY_MOTOR_LOWER	's'

SIGNAL(INT0_vect);
void LedMove();
void CdsTrans();
int GetCdsValue();
void RunBuzzor(char data);
void PlayBuzzorByMelody(const char melody);
void RunMortor();
void RunMortorByDir(const char dir);
void PutChar(const char data);
void PutString(const char *str);
char GetChar();
void FlushReceiveData();
void EnableClock();
void delay_10ms(char ten_millis);
void DisableClock();

const unsigned int melodies[] = {523, 587, 659, 698, 783, 880, 987, 1046};
const char menu[] = {"++++++ Menu ++++++\r\n"\
					 "L : LED\r\n"\
					 "C : CDS\r\n"\
					 "0 ~ 7 : Piano (Low C to High C)\r\n"\
					 "S : Step Motor (R,r : ClockWise F,f : CountClockWiseClockWise)\r\n"\
					 "Push Button : Stop\r\n"};
					 
volatile bool activate = false;
volatile bool isSendMenu = false;
volatile char timerCnt = 0;

int main(void)
{
	// 각 핀 입출력 설정
	LED_PIN = LED_PIN_MODE;
	BTN_PIN = BTN_PIN_MODE;
	CDS_PIN = CDS_PIN_MODE;
	MOTOR_PIN = MOTOR_PIN_MODE;
	UART_PIN = UART_PIN_MODE;
	
	// 인터럽트 설정
	EIMSK = (1 << INT0);
	EICRA = (1 << ISC00) | (1 << ISC01);
	EIFR |= (1 << INTF0);
	
	// A/D 컨버터 설정
	ADMUX = (1 << MUX0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);
	
	// UART 통신 설정
	UCSR0A = 0x00;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
	UBRR0H = 0x00;
	UBRR0L = 0x03;
	
	// 부저 PWM 설정
	TCCR1A |= (1 << COM1C1) | (1 << WGM31);
	TCCR1B |= (1 << WGM33) | (1 << WGM32) | (1 << CS30);
	TCCR1C = 0x00;
	TCNT1 = 0;
	
	sei();
	EnableClock();
	while(1)
	{
		if(isSendMenu == false)
		{
			PutString(menu);
			isSendMenu = true;
		}
		
		char data = GetChar();
		switch(data)
		{
		case INPUT_KEY_LED_LOWER:
		case INPUT_KEY_LED_UPPER:
			LedMove();
		break;
		
		case INPUT_KEY_CDS_LOWER:
		case INPUT_KEY_CDS_UPPER:
			CdsTrans();
		break;
		
		case INPUT_KEY_MOTOR_LOWER:
		case INPUT_KEY_MOTOR_UPPER:
			RunMortor();
		break;
		
		case INPUT_KEY_MEL_C_LOW:
		case INPUT_KEY_MEL_D:
		case INPUT_KEY_MEL_E:
		case INPUT_KEY_MEL_F:
		case INPUT_KEY_MEL_G:
		case INPUT_KEY_MEL_A:
		case INPUT_KEY_MEL_B:
		case INPUT_KEY_MEL_C_HIGH:
			RunBuzzor(data);
		break;
		}
	}

}

SIGNAL(INT0_vect)
{
	cli();
	activate = false;
	isSendMenu = false;
	FlushReceiveData();
	sei();
}

SIGNAL(TIMER_INTER_VECT)
{
	cli();
	timerCnt++;
	sei();
}

void LedMove()
{
	activate = true;
	EnableClock();
	char led = 0x01;
	char ledDir = LED_LEFT;
	while(activate)
	{
		LED_OUT = led;
		if(ledDir == LED_LEFT)
		{
			led <<= 1;
		}
		else if(ledDir == LED_RIGHT)
		{
			led >>= 1;
		}
		
		if(led == 0x80)
		{
			ledDir = LED_RIGHT;
		}
		else if(led == 0x01)
		{
			ledDir = LED_LEFT;
		}
		delay_10ms(50);
	}
	DisableClock();
	LED_OUT = 0x00;
}

void CdsTrans()
{
	activate = true;
	char buffer[BFR_MAX];
	CDS_OUT = CDS_OUT_PINS;
	while(activate)
	{
		int cdsValue = GetCdsValue();
		sprintf(buffer, "CDS : %d\r\n", cdsValue);
		PutString(buffer);
		_delay_ms(100);
	}
	CDS_OUT = 0x00;
}

int GetCdsValue()
{
	int cdsValue = 0;
	
	ADCSRA |= (1 << ADSC);
	while((ADCSRA & (1 << ADIF)) == 0x00);
	cdsValue = ADC;
	
	return cdsValue;
}

void RunBuzzor(char data)
{
	activate = true;
	while(activate)
	{
		PlayBuzzorByMelody(data);
		data = GetChar();
	}
	BUZZOR_PIN = 0x00;
}

void PlayBuzzorByMelody(char melody)
{
	if(INPUT_KEY_MEL_C_LOW <= melody && melody <= INPUT_KEY_MEL_C_HIGH)
	{
		BUZZOR_PIN = BUZZOR_PIN_MODE;
		ICR1 = F_CPU / melodies[melody - INPUT_KEY_MEL_C_LOW];
		OCR1C = ICR1 / BUZZOR_DUTY;
	}
}

void RunMortor()
{
	activate = true;
	while(activate)
	{
		char dir = GetChar();
		RunMortorByDir(dir);
	}
	MOTOR_OUT = 0x00;
}

void RunMortorByDir(char dir)
{
	EnableClock();
	char index;
	if(dir == 'f')
	{
		index = 0x01;
		while(activate && index != 0x10)
		{
			MOTOR_OUT = index;
			index <<= 1;
			delay_10ms(1);
		}
	}
	else if(dir == 'r')
	{
		index = 0x80;
		while(activate && index != 0x00)
		{
			MOTOR_OUT = index;
			index >>= 1;
			delay_10ms(1);
		}
	}
	DisableClock();
}

void PutChar(const char data)
{
	while((UCSR0A & (1 << UDRE0)) == 0x00)
	{
	}
	UDR0 = data;
	UCSR0A |= (1 << UDRE0);
}

void PutString(const char *str)
{
	char index = 0;
	while(str[index] != '\0')
	{
		PutChar(str[index++]);
	}
}

char GetChar()
{
	char data = NULL;
	while((UCSR0A & (1 << RXC0)) == (1 << RXC0))
	{
		data = UDR0;
		return data;
	}
	return data;
}

void FlushReceiveData()
{
	char flush;
	while((UCSR0A & (1 << RXC0)) == (1 << RXC0))
	{
		flush = UDR0;
	}
	UCSR0A |= (1 << RXC0);
}

void EnableClock()
{
	TCCR = TIMER_MODE | PRESCALER;
	TIMSK = TIMER_MASK;
	OCR = OCR_TARGET;
	TIFR |= TIMER_FLAG;
}

void delay_10ms(char ten_millis)
{
	timerCnt = 0;
	while(activate && timerCnt < ten_millis);
	timerCnt = 0;
}

void DisableClock()
{
	TIMSK &= ~(TIMER_MASK);
}