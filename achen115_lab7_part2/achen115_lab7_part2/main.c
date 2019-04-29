#include <avr/io.h>
#include "io.c"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: pre-scaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s

	// AVR output compare register OCR1A.
	OCR1A = 125;    // Timer interrupt will be generated when TCNT1==OCR1A
	// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	// AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	//Initialize avr counter
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds

	//Enable global interrupts
	SREG |= 0x80; // 0x80: 1000000
}

void TimerOff() {
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR() {
	TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void main()
{
	DDRA = 0; PINA = -1;	//PINA is input
	DDRB = -1; PORTB = 0;

	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	
	// Initializes the LCD display
	LCD_init();

	char i = 0;
	char score = 5;
	char out = 0;
	TimerSet(300);
	TimerOn();
	char pressed = 0;
	while(1) {
		if(i < 2) {
			i++;
		} else {
			i = 0;
		}

		if(pressed) {
			if(PINA & 1) {
				pressed = 0;
			}
		}

		PORTB = 1 << i;
		while(!TimerFlag) {
			if(!pressed) {
				if(~PINA & 1) {
					if(i == 1) {
						score++;
						} else {
						if(score > 0) {
							score--;
						}
					}
					pressed = 1;

					if(out != score) {
						out = score;
						LCD_ClearScreen();
						if(out == 10) {
							LCD_DisplayString(1, "You won!");
							TimerOff();
							} else {
							LCD_WriteData('0' + out);
						}
						
					}
				}
			}
			
		}
		TimerFlag = 0;

		if(out != score) {
			out = score;
			LCD_ClearScreen();
			if(out == 10) {
				LCD_DisplayString(1, "You won!");
				TimerOff();
			} else {
				LCD_WriteData('0' + out);
			}
			
		}

		/*

		if(pressed != 0 && pressed != 3) {
			
			TimerOn();
			PORTB = 1;
			while(!TimerFlag) {
				PORTB = 2;
				//Presses changed before one second
				pressed = (~PINA & 3);
				if(pressed == 0 || pressed == 3) {
					goto Debounce;
				}
			}
			PORTB = 3;
			TimerFlag = 0;
			TimerOff();
			TimerSet(1000);
			continue;

			Debounce:
			TimerOff();
			TimerSet(50);
			TimerOn();
			while(!TimerFlag);
			TimerFlag = 0;
			TimerOff();
			
		}
		*/
	}

}