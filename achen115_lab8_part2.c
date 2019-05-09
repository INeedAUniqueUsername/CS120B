/*
 * achen115_lab8_part1.c
 *
 * Created: 4/29/2019 2:58:43 PM
 * Author : Alex
 */ 

#include <avr/io.h>
#include "io.c"
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
	LCD_init();

	char max = 0xF0;
    while (1) {
		short n = ADC;
		PORTB = n & 255;
		PORTD = n & (n >> 8) & 3;
		/*
		LCD_ClearScreen();
		
		char digits[5];
		char i = 4;
		while(n > 0) {
			digits[i] = n%10;
			n /= 10;
			i--;
		}
		i++;
		while(i < 5) {
			LCD_WriteData('0' + digits[i]);
			i++;
		}
		*/
    }
}

