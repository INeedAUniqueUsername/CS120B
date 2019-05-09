/*
 * achen115_lab8_part1.c
 *
 * Created: 4/29/2019 2:58:43 PM
 * Author : Alex
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}

int main(void)
{
	ADC_init();
	DDRA = 0; PINA = -1;
	DDRB = -1; PORTB = 0;
	DDRD = 3; PORTD = 0;
    /* Replace with your application code */

	char max = 0xF0;
    while (1) {
		short n = ADC;
		PORTB = n >= max/2;
    }
}

