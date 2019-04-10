/* Alex Chen (achen115@ucr.edu)
 * Nicholas Ray (nray001@ucr.edu)
 * achen115_lab4_part5.c
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
	char state = 0;
	char open = 0;
	char states[] = {4, 0, 1, 0, 2, 0, 1};
    while (1) 
    {
		char pressed = PINA & 7;
		if(pressed == 0) {
			//Update on release if needed
			if(states[state] == pressed) {
				state++;
			}
		} else {
			if(states[state] == pressed) {
				state++;
				//Open and reset
				if(state == 7) {
					open =! open;
					state = 0;
				}
			} else if(states[state-1] != pressed) {
				//Wait for previous pressed to release if needed
				state = 0;
			}
		}
		
		PORTB = open;
		PORTC = state;
    }
}

