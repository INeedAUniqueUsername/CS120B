/*
 * achen115_lab4_part1.c
 *
 * Created: 4/10/2019 2:09:40 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>

int main(void)
{
    /* Replace with your application code */
	DDRA = 0; PINA = -1;
	DDRB = -1; PORTB = 0;
	
	char held = 0;
	char state = 0;
    while (1) 
    {
		if(PINA & 1) {
			if(held) {
				
			} else {
				held = 1;
				state = !state;
			}
		} else {
			held = 0;
		}
		
		if(state) {
			PORTB = 2;
		} else {
			PORTB = 1;
		}
    }
}

