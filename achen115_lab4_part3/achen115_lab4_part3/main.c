/*
 * achen115_lab4_part3.c
 *
 * Created: 4/10/2019 2:32:11 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>


int main(void)
{
    /* Replace with your application code */
	DDRA = 0;	PINA = -1;
	DDRB = -1; PORTB = 0;
	DDRC = -1; PORTC = 0;
	char open = 0;
	char state = 0;
	char released = 0;
    while (1) 
    {
		char pressed = PINA & 7;
		switch(state) {
			case 0:
				if(pressed == 4) {
					state = 1;
				}
				break;
			case 1:
				if(pressed == 0) {
					state = 2;
				} else if(pressed != 4) {
					state = 0;
				}
				break;
			case 2:
				if(pressed == 2) {
					open = 1;
					state = 0;
				} else if(pressed != 0) {
					state = 0;	
				}
				
				break;
			
		}
		PORTB = open;
		PORTC = state;
    }
}

